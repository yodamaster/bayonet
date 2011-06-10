/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        timer.h
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-30 13:41:16
#  History:         
=============================================================================*/
#ifndef _TIMER_H_
#define _TIMER_H_
#include <sys/time.h>
class CTimer
{
public:
    CTimer () {}
    virtual ~CTimer () {}

    int Start()
    {
        gettimeofday(&m_Start_TV, NULL);
        return 0;
    }
    int GetPastTime()
    {
        struct timeval now_tv;
        long past_time  = 0;
        gettimeofday(&now_tv, NULL);
        past_time = ((now_tv.tv_sec  - m_Start_TV.tv_sec ) * 1000000 + (now_tv.tv_usec - m_Start_TV.tv_usec)) / 1000;
        return past_time;
    }
    int GetPastTime(struct timeval& now_tv)
    {
        long past_time  = 0;
        gettimeofday(&now_tv, NULL);
        past_time = ((now_tv.tv_sec  - m_Start_TV.tv_sec ) * 1000000 + (now_tv.tv_usec - m_Start_TV.tv_usec)) / 1000;
        return past_time;
    }
private:
    struct timeval m_Start_TV;
};
#endif
