/*
 *	"video.h"
 *		IBM-PC/AT Video�֌W�ŁA����̉�ʃ��[�h�Ɉˑ����Ȃ����C�u����
 *			3h,70h	---> "console.h"�Q��
 *			13h		---> "sprite.h" �Q��
 */

#ifndef VIDEO_H	
#define VIDEO_H

#ifndef MACROS_H
#	include "macros.h"
#endif

/** Linkless Macros **/


void _asm_NB(SOURCE , BYTE );
BYTE _asm_BW(SOURCE , WORD );
void _asm_NW(SOURCE , WORD );
void _asm_NWB(SOURCE,WORD,BYTE);
WORD _asm_WWW(SOURCE , WORD , WORD );
void _asm_NWNNW( SOURCE , WORD , void (*)() , void (*)() , WORD );
void _asm_NWNW( SOURCE , WORD , void (*)() , WORD );
void _asm_NWWWW(SOURCE , WORD , WORD , WORD , WORD );
void _asm_NWWNW(SOURCE , WORD , WORD , void (*)() , WORD );
WORD _asm_WNNNW(SOURCE , WORD (*)() , WORD (*)() , WORD (*)() , WORD );
/*  N...void or �֐��߲��  ,  B...char  ,  W...short  */

#define set_video_mode(x)	\
	_asm_NW("\n\tINT\t10h\n" ,(x)& 0x00FF)
#define get_video_mode()	\
	_asm_BW("\n\tINT\t10h\n" , 0x0F00)

#define	vsync()		(_asm_WNNNW("\n\tIN\tAL,DX\n" , _asm_WNNNW , _asm_WNNNW , _asm_WNNNW , 0x3DA )&8)
	/* �A�����Ԓ��Ȃ� not 0 �ɂȂ� */
#define vsync_wait()		do{ while( vsync() ); while( !vsync() ); }while(0)
	/* vsync_wait()���s��́A�A�����ԁ@*/

void set_dspx(int mode);
int  get_dspx( void );


#endif /* defined VIDEO_H */
