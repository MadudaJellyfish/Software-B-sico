/* Maria Eduarda da Fonseca  2212985 33WB */
/* Mayara Ramos Damazio 2210833 33WC */

#include "converteutf832.h"

//FUNCOES PARA 32 - 8

/* log2C
*   -> Calcula o valor do log de algum número
*/
unsigned char log2C(unsigned int caractere32)
{
    char resultado  = 0;
    while(caractere32 > 1)
    {
        caractere32 >>= 1;
        resultado +=1;
    }
    return resultado;
}

/* inverteNum
*   -> Inverte os bytes de um inteiro
*/
unsigned int inverteNum(unsigned int caractere32)
{
    unsigned int aux = 0;
    for (unsigned char i = 0; i < 4; i++)
    {
        aux = aux << 8;
        aux += (caractere32 >> (i * 8)) & 0xff;
    }
    return aux;
}

/* contaBytes
*   -> Acha a quantidade de bytes que um caractere no utf-32 tem em utf-8
*/
unsigned char contaBytes(unsigned int caractere32)
{
    unsigned char qtdBytes = 0;
    unsigned char expoente = log2C(caractere32);
    if (expoente < 7)
    {
        qtdBytes = 1;
    }
    else if (expoente < 11)
    {
        qtdBytes = 2;
    }
    else if (expoente < 16)
    {
        qtdBytes = 3;
    }
    else{
        qtdBytes = 4;
    }
    return qtdBytes;
}

/* colocaByteNoArquivo
*   -> Coloca um Byte no arquivo e trata se der erro
*/
int colocaByteNoArquivo(unsigned char byte8, FILE* arqOut)
{
    int verificaErro = fputc(byte8, arqOut);
    if (verificaErro == EOF)
    {
        fputs("Erro ao inserir no arquivo\n", stderr);
        return -1;
    }
    return 0;
}

/* montaEscreveBytes
*   -> Com a quantidade de bytes que o caractere em utf-8 precisará ter e o inteiro que 
*      representa o caractere em utf-32, ele monta byte a byte pegando o bits específicos 
*      para montar um byte seguindo a converção utf-8
*/
int montaEscreveBytes(unsigned char qtdBytes, unsigned caractere32, FILE* arqOut)
{
    unsigned char byte8 = 0xff;
    unsigned aux = 0;
    int verificaErro = 0;
    if (qtdBytes == 1)
    {
        byte8 = (byte8 << 8) + caractere32;
        verificaErro = colocaByteNoArquivo(byte8, arqOut);
    }
    else
    {
        for (char i = qtdBytes; i >= 0; i--)
        {
            if (i == qtdBytes)
            {
                byte8 = byte8 << (8 - qtdBytes);
                aux = caractere32 >> (6* (qtdBytes -1) );
                byte8 += aux;            
            } 
            else if ( i == qtdBytes -1)
                continue;
            else
            {
                byte8 = byte8 << 7;

                byte8 += (caractere32 >> (6 * i)) & 0x3f;
            }
            
            verificaErro = colocaByteNoArquivo(byte8, arqOut);
            if(verificaErro)
            {
                return verificaErro;
            }
            byte8 = 0xff;
        }
    }
    return verificaErro;
}


//FUNCOES PARA A 8 - 32

/* contaQtdBytes
*   -> Conta a quantidade de bytes que o caractere em utf-8 tem
*      a partir da quantidade de bits 1 antes do primeiro 0
*/
unsigned char contaQtdBytes(unsigned char byte)
{
    unsigned char cont = 0;
    unsigned char i = 7;
    while((byte>>i) & 1)
    {
        cont++;
        i--;
    }
    return cont;
}

/* pegaBits
*   -> Retorna o char com apenas a parte mais significativa que tem 
*      os bits que serao utilizados
*/
unsigned char pegaBits(unsigned char caractere, unsigned char qtdBytes)
{
    unsigned char novoByte = 0;
    novoByte = caractere & (0xff >> (qtdBytes + 1));
    return novoByte;
}

/* colocaBits
*   -> Coloca bit a bit os bits do char no inteiro que guardara o valor 
*      do caractere em utf-32
*/
unsigned int colocaBits(unsigned char bits, unsigned int caractere32, unsigned char qtdBytes)
{
    for (unsigned char i = (8-qtdBytes - 1); i > 0; i--)
    {
        caractere32 = caractere32 << 1;
        caractere32 += (bits >> (i-1)) & 1;
    }
    return caractere32;
}

/* colocaInteiroNoArquivo
*   -> Coloca o inteiro no arquivo e trata se der erro
*/
int colocaInteiroNoArquivo(FILE *arqOut, int caractere32)
{
    int verificaErro = 0;
    for(unsigned char i = 0; i < 4; i++)
    {
        verificaErro = fputc((caractere32 >> i * 8), arqOut);
        if (verificaErro == EOF)
        {
            fputs("Erro ao inserir no arquivo", stderr);
            return -1;
        }
    }
    
    return 0;
}

/* convUtf32p8
*   -> Converte um arquivo escrito em utf-32 para utf-8. 
*      Trata se o código estiver em Big Endian.
*/
int convUtf32p8(FILE *arquivo_entrada, FILE *arquivo_saida)
{
    unsigned int caractere32 = 0;
    unsigned char qtdBytes = 0;
    int verificaEndian = 0; //se e littleEndian, é 0. Senão, é BigEndian
    int verificaErro = 0;
    char ePrimeira = 0;
    while (fread(&caractere32, sizeof(unsigned int), 1, arquivo_entrada) == 1)
    {
        if (!ePrimeira)
        {
            if (caractere32 == 0xfeff || caractere32 == 0xfffe0000)
            {
                if (caractere32 == 0xfffe0000)
                {
                    verificaEndian = 1;
                }
                ePrimeira++;
            }       
            else
            {
                return -1;
            }     
        }
        else{
            if (verificaEndian){
                caractere32 = inverteNum(caractere32);
            }
            qtdBytes = contaBytes(caractere32);
            verificaErro = montaEscreveBytes(qtdBytes, caractere32, arquivo_saida);
            if(verificaErro)
                return verificaErro;  
            }
    }
    return 0;
}

/* convUtf32p8
*   -> Converte um arquivo escrito em utf-8 para utf-32. 
*/
int convUtf8p32(FILE *arquivo_entrada, FILE *arquivo_saida)
{
    unsigned char caracter = 0;
    unsigned char qtdBytes = 0;
    unsigned char bits = 0;
    unsigned int caractere32 = 0xfeff;
    int verificaErro = 0;
    while(1)
    {
        caracter = fgetc(arquivo_entrada);
        
        qtdBytes = contaQtdBytes(caracter);
        if (qtdBytes != 1)
        {
            verificaErro = colocaInteiroNoArquivo(arquivo_saida, caractere32);
            if (verificaErro)
            {
                return verificaErro;
            }
            caractere32 = 0;
        }
        if(feof(arquivo_entrada))
        {
            break;
        }        
        bits = pegaBits(caracter, qtdBytes);
        caractere32 = colocaBits(bits, caractere32, qtdBytes);
    }
    return 0;
}
