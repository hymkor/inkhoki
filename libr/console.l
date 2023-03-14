-- アセンブリ言語データセグメント
DGROUP	GROUP	BSS
BSS	DSEG

- ctrlchar.a86
	even
ctrl_char_table_::
	rs	40h	; == 20h * 2

- dbcstabl.a86
	even
	rs	2	; dbcs_tableのEOF(-1)のための領域
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


-- アセンブリ言語コードセグメント
CGROUP	GROUP	TEXT
TEXT	CSEG
- dbcsinit.a86

;;;;; DBCSテーブル初期化ルーチン
;;;;; 	void  dbcs_table_init( void );

dbcs_table_init_::
	push	ds
	push	di
	push	si
	push	cx

; DS:SIにDBCSﾃｰﾌﾞﾙ先頭ｱﾄﾞﾚｽ を読み込む
	mov	ax,6300h
	int	21h
	jc	iniend

; ES を ﾃｰﾌﾞﾙ の ｾｸﾞﾒﾝﾄ(普段のDS値) にする。
	mov	ax,ss	; es <= ss(本当はdsにしたかった)
	mov	es,ax

	mov	ax,0	; ah は 0 で固定

; とりあえず、全ての ｷｬﾗｸﾀｰ の ﾌﾗｸﾞ を Reset する
	mov	di,dbcs_table_##-1
	mov	cx,257
rep	stosb

; DBCSテーブルに従って、ﾀﾞﾌﾞﾙﾊﾞｲﾄ ｷｬﾗｸﾀ の ﾌﾗｸﾞ を Set する
	mov	ch,0
dbcslop:lodsw
	and	ax,ax
	jz	iniend
	
	mov	cl,ah	; cx .. <終了ｺｰﾄﾞ> - <開始ｺｰﾄﾞ> + 1
	sub	cl,al
	inc	cl
	
	mov	di,dbcs_table_##
	mov	ah,ch	; ah <- 0
	add	di,ax	; di .. <ﾃｰﾌﾞﾙ先頭ｱﾄﾞﾚｽ> + <開始ｺｰﾄﾞ>
	
	mov	al,1
rep	stosb
	jmp	dbcslop

iniend:	pop	cx
	pop	si
	pop	di
	pop	ds
	ret


;;;;; cputatの色付き文字を定義する
;;;;;
;;;;; void set_cputat_ctrl_char( int code , int ascii , int color );
;;;;;	code (in AX)... セットするコード( 00h...1Fh )
;;;;;	ascii(in BX)... アスキーコード
;;;;;	color(in CX)... カラーコード

set_cputat_ctrl_char_::
	cmp	ax,20h
	jae	no_set		; コントロールコードでなければ却下
	 shl	ax,1
	 xchg	ax,bx
	 add	bx,ctrl_char_table_##
	 mov	0[bx],al	; アスキーコードをセット
	 mov	1[bx],cl	; カラーコードをセット
no_set:	ret


- calcvpos.a86

;;; TEXT-VRAMのアドレスを計算する ;;;;;
;;;	[in]	ax...X  bx...Y
;;;	[out]	es:di...アドレス
;;;		ax,bx...破壊

calc_vpos::
	push	ax		; 先頭アドレスを求める

	 mov	ax,0B800h
	 mov	es,ax
	 xor	di,di

	 mov	ah,0FEh
	 int	10h
	
	pop	ax

	shl	ax,1		; 書き込むアドレスを求める
	add	di,ax

	push	es
	 mov	ax,040h		; ax <-- 画面の桁数
	 mov	es,ax
	 mov	ax,es:[04Ah]	; [40:4A]に画面の桁数が入っている
	
	 push	dx
	  mul	bx		; [dx:ax] <- bx(y座標) * ax(画面の桁数)
	  shl	ax,1
	  add	di,ax
	 pop	dx
	pop	es
	ret

- cputat.a86

;;; int cputat( int x , int y , const char *string , int attr )
;;;	x...AX  y...BX  string...CX  attr...DX



;;;; ----- VRAMに書き込む(メインループ) -----
;;;;	al : 現在の文字 , cx : 文字数 , dl : 標準色 
;;;;	ds:si : 文字列アドレス , es:di : VRAMアドレス
;;;;	bx : テーブル参照用(汎用)
;;;; < 定数 >
;;;;	ah : al -> ax の無符号拡張のために常に 0 とする

cputat_entry:
	call	calc_vpos##

	push	di
	mov	si,cx
	xor	cx,cx

	mov	ah,0

cpylop:	lodsb
	and	al,al	;;; al == 0 : 文字列の終結
	je	break

	cmp	al,' '	;;; al <= ' ' : ｵﾘｼﾞﾅﾙｺﾝﾄﾛｰﾙｷｬﾗｸﾀｰ
	jb	ctrlchar

	stosb		; アスキーコード

; if 倍角文字 (倍角文字の2ﾊﾞｲﾄ目をｺﾝﾄﾛｰﾙｷｬﾗｸﾀとみなさないための処置)
	; ↓xlatと同じ処理(ｾｸﾞﾒﾝﾄが異なる場合があるので使えない)
	mov	bx,dbcs_table_## 
	add	bx,ax
	mov	al,ss:[bx]
	; ↑DSは変更されている可能性があるので SS を使う。

	and	al,al
	jz	single
; then 次のバイトをノーチェックで表示する。
	mov	al,dl	; 1byte目のカラーコード
	stosb

	movsb		; 2byte目のアスキーコード
	jz	break;
	mov	al,dl	; 2byte目のカラーコード
	stosb
	add	cx,2
	jmp	cpylop

; else つまり、表示可能半角文字
single:	mov	al,dl	; カラーコード
	stosb

	inc	cx
	jmp	cpylop

; end if & loop メインループ終わり

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
  ; -- 画面更新 --
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

;;;;; 特殊文字(色付き文字)を表示するルーチン ;;;;;

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
; 英語モード
	db	0C9h,0CDh,0BBh
	db	0BAh,020h,0BAh
	db	0C8h,0CDh,0BCh
jp_frame_::
; 日本語モード
	db	01h,06h,02h
	db	05h,20h,05h
	db	03h,06h,04h

--
CGROUP	GROUP	TEXT
TEXT	CSEG
- txtbox.a86
;
; void text_hline(int x,int y,int w,int leftcode,int midcode,int rightcode);
; void text_hline_unflush(..同上..);
;	code = 下位8bit : ｱｽｷｰｺｰﾄﾞ 
;	       上位8bit : 属性コード
;	x...AX   y...BX   w...CX   leftcode...DX
;	midcode...[BP+4]   rightcode...[bp+6]
;
	even
text_hline_entry:
	call	calc_vpos##
	
	push	di
	mov	bx,cx ; save cx into cx

;; 左端のキャラクター
	mov	ax,dx
	stosw

;; 中央のキャラクター列
	mov	ax,[bp+4]
xlop:	stosw
	loop	xlop

;; 右端のキャラクター
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

  /* 表示更新部 */
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