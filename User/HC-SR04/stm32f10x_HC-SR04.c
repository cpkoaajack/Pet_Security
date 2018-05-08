#include <stm32f10x_HC-SR04.h>
RCC_ClocksTypeDef RCC_ClocksStatus;
TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
TIM_OCInitTypeDef TIM_OCInitStruct;
TIM_ICInitTypeDef TIM_ICInitStruct;
uint16_t prescaler;

RCC_ClocksTypeDef RCC_ClocksStatus2;
TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct2;
TIM_OCInitTypeDef TIM_OCInitStruct2;
TIM_ICInitTypeDef TIM_ICInitStruct2;
uint16_t prescaler2;

static void initMeasureTimer_SENSOR1() {
	
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	prescaler = RCC_ClocksStatus.SYSCLK_Frequency / 1000000 - 1; //1 tick = 1us (1 tick = 0.165mm resolution)

	TIM_DeInit(US_TIMER_SENSOR1);		//TIM3
	TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(US_TIMER_SENSOR1, &TIM_TimeBaseInitStruct);

	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 15; //us
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(US_TIMER_SENSOR1, &TIM_OCInitStruct);		//TIM3 CH3
	
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICFilter = 0;

	TIM_PWMIConfig(US_TIMER_SENSOR1, &TIM_ICInitStruct);		//TIM3 CH1
	
	TIM_SelectInputTrigger(US_TIMER_SENSOR1, US_TIMER_TRIG_SOURCE);
	TIM_SelectMasterSlaveMode(US_TIMER_SENSOR1, TIM_MasterSlaveMode_Enable);
	
	TIM_CtrlPWMOutputs(US_TIMER_SENSOR1, ENABLE);
	
	TIM_ClearFlag(US_TIMER_SENSOR1, TIM_FLAG_Update);
}

static void initMeasureTimer_SENSOR2() {
	
	RCC_GetClocksFreq(&RCC_ClocksStatus2);
	prescaler2 = RCC_ClocksStatus2.SYSCLK_Frequency / 1000000 - 1; //1 tick = 1us (1 tick = 0.165mm resolution)

	TIM_DeInit(US_TIMER_SENSOR2);	//TIM4 
	TIM_TimeBaseInitStruct2.TIM_Prescaler = prescaler2;
	TIM_TimeBaseInitStruct2.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct2.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStruct2.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(US_TIMER_SENSOR2, &TIM_TimeBaseInitStruct2);

	TIM_OCStructInit(&TIM_OCInitStruct2);
	TIM_OCInitStruct2.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct2.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct2.TIM_Pulse = 15; //us
	TIM_OCInitStruct2.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(US_TIMER_SENSOR2, &TIM_OCInitStruct2);	//TIM4 CH3
	
	TIM_ICInitStruct2.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct2.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStruct2.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStruct2.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct2.TIM_ICFilter = 0;

	TIM_PWMIConfig(US_TIMER_SENSOR2, &TIM_ICInitStruct2);		//TIM4 CH1

	TIM_SelectInputTrigger(US_TIMER_SENSOR2, US_TIMER_TRIG_SOURCE);
	TIM_SelectMasterSlaveMode(US_TIMER_SENSOR2, TIM_MasterSlaveMode_Enable);

	TIM_CtrlPWMOutputs(US_TIMER_SENSOR2, ENABLE);

	TIM_ClearFlag(US_TIMER_SENSOR2, TIM_FLAG_Update);
}

void initPins() {
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//SENSOR1
	GPIO_InitStructure.GPIO_Pin = US_SENSOR1_TRIG_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(US_SENSOR1_TRIG_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = US_SENSOR1_ECHO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(US_SENSOR1_ECHO_PORT, &GPIO_InitStructure);
	//SENSOR2
	GPIO_InitStructure.GPIO_Pin = US_SENSOR2_TRIG_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(US_SENSOR2_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = US_SENSOR2_ECHO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(US_SENSOR2_PORT, &GPIO_InitStructure);
}

void InitHCSR04() {
	
	EnableHCSR04PeriphClock();
	initPins();
	initMeasureTimer_SENSOR1();
	initMeasureTimer_SENSOR2();
}

void EnableHCSR04PeriphClock() {
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
}

int32_t HCSR04GetDistanceS1() {
	
	(US_TIMER_SENSOR1)->CNT = 0;
	TIM_Cmd(US_TIMER_SENSOR1, ENABLE);
	while(!TIM_GetFlagStatus(US_TIMER_SENSOR1, TIM_FLAG_Update));
	TIM_Cmd(US_TIMER_SENSOR1, DISABLE);
	TIM_ClearFlag(US_TIMER_SENSOR1, TIM_FLAG_Update);
	
	return (TIM_GetCapture2(US_TIMER_SENSOR1)-TIM_GetCapture1(US_TIMER_SENSOR1))*165/1000;
}

int32_t HCSR04GetDistanceS2() {
	
	(US_TIMER_SENSOR2)->CNT = 0;
	TIM_Cmd(US_TIMER_SENSOR2, ENABLE);
	while(!TIM_GetFlagStatus(US_TIMER_SENSOR2, TIM_FLAG_Update));
	TIM_Cmd(US_TIMER_SENSOR2, DISABLE);
	TIM_ClearFlag(US_TIMER_SENSOR2, TIM_FLAG_Update);
	
	return (TIM_GetCapture2(US_TIMER_SENSOR2)-TIM_GetCapture1(US_TIMER_SENSOR2))*165/1000;
}

