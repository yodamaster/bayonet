/*=============================================================================
#
#     FileName: comm_func.h
#         Desc: 公共函数
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-09 12:15:10
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-09 12:15:10 | initialization
#
=============================================================================*/
#ifndef __COMM_FUNC_H__
#define __COMM_FUNC_H__
static int SplitString(const string &srcStr,const string &splitStr,vector<string> &destVec)
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

#endif
