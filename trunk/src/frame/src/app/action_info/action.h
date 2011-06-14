/*=============================================================================
#
#     FileName: action.h
#         Desc: IAction定义
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-15 02:02:39
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-15 02:02:39 | initialization
#
=============================================================================*/
#ifndef __ACTION_H__
#define __ACTION_H__
#include "socketactor_base.h"
#include "appactor_base.h"

class CSocketActorBase;
class CAppActorBase;

class IAction
{
public:
    virtual ~IAction () {}
    // 初始化-比如可以设置socketbuf，可选实现
    virtual int HandleInit(
            CSocketActorBase* pSocketActor,
            CAppActorBase* pAppActor) {return 0;}

    // 为发送打包
    virtual int HandleEncodeSendBuf(
            CSocketActorBase* pSocketActor,
            CAppActorBase* pAppActor,
            string & strSendBuf,
            int &len)=0;

    // 回应包完整性检查
    virtual int HandleInput(
            CSocketActorBase* pSocketActor,
            CAppActorBase* pAppActor,
            const char *buf,
            int len)=0;

    // 回应包解析
    virtual int HandleDecodeRecvBuf(
            CSocketActorBase* pSocketActor,
            CAppActorBase* pAppActor,
            const char *buf, 
            int len)=0;
};
#endif
