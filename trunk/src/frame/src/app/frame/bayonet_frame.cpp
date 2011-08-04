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
namespace bayonet {

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

    string statDir = param.info_dir+string("/stat/");
    string logDir = param.info_dir + string("/log/");

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

    byt_log_init((LogLevel)m_StFrameParam.log_level,logDir.c_str(),m_StFrameParam.log_filename.c_str(),m_StFrameParam.log_maxsize);

    ret = CFrameBase::Init((statDir+param.stat_filename).c_str(), param.stat_level, param.time_accuracy);
    if (ret != 0)
    {
        byt_error_log("CFrameBase init fail,ret:%d",ret);
        return -1;
    }

    m_epoller.SetFrame(this);
    return 0;
}
int CBayonetFrame::Init(const char* conf_path, IAction* action)
{
    int ret;
    StFrameParam param;
    ret = ParseConf(conf_path, param);
    if (ret)
    {
        fprintf(stderr,"ParseConf fail.ret:%d\n",ret);
        return -1;
    }
    param.action = action;

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
    switch(m_StFrameParam.proto_type)
    {
        case PROTO_TYPE_TCP:
            pSocketActorListenTcp = new CSocketActorListenTcp();
            pSocketActorListenTcp->AttachFrame(this);
            ret = pSocketActorListenTcp->Init(m_StFrameParam.ip,m_StFrameParam.port,m_StFrameParam.timeout_ms,m_StFrameParam.proto_type);
            if (ret)
            {
                byt_error_log("pSocketActorListenTcp init fail:%d",ret);
                return ret;
            }
            pSocketActorListenTcp->SetBackLog(m_StFrameParam.backlog);
            pSocketActorListenTcp->SetAttachedSocketMaxSize(m_StFrameParam.attached_socket_maxsize);
            pSocketActorListenTcp->SetKeepcnt(m_StFrameParam.keep_cnt);
            pSocketActorListenTcp->SetIActionPtr(m_StFrameParam.action);
            m_pSocketActorListen = pSocketActorListenTcp;
            //pSocketActorListenTcp->ChangeState(SOCKET_FSM_INIT);
            break;
        case PROTO_TYPE_UDP:
            pSocketActorListenUdp = new CSocketActorListenUdp();
            pSocketActorListenUdp->AttachFrame(this);
            ret = pSocketActorListenUdp->Init(m_StFrameParam.ip,m_StFrameParam.port,m_StFrameParam.timeout_ms,m_StFrameParam.proto_type);
            if (ret)
            {
                byt_error_log("pSocketActorListenUdp init fail:%d",ret);
                return ret;
            }
            pSocketActorListenUdp->SetAttachedSocketMaxSize(m_StFrameParam.attached_socket_maxsize);
            pSocketActorListenUdp->SetIActionPtr(m_StFrameParam.action);
            m_pSocketActorListen = pSocketActorListenUdp;
            //pSocketActorListenUdp->ChangeState(SOCKET_FSM_INIT);
            break;
        default:
            return -1;
    }

    pid_t pid;
    for(int i=0;i<m_StFrameParam.worker_num;++i)
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
        CONFVALUE2PARAM_INT(item_node, config_node, proto_type);
        CONFVALUE2PARAM_INT(item_node, config_node, keep_cnt);
        CONFVALUE2PARAM_INT(item_node, config_node, backlog);
        CONFVALUE2PARAM_INT(item_node, config_node, timeout_ms);
        CONFVALUE2PARAM_INT(item_node, config_node, attached_socket_maxsize);
    }

    config_node = root_node->FirstChildElement("comm");
    if (config_node)
    {
        CONFVALUE2PARAM_INT(item_node, config_node, worker_num);
        CONFVALUE2PARAM_STR(item_node, config_node, info_dir);
        CONFVALUE2PARAM_INT(item_node, config_node, time_accuracy);
    }

    config_node = root_node->FirstChildElement("epoll");
    if (config_node)
    {
        CONFVALUE2PARAM_INT(item_node, config_node, epoll_size);
        CONFVALUE2PARAM_INT(item_node, config_node, epoll_wait_time_ms);
    }

    config_node = root_node->FirstChildElement("timeout_check");
    if (config_node)
    {
        CONFVALUE2PARAM_INT(item_node, config_node, check_sock_interval_time_ms);
        CONFVALUE2PARAM_INT(item_node, config_node, check_app_interval_time_ms);
    }

    config_node = root_node->FirstChildElement("gc");
    if (config_node)
    {
        CONFVALUE2PARAM_INT(item_node, config_node, gc_maxcount);
    }

    config_node = root_node->FirstChildElement("log");
    if (config_node)
    {
        CONFVALUE2PARAM_INT(item_node, config_node, log_level);
        CONFVALUE2PARAM_STR(item_node, config_node, log_filename);
        CONFVALUE2PARAM_INT(item_node, config_node, log_maxsize);
    }

    config_node = root_node->FirstChildElement("stat");
    if (config_node)
    {
        CONFVALUE2PARAM_STR(item_node, config_node, stat_filename);
        CONFVALUE2PARAM_INT(item_node, config_node, stat_level);
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
    ret = m_epoller.Init(m_StFrameParam.epoll_size,
                         m_StFrameParam.epoll_wait_time_ms,
                         m_StFrameParam.check_sock_interval_time_ms,
                         m_StFrameParam.check_app_interval_time_ms,
                         m_StFrameParam.gc_maxcount);
    if (ret != 0)
    {
        byt_error_log("epoller init fail:%d",ret);
        return -2;
    }

    //socket转化状态
    if (m_pSocketActorListen)
    {
        m_pSocketActorListen->ChangeState(SOCKET_FSM_INIT);
    }
    else
    {
        byt_error_log("m_pSocketActorListen is NULL");
        return -1;
    }

    ret = m_epoller.LoopForEvent();
    if (ret != 0)
    {
        byt_error_log("epoller LoopForEvent fail:%d",ret);
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
        byt_error_log("fork error");
    }
    else if(pid==0)//child
    {
        //执行
        byt_trace_log("i am child");
        int ret = ChildWork();
        if (ret != 0)
        {
            byt_error_log("child error, ret: %d",ret);
        }
    }
    else
    {
        byt_error_log("i am farther,child is %d",pid);
    }
    return pid;
}
}
