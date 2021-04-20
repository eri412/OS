#include "socket.h"

#include <iostream>
#include <stdexcept>


Socket::Socket(void* context, socket_type socket_type, std::string ip)
    : socket_type_(socket_type), ip_(ip) {
    socket_ = create_zmq_socket(context, socket_type_);
    switch (socket_type_) {
        case socket_type::PUB:
            bind_zmq_socket(socket_, ip);
            break;
        case socket_type::SUB:
            connect_zmq_socket(socket_, ip);
            break;
        default:
            throw std::logic_error("Undefined connection type");
    }
}

Socket::~Socket() {
    try {
        close_zmq_socket(socket_);
    } 
    catch (std::exception& ex) {
        std::cerr << "Socket wasn't closed: " << ex.what() << std::endl;
    }
}

void Socket::send(Message message) {
    if (socket_type_ == socket_type::PUB) {
        send_zmq_msg(socket_, message);
    } 
    else {
        throw std::logic_error("SUB socket can't send messages");
    }
}

Message Socket::receive() {
    if (socket_type_ == socket_type::SUB) {
        return get_zmq_msg(socket_);
    } 
    else {
        throw std::logic_error("PUB socket can't receive messages");
    }
}

void Socket::subscribe(std::string ip) {
    if (socket_type_ == socket_type::SUB) {
        connect_zmq_socket(socket_, ip);
    } 
    else {
        throw std::logic_error("Subscribe is only for SUB sockets");
    }
}

std::string Socket::ip() const {
    return ip_;
}