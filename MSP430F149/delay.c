#include<msp430x14x.h>
#include"delay.h"

#define uint unsigned int
/************************************************************
   DelayNus  
   Function: delay N us
   CPU Frequency: 8MHZ, Timer A frequency: 1MHZ(1/8)
************************************************************/

void DelayNus(uint n)
{
  CCR0 =n;
  TACTL |=MC_1 + TACLR;            
  while(!(TACTL & BIT0));  //Wait TAIFG
  TACTL &=~MC_1;          //Stop
  TACTL &=~BIT0;         
}

/*************************************************************

   Delay ms

*************************************************************/

void DelayNms(uint n)
{
  while(n--)
  {
    DelayNus(1000);
  }
}