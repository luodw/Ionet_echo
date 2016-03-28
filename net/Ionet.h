#ifndef _IONET_
#define _IONET_
#include "Fdevent.h"
#include "EventLoop.h"
#include "../util/Nocopyable.h"
namespace ionet
{
    class EventLoop;
    class Ionet:public nocopyable
    {
    public:
        enum {
            IONET_NONE=0,
            IONET_READ=1,
            IONET_WRITE=2,
            IONET_ERROR=4
        };//事件类型
        Ionet();
        ~Ionet();
        //用ip和端口来初始化这个服务器
        void netInit(std::string ipstr,short port);
        void run();//执行事件监听循环
    private:
        void FdAccept(int listenfd);//listened读事件回调函数
        void FdHandler(int connfd);//客户端读事件回调函数
        int listenfd;//服务监听的文件描述符
        EventLoop *loop;
    };
}

#endif
