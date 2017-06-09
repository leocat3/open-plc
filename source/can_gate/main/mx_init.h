/*
	Name:			mx_init.h
	Purpose:		CAN Gate project
	Author:			Alexander Suvorov
	Created:		2016/07
	Modified by:
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/

#ifndef __mx_init_H
#define __mx_init_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dac.h"
#include "misc.h"
//#include "gbl.h"

void MX_Init( void );

#ifdef __cplusplus
	}
#endif

#endif /* __mx_init_H */
