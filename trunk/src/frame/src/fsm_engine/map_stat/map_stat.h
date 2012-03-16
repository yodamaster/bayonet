/*=============================================================================
#
#     FileName: map_stat.h
#         Desc: 字符串映射的stat
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-22 21:51:01
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-22 21:51:01 | initialization
#
=============================================================================*/
#ifndef _MAP_STAT_H_20110630001944_
#define _MAP_STAT_H_20110630001944_

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

#include <asm/atomic.h>
namespace bayonet {
using namespace std;

const int STAT_MAGICNUM                       = 83846584;                       //统计池幻数 

const int STAT_ID_MAX_SIZE                    = 128; 
const int STAT_ARRVAL_MAX_SIZE                = 128;

const int BUCKET_MAX_SIZE                     = 1024;                           //统计对象桶个数
const int STAT_ARROBJ_MAX_SIZE                = 1024;                           //统计对象的总个数 

const int INVALID_HANDLE                      = -1;                             //无效的next指向

typedef unsigned (*HASH_FUNC)(const char* id);

typedef struct _StStatObj
{
    char id[STAT_ID_MAX_SIZE];                                                  //统计ID
    atomic_t arr_vals[STAT_ARRVAL_MAX_SIZE];                                    //次数数组
    int next;                                                                   //下一个StStatObj
} StStatObj;

typedef struct _StStatPool
{
    long long magic;                                                        //幻数

    int arr_bucket[BUCKET_MAX_SIZE];                                        //StStatObj哈稀桶

    int objs_used;                                                          //使用的StStatObj数目
    StStatObj arr_objs[STAT_ARROBJ_MAX_SIZE];                               //StStatPool数组
} StStatPool;

class CMapStat
{
public:
    CMapStat();
    virtual ~CMapStat();
    
    /**
     * @brief   初始化
     *
     * @param   file_name
     *
     * @return  0
     */
    int Init(const char* file_name, HASH_FUNC func=NULL);

    /**
     * @brief   获取错误原因
     *
     * @return  
     */
    char* GetErrMsg()
    {
        return m_szErrMsg;
    }

    /**
     * @brief   把所有内容重置
     *
     * @return  0
     */
    int Clear();

    /**
     * @brief   清空统计数值（但是统计项还在，避免出现server不停lock文件的情况）
     *
     * @return  
     */
    int ResetStat();

    int AddCount(const char* id, int index);

    int DecCount(const char* id, int index);

    int SetCount(const char* id, int index, int val);

    int GetCount(const char* id, int index, int& val);

    /**
     * @brief   获取当前有效的obj，主要是为遍历准备
     *
     * @param   count
     *
     * @return  
     */
    StStatObj* GetValidObjs(int& count);

    /**
     * @brief   获取统计的展现
     *
     * @param   stat_desc[]
     * @param   stat_num
     *
     * @return  字符串
     */
    string GetStatInfo(const char * const stat_desc[], int stat_num);

    /**
     * @brief   获取一层的统计展现
     *
     * @return  字符串
     */
    string GetBriefStatInfo();

public:
    /**
     * @brief   查找obj
     *
     * @param   id                  id
     * @param   pObj                如果找到那就不为NULL，否则为NULL
     *
     * @return  0
     */
    int FindObj(const char* id, StStatObj*& pObj);

    /**
     * @brief   插入obj，要注意多进程加锁
     *
     * @param   id
     *
     * @return  
     */
    int InsertObj(const char* id, StStatObj*& pObj);

    /**
     * @brief   无论是新增还是读取，总之获取一个对象
     *
     * @param   id
     * @param   pObj
     *
     * @return  0
     */
    int GetObj(const char* id, StStatObj*& pObj);

private:
    /**
     * @brief   如果文件本来就存在，那么要检查格式是否是对的
     *
     * @return  0
     */
    int CheckStatFile();

    /**
     * @brief   如果文件本来不存在，那么要做初始化工作
     *
     * @return  0
     */
    int InitStatFile();

    /**
     * @brief   清理所有数据，包括fd，m_pStatPool
     */
    void CleanUp();


private:
    //哈希函数
    HASH_FUNC m_hashFunc;

    //文件fd，可以给锁用
    int m_fd;

    string m_strFilePath;
    StStatPool* m_pStatPool;
    char m_szErrMsg[1024];
};
}

#endif
