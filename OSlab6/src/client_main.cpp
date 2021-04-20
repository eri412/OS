#include <signal.h>

#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>

#include "client.h"


const int ERR_TERMINATED = 1;
const int MESSAGE_ALL = -256;

Client* client_ptr = nullptr;

void terminate(int) {
    if (client_ptr != nullptr) {
        client_ptr->~Client();
    }
    exit(0);
}

void process_msg(Client& client, const Message msg) {
    switch (msg.command) {
        case message_type::ERROR:
            throw std::runtime_error("error message received");
        case message_type::CHECK:
            client.send_up(msg);
            break;
        case message_type::CREATE_CHILD:
            client.add_child(msg.value);
            break;
        case message_type::REMOVE_CHILD: {
            if (msg.to_id != MESSAGE_ALL) {
                client.send_up(msg);
            }
            Message tmp = msg;
            tmp.to_id = MESSAGE_ALL;
            client.send_down(tmp);
            terminate(0);
            break;
        }
        case message_type::TIMER_START:
            client.start_timer();
            client.send_up(msg);
            break;
        case message_type::TIMER_STOP:
            client.stop_timer();
            client.send_up(msg);
            break;
        case message_type::TIMER_TIME: {
            int val = client.get_time();
            client.send_up(Message(message_type::TIMER_TIME, client.id(), val));
            break;
        }
        case message_type::PINGALL:
            client.send_down(msg);
            client.pingall();
            break;
        default:
            throw std::logic_error("no such message command");
    }
}

int main(int argc, char const* argv[]) {
    try {
        signal(SIGINT, terminate);
        signal(SIGSEGV, terminate);
        Client client(std::stoi(argv[1]), std::string(argv[2]));
        client_ptr = &client;
        while (true) {
            Message msg = client.receive();
            if (msg.to_id != client.id() && msg.to_id != MESSAGE_ALL) {
                if (msg.go_up) {
                    client.send_up(msg);
                } 
                else {
                    client.send_down(msg);
                }
                continue;
            }
            process_msg(client, msg);
        }
    } 
    catch (std::exception& ex) {
        std::cerr << std::to_string(getpid()) << ' ' << ex.what() << std::endl;
        exit(ERR_TERMINATED);
    }
    return 0;
}