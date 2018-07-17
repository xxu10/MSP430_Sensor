#include <msp430x14x.h>
#include "cry1602.h"
typedef unsigned char uchar;
typedef unsigned int  uint;

#define DataDir     P4DIR
#define DataPort    P4OUT				
#define Busy	    0x80
#define CtrlDir     P3DIR
#define CLR_RS P3OUT&=~BIT0;    //RS = P3.0	
#define SET_RS P3OUT|=BIT0;	
#define CLR_RW P3OUT&=~BIT1;	//RW = P3.1
#define SET_RW P3OUT|=BIT1;	
#define CLR_EN P3OUT&=~BIT2;	//EN = P3.2	
#define SET_EN P3OUT|=BIT2;	
/*******************************************
DispNchar
Function£º Display n characters from location(x,y)
Parameter£º x--the col 
           y--the row
           n--the number of characters
           ptr--the pointer to characters
********************************************/
void DispNChar(uchar x,uchar y, uchar n,uchar *ptr) 
{
    uchar i;
    
	for (i=0;i<n;i++)
	{
		Disp1Char(x++,y,ptr[i]);
		if (x == 0x0f)
		{
		   x = 0; 
                   y ^= 1;
		}
	}
}
/*******************************************
LocateXY
Function£º Give the location information to Lcd
Parameter£º x--the col
           y--the row
********************************************/
void LocateXY(uchar x,uchar y) 
{
        uchar temp;

	temp = x&0x0f;
	y &= 0x01;
	if(y)   temp |= 0x40;  //the second row
	temp |= 0x80;
    
	LcdWriteCommand(temp,1);
}
/*******************************************
Disp1Char
Function£º Display 1 character from location(x,y)
Parameter£º x--the col 
           y--the row
           data--the character which you want to display
********************************************/
void Disp1Char(uchar x,uchar y,uchar data) 
{
	LocateXY( x, y );			
	LcdWriteData( data );		
}
/*******************************************
LcdReset
Function£º Reset Lcd
********************************************/
void LcdReset(void) 
{
        CtrlDir |= 0x07;                 //Output state
        DataDir  = 0xFF;                 //Output state
  
   	LcdWriteCommand(0x38, 0);	    
	Delay5ms();
	LcdWriteCommand(0x38, 0);		
	Delay5ms();
	LcdWriteCommand(0x38, 0);
	Delay5ms();

	LcdWriteCommand(0x38, 1);		
        LcdWriteCommand(0x08, 1);		
   	LcdWriteCommand(0x01, 1);	    
	LcdWriteCommand(0x06, 1);		
	LcdWriteCommand(0x0c, 1);		
}
/*******************************************
LcdWriteCommand
Function£º Write command into Lcd
Parameter£º cmd--command£¬
           chk--Busy or Idle£¬1£ºBusy£¬0£ºIdle
********************************************/
void LcdWriteCommand(uchar cmd,uchar chk) 
{

	if (chk) WaitForEnable();   
	
	CLR_RS;	
	CLR_RW; 
	_NOP();

	DataPort = cmd;             
	_NOP();					
	
	SET_EN;                     
	_NOP();
	_NOP();
	CLR_EN;			
}
	
/*******************************************
LcdWriteData
Function£º Write data into Lcd at current address
Parameter£º data--characters you want to display
********************************************/
void LcdWriteData( uchar data ) 
{
	WaitForEnable();        

	SET_RS;
	CLR_RW; 
	_NOP();

    DataPort = data;       
    _NOP();

	SET_EN;                
	_NOP(); 
	_NOP(); 
	CLR_EN;		
}
/*******************************************
WaitForEnable
Function£º wait for the Lcd
********************************************/
void WaitForEnable(void) 
{
    P4DIR &= 0x00;  //Input state

	CLR_RS;
	SET_RW;
	_NOP();
	SET_EN; 
	_NOP();
	_NOP();
	
	while((P4IN & Busy)!=0);  
	CLR_EN;
    
	P4DIR |= 0xFF;  //Output state
}	
                       
/*******************************************
Delay5ms
Function£º delay 5ms
********************************************/
void Delay5ms(void)
{ 
    uint i=40000;
    while (i != 0)
    {
        i--;
    }
}
/*******************************************
Delay400ms
Function: delay 400ms
********************************************/
void Delay400ms(void)
{
    uchar i=50;
	uint j;
    
	while(i--)
	{
		j=7269;
		while(j--);
	}
}


/*********************************************************
Clear Lcd
*********************************************************/

void ClearPing()
{
  
  unsigned char i;
  for(i=0;i<16;i++)
  {
    Disp1Char(i,0,0x20);
    Disp1Char(i,1,0x20);
  }
}