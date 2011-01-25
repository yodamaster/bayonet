#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;
#include "syncframe.h"
#include "socketactor_active.h"
#include "socketactor_passive.h"
#include "socketactor_listen.h"
int main(int argc, const char *argv[])
{
    
    CSyncFrame::Ins()->Process();
    return 0;
}
