/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        action_info.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-28 18:33:54
#  History:         
=============================================================================*/
#include "action_info.h"
int CActionInfo::SetIActionPtr(IAction *pAction)
{
    m_pAction = pAction;
    return 0;
}
int CActionInfo::SetAppActor(IActor* pActor)
{
    m_pAppActor = pActor;
    return 0;
}
