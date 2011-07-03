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
#ifndef _ACTION_H_20110630001511_
#define _ACTION_H_20110630001511_

#include "socketactor_data.h"
#include "appactor_base.h"

class CSocketActorData;
class CAppActorBase;

class IAction
{
public:
    virtual ~IAction () {}

    /**
     * @brief   初始化-比如可以设置socket的接受缓存区大小，初始化recvbuf的大小，设置keepcnt，设置超时等
     *
     * @param   pSocketActor            pSocketActor
     * @param   pAppActor               pAppActor
     *
     * @return  0                       succ
     *          else                    fail
     */
    virtual int HandleInit(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor) {return 0;}

    /**
     * @brief   打包要发送的数据
     *
     * @param   pSocketActor            pSocketActor
     * @param   pAppActor               pAppActor
     * @param   strSendBuf              设置要发送的数据，可以为字符串也可以为二进制;如果为二进制，用assign即可
     * @param   len                     strSendBuf本身的size()是无效的，必须手工设置len
     *
     * @return  0                       succ
     *          else                    fail
     */
    virtual int HandleEncodeSendBuf(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor,
            string & strSendBuf,
            int &len)=0;

    /**
     * @brief   接收包完整性检查
     *
     * @param   pSocketActor            pSocketActor
     * @param   pAppActor               pAppActor
     * @param   buf                     接收到的buf指针
     * @param   len                     接收到的buf长度
     *
     * @return  0                       数据还没有接收完，继续接收
     *          >0                      数据已经接收完毕，有效的数据包长度
     *          <0                      数据有问题，停止接收，并断掉链接
     */
    virtual int HandleInput(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor,
            const char *buf,
            int len)=0;

    /**
     * @brief   解包接收到的数据。如果是第一个action，pAppActor会为空，要负责创建CAppActorBase对象；如果是其他，则负责给pAppActor中的字段赋值，并切换到下一状态
     *
     * @param   pSocketActor            pSocketActor
     * @param   pAppActor               pAppActor
     * @param   buf                     收到的buf指针
     * @param   len                     收到的buf长度
     *
     * @return  0                       succ
     *          else                    fail
     *
     * @example
     *          示例1
     *
     *          CAppActorBase * app_actor = new CAppActorBase();
     *          app_actor->AttachFrame(pSocketActor->GetFrame());
     *          app_actor->AttachCommu(pSocketActor);
     *          app_actor->ChangeState(APP_FSM_LOGIC1);
     *          return 0;
     *
     *
     *          示例2
     *
     *          CMyActor* app_actor = (CMyActor*)pAppActor;
     *          app_actor->m_str+=string(buf);
     *          return 0;
     */
    virtual int HandleDecodeRecvBuf(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor,
            const char *buf, 
            int len)=0;
};
#endif
