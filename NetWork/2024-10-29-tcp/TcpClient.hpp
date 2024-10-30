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

using namespace std;

const uint16_t default_serverport = 8080;
const string default_serverip = "0.0.0.0";

enum
{
    CREATE_SOCKFD_ERROR = 1,
    CONNECT_ERROR,
    LISTEN_ERROR,
    ACCEOPT_ERRPR,
};

//使用方式是 ./tcpclient 127.0.0.0.1 8080
class TcpClient
{
public:
    TcpClient(const string& serverip = default_serverip, const uint16_t& port = default_serverport)
    :_serverip(serverip)
    ,_serverport(default_serverport)
    ,_sockfd(-1)
    {}
    
    void InitTcpClient()
    {
        //1.创建套接字
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_sockfd < 0)
        {
            cout  << "客户端创建套接字失败..." << endl;
            exit(CREATE_SOCKFD_ERROR);
        }     
        cout << "client craete sock success" << endl;
    }

    void Start()
    {
        //1.填充套接字
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_port = _serverport;
        local.sin_addr.s_addr = inet_addr(_serverip.c_str());

        //2.和服务端连接
        socklen_t len = sizeof(local);
        if (connect(_sockfd, (const struct sockaddr*)&local, len) < 0)
        {
            cout << "客户端连接失败..." << endl;
            exit(CONNECT_ERROR);
        }
        cout << "client connect success" << endl;

        //和服务端进行通信
        string message;
        while(true)
        {
            cout << "Enter#";
            getline(cin, message);

            //通过sockfd，向服务端写数据，内容是message
            write(_sockfd, message.c_str(), sizeof(message) - 1);
            //可以通过read函数将服务端接收到的数据回显到服务端，这里就不作处理了
        }

        close(_sockfd);
    }
private:
    string _serverip;
    uint16_t _serverport;
    int _sockfd;
};