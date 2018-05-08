#ifndef STM32F10X_HC_SR04_H_
#define STM32F10X_HC_SR04_H_

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

#define US_TIMER_SENSOR1					TIM3
#define US_TIMER_SENSOR2					TIM4
//SENSOR1
#define US_SENSOR1_ECHO_PORT				GPIOA
#define US_SENSOR1_ECHO_PIN					GPIO_Pin_6	//TIM3 CH1 (echo input)

#define US_SENSOR1_TRIG_PORT				GPIOB
#define US_SENSOR1_TRIG_PIN					GPIO_Pin_0	//TIM3 CH3 (trig output)
//SENSOR2
#define US_SENSOR2_PORT							GPIOB
#define US_SENSOR2_ECHO_PIN					GPIO_Pin_6	//TIM4 CH1 (echo input)
#define US_SENSOR2_TRIG_PIN					GPIO_Pin_8	//TIM4 CH3 (trig output)

#define US_TIMER_TRIG_SOURCE		TIM_TS_TI1FP1

static void initMeasureTimer_SENSOR1(void);

static void initMeasureTimer_SENSOR2(void);

void EnableHCSR04PeriphClock(void);

void InitHCSR04(void);

int32_t HCSR04GetDistanceS1(void);

int32_t HCSR04GetDistanceS2(void);

#endif /* STM32F10X_HC_SR04_H_ */
