#include "UdpServer.hpp"
#include <memory>
#include <cstdio>

using namespace std;

void Usage(string proc)
{
    cout << "使用方法 : " << proc << "prot" << endl;
}

enum
{
    USE_ERROR = 1,
};

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USE_ERROR);
    }

    uint16_t port = atoi(argv[1]);
    unique_ptr<udpserver> userv(new udpserver(port));

    userv->initServer();
    userv->run();

    return 0;
}
