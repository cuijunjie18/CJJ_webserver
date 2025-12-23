#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

class CjjClient {
public:
    CjjClient();
    ~CjjClient();

    bool init_client(const char* server_ip, int server_port);
    bool send_data(const char* data, size_t data_len);
    ssize_t receive_data(char* buffer, size_t buffer_size);
    bool close_client();

public:
    char m_read_buf[4096];
    char m_write_buf[4096];
    int m_sockfd{-1};
};

#endif