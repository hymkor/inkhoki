#ifndef MICE_H
#define MICE_H

#include "macros.h"

void	_asm_NW(SOURCE , WORD);
WORD	_asm_WW(SOURCE , WORD);
void	_asm_WBWW(SOURCE , WORD , BYTE , WORD , WORD);

#define	mouse_init()		~_asm_WW("\n\tINT\t33h\n",0)
#define	mouse_on()			 _asm_NW("\n\tINT\t33h\n",1)
#define mouse_off()			 _asm_NW("\n\tINT\t33h\n",2)
#define mouse_locate(x,y)	 _asm_WBWW("\n\tINT\t33h\n",4,0,(x)<<1,y)

int	mouse    ( int *x , int *y );
int	mouse_alt( int *x , int *y );

unsigned char _asm_strig( const char * , short , unsigned char (*)() , unsigned char (*)() , short );
#define	strig()		(_asm_strig("\n\tINT\t15h\n",0x8400,_asm_strig,_asm_strig,0)>>4)

void stick(int *x1,int *y1,int *x2,int *y2);	/* ãAÇ¡ÇƒóàÇÈç¿ïWÇÕÅA0--360 ( 6--354 ? )*/
#define stickA(x,y) stick(x,y,NULL,NULL)	
#define stickB(x,y) stick(NULL,NULL,x,y)

#endif /* MICE_H */
