#include "TcpClient.hpp"
using namespace std;

void Usage(string proc)
{
    cout << "客户端使用说明 : " << proc << " ip " << "port" << endl;
}

//输入是 ： ./proc ip port
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        return 0;
    }
    
    string ip = argv[1];
    uint16_t port = atoi(argv[2]);

    unique_ptr<TcpClient> tclient(new TcpClient(ip, port));

    tclient->InitTcpClient();
    tclient->Start();

    return 0;
}