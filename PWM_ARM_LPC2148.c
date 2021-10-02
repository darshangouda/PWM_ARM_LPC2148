///LPC2148 PWM2 ,PWM4, PWM6 and UART0   is used
#include <lpc214x.h>
#include<stdio.h>

int UART_RX(void);
void DISPLAY(char str[],char);
 
int flag=0,m1_speed=25,m2_speed=50,m3_speed=75;
char check[]={"0123456789"};

#define SBIT_CNTEN     0 
#define SBIT_PWMEN     3

#define SBIT_PWMMR0R   1

#define SBIT_LEN0      0
#define SBIT_LEN1      1
#define SBIT_LEN2      2
#define SBIT_LEN3      3
#define SBIT_LEN4      4
#define SBIT_LEN6      6

#define SBIT_PWMENA1   9
#define SBIT_PWMENA2   10
#define SBIT_PWMENA3   11
#define SBIT_PWMENA4   12
#define SBIT_PWMENA6   14

#define PWM1_PINSEL           0 //P0_0 (0-1   Bits of PINSEL0)
#define PWM2_PINSEL          14 //P0_7 (14-15 Bits of PINSEL0)
#define PWM3_PINSEL           2 //P0_2 (2-3   Bits of PINSEL0)
#define PWM4_PINSEL          16 //P0_8 (16-17 Bits of PINSEL0)
#define PWM6_PINSEL			 18

//PLL() 60Mhz set through proteus

void UART_INIT()
{
	PINSEL0 |=(1<<0)|(1<<2);
	U0LCR=(0X03<<0)|(1<<7); 
	U0DLL=97;  //97 
	U0DLM=0;	
	U0LCR &= ~(1<<7);
}

void  PWM_BASIC(void)
{	
    
    PINSEL0 |= (2<<PWM2_PINSEL) | (2<<PWM4_PINSEL) |  (2<<PWM6_PINSEL);  
    PWMPR  =  0x00;             
    PWMMCR = (1<<SBIT_PWMMR0R);  
    PWMMR0 = 100;
	PWMMR2 = m1_speed;               
    PWMMR4 = m2_speed;
	PWMMR6 = m3_speed;
	PWMTC  = 0x00;
    
	PWMPCR = (1<<SBIT_PWMENA2) | (1<<SBIT_PWMENA4) |  (1<<SBIT_PWMENA6);
    PWMTCR = (1<<SBIT_CNTEN) | (1<<SBIT_PWMEN); 
    PWMLER = (1<<SBIT_LEN0) | (1<<SBIT_LEN2) | (1<<SBIT_LEN4) | (1<<SBIT_LEN6); 
        
} 
void PWM_CALL(int op)
{
   	 switch(op)
	 {
	 case 1:PWMMR2 = m1_speed;PWMLER =  (1<<SBIT_LEN2);break;
	 case 2:PWMMR4 = m2_speed;PWMLER =  (1<<SBIT_LEN4);break;
	 case 3:PWMMR6 = m3_speed;PWMLER =  (1<<SBIT_LEN6);break;
	 }
}
void MENU()
{		char str[4];
        int flag,j,num,*motor_speed;
		char motor_text[]="SET Mn SPEED =";
        DISPLAY("SET MOTOR SPEED(press enter to skip)",1);
		DISPLAY("MOTOR SPEED M1=",0);
	    sprintf(str,"%d",m1_speed);
		DISPLAY(str,0);
		DISPLAY("   M2=",0);
	    sprintf(str,"%d",m2_speed);
		DISPLAY(str,0);
		DISPLAY("   M3=",0);
	    sprintf(str,"%d",m3_speed);
		DISPLAY(str,1);
		while(1)
		{
		for(j=1;j<=3;j++)
		{ 
		  if(j==1){ motor_speed=&m1_speed; motor_text[5]='1'; };
		  if(j==2){ motor_speed=&m2_speed; motor_text[5]='2'; };
		  if(j==3){ motor_speed=&m3_speed; motor_text[5]='3'; };
		  flag=1;
		  do{
	        DISPLAY(motor_text,0);
		    num=UART_RX();
		    
			if(num==-2)
			{
			 DISPLAY("PREVIOUS VALUE SET ->",0);
			 sprintf(str,"%d",*motor_speed);
		    DISPLAY(str,1);
			flag=0;
			}
		    else if(num<=99 && num>=0)
		    {
		    *motor_speed=num;
		    PWM_CALL(j);
		    flag=0;
		    }
		    else
		    {
		    DISPLAY("ENTER VALID NUMBER (0 to 99)",1);
		    }
		   }while(flag);
		  }//for end
	    }
}
int GETNUM(char str[4],int len)
{
	int i,num=0;
	for(i=0;i<len;i++)
	num=(num*10)+(str[i]-'0');
	return num;
}
int UART_RX()
{			   
      unsigned char temp;
	  int i=0,j,flag=0;
	   
	 char str[4]={"000"};
	do
	{   
		while((U0LSR & 0x01) == 0);
		temp=U0RBR;
		if(temp==13 && i==0)return -2;
		if(temp==13)return GETNUM(str,i);
		flag=0;
		for(j=0;j<10;j++)
		     if(temp==check[j]) flag++;
		if(flag!=1) return -1;
		str[i++]=temp;
		if(i==3)return -1;
	}while(1); 
}
void UART_TX(unsigned char var)
{
	  while( (U0LSR & 0x40) == 0 );
      U0THR=var;
}
void DISPLAY(char str[],char newline)
{
 int i;
 for(i=0;str[i]!='\0';i++)
 {
   UART_TX(str[i]);
  }
  if(newline==1)
    UART_TX(13);
}
void delay_(void)
{	int i,j;
   for(i=0;i<10000;i++)
   for(j=0;j<100;j++);
}

main()
{
UART_INIT();
PWM_BASIC();       // this file
PWM_CALL(1);
PWM_CALL(2);
PWM_CALL(3);
MENU();	 
}
