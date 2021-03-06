#include "stm32f10x.h"
#include "stm32f10x_HC-SR04.h"
#include "lcd.h"
#include <stdio.h>
#include "bsp_beep.h"
#include "timer.h"
#include "string.h"
#include "usart.h"

#define Buf1_Max 60
#define Buf2_Max 200 

void GPIO_Config_LED(void);
void displayMenu(char *,char *,char *, char *,char *, char *);
void setSafeDist(void);
void scanPassThrough(void);
void alert(int alertSpeed);
void importData(void);
void exportData(void);

void Delayus(int duration);
void CLR_Buf2(void);
void Second_AT_Command(char *b,char *a,u8 wait_time);
void Set_Text_Mode(void);
void Send_Text_Sms(char*);
u8 Find(char *a);

GPIO_InitTypeDef GPIO_InitStructure;
//GSM
char *phone = "ATD66800613;\r\n";
//SMS
static unsigned char *SMS_Num="\"+85292040031\"";//短信中心号 关于中心号的介绍可以看串口调试笔记的第二部分
static unsigned char *phone_sms="\"93862966\"";     //短信接收方手机号
static unsigned char *content="ELEC3300 I LOVE U!!";//发送短信内容

char Uart2_Buf[Buf2_Max];
u8 First_Int = 0;
u8 Times = 0;
u8 shijian = 0;

vu8 Timer0_start;
vu8 Uart2_Start;
vu8 Uart2_End;

u8  find_string_flag = 0;
u8  has_call_flag = 0;

int32_t dist = 0;	
int32_t dist2 = 0;
int32_t dist3 = 0;

int32_t safeDist_S1 = -1;	
int32_t safeDist_S2 = -1;	
int32_t safeDist_S3 = -1;	

int32_t warnTime = 0x0000;
int32_t trigTime = 0x0000;


int32_t S1Pass= 0;
int32_t S1Inside = 0;

int32_t S2Pass= 0;
int32_t S2Inside = 0;

int32_t S3Pass= 0;
int32_t S3Inside = 0;

int32_t boolImport = 0; //indicate whether import function is called

int main(void)
{	
	
	LCD_INIT();	
	InitHCSR04();
	BEEP_GPIO_Config();
	USART2_Init_Config(115200);
	Timer2_Init_Config();
	macBEEP( OFF );
	
	while(1) {  
		char saveDist[4];
		char warned[4];
		char triggered[4];
		char distance[4];
		char distance2[4];	 
		char distance3[4];

		dist = HCSR04GetDistanceS1();
		dist2 = HCSR04GetDistanceS2();
		dist3 = HCSR04GetDistanceS3();
		
		importData();
		setSafeDist();
		scanPassThrough();
	
		exportData();
		
		sprintf(distance, "%03i", dist);
		sprintf(distance2, "%03i", dist2);
		sprintf(distance3, "%03i", dist3);
		sprintf(warned, "%03i", warnTime);
		sprintf(triggered, "%03i", trigTime);
		sprintf(saveDist, "%03i", safeDist_S1);
		displayMenu(distance,distance2, distance3,warned,triggered,saveDist);
		
	}

}

void GPIO_Config_LED(void) {
		
	/* Init K1 & K2 as Input */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 

	/* Config LED output */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

  GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5);
}





void displayMenu(char * dist, char * dist2, char * dist3 ,char * warn, char * trig, char * safeDist) {
	LCD_DrawString(15, 5, "Pet Security Monitor");
	LCD_DrawString(15, 25, "Yip Pak Kin & Ko Chung Pong");
	LCD_DrawString(15, 45, "Distance of Sensor 1:");
	LCD_DrawString(190, 45, dist);
	LCD_DrawString(215, 45, "mm    ");
	LCD_DrawString(15, 65, "Distance of Sensor 2:");
	LCD_DrawString(190, 65, dist2);
	LCD_DrawString(215, 65, "mm    ");
	LCD_DrawString(15, 85, "Distance of Sensor 3:");
	LCD_DrawString(190, 85, dist3);
	LCD_DrawString(215, 85, "mm    ");
	//LCD_DrawString(15, 95, "Warning Times: ");
	//LCD_DrawString(190, 95, warn);
	LCD_DrawString(15, 115, "Danger TImes: ");
	LCD_DrawString(190, 115, trig);
	
	LCD_DrawString(0, 130, "******************************");
	LCD_DrawString(100, 145, "Menu");
	LCD_DrawString(15, 160, "1) Call Out Test");
	LCD_DrawString(15, 175, "2) Export data");
	LCD_DrawString(15, 230, "Saved Safe Distance :");
	LCD_DrawString(190, 230, safeDist);
	LCD_DrawString(215, 230, "mm    ");
}



void scanPassThrough(){
		
	
		
		if(dist < 0) {
			dist = 0;
		} 
		
		if(dist2 < 0) {
			dist2 = 0;
		}
		
		if(dist3 < 0) {
			dist3 = 0;
		}
		

		
		if(dist < safeDist_S1 && S1Pass == 0 && S1Inside == 0) {
				S1Pass = 1;
		}	else if(S1Pass != 0 && dist > safeDist_S1) {
				S1Inside = 1;
		}	else if(S1Inside == 1 && S1Pass == 1 && dist < safeDist_S1) {
				Delayus(1000000);
				S1Inside = 0;
				S1Pass = 0;
		}
		
		if(S1Inside == 1 && dist2 < safeDist_S2 && S2Pass == 0 && S2Inside == 0) {
				S2Pass = 1;
		}else if(S1Inside == 1 && S2Pass != 0 && dist2 > safeDist_S2) {
				S2Inside = 1;
		}else if(S1Inside == 1 && S2Inside == 1 && S2Pass == 1 && dist2 < safeDist_S2) {
				Delayus(1000000);
				S2Inside = 0;
				S2Pass = 0;
		}
		
		
		if(S1Inside == 1 && S2Inside == 1 && dist3 < safeDist_S3 && S3Pass == 0 && S3Inside == 0) {
				S3Pass = 1;
		}else if(S1Inside == 1 && S2Inside == 1 && S3Pass != 0 && dist3 > safeDist_S3) {
				S3Inside = 1;
		}else if(S1Inside == 1 && S2Inside == 1 && S3Inside == 1 && S3Pass == 1 && dist3 < safeDist_S3) {
				Delayus(1000000);
				S3Inside = 0;
				S3Pass = 0;
		}
		
			
			
		
		if(S1Inside == 1 && S2Inside == 0 && S3Inside == 0) {
				alert(100000);
		} else if(S1Inside == 1 && S2Inside == 1 && S3Inside == 0) {
				alert(50000);
		} else if (S1Inside == 1 && S2Inside == 1 && S3Inside == 1) {
				alert(10000);
				UART2_SendString(phone);
		}
		
}



void setSafeDist() {
	
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1){
			safeDist_S1 = dist - 20;
			safeDist_S2 = dist - 20;
			safeDist_S3 = dist - 20;
		
			if(safeDist_S1 < 0) {
				safeDist_S1 = 0;
			}
			if(safeDist_S2 < 0) {
				safeDist_S2 = 0;
			}
			if(safeDist_S3 < 0) {
				safeDist_S3 = 0;
			}
		}
}




void alert(int alertSpeed){
		macBEEP( ON );			  
		Delayus(alertSpeed);  //100000 by jack	
		macBEEP( OFF );		  
		Delayus(alertSpeed);
}

/***********************************************************
 *
 *	Import SD card Data
 *
 * 	Press K2 before setting SafeDistance (K1) -> Import Data
 *	
 *  Press K2 After setting SafeDistance (K1) -> Export Data
 *
 ************************************************************
*/

void importData(){
	
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1 && boolImport == 0){
		/*
		*		import data
		*
		*/
	
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		boolImport = 1;
	}
}

void exportData() {
	
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1 && boolImport == 1){
		/*
		*		export data
		*
		*/
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	}
}


/*****************************
 *
 *
 *	NDS CODE BELOW
 *
 *****************************
*/
void USART2_IRQHandler(void)                	
{
			u8 Res=0;
			Res =USART_ReceiveData(USART2);
			Uart2_Buf[First_Int] = Res;
			First_Int++;
			if(First_Int > Buf2_Max)
			{
				First_Int = 0;
			}    
} 	

void TIM2_IRQHandler(void)
{
	static u8 flag =1;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );
	
		if(flag)
		{	
			flag=0;
		}
		else
		{
			flag=1;
		}
	}	
}

void TIM2_IRQHandler_SMS(void)
{
	static u8 flag =1;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );
		
		if(Timer0_start)
		Times++;
		if(Times > shijian)
		{
			Timer0_start = 0;
			Times = 0;
		}
	
		if(flag)
		{
			flag=0;
		}
		else
		{
			flag=1;
		}
	}	
}

void CLR_Buf2(void)
{
	u16 k;
	for(k=0;k<Buf2_Max;k++)
	{
		Uart2_Buf[k] = 0x00;
	}
    First_Int = 0;
}

u8 Find(char *a)
{ 
  if(strstr(Uart2_Buf,a)!=NULL)
	    return 1;
	else
			return 0;
}

void Second_AT_Command(char *b,char *a,u8 wait_time)         
{
	u8 i;
	char *c;
	c = b;
	CLR_Buf2(); 
  i = 0;
	while(i == 0)                    
	{
		if(!Find(a)) 
		{
			if(Timer0_start == 0)
			{
				b = c;
				for (; *b!='\0';b++)
				{
					while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
					USART_SendData(USART2,*b);
				}
				UART2_SendLR();	
				Times = 0;
				shijian = wait_time;
				Timer0_start = 1;
		   }
    }
 	  else
		{
			i = 1;
			Timer0_start = 0;
		}
	}
	CLR_Buf2(); 
}

void Set_Text_Mode(void)
{
	char temp[50]="AT+CSCA=";
	Second_AT_Command("ATE0","OK",3);
	Second_AT_Command("AT+CMGF=1","OK",3);
	Second_AT_Command("AT+CPMS=\"SM\",\"SM\",\"SM\"","OK",3);
	strcat(temp,(char*)SMS_Num);
	Second_AT_Command(temp,"OK",3);
}

void Send_Text_Sms(char* msg)
{
	char temp[50]="AT+CMGS=";
	
	Set_Text_Mode();
	
	strcat(temp,(char*)phone_sms);
	Second_AT_Command(temp,">",3);
	UART2_SendString((char*)msg);
	USART_SendData(USART2 ,0X1A);
	UART2_SendLR();
}

void Delayus(int duration)
{
		while(duration--) 
		{
			int i=0x02;				
			while(i--)
			__asm("nop");
		}
}
