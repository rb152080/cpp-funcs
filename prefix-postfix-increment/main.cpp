#include "increment.h"

int main(void)
{
    Increment i; // default is 0
    (i++).print();
    i.print();
    return 0;
}
