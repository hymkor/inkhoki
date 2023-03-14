--
EMSINT		equ	67h
EMSINToff	equ	4*EMSINT
EMSINTseg	equ	EMSINToff+2
- ems.a86
DGROUP		GROUP	DATA
DATA		DSEG

ems_errno_::	db	0
EMM_NAM:	db	"EMMXXXX0",0

CGROUP		GROUP	TEXT
TEXT		CSEG


;;;; ＥＭＳ存在チェック＆ページフレームアドレス取得
;;;; short  ems_init( void );
;;;;	帰り値 : EMSページフレイムアドレス ( == -1  : EMSは存在しない / エラー )

;; EMS-Check

ems_init_::
	push	di
	push	si
	push	cx

	xor	ax,ax
	mov	es,ax
	mov	ax,es:[EMSINTseg]
	mov	es,ax
	mov	di,0Ah

	mov	si,EMM_NAM
	mov	cx,8
	cld
repe	cmpsb
	jne	ems_ng2

;; EMSページフレイム取得

	mov	ah,41h
	int	EMSINT

	and	ah,ah
	jne	ems_ng1

	mov	ax,bx
	pop	cx
	pop	si
	pop	di
	ret

ems_ng1:mov	[ems_errno_],ah
ems_ng2:pop	cx
	pop	si
	pop	di
	mov	ax,-1
	ret

- ems1allc.a86
CGROUP		GROUP	TEXT
TEXT		CSEG

;;;;; ページのアロケート
;;;;; short ems_alloc( unsigned page )
;;;;;	帰り値 : ハンドル番号　( == -1 : エラー )


ems_alloc_::
	push	dx
	
	mov	bx,ax
	mov	ah,43h
	int	EMSINT
	
	and	ah,ah
	je	ea_ok
	 mov	dx,-1
ea_ok:	mov	ax,dx
	pop	dx
	ret

- emsstnam.a86
;;;;; ハンドル名のセット
;;;;; short ems_set_name( short handle , const char name[8] )
;;;;;	帰り値 : EMMステイタス ( != 0 : エラー )
CGROUP		GROUP	TEXT
TEXT		CSEG

ems_set_name_::
	push	si
	push	dx
	
	mov	si,bx
	mov	dx,ax
	mov	ax,5301h
	int	EMSINT
	
	mov	al,ah
	mov	ah,0
	
	pop	dx
	pop	si
	ret

- emsfdnam.a86
CGROUP		GROUP	TEXT
TEXT		CSEG

;;;; ハンドル名からハンドルを得る。
;;;; short ems_find_handle(const char name[8]);
;;;;   帰り値 : !=(-1) ハンドル
;;;;            ==(-1) 見つからなかった。

ems_find_handle_::
	push	si
	push	dx

	mov	ax,5401h
	mov	si,bx
	int	67h
	and	ah,ah
	je	find_handle
;見つからなかった。
	mov	ax,-1
	pop	dx
	pop	si
	ret
;見つかった。
find_handle:
	mov	ax,dx
	pop	dx
	pop	si
	ret
