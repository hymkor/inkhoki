#include <dos.h>
#include <stdio.h>
#include <string.h>

#include "video.h"
#include "pe.h"

Zahyo CurrentPattern={0,0};

int CurrentColor=15;
int Scale   = 4,/* or  8,16 */
	ScaleBit= 2,/* or  3, 4 */
	EditSize=32;/* or 16, 8 */

char default_file_name[FILENAME_MAX] = "\0";

void disp_current_color( void )
{
	box_fill( 0 , RGB_LINE<<3 , 64*2 + ((RGB_COLUMN+6)<<3)   , 24 , 0 );
	
	if( CurrentColor < 0 ){
		box_fill( 0 , BOTTOMS+8,  24 , 16 , 0 );
		box_fill( 11, BOTTOMS+15 , 2 ,  2 , 15);
		writes( 0 , RGB_LINE , "BGC" , 15 );
	}else{
		char buffer[32];
		int red=0,green=0,blue=0;
		
		pe_get_palette(CurrentColor,&blue,&red,&green);
		
		/* Ô */
		sprintf( buffer , "R[%3d]" , red );
		writes( RGB_COLUMN , RGB_LINE   , buffer , 12 );
		box_fill( GRAPH_COLUMN<<3 , RGB_LINE<<3 , red * 2 , 7 , 12 );
		
		/* —Î */
		sprintf( buffer , "G[%3d]" , green );
		writes( RGB_COLUMN , RGB_LINE+1 , buffer , 10 );
		box_fill( GRAPH_COLUMN<<3 , (RGB_LINE+1)<<3 , green * 2 , 7 , 10 );
		
		/* Â */
		sprintf( buffer , "B[%3d]" , blue );
		writes( RGB_COLUMN , RGB_LINE+2 , buffer ,  9 );
		box_fill( GRAPH_COLUMN<<3 , (RGB_LINE+2)<<3 , blue * 2 , 7 , 9 );
		
		sprintf( buffer , "%3d" , CurrentColor );
		writes( 0 , RGB_LINE , buffer, 15 );
		box_fill( 0 , BOTTOMS+8,  24 , 16 , CurrentColor );
	}
}

void disp_current_pattern_position( void )
{
	char buffer[32];
	sprintf( buffer , "Pos [%3d,%3d]" ,	CurrentPattern.x , CurrentPattern.y );
	writes( 26 , RGB_LINE   , buffer , 7);
	sprintf( buffer , "Size[%3d,%3d]" , EditSize , EditSize );
	writes( 26 , RGB_LINE+1 , buffer , 7);
#if 0
	char buffer[64];
	sprintf( buffer , "Position:[%3d,%3d]  Size:[%3d,%3d]" , CurrentPattern.x , CurrentPattern.y  , EditSize , EditSize );
	writes( 0 , MESSAGE_LINE-1  , buffer , 7);
#endif
}

CmdStatus click_real_region(int x,int y,int trig)
{
	int nextx=(x-REAL_LEFT) & ~7;
	int nexty=      y       & ~7;
	
	if( nextx != CurrentPattern.x  || nexty != CurrentPattern.y ){
		
		int xsize=EDIT_RIGHT/ Scale,
			ysize=BOTTOMS	/ Scale;
								
		xor_box( REAL_LEFT+CurrentPattern.x, CurrentPattern.y, EditSize, EditSize, 15);
		
		CurrentPattern.x = nextx;
		if( CurrentPattern.x > 128 - xsize ){
			CurrentPattern.x = 128 - xsize;
		}
								
		CurrentPattern.y = nexty;
		if( CurrentPattern.y > 128 - ysize ){
			CurrentPattern.y = 128 - ysize;
		}
		edit_screen_flush();
		disp_current_pattern_position();
		
		xor_box( REAL_LEFT+CurrentPattern.x, CurrentPattern.y, EditSize, EditSize, 15);
	}
	prevcmd = (cmdid_t)click_real_region;
	
	return CONTINUE;
}

