/*
	Name:			hex2bin.c
	Purpose:		CAN Gate project
	Author:			Alexander Suvorov
	Created:		2016/07
	Modified by:
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/

uint8_t hex2bin( char c )
{
	if( c >= 'A' )
	{
		c -= 'A';
		c += 10;
	}
	else
	{
		c -= '0';
	}
	return c;
}
