#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <VDP.h>
#include <Debug.h>
#include <Peripheral.h>
#include <USBCart.h>

#define BOOL	int
#define TRUE	1
#define FALSE	0

uint16_t Color16( uint8_t p_Red, uint8_t p_Green, uint8_t p_Blue );

const uint16_t SpriteWidth = 64;
const uint16_t SpriteHeight = 64;
const uint16_t SpriteStartX = 320 / 2;
const uint16_t SpriteStartY = 240 / 2;

BOOL HelpHidden = TRUE;

void ToggleHelp( void );

void main( void )
{
	unsigned char *pDestination;
	unsigned int Index;
	uint32_t VInt;
	uint16_t PadNew, PadOld, PadDelta;
	uint32_t FrameCount = 0U;
	uint16_t *pTexture =
		( uint16_t * )( VDP1_VRAM + 0x40000 );
	int Row, Column;
	uint16_t SpriteColor = Color16( 8, 31, 8 );
	int16_t SpriteX = SpriteStartX, SpriteY = SpriteStartY;
	int16_t SpriteVelX = 0, SpriteVelY = 0;
	uint16_t HalfSpriteWidth = SpriteWidth / 2;
	uint16_t HalfSpriteHeight = SpriteHeight / 2;

	VDP_Initialize( );
	PER_Initialize( );
	DBG_Initialize( );

	USB_Print( "SEGA Saturn SDK Sample 01 - Sprite\n" );

	/* Set the priority layers of the sprites */
    VDP2_PRISA = 0x0101;
    VDP2_PRISB = 0x0101;
    VDP2_PRISC = 0x0101;
    VDP2_PRISD = 0x0101;

	/* Back screen mode - one color for the entire screen */
	VDP2_BKTAU = 0x0000;

	VDP1_TVMR = 0x0000;
	VDP1_FBCR = 0x0000;
	VDP1_PTMR = 0x0002;
	VDP1_EWUL = 0x0000;
	/* 320x240 */
	VDP1_EWLR = 0x50EF;
	/* Transparent */
	VDP1_EWDR = 0x0000;

	/* Enable the TV screen, 320x240NI */
	VDP2_TVMD = 0x8010;

	/* Gamepad button states */
	PadNew = 0x0000;
	PadOld = 0x0000;

	/* First color in VRAM set to a SEGA blue */
	VDP2_SetBackgroundColor( 3, 11, 19 );

	DBG_Print( 2, 1, 0xF0, "Press Z for help" );

	DBG_Print( 20 - ( strlen( "SEGA Saturn Sample 01 - Sprite" ) / 2 ), 26,
		0xF0, "SEGA Saturn Sample 01 - Sprite" );
	DBG_Print( 20 - ( strlen( "[saturnsdk.github.io]" ) / 2 ), 27, 0xF0,
		"[saturnsdk.github.io]" );

	/* Create the sprite data */
	for( Column = 0; Column < SpriteWidth; ++Column )
	{
		for( Row = 0; Row < SpriteHeight; ++Row )
		{
			pTexture[ Column * SpriteWidth + Row ] = SpriteColor;
		}
	}

	/* Initialise command list */
	VDP1_ClearCommandList( );
	VDP1_EndCommandList( );

	for( ;; )
	{
		char PrintBuffer[ 80 ];
		size_t StringSize;
		uint16_t SpriteAccelX = 1, SpriteAccelY = 1;

		VDP_WaitVBlankOut( );
		VDP_WaitVBlankIn( );

		VDP1_FBCR = 0x0001;
			
		/* Poll peripherals */
		PadOld = PadNew;
		PadNew = PER_Read( 0 );
		PadDelta = ( PadNew ^ PadOld ) & PadNew;

		if( PadDelta & PER_Z )
		{
			ToggleHelp( );
		}

		if( PadNew & PER_START )
		{
			break;
		}

		if( PadNew & PER_RBUTTON )
		{
			SpriteAccelX = 5;
			SpriteAccelY = 5;
		}

		if( PadDelta & PER_LBUTTON )
		{
			SpriteX = SpriteStartX;
			SpriteY = SpriteStartY;
		}

		SpriteVelY = 0;
		if( PadNew & PER_UP )
		{
			SpriteVelY -= 1;
		}
		if( PadNew & PER_DOWN )
		{
			SpriteVelY += 1;
		}

		SpriteVelX = 0;
		if( PadNew & PER_LEFT )
		{
			SpriteVelX -= 1;
		}
		if( PadNew & PER_RIGHT )
		{
			SpriteVelX += 1;
		}

		/* Set the position, clamping to the bounds of the screen */
		SpriteX += SpriteVelX * SpriteAccelX;

		if( ( SpriteX - HalfSpriteWidth ) < 0 )
		{
			SpriteX = HalfSpriteHeight;
		}
		if( ( SpriteX + HalfSpriteWidth ) > 320 )
		{
			SpriteX = 320 - HalfSpriteWidth;
		}

		SpriteY += SpriteVelY * SpriteAccelY;

		if( ( SpriteY - HalfSpriteHeight ) <= 0 )
		{
			SpriteY = HalfSpriteHeight;
		}
		if( ( SpriteY + HalfSpriteHeight ) > 240 )
		{
			SpriteY = 240 - HalfSpriteHeight;
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

		VDP1_ClearCommandList( );
		VDP1_SetSystemClipCoordinates( 320, 240 );
		VDP1_SetLocalCoordinates( 0, 0 );

		VDP1_DrawSpriteNormalRGB( SpriteX - ( HalfSpriteWidth ),
			SpriteY - ( HalfSpriteHeight ), SpriteWidth, SpriteHeight,
			0x40000 );

		VDP1_EndCommandList( );
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

void ToggleHelp( void )
{
	if( HelpHidden )
	{
		DBG_Print( 4, 3, 0xF0, "Up       - Move sprite up" );
		DBG_Print( 4, 4, 0xF0, "Down     - Move sprite down" );
		DBG_Print( 4, 5, 0xF0, "Left     - Move sprite left" );
		DBG_Print( 4, 6, 0xF0, "Right    - Move sprite right" );
		DBG_Print( 4, 7, 0xF0, "R [HOLD] - Accelerate move speed" );
		DBG_Print( 4, 8, 0xF0, "L        - Reset position" );

		HelpHidden = FALSE;
	}
	else
	{
		int Index;
		for( Index = 0; Index < 10; ++Index )
		{
			DBG_ClearLine( 3 + Index );
		}

		HelpHidden = TRUE;
	}
}

