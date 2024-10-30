#pragma once
#include <iostream>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

const int defaultport = 8080;
const int backlog = 10;
const int size = 1024;

enum
{
    CREATE_SOCKFD_ERROR = 1,
    BIND_ERROR,
    LISTEN_ERROR,
    ACCEOPT_ERRPR,
};

class TcpServer;
struct ThreadData
{
    ThreadData(int sock, TcpServer* self)
    :_sock(sock)
    ,_self(self)
    {}

    int _sock;
    TcpServer* _self;
};



class TcpServer
{
public:
    TcpServer(const uint16_t& port = defaultport)
    :_listensock(-1)
    ,_port(port)
    {}

    void InitTcpServer()
    {
        //1.创建套接字
        _listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensock == -1)
        {
            cout << "创建套接字失败" << endl;
            exit(CREATE_SOCKFD_ERROR);
        }
        cout << "listensock create success" << endl;

        //2.填充套接字
        struct sockaddr_in local;
        bzero(&local, sizeof(local));

        local.sin_family = AF_INET;
        local.sin_port = _port;
        local.sin_addr.s_addr = INADDR_ANY;   //INDAAR_ANY:为0.0.0.0,表示任何ip地址

        //3.将套接字和文件描述符bind
        if (bind(_listensock, (const struct sockaddr*)&local, sizeof(local)) == -1)
        {
            //绑定失败
            cout << "绑定失败" << endl;
            exit(BIND_ERROR);
        }
        cout <<"bind success" << endl;

        //4.将listensock设置为监听状态
        //系统调用listen将套接字设置为监听状态，这个套接字和udp中的不一样
        //它不用来通信，而是用来监听客户端的状态，检测客户端的连接请求
        if (listen(_listensock, backlog) < 0)
        {
            cout << "TCP 监听失败..." << endl;
            exit(LISTEN_ERROR);
        }
        cout << "listen success" << endl;
    }

    //1.单进程版本
    // void Start()
    // {
    //     while(true)
    //     {   
    //         //1.获得client的套接字
    //         struct sockaddr_in client;
            
    //         //2.通过accept获取client的值
    //         //accept用来接收客户端的连接请求，recvform用来接收客户端的数据
    //         //listensock是用来接收客户端连接请求的，accept的返回值sock是操作系统用来和客户端一对一通信的
    //         socklen_t len = sizeof(client);
    //         int sock = accept(_listensock, (struct sockaddr*)&client, &len);
    //         if (sock < 0)
    //         {
    //             cout << "accept 失败" << endl;
    //             exit(ACCEOPT_ERRPR);
    //         }

    //         //启动服务器
    //         ServerIO(sock);
    //     }
    // }

    //2.多进程版本 --孙子进程执行通信，子进程退出
    //为什么不子进程进行通信，父进程阻塞等待呢？
    //因为子进程执行的ServerIO()是死循环，如果子进程一直执行ServerIO(),父进程就会一直阻塞等待
    //这样另一个客户端来连接服务端的时候，因为父进程被阻塞等待了，所以无法继续创建子进程，还是和单进程版本一样的效果

    // void Start()
    // {
    //     while(true)
    //     {   
    //         //1.获得client的套接字
    //         struct sockaddr_in client;
            
    //         //2.通过accept获取client的值
    //         //accept用来接收客户端的连接请求，recvform用来接收客户端的数据
    //         //listensock是用来接收客户端连接请求的，accept的返回值sock是操作系统用来和客户端一对一通信的
    //         socklen_t len = sizeof(client);
    //         int sock = accept(_listensock, (struct sockaddr*)&client, &len);
    //         if (sock < 0)
    //         {
    //             cout << "accept 失败" << endl;
    //             exit(ACCEOPT_ERRPR);
    //         }

    //         //创建子进程
    //         pid_t id = fork();

    //         //子进程
    //         if (id == 0)
    //         {   
    //             //关闭子进程中的监听套接字
    //             //为了防止子进程进行误操作以及一定程度上节省资源，在子进程中将_listnesock指向的监听套接字关闭。
    //             close(_listensock);

    //             //让父进程（也就是当前子进程退出）
    //             if (fork() > 0)
    //                 exit(0);
                
    //             //孙子进程执行代码
    //             ServerIO(sock);//启动服务器
    //             close(sock);   //不进行通信后关闭套接字
    //             exit(0);       //正常退出
    //         }
    //     }
    // }

    //3.多线程版本
    //从线程执行的函数
    static void* ThreadRoutine(void* args)
    {
        pthread_detach(pthread_self());  //线程分离，使操作系统回收线程
        ThreadData* td = static_cast<ThreadData*>(args);
        td->_self->ServerIO(td->_sock); //调用具体的服务函数
        return nullptr;
    }

    void Start()
    {
        while(true)
        {   
            //1.获得client的套接字
            struct sockaddr_in client;
            
            //2.通过accept获取client的值
            //accept用来接收客户端的连接请求，recvform用来接收客户端的数据
            //listensock是用来接收客户端连接请求的，accept的返回值sock是操作系统用来和客户端一对一通信的
            socklen_t len = sizeof(client);
            int sock = accept(_listensock, (struct sockaddr*)&client, &len);
            if (sock < 0)
            {
                cout << "accept 失败" << endl;
                exit(ACCEOPT_ERRPR);
            }

            //多线程
            pthread_t tid;
            ThreadData* td = new ThreadData(sock, this);  //创建用于传参的结构体
            pthread_create(&tid, nullptr, ThreadRoutine, td);
        }
    }

    //服务器的服务函数
    void ServerIO(int sock)
    {
        char inbuffer[size];
        while(true)
        {
            //使用read函数，将网络中传输的数据用sock文件描述符读取到inbuffer中，读取的大小是sizeof(inbuffer) - 1
            ssize_t n = read(sock, inbuffer, sizeof(inbuffer) - 1);
            
            if (n > 0)
            {   
                //在最后一个位置添加0，用于停止
                inbuffer[n] = 0;

                cout << "服务端接收到数据 : " << inbuffer << endl;

                string outbuffer = inbuffer;
                cout << outbuffer << " server[echo]" << endl;

                write(sock, outbuffer.c_str(), sizeof(outbuffer) - 1);
            }
            else if (n == 0)  //这种情况是客户端关闭连接了
            {
                cout << "客户端关闭退出，服务端也退出..." << endl;
                break;
            }
        }

        close(sock);
    }

    ~TcpServer()
    {}

private:
    int _listensock;
    uint16_t _port;
};