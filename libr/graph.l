--
CGROUP	GROUP	TEXT
TEXT	CSEG

- calc13h.a86
; 座標(AX,BX)をアドレス DI:ESに変換する。
; AX,BXは破壊される

calc13h::
	push	dx
	mov	di,ax
	mov	ax,320
	mul	bx
	add	di,ax
	add	di,[vram_offset_##].W
	
	mov	ax,0A000h
	mov	es,ax
	pop	dx
	ret

- boxfill.a86

; void boxfill(int x,int y,unsigned width,unsighed height,int color);
;		x...AX y...BX width...CX height...DX color...スタック(リターンアドレス直前)

boxfill_::
	jcxz	exit
	and	dx,dx
	jz	exit

	push	bp
	mov	bp,sp
	
	push	di
	push	dx
	
	call	calc13h##
	mov	bx,cx
	mov	ax,[bp+4].W	; color code --> ax

; al...カラーコード    bx...width保存用(const)
; cx...x-loopカウンタ  dx...y-loopカウンタ(destroyed)
	
ylop:	rep	stosb
	
	sub	di,bx
	add	di,320
	mov	cx,bx
	
	dec	dx
	jnz	ylop

	pop	dx
	pop	di

	pop	bp
exit:	ret

- boxfillx.a86

; void boxfillX(int x,int y,unsigned width,unsighed height,int color);
;		x...AX y...BX width...CX height...DX color...スタック(リターンアドレス直前)

boxfillx_::
	jcxz	exit
	and	dx,dx
	jz	exit

	push	bp
	mov	bp,sp
	
	push	di
	push	dx
	call	calc13h##
	mov	bx,cx
	mov	ax,[bp+4].W
	mov	ah,al

; al...カラーコード    bx...width保存用(const)
; cx...x-loopカウンタ  dx...y-loopカウンタ(destroyed)

xylop:	mov	al,ah
	xor	al,es:[di].B
	stosb
	loop	xylop

	sub	di,bx
	add	di,320
	mov	cx,bx
	
	dec	dx
	jne	xylop

	pop	dx
	pop	di

	pop	bp
exit:	ret


--
#include "sprite.h"

- boxdraw.c

void boxdraw(int x1,int y1,unsigned w,unsigned h,int c)
{
	int x2=x1+w-1 , y2=y1+h-1;
	int x,y;
	unsigned char (far *gram)[320] = VRAM;
	
	if( x1 > x2 ){ int tmp=x1; x1=x2; x2=tmp; }
	if( y1 > y2 ){ int tmp=y1; y1=y2; y2=tmp; }
	
	for( x=x1 ; x <=x2 ; x++ )
		gram[ y1 ][ x ] = gram[ y2 ][ x ] = c;
	
	for( y=y1+1 ; y < y2 ; y++ )
		gram[ y ][ x1 ] = gram[ y ][ x2 ] = c;
}

- boxdrawx.c

void boxdrawx(int x1,int y1,unsigned w,unsigned h,int c)
{
	int x2=x1+w-1 , y2=y1+h-1;
	int x,y;
	unsigned char (far *gram)[320] = VRAM;
	
	if( w==0 || h==0 ) return;
	
	for( x=x1 ; x <=x2 ; x++ ){
		gram[ y1 ][ x ] ^= c;
		gram[ y2 ][ x ] ^= c;
	}
	for( y=y1+1 ; y < y2 ; y++ ){
		gram[ y ][ x1 ] ^= c;
		gram[ y ][ x2 ] ^= c;
	}
}

- grayfram.c
void grayframe( int x1 , int y1 , unsigned width , unsigned height )
{
	unsigned char (far *gram)[320] = VRAM;
	int x2=x1+width-1 , y2=y1+height-1 ;
	int x,y;
	
	if( width==0 || height==0 ) return;
	
	for( x=x1 ; x<=x2 ; x++ ){
		gram[ y1 ][ x ] = 15 ;
		gram[ y2 ][ x ] =  8 ;
	}
	for( y=y1+1 ; y<y2 ; y++ ){
		gram[ y ][ x1 ] = 15 ;
		gram[ y ][ x2 ] =  8 ;
	}
	gram[ y1 ][ x2 ] = 7;
	gram[ y2 ][ x1 ] = 7;
}
- graybox.c
void graybox( int x1 , int y1 , unsigned width , unsigned height )
{
	grayframe(x1,y1,width,height);
	boxfill( x1+1 , y1+1 , width-2 , height-2 , 7);
}
