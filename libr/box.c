#include "video.h"

void xor_box(int x1,int y1,int w,int h,int c)
{
	int x2=x1+w-1 , y2=y1+h-1;
	int x,y;
	int tmp;
	
	if( x1 > x2 ){ tmp=x1; x1=x2; x2=tmp; }
	if( y1 > y2 ){ tmp=y1; y1=y2; y2=tmp; }
	
	for( x=x1 ; x <=x2 ; x++ ){
		GRAM[ y1 ][ x ] ^= c;
		GRAM[ y2 ][ x ] ^= c;
	}
	
	for( y=y1+1 ; y < y2 ; y++ ){
		GRAM[ y ][ x1 ] ^= c;
		GRAM[ y ][ x2 ] ^= c;
	}
}

void box_fill(int x1,int y1,int w,int h,int c)
{
	int x2=x1+w-1 , y2=y1+h-1;
	int x,y;
	int tmp;
	
	if( x1 > x2 ){ tmp=x1; x1=x2; x2=tmp; }
	if( y1 > y2 ){ tmp=y1; y1=y2; y2=tmp; }

	for( y=y1; y<=y2 ; y++ ){
		for( x=x1; x<=x2 ; x++){
			GRAM[ y ][ x ] = c;
		}
	}
}
void xor_box_fill(int x1,int y1,int w,int h,int c)
{
	int x2=x1+w-1 , y2=y1+h-1;
	int x,y;
	int tmp;
	
	if( x1 > x2 ){ tmp=x1; x1=x2; x2=tmp; }
	if( y1 > y2 ){ tmp=y1; y1=y2; y2=tmp; }

	for( y=y1; y<=y2 ; y++ ){
		for( x=x1; x<=x2 ; x++){
			GRAM[ y ][ x ] ^= c;
		}
	}
}