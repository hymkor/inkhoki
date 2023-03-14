#include <stdlib.h>

#include "sprite.h"
#include "mice.h"
#include "pe.h"

static void drawbox(int dx,int dy,int color)
{
	boxdrawx( ((dx >   320  -EditSize) ?   320  -EditSize : dx ) & ~7 ,
			 ((dy > BOTTOMS-EditSize) ? BOTTOMS-EditSize : dy ) & ~7 , EditSize , EditSize , color );
}

static int select_window(int *x,int *y)
{
	int dx,dy , _dx,_dy , boxflag=0 ;
	
	writes(0,MESSAGE_LINE,"Transfer from YELLOW to RED",15);
	
	boxdrawx( REAL_LEFT+CurrentPattern.x, CurrentPattern.y, EditSize, EditSize, CopyFromColor);
	
	mouse_on();
	while( mouse_alt(&_dx,&_dy) != 0 )
		;
	
	while( mouse_alt(&dx,&dy) == 0 ){
		if( ((dx & ~7)!=(_dx & ~7) || (dy & ~7)!=(_dy & ~7) ) 
			&& dx >= REAL_LEFT  &&  dy < BOTTOMS )
		{
			mouse_off();
			if( boxflag ) drawbox(_dx,_dy,CopyToColor);
			
			boxflag = 1;
			drawbox(_dx=dx,_dy=dy,CopyToColor);
			
			mouse_on();
		}
	}
	mouse_off();
	if( boxflag ) drawbox(_dx,_dy,CopyToColor);
	
	boxdrawx( REAL_LEFT+CurrentPattern.x, CurrentPattern.y, EditSize, EditSize, CopyFromColor);
	clear_message_line();
	
	if( dx < REAL_LEFT  ||  dy > BOTTOMS  )
		return 1;
	
	if( dy > BOTTOMS-EditSize ){
		dy = BOTTOMS-EditSize;
	}
	
	if( dx > 320-EditSize ){
		dx = 320-EditSize;
	}
	
	*x = (dx-REAL_LEFT)& ~7;
	*y =  dy           & ~7;
	
	return 0;
}

static void normal(int x,int y,int *X,int *Y,int s){ *X=x;		*Y=y;	}
static void rot90 (int x,int y,int *X,int *Y,int s){ *X=s-y-1;	*Y=x;	}
static void rot180(int x,int y,int *X,int *Y,int s){ *X=s-x-1;	*Y=s-y-1;}
static void	rot270(int x,int y,int *X,int *Y,int s){ *X=y;		*Y=s-x-1;}
static void revH  (int x,int y,int *X,int *Y,int s){ *X=s-x-1;	*Y=y;	}
static void revV  (int x,int y,int *X,int *Y,int s){ *X=x;		*Y=s-y-1;}

typedef CmdStatus (*func_t)();

struct icontable copytable[]={
	{ "a)normal" , (func_t)normal	,'a' },
	{ "s)M to W" , (func_t)revV		,'s' },
	{ "d)< to >" , (func_t)revH		,'d' },
	{ "f)V to <" , (func_t)rot270	,'f' },
	{ "g)9 to 6" , (func_t)rot180	,'g' },
	{ "h)V to >" , (func_t)rot90	,'h' },
};

CmdStatus Copy()
{
	int i;
	int x,y;
	int no=select_2nd_icon( copytable , numof(copytable) );
	
	if( no < 0 )
		return CONTINUE;
	
	if( select_window(&x,&y) )	return CONTINUE;
	clear_message_line();
	
	for( i=0  ;  i < EditSize ; i++ ){
		int j;
		for( j=0 ; j < EditSize ; j++ ){
			int _i,_j,dot;
			
			(*copytable[ no ].func)(j,i,&_j,&_i,EditSize);
			
			dot = real_attr( CurrentPattern.x + _j , CurrentPattern.y + _i );
			if( dot >= 0 ){
				real_pset( x + j , y + i , dot );
			}
		}
	}
}
