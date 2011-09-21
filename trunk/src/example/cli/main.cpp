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

    struct StSockFree
    {
        StSockFree(int sockfd)
        {
            m_sockfd = sockfd;
        }
        ~StSockFree()
        {
            if (m_sockfd > 0)
            {
                close(m_sockfd);
                m_sockfd = -1;
            }
        }
        int m_sockfd;
    } st_sock_free(sockfd);

    struct sockaddr_in srv_addr;
    socklen_t addr_len = sizeof(srv_addr);
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    inet_aton(srv_ip, &srv_addr.sin_addr);
    srv_addr.sin_port = htons(srv_port);

    ret = sendto(sockfd,send_buf, send_len, 0,(struct sockaddr *)(&srv_addr),addr_len);
    if (ret != send_len)
    {
        return -2;
    }

    struct sockaddr_in  recv_addr;
    int   recv_addr_len = sizeof(recv_addr);
    bzero(&recv_addr,sizeof(recv_addr));

    setsockopt(sockfd , SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

    ret = recvfrom(sockfd, recv_buf, max_len , 0, (struct sockaddr*)&recv_addr,(socklen_t *) &recv_addr_len);
    if (ret <= 0)
    {
        return -3;
    }
    recv_len = ret;

    ret = net_handleinput(recv_buf, recv_len);
    if (ret == 0)
    {   
        return -4;
    }   
    else if (ret < 0)
    {   
        return -5;
    }   

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
        return -1;
    }

    struct StSockFree
    {
        StSockFree(int sockfd)
        {
            m_sockfd = sockfd;
        }
        ~StSockFree()
        {
            if (m_sockfd > 0)
            {
                close(m_sockfd);
                m_sockfd = -1;
            }
        }
        int m_sockfd;
    } st_sock_free(sockfd);

    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port   = htons(srv_port);
    if (inet_pton(AF_INET, srv_ip, &srv_addr.sin_addr) <= 0)
    {
        return -2;
    }

    ret = connect(sockfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
    if (ret < 0)
    {
        return -3;
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
            return -4;
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
            return -5;
        }
        ret = recv(sockfd, recv_buf+recv_len, left_len, 0);
        if (ret == 0)
        {
            return -6;
        }
        else if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -7;
        }

        recv_len += ret;

        ret = net_handleinput(recv_buf, recv_len);
        if (ret == 0)
        {   
            continue;
        }   
        else if (ret < 0)
        {   
            return -8;
        }   
        else
        {   
            recv_len = ret;
            break;
        } 
    }

    return 0;
}

int tcp_connect_poll(const char* srv_ip,int srv_port,uint32_t timeout_ms)
{
    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port   = htons(srv_port);
    if (inet_pton(AF_INET, srv_ip, &srv_addr.sin_addr) <= 0)
    {
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( sockfd < 0 )
    {
        return -2;
    }

    //设置非阻塞
    int val = fcntl(sockfd, F_GETFL, 0);
    if (val == -1)
    {
        close(sockfd);
        return -3;
    }

    if (fcntl(sockfd, F_SETFL, val | O_NONBLOCK | O_NDELAY) == -1)
    {
        close(sockfd);
        return -4;
    }

    int ret;
    ret = connect(sockfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
    if (ret < 0)
    {
        if(errno != EINPROGRESS)
        {
            close(sockfd);
            return -1;
        }
        else
        {
            struct pollfd conncet_client[1];
            int nfd = 1;
            memset(&conncet_client[0],0,sizeof(pollfd));
            conncet_client[0].fd = sockfd;
            conncet_client[0].events = POLLIN | POLLOUT;
            int poll_timeout = timeout_ms;
            ret = ::poll(conncet_client, nfd, poll_timeout);
            if ( ret <= 0 )
            {
                close(sockfd);
                return -1;
            }
            else
            {
                if ((conncet_client[0].revents & POLLIN) || (conncet_client[0].revents & POLLOUT))
                {
                    int error;
                    int len = sizeof(error);
                    int bok = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error,(socklen_t*)&len);
                    if (bok < 0)
                    {
                        close(sockfd);
                        return -1;
                    }
                    else if (error)
                    {
                        close(sockfd);
                        return -1;
                    }
                }
                else if ((conncet_client[0].revents & POLLERR) ||
                         (conncet_client[0].revents & POLLHUP) ||
                         (conncet_client[0].revents & POLLNVAL))
                {
                    close(sockfd);
                    return -1;
                }
            }
            return sockfd;
        }
    }
    return sockfd;
}

int tcp_send_poll(int sockfd, const char *send_buf, int send_len)
{
    int ret;
    int nsend =0;
    while(nsend < send_len)
    {
        ret = send(sockfd,send_buf+nsend,(send_len-nsend),0);
        if (ret < 0) // 0 is not fail
        {
            if ( errno == EINTR || errno == EAGAIN ) //信号中断，或者缓冲区满
            {
                continue;
            }
            return -1;
        }
        else
        {
            nsend += ret;
        }
    }
    return 0;
}

int detect_single_read(int sockfd, uint32_t timeout_ms)
{
    struct pollfd conncet_client[1];
    int nfd = 1;
    memset(&conncet_client[0],0,sizeof(pollfd));
    conncet_client[0].fd = sockfd;
    conncet_client[0].events = POLLIN ;

    return poll(conncet_client, nfd, timeout_ms);
}

int tcp_recv_poll(int sockfd,char *recv_buf,int max_len,int& recv_len,uint32_t timeout_ms)
{
    int ret;
    recv_len = 0;
    while(true)
    {
        int left_len = max_len - recv_len;
        if (left_len <= 0)
        {
            return -4;
        }

        ret = detect_single_read(sockfd,timeout_ms);
        if (ret <= 0)
        {
            return -1;
        }

        ret = recv(sockfd, recv_buf+recv_len, left_len, 0);
        if (ret == 0)
        {
            return -5;
        }
        else if (ret < 0)
        {
            if ( errno == EINTR || errno == EAGAIN )
            {
                continue;
            }
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
            return -7;
        }   
        else
        {   
            recv_len = ret;
            break;
        } 
    }
    return 0;
}

int tcp_process_poll(
    const char* srv_ip,int srv_port,uint32_t timeout_ms,
    const char *send_buf, int send_len,
    char *recv_buf,int max_len,int& recv_len
    )
{
    int ret;
    int sockfd = tcp_connect_poll(srv_ip,srv_port,timeout_ms);
    if (sockfd<0)
    {
        return -1;
    }

    ret = tcp_send_poll(sockfd,send_buf,send_len);
    if (ret)
    {
        close(sockfd);
        return -2;
    }
    ret = tcp_recv_poll(sockfd, recv_buf, max_len, recv_len,timeout_ms);
    if (ret)
    {
        close(sockfd);
        return -3;
    }
    close(sockfd);
    return 0;
}

int main(int argc, const char *argv[])
{
    string strSend = "woaini";
    string strRecv;
    strRecv.resize(1024);
    int len;
    //int ret =tcp_process_poll(
    int ret =tcp_process(
            "0.0.0.0",10001,1000,
    //int ret =udp_process(
            //"0.0.0.0",20000,1000,
            strSend.c_str(),strSend.size(),
            (char*)strRecv.c_str(),strRecv.size(),len
            );
    cout<<"ret:"<< ret << ",recv:" << strRecv <<endl;
    return 0;
}
