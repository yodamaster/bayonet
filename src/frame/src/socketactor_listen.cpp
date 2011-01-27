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
void CSocketActorListen::SetBackLog(int backlog)
{
    m_BackLog = backlog;
}
int CSocketActorListen::GetBackLog()
{
    return m_BackLog;
}

int CSocketActorListen::OnInit()
{
    trace_log("%s",__func__);
    if (m_SocketFd <= 0)
    {
        int optval;
        int listen_fd;
        switch(m_ProtoType)
        {
            case PROTO_TYPE_TCP:
                listen_fd = socket(AF_INET,SOCK_STREAM,0);
                break;
            case PROTO_TYPE_UDP:
                //不支持udp
            default:
                return SOCKET_FSM_FINI;
        }
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
    }
    if(listen(m_SocketFd, m_BackLog)<0)
    {
        error_log("CreateListen listen fd:%d err:%s\n",
                m_SocketFd,strerror(errno));
        return SOCKET_FSM_CLOSING;
    }
    CEPoller* pEpoller = GetEpoller();
    if (!pEpoller)
    {
        error_log("pEpoller is NULL");
        return SOCKET_FSM_CLOSING;
    }
    pEpoller->AttachSocket(this);//加入到epoll中
    trace_log("%s over",__func__);

    return HandleInit();
}
int CSocketActorListen::OnRecv()
{
    trace_log("%s",__func__);
    struct sockaddr_in addr;
    int length = sizeof(struct sockaddr_in);
    int clientfd = accept(m_SocketFd,(struct sockaddr *)&addr,(socklen_t*)&length);
    if ( clientfd <= 0 )
    {
        error_log("netlisten accept rtn:%d error:%s\n",
                clientfd,strerror(errno));
        return SOCKET_FSM_FINI;
    }

    int flag = fcntl (clientfd, F_GETFL);
    if ( fcntl (clientfd, F_SETFL, O_NONBLOCK | flag) < 0 )
    {
        error_log("HandleConnect set noblock socket:%d error:%s\n",
                clientfd,strerror(errno));
        close(clientfd);
        return SOCKET_FSM_FINI;
    }

    CSocketActorBase* pSocketActorAccept = AllocSocketActorAccept();
    if (pSocketActorAccept == NULL)
    {
        error_log("AllocSocketActorAccept socket:%d error:%s\n",
                clientfd,strerror(errno));
        close(clientfd);
        return SOCKET_FSM_FINI;
    }
    pSocketActorAccept->Init(clientfd,m_TimeoutMs,m_ProtoType);
    pSocketActorAccept->AttachFrame(m_pFrame);
    pSocketActorAccept->ChangeState(SOCKET_FSM_INIT);
    trace_log("%s over",__func__);

    return SOCKET_FSM_RECVOVER;
}
int CSocketActorListen::OnSend()
{
    return SOCKET_FSM_SENDOVER;
}
