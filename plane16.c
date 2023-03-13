#include <stdio.h>
#include "pe.h"

CmdStatus save_plane16(FILE *fp,int x1,int y1,int width,int height)
{
	int x2=x1+width , y2=y1+height;
	int x,y,bit;
	
	for( bit=0 ; bit<16 ; bit<<=1 ){
		for( y=y1 ; y<y2 ; y++ ){
			int countor=0,buffer=0;
			for( x=x1 ; x<x2 ; x++ ){
				buffer <<= 1;
				if( real_color(x,y) & bit )
					buffer |= 1;
				if( countor >= 8 ){
					putc( buffer , fp );
					countor = 0;
				}
			}
			if( countor > 0 )
				putc( buffer , fp );
		}
	}
	for( y=y1 ; y<y2 ; y++ ){
		int countor=0,buffer=0;
		for( x=x1 ; x<x2 ; x++ ){
			buffer <<= 1;
			if( is_bg(x,y) )
				buffer |= 1;
			if( countor >= 8 ){
				putc( buffer , fp );
				countor =0;
			}
		}
		if( countor > 0 )
			putc( buffer , fp );
	}
	return CONTINUE;
}

CmdStatus load_plane16(FILE *fp,int x1,int y1,int width,int height)
{
	int x2=x1+width , y2=y1+height;
	int x,y,plane;
	
	for( plane=0 ; plane<16 ; plane<<=1 ){
		for( y=y1 ; y<y2 ; y++ ){
			x=x1;
			while( x<x2 ){
				int bit;
				int c=getc(fp);
				
				if( feof(fp) )
					return CONTINUE;
				
				for( bit=0 ; bit<256 ; bit<<=1 ){
					if( c & bit )
						real_pset( x , y , real_color(x,y) |  plane );
					else
						real_pset( x , y , real_color(x,y) & ~plane );
					if( ++x >= x2 )
						goto exit_xloop;
				}
			}
			exit_xloop:;
		}
	}
	for( y=y1 ; y<y2 ; y++ ){
		x=x1;
		while( x<x2 ){
			int bit;
			int c=getc(fp);
			
			if( feof(fp) )
				return CONTINUE;
			
			for( bit=0 ; bit<256 ; bit<<=1 ){
				if( c & bit )
					real_preset( x , y );
				if( ++x >= x2 )
					goto quit_xloop;
			}
		}
		quit_xloop:;
	}
	return CONTINUE;
}
