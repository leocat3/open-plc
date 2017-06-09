/*
	Name:			main.c
	Purpose:		CAN Gate project
	Author:			Alexander Suvorov
	Created:		2016/07
	Modified by:	2017/04
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/

#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_exti.h"

//#include "math.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "gbl.h"
#include "mx_misc.c"
#include "mx_init.c"
#include "mx_spi.c"
#include "mx_can.c"
#include "mx_cli.c"
#include "hex2bin.c"


// ===========================================================================
int main()
// ===========================================================================
{
	uint8_t		ui8, *p_ui8;
	uint16_t	l, l1, l2;
	uint32_t	i, j, k, i1, n;
	char		c;
	//char		s[128]; // For print

	SystemInit();
	MX_Init();
	read_flash();
	Init_Spi();
	Init_NVIC_Can();
	reset_can();
	delay_us( 100 );
	Init_Can();

	strcpy( cli_data, CLI );
	USART_ClearITPendingBit( USART1, USART_IT_RXNE );	// Clear UART buffer
	for( i = 0; i < 3; i++ )
	{
		put_str( "+" );
		delay_ms( 100 );
	}
	put_str( "\n\r" );
	if( Echo )
	{
		put_str( cli_data );
	}


	// ---------------------------------------------------------------------------
	// Basic cycle
	// ---------------------------------------------------------------------------
	while( 1 )
	{

	/* ---------------------------------------------------------------------------
	User functions

	For example:
		Calculate...
		...

	send data to CAN:
		strcpy( usart_rx_data, "1000 0000 # 11 22 33 44 55 66 77 88" );
		ret_button = TRUE;	// Simulate Return pressed
	------------------------------------------------------------------------------ */

		if( ret_button )	// Press Return button
		{
			ret_button = FALSE;
			n = strlen( usart_rx_data );

			// Remove space chars
			for( i = 0; i < n; i++ )
			{
				while( usart_rx_data[i] == ' ' )
				{
					for( j = i; j < n; j++ )
					{
						usart_rx_data[j] = usart_rx_data[j + 1];
					}
				}
			}

			// Validate input string
			j = 0;
			n = strlen( usart_rx_data );
			for( i = 0; i < n; i++ )
			{
				if( ( usart_rx_data[i] < 32 ) || ( usart_rx_data[i] > 126 ) )
				{
					if( usart_rx_data[i] )
					{
						usart_rx_data[i] = '$';
						j = 1;
					}
				}
				if( usart_rx_data[i] > 96 )
				{
					usart_rx_data[i] -= 32;
				}

			}	// for( i = 0; i < n; i++ )

			if( strcmp( usart_rx_data, "CFG" ) == 0 )		// Config mode?
			{
				cli();
			}
			else
			if( strcmp( usart_rx_data, "ECHO" ) == 0 )		// Echo mode?
			{
				if( Echo )
				{
					Echo = FALSE;
					put_str( "\r\nECHO OFF\r\n" );
				}
				else
				{
					Echo = TRUE;
					put_str( "\r\nECHO ON\r\n" );
				}
			}
			else
			if( strcmp( usart_rx_data, "SHORT" ) == 0 )		// Short output mode?
			{
				if( Short )
				{
					Short = FALSE;
					put_str( "\r\nSHORT_OUTPUT OFF\r\n" );
				}
				else
				{
					Short = TRUE;
					put_str( "\r\nSHORT_OUTPUT ON\r\n" );
				}
			}
			else
			{
				// Parse data and send to CAN
				n = strlen( usart_rx_data );
				if( n )
				{
					if( ( n > 10 ) && !j )
					{
						if( n > 25 )
						{
							j = 1;
						}
						k = n % 2;
						if( !k )
						{
							j = 1;
						}
						if( ( usart_rx_data[8] != '#' ) && !j )
						{
							j = 1;
						}
						if( ( usart_rx_data[0] != '0' ) && ( usart_rx_data[0] != '1' ) )
						{
							j = 1;
						}

						if( !j )
						{
							for( i = 0; i < 8; i++ )
							{
								c = usart_rx_data[i];
								if( !( ( c >= '0' ) && ( c <= '9' ) ) && !( ( c >= 'A' ) && ( c <= 'F' ) ) )
								{
									j = 1;
								}
							}	// for( i = 0; i < n; i++ )
							for( i = 9; i < n; i++ )
							{
								c = usart_rx_data[i];
								if( !( ( c >= '0' ) && ( c <= '9' ) ) && !( ( c >= 'A' ) && ( c <= 'F' ) ) )
								{
									j = 1;
								}
							}	// for( i = 9; i < n; i++ )
						}	// if( !j )
					}	// if( n > 10 )

// ---------------------------------------------------------------------------

					if( !j )
					{
						l1 = 0;
						for( i = 0; i < 4; i++ )
						{
							c	 = usart_rx_data[i];
							k	 = hex2bin( c );
							l1	 = l1 << 4;
							l1	|= k;
						}

						l2 = 0;
						for( i = 4; i < 8; i++ )
						{
							c	 = usart_rx_data[i];
							k	 = hex2bin( c );
							l2	 = l2 << 4;
							l2	|= k;
						}

						l = ( l1 << 3 ) & 0xFFE0;
						l = l | ( l1 & 0x03 );
						l = l | 0x08;

						// Normal identifier
						p_ui8		= (uint8_t*) &l;
						can_id[0]	= p_ui8[1];
						can_id[1]	= p_ui8[0];

						// Extend identifier
						p_ui8		= (uint8_t*) &l2;
						can_id[2]	= p_ui8[1];
						can_id[3]	= p_ui8[0];

						for( i = 0; i < 8; i++ )
						{
							can_data[i] = 0;
						}

						i1 = 0;
						for( i = 9; i < n; i+=2 )
						{
							c				= usart_rx_data[i];
							ui8				= hex2bin( c );
							ui8				= ui8 << 4;
							c				= usart_rx_data[i+1];
							ui8			   |= hex2bin( c );
							can_data[i1]	= ui8;
							i1++;
						}

						k = send_can();
						if( k )
						{
							put_str( "\n\rError send data to CAN" );
						}
					}

					// ---------------------------------------------------------------------------
					if( strlen( usart_rx_data ) < 11 )
					{
						j = 1;
					}
					// ---------------------------------------------------------------------------

					if( j )
					{
						put_str( "\r\n" );
						put_str( "\r\nWrong CAN-frame format" );
						put_str( "\r\n" );
						put_str( "\r\n    Example:" );
						put_str( "\r\n    NXXXXXXX#XXXXXXXXXXXXXXXX" );
						put_str( "\r\n    N - 0/1" );
						put_str( "\r\n    X - 0..F" );
						put_str( "\r\nOnly Extended frame format" );
						put_str( "\r\n" );
						put_str( "print:\r\n" );
						put_str( "       'cfg'   for configure\r\n" );
						put_str( "       'echo'  for echo on/off\r\n" );
						put_str( "       'short' for short output on/off\r\n" );
						put_str( "\r\n" );
					}
				}	// if( strlen( usart_rx_data ) )
				if( Echo )
				{
					put_str( cli_data );
				}
			}
			usart_rx_data[0] = 0;
		}	// if( ret_button )

		delay_ms( 10 );

	}	// while( 1 )
}
// ===========================================================================
