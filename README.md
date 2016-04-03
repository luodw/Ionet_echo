自己用C++11写的一个回显服务器,底层使用的是epoll系统调用,以及使用了C++11新加的特性,例如function,lambda,auto等等.
编译运行也很简单,只需要在主目录下运行make即可
```
make
```

主程序为:
```
#include "../net/Ionet.h"

int main(void)
{
    ionet::Ionet tcpnet;
    tcpnet.netInit("127.0.0.1", 9999);
    tcpnet.run();
    return 0;
}
```
声明一个Ionet类,然后调用初始化函数,绑定ip地址和端口,运行即可.下面是输出截图:

1. 运行服务器时,有如下输出:
![程序运行时初始日志输出](http://7xjnip.com1.z0.glb.clouddn.com/%E9%80%89%E5%8C%BA_065.png "")
由于只有一个listenfd,且使用的是fd=4,只有可读事件.

2. 当有两个客户端连接时,有如下输出:
![有客户端连接时,日志输出](http://7xjnip.com1.z0.glb.clouddn.com/%E9%80%89%E5%8C%BA_066.png "")
这两个客户端分别占用了fd=5和fd=6两个文件描述符,并加入可读事件

3. 当有一个客户端有数据输入时,输出如下:
![客户端有数据输入时日志输出](http://7xjnip.com1.z0.glb.clouddn.com/%E9%80%89%E5%8C%BA_067.png "")
首先服务器接收数据,然后注册一个可写事件,当数据写完之后,再将可写事件删除.

4. 客户端接收数据时,客户端输出如下,即回显:
![客户端接收数据输出](http://7xjnip.com1.z0.glb.clouddn.com/%E9%80%89%E5%8C%BA_068.png "")

5. 当客户端退出之后,删除这个fd上的可读事件,底层epoll也将这个fd删除,最后关闭这个fd.服务器输出为:
![客户端退出时日志输出](http://7xjnip.com1.z0.glb.clouddn.com/%E9%80%89%E5%8C%BA_069.png "")

这只是我写NoSQL的一个子项目,接下来,我先把这个子项目生成一个NoSQL数据库,而且功能还会继续完善...
