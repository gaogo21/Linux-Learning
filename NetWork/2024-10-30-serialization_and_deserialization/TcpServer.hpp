#pragma once

#include <functional>
#include <string>
#include <signal.h>
#include "Socket.hpp"

using namespace std;

using func_t = function<string(string &package);

class TcpServer
{
public:
    TcpServer(uint16_t port, func_t callback)
    :_port(port)
    ,_callback(callback)
    {}

bool InitServer()
{
    _listensock.Socket();
    _listensock.Bind(_port);
    _listensock.Listen();
    cout << "init server ... done" << endl;
    return true;
}

void Start()
{
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    while(true)
    {
        string clientip;
        uint16_t clientport;
        int sockfd = _listensock.Accept(&clientip, &clientport);
        if (sockfd < 0)
        {
            continue;
        }
        cout << "accepet a new link, sockfd : " 
        << sockfd << ", clientip : " << clientip.c_str() << ", clientport : " << clientport << endl;

        if (fork() == 0)
        {
            _listensock.Close();
            string inbuffer_stream;

            //数据计算
            while(true)
            {
                char buffer[10280];
                ssize_t n = read(sockfd, buffer, sizeof(buffer));
                if (n > 0)
                {
                    buffer[n] = 0;
                    inbuffer_stream += buffer;

                    cout << "debug :" << inbuffer_stream.c_str() << endl;

                    while(true)
                    {
                        string info = _callback(inbuffer_stream);
                        if (info.empty())
                            break;
                        
                        cout << "debug, response : " << inbuffer_stream.c_str() << endl;
                    }
                }
                else if (n == 0)
                {
                    break;
                }
                else
                {
                    break;
                }
            }

            exit(0);
        }
        close(sockfd);
    }
}

~TcpServer()
{}

private:
    uint16_t _port;
    Sock _listensock;
    func_t _callback;
};