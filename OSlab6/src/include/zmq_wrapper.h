#ifndef _ZMQ_WRAPPER_H_
#define _ZMQ_WRAPPER_H_

#include <string>

enum class message_type {
    ERROR,
    CHECK,
    CREATE_CHILD,
    REMOVE_CHILD,
    TIMER_TIME,
    TIMER_START,
    TIMER_STOP,
    PINGALL
};

enum class ip_type {
    CHILD_PUB,
    PARRENT_PUB,
};

enum class socket_type {
    PUB,
    SUB
};

struct Message {
    message_type command = message_type::ERROR;
    int to_id;
    int value;
    bool go_up = false;
    int uniq_num;

    Message();
    Message(message_type command_a, int to_id_a, int value_a);
};

void* create_zmq_context();
void destroy_zmq_context(void* context);


void* create_zmq_socket(void* context, socket_type type);
void close_zmq_socket(void* socket);


std::string create_ip(ip_type type, pid_t id);

void bind_zmq_socket(void* socket, std::string ip);
void unbind_zmq_socket(void* socket, std::string ip);
void connect_zmq_socket(void* socket, std::string ip);
void disconnect_zmq_socket(void* socket, std::string ip);

bool operator==(const Message& lhs, const Message& rhs);

void send_zmq_msg(void* socket, Message msg);
Message get_zmq_msg(void* socket);

#endif
