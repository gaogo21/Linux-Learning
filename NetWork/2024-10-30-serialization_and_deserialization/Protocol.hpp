#pragma once

#include <iostream>
#include <string>
// #include <jsoncpp/json/json.h>

using namespace std;

const string blank_space_sep = " ";
const string protocol_sep = "\n";

//拼接完成之后的package："len"\n"x op y"\n
string Encode(string &content)
{
    //package表示内容的长度， x op y = 6, package = 6
    string package = to_string(content.size());
    package += protocol_sep;
    package += content;
    package += protocol_sep;

    return package;
}

//"len"\n"x op y"\nXXXXXXX
//
bool Decode(string &package, string *content)
{
    size_t pos = package.find(protocol_sep);
    if (pos == string::npos)
        return false;
    
    string len_str = package.substr(0, pos);
    size_t len = stoi(len_str);

    size_t total_len = len_str.size() + len + 2;
    if (package.size() < total_len)
        return false;
    
    *content = package.substr(pos + 1, len);
    package.erase(0, total_len);

    return true;
}

class Request
{
public:
    Request(int data1, int data2, char oper)
    :x(data1)
    ,y(data2)
    ,op(oper)
    {}

    Request()
    {}

public:
//序列化
    bool Serialize(string *out)
    {
//#ifdef MySelf
        //构建报文的有效载荷
        //struct => string "x op y"
        string s = to_string(x);
        s += blank_space_sep;
        s += op;
        s += blank_space_sep;
        s += to_string(y);
        *out = s;
        return true;
    }

    //反序列化
    bool Deserialize(const string &in)
    {
        size_t left = in.find(blank_space_sep);
        if (left == string::npos) 
            return false;
        string part_x = in.substr(0, left);

        size_t right = in.rfind(blank_space_sep);
        if (right == string::npos)
            return false;
        
        string part_y = in.substr(right + 1);

        if (left + 2 != right)
            return false;
        op = in[left + 1];
        x = stoi(part_x);
        y = stoi(part_y);
        return true;
    }

    void DebugPrint()
    {
        cout << "新请求构建完成： " << x << op << y << "=?" << endl;
    }
public:
    //x op y
    int x;
    int y;
    char op;
};

class Response
{
public:
    Response(int res, int c)
    :result(res)
    ,code(c)
    {}

    Response()
    {}

public:
    //序列化  拿到了计算的结果out,最终序列化为一个字符串:result code
    bool Serialize(string *out)
    {
        //"result code"
        //构建报文的有效载荷
        string s = to_string(result);
        s += blank_space_sep;
        s += to_string(code);
        *out = s;
        return true;
    }

    //反序列化 将拿到的result code,最终得到result
    bool Deserialize(const string &in)
    {
        size_t pos = in.find(blank_space_sep);
        if (pos == string::npos)
            return false;
        
        string part_left = in.substr(0, pos);   //result
        string part_right = in.substr(pos + 1);  //code

        result = stoi(part_left);
        code = stoi(part_right);

        return true;
    }

    void DebugPrint()
    {
        cout << "结果响应完成， result: " << result << ", code: " << code << endl;
    }

public:
    int result;
    int code;  //0：可信，否则!0具体是几，表明对应的错误原因
};