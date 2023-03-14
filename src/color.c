#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <machine.h>
#include "video.h"
#include "mice.h"
#include "pe.h"

/*
 * nnnR[012].............
 */

CmdStatus click_color_region(int x,int y,int trig)
{
	int rgb[3];
	int plane=((y-128)>>3);
	
	assert( 0<=plane && plane<3 );
	
	if( CurrentColor < 0 )
		return CONTINUE;
	
	pe_get_palette( CurrentColor , &rgb[2], &rgb[0], &rgb[1] );

	if( x >= GRAPH_COLUMN*8 ){
		int X,_X;
		
		_X = (x >>= 1);
		
		while( mouse_alt(&X,NULL) != 0 ){
			X >>= 1;
			if( X != _X ){
				rgb[plane] += (X-_X);
				pe_set_palette( CurrentColor , rgb[2] , rgb[0] , rgb[1] );
				disp_current_color();
			}
			_X = X;
		}
	}
	mouse_locate(x,y);
	return CONTINUE;
}
