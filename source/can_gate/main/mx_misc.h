/*
	Name:			mx_misc.h
	Purpose:		CAN Gate project
	Author:			Alexander Suvorov
	Created:		2016/07
	Modified by:
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/

#ifndef __mx_misc_H
#define __mx_misc_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"

void delay_ms( unsigned int delay );
void delay_us( unsigned int delay );
void put_str( char* str );
void USART1_IRQHandler( void );
void SysTick_Handler( void );

#ifdef __cplusplus
	}
#endif

#endif /* __mx_misc_H */
