#include <stdio.h>
#include "pe.h"

CmdStatus save_4dotmask(FILE *fp,int x1,int y1,int width,int height)
{
	int x,y;
	int x2=x1+width , y2=y1+height ;
	
	for( y=y1 ; y<y2 ; y++ ){
		for( x=x1 ; x<x2 ; x+=4 ){
			putc( is_bg(x+3,y) ? 0xFF : 0 , fp );
			putc( is_bg(x+2,y) ? 0xFF : 0 , fp );
			putc( is_bg(x+1,y) ? 0xFF : 0 , fp );
			putc( is_bg(x+0,y) ? 0xFF : 0 , fp );
		}
	}
	for( y=y1 ; y<y2 ; y++ ){
		for( x=x1 ; x<x2 ; x+=4 ){
			putc( real_color(x+3,y) , fp );
			putc( real_color(x+2,y) , fp );
			putc( real_color(x+1,y) , fp );
			putc( real_color(x  ,y) , fp );
		}
	}
	return CONTINUE;
}

CmdStatus load_4dotmask(FILE *fp,int x1,int y1,int width,int height)
{
	int x2=x1+width , y2=y1+height ;
	int x,y;
	
	for( y=y1 ; y<y2 ; y++ ){
		for( x=x1 ; x<x2 ; x+=4 ){
			if( getc(fp) )	real_preset(x+3,y);		else real_pset(x+3,y,0);
			if( getc(fp) )	real_preset(x+2,y);		else real_pset(x+2,y,0);
			if( getc(fp) )	real_preset(x+1,y);		else real_pset(x+1,y,0);
			if( getc(fp) )	real_preset(x  ,y);		else real_pset(x  ,y,0);
		}
	}
	for( y=y1 ; y<y2 ; y++ ){
		for( x=x1 ; x<x2 ; x+=4 ){
			int c;
			c=getc(fp);	if( ! is_bg(x+3,y) ) real_pset(x+3,y,c);
			c=getc(fp);	if( ! is_bg(x+2,y) ) real_pset(x+2,y,c);
			c=getc(fp);	if( ! is_bg(x+1,y) ) real_pset(x+1,y,c);
			c=getc(fp);	if( ! is_bg(x  ,y) ) real_pset(x  ,y,c);
		}
	}
	return CONTINUE;
}
