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

#include <errno.h>
#include <error.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <getopt.h>
#include "map_stat.h"
#include "stat_def.h"

int _clear_flag = 0;
int _loop_flag = 0;
int main(int argc,char** argv)
{
    int input;
    std::string _stat_file;
    while ((input = getopt (argc, argv, "f:cl")) != -1) 
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
    }

    if ( _stat_file.length() == 0 )
    {
        printf("invalid input,please input \"-f stat_file\" \n");
        return -1;
    }

    CMapStat _stat;
    if ( _stat.Init(_stat_file.c_str()) < 0 )
    {
        printf("open stat error\n");
        return 0;
    }

    while(1)
    {
        printf("%s\n",_stat.GetStatInfo(stat_desc,STAT_OVER).c_str());
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
