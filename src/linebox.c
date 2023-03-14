#include <malloc.h>
#include <conio.h>
#include "video.h"
#include "mice.h"
#include "pe.h"

static int _Line(int x1,int y1,int x2,int y2)
{
	int x,y,r,dx,dy,signy;
	
	if( x1 > x2 ){
		int tmp;
		tmp=x1 ; x1=x2 ; x2=tmp ;
		tmp=y1 ; y1=y2 ; y2=tmp ;
	}
	
	dx = x2-x1+1;
	
	if( y1 < y2 ){
		signy = +1;
		dy = y2-y1;
	}else{
		signy = -1;
		dy = y1-y2;
	}
	
	x=x1; y=y1; r=dx/2;
	
	/* r は y 方向の増分の小数部分。
	 * (++x)毎に
	 *    r/dx += dy/dx; <=> r += dy
	 * rがdxよりも大きいと、整数部分つまり yを増やす。
	 */
	
	while( x <= x2 ){
		dotset( x , y , CurrentColor );
		r += dy;
		while( r >= dx ){
			dotset( x , y , CurrentColor );
			y += signy;
			r -= dx;
		}
		++x;
	}
	return 0;
}

int getpoint(int *x,int *y)
{
	int c;
	int trig;
	for(;;){
		while( mouse_alt(x,y) != 0 )
			;
		mouse_on();
		while( (trig=mouse_alt(x,y)) == 0 ){
			if( kbhit()  &&  getch()=='\x1b'  )
				return -1;
		}
		mouse_off();
		if( *y < PALET_TOP )
			return trig;
		
		c = *x/PALET_DOT_SIZE+((*y-PALET_TOP)/PALET_DOT_SIZE)*PALET_NUM_LINE;
		if( c >= 256 ){
			if( c < 258  ||  ( 320 <= c && c < 322 ) ){
				c = -1;
			}else{
				return trig;
			}
		}
		CurrentColor = c;
		disp_current_color();
		
	}/* end for(;;) */
}/* end function */

int cross_cursor(int x,int y)
{
	x <<= ScaleBit ; y <<= ScaleBit ;
	
	boxfillx( x,0 , Scale,128 , 15 );
	boxfillx( 0,y , 128,Scale , 15 );
}

CmdStatus Line(void)
{
	int x1,y1,x2,y2;
	
	writes(0,MESSAGE_LINE,"<LINE MODE>",15);
	
	if( getpoint(&x1,&y1) & 2 )
		return CONTINUE;
	
	clear_message_line();
	
	if( x1 >= EDIT_RIGHT || y1 >= BOTTOMS )
		return CONTINUE;
	
	for(;;){
		int trig;
		cross_cursor( x1>>ScaleBit, y1>>ScaleBit );
		
		if( (trig=getpoint(&x2,&y2))==-1 )
			return CONTINUE;
		
		cross_cursor( x1>>ScaleBit , y1>>ScaleBit );
		
		if( which_region(x2,y2,NULL,NULL) != EDIT_REGION ){
			clear_message_line();
			return CONTINUE;
		}
		
		/* 右ボタン、ダブルクリックでただちに終了 */
		if( (trig&2) || (x1==x2 && y1==y2) ){
			clear_message_line();
			return CONTINUE;
		}
		
		_Line(x1>>ScaleBit , y1>>ScaleBit , x2>>ScaleBit , y2>>ScaleBit );
		x1=x2;	y1=y2;
	}
}

CmdStatus Box(void)
{
	int x1,y1,x2,y2,y;
	
	writes(0,MESSAGE_LINE,"<BOXFILL MODE>",15);
	
	/* 始点の取得 */
	if( getpoint(&x1,&y1) & 2 )
		goto quit;
	
	clear_message_line();
	
	if( x1 >= EDIT_RIGHT || y1 >= BOTTOMS )
		goto quit;
	
	x1 >>= ScaleBit;	y1 >>= ScaleBit;
	
	/*  終点の取得 */
	
	cross_cursor( x1,y1 );
	if( getpoint(&x2,&y2) & 2 )
		goto quit;
	
	cross_cursor( x1,y1 );
	
	if( x2 >= EDIT_RIGHT || y2 >= BOTTOMS )
		return CONTINUE;
	x2 >>= ScaleBit;	y2 >>= ScaleBit;
	
	
	if( x1 > x2 ){ int tmp=x2; x2=x1; x1=tmp;}
	if( y1 > y2 ){ int tmp=y2; y2=y1; y1=tmp;}
	
	for( y=y1; y<=y2; y++){
		int x;
		for( x=x1; x<=x2; x++){
			dotset( x,y,CurrentColor );
		}
	}
quit:
	clear_message_line();
	return CONTINUE;
}

#if 0

static void modify_zahyo(int *x1,int *y1,int *x2,int *y2)
{
	int tmp;
	if( *x1 > *x2 ){
		tmp = *x1; *x1=*x2; *x2=tmp;
	}
	if( *y1 > *y2 ){
		tmp = *y1; *y1=*y2; *y2=tmp;
	}
}

static void XorBox(int x1,int y1,int x2,int y2,int c)
{
	modify_zahyo(&x1,&y1,&x2,&y2);
	xor_box( x1<<ScaleBit , y1<<ScaleBit , (x2-x1+1)<<ScaleBit , (y2-y1+1)<<ScaleBit , c);
}
CmdStatus BoxCopy( void )
{
	int trig;
	int x1,y1;
	int x2,y2,y;
	int prev_x  , prev_y  , sw=0 ;
	int to_x , to_y , size_x , size_y;
	short *buffer;
	short *ptr;
	
	
	while( mouse_alt(NULL,NULL) != 0 )
		;
	
	/* 転送元 左上の座標を入力 */
	mouse_on();
	while( mouse_alt(&x1,&y1) == 0 ){
		if( kbhit() &&  
		;
	mouse_off();
	
	while( mouse_alt(NULL,NULL) != 0 )
		;
	
	if( x1 > 128  ||  y1 > 128 )
		return CONTINUE;
	
	x1 >>= ScaleBit;
	y1 >>= ScaleBit;
	
	/* 転送元 右下入力 */
	prev_x = x1 , prev_y = y1 , sw=0 ;
	
	mouse_on();
	while( mouse_alt( &x2 , &y2 ) == 0 ){
		x2 >>= ScaleBit;
		y2 >>= ScaleBit;
		if( x2 != prev_x  ||  y2 != prev_y ){
			mouse_off();
			if( sw )
				XorBox(x1,y1,prev_x,prev_y,CopyFromColor);
			
			prev_x=x2 ; prev_y=y2;
			
			if( x2 < EditSize  &&  y2 < EditSize ){
				XorBox(x1,y1,x2,y2,CopyFromColor);
				sw=1;
			}else{
				sw=0;
			}
			mouse_on();
		}
	}
	mouse_off();
	if( ! sw )
		return CONTINUE;
	
	x2 = prev_x ;
	y2 = prev_y ;
	
	while( mouse_alt(NULL,NULL) != 0 )
		;
	
	modify_zahyo(&x1,&y1,&x2,&y2);
	
	/* 転送先 入力 */
	size_x=x2-x1 , size_y=y2-y1;
	
	sw = 0;
	mouse_on();
	while( mouse_alt(&to_x,&to_y) == 0 ){
		to_x >>= ScaleBit; to_y >>= ScaleBit;
		if( to_x != prev_x  ||  to_y != prev_y ){
			mouse_off();
			if( sw )
				XorBox( prev_x , prev_y , prev_x + size_x , prev_y + size_y , CopyToColor );
			
			prev_x = to_x ; prev_y = to_y;
			
			if( to_x+size_x < EditSize  &&  to_y+size_y < EditSize ){
				XorBox( to_x , to_y , to_x + size_x , to_y + size_y , CopyToColor );
				sw=1;
			}else{
				sw=0;
			}
			mouse_on();
		}
	}
	mouse_off();
	XorBox( x1 , y1 , x2 , y2 , CopyFromColor );
	
	if( ! sw )
		return CONTINUE;
	
	to_x >>= ScaleBit;
	to_y >>= ScaleBit;
	XorBox( to_x , to_y , to_x + size_x , to_y + size_y , CopyToColor );

	while( mouse_alt(NULL,NULL) != 0 )
		;
	
	ptr = buffer = malloc( (x2-x1+1)*(y2-y1+1)*sizeof(short) );
	if( buffer == NULL )
		return CONTINUE;
	
	for(  y = y1 ; y <= y2 ; y++ ){
		int x;
		for( x = x1 ; x <= x2 ; x++ ){
			*ptr++ = real_attr( CurrentPattern.x + x , CurrentPattern.y + y );
		}
	}
	ptr = buffer;
	for( y = to_y ; y <= to_y + size_y ; y++ ){
		int x;
		for( x = to_x ; x<= to_x + size_x ; x++ ){
			if( *ptr >= 0 ){
				real_pset( CurrentPattern.x+x , CurrentPattern.y+y , *ptr );
			}else{
				real_preset( CurrentPattern.x+x , CurrentPattern.y+y );
			}
			ptr++;
		}
	}
	edit_screen_flush();
	
	free( buffer );
	return CONTINUE;
}
#endif
