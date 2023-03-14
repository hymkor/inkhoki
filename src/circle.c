#include "mice.h"
#include "pe.h"

static void HLine( int x1,int x2,int y,int col)
{
	if( y < 0 || y >= EditSize )
		return;

	if( x1 <  0 )
		x1 = 0;
	
	if( x2 >= EditSize )
		x2 = EditSize-1;
	
	while( x1 <= x2 )
		dotset( x1++ , y , col );
}

CmdStatus Circle(void)
{
	unsigned  a,b;
	int x0,y0,x1,y1,i;
	unsigned long b2;
	
	/* ’†SÀ•W‚ð“ü—Í */
	writes(0,MESSAGE_LINE,"<CIRCLE MODE>",15);

	if( (getpoint(&x0,&y0) & 2) || x0 >= 128 || y0 >= 128 )
		return CONTINUE;
	
	x0 >>= ScaleBit;
	y0 >>= ScaleBit;
	cross_cursor( x0,y0 );
	
	/* ‰~Žüã‚Ìˆê“_‚ð“ü—Í */
	
	if( (getpoint(&x1,&y1) & 2) || x1 > 128 || y1 > 128 )
		goto quit;
	
	cross_cursor( x0,y0 );
	
	x1 >>= ScaleBit;	y1 >>= ScaleBit;
	
	if( x0 > x1 ){int tmp=x0;x0=x1;x1=tmp;}
	if( y0 > y1 ){int tmp=y0;y0=y1;y1=tmp;}
	
	a = x1-x0;	b = y1-y0;
	b2 = (unsigned long)b*b;
	
	for( i=0 ; i<= b ; i++ ){
		int w = (a*intsqrt(b2-i*i,NULL)+b-1)/b;
		int L = x0+(a-w)/2 , R=x0+(a+w+1)/2 ;
		
		HLine( L,R , y0+(b-i  )/2 , CurrentColor );
		HLine( L,R , y0+(b+i+1)/2 , CurrentColor );
	}
quit:
	clear_message_line();
	return CONTINUE;
}
