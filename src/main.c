#include "debug.h"

int main(void)
{
    SystemCoreClockUpdate();

    while (1)
    {
        Delay_Ms(1000);
    }
}
