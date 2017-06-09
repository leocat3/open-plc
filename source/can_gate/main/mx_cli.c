/*
	Name:			mx_cli.c
	Purpose:		CAN Gate project
	Author:			Alexander Suvorov
	Created:		2016/07
	Modified by:
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/

//#include "mx_cli.h"

// ===========================================================================
void write_flash( void )
// ===========================================================================
{
	// FLASH_Unlock
	FLASH->KEYR	= FLASH_KEY1;
	FLASH->KEYR	= FLASH_KEY2;

	// FLASH_Erase Page
	while((FLASH->SR&FLASH_SR_BSY));
	FLASH->CR |= FLASH_CR_PER;			//Page Erase Set
	FLASH->AR = Page_63;				//Page Address
	FLASH->CR |= FLASH_CR_STRT;			//Start Page Erase
	while((FLASH->SR&FLASH_SR_BSY));
	FLASH->CR &= ~FLASH_CR_PER;			//Page Erase Clear

	//FLASH_Program HalfWord
	FLASH->CR |= FLASH_CR_PG;

	// Speedc
	while( FLASH->SR&FLASH_SR_BSY );
	*( __IO uint16_t* ) ( Page_63 + 10 ) = Speedc;

	while((FLASH->SR&FLASH_SR_BSY));
	FLASH->CR &= ~FLASH_CR_PG;
	FLASH->CR |= FLASH_CR_LOCK;
}
// ===========================================================================


// ===========================================================================
void read_flash( void )
// ===========================================================================
{
	DeviceType	= *( uint32_t* )   Page_63;
	Sn			= *( uint32_t* ) ( Page_63 + 4 );
	Noc			= *( uint16_t* ) ( Page_63 + 8 );
	Speedc		= *( uint16_t* ) ( Page_63 + 10 );
}
// ===========================================================================


// ===========================================================================
void cli( void )
// ===========================================================================
{
	bool	cmd;
	char	s[RX_LEN];
	//char	s1[RX_LEN];
	int		i, j, n, m;
	int		space_index[4];

	cli_mode = TRUE;
	strcpy( cli_data, "\n\rConfig> " );		// Set prompt
	put_str( "\r\n\r\n(c) Open-PLC\r\n(open-plc.com)\r\n\r\n? - help\r\n" );
	put_str( cli_data );
	m = sizeof( space_index ) / sizeof( int );

	while( cli_mode )
	{
		if( ret_button )	// Press Enter
		{
			ret_button = FALSE;
			cmd = FALSE;

			n = strlen( usart_rx_data );
			if( n )		// Length > 0
			{
				// Trim left spaces
				j = 0;
				while( usart_rx_data[0] == ' ' )
				{
					n = strlen( usart_rx_data );
					for( j = 0; j < n; j++ )
					{
						usart_rx_data[j] = usart_rx_data[j + 1];
					}
				}

				// -----------------------------------------------------------
				// Trim right spaces
				// -----------------------------------------------------------
				n = strlen( usart_rx_data ) - 1;
				if( n > 0 )
				{
					while( usart_rx_data[n] == ' ' )
					{
						if( n >= 0 )
						{
							usart_rx_data[n] = 0;
							if( n )
							{
								n--;
							}
						}
					}
				}

				// -----------------------------------------------------------
				// Prepare cmd string
				// -----------------------------------------------------------
				n = strlen( usart_rx_data );
				for( i = 0; i < n; i++ )
				{
					if( ( usart_rx_data[i] < 32 ) || ( usart_rx_data[i] > 126 ) )
					{
						if( usart_rx_data[i] )
						{
							usart_rx_data[i] = '$';
						}
					}
					if( usart_rx_data[i] > 96 )
					{
						usart_rx_data[i] -= 32;
					}

					while( ( usart_rx_data[i] == ' ' ) && ( usart_rx_data[i + 1] == ' ' ) )
					{
						for( j = i; j < n; j++ )
						{
							usart_rx_data[j] = usart_rx_data[j + 1];
						}
					}
				}	// for( i = 0; i < n; i++ )

				for( j = 0; j < m; j++ )
				{
					space_index[j] = 0;
				}

				j = 0;
				for( i = 0; i < n; i++ )
				{
					if( ( usart_rx_data[i] == ' ' ) && ( j < m ) )
					{
						space_index[j++] = i;
					}
				}
				// -----------------------------------------------------------

				// -----------------------------------------------------------
				// Get first param
				// -----------------------------------------------------------
				if( space_index[0] )
				{
					for( j = 0; j < space_index[0]; j++ )
					{
						s[j] = usart_rx_data[j];
					}
					s[j] = 0;
				}
				else
				{
					strcpy( s, usart_rx_data );
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Quit from CLI
				// -----------------------------------------------------------
				if( strcmp( s, "Q" ) == 0 )
				{
					strcpy( cli_data, CLI );
					put_str( cli_data );
					cli_mode = FALSE;

					return;
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Help
				// -----------------------------------------------------------
				if( strcmp( s, "?" ) == 0 )
				{
					put_str( "\n\r" );
					put_str( "\n\r?       - This page" );
					put_str( "\n\rq       - Exit from Config" );
					put_str( "\n\rsh      - Show parameters" );
					put_str( "\n\rcs <Nn> - CAN speed" );
					put_str( "\n\r          Nn = 0; Speed 500000 bps" );
					put_str( "\n\r          Nn = 1; Speed 250000 bps" );
					put_str( "\n\r          Nn = 2; Speed 125000 bps" );
					put_str( "\n\r          Nn = 3; Speed 100000 bps" );
					put_str( "\n\r          Nn = 4; Speed 50000  bps" );
					put_str( "\n\r          Nn = 5; Speed 20000  bps" );
					put_str( "\n\r          Nn = 6; Speed 10000  bps" );
					put_str( "\n\r" );

					cmd = TRUE;
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Show parameters
				// -----------------------------------------------------------
				if( !cmd )
					if( strcmp( s, "SH" ) == 0 )
					//if( strcmp( usart_rx_data, "SH" ) == 0 )
					{
						switch( Speedc )
						{
							case 0:
								n = 500000;
								break;
							case 1:
								n = 250000;
								break;
							case 2:
								n = 125000;
								break;
							case 3:
								n = 100000;
								break;
							case 4:
								n = 50000;
								break;
							case 5:
								n = 20000;
								break;
							case 6:
								n = 10000;
								break;
							default:
								n = 0;
						}
						put_str( "\r\n" );
						sprintf( s, "Name        Value\r\n" );			put_str( s );
						sprintf( s, "----------  ----------\r\n" );		put_str( s );
						sprintf( s, "CAN Speed   %d bps\r\n", n );		put_str( s );

						cmd = TRUE;
					}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// CAN speed
				// -----------------------------------------------------------
				if( !cmd )
				{
					if( strcmp( s, "CS" ) == 0 )
					{
						// Get first param
						s[0] = 0;
						if( space_index[0] )
						{
							if( space_index[1] )
							{
								i = 0;
								for( j = ( space_index[0] + 1 ); j < space_index[1]; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[0] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}

							if( s[0] )
							{
								i = atoi( s );
								if( ( i >= 0 ) && ( i < 7 ) )
								{
									Speedc = i;
									write_flash();

									reset_can();
									delay_us( 100 );
									Init_Can();
								}
								else
								{
									put_str( "\r\nError CAN speed\r\n" );
								}
							}
						}	// if( space_index[0] )
						else
						{
							put_str( "\r\nCommand error\r\n" );
						}
						cmd = TRUE;
					}	// if( strcmp( s, "CS" ) == 0 )
				}	// if( !cmd )
				// -----------------------------------------------------------


				if( !cmd )
				{
					put_str( "\r\nUnknown command\r\n" );
					put_str( cli_data );
				}
				else
				{
					put_str( cli_data );
				}
			}	// if( n ) -- Length > 0
			else
			{
				put_str( cli_data );
			}
		}	// if( ret_button )
	}	// while( cli_mode )
}
// ===========================================================================
