#pragma once

#include <iostream>
#include <string>
// #include <jsoncpp/json/json.h>

using namespace std;

const string blank_space_sep = " ";
const string protocol_sep = "\n";

string Encode(string &content)
{
    string package = to_string(content.size());
    package += protocol_sep;
    package += content;
    package += protocol_sep;

    return package;
}

bool Encode(string &package, string *content)
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

