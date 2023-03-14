--
#include <conio.h>
#include <dos.h>
#include "keyb.h"
#include "beep.h"


- beepon.c

void beep_on(unsigned dncount)
{
	outp( 0x43 , 0xb6 );
	
	outp( 0x42 , dncount & 0xff );
	outp( 0x42 , dncount >> 8 );
	
	outp( 0x61 , inp(0x61) | 0x03 );
}

- beepoff.c

void beep_off( void )
{
	unsigned char r=inp(0x61) & 0xfc;
	outp( 0x61 , r );
	outp( 0x61 , r | 2 );
}
--
- beeptbl.c

const unsigned short  beep_table[]={
	36488,34440,32507,30682,28960,27334,25801,24353,22986,21696,20478,19561
};
--
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "beep.h"

- music.c
char	_asm_c(const char *);

#define get_interval_timer()	_asm_c(\
	"\n\tmov\tax,040h"	\
	"\n\tmov\tes,ax"	\
	"\n\tmov\tax,es:[6Ch].W"	\
	"\n\tmov\tbx,es:2[6Ch].W"	)


static void irq_wait( unsigned long dncount )
{
	unsigned long timer = get_interval_timer() + dncount ;
	
	while( get_interval_timer() < timer )
		;
}

int play( const char *MMLSTR )
{
	const static onpu_t  onpu_table[3][ 'G'-'A'+1 ]={
		{ A , B    ,  C   , D ,  E   ,  F   , G },
		{ As,NoOnpu,  Cs  , Ds,NoOnpu,  Fs  , Gs},	/* # :シャープ */
		{ Af, Bf   ,NoOnpu, Df,  Ef  ,NoOnpu, Gf},	/* b :フラット */
	};
	unsigned	O=4 ,	/* オクターブ */
				L=4 ,	/* 音符 */
				T=100 ;	/* テンポ(一分間に四分音符をいくつならすか) */
	const char *MML = MMLSTR;
	
	for(;;){
		/* wait量 = 1093    ÷   <テンポ>     ÷ <音符番号> × 4 
		 *			[回/分]   [四分音符数/分] [全音符の長さ/四分音符の長さ] 
		 *        = 4*1093 / Tf
		 */
		
		const unsigned  long numer = 4*1093;
		
		switch( toupper(*MML) ){
		case '\0':
			return 0;
		
		case 'O':
			O=(unsigned) strtoul( MML+1 , (char**)&MML , 10 );
			break;
	
		case 'L':
			L=(unsigned) strtoul( MML+1 , (char**)&MML , 10 );
			break;
		
		case 'R':{
			int length;
			if( isdigit(MML[1]) ){
				length = strtoul( MML+1 , (char**)&MML , 10 );
			}else{
				length = L;
			}
			irq_wait( numer / (T*length) );
			break;
		}
		default:
			if( 'A' <= *MML  &&  *MML <= 'G' ){
				int n;
				onpu_t   onpu;
				unsigned length;
				
				n = *MML++ -'A';
				if( *MML=='+'  ||  *MML=='#' ){
					onpu = onpu_table[1][n];
					MML++;
				}else if( *MML=='-' ){
					onpu = onpu_table[2][n];
					MML++;
				}else{
					onpu = onpu_table[0][n];
				}
				
				if( onpu == NoOnpu )
					return MMLSTR - MML;
				
				if( isdigit(*MML) ){
					length = (unsigned) strtoul( MML , (char**)&MML , 10 );
				}else{
					length = L;
				}
				
				piano( O , onpu );
				/*  printf("T=%d L=%d wait=%ld " , T , length , numer / (T*length) );  */
				irq_wait( numer / (T*length) );
				beep_off();
			}
			break;
		}/* switch */
	}/* for(;;) */
}/* music */
