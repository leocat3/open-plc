/*
	Name:			mx_spi.c
	Purpose:		CAN Gate project
	Author:			Alexander Suvorov
	Created:		2016/07
	Modified by:
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_spi.h"
#include "misc.h"


// ===========================================================================
// Init SPI
// ===========================================================================
void Init_Spi( void )
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	SPI_InitTypeDef		SPI_InitStructure;

	// Clocking SPI1 & port А
	// =======================================================================

	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2, ENABLE );	// Enable SPI2

	// --- SPI2 --------------------------------------------------------------

	/* SPI2 GPIO Configuration
	PB12 ------> SPI2_NSS
	PB13 ------> SPI2_SCK
	PB14 ------> SPI2_MISO
	PB15 ------> SPI2_MOSI
	*/

	// SPI2_NSS
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );
	GPIO_SetBits( GPIOB, GPIO_Pin_12 );		// NSS Up

	// SPI2_SCK; SPI2_MOSI
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	// SPI2_MISO
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	// SPI2 Structure Init
	SPI_StructInit( &SPI_InitStructure );
	SPI_InitStructure.SPI_Direction			= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize			= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL				= SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA				= SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler	= SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit			= SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode				= SPI_Mode_Master;
	//SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init( SPI2, &SPI_InitStructure );
	SPI_Cmd( SPI2, ENABLE );
}
// ===========================================================================


// ===========================================================================
void send_spi( uint8_t cnt )
// ===========================================================================
{
	int	i;

	if( cnt > SPI_LEN )
		cnt = SPI_LEN;

	GPIO_ResetBits( GPIOB, GPIO_Pin_12 );
	delay_us( 2 );

	for( i = 0; i < cnt; i++ )
	{
		SPI_I2S_SendData( SPI2, spi_tx_data[i] );
		while( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_BSY) == SET );

		while( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_RXNE ) != SET );
		spi_rx_data[i] =  SPI_I2S_ReceiveData( SPI2 );
	}	// for( i = 0; i < cnt; i++)

	delay_us( 2 );
	GPIO_SetBits( GPIOB, GPIO_Pin_12 );
	delay_us( 10 );
}
// ===========================================================================
