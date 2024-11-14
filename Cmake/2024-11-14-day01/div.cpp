#include "head.h"

int Div(int x, int y)
{
    if (y == 0)
        std::cout << "被除数为0，错误" << std::endl;
    return x / y;
}