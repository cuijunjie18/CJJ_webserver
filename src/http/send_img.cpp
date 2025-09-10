#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <string.h>

void usage(){
    std::cerr << "usage: main <Port>" << std::endl;
}

std::string read_image(const std::string& image_path){
    std::ifstream is(image_path.c_str(), std::ifstream::in);
    is.seekg(0, is.end);
    int flength = is.tellg();
    std::cout << flength << std::endl;
    is.seekg(0, is.beg);
    char * buffer = new char[flength];
    is.read(buffer, flength);
    std::string image(buffer, flength);
    return image;
}

// std::string read_image_c(const std::string& image_path){
//     int fd = open(image_path.c_str(),O_RDONLY, 0664);
//     if (fd < 0){
//         std::cerr << "Failed to open image!" << std::endl;
//         return nullptr;
//     }
//     int flength = 0;
//     int n = 0;
//     char *p = buf;
//     while (n = read(fd,p,1024)){
//         p = p + n;
//         flength += n;
//     }
//     std::cout << flength << std::endl;
// }

void send_image(int socket,std::string image_data){
    // 构建HTTP响应头
    std::string header = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: image/png\r\n"
        "Content-Length: " + std::to_string(image_data.size()) + "\r\n\r\n";
        // "Connection:\r\n\r\n";

    // 先发送响应头
    if (send(socket, header.c_str(), header.size(), 0) <= 0) {
        std::cerr << "Failed to send header" << std::endl;
        return;
    }

    // 发送图片数据
    if (send(socket, image_data.c_str(), image_data.size(), 0) <= 0){
        std::cerr << "Faile to send image!" << std::endl;
        return;
    }
    std::cout << "Send image successfully!" << std::endl;
}

int main(int argc,char *argv[])
{
    if (argc < 2){
        usage();
        return -1;
    }
    int port = atoi(argv[1]);

    // Set server address
    sockaddr_in server_addr;
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0){
        std::cerr << "Failed to create listen socket!" << std::endl;
        return -1;
    }

    if (bind(listen_sock,(sockaddr *)&server_addr,sizeof(server_addr)) < 0){
        std::cerr << "Failed to bind port with listen socket!" << std::endl;
        return -1;
    }

    if (listen(listen_sock, 1) < 0){
        std::cerr << "Listen failed!" << std::endl;
        close(listen_sock);
        return -1;
    }

    std::cout << "Server is listening on port " << port << std::endl;

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int conn_socket = accept(listen_sock,(sockaddr*) &client_addr,&client_len);
    if (conn_socket < 0){
        close(listen_sock);
        std::cerr << "Accept client failed!" << std::endl;
        return -1;
    }

    char client_addr_name[105];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_addr_name, client_len);
    printf("Connect with client: %s\n",client_addr_name);

    std::string image_path = "/home/cjj/Desktop/my_project/CJJ_WebServer/assets/test_timer.png";
    std::string image_data = read_image(image_path);
    send_image(conn_socket, image_data);
    return 0;
}