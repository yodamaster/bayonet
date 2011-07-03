/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        action_info.h
#  Description:     组合的方式封装了socket的细节
#  Version:         1.0
#  LastChange:      2011-01-28 15:43:20
#  History:         
=============================================================================*/
#ifndef _ACTION_INFO_H_20110630001523_
#define _ACTION_INFO_H_20110630001523_

#include "comm_def.h"
#include "fl_log.h"
#include "fsm_achieve.h"
#include "action.h"

class IAction;

typedef struct _StActionInfoParam
{
    int id;             //用来做标识
    string ip;          //ip
    int port;           //端口
    int protoType;      //协议类型
    int timeout_ms;     //超时时间
    IAction* pAction;   //最开始的Action

    ACTIONTYPE actionType;//发送接受类型
    _StActionInfoParam ()
    {
        id = 0;
        port = 0;
        protoType = PROTO_TYPE_TCP;
        pAction = NULL;
        actionType = ACTIONTYPE_SENDRECV;
    }
} StActionInfoParam;

class CActionInfo:public IPtrProxy<CActionInfo>
{
public:
    CActionInfo ();

    /**
     * @brief   初始化
     *
     * @param   param
     *
     * @return  0
     */
    int Init(StActionInfoParam param);

    /**
     * @brief   获取唯一标识ID
     *
     * @return  id
     */
    int GetID();

    /**
     * @brief   socket处理完成时标记结束
     *
     * @param   err_no                  错误码
     * @param   timecost_ms             消耗时间
     * @param   bProc                   appactor是否要ProcessState
     */
    void SetDealOver(int err_no,int timecost_ms,bool bProc=true);

    bool IsDealOver();

    /**
     * 获取错误码，插件在处理动作执行结果时可以调用此方法获取错误码，0为处理成功。
     * 具体错误码含义如下： 
     *         enum ErrorNo  {                               
     *             EConnect    = -1,      // 连接失败
     *             ESend       = -2,      // send失败 
     *             ERecv       = -3,      // recv失败
     *             ECheckPkg   = -4,      // 包检查失败
     *             ETimeout    = -5,      // 超时
     *             ECreateSock = -6,      // 创建socket失败
     *             EAttachPoller = -7,    // AttachPoller失败
     *             EInvalidState = -8,    // 非法状态
     *             EHangup       = -9,    // Hangup事件发生
     *             EShutdown     = -10,   // 对端关闭连接
     *             EEncodeFail   = -11,   // 编码错误 
     *             EInvalidRouteType = -12,// 非法路由配置类型
     *             EAppActorTimeout       = -13,// AppActor处理超时
     *        };
     *
     * @param err_no 错误码
     */
    int GetErrno();

    /**
     * 获取动作处理时间消耗，单位：ms
     *
     * @param cost
     */
    int GetTimeCost();

    /**
     * @brief   设置AppActor指针
     *
     * @param   pActor
     */
    void SetAppActor(CActorBase* pActor);

    /**
     * 以下方法，插件禁止调用
     */
    int HandleStart();

protected:
    StActionInfoParam m_stActionTypeParam;
    ptr_proxy<CActorBase> m_pAppActorProxy;

    int m_err_no;
    int m_timecost_ms;

    ptr_proxy<CActorBase> m_pSocketActorActiveProxy;

    bool m_bDealOver;
};
#endif
