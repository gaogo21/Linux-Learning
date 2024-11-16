#include "head.h"

using namespace std;

int main()
{
    int x = 5, y = 5;
    cout << "x + y = " << Add(x, y) << endl;

    cout << "x * y = " << Mul(x, y) << endl;

    cout << "x - y = " << Sub(x, y) << endl;

    cout << "x / y = " << Div(x, y) << endl;
}