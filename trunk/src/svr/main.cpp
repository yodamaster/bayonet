#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;
#include "bayonet_frame.h"
class CActionFirst : public IAction
{
public:
    CActionFirst () {}
    virtual ~CActionFirst () {}
    // 为发送打包
    virtual int HandleEncodeSendBuf(
            string & strSendBuf,
            int &len)
    {
        strSendBuf="woainizhende111111";
        len = strSendBuf.size();
        return 0;
    }

    // 回应包完整性检查
    virtual int HandleInput(
            const char *buf,
            int len)
    {
        return len;
    }

    // 回应包解析
    virtual int HandleDecodeRecvBuf(const char *buf, int len)
    {
        return 0;
    }
};
int main(int argc, const char *argv[])
{
    CBayonetFrame srv;
    StFrameParam param;
    param.ip="0.0.0.0";
    param.port = 10001;
    param.bKeepcnt= true;
    param.protoType = PROTO_TYPE_UDP;
    //param.protoType = PROTO_TYPE_TCP;
    param.pAction = new CActionFirst();
    
    srv.Init(param);
    srv.Process();
    return 0;
}
