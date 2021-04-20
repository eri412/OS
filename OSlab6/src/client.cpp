#include "client.h"

#include <unistd.h>

#include <iostream>

#include "zmq_wrapper.h"

//using namespace std;

const std::string CLIENT_PROG = "./client";
const double MESSAGE_WAITING_TIME = 1;

Client::Client(int id, std::string parrent_ip) {
    id_ = id;
    pid_ = getpid();
    context_ = create_zmq_context();

    std::string ip = create_ip(ip_type::CHILD_PUB, getpid());
    child_pub_ = std::make_unique<Socket>(context_, socket_type::PUB, ip);
    ip = create_ip(ip_type::PARRENT_PUB, getpid());
    parrent_pub_ = std::make_unique<Socket>(context_, socket_type::PUB, ip);

    sub_ = std::make_unique<Socket>(context_, socket_type::SUB, parrent_ip);

    sleep(MESSAGE_WAITING_TIME);
    send_up(Message(message_type::CREATE_CHILD, id, getpid()));
}

Client::~Client() {
    if (terminated_) {
        std::cerr << std::to_string(pid_) << " Client double termination" << std::endl;
        return;
    }

    terminated_ = true;

    sleep(MESSAGE_WAITING_TIME);
    try {
        child_pub_ = nullptr;
        parrent_pub_ = nullptr;
        sub_ = nullptr;
        destroy_zmq_context(context_);
    } 
    catch (std::exception& ex) {
        std::cerr << std::to_string(pid_) << ex.what() << std::endl;
    }
}

void Client::send_up(Message message) {
    message.go_up = true;
    parrent_pub_->send(message);
}

void Client::send_down(Message message) {
    message.go_up = false;
    child_pub_->send(message);
}

Message Client::receive() {
    Message msg = sub_->receive();
    return msg;
}

void Client::add_child(int id) {
    pid_t child_pid = fork();
    if (child_pid == -1) {
        throw std::runtime_error("Can't fork");
    }
    if (child_pid == 0) {
        execl(CLIENT_PROG.data(), CLIENT_PROG.data(), std::to_string(id).data(), child_pub_->ip().data(), NULL);
    }

    std::string ip = create_ip(ip_type::PARRENT_PUB, child_pid);
    sub_->subscribe(ip);
}

void Client::start_timer() {
    is_timer_started = true;
    start_ = std::chrono::steady_clock::now();
}

void Client::stop_timer() {
    is_timer_started = false;
    finish_ = std::chrono::steady_clock::now();
}

int Client::get_time() {
    if (is_timer_started) {
      finish_ = std::chrono::steady_clock::now();
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(finish_ - start_).count();
}

void Client::pingall() {
    send_up(Message(message_type::PINGALL, id_, 0));
}

int Client::id() const {
    return id_;
}

pid_t Client::pid() const {
    return pid_;
}