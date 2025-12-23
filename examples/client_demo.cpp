#include <string>
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include "client/client.hpp"

void webbench() {
    CjjClient client;
    std::string server_ip = "10.21.15.19";
    int server_port = 50001;
    bool ret = client.init_client(server_ip.c_str(), server_port);
    assert(ret == true);
    std::cout << "Socket fd: " << client.m_sockfd << std::endl; 
    ret = client.send_data(server_ip.c_str(), server_ip.length());
    assert(ret == true);
}

int main(int argc, char *argv[]) 
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << " <client_num>" << std::endl;
        return 1;
    }
    int client_num = atoi(argv[1]);
    for (int i = 0; i < client_num; i++) {
        if (fork() == 0) {
            webbench();
            break;
        }
    }
    return 0;
}