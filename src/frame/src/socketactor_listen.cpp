/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_listen.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-25 17:04:07
#  History:         
=============================================================================*/
#include "socketactor_listen.h"
int CSocketActorListen::OnInit()
{
    if (m_SocketFd <= 0)
    {
        int optval;
        int listen_fd = socket(AF_INET,SOCK_STREAM,0);
        if(listen_fd < 0)
        {   
            error_log("Create socket error:%s\n",strerror(errno));
            return -1; 
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
            return -1; 
        }
        m_SocketFd = listen_fd;
    }

    return HandleFini();
}
int CSocketActorListen::OnRecv()
{
    return SOCKET_FSM_RECVOVER;
}
int CSocketActorListen::OnSend()
{
    return SOCKET_FSM_WAITRECV;
}
