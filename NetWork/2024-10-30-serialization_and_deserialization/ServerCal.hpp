#pragma once
#include <iostream>
#include "Protocol.hpp"

using namespace std;

enum
{
    Div_Zero = 1,
    Mod_Zero,
    Other_Oper
};

class ServerCal
{
public:
    ServerCal()
    {}

    //根据Request的三个值，计算出最终的result
    Response CalculatorHelper(const Request &req)
    {
        //构造
        Response resp(0, 0);
        switch(req.op)
        {
        case '+':
            resp.result = req.x + req.y;
            break;
        case '-':
            resp.result = req.x - req.y;
            break;
        case '*':
            resp.result = req.x * req.y;
            break;
        case '/':
        {
            if (req.y == 0)
                resp.code = Div_Zero;
            else
                resp.result = req.x / req.y;
        }
        break;
        case '%':
        {
            if (req.y == 0)
                resp.result = Mod_Zero;
            else
                resp.result = req.x % req.y;
        }
        break;
        default:
            resp.code = Other_Oper;
            break;
        }

        return resp;
    }

    string Calculator(string &package)
    {
        string content;
        bool r = Decode(package, &content);  //"len"\n"10 + 20"\n
        if (!r)
            return "";
        
        Request req;
        r = req.Deserialize(content);  //"10 + 20" --> x = 10, op = +, y = 20
        if (!r)
            return "";
        
        content = "";
        Response resp = CalculatorHelper(req);  //result = 30, code = 0
        content = Encode(content);  //"len"\n"30 0"

        return content;
    }

    ~ServerCal()
    {}
};