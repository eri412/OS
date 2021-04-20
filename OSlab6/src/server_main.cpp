#include <signal.h>

#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>

#include "server.h"


Server* server_ptr = nullptr;
void terminate(int ret_code) {
    if (server_ptr != nullptr) {
        server_ptr->~Server();
    }
    exit(ret_code);
}

void process_cmd(Server& server, std::string cmd) {
    if (cmd == "create") {
        int id, parrent_id;
        std::cin >> id >> parrent_id;
        server.create_child_cmd(id, parrent_id);
    } 
    else if (cmd == "remove") {
        int id;
        std::cin >> id;
        server.remove_child_cmd(id);
    } 
    else if (cmd == "exec") {
        int id;
        std::string sub_cmd;
        std::cin >> id >> sub_cmd;
        message_type type;
        if (sub_cmd == "time") {
            type = message_type::TIMER_TIME;
        } 
        else if (sub_cmd == "start") {
            type = message_type::TIMER_START;
        } 
        else if (sub_cmd == "stop") {
            type = message_type::TIMER_STOP;
        } 
        else {
            std::cout << "incorrect arguments" << std::endl;
            return;
        }
        server.exec_cmd(id, type);
    } 
    else if (cmd == "pingall") {
        server.pingall_cmd();
    } 
    else {
        std::cout << "no such command" << std::endl;
    }
}

int main() {
    try {
        signal(SIGINT, terminate);
        signal(SIGSEGV, terminate);
        Server server;
        server_ptr = &server;
        std::string cmd;
        while (std::cin >> cmd) {
            process_cmd(server, cmd);
        }
    } 
    catch (std::exception& ex) {
        std::cerr << std::to_string(getpid()) << ' ' << ex.what() << std::endl;
        exit(1);
    }
    return 0;
}
