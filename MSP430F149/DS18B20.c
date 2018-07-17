#include<msp430x14x.h>
#include"ds18b20.h"
#include"delay.h"


#define DQ1 P2OUT |=BIT0;
#define DQ0 P2OUT &=~BIT0;
#define DQ_in P2DIR &=~BIT0;
#define DQ_out P2DIR |=BIT0;
#define DQ_val (P2IN & BIT0)



/*************************************************************
  Init DS18B20 

*************************************************************/

uchar Init_18B20(void)
{
  uchar error;
  DQ_out;
 // _DINT();
  DQ0;
  DelayNus(499);
  DQ1;
  DelayNus(54);
  DQ_in;
  _NOP();
  if(DQ_val)
    error=1;     
  else
    error=0;    
  DQ_out;
  DQ1;
  
  DelayNus(399);
 // _EINT();
  
  return error;
}


/***********************************************************
Write_18B20 
Function: write data into DS18B20
Parameter: wdata--data you want write
***********************************************************/

void Write_18B20(uchar wdata)
{
  uchar i;
  for(i=0;i<8;i++)
  {
    DQ0;                      
    DelayNus(6);               
    if(wdata & 0x01)          
    {
      DQ1;
    }
    else 
    {
      DQ0;
    }
    wdata>>=1;                
    DelayNus(49);            
    DQ1;                     
    DelayNus(9);         
        
  }
  
}

/************************************************
Read_18B20
Function: read data from DS18B20
************************************************/

uchar Read_18B20(void)
{
  uchar i;
  uchar temp=0;
  for(i=0;i<8;i++)
  {
    temp>>=1;      
    DQ0;          
    DelayNus(5);  
    DQ1;          
    DelayNus(7);  
    DQ_in;       
    _NOP();
    if(DQ_val)        
      temp |=0x80;
    DelayNus(44);    
    DQ_out;         
    DQ1;            
    DelayNus(9); 
  }
  
  return temp;
}

/******************************************************************

  Skip reading Product ID process

******************************************************************/

void Skip(void)
{
  Write_18B20(0xcc);
  
}

/***********************************************************
convert
Function: send temperature-convert command

***********************************************************/
void Convert(void)
{
  
  Write_18B20(0x44);
  
}

/**********************************************************
Read_SP
Function: send scratchpad £¨read high-speed cache command£© command

**********************************************************/
void Read_SP(void)
{
  Write_18B20(0Xbe);
  
}


/*********************************************************
ReadTemp
Function: read convert results from scratchpad in DS18B20

**********************************************************/
uint ReadTemp(void)
{
  uchar temp_low;
  uint temp;
  temp_low=Read_18B20();  
  temp=Read_18B20();      
  temp = (temp<<8)|temp_low;
  
  return temp;
}

/*****************************************************************
ReadTemp: DS18B20 measures temperature and return results

******************************************************************/

uint  Do1Convert(void)
{
  uchar i;
  do
  {
    i=Init_18B20();
  }
  while(i);
  Skip();
  Convert();
  for(i=20;i>0;i--)
  DelayNus(60000);
  
  
  do
  {
    i=Init_18B20();
  }
  while(i);  
  Skip();
  Read_SP();
  return ReadTemp(); 
  
}



