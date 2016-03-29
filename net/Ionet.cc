#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "Ionet.h"
#include "../util/Logger.h"
#include "Fdevent.h"
#include "../util/buffer.h"
using namespace std;
namespace ionet
{
    void setnoblocking(int fd)
    {
        //设置文件描述符为非阻塞
        fcntl(fd,F_SETFL,fcntl(fd, F_GETFL,0)|O_NONBLOCK);
    }
    void Ionet::FdHandler(int clientfd)
    {
        //这只是个简单的服务器,支持回显,所以客户端读事件简单读取客户端发送的内容,在回显给客户端
        char mesg[]="Come from server:";
        char buf1[1024];
        int nr=recv(clientfd,buf1,sizeof(buf1),0);
        buf1[nr]='\0';//手动添加字符串结束符
        if(nr<0)//读取失败
        {
            Logger::logging(Logger::ERROR,__FILE__,__LINE__, strerror(errno));
            exit(-1);
        }
        else if(nr==0)//客户端关闭,服务器读取0字节字符
        {
            loop->delEvent(clientfd, Ionet::IONET_READ);
        }
        else
        {//到这表示正常读取客户端数据
            Buffer buffer;//声明一个缓存类
            buffer.len=sizeof(mesg)+nr;
            buffer.buf=(char*)malloc(buffer.len);
            char *p=strcpy(buffer.buf,mesg);
            p += sizeof(mesg)-1;
            strcpy(p, buf1);
            //因为读取客户端数据,服务器处理之后就要回复客户端,所以必须注册一个客户端写事件,这里直接用
            //c++11的lambda函数注册
            loop->addEvent(clientfd, Ionet::IONET_WRITE, [this,&buffer](int connfd)
            {

                    int ret=send(connfd,buffer.buf,buffer.len,0);
                    if(ret<0)
                    {
                        Logger::logging(Logger::ERROR,__FILE__,__LINE__, strerror(errno));
                        exit(-1);
                    }
                    free(buffer.buf);
                    //写事件结束之前必须关闭这个客户端的写事件,因为没有数据可写了
                    this->loop->delEvent(connfd, Ionet::IONET_WRITE);
                    //还要打开该客户端上的读事件,之前在注册读事件时,有关闭
                    this->loop->updateEvent(connfd);
            });
        }
    }
    void Ionet::FdAccept(int listenfd)
    {
        int clientfd=accept(listenfd,NULL,NULL);
        setnoblocking(clientfd);
        Logger::logging(Logger::TRACE,__FILE__,__LINE__, "Accept a clientfd=%d",clientfd);
        if(clientfd==-1)
        {
            Logger::logging(Logger::ERROR,__FILE__,__LINE__, "Accept error!");
            exit(-1);
        }
        loop->addEvent(clientfd, Ionet::IONET_READ, [this](int connfd)
        {
            this->FdHandler(connfd);//客户端可读事件的回调函数
        });
    }
    Ionet::Ionet(){
        loop=new EventLoop;
    }
    Ionet::~Ionet()
    {
        delete loop;
    }
    //服务器初始化
    void Ionet::netInit(std::string ipstr, short port)
    {
        int opt=1;
        struct sockaddr_in sinet;
        listenfd=socket(AF_INET,SOCK_STREAM,0);
        setnoblocking(listenfd);
        setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
        bzero(&sinet,sizeof(sinet));
        sinet.sin_family=AF_INET;
        sinet.sin_port=htons(port);
        sinet.sin_addr.s_addr=inet_addr(ipstr.c_str());
        int ret=bind(listenfd, (struct sockaddr*)&sinet, sizeof(sinet));
        if(ret==-1)
        {
            Logger::logging(Logger::ERROR,__FILE__,__LINE__, strerror(errno));
            exit(-1);
        }
        ret=listen(listenfd,5);
        if(ret==-1)
        {
            Logger::logging(Logger::ERROR,__FILE__,__LINE__, strerror(errno));
            exit(-1);
        }
        //listenfd可读事件的回调函数,即accept接收客户端请求
        loop->addEvent(listenfd, Ionet::IONET_READ, [this](int lisfd)
        {
            this->FdAccept(lisfd);
        });
        Logger::logging(Logger::TRACE,__FILE__,__LINE__, "Net init successfully!");
    }

    //Ionet执行,进入事件监听循环
    void Ionet::run()
    {
        while(1)
        {
            loop->startLoop();
        }
    }
}
