
#include "server.hpp"

unsigned char vidBuf[1400];
bool first_buf = false;
int ctrlBuf[1];
int recvLen = 0;
void createTcpPorts(){
    Server* server = new TcpServer;
    int ports[] = {9001, 9002};
    server->addClient(ports, 2);


    while(true) {
        server->receiveBuffer(*server->clientPool[0]);
        server->sendBuffer(*server->clientPool[1], server->clientPool[0]->recvBuffer, 512);
    }
}

void createUdpPort(){
    Server* server = new UdpServer;
    int ports[] = {9006, 9007};
    server->addClient(ports, 2);
    server->receiveBuffer(*server->clientPool[1]);
    server->receiveBuffer(*server->clientPool[0]);
    while(true) {
        server->receiveBuffer(*server->clientPool[0]);
        server->sendBuffer(*server->clientPool[1], server->clientPool[0]->recvBuffer, 64000);
    }
}

int main() {
//    std::thread tcpWorker(createTcpPorts);
    std::thread udpWorker(createUdpPort);
//    tcpWorker.join();
    udpWorker.join();
    return 0;
}

int TcpServer::acceptClient(Client& client){

    if ((client.sock_client_ = accept(client.socket.sock_listener_, (sockaddr*)&client.client_addr_, &client.client_addr_size_)) < 0) {
        std::cerr << "[ERROR] Connections cannot be accepted for a reason.\n";
        return -5;
    }

    std::cout << "[INFO] A connection is accepted now.\n";


    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    if (getnameinfo(
            (sockaddr*)&client.client_addr_, client.client_addr_size_,
            host, NI_MAXHOST,
            svc, NI_MAXSERV, 0) != 0) {
        std::cout << "[INFO] Client: (" << inet_ntop(AF_INET, &client.client_addr_.sin_addr, m_buf, INET_ADDRSTRLEN)
                  << ":" << ntohs(client.client_addr_.sin_port) << ")\n";
    } else {
        std::cout << "[INFO] Client: (host: " << host << ", service: " << svc << ")\n";
    }
//    acceptData(client);
    return true;
//    return acceptData(client);
}

int TcpServer::acceptData(Client& client) {
    char msg_buf[4096];
    int bytes;
    // While receiving - display & echo msg
    while (true) {
        bytes = recv(client.sock_client_, &msg_buf, 4096, 0);

        if (bytes == 0) {
            std::cout << "[INFO] Client is disconnected.\n";
            break;
        }
        if (bytes < 0) {
            std::cerr << "[ERROR] Something went wrong while receiving data!.\n";
            break;
        }
        else {
            // Print message
            std::cout << "client> " << std::string(msg_buf, 0, bytes) << "\n";
            std::cout << "client> " << std::string(msg_buf, 0, bytes) << "\n";
            if(m_buf == std::string("exit")) {
                break;
            }
            if (send(client.sock_client_, &m_buf, bytes, 0) < 0) {
                std::cerr << "[ERROR] Message cannot be send, exiting...\n";
                break;
            }
        }
    }
// close(client.socket.sock_listener_);
//    createSocket(client, client.socket.port_);
    return reopenClient(client);
}

int TcpServer::reopenClient(Client& client) {

    setsockopt(client.socket.sock_listener_, SOL_SOCKET,  SO_REUSEADDR,(char *)&client.socket.sock_option_, sizeof(int));

    client.client_addr_size_ = sizeof(client.client_addr_);
//  client.socket.sock_listener_ = socket(AF_INET, SOCK_STREAM, 0);
    if ((client.sock_client_ = accept(client.socket.sock_listener_, (sockaddr*)&client.client_addr_, &client.client_addr_size_)) < 0) {
        std::cerr << "[ERROR] Connections cannot be accepted for a reason.\n";
        return -5;
    }

    std::cout << "[INFO] A connection is accepted now.\n";


    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    if (getnameinfo(
            (sockaddr*)&client.client_addr_, client.client_addr_size_,
            host, NI_MAXHOST,
            svc, NI_MAXSERV, 0) != 0) {
        std::cout << "[INFO] Client: (" << inet_ntop(AF_INET, &client.client_addr_.sin_addr, m_buf, INET_ADDRSTRLEN)
                  << ":" << ntohs(client.client_addr_.sin_port) << ")\n";
    } else {
        std::cout << "[INFO] Client: (host: " << host << ", service: " << svc << ")\n";
    }
    return true;
//  return acceptData(client);

}

int TcpServer::createSocket(Client& client, int port) {
    client.socket.port_ = port;
    client.socket.sock_listener_ = socket(AF_INET, SOCK_STREAM, 0);
    // Check If the socket is created
    if (client.socket.sock_listener_ < 0) {
        std::cerr << "[ERROR] Socket cannot be created!\n";
        return -2;
    }


    std::cout << "[INFO] Socket has been created.\n";

    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(client.socket.port_);
    server_addr_.sin_addr.s_addr = INADDR_ANY;

//  char buf[INET_ADDRSTRLEN];

    // Bind socket
    if (bind(client.socket.sock_listener_, (sockaddr *)&server_addr_, sizeof(server_addr_)) < 0) {
        std::cerr << "[ERROR] Created socket cannot be binded to ( "
                  << inet_ntop(AF_INET, &server_addr_.sin_addr, m_buf, INET_ADDRSTRLEN)
                  << ":" << ntohs(server_addr_.sin_port) << ")\n";
        return -3;
    }
    // udp icin recv from diye bi fonksiyon var onu kullanman lazim
    std::cout << "[INFO] Sock is binded to ("
              << inet_ntop(AF_INET, &server_addr_.sin_addr, m_buf, INET_ADDRSTRLEN)
              << ":" << ntohs(server_addr_.sin_port) << ")\n";


    if (listen(client.socket.sock_listener_, SOMAXCONN) < 0) {
        std::cerr << "[ERROR] Socket cannot be switched to listen mode!\n";
        return -4;
    }
    std::cout << "[INFO] Socket is listening now.\n";
//   auto a = std::async(&TcpServer::acceptClient, this, std::ref(client));
    return acceptClient(client);
}

int UdpServer::createSocket(Client& client, int port) {
    client.socket.port_ = port;
    client.socket.sock_listener_ = socket(AF_INET, SOCK_DGRAM, 0);
    // Check If the socket is created
    if (client.socket.sock_listener_ < 0) {
        std::cerr << "[ERROR] Socket cannot be created!\n";
        return -2;
    }
    std::cout << "[INFO] Socket has created\n";
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(client.socket.port_);
    server_addr_.sin_addr.s_addr = INADDR_ANY;
    //    inet_aton("127.0.0.1", &server_addr_.sin_addr);

//  char buf[INET_ADDRSTRLEN];

    // Bind socket
    if (bind(client.socket.sock_listener_, (sockaddr *)&server_addr_, sizeof(server_addr_)) < 0) {
        std::cerr << "[ERROR] Created socket cannot be binded to ( "
                  << inet_ntop(AF_INET, &server_addr_.sin_addr, m_buf, INET_ADDRSTRLEN)
                  << ":" << ntohs(server_addr_.sin_port) << ")\n";
        return -3;
    }
    std::cout << "[INFO] Sock is binded to ("
              << inet_ntop(AF_INET, &server_addr_.sin_addr, m_buf, INET_ADDRSTRLEN)
              << ":" << ntohs(server_addr_.sin_port) << ")\n";


//    return acceptData(client);
}

int UdpServer::acceptData(Client& client) {
    client.client_addr_size_ = sizeof(client.client_addr_);
    while (true) {

        if (recvfrom(client.socket.sock_listener_, m_buf, SOMAXCONN, MSG_WAITALL, (struct sockaddr *)&client.client_addr_, &client.client_addr_size_)< 0) {
            std::cerr << "[ERROR] Socket cannot be switched to listen mode!\n";
            return -4;
        }
        std::cout << std::string(m_buf);
    }
}

int TcpServer::receiveBuffer(Client &client) {
    static int bytes;
    bytes = recv(client.sock_client_, &client.recvBuffer, 512, 0);
    for(int i = 0; i < 11; ++i)
        printf("%d\t", client.recvBuffer[i]);
    printf("\n");
    //std::cout << std::string(client.recvBuffer) << "\n";
    if(bytes == 0) {
        reopenClient(client);
    }
    return 0;
}

int TcpServer::sendBuffer(Client &client, char* buf, int size) {

    if (send(client.sock_client_, buf, size, 0) < 0) {
        std::cerr << "[ERROR] Message cannot be send, exiting...\n";
        reopenClient(client);
    }
    //std::cout < std::string(buf) << "\n";
    for(int i = 0; i < 10; i++) {
        printf("%d", buf[i]);
    }
    printf("\n");
    return 0;
}

int UdpServer::receiveBuffer(Client &client) {

    client.client_addr_size_ = sizeof(client.client_addr_);
    int anan = recvfrom(client.socket.sock_listener_, (char*)vidBuf, SOMAXCONN, MSG_WAITALL, (struct sockaddr *)&client.client_addr_, &client.client_addr_size_);
    std::cout << anan << " bu da benim \n";
    if (anan < 0) {
        std::cerr << "[ERROR] Socket cannot be switched to listen mode!\n";
        return -4;
    }
    else if(anan <= 4) {
        std::memcpy(ctrlBuf, vidBuf, sizeof(int));
        sendto(clientPool[1]->socket.sock_listener_, ctrlBuf, sizeof(ctrlBuf), MSG_DONTWAIT, (const struct sockaddr *)&clientPool[1]->client_addr_, sizeof(clientPool[1]->client_addr_));
        memset(vidBuf, 0, sizeof(vidBuf));
        first_buf = false;
    }
    else {
        first_buf = true;
    }
    std::cout << "i've received this much packet =" << anan<< " \n";
    return 0;
}

int UdpServer::sendBuffer(Client &client, char* buf, int size) {
    if(first_buf){
        int anan = sendto(client.socket.sock_listener_, (const char*)vidBuf, sizeof(vidBuf), MSG_DONTWAIT, (const struct sockaddr *)&client.client_addr_, sizeof(client.client_addr_));
        if(anan<0){
            std::cout << "sorun var" << anan << "\n";
        }
    }
    return 0;
}

void UdpServer::splitBuffer() {
    for (int i = 0; i < VID_PACK_SIZE; ++i) {
        m_splittedBuffer[i][0] = i + '0';
        //std::memset(m_splittedBuffer, (vidBuf + (i * (VID_PACK_SIZE - 1)), VID_PACK_SIZE - 1));
    }
}
