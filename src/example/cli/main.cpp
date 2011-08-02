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

/**
 * @brief   告诉recv是否继续
 *
 * @param   pBuf
 * @param   bufLen
 *
 * @return  >0          已经recv完毕，截取总长度
 *          =0          继续recv
 *          <0          协议解析出错
 */
int net_handleinput(char* pBuf, int bufLen)
{
    return bufLen;
}

int udp_process(
        const char* serverIp,int serverPort,uint32_t timeoutMs,
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

    recvLen = 0;
    while(true)
    {
        int leftLen = maxLen - recvLen;
        if (leftLen <= 0)
        {
            return -4;
        }
        ret = recvfrom(socketFd,recvBuf+recvLen ,leftLen , 0, (struct sockaddr*)&recv_addr,(socklen_t *) &recv_addr_len);
        if (ret == 0)
        {
            close(socketFd);
            return -5;
        }
        else if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            close(socketFd);
            return -6;
        }

        recvLen += ret;

        ret = net_handleinput(recvBuf, recvLen);
        if (ret == 0)
        {   
            continue;
        }   
        else if (ret < 0)
        {   
            close(socketFd);
            return -7;
        }   
        else
        {   
            recvLen = ret;
            break;
        } 
    }
    close(socketFd);
    return 0;
}

int tcp_process(
        const char* serverIp,int serverPort,uint32_t timeoutMs,
        const char *sendBuf, int sendLen,
        char *recvBuf,int maxLen,int& recvLen
        )
{
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = timeoutMs*1000;

    int ret;

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if ( socketFd < 0 )
    {
        return -2;
    }

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(serverPort);
    if (inet_pton(AF_INET, serverIp, &serv_addr.sin_addr) <= 0)
    {
        close(socketFd);
        return -1;
    }

    ret = connect(socketFd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret < 0)
    {
        close(socketFd);
        return -2;
    }

    int nsend =0;
    while(1)
    {
        ret = send(socketFd,sendBuf+nsend,(sendLen-nsend),0);
        if (ret <= 0)
        {
            if (errno == EINTR) //信号中断，或者缓冲区满
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
        return -3;
    }

    setsockopt(socketFd , SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

    recvLen = 0;
    while(true)
    {
        int leftLen = maxLen - recvLen;
        if (leftLen <= 0)
        {
            return -4;
        }
        ret = recv(socketFd, recvBuf+recvLen, leftLen, 0);
        if (ret == 0)
        {
            close(socketFd);
            return -5;
        }
        else if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            close(socketFd);
            return -6;
        }

        recvLen += ret;

        ret = net_handleinput(recvBuf, recvLen);
        if (ret == 0)
        {   
            continue;
        }   
        else if (ret < 0)
        {   
            close(socketFd);
            return -7;
        }   
        else
        {   
            recvLen = ret;
            break;
        } 
    }

    close(socketFd);
    return 0;
}

int tcp_connect_poll(const char* serverIp,int serverPort,uint32_t timeoutMs)
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

int detect_single_read(int socketFd, uint32_t timeoutMs)
{
    struct pollfd conncet_client[1];
    int nfd = 1;
    memset(&conncet_client[0],0,sizeof(pollfd));
    conncet_client[0].fd = socketFd;
    conncet_client[0].events = POLLIN ;

    return poll(conncet_client, nfd, timeoutMs);
}

int tcp_recv_poll(int socketFd,char *recvBuf,int maxLen,int& recvLen,uint32_t timeoutMs)
{
    int ret;
    recvLen = 0;
    while(true)
    {
        int leftLen = maxLen - recvLen;
        if (leftLen <= 0)
        {
            return -4;
        }

        ret = detect_single_read(socketFd,timeoutMs);
        if (ret <= 0)
        {
            return -1;
        }

        ret = recv(socketFd, recvBuf+recvLen, leftLen, 0);
        if (ret == 0)
        {
            close(socketFd);
            return -5;
        }
        else if (ret < 0)
        {
            if ( errno == EINTR || errno == EAGAIN )
            {
                continue;
            }
            close(socketFd);
            return -6;
        }

        recvLen += ret;

        ret = net_handleinput(recvBuf, recvLen);
        if (ret == 0)
        {   
            continue;
        }   
        else if (ret < 0)
        {   
            close(socketFd);
            return -7;
        }   
        else
        {   
            recvLen = ret;
            break;
        } 
    }
    return 0;
}

int tcp_process_poll(
        const char* serverIp,int serverPort,uint32_t timeoutMs,
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
    //int ret =tcp_process(
            "0.0.0.0",10001,1000,
    //int ret =udp_process(
            //"0.0.0.0",20000,1000,
            strSend.c_str(),strSend.size(),
            (char*)strRecv.c_str(),strRecv.size(),len
            );
    cout<<"ret:"<< ret << ",recv:" << strRecv <<endl;
    return 0;
}
