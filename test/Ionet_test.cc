#include "../net/Ionet.h"
int main(void)
{
    ionet::Ionet tcpnet;
    tcpnet.netInit("127.0.0.1", 9999);
    tcpnet.run();
    return 0;
}
