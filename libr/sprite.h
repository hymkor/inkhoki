/*
 *	"sprite.h"
 *		画面モード13h(320×200×256色)をベースとした
 *		スプライト等のライブラリ
 *
 *		必要ヘッダ ... <machine.h> , "video.h"
 */

#ifndef SPRITE_H
#define SPRITE_H

#include <machine.h>
#ifndef VIDEO_H
#	include "video.h"
#endif


/** Linkless **/

#define GRAM	((unsigned char (far *)[320])0xA0000000L)

#define	set_palette(code,blue,red,green)	\
	( outp(0x3c8,(code)),outp(0x3c9,(red)),outp(0x3c9,(green)),outp(0x3c9,(blue)) )
#define get_palette(code,blue,red,green)	\
	( outp(0x3c7,(code)),*(red)=inp(0x3c9),*(green)=inp(0x3c9),*(blue)=inp(0x3c9) )


/** "VRAMOFF.L" **/

extern unsigned short vram_offset;

void set_crtc_register(unsigned short address,unsigned short data);
void set_horizontal_planning( unsigned short npixel );
void set_start_address( unsigned short address);
void add_vram_offset_13h( signed int delta );

#define VRAM ((unsigned char (far *)[320])(0xA0000000L|vram_offset))


/** "SPRITE.L" **/

typedef void far *pattern_t;

int       	sprite_init ( void );
pattern_t	sprite_load( const char *fname );
short		sprite_putat ( int x , int y , pattern_t pattern );
void 		sprite_putoff( short address , pattern_t pattern );
void  		bgcls( void );
void		bgsync( void );

extern unsigned short   bgsegment;

#define BGRAM	((unsigned char (far *)[320])(((unsigned long)bgsegment<<16)|vram_offset))



/** "BETA.L" **/

typedef struct beta{
	unsigned char color[128][128];
	unsigned char mask [128][128/8];
} far *beta_t;

beta_t		beta_open( const char *fname );
void		beta_close( beta_t beta );
pattern_t	beta_to_sprite( beta_t beta , int x , int y , int width , int height );
void		beta_putat( beta_t beta , int x , int y , int width , int height , int X , int Y );
void 		beta_bgputat( beta_t beta , int x , int y , int width , int height , int X , int Y );
/*	(x,y) ... betaバッファ内での座標 	0≦ｘ＜128  0≦ｙ＜128
	(X,Y) ... 画面上の座標				0≦Ｘ＜320  0≦Ｙ＜200
 */


/** "BGBMP.C" **/

int bmp_bgload( const char *fname , int palette_load_flag );


/** "GPUTAT.A86" (AH=08h int 10h) **/

int gputat  ( int x , int y , const char     *string , unsigned attr);
int gputat_f( int x , int y , const char far *string , unsigned attr);


/** "GRAPH.L" **/

void boxdraw  ( int x , int y , unsigned width , unsigned height , int color );
void boxdrawx ( int x , int y , unsigned width , unsigned height , int color );
void boxfill  ( int x , int y , unsigned width , unsigned height , int color );
void boxfillx ( int x , int y , unsigned width , unsigned height , int color );
void grayframe( int x , int y , unsigned width , unsigned height );
void graybox  ( int x , int y , unsigned width , unsigned height );
/*   (x,y)...左上の座標   (width,height)...箱のサイズ    */
/*   draw...枠のみ   fill...中も塗り潰す   x...XOR演算   */


void line13h( int x1,int y1,int x2,int y2,int col);

#endif /* SPRITE_H */
