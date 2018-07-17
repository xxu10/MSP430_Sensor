#include<msp430x14x.h>
#include"uart.h"
typedef unsigned char uchar;
//Init UART
void UartInit()
{
  P3SEL |=0X30;        
  UCTL0 |=CHAR + SWRST;
  ME1 |=UTXE0;         
  UTCTL0 |=SSEL0;      
  UBR00=0X03;          
  UBR10=0;
  UMCTL0=0X4A;       
  UCTL0 &=~SWRST;     
}

void uartsend(uchar ptr)
{
    while((IFG1 & UTXIFG0)==0);
    TXBUF0=ptr;
 
}