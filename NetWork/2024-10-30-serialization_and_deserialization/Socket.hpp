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

using namespace std;

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
            cout << "socker error" << endl;
            exit(SocketErr);
        }
    }

    void Bind(uint16_t port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;

        if (bind(_sockfd, (struct sockaddr *) &local, sizeof(local)) < 0)
        {
            cout << "bind error" << endl;
            exit(BindErr);
        }
    }    

    void Listen()
    {
        if (listen(_sockfd, backlog) < 0)
        {
            cout << "listen error" << endl;
            exit(ListenErr);
        }
    }

    int Accept(std::string *clientip, uint16_t *clientport)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int newfd = accept(_sockfd, (struct sockaddr*) &peer, &len);
        if (newfd < 0)
        {
            cout << "accpet error" << endl;
            return -1;
        }

        char ipstr[64];
        inet_ntop(AF_INET, &peer.sin_addr, ipstr, sizeof(ipstr));
        *clientip = ipstr;
        *clientport = ntohs(peer.sin_port);

        return newfd;
    }

    bool Connect(const string &ip, const uint16_t &port)
    {
        struct sockaddr_in peer;
        memset(&peer, 0, sizeof(peer));
        peer.sin_family = AF_INET;
        peer.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &(peer.sin_addr));

        int n = connect(_sockfd, (struct sockaddr*)&peer, sizeof(peer));

        if (n == -1)
        {
            cout << "connect to " << ip << ":" << port << "error" << endl;
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
    int _sockfd;
};