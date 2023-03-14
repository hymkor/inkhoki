#include <stdio.h>
#include "mice.h"
#include "pe.h"

CmdStatus save_standard(FILE *fp,int x1,int y1,int w,int h)
{
	int x2=x1+w , y2=y1+h;
	int x,y;
	
	for( y=y1 ; y<y2 ; y++){
		int x;
		for( x=x1; x<x2; x++){
			putc( real_color(x,y) , fp );
		}
	}
	
	for( y=y1 ; y<y2 ; y++){
		int bitbuf=0,bitcnt=0;
		for( x=x1; x<x2; x++){
			bitbuf <<=1;
			if( is_bg(x,y) )
				bitbuf |= 1;
			if( ++bitcnt >=8 ){
				putc( bitbuf , fp );
				bitbuf = bitcnt = 0;
			}
		}
	}
	return CONTINUE;
}

CmdStatus load_standard(FILE *fp,int x1,int y1,int width,int height)
{
	int length = intsqrt( filelength(fileno(fp))*8/9 , NULL ) & ~7;
	int x,y,x2,y2;
	
	if( length < width )
		x2=x1+length;
	else
		x2=x1+width; y2=y1+height;
	
	if( length < height )
		y2=y1+length;
	else
		y2=y1+height;
	
	for( y=y1 ; y<y2 ; y++ ){
		for( x=x1; x<x2 ; x++){
			int c=getc(fp);
			if( feof(fp) )
				return CONTINUE;
			real_pset(x,y,c);
		}
		if( length > width )
			fseek(fp,length-width,SEEK_CUR);
	}
	for( y=y1 ; y<y2 ; y++ ){
		x=x1;
		while( x < x2 ){
			int c=getc(fp),i;
			if( feof(fp) )
				return CONTINUE;
			
			for( i=0 ; i<8 ; i++ ){
				c <<= 1;
				if( c & 256 )
					real_preset(x,y);
				if( ++x >= x2 )
					goto exit_xloop;
			}
		}
	exit_xloop: ;
		if( length-width >= 8 )
			fseek(fp,(length-width)/8,SEEK_CUR);
	}
	return CONTINUE;
}
