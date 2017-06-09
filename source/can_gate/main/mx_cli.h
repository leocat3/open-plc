/*
Name:			mx_cli.h
Purpose:		CAN Gate project
Author:			Alexander Suvorov
Created:		2016/07
Modified by:
RCS-ID:
Copyright:		(c) Alexander Suvorov
Licence:		The MIT License (MIT)
*/

#ifndef __mx_cli_H
#define __mx_cli_H

#ifdef __cplusplus
	extern "C" {
#endif

/* Includes ---------------------------------------------------------------- */
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#include "mx_misc.h"
#include "string.h"

void cli( void );

#ifdef __cplusplus
	}
#endif

#endif /* __mx_cli_H */
