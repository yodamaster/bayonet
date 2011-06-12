/*=============================================================================
#  Author:          DanteZhu - http://www.vimer.cn
#  Email:           dantezhu@vip.qq.com
#  FileName:        appwork_stat.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2010-10-09 10:27:07
#  History:         
=============================================================================*/
#include <errno.h>
#include <error.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <getopt.h>
#include "dirstat.h"
#include "stat_def.h"

int _clear_flag = 0;
int _loop_flag = 0;

int main(int argc,char** argv)
{
    int input;
    std::string _stat_file="stat_file";
    std::string stat_dir;
    int num=-1;

    string key1;
    string key2;
    while ((input = getopt (argc, argv, "m:f:d:x:y:cl")) != -1) 
    {
        if ( input == 'd' )
        {
            num = atoi(optarg);
            continue;
        }
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
        if ( input == 'm' )
        {
            stat_dir = optarg;
            continue;
        }
    }

    if ( stat_dir.length() == 0 )
    {
        printf("invalid input,please input \"-m stat_dir\" \n");
        return -1;
    }
    //printf("show num:%d\n",num);

    vector<string> vecKey1,vecKey2;
    CDirStat::SplitString(key1,"&",vecKey1);
    CDirStat::SplitString(key2,"&",vecKey2);

    CDirStat _stat;
    if ( _stat.Init(stat_dir.c_str(),_stat_file.c_str(),&stat_desc[0],STAT_OVER) < 0 )
    {
        printf("open stat error\n");
        return 0;
    }

    while(1)
    {
        cout << _stat.GetStatInfo(vecKey1,vecKey2);
        if ( _clear_flag )
        {
            _stat.ResetStat(vecKey1,vecKey2);
        }
        printf("\n");
        printf(".............................................\n");
        printf("\n");
        sleep(1);

        if ( !_loop_flag ) break;
    }

    return 0;
}
