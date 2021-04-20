#include "zmq_wrapper.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

#include <iostream>
#include <tuple>


void* create_zmq_context() {
    void* context = zmq_ctx_new();
    if (context == NULL) {
        throw std::runtime_error(zmq_strerror(errno));
    }
    return context;
}

void destroy_zmq_context(void* context) {
  if (zmq_ctx_destroy(context) != 0) {
    throw std::runtime_error(zmq_strerror(errno));
  }
}

int get_zmq_socket_type(socket_type type) {
    switch (type) {
        case socket_type::PUB:
            return ZMQ_PUB;
        case socket_type::SUB:
            return ZMQ_SUB;
        default:
            throw std::logic_error("Undefined socket type");
    }
}

void* create_zmq_socket(void* context, socket_type type) {
    int zmq_type = get_zmq_socket_type(type);
    void* socket = zmq_socket(context, zmq_type);
    if (socket == NULL) {
        throw std::runtime_error(zmq_strerror(errno));
    }
    if (zmq_type == ZMQ_SUB) {
        if (zmq_setsockopt(socket, ZMQ_SUBSCRIBE, 0, 0) == -1) {
            throw std::runtime_error(zmq_strerror(errno));
        }
        int linger_period = 0;
        if (zmq_setsockopt(socket, ZMQ_LINGER, &linger_period, sizeof(int)) == -1) {
            throw std::runtime_error(zmq_strerror(errno));
        }
    }
    return socket;
}

void close_zmq_socket(void* socket) {
    sleep(1);
    if (zmq_close(socket) != 0) {
        throw std::runtime_error(zmq_strerror(errno));
    }
}

std::string create_ip(ip_type type, pid_t id) {
    switch (type) {
        case ip_type::PARRENT_PUB:
            return "ipc:///tmp/parrent_pub_" + std::to_string(id);
        case ip_type::CHILD_PUB:
            return "ipc:///tmp/child_pub_" + std::to_string(id);
        default:
            throw std::logic_error("undefined ip type");
    }
}

void bind_zmq_socket(void* socket, std::string ip) {
    if (zmq_bind(socket, ip.data()) != 0) {
        throw std::runtime_error(zmq_strerror(errno));
    }
}

void unbind_zmq_socket(void* socket, std::string ip) {
    if (zmq_unbind(socket, ip.data()) != 0) {
        throw std::runtime_error(zmq_strerror(errno));
    }
}

void connect_zmq_socket(void* socket, std::string ip) {
    if (zmq_connect(socket, ip.data()) != 0) {
        throw std::runtime_error(zmq_strerror(errno));
    }
}

void disconnect_zmq_socket(void* socket, std::string ip) {
    if (zmq_disconnect(socket, ip.data()) != 0) {
        throw std::runtime_error(zmq_strerror(errno));
    }
}

int counter = 0;
Message::Message() {
    uniq_num = counter++;
}

Message::Message(message_type command_a, int to_id_a, int value_a)
    : Message() {
    command = command_a;
    to_id = to_id_a;
    value = value_a;
}

bool operator==(const Message& lhs, const Message& rhs) {
    return std::tie(lhs.command, lhs.to_id, lhs.value, lhs.uniq_num) == std::tie(rhs.command, rhs.to_id, rhs.value, rhs.uniq_num);
}

void create_zmq_msg(zmq_msg_t* zmq_msg, Message msg) {
    zmq_msg_init_size(zmq_msg, sizeof(Message));
    memcpy(zmq_msg_data(zmq_msg), &msg, sizeof(Message));
}

void send_zmq_msg(void* socket, Message msg) {
    zmq_msg_t zmq_msg;
    create_zmq_msg(&zmq_msg, msg);
    if (!zmq_msg_send(&zmq_msg, socket, 0)) {
        throw std::runtime_error(zmq_strerror(errno));
    }
    zmq_msg_close(&zmq_msg);
}

Message get_zmq_msg(void* socket) {
    zmq_msg_t zmq_msg;
    zmq_msg_init(&zmq_msg);
    if (zmq_msg_recv(&zmq_msg, socket, 0) == -1) {
        return Message{message_type::ERROR, 0, 0};
    }
    Message msg;
    memcpy(&msg, zmq_msg_data(&zmq_msg), sizeof(Message));
    zmq_msg_close(&zmq_msg);
    return msg;
}