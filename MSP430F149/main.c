/****************************************************************************************

Function£º  ADC12 converts absorbance measured at P6.0, shows the results on Lcd1602 and send data to PC
    

Switch function£º  P1.0£º start and stop motor 
                  p1.1£º stop motor and get into low power consumptiom mode
                  Reset: reset the system

****************************************************************************************/
#include  <msp430x14x.h>
#include  <math.h>
#include  "cry1602.h"
#include  "ds18b20.h"
#include  "uart.h"
#include  "delay.h"

#define  Num_of_Results   128
#define  ulong unsigned long
#define  uchar unsigned char
#define  uint unsigned int

#define CPU_F 8000000
#define delay_us(x)  __delay_cycles(CPU_F*x/1000000)
#define delay_ms(x)  __delay_cycles(CPU_F*x/1000)
#define delay_s(x)   __delay_cycles(CPU_F*x)


uchar tishi[] = {"T:"};
uchar shuzi[]={"0123456789."};
uchar disp[7];
uint results1[Num_of_Results];       
uint results2[Num_of_Results];      


ulong Trans_Val(uint Hex_Val);     //convert absorbance to voltage
void  Display(long);               
void  Display_temp(unsigned long temp);
void  xgd_cl(void);                
void  xgd_js(void);                
void  Init_Xgd(void);              


uchar lpmflag=0;            
uchar cnt=0;
uint  flag2=0;

ulong sum0=1,sum1=1;   //sum0:Blue light£¬sum1:Red light
double Xgd;             


ulong temp=0;  
ulong a,t;               

/************************************************************************

Init clock and IO

*************************************************************************/
void Init()
{
  WDTCTL = WDTPW+WDTHOLD;         
  P1OUT =0;        
  P5OUT=0;         
  
  P5DIR =0X18;     
  P6SEL =0X01;     
  P6DIR &=~BIT0;   
  
  P1DIR |=BIT2;
  P1IE=0X03;      
  P1IES=0;        
  P1IFG=0;        
  
  BCSCTL1 &=~XT2OFF;
  do
  {
    uchar i;
    IFG1 &=~OFIFG;
    for(i=0xff;i>0;i--);
  }while(IFG1 & OFIFG);
  
  BCSCTL2 |=SELM_2 +SELS;
  
}

/********************************************************************

Init Timer A and Timer B

*********************************************************************/
void InitTimerA_B()
{
  TACTL = TASSEL_2 + ID_3 ;
  
  TBCTL  =TBSSEL_2 + ID_3;
  TBCCTL0=CCIE;
  TBCCR0 |=10000;
  
  
}
/**********************************************************************

Init ADC

***********************************************************************/
void InitAdc()
{

   ADC12CTL0 = ADC12ON+SHT0_8+MSC+REFON+REF2_5V; 
   ADC12CTL1 = SHP + CONSEQ_2;                   
   ADC12MCTL0 =SREF_1+INCH_0;                    
  // ADC12CTL0 |=ENC;                              
  // ADC12CTL0 |=ADC12SC;                          
}

/************************Main****************************/


void main(void)
{
 
  uchar i; 
 
  Init_Xgd();
  while(1)
  {
    temp=Do1Convert();           
    Display_temp(temp);
  
    xgd_cl();
    xgd_js();
    
    for(i=0;i<7;i++)           
      uartsend(disp[i]);
    
    if(lpmflag==1)            
    {
      lpmflag=0;
      LPM3;
    }
    delay_s(1);
  }
}



/******************************************************************************
P1 interrupt

*****************************************************************************/
#pragma  vector=PORT1_VECTOR
__interrupt void port1(void)
{
  LPM3_EXIT;                 
  if((P1IFG & BIT0)==BIT0)
  {
   
    P1IFG &=~BIT0;            
    P1OUT |=BIT2;            
    cnt++;
    if((cnt%2)==0)          
    {
        cnt=0;
        P1OUT &=~BIT2;
    }
   
 
 
  }
  if((P1IFG & BIT1)==BIT1)   
  {
    P1IFG &=~BIT1;           
    P1OUT &=~BIT2;           
    P5OUT &=~BIT3;           
    P5OUT &=~BIT4;           
    lpmflag=1;               
       
  }
  
}
/**********************************************************************
TimerB CCR0 interrupt

***********************************************************************/

#pragma vector=TIMERB0_VECTOR
__interrupt void timerb0(void)
{     
      
      flag2++;
     
}


/******************************************************************
Trans_val
Function£º Convert 0x ADC date to analog voltage 
Parameter£º Hex_Val--hexadecimal data
*********************************************************************/
ulong Trans_Val(uint Hex_Val)
{
    unsigned long caltmp;
    ulong Curr_Volt;

    
    caltmp = Hex_Val*25;                  
    
    Curr_Volt = (ulong)(1000*(caltmp /40950.0));  
  
    return (Curr_Volt);                   
 
}

void Display(long temp)
{
  disp[0]=temp/1000+0x30;      
  disp[1]=0x2e;                
  disp[2]=(temp%1000)/100+0x30;
  disp[3]=(temp%100)/10+0x30;  
  disp[4]=temp%10 + 0x30;      
  disp[5]=0x20;                
  disp[6]=0x20;                
  DispNChar(8,1,7,disp);       
  Disp1Char(6,1,'A');       
  Disp1Char(7,1,':');
}

void Display_temp(unsigned long temp)
{
  uchar i,tmp[4];
  temp=temp*625;      
  tmp[0]=temp/100000; 
  tmp[1]=(temp%100000)/10000;
  tmp[2]=10;          
  tmp[3]=(temp%10000)/1000;
 
  for(i=0;i<4;i++)
    Disp1Char(8+i,0,shuzi[tmp[i]]);
 }

/******************************************************************
Measure absorbance
******************************************************************/

void xgd_cl(void)
{
    uchar index=0;
    uchar i;
    
    P5OUT |=BIT3;           //Open blue LED
    delay_ms(100);     
    ADC12CTL0 |=ENC;        //Init ADC12
    ADC12CTL0 |=ADC12SC;
    
      
    for(i=0;i<128;i++)
    {
       while(!(ADC12IFG& BIT0));           
       results1[i] = ADC12MEM0;             
       index++;
    }
    ADC12CTL0 &=~ENC;                        //Stop ADC12
    ADC12CTL0 &=~ADC12SC;                   
    if(index == Num_of_Results)
    {
          sum0 = 0;
          index = 0;
          
          for(i = 0; i < Num_of_Results; i++)
          {
              sum0 += results1[i];
          }
          sum0 >>= 7;                            
      
             
     }   
 
    P5OUT &=~BIT3;                   //Close blue LED
    
    delay_s(1);
    P5OUT |=BIT4;                    //Init red LED
    delay_ms(100);
    ADC12CTL0 |=ENC;                 
    ADC12CTL0 |=ADC12SC;
    for(i=0;i<128;i++)              
    {
         while(!(ADC12IFG & BIT0));           
         results2[i] = ADC12MEM0;                
         index++;
    }
    ADC12CTL0 &=~ENC;                  //Stop ADC12
    ADC12CTL0 &=~ADC12SC;              
    if(index == Num_of_Results)
    {
         
          sum1 = 0;
          index=0; 
          for(i = 0; i < Num_of_Results; i++)
          {
              sum1 += results2[i];
          }
          sum1 >>= 7;                                                      
     } 
     P5OUT&=~BIT4;                               //Close red LED
}

/***************************************************************************
Calculate absorbance
*****************************************************************************/

void xgd_js(void)
{
  if(sum1>1&&sum0>1)
    {
      if(sum1<sum0)                     
      {
        t=sum1;
        sum1=sum0;
        sum0=t;
      }
      Xgd=(10000.0*sum1)/sum0;            
                                          
      a=(long)((log10(Xgd)-4)*1000);
      Display(a);                         
    }
    else
    {
      a=0;
      Display(a);
    }  
}

void Init_Xgd(void)
{
  Init();                
  InitTimerA_B();        
  UartInit();            //Init Uart
  InitAdc();             //Init ADC
  
  LcdReset();                       
  ClearPing();                      
  DispNChar(6,0,7,tishi);           
  temp=Do1Convert();                
  Display_temp(temp);               
  Disp1Char(12,0,'C');              
  Disp1Char(6,1,'A');               
  Disp1Char(7,1,':');
  a=0;
  Display(a);
  
  _EINT();                 
  
}
