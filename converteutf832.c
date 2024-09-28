#include <stdio.h>
#include "converteutf832.h"

//conta a quantidade de bits 1 no byte
//tá funcionando
unsigned char countOneBits(unsigned char byte)
{
    unsigned char ct_bit = 0;
    unsigned char i = 7;
    while ((byte>>i)&1)
    {
        ct_bit++;
        i--;
    }
    return ct_bit;
    
}
/*
    Serve para pegar os bits que vamos utilizar, que é a partir depois do primeiro 0 
    depois dos bits 1 de observação
*/
unsigned char pegaBit(unsigned char caracter, unsigned char qtd_bytes)
{
    unsigned char novo_byte =  0;
    novo_byte = caracter & (0xff>>(qtd_bytes + 1));

    return novo_byte;

}

/*
    Recebe os bits que a gente já separou 
*/
unsigned int colocaBits(unsigned char bits, unsigned int caracter32, unsigned char qtd_bytes)
{
  for(unsigned char i = (8 - qtd_bytes - 1); i > 0; i--)
  {
    caracter32 = caracter32<<1;
    caracter32+=(bits>>(i-1))&1;
  }

  return caracter32;
  
}

/*
TO DO - Função para converter em little end
*/

int colocaInteiroNoArquivo(FILE* arquivo, unsigned int caracter32)
{
    int verifica_erro = 0;
    for(unsigned char i = 0; i < 4; i++)
    {
        verifica_erro = fputc((caracter32>>(i * 8)), arquivo);
    
        if(verifica_erro == EOF)
        {
            fputs("Nao foi possível escrever no arquivo\n", stderr);
            return -1;
        }

    }
    
    return 0;

}

int convUtf8p32(FILE *arquivo_entrada, FILE *arquivo_saida)
{
    unsigned char caracter = 0;
    unsigned char qtd_bytes = 0;
    unsigned char bits = 0;
    unsigned int caracter32 = 0xfffe0000;
    int verifica_erro = 0;
    while(1)
    {
        
        caracter = fgetc(arquivo_entrada);
        qtd_bytes = countOneBits(caracter);
        if(qtd_bytes!=1) //verificando se é ou não um byte de continuação
        {
            
            verifica_erro = colocaInteiroNoArquivo(arquivo_saida, caracter32);
            if(verifica_erro == -1) printf("Nao foi possível escrever no arquivo\n");
            caracter32 = 0;
        }
        
        bits = pegaBit(caracter, qtd_bytes);
        caracter32 = colocaBits(bits, caracter32, qtd_bytes);

        if(feof(arquivo_entrada)) break;

    }

    return 0;

}
