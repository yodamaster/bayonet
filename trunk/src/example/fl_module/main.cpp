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
 * @param   buf
 * @param   len
 *
 * @return  >0          已经recv完毕，截取总长度
 *          =0          继续recv
 *          <0          协议解析出错
 */
int net_handleinput(char* buf, int len)
{
    return len;
}

int udp_process(
    const char* srv_ip,int srv_port,uint32_t timeout_ms,
    const char *send_buf, int send_len,
    char *recv_buf,int max_len,int& recv_len
    )
{
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    int ret;

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
    {
        return -1;
    }

    struct sockaddr_in srv_addr;
    socklen_t addr_len = sizeof(srv_addr);
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    inet_aton(srv_ip, &srv_addr.sin_addr);
    srv_addr.sin_port = htons(srv_port);

    int nsend =0;
    while(nsend < send_len)
    {
        ret = sendto(sockfd,send_buf+nsend, (send_len-nsend), 0,(struct sockaddr *)(&srv_addr),addr_len);
        if (ret < 0)
        {
            if (errno == EINTR) //信号中断，或者缓冲区满
            {
                continue;
            }
            else
            {
                return -3;
            }
        }
        else
        {
            nsend += ret;
        }
    }

    struct sockaddr_in  recv_addr;
    int   recv_addr_len = sizeof(recv_addr);
    bzero(&recv_addr,sizeof(recv_addr));

    setsockopt(sockfd , SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

    recv_len = 0;
    while(true)
    {
        int left_len = max_len - recv_len;
        if (left_len <= 0)
        {
            return -4;
        }
        ret = recvfrom(sockfd,recv_buf+recv_len ,left_len , 0, (struct sockaddr*)&recv_addr,(socklen_t *) &recv_addr_len);
        if (ret == 0)
        {
            close(sockfd);
            return -5;
        }
        else if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            close(sockfd);
            return -6;
        }

        recv_len += ret;

        ret = net_handleinput(recv_buf, recv_len);
        if (ret == 0)
        {   
            continue;
        }   
        else if (ret < 0)
        {   
            close(sockfd);
            return -7;
        }   
        else
        {   
            recv_len = ret;
            break;
        } 
    }
    close(sockfd);
    return 0;
}

int tcp_process(
    const char* srv_ip,int srv_port,uint32_t timeout_ms,
    const char *send_buf, int send_len,
    char *recv_buf,int max_len,int& recv_len
    )
{
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    int ret;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( sockfd < 0 )
    {
        return -2;
    }

    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port   = htons(srv_port);
    if (inet_pton(AF_INET, srv_ip, &srv_addr.sin_addr) <= 0)
    {
        close(sockfd);
        return -1;
    }

    ret = connect(sockfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
    if (ret < 0)
    {
        close(sockfd);
        return -2;
    }

    int nsend =0;
    while(nsend < send_len)
    {
        ret = send(sockfd,send_buf+nsend,(send_len-nsend),0);
        if (ret < 0)
        {
            if (errno == EINTR) //信号中断，或者缓冲区满
            {
                continue;
            }
            else
            {
                return -3;
            }
        }
        else
        {
            nsend += ret;
        }
    }

    setsockopt(sockfd , SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

    recv_len = 0;
    while(true)
    {
        int left_len = max_len - recv_len;
        if (left_len <= 0)
        {
            return -4;
        }
        ret = recv(sockfd, recv_buf+recv_len, left_len, 0);
        if (ret == 0)
        {
            close(sockfd);
            return -5;
        }
        else if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            close(sockfd);
            return -6;
        }

        recv_len += ret;

        ret = net_handleinput(recv_buf, recv_len);
        if (ret == 0)
        {   
            continue;
        }   
        else if (ret < 0)
        {   
            close(sockfd);
            return -7;
        }   
        else
        {   
            recv_len = ret;
            break;
        } 
    }

    close(sockfd);
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
