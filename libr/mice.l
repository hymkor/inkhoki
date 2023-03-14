--
CGROUP	GROUP	TEXT
TEXT	CSEG
- mice.a86

;;;; int mouse(int *x,int *y)
;;;;	x : ax (==0 : 代入しない)
;;;;	y : bx (==0 : 代入しない)

mouse_::
	push	dx
	push	cx
	push	bx
	push	ax
	
	mov	ax,3
	int	33h

;;; ボタン情報を戻り値とする 
	mov	ax,bx

;;; Ｘ座標を代入
	pop	bx
	and	bx,bx
	jz	next
	shr	cx,1
	mov	[bx],cx

;;; Ｙ座標を代入 
next:	pop	bx
	and	bx,bx
	jz	exit
	mov	[bx],dx

exit:	pop	cx
	pop	dx
	ret

- micealt.a86

mouse_alt_::
	call	mouse_##
	push	ax
	 mov	ah,02h
	 int	16h
	 shr	al,1
	 shr	al,1
	 and	ax,3
	pop	bx
	or	ax,bx
	ret

- stick.a86
;;;; void stick(int *x1,int *y1,int *x2,int *y2)
;;;;	x : ax (==0 : 代入しない)
;;;;	y : bx (==0 : 代入しない)

stick_::
	push	di
	push	dx
	push	cx
	push	bx
	push	ax
	
	mov	ah,84h
	mov	dx,1
	int	15h

; ジョイスティック-A
;   X-座標
	pop	di
	and	di,di
	jz	skip_x1
	mov	[di],ax
skip_x1:
;   Y-座標
	pop	di
	and	di,di
	jz	skip_y1
	mov	[di],bx
skip_y1:

; ジョイスティック-B
;   X-座標
	pop	di
	and	di,di
	jz	skip_x2
	mov	[di],cx
skip_x2:
	mov	cx,di
;   Y-座標
	pop	di
	and	di,di
	jz	skip_y2
	mov	[di],dx
skip_y2:
	mov	dx,di

	pop	di
	ret
