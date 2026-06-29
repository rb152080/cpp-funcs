#include <iostream>

void foo()
{
    // static doesn't live in the stack or heap; it is with the executable
    // static variable gets defined once during runtime in the function foo and
    // lives in that function only
    static int s_variable = 0;
    s_variable++;
    std::cout << s_variable << "\n";
}

int main()
{
    for (int i = 0; i < 10; i++)
        foo();
    // s_variable = 10; // can't do this
    return 0;
}
