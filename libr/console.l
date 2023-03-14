-- �A�Z���u������f�[�^�Z�O�����g
DGROUP	GROUP	BSS
BSS	DSEG

- ctrlchar.a86
	even
ctrl_char_table_::
	rs	40h	; == 20h * 2

- dbcstabl.a86
	even
	rs	2	; dbcs_table��EOF(-1)�̂��߂̗̈�
isdbcs_::
dbcs_table_::
	rs	256

--
- jp2us.a86
DGROUP	GROUP	DATA
DATA	DSEG

	even
	db	0,-1
jp2us_::
  ; 0?h
	db	000h,0C9h,0BBh,0C8h , 0BCh,0BAh,0CDh,019h
	db	020h,009h,020h,00Bh , 020h,020h,0FEh,00Fh
  ; 1?h
	db	0CEh,011h,017h,013h , 0B2h,0CAh,0C9h,0B9h
	db	020h,0CCh,0B0h,01Ch , 018h,0B3h,01Ah,01Bh


-- �A�Z���u������R�[�h�Z�O�����g
CGROUP	GROUP	TEXT
TEXT	CSEG
- dbcsinit.a86

;;;;; DBCS�e�[�u�����������[�`��
;;;;; 	void  dbcs_table_init( void );

dbcs_table_init_::
	push	ds
	push	di
	push	si
	push	cx

; DS:SI��DBCSð��ِ擪���ڽ ��ǂݍ���
	mov	ax,6300h
	int	21h
	jc	iniend

; ES �� ð��� �� ������(���i��DS�l) �ɂ���B
	mov	ax,ss	; es <= ss(�{����ds�ɂ���������)
	mov	es,ax

	mov	ax,0	; ah �� 0 �ŌŒ�

; �Ƃ肠�����A�S�Ă� ��׸�� �� �׸� �� Reset ����
	mov	di,dbcs_table_##-1
	mov	cx,257
rep	stosb

; DBCS�e�[�u���ɏ]���āA������޲� ��׸� �� �׸� �� Set ����
	mov	ch,0
dbcslop:lodsw
	and	ax,ax
	jz	iniend
	
	mov	cl,ah	; cx .. <�I������> - <�J�n����> + 1
	sub	cl,al
	inc	cl
	
	mov	di,dbcs_table_##
	mov	ah,ch	; ah <- 0
	add	di,ax	; di .. <ð��ِ擪���ڽ> + <�J�n����>
	
	mov	al,1
rep	stosb
	jmp	dbcslop

iniend:	pop	cx
	pop	si
	pop	di
	pop	ds
	ret


;;;;; cputat�̐F�t���������`����
;;;;;
;;;;; void set_cputat_ctrl_char( int code , int ascii , int color );
;;;;;	code (in AX)... �Z�b�g����R�[�h( 00h...1Fh )
;;;;;	ascii(in BX)... �A�X�L�[�R�[�h
;;;;;	color(in CX)... �J���[�R�[�h

set_cputat_ctrl_char_::
	cmp	ax,20h
	jae	no_set		; �R���g���[���R�[�h�łȂ���΋p��
	 shl	ax,1
	 xchg	ax,bx
	 add	bx,ctrl_char_table_##
	 mov	0[bx],al	; �A�X�L�[�R�[�h���Z�b�g
	 mov	1[bx],cl	; �J���[�R�[�h���Z�b�g
no_set:	ret


- calcvpos.a86

;;; TEXT-VRAM�̃A�h���X���v�Z���� ;;;;;
;;;	[in]	ax...X  bx...Y
;;;	[out]	es:di...�A�h���X
;;;		ax,bx...�j��

calc_vpos::
	push	ax		; �擪�A�h���X�����߂�

	 mov	ax,0B800h
	 mov	es,ax
	 xor	di,di

	 mov	ah,0FEh
	 int	10h
	
	pop	ax

	shl	ax,1		; �������ރA�h���X�����߂�
	add	di,ax

	push	es
	 mov	ax,040h		; ax <-- ��ʂ̌���
	 mov	es,ax
	 mov	ax,es:[04Ah]	; [40:4A]�ɉ�ʂ̌����������Ă���
	
	 push	dx
	  mul	bx		; [dx:ax] <- bx(y���W) * ax(��ʂ̌���)
	  shl	ax,1
	  add	di,ax
	 pop	dx
	pop	es
	ret

- cputat.a86

;;; int cputat( int x , int y , const char *string , int attr )
;;;	x...AX  y...BX  string...CX  attr...DX



;;;; ----- VRAM�ɏ�������(���C�����[�v) -----
;;;;	al : ���݂̕��� , cx : ������ , dl : �W���F 
;;;;	ds:si : ������A�h���X , es:di : VRAM�A�h���X
;;;;	bx : �e�[�u���Q�Ɨp(�ėp)
;;;; < �萔 >
;;;;	ah : al -> ax �̖������g���̂��߂ɏ�� 0 �Ƃ���

cputat_entry:
	call	calc_vpos##

	push	di
	mov	si,cx
	xor	cx,cx

	mov	ah,0

cpylop:	lodsb
	and	al,al	;;; al == 0 : ������̏I��
	je	break

	cmp	al,' '	;;; al <= ' ' : �ؼ��ٺ��۰ٷ�׸��
	jb	ctrlchar

	stosb		; �A�X�L�[�R�[�h

; if �{�p���� (�{�p������2�޲Ėڂ���۰ٷ�׸��Ƃ݂Ȃ��Ȃ����߂̏��u)
	; ��xlat�Ɠ�������(�����Ă��قȂ�ꍇ������̂Ŏg���Ȃ�)
	mov	bx,dbcs_table_## 
	add	bx,ax
	mov	al,ss:[bx]
	; ��DS�͕ύX����Ă���\��������̂� SS ���g���B

	and	al,al
	jz	single
; then ���̃o�C�g���m�[�`�F�b�N�ŕ\������B
	mov	al,dl	; 1byte�ڂ̃J���[�R�[�h
	stosb

	movsb		; 2byte�ڂ̃A�X�L�[�R�[�h
	jz	break;
	mov	al,dl	; 2byte�ڂ̃J���[�R�[�h
	stosb
	add	cx,2
	jmp	cpylop

; else �܂�A�\���\���p����
single:	mov	al,dl	; �J���[�R�[�h
	stosb

	inc	cx
	jmp	cpylop

; end if & loop ���C�����[�v�I���

break:	pop	di
	and	cx,cx
	je	exit
	
exit:	mov	ax,cx
	ret


cputat_::
	push	di
	push	si
	push	cx
	  call	cputat_entry
  ; -- ��ʍX�V --
	push	ax
	push	ds
	  mov	ah,0FFh
	  int	10h
	pop	ds
  	pop	ax
  ; --------------
	pop	cx
	pop	si
	pop	di
	ret

cputat_unflush_::
	push	di
	push	si
	push	cx
	  call	cputat_entry
	pop	cx
	pop	si
	pop	di
	ret

;;;;; ���ꕶ��(�F�t������)��\�����郋�[�`�� ;;;;;

ctrlchar:
	mov	bx,ctrl_char_table_##
	shl	al,1
	add	bx,ax

	mov	al,ss:bx[0]
	stosb
	mov	al,ss:bx[1]
	stosb
	inc	cx

	jmp	cpylop

- cputat_f.a86

cputat_f_::
	push	bp
	 mov	bp,sp
	push	ds
	push	dx

	mov	ds,dx
	mov	dx,[bp+4]
	call	cputat_##

	pop	dx
	pop	ds
	pop	bp

	ret

cputat_f_unflush_::
	push	bp
	 mov	bp,sp
	push	ds
	push	dx

	mov	ds,dx
	mov	dx,[bp+4]
	call	cputat_unflush_##

	pop	dx
	pop	ds
	pop	bp
	ret



- vdoflush.a86

;;; void video_flush( int x , int y , int w );

video_flush_::
	push	ds
	push	di
	
	call	calc_vpos##
	mov	ah,0FFh
	int	10h
	
	pop	di
	pop	ds
	ret
--
- textfram.a86

DGROUP	GROUP	DATA
DATA	DSEG

;;; unsigned char framechar[ 1 ][ 9 ];

framechar_::
us_frame_::
; �p�ꃂ�[�h
	db	0C9h,0CDh,0BBh
	db	0BAh,020h,0BAh
	db	0C8h,0CDh,0BCh
jp_frame_::
; ���{�ꃂ�[�h
	db	01h,06h,02h
	db	05h,20h,05h
	db	03h,06h,04h

--
CGROUP	GROUP	TEXT
TEXT	CSEG
- txtbox.a86
;
; void text_hline(int x,int y,int w,int leftcode,int midcode,int rightcode);
; void text_hline_unflush(..����..);
;	code = ����8bit : �������� 
;	       ���8bit : �����R�[�h
;	x...AX   y...BX   w...CX   leftcode...DX
;	midcode...[BP+4]   rightcode...[bp+6]
;
	even
text_hline_entry:
	call	calc_vpos##
	
	push	di
	mov	bx,cx ; save cx into cx

;; ���[�̃L�����N�^�[
	mov	ax,dx
	stosw

;; �����̃L�����N�^�[��
	mov	ax,[bp+4]
xlop:	stosw
	loop	xlop

;; �E�[�̃L�����N�^�[
	mov	ax,[bp+6]
	stosw

	mov	cx,bx
	pop	di

	ret

text_hline_unflush_::
	push	bp
	 mov	bp,sp
	push	di
	  call	text_hline_entry
	pop	di
	pop	bp
	ret

text_hline_::
	push	bp
	 mov	bp,sp
	push	di
	call	text_hline_entry

  /* �\���X�V�� */
	push	cx
	add	cx,2
	push	ds
	  mov	ah,0FFh
	  int	10h
	pop	ds
	pop	cx
  /* ---------- */
	pop	di
	pop	bp
	ret


- colorbar.a86
;;; void colorbar(int x,int y,int w,int code)

	even
colorbar_::
	push	di
	push	cx

	call	calc_vpos##

	push	di
	push	cx

	mov	al,dl
	
xlop:	inc	di
	stosb
	loop	xlop

	pop	cx
	pop	di
	
	push	ds
	 mov	ah,0FFh
	 int	10h
	pop	ds
	
	pop	cx
	pop	di
	ret
