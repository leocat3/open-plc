/*
	Name:			mx_misc.c
	Purpose:		CAN Gate project
	Author:			Alexander Suvorov
	Created:		2016/07
	Modified by:	2017/05
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/

#include "mx_misc.h"
#include "string.h"

void reset_can( void );
void Init_Can( void );


// ===========================================================================
// Delay in milliseconds
// ===========================================================================
void delay_ms( unsigned int delay )
{
	int i;

	if( delay )
	{
		for( i = 0; i < delay; i++ )
		{
			delay_us( 1000 );
		}
	}
}
// ===========================================================================


// ===========================================================================
// Delay in microseconds
// ===========================================================================
void delay_us( unsigned int delay )
{
	if( delay < 5 )
	{
		return;
	}
	else
	{
		delay -= 4;
	}
	TIM4->PSC	 = F_APB1 / 1000000 - 1;
	TIM4->ARR	 = delay;
	TIM4->EGR	|= TIM_EGR_UG;
	TIM4->CR1	|= TIM_CR1_CEN | TIM_CR1_OPM;
	while( TIM4->CR1 & ( TIM_CR1_CEN != 0 ) ) {}
}
// ===========================================================================


// ===========================================================================
void put_str( char* str )
// ===========================================================================
{
	int	i, n;

	while( !( USART1->SR & USART_SR_TC ) );
	n = strlen( str );
	for( i = 0; i < n; i++ )
	{
		while( !( USART1->SR & USART_SR_TC ) );
		USART1->DR = str[i];
	}
}
// ===========================================================================


// ===========================================================================
void USART1_IRQHandler( void )
// ===========================================================================
{
	char	c, s[2];

	// Interrupt for read
	if ( USART_GetITStatus( USART1, USART_IT_RXNE ) != RESET )
	{
		c = USART_ReceiveData( USART1 );		// read char
		if( Echo )
		{
			s[0] = c;							// Put char in bufer for "echo"
			s[1] = 0;							// End of string
		}

		if( c == 13 )
		{
			usart_rx_data[rx_idx] = 0;
			ret_button = TRUE;					// Set Enter flag
			rx_idx = 0;
		}
		else
		{
			if( rx_idx < ( RX_LEN - 1 ) )
			{
				usart_rx_data[rx_idx++] = c;	// char to buffer
				usart_rx_data[rx_idx] = 0;		// End of string
				if( Echo )
				{
					put_str( s );				// echo of char
				}
			}
		}
		USART_ClearITPendingBit( USART1, USART_IT_RXNE );
	}
}
// ===========================================================================


// ===========================================================================
// Timer SysTick
// Priority = 6
// Frequency = 100 Hz
// ===========================================================================
void SysTick_Handler( void )
{
	// Blink lcd

	#ifndef IO_BLINK
		if( DelayTime )					// Time is up
			DelayTime--;				// If not, decrement
		else
		{
			if( cli_mode )				// In Cmd mode
			{
				DelayTime = 25;			// Set CLI timer
			}
			else
			{
				DelayTime = 75;			// Timer in standart mode
			}
			GPIOA->ODR ^= GPIO_Pin_8;	// Inver lamp status
		}
	#else
		if( DelayTime )					// Time is up
		{
			if( !DelayTime2 )					// Time2 is up
			{
				DelayTime--;				// If not, decrement
				GPIO_ResetBits( GPIOA,GPIO_Pin_8 );
			}
			else
			{
				DelayTime2--;				// If not, decrement
			}
		}
		else
		{
			GPIO_SetBits( GPIOA,GPIO_Pin_8 );
		}
	#endif

	CntTime++;
	CANDownTime++;

	if( CANDownTime > 500 )			// CAN status
	{
		reset_can();
		delay_us( 100 );
		Init_Can();
		CANDownTime = 0;
	}

	if( WD_Time++ > 10 )
	{
		WD_Time = 0;
		IWDG_ReloadCounter();		// Reload watchdog
	}
}
// ===========================================================================
