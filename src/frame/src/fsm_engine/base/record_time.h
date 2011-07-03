/*=============================================================================
#
#     FileName: record_time.h
#         Desc: 用来减少gettimeofday的频率，在一次循环里面获取一次足够了
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-17 10:35:04
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-17 10:35:04 | initialization
#
=============================================================================*/
#ifndef _RECORD_TIME_H_20110630001843_
#define _RECORD_TIME_H_20110630001843_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>

#include <sys/time.h>
using namespace std;
class CRecordTime
{
public:
    static CRecordTime * Ins()
    {
        static CRecordTime * _ins = NULL;
        if ( _ins == NULL)
            _ins = new CRecordTime();
        return _ins;
    }

    struct timeval record_time()
    {
        return deal_time(0);
    }

    struct timeval get_time()
    {
        return deal_time(1);
    }

    void set_accuracy(int accuracy)
    {
        m_accuracy = accuracy;
    }

    int get_accuracy()
    {
        return m_accuracy;
    }

protected:
    //0: set, 1:get
    struct timeval deal_time(int type)
    {
        static struct timeval tv;
        if (m_accuracy == 1) // 高精度
        {
            gettimeofday(&tv, NULL);
        }
        else // 低精度
        {
            if (type == 0)
            {
                gettimeofday(&tv, NULL);
            }
        }
        return tv;
    }

protected:
    CRecordTime()
    {
        static auto_ptr<CRecordTime> _auto_ptr = auto_ptr<CRecordTime>(this);
        m_accuracy = 1; // 默认使用高精度
        record_time();
    }
    CRecordTime(const CRecordTime&);
    virtual ~CRecordTime(){}
    friend class auto_ptr<CRecordTime>;

    //精度，0：低，1：高。
    //低精度采用record_time一次时间，之后都用记录的这个时间
    //高精度采用每个都真实取时间
    int m_accuracy;
};
#endif
