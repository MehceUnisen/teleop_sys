#include "client.hpp"


int main() {

//    if (argc <= 1) {
//        std::cerr << "Please enter the host ip addr\n";
//        return -1;
//    }

    Client* cl = new UdpClient;
    char ip[15] = "18.157.160.198";
    cl->createSocket(new Server, ip, 9007);
    // cl->createSocket(new Server, ip, 9002);
    // std::cin.get();
    
    return 0;
   
}

bool TcpClient::createSocket(Server* server, char* host_addr, int port) {
    
    server->cli_socket.port_ = port;
    server->host_addr_ = host_addr; 
    //open port
    server->cli_socket.sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(server->cli_socket.sock_fd_ < 0) {
        std::cerr << "Failed while creating socket\n";
        return false;
    }

    std::cout << "Socket has created\n";
    return establishConnection(*server);
}

bool UdpClient::createSocket(Server* server, char* host_addr, int port) {
    
    server->cli_socket.port_ = port;
    server->host_addr_ = host_addr; 
    //open port
    server->cli_socket.sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if(server->cli_socket.sock_fd_ < 0) {
        std::cerr << "Failed while creating socket\n";
        return false;
    }

    std::cout << "Socket has created\n";
    return acceptData(*server);
}

bool TcpClient::establishConnection(Server& server){

    //get host by name
    server.server_ = gethostbyname(server.host_addr_);

    if(!server.server_) {
        std::cerr << "Failed to find host\n";
        return false;
    }

    std::cout << "Server has found\n";
    //get host by name ended

    // Fill address fields before try to connect
    std::memset((char*)&server.server_addr_, 0, sizeof(server.server_addr_));
    server.server_addr_.sin_family = AF_INET;
    server.server_addr_.sin_port = htons(server.cli_socket.port_);

    inet_aton(server.host_addr_, &server.server_addr_.sin_addr); 
    
    if (server.server_->h_addr_list[0])
        std::memcpy((char*)server.server_->h_addr_list[0], (char*)&server.server_addr_.sin_addr, server.server_->h_length);
    else {
        std::cerr << "[ERROR] There is no a valid address for that hostname!\n";
        return false;
    }
    if (connect(server.cli_socket.sock_fd_, (sockaddr*)&server.server_addr_, sizeof(server.server_addr_)) < 0) {
        std::cerr << "Connection cannot be established!\n";
        return false;
    }
    

    std::cout << "[INFO] Connection established.\n";
    return acceptData(server);
}

bool TcpClient::sendData(Server& server) {
    static char buf[10] = "aaaaaa!";
    while(true){
//        static std::string temp;
//        std::memset(buf, 0, 4096);
//        std::cout << "> ";
//        std::getline(std::cin, temp, '\n');
//        std::strcpy(buf, temp.c_str());

        // Send the data that buffer contains
        int bytes_send = send(server.cli_socket.sock_fd_, &buf, (size_t)strlen(buf), 0);
        // Check if message sending is successful
        if (bytes_send < 0) {
           std::cerr << "[ERROR] Message cannot be sent!\n";
        }
    }
}

bool UdpClient::sendData(Server& server) {
    while(true) {
        sendto(server.cli_socket.sock_fd_, (const char *)buf, sizeof(buf), 
            MSG_CONFIRM, (const struct sockaddr *) &server.server_addr_,  
                *server.server_addr_size); 
    }
}

bool TcpClient::receiveData(Server& server){
    while(true) {
        // Wait for a message
        int bytes_recv = recv(server.cli_socket.sock_fd_, &buf, 4096, 0);

        // Check how many bytes recieved
        // If something gone wrong
        if (bytes_recv < 0) {
        //   std::cerr << "[ERROR] Message cannot be recieved!\n";
        }
        // If there is no data, it means server is disconnected
        else if (bytes_recv == 0) {
          std::cout << "[INFO] Server is disconnected.\n";
        }
        // If there is some bytes, print
        else {
          std::cout << "server> " << std::string(buf, 0, bytes_recv) << "\n";
        }       
    }
}

bool UdpClient::receiveData(Server& server) {
    while(true) {
        int n = recvfrom(server.cli_socket.sock_fd_, (char*)buf, 4096,
                    MSG_WAITALL, (struct sockaddr *) &server.server_addr_, 
                    server.server_addr_size); 
        buf[n] = '\0'; 
        std::cout << buf;
    }    
}