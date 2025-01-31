#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

enum
{
    SocketErr = 2,
    BindErr,
    ListenErr,
};

const int backlog = 10;

class Sock
{
public:
    Sock()
    {}

    ~Sock()
    {}

public:
    void Socket()
    {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_sockfd < 0)
        {
            std::cout << "sock error " << strerror(errno) << errno << std::endl;
            exit(SocketErr);
        }
    }

    void Bind(uint16_t port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr =INADDR_ANY;

        if (bind(_sockfd, (struct sockaddr*)&local, sizeof(local)) < 0)
        {
            std::cout << "bind error " << strerror(errno) << errno << std::endl;
            exit(BindErr);
        }
    }

    void Listen()
    {
        if (listen(_sockfd, backlog) < 0)
        {
            std::cout << "listen error " << strerror(errno) << errno << std::endl;
            exit(ListenErr);
        }
    }

    int Accept(std::string *clientip, uint16_t *clientport)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int newfd = accept(_sockfd, (struct sockaddr*)& peer, &len);
        if (newfd < 0)
        {
            std::cout << "accept error " << strerror(errno) << errno << std::endl;
            return -1;
        }

        char ipstr[64];
        inet_ntop(AF_INET, &peer.sin_addr, ipstr, sizeof(ipstr));
        *clientip = ipstr;
        *clientport = ntohs(peer.sin_port);

       return newfd; 
    }

    bool Connnect(const std::string &ip, const uint16_t &port)
    {
        struct sockaddr_in peer;
        memset(&peer, 0, sizeof(peer));
        peer.sin_family =AF_INET;
        peer.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &(peer.sin_addr));

        int n = connect(_sockfd, (struct sockaddr*)&peer, sizeof(peer));
        if (n == -1)
        {
            std::cout << "connect to" << ip << " : " << port << " error "  << std::endl;
            return false;
        }
        return true;
    }

    void Close()
    {
        close(_sockfd);
    }

    int Fd()
    {
        return _sockfd;
    }
    
private:
    //文件描述符
    int _sockfd;  
};