#include "Httpserver.hpp"
#include <iostream>
#include <memory>

using namespace std;

int main()
{
    //std::unique<HttpServer> svr(new HttpServer());
    HttpServer *svr = new HttpServer();
    svr->Start();

    return 0;
}