#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

void Usage(string proc)
{
    cout << "使用说明:" << proc << " clientip clientport" << endl;
}


//用户输入，所以第二个参数就是ip地址，第三个参数就是port
//用户输入访问的是服务端的ip地址和端口号，就像我们访问浏览器一样

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(0);
    }

    //获取server的ip和port --> 通过客户输入得到的
    string serverip = argv[1];
    uint16_t serverport = stoi(argv[2]);

    struct sockaddr_in server;
    bzero(&server, sizeof(server));

    //填充套接字结构体
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(serverip.c_str());
    server.sin_port = htons(serverport);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        cout << "socker error" << endl;
        return 1;
    }
    cout << "serverport: " << serverport << endl;
    
    //client需要bind,但是不需要用户自己显式的bind,由os自主选择。只要保证了client的唯一性就行了
    
    string message;
    char buffer[1024];

    while(true)
    {
        cout << "please enter@ ";
        getline(cin, message);
        
        sendto(sockfd, message.c_str(), message.size(), 0, (const struct sockaddr*)&server, sizeof(server));
    }

    close(sockfd);
    return 0;
}