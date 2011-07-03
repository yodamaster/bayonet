/*=============================================================================
#
#     FileName: dir_func.h
#         Desc: 目录相关的函数
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-09 12:00:16
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-09 12:00:16 | initialization
#
=============================================================================*/
#ifndef _DIR_FUNC_H_20110630001626_
#define _DIR_FUNC_H_20110630001626_

#include <dirent.h>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <error.h>
#include <errno.h>
using namespace std;

/**
 * @brief   递归获取文件[要注意保存原来的目录]
 *
 * @param   dirName     目录
 * @param   repr        文件匹配，为空即代表所有文件
 * @param   vecFiles    返回的文件名列表
 *
 * @return  
 */
static int GetFileInDir(string dirName,string repr,vector<string> &vecFiles)
{
    DIR* Dir = NULL;
    if (dirName[dirName.size()-1] != '/')
    {
        dirName += "/";
    }
    if ((Dir = opendir(dirName.c_str())) == NULL)
    {
        return -1;
    }

    int ret;
    struct dirent entry;
    struct dirent *entryPtr = NULL;

    while (readdir_r(Dir,&entry,&entryPtr) == 0)
    {
        if (entryPtr == NULL)
        {
            break;
        }
        //if the entry is a normal entry
        if (entry.d_type == DT_REG)
        {
            if (repr.empty() || string(entry.d_name) == repr)
            {
                vecFiles.push_back(dirName + entry.d_name);
            }
        }
        //if the entry is a directory
        else if (entry.d_type == DT_DIR && strcmp(entry.d_name, ".") != 0 && strcmp(entry.d_name, "..") != 0)
        {
            ret = GetFileInDir(dirName + entry.d_name, repr, vecFiles);
            if (ret != 0)
            {
                return ret;
            }
        }
    }
    closedir(Dir);
    return 0;
}

/**
 * @brief   递归创建目录[要注意保存原来的目录]
 *
 * @param   dir
 *
 * @return  
 */
static int mkdirs(string dir, string::size_type index = 0)
{
    string::size_type pos;
    int ret;

    if (dir.empty()) {
        return -1;
    }

    if (dir.size()>=2 && dir[index] == '/' && dir[index+1]== '/')
        pos = dir.find('/', index+2);
    else
        pos = dir.find('/', index);

    if (pos != string::npos) {
        string sstr = dir.substr(0, pos);
        ret = mkdir(sstr.c_str(),0777);
        if (ret != 0 && errno != EEXIST)
        {
            return -1;
        }
    } else {
        ret = mkdir(dir.c_str(),0777);
        if (ret != 0 && errno != EEXIST)
        {
            return -1;
        }
        return 0;
    }

    return mkdirs(dir, pos+1);
}

#endif
