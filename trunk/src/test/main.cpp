#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;
#include "syncframe.h"
int main(int argc, const char *argv[])
{
    
    CSyncFrame::Ins()->ServeForEver();
    return 0;
}
