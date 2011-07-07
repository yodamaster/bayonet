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
#include "tinyxml.h"

#define CONFVALUE2PARAM_STR(item_node, config_node, key_name)\
        item_node = config_node->FirstChildElement(#key_name); \
        if (item_node && item_node->GetText()) \
        {\
            fprintf(stdout,"init from config.[%s]:[%s]\n", #key_name, item_node->GetText());\
            param.key_name = item_node->GetText();\
        }

#define CONFVALUE2PARAM_INT(item_node, config_node, key_name)\
        item_node = config_node->FirstChildElement(#key_name); \
        if (item_node && item_node->GetText()) \
        {\
            fprintf(stdout,"init from config.[%s]:[%s]\n", #key_name, item_node->GetText());\
            param.key_name = atoi(item_node->GetText());\
        }

CBayonetFrame::CBayonetFrame()
{
    m_pSocketActorListen = NULL;
    RegDefaultSocketFsms();
    RegDefaultAppFsms();
}
CBayonetFrame::~CBayonetFrame(){}

int CBayonetFrame::Init(const StFrameParam& param)
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

    log_init((LogLevel)m_StFrameParam.iLogLevel,logDir.c_str(),m_StFrameParam.logFileName.c_str(),m_StFrameParam.iLogMaxSize);

    ret = CFrameBase::Init((statDir+param.statFileName).c_str(), param.statLevel, param.timeAccuracy);
    if (ret != 0)
    {
        error_log("CFrameBase init fail,ret:%d",ret);
        return -1;
    }

    m_epoller.SetFrame(this);
    return 0;
}
int CBayonetFrame::Init(const char* conf_path, IAction* pAction)
{
    int ret;
    StFrameParam param;
    ret = ParseConf(conf_path, param);
    if (ret)
    {
        fprintf(stderr,"ParseConf fail.ret:%d\n",ret);
        return -1;
    }
    param.pAction = pAction;

    return Init(param);
}

CEPoller* CBayonetFrame::GetEpoller()
{
    return &m_epoller;
}
int CBayonetFrame::Process()
{
    int ret = 0;

    CSocketActorListenTcp* pSocketActorListenTcp = NULL;
    CSocketActorListenUdp* pSocketActorListenUdp = NULL;
    switch(m_StFrameParam.protoType)
    {
        case PROTO_TYPE_TCP:
            pSocketActorListenTcp = new CSocketActorListenTcp();
            pSocketActorListenTcp->AttachFrame(this);
            ret = pSocketActorListenTcp->Init(m_StFrameParam.ip,m_StFrameParam.port,m_StFrameParam.timeOutMs,m_StFrameParam.protoType);
            if (ret)
            {
                error_log("pSocketActorListenTcp init fail:%d",ret);
                return ret;
            }
            pSocketActorListenTcp->SetBackLog(m_StFrameParam.backlog);
            pSocketActorListenTcp->SetAttachedSocketMaxSize(m_StFrameParam.attachedSocketMaxSize);
            pSocketActorListenTcp->SetKeepcnt(m_StFrameParam.bKeepcnt);
            pSocketActorListenTcp->SetIActionPtr(m_StFrameParam.pAction);
            m_pSocketActorListen = pSocketActorListenTcp;
            //pSocketActorListenTcp->ChangeState(SOCKET_FSM_INIT);
            break;
        case PROTO_TYPE_UDP:
            pSocketActorListenUdp = new CSocketActorListenUdp();
            pSocketActorListenUdp->AttachFrame(this);
            ret = pSocketActorListenUdp->Init(m_StFrameParam.ip,m_StFrameParam.port,m_StFrameParam.timeOutMs,m_StFrameParam.protoType);
            if (ret)
            {
                error_log("pSocketActorListenUdp init fail:%d",ret);
                return ret;
            }
            pSocketActorListenUdp->SetAttachedSocketMaxSize(m_StFrameParam.attachedSocketMaxSize);
            pSocketActorListenUdp->SetIActionPtr(m_StFrameParam.pAction);
            m_pSocketActorListen = pSocketActorListenUdp;
            //pSocketActorListenUdp->ChangeState(SOCKET_FSM_INIT);
            break;
        default:
            return -1;
    }

    pid_t pid;
    for(int i=0;i<m_StFrameParam.workerNum;++i)
    {
        pid = ForkWork();
        if (pid <= 0)//报错，或者是子进程
        {
            exit(-1);
        }
    }

    for(;;)
    {
        //会阻塞在这里，等待有子进程退出
        pid = waitpid(-1,NULL,0);
        if ( pid < 0 )
        {
            sleep(1);
            continue;
        }
        pid = ForkWork();
        if (pid == 0)//子进程自己结束了
        {
            exit(-2);
        }
    }


    return 0;
}


int CBayonetFrame::ParseConf(const char* conf_path, StFrameParam& param)
{
    if (conf_path == NULL)
    {
        fprintf(stderr, "ParseConf fail.conf_path is NULL\n");
        return -1;
    }

    TiXmlDocument* doc = new TiXmlDocument(conf_path);
    //将会在函数结束时，自动释放
    std::auto_ptr<TiXmlDocument> autoDeldoc(doc);
    doc->LoadFile();

    TiXmlElement* root_node = doc->RootElement();

    TiXmlElement* config_node = NULL;
    TiXmlElement* item_node = NULL;

    config_node = root_node->FirstChildElement("server");
    if (config_node)
    {
        CONFVALUE2PARAM_STR(item_node, config_node, ip);
        CONFVALUE2PARAM_INT(item_node, config_node, port);
        CONFVALUE2PARAM_INT(item_node, config_node, protoType);
        CONFVALUE2PARAM_INT(item_node, config_node, bKeepcnt);
        CONFVALUE2PARAM_INT(item_node, config_node, backlog);
        CONFVALUE2PARAM_INT(item_node, config_node, timeOutMs);
        CONFVALUE2PARAM_INT(item_node, config_node, attachedSocketMaxSize);
    }

    config_node = root_node->FirstChildElement("comm");
    if (config_node)
    {
        CONFVALUE2PARAM_INT(item_node, config_node, workerNum);
        CONFVALUE2PARAM_STR(item_node, config_node, infoDir);
        CONFVALUE2PARAM_INT(item_node, config_node, timeAccuracy);
    }

    config_node = root_node->FirstChildElement("epoll");
    if (config_node)
    {
        CONFVALUE2PARAM_INT(item_node, config_node, epollSize);
        CONFVALUE2PARAM_INT(item_node, config_node, epollWaitTimeMs);
    }

    config_node = root_node->FirstChildElement("timeout_check");
    if (config_node)
    {
        CONFVALUE2PARAM_INT(item_node, config_node, checkSockIntervalTimeMs);
        CONFVALUE2PARAM_INT(item_node, config_node, checkAppIntervalTimeMs);
    }

    config_node = root_node->FirstChildElement("gc");
    if (config_node)
    {
        CONFVALUE2PARAM_INT(item_node, config_node, gcMaxCount);
    }

    config_node = root_node->FirstChildElement("log");
    if (config_node)
    {
        CONFVALUE2PARAM_INT(item_node, config_node, iLogLevel);
        CONFVALUE2PARAM_STR(item_node, config_node, logFileName);
        CONFVALUE2PARAM_INT(item_node, config_node, iLogMaxSize);
    }

    config_node = root_node->FirstChildElement("stat");
    if (config_node)
    {
        CONFVALUE2PARAM_STR(item_node, config_node, statFileName);
        CONFVALUE2PARAM_INT(item_node, config_node, statLevel);
    }

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
int CBayonetFrame::ChildWork()
{
    int ret;
    //epoll的fd和select一样，不能被fork
    ret = m_epoller.Init(m_StFrameParam.epollSize,
                         m_StFrameParam.epollWaitTimeMs,
                         m_StFrameParam.checkSockIntervalTimeMs,
                         m_StFrameParam.checkAppIntervalTimeMs,
                         m_StFrameParam.gcMaxCount);
    if (ret != 0)
    {
        error_log("epoller init fail:%d",ret);
        return -2;
    }

    //socket转化状态
    if (m_pSocketActorListen)
    {
        m_pSocketActorListen->ChangeState(SOCKET_FSM_INIT);
    }
    else
    {
        error_log("m_pSocketActorListen is NULL");
        return -1;
    }

    ret = m_epoller.LoopForEvent();
    if (ret != 0)
    {
        error_log("epoller LoopForEvent fail:%d",ret);
        return -2;
    }
    return 0;
}
int CBayonetFrame::ForkWork()
{
    pid_t pid=0;

    pid=fork();

    if(pid==-1)//err
    {
        error_log("fork error");
    }
    else if(pid==0)//child
    {
        //执行
        trace_log("i am child");
        int ret = ChildWork();
        if (ret != 0)
        {
            error_log("child error, ret: %d",ret);
        }
    }
    else
    {
        error_log("i am farther,child is %d",pid);
    }
    return pid;
}
