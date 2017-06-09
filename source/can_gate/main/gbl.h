/*
	Name:			gbl.h
	Purpose:		CAN Gate project
	Author:			Alexander Suvorov
	Created:		2016/07
	Modified by:
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/

#ifndef __GBL__
#define __GBL__

#ifdef __cplusplus
	extern "C" {
#endif


#define CLI "\n\rSEND: "

#define OSC_INT
#define F_APB1		24000000
#define FLASH_KEY1	( ( uint32_t ) 0x45670123 )
#define FLASH_KEY2	( ( uint32_t ) 0xCDEF89AB )
#define Page_63		0x0800FC00

#define RX_LEN		64
#define SPI_LEN		24

#define IO_BLINK
#define DELAY_TIME		2
#define SHORT_OUTPUT
//#define ECHO
//#define UART_SPEED	115200
//#define UART_SPEED	230400
//#define UART_SPEED	460800
#define UART_SPEED		921600

static volatile uint32_t	DeviceType;
static volatile uint32_t	Sn;
static volatile uint16_t	Noc;
static volatile uint16_t	Speedc;

static volatile uint8_t		rx_idx;
static volatile bool		ret_button;
static volatile bool		cli_mode;
static volatile bool		Echo;
static volatile bool		Short;
static char					usart_rx_data[RX_LEN];
static char					cli_data[16];

static volatile uint32_t	DelayTime;
static volatile uint32_t	DelayTime2;
static volatile uint32_t	CntTime;
static volatile uint32_t	WD_Time;
static volatile uint32_t	CANDownTime;

static volatile uint8_t		spi_tx_data[SPI_LEN];
static volatile uint8_t		spi_rx_data[SPI_LEN];

static volatile uint8_t		can_id[4];
static volatile uint8_t		can_data[8];


#ifdef __cplusplus
	}
#endif

#endif /* __GBL__ */
