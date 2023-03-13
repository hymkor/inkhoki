#include <stdlib.h> /* for malloc */
#include "mice.h"
#include "video.h"
#include "pe.h"

int is_forcus=0;
int forcus_x , forcus_y , forcus_width , forcus_height ;

void forcus_off( void )
{
	xor_box( forcus_x<<ScaleBit , forcus_y<<ScaleBit , forcus_width<<ScaleBit , forcus_height<<ScaleBit , 15 );
}

void sort_zahyo(int *x1,int *y1,int *x2,int *y2)
{
	int tmp;
	if( *x1 > *x2 ){
		tmp = *x1; *x1=*x2; *x2=tmp;
	}
	if( *y1 > *y2 ){
		tmp = *y1; *y1=*y2; *y2=tmp;
	}
}

static void forcus_box(int x1,int y1,int x2,int y2,int c)
{
	sort_zahyo(&x1,&y1,&x2,&y2);
	xor_box( x1<<ScaleBit , y1<<ScaleBit , (x2-x1+1)<<ScaleBit , (y2-y1+1)<<ScaleBit , c);
}
static void forcus_box2(int x1,int y1,int w,int h,int c)
{
	xor_box( x1<<ScaleBit , y1<<ScaleBit , w<<ScaleBit , h<<ScaleBit , c );
}

void movebox(int x1,int y1,int x2,int y2,int width,int height)
{
	int i,j;
	
	if( y1 > y2 ){
		for( i=0 ; i<height ; i++ ){
			if( x1 > x2 ){
				for( j=0 ; j<width ; j++ )
					dotset( x2+j , y2+i , dotget(x1+j,y1+i) );
			}else{
				for( j=width-1 ; j>=0 ; j-- )
					dotset( x2+j , y2+i , dotget(x1+j,y1+i) );
			}
		}
	}else{
		for( i=height-1 ; i>=0 ; i-- ){
			if( x1 > x2 ){
				for( j=0 ; j<width ; j++ )
					dotset( x2+j , y2+i , dotget(x1+j,y1+i) );
			}else{
				for( j=width-1 ; j>=0 ; j-- )
					dotset( x2+j , y2+i , dotget(x1+j,y1+i) );
			}
		}
	}
}

CmdStatus click_edit_region(int x,int y,int trig)
{
	int x0 = x >> ScaleBit, y0 = y >> ScaleBit;
	
	if( trig & 1 ){
		if( x0 == 0 || x0 == EditSize-1 || y0 == 0 || y0 == EditSize-1 ){
			xor_box( REAL_LEFT+CurrentPattern.x, CurrentPattern.y, EditSize, EditSize, 15);
			dotset( x0 , y0 ,CurrentColor);
			xor_box( REAL_LEFT+CurrentPattern.x, CurrentPattern.y, EditSize, EditSize, 15);
		}else{
			dotset( x0 , y0 ,CurrentColor);
		}
		prevcmd = (cmdid_t)click_edit_region;
	}else if( trig & 2 ){
		int x1=x0 , y1=y0 ;
		
		if( 	prevcmd == cmdid_box_forcus  &&  forcus_x <= x0  &&  forcus_y <= y0  
			&&  x0 < forcus_x+forcus_width  &&  y0 < forcus_y+forcus_height  )
		{
			int dx=0,dy=0 , _X=x1,_Y=y1;
			mouse_on();
			for(;;){
				int X,Y,TRIG=mouse_alt(&X,&Y) ;
				
				if( (TRIG & 2)==0 )
					break;
				
				X >>= ScaleBit ; Y >>= ScaleBit;
				
				if( ( X != _X || Y != _Y ) ){
					int  X0 , Y0 , X1 , Y1;
					
					X0 = forcus_x + X - x0 ; Y0 = forcus_y + Y - y0 ;
					X1 = X0 + forcus_width -1 ; Y1 = Y0 + forcus_height - 1;
					
					mouse_off();
					forcus_box( forcus_x+dx , forcus_y+dy , forcus_x+forcus_width+dx-1 , forcus_y+forcus_height+dy-1,15);
					if( 0<=X0  &&  X1<EditSize )
						dx = (x1=X)-x0 ;
					if( 0<=Y0  &&  Y1<EditSize )
						dy = (y1=Y)-y0;
					forcus_box( forcus_x+dx , forcus_y+dy , forcus_x+forcus_width+dx-1 , forcus_y+forcus_height+dy-1,15);
					mouse_on();
				}
				_X = X ; _Y = Y;
				
				if( (TRIG & 1)  &&  (x0 != x1 ||  y0 != y1) ){
					mouse_off();
					forcus_off();
					forcus_box2( forcus_x+dx , forcus_y+dy , forcus_width , forcus_height , 15 );
					
					xor_box( REAL_LEFT+CurrentPattern.x, CurrentPattern.y, EditSize, EditSize, 15);
					movebox( forcus_x,forcus_y , forcus_x+dx,forcus_y+dy , forcus_width,forcus_height);
					xor_box( REAL_LEFT+CurrentPattern.x, CurrentPattern.y, EditSize, EditSize, 15);
					
					prevcmd = (cmdid_t)click_edit_region;
					
					while( mouse_alt(NULL,NULL) != 0 )
						;
					
					return CONTINUE;
				}
			}
			mouse_off();
			forcus_off();
			forcus_box2( forcus_x+dx , forcus_y+dy , forcus_width , forcus_height , 15 );
			prevcmd = (cmdid_t)click_edit_region;
			
		}else{	/* 領域指定 */
			int _X=x1,_Y=y1;
			
			forcus_box(x0,y0,x1,y1,15); /* 表示 */
			mouse_on();
			for(;;){
				int X , Y , TRIG = mouse_alt( &X , &Y );
			
				if( (TRIG & 2)==0 )
					break;
			
				X >>= ScaleBit ; Y >>= ScaleBit;
			
				if( X != _X || Y != _Y ){
					mouse_off();
					forcus_box(x0,y0,x1,y1,15); /* 消去 */
					if( X < EditSize )
						x1=X ; 
					if( Y < EditSize )
						y1=Y ;
					forcus_box(x0,y0,x1,y1,15); /* 表示 */
					mouse_on();
				}
				_X=X;_Y=Y;
			}
			mouse_off();
		
			if( x0 == x1  &&  y0 == y1 ){ /* 同じ座標だったら、ドットのカラーを拾う */
				forcus_box(x0,y0,x1,y1,15);
				CurrentColor = real_attr( CurrentPattern.x+x0 , CurrentPattern.y+y0 );
				disp_current_color();
				prevcmd = cmdid_spoit;
			}else{
				sort_zahyo(&x0,&y0,&x1,&y1);
				forcus_x = x0 ; forcus_y = y0;
				forcus_width = x1-x0+1 ; forcus_height = y1-y0+1;
				is_forcus = 2;
				prevcmd = cmdid_box_forcus;
			}
		}
	}
	return CONTINUE ;
}

CmdStatus raise_command( int x,int y ,int trig)
{
	region_t region;
	
	region = which_region(x,y,NULL,NULL);
	if( region != NOWHERE_REGION ){
		const static CmdStatus (*jump_table[])(int x,int y,int trig) = {
			click_edit_region , 
			click_icon_region ,
			click_real_region ,
			click_palette_region ,
			click_color_region ,
		};
		if( ( *jump_table[ region - 1 ] )( x , y , trig ) == ABORT )
			return ABORT;
	};

	return CONTINUE;
}

CmdStatus SpaceKey( void )
{
	int x,y;
	(void)mouse_alt(&x,&y);
	return raise_command(x,y,1);
}

CmdStatus CursorRight( void )
{	
	int x,y;
	(void)mouse_alt(&x,&y);
	if( x < 316 )
		mouse_locate(x+4,y);
}
CmdStatus CursorLeft( void )
{	
	int x,y;
	(void)mouse_alt(&x,&y);
	if( x > 4 )
		mouse_locate(x-4,y);
}
CmdStatus CursorUp( void )
{	
	int x,y;
	(void)mouse_alt(&x,&y);
	if( y > 4 )
		mouse_locate(x,y-4);
}
CmdStatus CursorDown( void )
{	
	int x,y;
	(void)mouse_alt(&x,&y);
	if( y < 196 )
		mouse_locate(x,y+4);
}
