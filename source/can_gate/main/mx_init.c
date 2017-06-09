/*
	Name:			mx_init.c
	Purpose:		CAN Gate project
	Author:			Alexander Suvorov
	Created:		2016/07
	Modified by:	2017/04
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/

#include "mx_init.h"

void MX_Init( void )
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	USART_InitTypeDef	USART_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	// -----------------------------------------------------------------------
	// Oscilator
	// -----------------------------------------------------------------------
	RCC_DeInit();												// Resets the RCC clock configuration to the default reset state.
	#ifdef OSC_INT												// === Internal ===
	RCC_HSEConfig( RCC_HSE_OFF );								// Configures the Internal oscillator (HSI).
	RCC_HSICmd( ENABLE );										// Enables the Internal High Speed oscillator (HSI).
	RCC_PLLConfig( RCC_PLLSource_HSI_Div2, RCC_PLLMul_6 );		// Configures the PLL clock source and multiplication factor.
	RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );				// Configures the system clock as PLL source (SYSCLK).
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );							// Configures the AHB clock (HCLK).
	#else														// === External ===
	RCC_HSICmd( DISABLE );										// Enables the External High Speed oscillator (HSE).
	RCC_HSEConfig( RCC_HSE_ON );								// Configures the External High Speed oscillator (HSE).
	RCC_PLLConfig( RCC_PLLSource_HSE_Div2, RCC_PLLMul_6 );		// Configures the PLL clock source and multiplication factor.
	RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );				// Configures the system clock as PLL source (SYSCLK).
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );							// Configures the AHB clock (HCLK).
	#endif
	RCC_PCLK1Config( RCC_HCLK_Div1 );							// Configures the Low Speed APB clock (PCLK1).
	RCC_PCLK2Config( RCC_HCLK_Div1 );							// Configures the Low Speed APB clock (PCLK2).

	RCC_PLLCmd( ENABLE );										// Enables the PLL.

	// Enable peripheral clock
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,	ENABLE );	// Clocking GPIOA
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,	ENABLE );	// Clocking GPIOB
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4,	ENABLE );	// Clocking TIM4 ( for delays )
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1,	ENABLE );	// Clocking USART1


	// -----------------------------------------------------------------------
	// Init GPIOS
	// -----------------------------------------------------------------------

	// Configure GPIO pin : PB pin 8 ( blink led )
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	#ifdef IO_BLINK
		GPIO_SetBits( GPIOA,GPIO_Pin_8 );
	#else
		GPIO_ResetBits( GPIOA,GPIO_Pin_8 );
	#endif

	// -----------------------------------------------------------------------
	// Pin for interrupt MCP2515
	// -----------------------------------------------------------------------
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	// -----------------------------------------------------------------------
	// Init UARTS
	// -----------------------------------------------------------------------
	// USART1 Configuration
	// PA9	-> USART1_TX
	// PA10	<- USART1_RX

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	// Init pins PA9 & PA10 ( Rx & Tx USART )
	GPIO_StructInit( &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	// Setup USART
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate				= UART_SPEED;
	USART_InitStructure.USART_WordLength			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits				= USART_StopBits_1;
	USART_InitStructure.USART_Parity				= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_Init( USART1, &USART_InitStructure );
	USART_Cmd( USART1, ENABLE );

	// Start UART + interrupt
	NVIC_InitStructure.NVIC_IRQChannel		= USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd	= ENABLE;
	NVIC_Init( &NVIC_InitStructure );
	USART_Cmd( USART1, ENABLE );

	NVIC_EnableIRQ( USART1_IRQn );
	USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );

	// Init timers ( SysyTick )
	// -----------------------------------------------------------------------
	SysTick_Config( F_APB1 / 100 );							// interrupt 10 ms
	// -----------------------------------------------------------------------

	// -----------------------------------------------------------------------
	// Init watchdog
	// -----------------------------------------------------------------------
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	IWDG_SetPrescaler( IWDG_Prescaler_32 );
	//IWDG_SetReload( 40000 / 256 );	// 150 ms
	IWDG_SetReload( 40000 / 128 );		// 312 ms
	IWDG_ReloadCounter();
	IWDG_Enable();

	// Init	global variables
	rx_idx				= 0;
	DelayTime			= 0;
	CntTime				= 0;
	WD_Time				= 0;
	CANDownTime			= 10000;
	ret_button			= FALSE;
	cli_mode			= FALSE;
	#ifdef ECHO
		Echo			= TRUE;
	#else
		Echo			= FALSE;
	#endif
	#ifdef SHORT_OUTPUT
		Short			= TRUE;
	#else
		Short			= FALSE;
	#endif
}
