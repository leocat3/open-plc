/*
	Name:			mx_can.c
	Purpose:		CAN Gate project
	Author:			Alexander Suvorov
	Created:		2016/07
	Modified by:
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/

#include "stm32f10x_exti.h"
#include "mx_can.h"

void read_flash( void );
void write_flash( void );


/*
   ===========================================================================
   *** MCP2515 ***
   Init CAN EXTI and NVIC
   ===========================================================================
*/
void Init_NVIC_Can( void )
{
	EXTI_InitTypeDef	EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	// Init_EXTI
	GPIO_EXTILineConfig( GPIO_PortSourceGPIOA, GPIO_PinSource0 );
	EXTI_InitStructure.EXTI_Line	= EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode	= EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger	= EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd	= ENABLE;
	EXTI_Init( &EXTI_InitStructure );

	// Init_NVIC
	NVIC_InitStructure.NVIC_IRQChannel						= EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0x0D;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init( &NVIC_InitStructure );
}
// ===========================================================================


// ===========================================================================
// Init MCP2515
// ===========================================================================
void Init_Can( void )
{
	// Set config to CANINTE reg - interrupt flags
	spi_tx_data[0]		= SPI_WRITE;
	spi_tx_data[1]		= CANINTE;
	//spi_tx_data[2]	= 0xA5;
	spi_tx_data[2]		= 0x41;
	send_spi( 3 );

	// Set config to RXB0CTRL reg
	spi_tx_data[0] = SPI_WRITE;
	spi_tx_data[1] = RXB0CTRL;
	spi_tx_data[2] = 0x60;
	send_spi( 3 );

	// Set config to RXB1CTRL
	spi_tx_data[0] = SPI_WRITE;
	spi_tx_data[1] = RXB1CTRL;
	spi_tx_data[2] = 0x60;
	send_spi( 3 );

	spi_tx_data[0] = SPI_WRITE;
	spi_tx_data[1] = BFPCTRL;
	spi_tx_data[2] = 0x30;
	send_spi( 3 );

	// Set config to RXB0DLC reg
	spi_tx_data[0] = SPI_WRITE;
	spi_tx_data[1] = RXB0SIDL;
	spi_tx_data[2] = 0x08;	// set data length code
	send_spi( 3 );

	switch( Speedc )
	{
		case	0:	// Speed 500000
					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF1;
					spi_tx_data[2] = 0x80;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF2;
					spi_tx_data[2] = 0x91;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF3;
					spi_tx_data[2] = 0x01;
					send_spi( 3 );

					break;

		case	1:	// Speed 250000
					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF1;
					spi_tx_data[2] = 0x80;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF2;
					spi_tx_data[2] = 0xB5;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF3;
					spi_tx_data[2] = 0x01;
					send_spi( 3 );

					break;

		case	2:	// Speed 125000
					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF1;
					spi_tx_data[2] = 0x81;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF2;
					spi_tx_data[2] = 0xb5;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF3;
					spi_tx_data[2] = 0x01;
					send_spi( 3 );

					break;

		case	3:	// Speed 100000
					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF1;
					spi_tx_data[2] = 0x81;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF2;
					spi_tx_data[2] = 0xbf;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF3;
					spi_tx_data[2] = 0x02;
					send_spi( 3 );

					break;

		case	4:	// Speed 50000
					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF1;
					spi_tx_data[2] = 0x84;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF2;
					spi_tx_data[2] = 0xb5;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF3;
					spi_tx_data[2] = 0x01;
					send_spi( 3 );

					break;

		case	5:	// Speed 20000
					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF1;
					spi_tx_data[2] = 0x89;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF2;
					spi_tx_data[2] = 0xbf;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF3;
					spi_tx_data[2] = 0x02;
					send_spi( 3 );

					break;

		case	6:	// Speed 10000
		default:
					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF1;
					spi_tx_data[2] = 0x98;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF2;
					spi_tx_data[2] = 0xB5;
					send_spi( 3 );

					spi_tx_data[0] = SPI_WRITE;
					spi_tx_data[1] = CNF3;
					spi_tx_data[2] = 0x01;
					send_spi( 3 );
	}

	// All done. Set to normal mode
	spi_tx_data[0] = SPI_WRITE;
	spi_tx_data[1] = CANCTRL;
	spi_tx_data[2] = 0x08;	// One-Shot mode bit
	send_spi( 3 );
}
// ===========================================================================


// ===========================================================================
// Program reset MCP2515
// ===========================================================================
void reset_can( void )
{
	spi_tx_data[0] = SPI_RESET;
	send_spi( 1 );
}
// ===========================================================================


// ===========================================================================
int receive_can( void )
// ===========================================================================
{
	int			ret;
	uint32_t	i;
	uint16_t	id1, id2;
	uint8_t		ddi, d8_1, d8_2;
	char		s[128];

	ret	= 0;

	// Read	interrupt condition	register
	spi_tx_data[0]	= SPI_READ;
	spi_tx_data[1]	= CANINTF;
	spi_tx_data[2]	= 0x00;
	send_spi( 3 );
	ddi				= spi_rx_data[2];	// Read register to ddi

	// Interrupt on filling buffer 0:
	if( ddi & 1 )
	{
		CANDownTime = 0;

		// Get identifier
		for( i = 2; i < SPI_LEN; i++ )
		{
			spi_tx_data[i] = 0;
		}
		spi_tx_data[0] = SPI_READ;	//
		spi_tx_data[1] = RXB0SIDH;	//
		send_spi( 6 );

		// Normal identifier
		can_id[0] = spi_rx_data[2];
		can_id[1] = spi_rx_data[3];

		// Extend identifier
		can_id[2] = spi_rx_data[4];
		can_id[3] = spi_rx_data[5];

		if( can_id[1] && 0x08 )
		{
			// Identifier to normal view
			id1		= ( can_id[0] << 5 );	//
			id2		= ( can_id[1] >> 3 );	//
			id1	   |= ( id2 & 0xFC );
			id1	   |= ( can_id[1] & 0x03 );

			d8_1 = id1 >> 8;
			d8_2 = id1 & 0xFF;

			// Get 8 byte data
			for( i = 1; i < SPI_LEN; i++ )
			{
				spi_tx_data[i] = 0;
			}
			spi_tx_data[0] = SPI_READ_RX | 0x02;	// Read Rx bufer0
			send_spi( 9 );

			for( i = 0; i < 8; i++ )
			{
				can_data[i] = spi_rx_data[i + 1];
			}

			if( Echo )
			{
				if( Short )
				{
					sprintf( s, "\rRECV: %02X%02X%02X%02X#", d8_1, d8_2, can_id[2], can_id[3] );
				}
				else
				{
					sprintf( s, "\rRECV: %02X%02X%02X%02X #", d8_1, d8_2, can_id[2], can_id[3] );
				}
			}
			else
			{
				if( Short )
				{
					sprintf( s, "%02X%02X%02X%02X#", d8_1, d8_2, can_id[2], can_id[3] );
				}
				else
				{
					sprintf( s, "%02X%02X%02X%02X #", d8_1, d8_2, can_id[2], can_id[3] );
				}
			}
			put_str( s );
			for( i = 1; i < 9; i++ )
			{
				if( Short )
				{
					sprintf( s, "%02X", spi_rx_data[i] );
				}
				else
				{
					sprintf( s, " %02X", spi_rx_data[i] );
				}
				put_str( s );
			}
			if( Echo )
			{
				put_str( CLI );
				put_str( usart_rx_data );
			}
			else
			{
				put_str( "\n\r" );
			}
		#ifdef IO_BLINK
			if( !DelayTime )
			{
				DelayTime	= DELAY_TIME;
				DelayTime2	= DELAY_TIME;
			}
		#endif
		}	// if( can_id[1] && 0x08 )
	}	// if( ddi & 1 ) --- interrupt of filling buffer 0 :
	return ret;
}
// ===========================================================================


// ===========================================================================
int send_can( void )
// ===========================================================================
{
	uint32_t	CANCntTime, Cnt;
	int			i, ret;

	ret = 0;

	// Setup data
	for( i = 6; i < SPI_LEN; i++ )
	{
		spi_tx_data[i] = 0;
	}

	spi_tx_data[0] = SPI_WRITE;
	spi_tx_data[1] = TXB0SIDH;

	spi_tx_data[2] = can_id[0];
	spi_tx_data[3] = can_id[1];
	spi_tx_data[4] = can_id[2];
	spi_tx_data[5] = can_id[3];
	spi_tx_data[6] = 0x08;		// Data Length

	for( i = 0; i < 8; i++ )
	{
		spi_tx_data[i+7] = can_data[i];
	}

	send_spi( 15 );

	// Send data
	spi_tx_data[0] = SPI_RTS | 0x01;
	send_spi( 1 );

	CANCntTime = CntTime;
	i = 0;
	Cnt = 0;
	while( ( Cnt < 2 ) && ( i == 0 ) )
	{
		// Read interrupt status
		delay_us( 10 );
		spi_tx_data[0] = SPI_READ;
		spi_tx_data[1] = CANINTF;
		spi_tx_data[2] = 0x00;
		send_spi( 3 );

		// If bufer empty, cleat interrupt flag
		if( spi_rx_data[2] & 4 )
		{
			i = 1;
		}
		else
		if( spi_rx_data[2] == 1 )	// Can't send packet, Reset
		{
			reset_can();
			delay_us( 100 );
			Init_Can();
			i = 1;
		}	// if( spi_rx_data[2] == 1 )
		Cnt = CntTime - CANCntTime;
	}
	if( Cnt > 1 )
	{
		ret = 1;
	}
	return ret;
}
// ===========================================================================


// ===========================================================================
void EXTI0_IRQHandler( void	)
// ===========================================================================
{
	receive_can();

	// Clear interrupt bits
	spi_tx_data[0] = SPI_BIT_MODIFY;
	spi_tx_data[1] = CANINTF;
	spi_tx_data[2] = 0xFF;
	spi_tx_data[3] = 0x00;
	send_spi( 4 );

	if( EXTI_GetITStatus( EXTI_Line0 ) )
	{
		EXTI_ClearITPendingBit( EXTI_Line0 );
	}
}
// ===========================================================================
