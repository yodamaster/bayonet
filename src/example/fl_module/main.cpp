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

    int nsend =0;
    while(1)
    {
        ret = sendto(socketFd,sendBuf+nsend, (sendLen-nsend), 0,(struct sockaddr *)(&serv_addr),addr_len);
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
        close(socketFd);
        return -3;
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

/**
 * @brief   第一次进入so时，需要做的初始化工作，只会执行一次。
 *
 * @return  0               succ
 *          else            fail
 */
extern "C" int fuload_handle_init()
{
    return 0;
}

/**
 * @brief   业务逻辑，每次进入
 *
 * @param   mapParams       将输入数据按照url方式解析之后的key-value结构
 *
 * @return  0               succ
 *          else            返回值,会用来做统计
 */
extern "C" int fuload_handle_process(map<string,string>& mapParams)
{
    
    string strSend = "woaini";
    string strRecv;
    strRecv.resize(1024);
    int len;
    int ret =tcp_process(
            "0.0.0.0",10001,1000,
    //int ret =udp_process(
            //"0.0.0.0",20000,1000,
            strSend.c_str(),strSend.size(),
            (char*)strRecv.c_str(),strRecv.size(),len
            );
    return ret;
}

/**
 * @brief   so结束时的收尾工作，只会调用一次。一般不用编写
 *
 * @return  0               succ
 *          else            fail
 */
extern "C" int fuload_handle_fini()
{
    return 0;
}
