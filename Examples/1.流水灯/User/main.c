#include <intrins.h>
#include <reg52.h>

unsigned char tab[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF,
                       0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD};

void Delayms(unsigned int xms);

void main()
{
    unsigned char i;
    while (1)
    {
        for (i = 0; i < sizeof(tab); i++)
        {
            P1 = tab[i];
            Delayms(200);
        }
    }
}

void Delayms(unsigned int xms) //@12.000MHz
{
    unsigned char i, j;
    while (xms)
    {
        i = 2;
        j = 239;
        do
        {
            while (--j)
                ;
        } while (--i);
        xms--;
    }
}
