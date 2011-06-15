/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_data.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-28 01:15:18
#  History:         
=============================================================================*/
#include "socketactor_data.h"
#include "bayonet_frame.h"
CSocketActorData::CSocketActorData () {
    m_pNetHandler = NULL;
    //m_strSingleRecvBuf.resize(1024);
    //m_strRecvBuf.resize(2048);
    ResetStatusData();
}
CSocketActorData::~CSocketActorData () {
    if (m_pNetHandler)
    {
        delete m_pNetHandler;
        m_pNetHandler = NULL;
    }
}

int CSocketActorData::Init(string ip,int port,int timeout_ms,int protoType)
{
    int ret = CSocketActorBase::Init(ip,port,timeout_ms,protoType);
    if (ret)
    {
        return -1;
    }
    switch(m_ProtoType)
    {
        case PROTO_TYPE_TCP:
            m_pNetHandler = new CNetHandlerTcp();
            break;
        case PROTO_TYPE_UDP:
            m_pNetHandler = new CNetHandlerUdp();
            break;
        default:
            return -2;
    }
    ret = m_pNetHandler->Init(m_IP,m_Port);
    if (ret)
    {
        return -3;
    }
    return 0;
}

int CSocketActorData::Init(int socketFd,int timeout_ms,int protoType)
{
    int ret = CSocketActorBase::Init(socketFd,timeout_ms,protoType);
    if (ret)
    {
        return -1;
    }
    switch(m_ProtoType)
    {
        case PROTO_TYPE_TCP:
            m_pNetHandler = new CNetHandlerTcp();
            break;
        case PROTO_TYPE_UDP:
            m_pNetHandler = new CNetHandlerUdp();
            break;
        default:
            return -2;
    }
    ret = m_pNetHandler->Init(m_SocketFd);
    if (ret)
    {
        return -3;
    }
    return 0;
}
int CSocketActorData::OnInit()
{
    int ret = m_pNetHandler->Create();
    if (ret)
    {
        return SOCKET_FSM_CLOSING;
    }
    else
    {
        m_SocketFd = m_pNetHandler->GetSocketFd();
    }
    CEPoller* pEpoller = GetEpoller();
    if (!pEpoller)
    {
        error_log("[class:%s]pEpoller is NULL",Name().c_str());
        return SOCKET_FSM_CLOSING;
    }
    pEpoller->AttachSocket(this);//加入到epoll中

    ret = ActionHandleInit();
    if (ret != 0)
    {
        error_log("m_pAction HandleInit fail,ret:%d",ret);
        return SOCKET_FSM_CLOSING;
    }
    return SOCKET_FSM_INITOVER;
}
int CSocketActorData::OnRecv()
{
    int ret = 0;

    if (m_recvFlag == 0)
    {
        //万一用户没有调用ResizeRecvBuf，那么用默认值
        if (m_strSingleRecvBuf.size()==0)
        {
            m_strSingleRecvBuf.resize(RECV_SINGLE_BUFSIZE);
        }
        if (m_strRecvBuf.size()==0)
        {
            m_strSingleRecvBuf.resize(RECV_SINGLE_BUFSIZE);
        }
        m_recvFlag = 1;
    }
    for(;;)
    {
        ret = m_pNetHandler->Recv((char*)m_strSingleRecvBuf.c_str(),m_strSingleRecvBuf.size());
        if (ret == 0)
        {
            return SOCKET_FSM_CLOSING;
        }
        else if (ret < 0)
        {
            if ( errno == EINTR || errno == EAGAIN )
            {
                return SOCKET_FSM_WAITRECV;
            }
            return SOCKET_FSM_CLOSING;
        }
        else
        {
            if ((ret + m_recvedLen) > (int)m_strRecvBuf.size())
            {
                m_strRecvBuf.resize(m_recvedLen*2 + ret);
            }
            memcpy((char*)(m_strRecvBuf.c_str()+m_recvedLen),m_strSingleRecvBuf.c_str(),ret);
            m_recvedLen += ret;
        }
        ret = ActionHandleInput(m_strRecvBuf.c_str(), m_recvedLen);
        if (ret == 0)
        {
            continue;
        }
        else if (ret < 0)
        {
            return SOCKET_FSM_CLOSING;
        }
        else
        {
            //数据完整了
            m_strRecvBuf.resize(ret);
            m_recvedLen = ret;
            break;
        }
    }
    ret = ActionHandleDecodeRecvBuf(m_strRecvBuf.c_str(), m_recvedLen);

    if (ret)
    {
        return SOCKET_FSM_CLOSING;
    }

    //Mod-Begin by dantezhu in 2011-06-12 18:58:48
    //FROM
    /*return SOCKET_FSM_RECVOVER;*/
    //TO
    //这里的主要是考虑，appactor可能直接就返回
    if (!m_vecFsmNodes.empty() && m_vecFsmNodes.back().fsm->GetStateID() == SOCKET_FSM_RECVING)
    {
        return SOCKET_FSM_RECVOVER;
    }
    else
    {
        return 0;
    }
    //Mod-End
}
int CSocketActorData::OnSend()
{
    int ret = 0;
    if (m_sendFlag == 0)
    {
        m_sendFlag = 1;
        ret = ActionHandleEncodeSendBuf(m_strSendBuf,m_sendBufLen);
        if (ret)
        {
            error_log("[class:%s]HandleEncodeSendBuf error :%d",Name().c_str(), ret);
            return SOCKET_FSM_CLOSING;
        }
        if ((int)m_strSendBuf.size() < m_sendBufLen)
        {
            error_log("[class:%s]HandleEncodeSendBuf len error:%d/%d",Name().c_str(),m_strSendBuf.size(),m_sendBufLen);
            return SOCKET_FSM_CLOSING;
        }
    }
    while (m_sendedLen != m_sendBufLen)
    {
        ret = m_pNetHandler->Send((char*)(m_strSendBuf.c_str()+m_sendedLen),m_sendBufLen-m_sendedLen);
        if ( ret == 0 )
        {
            return SOCKET_FSM_WAITSEND;
        }
        else if ( ret < 0 )
        {
            if ( errno == EINTR || errno == EAGAIN )
            {
                return SOCKET_FSM_WAITSEND;
            }
            return SOCKET_FSM_CLOSING;
        }
        else
        {
            m_sendedLen += ret;
        }
    }
    return SOCKET_FSM_SENDOVER;
}
int CSocketActorData::OnClose()
{
    DetachFromEpoller();
    if (m_pNetHandler)
    {
        m_pNetHandler->Close();
        m_SocketFd = m_pNetHandler->GetSocketFd();
    }
    return SOCKET_FSM_CLOSEOVER;
}
int CSocketActorData::OnCloseOver()
{
    return SOCKET_FSM_FINI;
}
int CSocketActorData::ResetStatusData()
{
    m_sendedLen = 0;
    m_sendBufLen = 0;

    m_recvedLen = 0;

    m_sendFlag = 0;
    m_recvFlag = 0;

    return 0;
}
int CSocketActorData::ResizeRecvBuf(int singleBufSize, int initBufSize)
{
    if (singleBufSize>0)
    {
        m_strSingleRecvBuf.resize(singleBufSize);
    }
    if (initBufSize > 0 && initBufSize > m_recvedLen)
    {
        m_strRecvBuf.resize(initBufSize);
    }
    return 0;   
}
//=============================================================================
