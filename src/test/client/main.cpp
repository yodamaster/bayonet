#include <error.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;
int udp_process(
        const char* serverIp,int serverPort,unsigned timeoutMs,
        const char *sendBuf, int sendLen,
        char *recvBuf,int maxLen,int& recvLen
        )
{
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = timeoutMs*1000;

    int ret;

    int socketFd = socket(AF_INET,SOCK_DGRAM,0);
    if(socketFd < 0)
    {
        return -1;
    }

    struct sockaddr_in serv_addr;
    socklen_t addr_len = sizeof(serv_addr);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_aton(serverIp, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(serverPort);

    ret = sendto(socketFd,sendBuf, sendLen, 0,(struct sockaddr *)(&serv_addr),addr_len);
    if (ret != sendLen)
    {
        close(socketFd);
        return -2;
    }

    struct sockaddr_in  recv_addr;
    int   recv_addr_len = sizeof(recv_addr);
    bzero(&recv_addr,sizeof(recv_addr));

    setsockopt(socketFd , SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    ret = recvfrom(socketFd,recvBuf,maxLen,0,(struct sockaddr*)&recv_addr,(socklen_t *) &recv_addr_len);
    if ( ret <= 0 )
    {
        close(socketFd);
        return -3;
    }
    recvLen = ret;
    close(socketFd);
    return 0;
}
int main(int argc, const char *argv[])
{
    string strSend = "woaini";
    string strRecv;
    strRecv.resize(1024);
    int len;
    int ret =udp_process(
            "127.0.0.1",80,100,
            strSend.c_str(),strSend.size(),
            (char*)strRecv.c_str(),strRecv.size(),len
            );
    cout<<ret<<endl;
    return 0;
}
