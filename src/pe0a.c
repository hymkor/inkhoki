#include <conio.h>
#include <stdio.h>
#include "pe.h"
#include "mice.h"

static void keybuf_clear()
{
	while( kbhit() )
		(void) getch();
	while( mouse_alt(NULL,NULL) )
			;
}

CmdStatus AreYouSure(const char *message)
{
	keybuf_clear();
	
	writes(0,MESSAGE_LINE,	message,15 );
	writes(0,USAGE_LINE,	"Yes..[LEFT]/\"Y\"  No..[RIGHT]/\"N\"",1);
	for(;;){
		int trig;
		if( kbhit() ){
			int ch=getch();

			clear_message_line();
			clear_usage_line();
			if( ch=='Y'  ||  ch=='y' ||  ch=='\r'){
				keybuf_clear();
				return CONTINUE;
			}else{
				keybuf_clear();
				return ABORT;
			}
		}
		trig=mouse_alt(NULL,NULL);
		if( trig==1 ){
			clear_message_line();
			clear_usage_line();
			keybuf_clear();
			return CONTINUE;
		}
		if( trig==2 ){
			clear_message_line();
			clear_usage_line();
			keybuf_clear();
			return ABORT;
		}
	}
}

CmdStatus Repaint( void )
{
	real_repaint();
	edit_screen_flush();
	return CONTINUE;
}

CmdStatus Clear( void )
{
	if( AreYouSure("Are You Sure ?")==CONTINUE )
		edit_screen_init();

	return CONTINUE;
}

static void modify_position()
{
	EditSize= EDIT_SCREEN_SIZE / Scale;
	
	CurrentPattern.x = CurrentPattern.x / EditSize * EditSize;
	CurrentPattern.y = CurrentPattern.y / EditSize * EditSize;
	
	edit_screen_flush();
	disp_current_pattern_position();
}

CmdStatus ChSize64( void )
{ Scale =  2 ; ScaleBit = 1 ; modify_position() ; return CONTINUE ; }

CmdStatus ChSize32( void )
{ Scale =  4 ; ScaleBit = 2 ; modify_position() ; return CONTINUE ; }

CmdStatus ChSize16( void )
{ Scale =  8 ; ScaleBit = 3 ; modify_position() ; return CONTINUE ; }

CmdStatus ChSize8 ( void )
{ Scale = 16 ; ScaleBit = 4 ; modify_position() ; return CONTINUE ; }
