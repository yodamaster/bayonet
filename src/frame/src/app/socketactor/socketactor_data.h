/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_data.h
#  Description:     socketactor传输数据相关的actor
#  Version:         1.0
#  LastChange:      2011-01-27 23:24:32
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_DATA_H_
#define _SOCKETACTOR_DATA_H_
#include "comm_def.h"
#include "fsm_achieve.h"
#include "epoller.h"
#include "fl_log.h"
#include "net_handler.h"
#include "socketactor_base.h"
//=============================================================================
class CSocketActorData:public CSocketActorBase
{
public:
    CSocketActorData () {
        m_pNetHandler = NULL;
        //m_strSingleRecvBuf.resize(1024);
        //m_strRecvBuf.resize(2048);
        ResetStatusData();
    }
    virtual ~CSocketActorData () {
        if (m_pNetHandler)
        {
            delete m_pNetHandler;
            m_pNetHandler = NULL;
        }
    }

    virtual int Init(string ip,int port,int timeout_ms,int protoType);
    virtual int Init(int socketFd,int timeout_ms,int protoType);

    virtual int OnInit();
    virtual int OnInitOver()=0;
    virtual int OnFini();
    virtual int OnFiniOver();
    virtual int OnSend();
    virtual int OnSendOver()=0;
    virtual int OnRecv();
    virtual int OnRecvOver()=0;
    virtual int OnClose();
    virtual int OnCloseOver();

    virtual int ResizeRecvBuf(int singleBufSize, int initBufSize);

protected:
    //清理接收/发送标记位等
    virtual int ResetStatusData();

    // 为发送打包
    virtual int HandleEncodeSendBuf(
            string & strSendBuf,
            int &len);

    // 回应包完整性检查
    virtual int HandleInput(
            const char *buf,
            int len);

    // 回应包解析
    virtual int HandleDecodeRecvBuf(const char *buf, int len);

protected:
    CNetHandlerBase* m_pNetHandler;

    int m_sendedLen;
    int m_sendBufLen;
    string m_strSendBuf;

    int m_recvedLen;
    string m_strRecvBuf;

    string m_strSingleRecvBuf;

    int m_sendFlag;
    int m_recvFlag;
};
//=============================================================================
#endif
