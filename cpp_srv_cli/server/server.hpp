#pragma once

#include <iostream>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <type_traits>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <future>

#define VID_PACK_SIZE 1400

// Socket structures //

struct Socket {
    int port_;
    int sock_option_ = 1;
    int sock_listener_;
};
// Socket structure end //

// Client class //
struct Client {
    int sock_client_;
    sockaddr_in client_addr_;
    socklen_t client_addr_size_;

    char recvBuffer[4096];
    char sendBuffer[512];

    Socket socket;
};
// Client class ends //

// Server classes //
class Server {
public:
    virtual Client* addClient(int* port, int size) {
        port += 0;
        size += 0;
        return nullptr;
    }

    virtual int createSocket(Client& client, int port) {
        std::cout << "you've called the wrong function\n";
        return client.sock_client_ + port;
    }

    virtual int sendBuffer(Client &client, char* buf, int size) {
        std::cout << "you've called the wrong function\n";
        return client.sock_client_;
    }

    virtual int receiveBuffer(Client& client) {
        std::cout << "you've called the wrong function\n";
        return client.sock_client_;
    }

    sockaddr_in server_addr_;
    Client **clientPool;

private:

    virtual int acceptData(Client& client) {
        std::cout << "you've called the wrong function\n";
        return client.sock_client_;
    }

};

class TcpServer : public Server {
    int createSocket(Client& client, int port);
    int acceptClient(Client& client);
    int acceptData(Client& client);
    int reopenClient(Client& client);

    Client* addClient(int *port, int size) {
        std::thread* pool = (std::thread *)malloc(sizeof(std::thread) * size);
        clientPool = (Client**)malloc(sizeof(Client) * size);

        for (int i = 0; i < size; ++i) {
            clientPool[i] = new Client;
            pool[i] = std::thread(&TcpServer::createSocket, this, std::ref((*clientPool[i])), port[i]);
        }

        for (int i = 0; i < size; ++i) {
            pool[i].join();
        }
        return clientPool[0];
    }

    char m_buf[4096] {"test"};

public:

    int sendBuffer(Client &client, char* buf, int size);
    int receiveBuffer(Client& client);

};

class UdpServer : public Server {
    int createSocket(Client& client, int port);
    int acceptData(Client& client);

    Client* addClient(int* port, int size) {
        std::thread* pool = (std::thread *)malloc(sizeof(std::thread) * size);
        clientPool = (Client**)malloc(sizeof(Client) * size);

        for (int i = 0; i < size; ++i) {
            clientPool[i] = new Client;
            pool[i] = std::thread(&UdpServer::createSocket, this, std::ref((*clientPool[i])), port[i]);
        }

        for (int i = 0; i < size; ++i) {
            pool[i].join();
        }
        return clientPool[0];
    }

    char m_buf[4096] {"test"};
    char m_splittedBuffer[17][1400];
public:
    int sendBuffer(Client &client, char* buf, int size);
    int receiveBuffer(Client& client);
    void splitBuffer();
};

