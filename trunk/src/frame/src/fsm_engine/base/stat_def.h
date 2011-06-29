/*=============================================================================
#  Author:          DanteZhu - http://www.vimer.cn
#  Email:           dantezhu@vip.qq.com
#  FileName:        stat_def.h
#  Description:     统计码定义
#  Version:         1.0
#  LastChange:      2010-10-08 15:42:38
#  History:
=============================================================================*/
#ifndef __STAT_DEF_H_20110630001853__
#define __STAT_DEF_H_20110630001853__

typedef enum
{
    STAT_TOTAL = 0,
    STAT_ALIVE,

    STAT_5MS_REQ,
    STAT_10MS_REQ,
    STAT_50MS_REQ,
    STAT_100MS_REQ,
    STAT_200MS_REQ,
    STAT_500MS_REQ,
    STAT_1000MS_REQ,
    STAT_2000MS_REQ,
    STAT_3000MS_REQ,
    STAT_5000MS_REQ,
    STAT_MORE_REQ,

    STAT_OVER
}STAT_ID;

const char * const stat_desc[] =
{
    "STAT_TOTAL",
    "STAT_ALIVE",

    "STAT_5MS_REQ",
    "STAT_10MS_REQ",
    "STAT_50MS_REQ",
    "STAT_100MS_REQ",
    "STAT_200MS_REQ",
    "STAT_500MS_REQ",
    "STAT_1000MS_REQ",
    "STAT_2000MS_REQ",
    "STAT_3000MS_REQ",
    "STAT_5000MS_REQ",
    "STAT_MORE_REQ",
};
#endif
