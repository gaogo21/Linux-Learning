#pragma once

#include <iostream>
#include <pthread.h>
#include "Socket.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>


static const int defaultport = 8085;

const std::string seq = "\r\n";
const std::string wwwroot = "./wwwroot";
const std::string homepage = "index.html";

class HttpServer;

class ThreadData
{
public:
    ThreadData(int fd, HttpServer *ts)
        :sockfd(fd)
        ,httpsvr(ts)
    {}
public:
    int sockfd;
    HttpServer *httpsvr;
};

class HttpRequest
{
public:

    //反序列化 -- 将从客户端读到的http请求push_back进req_header中
    void Deserialize(std::string req)
    {
        while(true)
        {
            ssize_t pos = req.find(seq);
            if (pos == std::string::npos)
                break;
            
            std::string temp = req.substr(0, pos);
            if (temp.empty())
                break;
            
            req_header.push_back(temp);
            req.erase(0, pos + seq.size());
        }

        //req去掉前面的内容之后，剩下的全是文本
        text = req;
    }
    
    void DebugPrint()
    {
        for (auto &line : req_header)
        {
            std::cout << "-------------------" << std::endl;
            std::cout << line << "\n\n";
        }

        std::cout << "method : " << method << std::endl;
        std::cout << "url : " << url << std::endl;
        std::cout << "http version : " << http_version << std::endl;
        std::cout << "file path : " << file_path << std::endl;
        std::cout << "text : " << text << std::endl;
    }

    void Parse()
    {
        std::stringstream ss(req_header[0]);
        ss >> method >> url >> http_version;   //用stringstream将method等直接分割了
        file_path = wwwroot;
        if (url == "/" || url == "/index.html")  //根目录
        {
            file_path += "/";
            file_path += homepage;
        }
        else
        {
            file_path += url;
        }

        // auto pos = file_path.rfind(".");
        // if (pos == std::string::npos)

    }
public:
    std::vector<std::string> req_header;  //请求
    std::string text;

    //解析之后的结果  --> 这是http的请求报文的格式
    std::string method;
    std::string url;
    std::string http_version;
    std::string file_path;
};

class HttpServer
{
public:
    HttpServer(int port = defaultport)
    :_port(defaultport)
    {}

    bool Start()
    {
        //创建套接字
        _listensock.Socket();
        _listensock.Bind(_port);
        _listensock.Listen();

        for (;;)
        {
            std::string clientip;
            uint16_t clientport;
            int sockfd = _listensock.Accept(&clientip, &clientport);

            pthread_t tid;
            ThreadData *td = new ThreadData(sockfd, this);
            td->sockfd = sockfd;
            pthread_create(&tid, nullptr, ThreadRun, td);
        }
    }   

    static void* ThreadRun(void* args)
    {
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData*>(args);
        // char buffer[10240];
        // //ssize_t n = read(?; buffer, sizeof(buffer - 1));  可以使用read
        
        // //也可以使用recv
        // ssize_t n = recv(td->sockfd, buffer, sizeof(buffer - 1), 0);
        // if (n > 0)
        // {
        //     buffer[n] = 0;
        //     std::cout << buffer;
        // }

        HandlerHttp(td->sockfd, td->httpsvr);
        delete td;
        return nullptr;
    }

    //固定版本
    // static void HanderHttp(int sockfd, HttpServer *httpsvr)
    // {
    //     char buffer[10240];
    //     ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);   //在这里，不能保证读到了完整的http请求
    //     if (n > 0)
    //     {
    //         buffer[n] = 0;
    //         std::cout << buffer;

    //         //返回相应的过程
    //         std::string text = "mayue is a pig! xixi~";
    //         std::string response_lines = "HTTP/1.0 200 OK\r\n";
    //         std::string response_header = "Content-Length: ";
    //         response_header += std::to_string(text.size());
    //         response_header += "\r\n";
    //         std::string blank_lines = "\r\n";

    //         std::string response = response_lines;
    //         response += response_header;
    //         response += blank_lines;
    //         response += text;

    //         //将内容发送给发送方  -- 响应
    //         send(sockfd, response.c_str(), response.size(), 0);
    //     }
        
    //     close(sockfd);
    // }

    //读取文件内容
    static std::string ReadHtmlContent(const std::string &htmlpath)
    {
        std::ifstream in(htmlpath, std::ios::binary);
        if (!in.is_open())
            return "";
        
        in.seekg(0, std::ios_base::end);
        auto len = in.tellg();
        in.seekg(0, std::ios_base::beg);

        std::string content;
        content.resize(len);

        in.read((char*)content.c_str(), content.size());

        in.close();

        return content;
    }

    //显示不同的html，进行处理
    static void HandlerHttp(int sockfd, HttpServer *httpsvr)
    {
        char buffer[10240];
        ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);   //在这里，不能保证读到了完整的http请求
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << buffer << std::endl;  //假设我们读到的是一个完整的http请求

            HttpRequest req;
            req.Deserialize(buffer);
            req.Parse();

            std::string text;
            bool ok = true; 
            text = ReadHtmlContent(req.file_path);  //读取客户端请求的文件 --> "./index.html"

            if (text.empty())
            {
                ok = false;
                std::string err_html = wwwroot;
                err_html += "/";
                err_html += "err.html";
                text = ReadHtmlContent(err_html);
            }
            
            //返回响应的过程
            std::string response_lines;
            if (ok)
            {
                response_lines = "HTTP/1.0 200 OK\r\n";
            }
            else 
            {
                response_lines = "HTTP/1.0 404 Not Found\r\n";
            }

            std::string response_header = "Content-Length: ";
            response_header += std::to_string(text.size());
            response_header += "\r\n";
            response_header += "Content-Type: text/html\r\n";
            response_header += "\r\n";
            response_header += "Set-Cookie : name=haha&&passds=12345";
            response_header += "\r\n";
            
            std::string blank_lines = "\r\n";

            std::string response = response_lines;
            response += response_header;
            response += blank_lines;
            response += text;

            //将内容发送给客户端  -- 响应
            send(sockfd, response.c_str(), response.size(), 0);
        }
        
        close(sockfd);
    }

    ~HttpServer()
    {}

private:
    Sock _listensock;
    uint16_t _port;
};