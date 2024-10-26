#include <iostream>
#include <string>

std::string opers = "+-*%";

enum
{
    DivZero = 1,
    ModZero,
    Unkown
};

class Task
{
public:
    Task(int x1, int x2, char oper)
    :_data1(x1)
    ,_data2(x2)
    ,_oper(oper)
    ,_result(0)
    ,_exitcode(0)
    {}

    void run()
    {
        switch(_oper)
        {
        case '+':
            _result = _data1 + _data2;
            break;
        
        case '-':
            _result = _data1 - _data2;
            break;
        
        case '*':
            _result = _data1 * _data2;
            break;
        
        case '/':
            if (_data2 == 0)
                _exitcode = DivZero;
            else
                _result = _data1 / _data2;
            break;
        case '%':
            if (_data2 == 0)
                _exitcode = ModZero;
            else
                _result = _data1 % _data2;
            break;
        default:
            _exitcode = Unkown;
            break;
        }
    }
    

    //重载operator()
    void operator()()
    {
        run();
    }

    std::string GetTask()
    {
        std::string r = std::to_string(_data1);
        r += _oper;
        r += std::to_string(_data2);
        r += "= ?";

        return r;
    }

    std::string GetResult()
    {
        std::string r = std::to_string(_data1);
        r += _oper;
        r += std::to_string(_data2);
        r += "= ";
        r += std::to_string(_result);
        r += "[code: ";
        r += std::to_string(_exitcode);
        r += "]";

        return r;
    }

    ~Task()
    {}

private:
    int _data1;
    int _data2;
    char _oper;

    int _result;
    int _exitcode;
};