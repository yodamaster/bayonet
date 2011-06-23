/*=============================================================================
#
#     FileName: map_stat.cpp
#         Desc:
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-23 00:24:37
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-23 00:24:37 | initialization
#
=============================================================================*/

#include <stdio.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "map_stat.h"

/**
 * @brief   哈希函数
 *
 * @param   id
 *
 * @return  
 */
inline unsigned hashid(const char* id) {
    int len = strlen(id);
    unsigned hash = 1315423911;

    for (int i = 0; i < len; ++i) {
        hash ^= ((hash << 5) + id[i] + (hash >> 2));
    }

    return hash;
}

class CLockProxy
{
public:
    CLockProxy(int fd) {
        m_fd = fd;
        fcntl(m_fd, F_SETLKW, file_lock(F_WRLCK, SEEK_SET));
    }
    virtual ~CLockProxy() {
        fcntl(m_fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
    }

    /**
     * @brief    用来给文件加锁的函数
     *
     * @param    type
     * @param    whence
     *
     * @return   
     */
    struct flock* file_lock(short type, short whence)
    {
        static struct flock ret;
        ret.l_type = type ;
        ret.l_start = 0;
        ret.l_whence = whence;
        ret.l_len = 0;
        ret.l_pid = getpid();
        return &ret;
    }

    int m_fd;
};
//=============================================================================

CMapStat::CMapStat() {
    m_hashFunc = hashid;
    m_fd = -1;
    m_pStatPool = NULL;
    m_szErrMsg[0]='\0';
}
CMapStat::~CMapStat()
{
    CleanUp();
}

int CMapStat::Init(const char* file_path, HASH_FUNC func)
{
    if (file_path == NULL)
    {
        STAT_ERROR("file_path is null");
        return -1;
    }

    if (func)
    {
        m_hashFunc = func;
    }

    m_strFilePath = file_path;

    bool fileexist = false;

    if (!access(file_path, R_OK)) {
        fileexist = true;
    }

    m_fd = open(file_path, O_RDWR | O_CREAT, 0644);
    if(m_fd < 0)
    {
        STAT_ERROR("file open fail,file_path:%s",file_path);
        return -2;
    }

    if ((!fileexist && ftruncate(m_fd, sizeof(StStatPool)) < 0)) {
        CleanUp();
        STAT_ERROR("ftruncate fail,file_path:%s",file_path);
        return -3;
    }

    m_pStatPool = (StStatPool*)mmap(NULL, sizeof(StStatPool), PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);

    if (m_pStatPool == MAP_FAILED)
    {
        CleanUp();
        STAT_ERROR("mmap fail");
        return -4;
    }

    int ret;
    if (fileexist)
    {
        ret = CheckStatFile();
    }
    else
    {
        ret = InitStatFile();
    }

    if (ret != 0)
    {
        CleanUp();
    }
    return ret;
}
int CMapStat::CheckStatFile()
{
    STAT_CHECK_PSTATPOOL(m_pStatPool);

    if (m_pStatPool->magic != STAT_MAGICNUM)
    {
        STAT_ERROR("magic not match,should:%d,real:%lld",STAT_MAGICNUM,m_pStatPool->magic);
        return -1;
    }

    struct stat file_stat;
    if (stat(m_strFilePath.c_str(), &file_stat) < 0) {
        STAT_ERROR("Stat mapfile:%s stat error.ErrMsg:%m\n", m_strFilePath.c_str());
        return -1;
    }

    if ((unsigned long)file_stat.st_size != sizeof(StStatPool)) {
        fprintf(stderr, "Stat mapfile:%s size=%lu error, should be = %u.\n",
                m_strFilePath.c_str(), (unsigned long)file_stat.st_size, sizeof(StStatPool));
        return -1;
    }
    return 0;
}

int CMapStat::InitStatFile()
{
    STAT_CHECK_PSTATPOOL(m_pStatPool);

    memset(m_pStatPool, 0, sizeof(StStatPool));
    m_pStatPool->magic = STAT_MAGICNUM;
    m_pStatPool->objs_used= 0;

    int i;

    for (i = 0; i < BUCKET_MAX_SIZE; ++i)
        m_pStatPool->arr_bucket[i] = INVALID_HANDLE;

    for (i = 0; i < STAT_ARROBJ_MAX_SIZE - 1; ++i)
        m_pStatPool->arr_objs[i].next = INVALID_HANDLE;

    return 0;
}

void CMapStat::CleanUp()
{
    if (m_fd > 0)
    {
        close(m_fd);
    }
    m_fd = -1;

    if (m_pStatPool)
    {
        munmap(m_pStatPool, sizeof(StStatPool));
    }
    m_pStatPool = NULL;
}

int CMapStat::FindObj(const char* id, StStatObj*& pObj)
{
    STAT_CHECK_PSTATPOOL(m_pStatPool);

    pObj = NULL;

    int bucketid  = m_hashFunc(id) % BUCKET_MAX_SIZE;

    int next = m_pStatPool->arr_bucket[bucketid];

    StStatObj* tmp = NULL;

    while (1)
    {
        if (INVALID_HANDLE == next)
        {
            break;
        }
        else
        {
            tmp = &(m_pStatPool->arr_objs[next]);

            if (!strcmp(tmp->id, id))
            {
                pObj = tmp;
                break;
            }
            else
            {
                next = tmp->next;
            }
        }
    }

    return 0;
}
int CMapStat::InsertObj(const char* id, StStatObj*& pObj)
{
    STAT_CHECK_PSTATPOOL(m_pStatPool);

    //lock

    CLockProxy tmp_lock(m_fd);

    int ret;

    pObj = NULL;

    //还是得再查一遍，说不定已经被生成了
    ret = FindObj(id, pObj);
    if (ret != 0)
    {
        STAT_ERROR("FindObj fail:%d",ret);
        return -1;
    }

    if (pObj)//找到了，所以可以直接返回了
    {
        return 0;
    }

    if (m_pStatPool->objs_used >= STAT_ARROBJ_MAX_SIZE)
    {
        STAT_ERROR("objs_used reach the max:%d/%d",m_pStatPool->objs_used, STAT_ARROBJ_MAX_SIZE);
        return -1;
    }


    int bucketid = m_hashFunc(id) % BUCKET_MAX_SIZE;
    int next = m_pStatPool->arr_bucket[bucketid];

    int obj_idx = m_pStatPool->objs_used++;

    pObj = &(m_pStatPool->arr_objs[obj_idx]);
    snprintf(pObj->id,sizeof(pObj->id),"%s",id);

    pObj->next = next;
    m_pStatPool->arr_bucket[bucketid] = obj_idx;

    //unlock
    return 0;
}
int CMapStat::GetObj(const char* id, StStatObj*& pObj)
{
    STAT_CHECK_PSTATPOOL(m_pStatPool);

    int ret;

    ret = FindObj(id,pObj);
    if (ret != 0)
    {
        STAT_ERROR("FindObj fail,ret:%d,id:%s",ret,id);
        return ret;
    }
    if (pObj)
    {
        return 0;
    }

    ret = InsertObj(id, pObj);
    if (ret != 0)
    {
        STAT_ERROR("InsertObj fail,ret:%d,id:%s",ret,id);
        return -2;
    }

    return 0;
}

int CMapStat::Reset()
{
    return InitStatFile();
}

int CMapStat::AddCount(const char* id, int index)
{
    STAT_CHECK_PSTATPOOL(m_pStatPool);

    if (index >= STAT_ARRVAL_MAX_SIZE)
    {
        STAT_ERROR("index is too large,%d/%d",index,STAT_ARRVAL_MAX_SIZE);
        return -1;
    }

    int ret;
    StStatObj* pObj = NULL;
    ret = GetObj(id, pObj);
    if (ret != 0)
    {
        STAT_ERROR("GetObj fail.ret:%d,id:%s",ret,id);
        return -1;
    }
    if (pObj == NULL)
    {
        STAT_ERROR("GetObj fail.pObj is NULL");
        return -1;
    }

    STATVAL_INC(pObj->arr_vals[index]);

    return 0;
}

int CMapStat::DecCount(const char* id, int index)
{
    STAT_CHECK_PSTATPOOL(m_pStatPool);

    if (index >= STAT_ARRVAL_MAX_SIZE)
    {
        STAT_ERROR("index is too large,%d/%d",index,STAT_ARRVAL_MAX_SIZE);
        return -1;
    }

    int ret;
    StStatObj* pObj = NULL;
    ret = GetObj(id, pObj);
    if (ret != 0)
    {
        STAT_ERROR("GetObj fail.ret:%d,id:%s",ret,id);
        return -1;
    }
    if (pObj == NULL)
    {
        STAT_ERROR("GetObj fail.pObj is NULL");
        return -1;
    }

    STATVAL_DEC(pObj->arr_vals[index]);

    return 0;
}
int CMapStat::SetCount(const char* id, int index, int val)
{
    STAT_CHECK_PSTATPOOL(m_pStatPool);

    if (index >= STAT_ARRVAL_MAX_SIZE)
    {
        STAT_ERROR("index is too large,%d/%d",index,STAT_ARRVAL_MAX_SIZE);
        return -1;
    }

    int ret;
    StStatObj* pObj = NULL;
    ret = GetObj(id, pObj);
    if (ret != 0)
    {
        STAT_ERROR("GetObj fail.ret:%d,id:%s",ret,id);
        return -1;
    }
    if (pObj == NULL)
    {
        STAT_ERROR("GetObj fail.pObj is NULL");
        return -1;
    }

    STATVAL_SET(pObj->arr_vals[index], val);

    return 0;
}
int CMapStat::GetCount(const char* id, int index, int& val)
{
    STAT_CHECK_PSTATPOOL(m_pStatPool);

    if (index >= STAT_ARRVAL_MAX_SIZE)
    {
        STAT_ERROR("index is too large,%d/%d",index,STAT_ARRVAL_MAX_SIZE);
        return -1;
    }

    int ret;
    StStatObj* pObj = NULL;
    ret = GetObj(id, pObj);
    if (ret != 0)
    {
        STAT_ERROR("GetObj fail.ret:%d,id:%s",ret,id);
        return -1;
    }
    if (pObj == NULL)
    {
        STAT_ERROR("GetObj fail.pObj is NULL");
        return -1;
    }

    val = STATVAL_READ(pObj->arr_vals[index]);

    return 0;
}
StStatObj* CMapStat::GetValidObjs(int& count)
{
    if (m_pStatPool == NULL)
    {
        STAT_ERROR("pStatPool is NULL");
        return NULL;
    }

    count = m_pStatPool->objs_used;

    return m_pStatPool->arr_objs;
}

string CMapStat::GetStatInfo(const char * const stat_desc[], int stat_num)
{
    if (m_pStatPool == NULL)
    {
        STAT_ERROR("pStatPool is NULL");
        return "";
    }

    int num = stat_num <= STAT_ARRVAL_MAX_SIZE ? stat_num : STAT_ARRVAL_MAX_SIZE;

    StStatObj* p = m_pStatPool->arr_objs;
    int count = m_pStatPool->objs_used;

    char buf[512];
    string output;

    for (int i = 0; i < count; i++)
    {
        output.append(p->id);
        output.append("\n");
        for (int j = 0; j < num; j++)
        {
            snprintf(buf,sizeof(buf), "\t%-40s:  %-20d\n", 
                     stat_desc[j], 
                     STATVAL_READ(p->arr_vals[j]));
            output.append(buf);
        }
        output.append("\n");
        ++p;
    }
    return output;
}
