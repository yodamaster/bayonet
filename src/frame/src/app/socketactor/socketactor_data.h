/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_data.h
#  Description:     socketactor传输数据相关的actor
#  Version:         1.0
#  LastChange:      2011-01-27 23:24:32
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_DATA_H_20110630001730_
#define _SOCKETACTOR_DATA_H_20110630001730_

#include "comm_def.h"
#include "fsm_achieve.h"
#include "epoller.h"
#include "fl_log.h"
#include "net_handler.h"
#include "socketactor_base.h"
#include "action.h"
//=============================================================================
namespace bayonet {
class CSocketActorData:public CSocketActorBase
{
public:
    CSocketActorData ();
    virtual ~CSocketActorData ();

    virtual int Init(string ip,int port,int timeout_ms,int protoType);
    virtual int Init(int socketFd,int timeout_ms,int protoType);

    //初始化接收缓冲区大小，包括一次接收的大小，和总共的初始化大小;<=0代表不修改
    virtual int ResizeRecvBuf(int singleBufSize, int initBufSize);

    virtual int OnInit();
    virtual int OnInitOver()=0;
    virtual int OnSend();
    virtual int OnSendOver()=0;
    virtual int OnRecv();
    virtual int OnRecvOver()=0;
    virtual int OnClose();
    virtual int OnCloseOver();

protected:
    //清理接收/发送标记位等
    virtual int ResetStatusData();

    //初始化数据
    virtual int ActionHandleInit();

    // 为发送打包
    virtual int ActionHandleEncodeSendBuf(
            string & strSendBuf,
            int &len);

    // 收到包完整性检查,<0出错，=0继续接收，>0收到的长度
    virtual int ActionHandleInput(
            const char *buf,
            int len);

    // 收到包解析
    virtual int ActionHandleDecodeRecvBuf(const char *buf, int len);

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
}
//=============================================================================
#endif
