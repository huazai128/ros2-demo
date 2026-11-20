#include "stdio.h"

#define N 2

void main()
{
    float w1[] = {1.0, 2.0};
    unsigned char *p = (char*)w1;
    for (int i = 0; i < sizeof(float) * N; i++)
    {
        printf("0x%2x,", p[i]);
    }
}