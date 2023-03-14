#ifndef CONSOLE_H
#define	CONSOLE_H

#ifndef	VIDEO_H
#	include "video.h"
#endif

#define	get_text_width()	( *(unsigned short far *)0x0040004AL )
#define	get_text_height()	( *(unsigned char  far *)0x00400084L + 1)

long _asm_LWW( SOURCE , WORD , WORD );
#define isdosv() (		\
	_asm_LWW(			\
		"\n\tMOV\tES,BX"\
		"\n\tMOV\tBX,DI"\
		"\n\tXOR\tDI,DI"\
		"\n\tINT\t10h"	\
		"\n\tMOV\tAX,DI"\
		"\n\tMOV\tDI,BX"\
		"\n\tMOV\tBX,ES"\
		"\n"			\
		,0xFE00,0xB800 	\
	) != 0xB8000000L	\
)

void _asm_NWWWW( SOURCE , WORD , WORD , WORD , WORD );
#define	rollup(x1,y1,x2,y2,atr,n)	\
	_asm_NWWWW( "\n\tINT\t10h\n"	\
		,0x0600|(n),(atr)<<8,((y1)<<8)|(x1),((y2)<<8)|(x2) )
#define	rolldown(x1,y1,x2,y2,atr,n)	\
	_asm_NWWWW( "\n\tINT\t10h\n"	\
		,0x0700|(n),(atr)<<8,((y1)<<8)|(x1),((y2)<<8)|(x2) )
#define cls(x1,y1,x2,y2,atr)		\
	_asm_NWWWW( "\n\tINT\t10h\n"	\
		,0x0700    ,(atr)<<8,((y1)<<8)|(x1),((y2)<<8)|(x2) )

#define set_longtext_mode()	\
	_asm_NWB("\n\tINT\t10h\n",0x1118,0)
	/* ↑英語モードで使うとハングするので、注意 */

signed char get_sub_video_mode( int video_mode );
#define set_sub_video_mode(sub_mode)	\
	_asm_NWW("\n\tINT\t10h\n" , 0x1200|((sub_mode)&0xFF) , 0x39)

#define set_video_page(x)	\
	_asm_NW("\n\tINT\t10h\n" , 0x0500|(x) )
#define	get_video_page()	\
	_asm_BW("\n\tINT\t10h\n\tMOV\tAL,BH\n" , 0x0F00 )

#define set_cursor_pos(x,p)	\
	_asm_NWWNW("\n\tINT\t10h\n" , 0x0200 , (p)<<8 , _asm_NWWNW , (x) )
#define get_cursor_pos(p)	\
	_asm_WWW( "\n\tPUSH\tDX\n\tINT\t10h\n\tMOV\tAX,DX\n\tPOP\tDX\n" \
		, 0x0300 , (p)<<8 )

#define locate(x,y)	\
	_asm_NWNNW( "\n\tINT\t10h\n\tMOV\tAH,02h\n\tINT\t10h\n" \
		, 0x0F00, _asm_NWNNW, _asm_NWNNW, (x)|((y)<<8) )

void _asm_set_timer(SOURCE , WORD , volatile char near * , WORD , WORD );
#define set_timer(ms,p)	\
	_asm_set_timer( "\n\tPUSH\tDS\n\tPOP\tES\n\tINT\t15h\n" \
	, 0x8300 , (p) , (ms)>>16 , (ms)& 0xFFFF )
#define reset_timer()	\
	_asm_NB( "\n\tINT\t15h\n" , 0x01 )

#define isansi()	\
	(_asm_BW( "\n\tINT\t2Fh\n" , 0x1A00 )==0xFF)


#define get_cursor_type()	( *(unsigned short far *)0x00000460L )
#define set_cursor_type(x)	_asm_NWNW("\n\tINT\t10h\n",0x0100,_asm_NWNW,(x))
#define cursor_off()		_asm_NWNW("\n\tINT\t10h\n",0x0100,_asm_NWNW,0x2000)

/** "CONSOLE.L" **/

extern unsigned char framechar[2][9] ;
extern unsigned char jp_frame[9] , us_frame[9];
extern unsigned char ctrl_char_table[ 0x20 ][ 2 ];
extern unsigned char isdbcs[ 256 ];
extern unsigned char jp2us[ 32 ];

int cputat          (int x , int y , const char near*string , unsigned attr);
int cputat_f        (int x , int y , const char far *string , unsigned attr);
int cputat_unflush  (int x , int y , const char near*string , unsigned attr);
int cputat_f_unflush(int x , int y , const char far *string , unsigned attr);
int	video_flush     ( int x , int y , int w );
	/* 帰り値は、文字数 */
void text_hline(int x,int y,int w,int lcode,int mcode,int rcode);
void text_hline_unflush(int x,int y,int w,int lcode,int mcode,int rcode);
	/* w は、midcodeの長さ */
void colorbar( int x , int y , int w , int code );
void dbcs_table_init( void );
void set_cputat_ctrl_char( int code , int ascii , int color );
void vramsave86( void far *buffer , unsigned size , unsigned offset );
void vramload86( void far *buffer , unsigned size , unsigned offset );

enum{
/* CHR... 日本語モード(DOS/V)  ASC... 英語モード(PC-DOS) */
	CHR_ENTER_MARK	= 0x1B ,	ASC_ENTER_MARK	= 0x1C ,
	
	/* arrows */
	CHR_DOWN_ARROW	= 0x07 ,	ASC_DOWN_ARROW	= 0x19 ,
	CHR_UP_ARROW	= 0x1C ,	ASC_UP_ARROW	= 0x18 ,
	CHR_LEFT_ARROW	= 0x1F ,	ASC_LEFT_ARROW	= 0x1B ,
	CHR_RIGHT_ARROW	= 0x1E ,	ASC_RIGHT_ARROW	= 0x1A ,
	CHR_UPDOWN_ARROW= 0x12 ,	ASC_UPDOWN_ARROW= 0x12 ,
	
	/* bar frames */
	CHR_NS_FRAME	= 0x05 ,	ASC_NS_FRAME	= 0xBA ,
	CHR_EW_FRAME	= 0x06 ,	ASC_EW_FRAME	= 0x19 ,
	/* L frames */
	CHR_ES_FRAME	= 0x01 ,	ASC_ES_FRAME	= 0xC9 ,
	CHR_SW_FRAME	= 0x02 ,	ASC_SW_FRAME	= 0xBB ,
	CHR_NE_FRAME	= 0x03 ,	ASC_NE_FRAME	= 0xC8 ,
	CHR_NW_FRAME	= 0x04 ,	ASC_NW_FRAME	= 0xBC ,
	/* T frames */
	CHR_NEW_FRAME	= 0x15 ,	ASC_NEW_FRAME	= 0xCA ,
	CHR_EWS_FRAME	= 0x16 ,	ASC_EWS_FRAME	= 0xCB ,
	CHR_NWS_FRAME	= 0x17 ,	ASC_NWS_FRAME	= 0xB9 ,
	CHR_NES_FRAME	= 0x19 ,	ASC_NES_FRAME	= 0xCC ,
	/* cross frame */
	CHR_NEWS_FRAME	= 0x10 ,	ASC_NEWS_FRAME	= 0xCE ,
	
	JP_CODE_PAGE	= 932 ,		US_CODE_PAGE	= 437 ,
	
	/* 以下、互換性の為 */
	jp_down_arrow	= 0x07 ,	us_down_arrow	= 0x19 ,
	jp_up_arrow		= 0x1C ,	us_up_arrow		= 0x18 ,
	jp_left_arrow	= 0x1F ,	us_left_arrow	= 0x1B ,
	jp_right_arrow	= 0x1E ,	us_right_arrow	= 0x1A ,
	jp_enter_mark	= 0x1B ,	us_enter_mark	= 0x1C ,
};

#endif /* CONSOLE_H */
