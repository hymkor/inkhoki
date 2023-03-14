#ifndef	KEYB_H
#define KEYB_H

#ifndef	MACROS_H
#	include "macros.h"
#endif

WORD _asm_WW   ( SOURCE , WORD );
BYTE _asm_BW   ( SOURCE , WORD );
void _asm_NWNNB( SOURCE , WORD , void (*)() , void (*)() , BYTE);
void _asm_NWNNW( SOURCE , WORD , void (*)() , void (*)() , WORD);
void _asm_NWNW ( SOURCE , WORD , void (*)() , WORD );

#define	kbdhit()	\
	(~_asm_WW("\n\tINT\t16h\n\tLAHF\n" , 0x0100 ) & 0x4000 )
#define	getkey()	\
	_asm_WW( "\n\tINT\t16h\n" , 0x0000 )
#define	kbshift()	\
	_asm_BW( "\n\tINT\t16h\n" , 0x0200 )
#define	xkbshift()	\
	_asm_WW( "\n\tINT\t16h\n" , 0x1200 )
#define append_keybuf(x) \
	_asm_NWNW( "\n\tINT\t16h\n",0x0500,_asm_NWNW,(x))
#define get_ctrl_c_check()	\
	_asm_BW("\n\tMOV\tBX,DX\n\tINT\t21h\n\tMOV\tAL,DL\n\tMOV\tDX,BX\n",0x3300)
#define set_ctrl_c_check(x)	\
	_asm_NWNNB( "\n\tINT\t21h\n" , 0x3301 , _asm_NWNNB , _asm_NWNNB , (x) )
#define ctrl_c_check_off()	\
	set_ctrl_c_check(0)
#define ctrl_c_check_on()	\
	set_ctrl_c_check(1)
#define get_ias_mode()	\
	_asm_WW("\n\tMOV\tBX,DX\n\tINT\t16h\n\tMOV\tAX,DX\n\tMOV\tDX,BX\n",0x1301)
#define set_ias_mode(x)	\
	_asm_NWNNW("\n\tMOV\tBX,DX\n\tINT\t16h\n\tMOV\tDX,BX\n" \
		,0x1300,_asm_NWNNW,_asm_NWNNW,(x))

enum{
  /* getkey() */
	KEY_ESCAPE			= 1,
	KEY_SPACE			= 2,
	KEY_UP				= 72,
	KEY_LEFT			= 75,
	KEY_RIGHT			= 77,
	KEY_DOWN			= 80,
	KEY_HENKAN			= 121,
	KEY_MUHENKAN		= 123,
	KEY_PAGEUP			= 73,
	KEY_PAGEDOWN		= 81,
	KEY_HOME			= 71,
	KEY_END				= 79,
	KEY_ALT_F4			= 0x6B,

  /* kbshift() & xkbshift */
	SHIFT_LEFT			= 0x1,
	SHIFT_RIGHT			= 0x2,
	SHIFT_BOTH			= 0x3,
	SHIFT_CTRL			= 0x4,
	SHIFT_ALT			= 0x8,
	SHIFT_CAPSLOCK		= 0x10,
	SHIFT_NUMLOCK		= 0x20,
	SHIFT_INSERTLOCK	= 0x80,

  /* xkbshift() */
	SHIFT_CTRL_LEFT		= 0x100,
	SHIFT_ALT_LEFT		= 0x200,
	SHIFT_CTRL_RIGHT	= 0x400,
	SHIFT_ALT_RIGHT		= 0x800,
	SHIFT_SYSRQ			= 0x8000,
};

/** "SCNKEY.L" **/

extern volatile unsigned char  scantable[128];
extern volatile unsigned char  lastkey;

void swap_int15h( void );
void restore_int15h( void );
void keybuf_clear( void );
int  scankey_init(void);
void set_led( unsigned short led_pattern );

enum{  
	LED_SCROLL_LOCK = 1 ,
	LED_NUM_LOCK	= 2 ,
	LED_CAPS_LOCK	= 4 ,
};


/** "INPUT.C" **/

typedef struct {
	void (*routine)(void *);
	void *argument;
}task_t;

int        input(int x,int y,                   char *buf,int size,int color);
int prompt_input(int x,int y,const char *prompt,char *buf,int size,int color);
task_t set_input_bg_task( task_t new_task );

	/* buffer...ﾃﾞﾌｫﾙﾄの文字列を入れておく
	 * 帰り値 == >=0: 文字数   == (-1): キャンセル
	 */


#endif /* KEYB_H */
