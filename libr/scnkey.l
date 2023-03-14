-- �A�Z���u������Z�N�V����
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
	
; lastkey �� �Ō�ɉ����ꂽ�R�[�h�����ׂ�

	mov	bx,DGROUP	; get segment
	mov	es,bx
	mov	es:[lastkey_] , al

; scantable[] �ōŌ�ɉ����ꂽ�R�[�h�̃t���O�𗧂�����

	mov	bx,ax		; get offset
	and	bx,07Fh
	add	bx,scantable_
	
	not	al
	rol	al,1		; get data
	and	al,1
	mov	es:[bx],al

; �����Ȃ���΁A���ʂɏI���B

	pop	bx
	pop	es
	pop	ax
exit15h:popf
	jmpf	cs:[old15h].D


;--------- ���肱�݃x�N�^���������郋�[�`�� -----------

	even
swap_int15h_::
	xor	ax,ax
	mov	es,ax

; 15h�x�N�^��ۑ�
;   offset
	mov	ax , es:[vect15h+0].W
	mov	cs:[old15h+0].W , ax
;   segment
	mov	ax , es:[vect15h+2].W
	mov	cs:[old15h+2].W , ax

; 15h�x�N�^��ύX
	cli	; ���荞�݂̋֎~
;   offset
	mov	ax , new15h
	mov	es:[vect15h+0].W , ax
;   segment
	mov	ax , cs
	mov	es:[vect15h+2].W , ax
	
	sti	; ���荞�݂̋���
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

KEYCMD	equ	060h	; �L�[�{�[�h�R���g���[���̓��o�̓o�b�t�@�|�[�g
KEYSTT	equ	064h	; �L�[�{�[�h�R���g���[���̃X�e�[�^�X�|�[�g
ACK	equ	0FAh	; �L�[�{�[�h�̉����R�[�h
LEDSET	equ	0EDh	; �L�[�{�[�h��LED�Z�b�g���Z�b�g�R�}���h

TEXT	CSEG

; void set_led( unsigned int flag );
;
;  flag:bit-0 ... ScrollLock
;	bit-1 ... NumLock
;	bit-2 ... CapsLock

	even
set_led_::
	mov	bx,ax

; LED�R�}���h�𑗂�
	mov	al,LEDSET
	out	KEYCMD,al

led1:	in	al,KEYCMD
	cmp	al,ACK
	jne	led1

; LED�f�[�^�𑗂�
	mov	al,bl
	and	al,7
	out	KEYCMD,al
	
led2:	in	al,KEYCMD
	CMP	al,ACK
	jne	led2
	
; �������Ƃ͎v�����A�o�b�t�@���t���Ȃ�҂�
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