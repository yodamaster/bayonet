/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_listen_udp.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-28 00:48:05
#  History:         
=============================================================================*/
#include "socketactor_listen_udp.h"

int CSocketActorListenUdp::OnInit()
{
    if (m_SocketFd <= 0)
    {
        int optval;
        int listen_fd;
        listen_fd = socket(AF_INET,SOCK_DGRAM,0);
        if(listen_fd < 0)
        {   
            error_log("Create socket error:%s\n",strerror(errno));
            return SOCKET_FSM_FINI; 
        }   
        struct sockaddr_in myaddr;
        myaddr.sin_family = AF_INET;
        myaddr.sin_port = htons(m_Port);
        myaddr.sin_addr.s_addr = inet_addr(m_IP.c_str());

        optval = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        if(bind(listen_fd,(struct sockaddr*)&myaddr,sizeof(struct sockaddr)) < 0)
        {   
            close(listen_fd);
            error_log("CreateListen bind ip:%s port:%d sock:%d err:%s\n",
                    m_IP.c_str(),m_Port,listen_fd,strerror(errno));
            return SOCKET_FSM_FINI; 
        }
        m_SocketFd = listen_fd;
        int ret = m_pNetHandler->Init(m_SocketFd);
        if (ret)
        {
            error_log("nethandler init error:%d",ret);
            return SOCKET_FSM_CLOSING; 
        }
    }
    CEPoller* pEpoller = GetEpoller();
    if (!pEpoller)
    {
        error_log("pEpoller is NULL");
        return SOCKET_FSM_CLOSING;
    }
    pEpoller->AttachSocket(this);//加入到epoll中
    trace_log("%s over",__func__);

    return OnRecvOver();
}