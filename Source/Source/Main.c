#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <VDP.h>
#include <Debug.h>
#include <Peripheral.h>
#include <USBCart.h>

uint16_t Color16( uint8_t p_Red, uint8_t p_Green, uint8_t p_Blue );

void main( void )
{
	unsigned char *pDestination;
	unsigned int Index;
	uint32_t VInt;
	uint16_t PadNew, PadOld, PadDelta;
	uint16_t Red = 3, Green = 11, Blue = 19;
	uint32_t FrameCount = 0U;

	VDP_Initialize( );
	PER_Initialize( );
	DBG_Initialize( );

	USB_Print( "SEGA Saturn SDK Sample 01 - Sprite\n" );

	VDP1_TVMR = 0x0000;
	VDP1_FBCR = 0x0000;
	VDP1_PTMR = 0x0002;
	VDP1_EWUL = 0x0000;
	VDP1_EWLR = 0x28EF;
	VDP1_EWDR = Color16( 31, 31, 31 );

	/* Enable the TV screen, 320x240NI */
	VDP2_TVMD = 0x8010;

	PadNew = 0x0000;
	PadOld = 0x0000;

	/* First color in VRAM set to a SEGA blue */
	VDP2_SetBackgroundColor( 3, 11, 19 );

	DBG_Print( 2, 1, 0xF0, "Press Z for help" );

	DBG_Print( 6, 26, 0xF0, "SEGA Saturn Sample 01 - Sprite" );
	DBG_Print( 8, 27, 0xF0, "[saturnsdk.github.io]" );

	for( ;; )
	{
		char PrintBuffer[ 80 ];
		size_t StringSize;

		VDP_WaitVBlankOut( );
		VDP_WaitVBlankIn( );

		VDP1_FBCR = 0x0001;
			
		/* Poll peripherals */
		PadOld = PadNew;
		PadNew = PER_Read( 0 );
		PadDelta = ( PadNew ^ PadOld ) & PadNew;

		if( PadNew & PER_START )
		{
			break;
		}

		utoa( FrameCount, PrintBuffer, 10 );
		StringSize = strlen( PrintBuffer );

		/* Clear the nine characters before the last one if the frame counter
		 * loops around */
		if( FrameCount == 0 )
		{
			DBG_Print( 30, 0, 0x00, "         " );
		}

		DBG_Print( 40 - StringSize, 0, 0xF0, PrintBuffer );

		++FrameCount;

		/* Change the background color */

		VDP2_SetBackgroundColor( Red, Green, Blue );
	}

	PER_Shutdown( );
	VDP_Shutdown( );
}

uint16_t Color16( uint8_t p_Red, uint8_t p_Green, uint8_t p_Blue )
{
	return	( ( ( p_Red ) & 0x1F ) |
			( ( p_Green ) & 0x1F ) << 5 |
			( ( p_Blue ) & 0x1F ) << 10 |
			0x8000 );
}

