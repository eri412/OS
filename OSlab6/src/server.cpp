#include "server.h"

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#include <iostream>

#include "zmq_wrapper.h"


const int ERR_LOOP = 2;
const std::string CLIENT_PROG = "./client";
const double MESSAGE_WAITING_TIME = 1;
const int MESSAGE_ALL = -256;

void* second_thread(void* serv_arg) {
    Server* server_ptr = (Server*)serv_arg;
    pid_t server_pid = server_ptr->pid();
    try {
        pid_t child_pid = fork();
        if (child_pid == -1) {
            throw std::runtime_error(std::strerror(errno));
        }
        if (child_pid == 0) {
            execl(CLIENT_PROG.data(), CLIENT_PROG.data(), "0", server_ptr->pub_->ip().data(), NULL);
        }

        std::string ip = create_ip(ip_type::PARRENT_PUB, child_pid);
        server_ptr->sub_ = std::make_unique<Socket>(server_ptr->context_, socket_type::SUB, ip);
        server_ptr->tree_.add_to(0, {0, child_pid});

        while (true) {
            Message msg = server_ptr->sub_->receive();
            if (msg.command == message_type::ERROR) {
                if (server_ptr->terminated_) {
                    return NULL;
                } 
                else {
                    throw std::runtime_error("error message");
                }
            }
            server_ptr->last_message_ = msg;

            switch (msg.command) {
                case message_type::CREATE_CHILD: {
                    auto& pa = server_ptr->tree_.get(msg.to_id);
                    pa.second = msg.value;
                    std::cout << "OK: " << server_ptr->last_message_.value << std::endl;
                    break;
                }
                case message_type::REMOVE_CHILD:
                    server_ptr->tree_.remove(msg.to_id);
                    std::cout << "OK" << std::endl;
                    break;
                case message_type::TIMER_START:
                    std::cout << "OK:" << msg.to_id << std::endl;
                    break;
                case message_type::TIMER_STOP:
                    std::cout << "OK:" << msg.to_id << std::endl;
                    break;
                case message_type::TIMER_TIME: {
                    std::cout << "OK:" << msg.to_id << ": " << msg.value << std::endl;
                    break;
                }
              case message_type::PINGALL:
                  server_ptr->map_for_check_[msg.to_id] = true;
                  break;
              default:
                  break;
            }
        }
    } 
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        exit(ERR_LOOP);
    }
    return NULL;
}

Server::Server() {
    pid_ = getpid();
    context_ = create_zmq_context();
    std::string ip = create_ip(ip_type::CHILD_PUB, getpid());
    pub_ = std::make_unique<Socket>(context_, socket_type::PUB, ip);
    if (pthread_create(&receive_msg_loop_id, 0, second_thread, this) != 0) {
        throw std::runtime_error(std::strerror(errno));
    }
}

Server::~Server() {
    if (terminated_) {
        std::cerr << std::to_string(pid_) + " server double termination" << std::endl;
        return;
    }
    terminated_ = true;
    for (pid_t pid : tree_.get_all_second()) {
        kill(pid, SIGINT);
    }

    try {
        pub_ = nullptr;
        sub_ = nullptr;
        destroy_zmq_context(context_);
    } 
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void Server::send(Message message) {
    message.go_up = false;
    pub_->send(message);
}

Message Server::receive() {
    return sub_->receive();
}

pid_t Server::pid() const {
    return pid_;
}

Message Server::last_message() const {
    return last_message_;
}

bool Server::check(int id) {
    Message msg(message_type::CHECK, id, 0);
    send(msg);
    sleep(MESSAGE_WAITING_TIME);
    if (last_message_ == msg) {
        return true;
    } else {
        return false;
    }
}

void Server::create_child_cmd(int id, int parrent_id) {
    if (tree_.find(id)) {
        std::cout << "Error: Already exists" << std::endl;
        return;
    }
    if (!tree_.find(parrent_id)) {
        std::cout << "Error: Parent not found" << std::endl;
        return;
    }
    if (!check(parrent_id)) {
        std::cout << "Error: Parent is unavailable" << std::endl;
        return;
    }
    send(Message(message_type::CREATE_CHILD, parrent_id, id));
    tree_.add_to(parrent_id, {id, 0});
}

void Server::remove_child_cmd(int id) {
    if (id == 0) {
        std::cout << "Error: Can't remove server node" << std::endl;
        return;
    }
    if (!tree_.find(id)) {
        std::cout << "Error: Not found" << std::endl;
        return;
    }
    if (!check(id)) {
        std::cout << "Error: Node is unavailable" << std::endl;
        return;
    }
    send(Message(message_type::REMOVE_CHILD, id, 0));
}

void Server::exec_cmd(int id, message_type type) {
    if (!tree_.find(id)) {
        std::cout << "Error: Not found" << std::endl;
        return;
    }
    if (!check(id)) {
        std::cout << "Error: Node is unavailable" << std::endl;
        return;
    }
    send(Message(type, id, 0));
}

void Server::pingall_cmd() {
    send(Message(message_type::PINGALL, MESSAGE_ALL, 0));
    auto uset = tree_.get_all_first();
    for (int id : uset) {
        map_for_check_[id] = false;
    }
    sleep(MESSAGE_WAITING_TIME);
    std::cout << "Ok: ";
    for (auto& [id, bit] : map_for_check_) {
        if (!bit) {
            std::cout << id << ";";
        }
        bit = false;
    }
    std::cout << std::endl;
}