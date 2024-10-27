#include <iostream>
#include <string>
#include <strings.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>
#include <assert.h>
#include "Log.hpp"
#include <unistd.h>
using std::string;

enum
{
    SOCKED_ERROR = 1,
    BIND_ERROE,
};

static const string defaultip = "0.0.0.0";
const uint16_t defaultport = 8080;
const int size = 1024;      //缓冲区的大小

class udpserver
{
public:
    udpserver(const uint16_t& port = defaultport, const string& ip = defaultip)
        :_sockfd(-1)
        ,_ip(ip)
        ,_port(port)
    {}
    void initServer()
    {
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INET:表示ipv4. SOCK_DGRAM:表示UDP
        if (_sockfd == -1)
        {
            //创建失败
            assert("create sockfd error");
            exit(SOCKED_ERROR);
        }

        //网络套接字
        struct sockaddr_in local;  //定义一个结构体变量，用来填充sockaddr_in的ip和port
        bzero(&local, sizeof(local));  //将local清零

        //填充网络套接字local
        //将string类型的ip地址转化为uint32_t类型的ip地址
        local.sin_addr.s_addr = inet_addr(_ip.c_str());
        local.sin_family = AF_INET;
        local.sin_port = _port;

        //2.将套接字和ip地址port进行绑定
        if (bind(_sockfd, (const sockaddr *)&local, sizeof(local)) < 0)
        {
            assert("bind error");
            exit(BIND_ERROE);
        }
        std::cout << "serverport:" << _port << std::endl;
        std::cout << "bind success" << std::endl;
    }

    void run()
    {
        _isruning = true;
        char inbuffer[size];

        while(_isruning)
        {
            std::cout << "server say hello" << std::endl;

            struct sockaddr_in client;  //记录客户端（发送方）的ip地址和端口号
            socklen_t len = sizeof(client);

            //通过recvfrom拿到client结构体的ip地址，port端口等,因为sockaddr* src_addr是输出型参数
            int n = recvfrom(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (sockaddr*)&client, &len);
            std::cout << "in: " << inbuffer << std::endl;
            //std::cout << "clientport:" << inet_ntoa(client.sin_addr) << std::endl;

            if(n > 0)
            {
                //结束标志
                inbuffer[n] = 0;

                //拿到的client的结构体中ip是uint32_t类型，需要转化为string类型
                string clientip = inet_ntoa(client.sin_addr);
                uint16_t clientport = ntohs(client.sin_port);
                string message = inbuffer;
                std::cout << clientip << "[" << clientport << "]say# " << message << std::endl;
            }   
        }
    }

    ~udpserver()
    {   
        //判断_sockfd > 0，是为了确保文件描述符有效
        if (_sockfd > 0)
            close(_sockfd);
    }
private:
    int _sockfd;          //文件描述符
    string _ip;      //ip地址
    uint16_t _port;       //端口 --> 表明服务器进程的端口号
    bool _isruning;       //判断是否在运行状态
};