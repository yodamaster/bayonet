/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        bayonet_frame.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-26 17:27:51
#  History:         
=============================================================================*/
#include "bayonet_frame.h"
#include "socketactor_listen_tcp.h"
#include "socketactor_listen_udp.h"
CBayonetFrame::CBayonetFrame()
{
    RegDefaultSocketFsms();
    RegDefaultAppFsms();
}
CBayonetFrame::~CBayonetFrame(){}

int CBayonetFrame::Init(StFrameParam param)
{
    m_StFrameParam = param;

    int ret;

    string statDir = param.infoDir+string("/stat/");
    string logDir = param.infoDir + string("/log/");

    ret = mkdir(statDir.c_str(),0777);
    if (ret != 0 && errno != EEXIST)
    {
        fprintf(stderr,"mkdir %s fail\n", statDir.c_str());
        return -1;
    }
    ret = mkdir(logDir.c_str(),0777);
    if (ret != 0 && errno != EEXIST)
    {
        fprintf(stderr,"mkdir %s fail\n", logDir.c_str());
        return -1;
    }

    log_init(m_StFrameParam.iLogLevel,logDir.c_str(),m_StFrameParam.logFileName.c_str(),m_StFrameParam.iLogMaxSize);

    ret = CFrameBase::Init(statDir.c_str(), param.statFileName.c_str());
    if (ret != 0)
    {
        error_log("CFrameBase init fail,ret:%d",ret);
        return -1;
    }

    m_epoller.Init(m_StFrameParam.epollSize,m_StFrameParam.epollWaitTimeMs,m_StFrameParam.epollCheckTimeMs,m_StFrameParam.gcMaxCount);
    m_epoller.SetFrame(this);
    return 0;
}

CEPoller* CBayonetFrame::GetEpoller()
{
    return &m_epoller;
}
int CBayonetFrame::Process()
{
    CSocketActorListenTcp* pSocketActorListenTcp = NULL;
    CSocketActorListenUdp* pSocketActorListenUdp = NULL;
    switch(m_StFrameParam.protoType)
    {
        case PROTO_TYPE_TCP:
            pSocketActorListenTcp = new CSocketActorListenTcp();
            pSocketActorListenTcp->AttachFrame(this);
            pSocketActorListenTcp->Init(m_StFrameParam.ip,m_StFrameParam.port,m_StFrameParam.timeOutMs,m_StFrameParam.protoType);
            pSocketActorListenTcp->SetBackLog(m_StFrameParam.backlog);
            pSocketActorListenTcp->SetKeepcnt(m_StFrameParam.bKeepcnt);
            pSocketActorListenTcp->SetIActionPtr(m_StFrameParam.pAction);
            pSocketActorListenTcp->ChangeState(SOCKET_FSM_INIT);
            break;
        case PROTO_TYPE_UDP:
            pSocketActorListenUdp = new CSocketActorListenUdp();
            pSocketActorListenUdp->AttachFrame(this);
            pSocketActorListenUdp->Init(m_StFrameParam.ip,m_StFrameParam.port,m_StFrameParam.timeOutMs,m_StFrameParam.protoType);
            pSocketActorListenUdp->SetIActionPtr(m_StFrameParam.pAction);
            pSocketActorListenUdp->ChangeState(SOCKET_FSM_INIT);
            break;
        default:
            return -1;
    }

    m_epoller.LoopForEvent();

    return 0;
}
void CBayonetFrame::RegDefaultAppFsms()
{
    RegFsm(APP_FSM_RSP,new CAppFsmRsp());
    RegFsm(APP_FSM_FINI,new CAppFsmFini());
}
void CBayonetFrame::RegDefaultSocketFsms()
{
    RegFsm(SOCKET_FSM_INIT, new CSocketFsmInit());
    RegFsm(SOCKET_FSM_INITOVER, new CSocketFsmInitOver());
    RegFsm(SOCKET_FSM_FINI, new CSocketFsmFini());
    RegFsm(SOCKET_FSM_FINIOVER, new CSocketFsmFiniOver());
    RegFsm(SOCKET_FSM_WAITSEND, new CSocketFsmWaitSend());
    RegFsm(SOCKET_FSM_SENDING, new CSocketFsmSending());
    RegFsm(SOCKET_FSM_SENDOVER, new CSocketFsmSendOver());
    RegFsm(SOCKET_FSM_WAITRECV, new CSocketFsmWaitRecv());
    RegFsm(SOCKET_FSM_RECVING, new CSocketFsmRecving());
    RegFsm(SOCKET_FSM_RECVOVER, new CSocketFsmRecvOver());
    RegFsm(SOCKET_FSM_WAITCLOSE, new CSocketFsmWaitClose());
    RegFsm(SOCKET_FSM_CLOSING, new CSocketFsmClosing());
    RegFsm(SOCKET_FSM_CLOSEOVER, new CSocketFsmCloseOver());
    RegFsm(SOCKET_FSM_ERROR, new CSocketFsmError());
    RegFsm(SOCKET_FSM_TIMEOUT, new CSocketFsmTimeout());
}
