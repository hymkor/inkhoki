-- アセンブリ言語セクション
CGROUP	GROUP	TEXT

- scnkey.a86

DGROUP	GROUP	BSS

vect15h	equ	15h*4

BSS	DSEG

	even
scantable_::
	RS	128
lastkey_::
	RS	2

TEXT	CSEG
	even

old15h:	DW	0,0

	even
new15h:
	pushf
	cmp	ah,04Fh
	jne	exit15h

	push	ax
	push	es
	push	bx
	
; lastkey に 最後に押されたコードをくべる

	mov	bx,DGROUP	; get segment
	mov	es,bx
	mov	es:[lastkey_] , al

; scantable[] で最後に押されたコードのフラグを立たせる

	mov	bx,ax		; get offset
	and	bx,07Fh
	add	bx,scantable_
	
	not	al
	rol	al,1		; get data
	and	al,1
	mov	es:[bx],al

; さもなければ、普通に終了。

	pop	bx
	pop	es
	pop	ax
exit15h:popf
	jmpf	cs:[old15h].D


;--------- 割りこみベクタを書換えるルーチン -----------

	even
swap_int15h_::
	xor	ax,ax
	mov	es,ax

; 15hベクタを保存
;   offset
	mov	ax , es:[vect15h+0].W
	mov	cs:[old15h+0].W , ax
;   segment
	mov	ax , es:[vect15h+2].W
	mov	cs:[old15h+2].W , ax

; 15hベクタを変更
	cli	; 割り込みの禁止
;   offset
	mov	ax , new15h
	mov	es:[vect15h+0].W , ax
;   segment
	mov	ax , cs
	mov	es:[vect15h+2].W , ax
	
	sti	; 割り込みの許可
	ret

	even
restore_int15h_::
	xor	ax,ax
	mov	es,ax
;   offset
	mov	ax , cs:[old15h+0].W
	mov	es:[vect15h+0].W , ax
;   segment
	mov	ax , cs:[old15h+2].W
	mov	es:[vect15h+2].W , ax

	ret

- keyclear.a86
TEXT	CSEG

	even
keybuf_clear_::
	cli
	mov	ax,40h
	mov	es,ax

	mov	ax,es:[ 80h ].W  ; < Keyboard Start >
	mov	es:[ 1Ah ].W,ax  ; < Keyboard Head >
	mov	es:[ 1Ch ].W,ax  ; < Keyboard Tail >

	sti
	ret

- LED.a86

KEYCMD	equ	060h	; キーボードコントローラの入出力バッファポート
KEYSTT	equ	064h	; キーボードコントローラのステータスポート
ACK	equ	0FAh	; キーボードの応答コード
LEDSET	equ	0EDh	; キーボードのLEDセットリセットコマンド

TEXT	CSEG

; void set_led( unsigned int flag );
;
;  flag:bit-0 ... ScrollLock
;	bit-1 ... NumLock
;	bit-2 ... CapsLock

	even
set_led_::
	mov	bx,ax

; LEDコマンドを送る
	mov	al,LEDSET
	out	KEYCMD,al

led1:	in	al,KEYCMD
	cmp	al,ACK
	jne	led1

; LEDデータを送る
	mov	al,bl
	and	al,7
	out	KEYCMD,al
	
led2:	in	al,KEYCMD
	CMP	al,ACK
	jne	led2
	
; 無い事とは思うが、バッファがフルなら待つ
led3:	in	al,KEYSTT
	and	al,2
	jne	led3
	
	ret

--
#include <stdlib.h>
#include "scnkey.h"

- scnkyini.c

int scankey_init(void)
{
	if( atexit(restore_int15h) )
		return 1;
	
	swap_int15h();
	return 0;
}

