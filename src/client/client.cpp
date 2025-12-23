#include "client.hpp"
#include <cstdio>

CjjClient::CjjClient() {
    
}

CjjClient::~CjjClient() {

}

bool CjjClient::init_client(const char *server_ip, int server_port) {
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        perror("Socket creation failed");
        return false;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(m_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(m_sockfd);
        return false;
    }

    return true;
}

bool CjjClient::send_data(const char *data, size_t data_len) {
    ssize_t total_sent = 0;
    while (total_sent < data_len) {
        ssize_t sent = send(m_sockfd, data + total_sent, data_len - total_sent, 0);
        if (sent < 0) {
            perror("Send failed");
            return false;
        }
        total_sent += sent;
    }
    return true;
}

ssize_t CjjClient::receive_data(char *buffer, size_t buffer_size) {
    ssize_t received = recv(m_sockfd, buffer, buffer_size - 1, 0);
    if (received < 0) {
        perror("Receive failed");
        return -1;
    }
    buffer[received] = '\0'; // Null-terminate the received data
    return received;
}

bool CjjClient::close_client() {
    if (m_sockfd != -1) {
        close(m_sockfd);
        m_sockfd = -1;
    }
    return true;
}