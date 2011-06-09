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
#include "stat_def.h"
#include "dirstat.h"

using namespace std;
int main(int argc, char **argv)
{
    CDirStat dirStat;
    int ret = dirStat.Init("../../svr2/stat/","stat_file",stat_desc,STAT_OVER);
    if (ret)
    {
        printf("init fail:%d\n",ret);
    }

    dirStat.ShowStatInfo();
    return 0;
}
