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
//我们这里假定返回包中，和pymman那边server相同的协议。即：len(11字节) + data(len)
//data = cmd(11) + ret(11) + body(len-33)
int tcp_handleinput(char* pBuf, int bufLen)
{
    printf("%d\n",bufLen);
    return bufLen;
    if (bufLen < 11)
    {
        return -1;
    }
    char allLenBuf[12];
    memcpy(allLenBuf,pBuf,11);
    allLenBuf[11]='\0';

    int allLen = atoi(allLenBuf);
    return allLen;
}

int tcp_connect_poll(const char* serverIp,int serverPort,unsigned timeoutMs)
{
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(serverPort);
    if (inet_pton(AF_INET, serverIp, &serv_addr.sin_addr) <= 0)
    {
        return -1;
    }

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if ( socketFd < 0 )
    {
        return -2;
    }

    //设置非阻塞
    int val = fcntl(socketFd, F_GETFL, 0);
    if (val == -1)
    {
        close(socketFd);
        return -3;
    }

    if (fcntl(socketFd, F_SETFL, val | O_NONBLOCK | O_NDELAY) == -1)
    {
        close(socketFd);
        return -4;
    }

    int ret;
    ret = connect(socketFd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret < 0)
    {
        if(errno != EINPROGRESS)
        {
            close(socketFd);
            return -1;
        }
        else
        {
            struct pollfd conncet_client[1];
            int nfd = 1;
            memset(&conncet_client[0],0,sizeof(pollfd));
            conncet_client[0].fd = socketFd;
            conncet_client[0].events = POLLIN | POLLOUT;
            int poll_timeout = timeoutMs;
            ret = ::poll(conncet_client, nfd, poll_timeout);
            if ( ret <= 0 )
            {
                close(socketFd);
                return -1;
            }
            else
            {
                if ((conncet_client[0].revents & POLLIN) || (conncet_client[0].revents & POLLOUT))
                {
                    int error;
                    int len = sizeof(error);
                    int bok = getsockopt(socketFd, SOL_SOCKET, SO_ERROR, &error,(socklen_t*)&len);
                    if (bok < 0)
                    {
                        close(socketFd);
                        return -1;
                    }
                    else if (error)
                    {
                        close(socketFd);
                        return -1;
                    }
                }
                else if ((conncet_client[0].revents & POLLERR) ||
                        (conncet_client[0].revents & POLLHUP) ||
                        (conncet_client[0].revents & POLLNVAL))
                {
                    close(socketFd);
                    return -1;
                }
            }
            return socketFd;
        }
    }
    return socketFd;
}
int tcp_send_poll(int socketFd, const char *sendBuf, int sendLen)
{
    int ret;
    int nsend =0;
    while(1)
    {
        ret = send(socketFd,sendBuf+nsend,(sendLen-nsend),0);
        if (ret <= 0)
        {
            if ( errno == EINTR || errno == EAGAIN ) //信号中断，或者缓冲区满
                continue;
            else
                break;
        }
        nsend += ret;
        if ( nsend >= sendLen )
            break;
    }
    if (nsend != sendLen)
    {
        return -1;
    }
    return 0;
}
int detect_single_read(int socketFd, unsigned timeoutMs)
{
    struct pollfd conncet_client[1];
    int nfd = 1;
    memset(&conncet_client[0],0,sizeof(pollfd));
    conncet_client[0].fd = socketFd;
    conncet_client[0].events = POLLIN ;

    return poll(conncet_client, nfd, timeoutMs);
}

int tcp_recv_poll(int socketFd,char *recvBuf,int maxLen,int& recvLen,unsigned timeoutMs)
{
    int ret = detect_single_read(socketFd,timeoutMs);
    if (ret <= 0)
    {
        return -2;
    }
    ret = recv(socketFd, recvBuf, maxLen, 0);
    if (ret <= 0)
    {
        return -3;
    }
    //已经接受到的数据长度
    recvLen = ret;

    ret = tcp_handleinput(recvBuf,recvLen);
    if (ret <= 0)
    {
        return -4;
    }

    int allLen = ret;
    int iRemain = allLen - recvLen;

    if (allLen > maxLen)
    {
        return -5;
    }

    while (iRemain > 0)
    {
        ret = detect_single_read(socketFd,timeoutMs);
        if(ret<=0)
        {
            return -6;
        }

        ret = recv(socketFd, recvBuf + recvLen, iRemain, 0);
        if (ret <= 0)
        {
            if ( errno == EINTR || errno == EAGAIN ) //信号中断，或者缓冲区满
                continue;
            else
                break;
        }
        recvLen += ret;
        iRemain -= ret;
    }
    if (recvLen != allLen)
    {
        return -7;
    }

    return 0;
}
int tcp_process_poll(
        const char* serverIp,int serverPort,unsigned timeoutMs,
        const char *sendBuf, int sendLen,
        char *recvBuf,int maxLen,int& recvLen
        )
{
    int ret;
    int socketFd = tcp_connect_poll(serverIp,serverPort,timeoutMs);
    if (socketFd<0)
    {
        return -1;
    }

    ret = tcp_send_poll(socketFd,sendBuf,sendLen);
    if (ret)
    {
        close(socketFd);
        return -2;
    }
    ret = tcp_recv_poll(socketFd, recvBuf, maxLen, recvLen,timeoutMs);
    if (ret)
    {
        close(socketFd);
        return -3;
    }
    close(socketFd);
    return 0;
}
int main(int argc, const char *argv[])
{
    string strSend = "woaini";
    string strRecv;
    strRecv.resize(1024);
    int len;
    int ret =tcp_process_poll(
            "0.0.0.0",10001,1000,
    //int ret =udp_process(
            //"0.0.0.0",20000,1000,
            strSend.c_str(),strSend.size(),
            (char*)strRecv.c_str(),strRecv.size(),len
            );
    cout<<"ret:"<< ret << ",recv:" << strRecv <<endl;
    return 0;
}
