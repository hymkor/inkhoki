--
#include <stdio.h>
#include <dos.h>	/* for farmalloc */
#include "video.h"
#include "sprite.h"

- betaopen.c
#include <farstr.h>
#include <io.h>

#define FpOff(x)	((unsigned short *)&x)[0]
#define FpSeg(x)	((unsigned short *)&x)[1]

beta_t beta_open( const char *fname )
{
	beta_t beta;
	FILE *fp;
	int i;
	
	if( (fp=fopen(fname,"rb")) == NULL ){
		return NULL;
	}
	if( (beta=farmalloc(sizeof(struct beta))) == NULL ){
		fclose(fp);
		return NULL;
	}
	
	if( filelength(fileno(fp)) == sizeof(struct beta) ){
		/* ファイルの長さが 128dotBETA形式のサイズならば、そうみなす */
		
		unsigned char far *ptr;
		
		ptr = &beta->color[0][0];
		
		/* farポインタの正規化 */
		FpSeg(ptr) = FpSeg(ptr) + (FpOff(ptr)>>4);
		FpOff(ptr) = (FpOff(ptr) & 0xF);
		
		for( i=0 ; i<sizeof(*beta) ; i++ ){
			*ptr++ = getc(fp);
		}
		
	}else{
		/* さもなければ、SKIPDOT形式と認識する。*/
		
		unsigned pointor=0;
		unsigned skip;
		unsigned delta;
		
		far_memset( beta->color , 0u , sizeof(beta->color) );
		far_memset( beta->mask  ,~0u , sizeof(beta->mask)  );
		
		while( (skip=getw(fp))!=0  &&  (delta=getw(fp))!=0  &&  !feof(fp) ){
			int x,y;
			
			pointor += skip;
			x = pointor % 320;
			y = pointor / 320;
			
			while( delta-- ){
				const static unsigned char bit[]
					={ ~128u,~64u,~32u,~16u , ~8u,~4u,~2u,~1u };
				
				if( x < 128 ){
					beta->color[ y ][ x ]    = getc(fp);
					beta->mask [ y ][ x/8 ] &= bit[x%8];
					if( feof(fp) )
						goto quit;
				}
				if( ++x >= 320 ){
					y++; x=0;
				}
			}
		}
	}
quit:
	fclose(fp);
	return beta;
}

void beta_close( beta_t beta )
{
	farfree( beta );
}

- beta2spr.c

const static unsigned char bit_pattern[]={128,64,32,16,8,4,2,1};

#define real_color(x,y) beta->color[y][x]
#define is_bg(x,y)		(beta->mask[y][(x)>>3] & bit_pattern[(x)&7])

pattern_t beta_to_sprite(beta_t beta, int x, int y, int width, int height)
{
	unsigned countor=0;	/* DRAW-MODEの時、描画するドット数のカウント用 */
	
	enum{ SKIP , DRAW } mode = SKIP;
	unsigned char far *top = farmalloc( 5000 );
	union {
		unsigned char  far *byte;
		unsigned short far *word;
	} ptr;
	
	int x1=x,y1=y , x2=x+width,y2=y+height;
	const int linesize=320;
	int lastx=x1,lasty=y1;
	
	if( top == NULL )
		return NULL;
	
	ptr.byte = top;
	
	for( y=y1; y<y2; y++){
		for( x=x1; x<x2; x++){
			switch( mode ){
			case SKIP:
				/* 非透明色が現れたら、DRAW-MODEに変更するだけ */
				if( ! is_bg( x , y )  ){
					/* スキップするバイト数　*/
					
					*ptr.word++ = (x-lastx)+linesize*(y-lasty) ;
					
					/* draw-bytes alloc */
					 ptr.word++;
					
					/* 1ドット目を書き込む */
					*ptr.byte++ = real_color(x,y);
					countor=1;
					mode=DRAW;
				}
				break;
			
			case DRAW:
				if( is_bg( x , y )  ){
					/* 先に確保しておいた場所に、描画バイト数を書き込む */
					ptr.byte -= countor+2 ;
					*ptr.word++ = countor ;
					ptr.byte += countor ;
					
					lastx=x; lasty=y;
					mode=SKIP;
				}else{
					*ptr.byte++ = real_color( x , y );
					countor++;
				}
				break;
			}
		}/* end of X loop */
		
		if( mode == DRAW ){
			ptr.byte -= countor+2 ;
			*ptr.word++ = countor ;
			ptr.byte += countor ;
			
			countor = 0;
			lastx=x; lasty=y;
			mode=SKIP;
		}
	}/* end of Y loop */
	
	if( mode== DRAW ){
		ptr.byte -= countor+2 ;
		*ptr.word++ = countor ;
		ptr.byte += countor ;
	}
	
	*ptr.word++ = 0;	/* Skip Byte なし */
	*ptr.word++ = 0;	/* Draw Byte なし => 描画終了マーク */

	return farrealloc( top , ptr.byte - top );
}

- betaputa.c

void beta_putat( beta_t beta , int x , int y , int width , int height , int X , int Y )
{
	int i,j;
	
	for( i=0 ; i<height ; i++ ){
		int _x=x , _X=X ;
		for( j=0 ; j<width ; j++ ){
			VRAM[Y][_X++] = beta->color[y][_x++];
		}
		y++;
		Y++;
	}
}

- betabgpu.c

void beta_bgputat( beta_t beta , int x , int y , int width , int height , int X , int Y )
{
	int i,j;
	
	for( i=0 ; i<height ; i++ ){
		int _x=x , _X=X ;
		for( j=0 ; j<width ; j++ ){
			BGRAM[Y][_X++] = GRAM[Y][_X] = beta->color[y][_x++];
		}
		y++;
		Y++;
	}
}
