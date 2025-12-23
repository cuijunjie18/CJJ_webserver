#include <string>
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include "client/client.hpp"

void webbench() {
    // Connect to server
    CjjClient client;
    std::string server_ip = "10.21.15.19";
    int server_port = 50001;
    bool ret = client.init_client(server_ip.c_str(), server_port);
    assert(ret == true);
    std::cout << "Connected to server " << server_ip << ":" << server_port << std::endl;

    // Send data to server
    char buffer[1024] = "GET / HTTP/1.1\r\n"
                        "Host: 10.21.15.19\r\n"
                        "Connection: close\r\n"
                        "\r\n";
    ret = client.send_data(buffer, strlen(buffer));
    assert(ret == true);

    // recvive data from server
    char recv_buf[4096];
    int bytes = client.receive_data(recv_buf, sizeof(recv_buf) - 1);
    if (bytes > 0) {
        recv_buf[bytes] = '\0';
        std::cout << "Received " << bytes << " bytes from server:" << std::endl;
        // std::cout << recv_buf << std::endl;
    } else {
        std::cout << "No data received from server." << std::endl;
    }
    
    // 正确关闭客户端连接
    client.close_client();
}

int main(int argc, char *argv[]) 
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << " <client_num>" << std::endl;
        return 1;
    }
    int client_num = atoi(argv[1]);
    
    // 创建指定数量的子进程
    for (int i = 0; i < client_num; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程执行测试并退出
            webbench();
            exit(0);
        } else if (pid < 0) {
            perror("fork failed");
            return 1;
        }
        // 父进程继续循环创建下一个子进程
    }
    
    // 父进程等待所有子进程完成
    for (int i = 0; i < client_num; i++) {
        wait(NULL);
    }
    
    return 0;
}
