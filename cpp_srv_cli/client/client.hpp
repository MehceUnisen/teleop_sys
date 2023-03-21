#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <vector>

struct Socket {
    int port_;
    int sock_fd_;
};

struct Server {
    const char* host_addr_;
    sockaddr_in server_addr_;
    hostent *server_;
    socklen_t* server_addr_size;
    Socket cli_socket;
};

class Client {

    virtual bool sendData(Server& server) {
        server.cli_socket.port_ += 0;
        return false;
    }
    virtual bool receiveData(Server& server) {
        server.cli_socket.port_ += 0;
        return false;
    }
    virtual bool acceptData(Server& server) {
        server.cli_socket.port_ += 0;
        return false;
    }

public:
    virtual bool createSocket(Server* server, char* host_addr, int port) {
        server->host_addr_ = host_addr;
        server->cli_socket.port_ = port;
        return false;
    }    
};

class TcpClient : public Client {
    std::vector<std::thread> m_threadPool;
    bool createSocket(Server* server, char* host_addr, int port);
    bool establishConnection(Server& server);
    bool sendData(Server& server);
    bool receiveData(Server& server);
    virtual bool acceptData(Server& server) {
        std::thread sendWorker(&TcpClient::sendData, this, std::ref(server));
        std::thread receiveWorker(&TcpClient::receiveData, this, std::ref(server));
       receiveWorker.join();
       sendWorker.join();
        std::cin.get();
        return false;
    }
    char buf[4096] {"test"};
};

class UdpClient : public Client {

    bool createSocket(Server* server, char* host_addr, int port);
    bool sendData(Server& server);
    bool receiveData(Server& server);
    virtual bool acceptData(Server& server) {
        std::thread sendWorker(&UdpClient::sendData, this, std::ref(server));
        std::thread receiveWorker(&UdpClient::receiveData, this, std::ref(server));
        sendWorker.join();
        receiveWorker.join();
        return false;
    }
    char buf[4096] {"test"};
};