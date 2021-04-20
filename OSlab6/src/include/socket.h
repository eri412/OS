#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string>

#include "zmq_wrapper.h"

enum class connection_type {
    BIND,
    CONNECT
};

class Socket {
public:
    Socket(void* context, socket_type socket_type, std::string ip);
    ~Socket();

    void send(Message message);
    Message receive();

    void subscribe(std::string ip);
    std::string ip() const;

private:
    void* socket_;
    socket_type socket_type_;
    std::string ip_;
};

#endif