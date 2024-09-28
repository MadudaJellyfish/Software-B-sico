#include <stdio.h>
#include "converteutf832.h"

int main(void)
{
    //hhu é o formato para usigned char
    //printf("qtd_bytes = %hhu\n", countOneBits(0b11000010));
    //unsigned char bits = pegaBit(0b11010010, 2);
    //printf("%u\n", colocaBits(bits, 6, 2));
    FILE* arq_In = fopen("utf8_peq.txt", "r");
    FILE* arq_Out = fopen("utf32_teste.txt", "w");
    convUtf8p32(arq_In, arq_Out);

    fclose(arq_In);
    fclose(arq_Out);

    return 0;
}