#include <stdio.h>
#include <string.h>
#include "sprite.h"
#include "mice.h"
#include "pe.h"
#include "keyb.h"

/** A **/
CmdStatus Clear( void );
CmdStatus Quit( void );
CmdStatus Repaint( void );
CmdStatus ChSize64( void );
CmdStatus ChSize32( void );
CmdStatus ChSize16( void );
CmdStatus ChSize8 ( void );

/** B **/
CmdStatus Copy( void );

/** "LINEBOX.C" **/
CmdStatus Line( void );
CmdStatus Box( void );
/* CmdStatus BoxCopy( void ); */

/** "CIRCLE.C" **/
CmdStatus Circle(void);

/** "PE6.C" **/
CmdStatus SpaceKey( void );
CmdStatus CursorRight( void );
CmdStatus CursorLeft( void );
CmdStatus CursorUp( void );
CmdStatus CursorDown( void );

#undef	CTRL
#define CTRL(A)	( (A) & 0x1F )

static struct icontable IconTable[]={
	{"l)LINE  "	,Line,		'-'},
	{"b)BOX   "	,Box,		'b'},
	{"r)CIRCLE"	,Circle,	'r'},
	{"4)64x64 "	,ChSize64,	'4'},
	{"3)32x32 "	,ChSize32,	'3'},
	{"2)16x16 "	,ChSize16,	'2'},
	{"1)8x8   "	,ChSize8,	'1'},
	{"c)COPY  "	,Copy,		'c'},
	{"\x1D)CHGPAL",ChPalette,'\t'},
	{"s)SAVE/W"	,WinSaver,	's'},
	{"l)LOAD/W"	,WinLoader,	'l'},
	{"S)SAVE/A"	,AllSaver,	'S'},
	{"L)LOAD/A"	,AllLoader,	'L'},
	{"C)CLS   "	,Clear,		'C'},
	{"q)QUIT  "	,Quit,		'q'},
	{"Q)EXIT  "	,Quit,		'Q'},
};
typedef struct icontable IconTable_t;

static struct ShortCutOnly_tag {
	CmdStatus (*func)();
	int shortcut;
} ShortCutOnlyTable[] = {
	{ SpaceKey		, KEY_SPACE	},
	{ CursorLeft	, KEY_LEFT	},
	{ CursorRight	, KEY_RIGHT },
	{ CursorUp		, KEY_UP	},
	{ CursorDown	, KEY_DOWN  },
};

enum{
	ICON_TABLE_SCREEN_SIZE	=16,
	UP_BUTTON				=0,
	DOWN_BUTTON				=15,
};

static int icon_top = 0 , icon_bottom = ICON_TABLE_SCREEN_SIZE - 2;

void icon_init(void)
{
	IconTable_t *ptr = &IconTable[ icon_top ] ;
	int y;
	
	writes( ICON_COLUMN , 0 , "  <UP>  " , 0x2 );
	boxfillx( ICON_COLUMN<<3 , 0 , 64 , 7 , 0x8 );
	
	for( y=0; y < ICON_TABLE_SCREEN_SIZE - 2 ; y++ ){
		writes( ICON_COLUMN , y+1 , ptr++ ->name , 0x7 );
		boxfillx( ICON_COLUMN<<3 , (y+1)<<3 , 64 , 7 , 0x8 );
	}
	
	writes( ICON_COLUMN , 128/8-1 , " <DOWN> " , 0x2 );
	boxfillx( ICON_COLUMN<<3 , 120 , 64 , 7 , 0x8 );
}

static CmdStatus select_icon(int no)
{
	CmdStatus result = CONTINUE;
	
	if( no < ICON_TABLE_SCREEN_SIZE ){
		if( no == UP_BUTTON ){
			if( icon_top > 0 ){
				--icon_top;
				--icon_bottom;
			}
			prevcmd = (cmdid_t)select_icon;
		}else if( no == DOWN_BUTTON ){
			if( icon_bottom < numof(IconTable) ){
				++icon_top;
				++icon_bottom;
			}
			prevcmd = (cmdid_t)select_icon;
		}else{
			boxfillx( ICON_COLUMN << 3 , no<<3 , 64 , 8 , 8 );
			result = (*IconTable[ icon_top + no - 1 ].func)();
			prevcmd = (cmdid_t)IconTable[ icon_top + no - 1].func;
		}
	}
	icon_init();
	return result;
}

CmdStatus short_cut_key(int key)
{
	int i;
	int scancode = (key>>8);
	int ascicode = (key&0xFF);
	
	for( i=0; i<numof(ShortCutOnlyTable);i++){
		if( ShortCutOnlyTable[i].shortcut == scancode ){
			CmdStatus result = (*ShortCutOnlyTable[i].func)();
			prevcmd = (cmdid_t)ShortCutOnlyTable[i].func;
			icon_init();
			return result;
		}
	}
	
	for( i=0; i<numof(IconTable); i++){
		if( IconTable[i].shortcut == ascicode ){
			CmdStatus result;
			int no=i-icon_top+1;
			
			if( UP_BUTTON < no  &&  no < DOWN_BUTTON ){
				boxfillx( ICON_COLUMN << 3 , no<<3 , 64 , 8 , 8 );
				result = (*IconTable[i].func)();
			}else{
				result = (*IconTable[i].func)();
			}
			icon_init();
			prevcmd = (cmdid_t)IconTable[i].func;
			return result;
		}
	}
	return CONTINUE;
}

const char std_usage[]=
"[LEFT]..PAINT [RIGHT]..SPOIT/SELECT";

CmdStatus click_icon_region(int x,int y,int trig)
{
	/** —¼’[‚Ì•”•ª‚ÍŠÔˆá‚Á‚Ä‰Ÿ‚·‚±‚Æ‚ª‘½‚¢‚Ì‚ÅA–³Ž‹‚·‚é */
	if( EDIT_RIGHT+3 <= x && x < ICON_RIGHT-3 ){
		boxdrawx( REAL_LEFT+CurrentPattern.x, CurrentPattern.y, 
					EditSize, EditSize,15);
		clear_usage_line();
		if( select_icon(y>>3)==ABORT )
			return ABORT;
		writes( 0 , USAGE_LINE , std_usage ,1);
		boxdrawx( REAL_LEFT+CurrentPattern.x, CurrentPattern.y,
					EditSize, EditSize,15);
	}
	return CONTINUE;
}
