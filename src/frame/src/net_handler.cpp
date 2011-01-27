/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        net_handler.cpp
#  Description:
#  Version:         1.0
#  LastChange:      2011-01-27 15:03:49
#  History:
=============================================================================*/
#include "net_handler.h"
CNetHandlerBase::~CNetHandlerBase ()
{
    Close();
}
int CNetHandlerBase::Init(string ip,int port)
{
    m_IP = ip;
    m_Port = port;
    return 0;
}
int CNetHandlerBase::Init(int socketFd)
{
    m_SocketFd = socketFd;
    int ret = SetNoBlock(m_SocketFd);
    if (ret)
    {
        error_log("SetNoBlock error:%d",ret);
        return -1;
    }
    return 0;
}
int CNetHandlerBase::GetSocketFd()
{
    return m_SocketFd;
}
string CNetHandlerBase::GetClientIp()
{
    return m_ClientIp;
}

int CNetHandlerBase::GetClientPort()
{
    return m_ClientPort;
}
int CNetHandlerBase::SetNoBlock(int socketFd)
{
    int val = fcntl(socketFd, F_GETFL, 0);
    if (val == -1)
    {
        error_log("fcntl get error");
        return -1;
    }
    if (fcntl(socketFd, F_SETFL, val | O_NONBLOCK | O_NDELAY) == -1)
    {
        error_log("fcntl set error");
        return -2;
    }
    return 0;
}
int CNetHandlerBase::Close()
{
    if (m_SocketFd>0)
    {
        close(m_SocketFd);
        m_SocketFd = -1;
    }
    return 0;
}
//=============================================================================
int CNetHandlerTcp::Create()
{
    if (m_SocketFd>0)
    {
        return 0;
    }
    int _fd = socket(AF_INET, SOCK_STREAM, 0);
    if ( _fd <= 0 )
    {
        error_log("socket create error:%d",_fd);
        return -1;
    }
    m_SocketFd = _fd;
    int ret = SetNoBlock(m_SocketFd);
    if (ret)
    {
        close(m_SocketFd);
        return -2;
    }
    linger   m_sLinger;
    m_sLinger.l_onoff   =   1;
    m_sLinger.l_linger   =   0;
    setsockopt(m_SocketFd, SOL_SOCKET, SO_LINGER,(const char*)&m_sLinger,sizeof(linger));

    ret = netConnect();
    if (ret)
    {
        error_log("socket connect error:%d",_fd);
        return -3;
    }
    return 0;
}
int CNetHandlerTcp::Send(char* pBuf,int bufLen)
{
    int ret = send(m_SocketFd,pBuf,bufLen,0);
    return ret;
}
int CNetHandlerTcp::Recv(char* pBuf,int bufSize)
{
    int ret = recv(m_SocketFd, pBuf, bufSize- 1, 0);
    return ret;
}
int CNetHandlerTcp::netConnect()
{
    struct sockaddr serveraddr;
    struct sockaddr_in *p = (struct sockaddr_in *) &serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    p->sin_family = AF_INET;
    unsigned * ptr = (unsigned*)&p->sin_addr;
    *ptr = inet_addr(m_IP.c_str());
    p->sin_port = htons(m_Port);
    int ret = connect(m_SocketFd, &serveraddr, sizeof(serveraddr));
    if (ret < 0)
    {
        if(errno != EINPROGRESS)
        {
            close(m_SocketFd);
            error_log("socket connect error,ret:%d,error:%s,errno:%d",ret,strerror(errno),errno);
            return -1;
        }
    }
    return 0;
}
//=============================================================================
int CNetHandlerUdp::Create()
{
    if (m_SocketFd>0)
    {
        return 0;
    }
    int _fd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( _fd <= 0 )
    {
        error_log("socket create error:%d",_fd);
        return -1;
    }
    m_SocketFd = _fd;
    SetNoBlock(m_SocketFd);
    return 0;
}
int CNetHandlerUdp::Send(char* pBuf,int bufLen)
{
    struct  sockaddr_in _servaddr;
    int _tolen = sizeof (_servaddr);

    bzero(&_servaddr, sizeof(_servaddr));
    _servaddr.sin_family = AF_INET;
    _servaddr.sin_port = htons(m_Port);
    inet_pton(AF_INET, m_IP.c_str() , &_servaddr.sin_addr);
    int ret = sendto(m_SocketFd,pBuf,bufLen,MSG_DONTWAIT,(const struct sockaddr*)&_servaddr,_tolen);
    return ret;
}

int CNetHandlerUdp::Recv(char* pBuf,int bufSize)
{
    struct  sockaddr_in _servaddr;
    int _tolen = sizeof(struct sockaddr_in);

    int ret = recvfrom (m_SocketFd, pBuf, bufSize, MSG_DONTWAIT, (struct sockaddr*)&_servaddr,(socklen_t *) &_tolen);
    m_ClientIp = inet_ntoa(_servaddr.sin_addr);
    m_ClientPort = ntohs(_servaddr.sin_port);
    return ret;
}
