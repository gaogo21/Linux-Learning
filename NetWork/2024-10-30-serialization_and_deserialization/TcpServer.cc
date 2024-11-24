#include "TcpServer.hpp"

using namespace std;

void Usage(string proc)
{
    cout << "服务端使用说明 : " << proc << " port" << endl;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        return 0;
    }
    
    //argc[1]是string类型，serverIp是uint16_t类型，需要用atoi()转换
    uint16_t sercerIp = atoi(argv[1]);

    unique_ptr<TcpServer> tser(new TcpServer(sercerIp));
    tser->InitTcpServer();
    tser->Start();

    return 0;
}