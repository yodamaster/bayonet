/*=============================================================================
#
#     FileName: fsm_base_def.h
#         Desc: 定义了fsm引擎的一些基本定义
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-29 23:56:52
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-29 23:56:52 | initialization
#
=============================================================================*/
#ifndef _FSM_BASE_DEF_H_20110629235706_
#define _FSM_BASE_DEF_H_20110629235706_
//统计等级，数字越大，性能越差
//测试过，能让性能降低一半
enum EnumStatLevel
{
    EnumStatLevelNone  = 0,             //所有的统计都关闭
    EnumStatLevelBrief = 1,             //统计所有的SELF和VALID
    EnumStatLevelFull  = 2,             //所有统计
};
#endif
