#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;
#include "bayonet_frame.h"
#include "socketactor_base.h"
#include "socketactor_active.h"
#include "socketactor_passive.h"
#include "socketactor_listen.h"
class CSocketActorAccept : public CSocketActorPassive
{
public:
    CSocketActorAccept () {}
    virtual ~CSocketActorAccept () {}
protected:
    virtual int HandleEncode(
            char *buf,
            int &len)
    {
        return 0;
    }
    virtual int HandleInput(
            const char *buf,
            int len)
    {
        return 0;
    }
    virtual int HandleRecvOver(const char *buf, int len)
    {
        return 0;
    }
};
class CSocketActorListenImpl : public CSocketActorListen
{
public:
    CSocketActorListenImpl () {}
    virtual ~CSocketActorListenImpl () {}

protected:
    virtual CSocketActorBase* AllocSocketActorAccept()
    {
        return new CSocketActorAccept();
    }
};
int main(int argc, const char *argv[])
{
    CBayonetFrame srv;
    StFrameParam param;
    param.ip="127.0.0.1";
    param.port = 10001;
    param.protoType = PROTO_TYPE_UDP;
    param.pSocketActorListen = new CSocketActorListenImpl();
    
    srv.Init(param);
    srv.Process();
    return 0;
}
