#ifndef PE_H
#define PE_H

void xor_box(int x,int y,int w,int h,int c);
void box_fill(int x,int y,int w,int h,int c);
void xor_box_fill(int x,int y,int w,int h,int c);

#include <stddef.h>
#include "sprite.h"

typedef struct {
	int x,y;
}Zahyo;

extern Zahyo CurrentPattern;
extern int CurrentColor;
extern int Scale,ScaleBit;
extern int EditSize;

#undef	numof
#define	numof(A)	(sizeof(A)/sizeof((A)[0]))

enum{
/* Y 座標 */
	BOTTOMS		=128,
	PALET_TOP	=152,

	RGB_COLUMN	=3,
	GRAPH_COLUMN=9,
	RGB_LINE	=16,
	TITLE_LINE  =22,
	MESSAGE_LINE=23,
	USAGE_LINE	=24,

/* X 座標 */
	EDIT_SCREEN_SIZE=128,
	EDIT_RIGHT	=128,
	ICON_LEFT	=128,	ICON_COLUMN=16,
	ICON_RIGHT	=192,
	REAL_LEFT	=192,

	CopyFromColor = 14,
	CopyToColor	= 12,
};
typedef enum { CONTINUE , ABORT } CmdStatus ;

#define clear_message_line()	boxfill(0,MESSAGE_LINE<<3,320,8,0)
#define clear_usage_line()		boxfill(0,USAGE_LINE<<3,320,8,0)

void title_draw(int x,int y );

/*** module PE メインモジュール ****/

typedef enum {
	  NOWHERE_REGION , EDIT_REGION , ICON_REGION 
	, REAL_REGION , PALETTE_REGION , COLOR_REGION
} region_t;
region_t which_region(int x,int y,int *X , int *Y );

extern int CurrentColor;
typedef CmdStatus (*cmdid_t)();
extern cmdid_t prevcmd;

/*** module PE0 アイコン関係 ******/
/*** 下請けに PE0A , PE0B が存在 ***/

extern const char std_usage[];
void icon_init( void );
CmdStatus AreYouSure(const char *message); /* "Are You Sure ?" */
CmdStatus click_icon_region(int x,int y,int trig);
CmdStatus short_cut_key(int key);
int getpoint(int *x,int *y);
int cross_cursor(int x,int y);

/*** module PE2 いろいろ ******/

void disp_current_color( void );
void disp_current_pattern_position( void );
CmdStatus click_real_region(int x,int y,int trig);


/*** module PE3 エディット画面関係 ***/

void dotset(int x,int y,int color);
void edit_screen_flush();
void edit_screen_init();

enum{
	X0 = REAL_LEFT , Y0 = 0 ,
	WIDTH = 128 , HEIGHT = 128 ,
	ATTR = 0 , COLOR = 1 ,
};

extern struct realbuffer_t{
	char color,attr;
} RealBuffer[ HEIGHT ][ WIDTH ];

void real_screen_init( void );
int real_pset(int x,int y,int color);
int real_preset(int x,int y);
int real_repaint( void );

#define real_color(x,y)		 RealBuffer[y][x].color
#define real_attr(x,y)		(RealBuffer[y][x].attr ? -1 : RealBuffer[y][x].color)
#define is_bg(x,y)			 RealBuffer[y][x].attr

#define BACKGROUND	(-1)
#define point(x,y)	(RealBuffer[y][x].attr ? BACKGROUND : RealBuffer[y][x].color )
#define dotget(X,Y)	real_attr(CurrentPattern.x+(Y),CurrentPattern.y+(Y))



/**** module PE4 ファイルセレクタ関係 *****/

int input_fname(char *fname);
void save_current_paths( void );

#define	writes gputat
int prompt_input(int x,int y,const char *prompt,char *buffer,int size,int color );
int input(int x,int y,char *buffer,int size,int color );

/**** PE6.C *****/
extern int is_forcus;
extern int forcus_x , forcus_y , forcus_width , forcus_height ;
void forcus_off( void );

CmdStatus click_edit_region(int x,int y,int trig);
#define cmdid_spoit			((cmdid_t)1)
#define cmdid_box_forcus	((cmdid_t)2)

CmdStatus raise_command( int x,int y ,int trig);

/*** PALETTE.C ****/
enum{
	PALET_DOT_SIZE = 5,
	PALET_NUM_LINE =64,
};

extern unsigned char far bglogo[];
void palette_table_init();
CmdStatus click_palette_region(int x,int y,int trig);
void pe_set_palette(int i,int blue,int red,int green);
void pe_get_palette(int i,void *blue,void *red,void *green);
CmdStatus ChPalette( void );

/*** OUTPUT.C ***/

struct icontable {
	const char name[10];
	CmdStatus (*func)();
	int shortcut;
};
int select_2nd_icon( struct icontable *table , int n );

CmdStatus WinSaver( void );
CmdStatus WinLoader( void );
CmdStatus AllSaver( void );
CmdStatus AllLoader( void );
const char *do_option(const char *fname );
extern char defaultfname[];

/*** COLOR.C ***/
CmdStatus click_color_region(int x,int y,int trig);

#endif	/*** PE_H ***/
