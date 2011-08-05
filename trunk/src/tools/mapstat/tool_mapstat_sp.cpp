/*=============================================================================
#
#     FileName: tool_mapstat.cpp
#         Desc: 
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-23 02:18:58
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-23 02:18:58 | initialization
#
=============================================================================*/

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
#include <errno.h>
#include <error.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <getopt.h>
#include "map_stat.h"
#include "stat_def.h"

using namespace std;

#ifndef foreach
#define foreach(container,it) \
    for(typeof((container).begin()) it = (container).begin();it!=(container).end();++it)
#endif

//定义一下
#define STATVAL_READ(val)                   atomic_read(&(val))

int _clear_flag = 0;
int _loop_flag = 0;

int SplitString(const string &srcStr,const string &splitStr,vector<string> &destVec)
{
    if(srcStr.size()==0)
    {
        return 0;
    }
    size_t oldPos,newPos;
    oldPos=0;
    newPos=0;
    string tempData;
    while(1)
    {
        newPos=srcStr.find(splitStr,oldPos);
        if(newPos!=string::npos)
        {
            tempData = srcStr.substr(oldPos,newPos-oldPos);
            destVec.push_back(tempData);
            oldPos=newPos+splitStr.size();
        }
        else if(oldPos<=srcStr.size())
        {
            tempData= srcStr.substr(oldPos);
            destVec.push_back(tempData);
            break;
        }
        else
        {
            break;
        }
    }
    return 0;
}

string GetRichStatInfo(bayonet::CMapStat& _stat, const char * const stat_desc[], int stat_num, vector<string>& vecKey1, vector<string>& vecKey2)
{
    int num = stat_num <= bayonet::STAT_ARRVAL_MAX_SIZE ? stat_num : bayonet::STAT_ARRVAL_MAX_SIZE;

    bayonet::StStatObj* pObj;
    int count;

    pObj = _stat.GetValidObjs(count);

    map<string, map<string, string > > mapTmp;

    char buf[512];

    for (int i = 0; i < count; i++)
    {
        bayonet::StStatObj* p = pObj+i;
        vector<string> vecRes;
        SplitString(p->id,"*",vecRes);

        if (vecRes.size()!=2)
        {
            continue;
        }

        if (!vecKey1.empty() && find(vecKey1.begin(), vecKey1.end(), vecRes[0]) == vecKey1.end())
        {
            continue;
        }
        if (!vecKey2.empty() && find(vecKey2.begin(), vecKey2.end(), vecRes[1]) == vecKey2.end())
        {
            continue;
        }
        string output;
        for (int j = 0; j < num; j++)
        {
            snprintf(buf,sizeof(buf), "\t\t%-40s:  %-20u\n", 
                     stat_desc[j], 
                     STATVAL_READ(p->arr_vals[j]));
            output.append(buf);
        }
        mapTmp[vecRes[0]][vecRes[1]] = output;
    }

    stringstream ss;
    foreach(mapTmp, it1)
    {
        ss << it1->first << endl;
        foreach(it1->second, it2)
        {
            ss << "\t" << it2->first << endl;

            ss << it2->second << endl;

        }
    }
    return ss.str();
}

int main(int argc,char** argv)
{
    int input;
    std::string _stat_file;
    string key1;
    string key2;
    while ((input = getopt (argc, argv, "f:x:y:cl")) != -1) 
    {
        if ( input == 'c' )
        {
            _clear_flag = 1;
            continue;
        }
        if ( input == 'l' )
        {
            _loop_flag = 1;
            continue;
        }
        if ( input == 'f' )
        {
            _stat_file = optarg;
            continue;
        }
        if ( input == 'x' )
        {
            key1 = optarg;
            continue;
        }
        if ( input == 'y' )
        {
            key2 = optarg;
            continue;
        }
    }

    if ( _stat_file.length() == 0 )
    {
        printf("invalid input,please input \"-f stat_file\" \n");
        return -1;
    }

    vector<string> vecKey1,vecKey2;
    SplitString(key1,"*",vecKey1);
    SplitString(key2,"*",vecKey2);

    bayonet::CMapStat _stat;
    if ( _stat.Init(_stat_file.c_str()) < 0 )
    {
        printf("open stat error\n");
        return 0;
    }

    while(1)
    {
        printf("%s\n",GetRichStatInfo(_stat, bayonet::stat_desc,bayonet::STAT_OVER,vecKey1,vecKey2).c_str());
        if ( _clear_flag )
        {
            _stat.ResetStat();
        }
        printf("\n");
        printf(".............................................\n");
        printf("\n");
        sleep(1);

        if ( !_loop_flag ) break;
    }

    return 0;
}
