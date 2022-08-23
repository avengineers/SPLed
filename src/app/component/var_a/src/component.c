#include "component.h"
#include "some_func.h"

int dummyInterface(void)
{
    return some_func(1);
}

int anotherDummyInterface(int in)
{
    if (in > 5)
    {
        return some_func(in);
    }
    else
    {
        return 3;
    }
}
