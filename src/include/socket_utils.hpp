#include <sys/socket.h>
#include <netinet/in.h>

int initialize_server(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

sockaddr_in set_address(int port){
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    return serverAddress;
}

bool bind_socket(sockaddr_in server_address, int server_socket){
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Bind failed, error: " << strerror(errno) << std::endl;
        return false;
    }
    else return true;
}

bool socket_listen(int server_socket){
    if (listen(server_socket, 1) < 0) {
        std::cerr << "Listen failed, error: " << strerror(errno) << std::endl;
        return false;
    }
    else return true;
}

bool enable_socket_reuse(int server_socket){
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        std::cerr << "setsockopt(SO_REUSEADDR) failed, error: " << strerror(errno) << std::endl;
        return false;
    }
    else return true;
}

int accept_socket(int server_socket){
    return accept(server_socket, nullptr, nullptr);
}