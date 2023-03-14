--

;★★★★★★★★ スプライトライブラリ ★★★★★★★


CGROUP	GROUP	TEXT
TEXT	CSEG

- sprputat.a86

; short sprite_putat(int x,int y,void far *pattern)
;	x -> ax
;	y -> bx
;	pattern:off -> cx
;	pattern:seg -> dx
;
	even
sprite_putat_::
	push	ds
	push	di
	push	si
	push	dx
	push	cx
	
; ds:si <-- pattern
	mov	si,cx
	mov	ds,dx

; es:di <-- VRAM
	xchg	ax,bx
	mov	dx,320
	mul	dx
	add	ax,bx
	add	ax,ss:[vram_offset_##].W
	push	ax	; 帰り値のためにPUSH
	mov	di,ax
	mov	ax,0A000h
	mov	es,ax

cpylop:	lodsw
	add	di,ax

	lodsw
	mov	cx,ax
	jcxz exit

	rep movsb
	jmp	cpylop

exit:	pop	ax	; 帰り値
	
	pop	cx
	pop	dx
	pop	si
	pop	di
	pop	ds
	ret

- sproff.a86

; void sprite_putoff( short vaddress , void far *pattern )
;	vaddress -> ax
;	pattern:offset  -> cx
;	pattern:segment -> dx
;
	even
sprite_putoff_::
	push	ds
	push	di
	push	si
	push	cx
	
; es:di ... GRAM
	mov	di,ax
	mov	ax,0A000h
	mov	es,ax
; dx:si ... pattern
	mov	si,cx
; bx:di ... Background
	mov	bx,[bgsegment_##].W

;**** メインループ ****

clrlop1:mov	ds,dx	; ds <- pattern
	lodsw
	add	di,ax
		
	lodsw
	mov	cx,ax
	jcxz	clrexit	; ==0 ならば、終了
	
	add	si,ax	; 描画データは参照しないのでスキップ
	
	mov	ds,bx	; ds <- background
clrlop2:mov	al,ds:[di]
	stosb
	loop	clrlop2
	jmp	clrlop1

clrexit:pop	cx
	pop	si
	pop	di
	pop	ds
	ret

- bgcls.a86

; void bgcls(void)

bgcls_::push	di
	push	cx
	
;  segment
	mov	ax,[bgsegment_##].W
	mov	es,ax
;  offset
	xor	di,di
;  counter
	mov	cx,32768
;  fill-value
	xor	ax,ax

	rep stosw

	pop	cx
	pop	di
	ret

- bgsync.a86
; void bgsync(void);

	bgsync_::
	push	ds
	push	di
	push	si
	push	cx
	
	mov	es,[bgsegment_##].W
	xor	di,di
	
	mov	ax,0A000h
	mov	ds,ax
	xor	si,si
	
	mov	cx,32768
	rep	movsw
	
	pop	cx
	pop	si
	pop	di
	pop	ds
	ret

-- Ｃ言語モジュール

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>		/* for atexit */
#include <sys/types.h>	/* for fstat */
#include <sys/stat.h>

#include "sprite.h"

- sprinit.c

unsigned short bgsegment = 0;
void far *BackGroundBuffer;

static void when_exit( void )
{
	farfree( BackGroundBuffer );
}

int sprite_init( void )
{
	if( bgsegment )
		return 0;
	
	BackGroundBuffer = farmalloc( 65536L + 16 );
	
	if( BackGroundBuffer != (void far *)0 ){
		bgsegment = FP_SEG(BackGroundBuffer)  +  ( ( FP_OFF(BackGroundBuffer) + 15 )>>4 );
		bgcls();
		atexit( when_exit );
		return 0;
	}else{
		return 1;
	}
}

- sprload.c

pattern_t sprite_load( const char *fname )
{
	char far *ptr;
	FILE *fp;
	struct stat statbuf;
	pattern_t buffer;
	
	fp=fopen(fname,"rb");
	if( fp==NULL ) return NULL;
	
	fstat( fileno(fp) , &statbuf );
	
	buffer = farmalloc( statbuf.st_size );
	if( buffer == NULL ) return NULL;
	
	ptr = buffer;
	while( statbuf.st_size-- > 0  &&  !feof(fp) ){
		*ptr++ = getc(fp);
	}
	return buffer;
}
