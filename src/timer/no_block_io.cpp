#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/wait.h>
#define RD 0
#define WD 1

//对文件描述符设置非阻塞
int setnonblocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

bool flag;
char buf1[1024];
char buf2[1024];

int main()
{
    int fd[2];
    pipe(fd);
    setnonblocking(fd[RD]); // 设置读
    if (fork() == 0){
        close(fd[RD]);
        sleep(10);
        close(fd[WD]);
        return 0;
    }else{
        close(fd[WD]);
        int n = read(fd[RD], buf1, 1024);
        close(fd[RD]);
    }
    std::cout << "Wait son process exit..." << std::endl;
    wait(nullptr);
    return 0;
}