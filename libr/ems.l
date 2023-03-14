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


;;;; �d�l�r���݃`�F�b�N���y�[�W�t���[���A�h���X�擾
;;;; short  ems_init( void );
;;;;	�A��l : EMS�y�[�W�t���C���A�h���X ( == -1  : EMS�͑��݂��Ȃ� / �G���[ )

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

;; EMS�y�[�W�t���C���擾

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

;;;;; �y�[�W�̃A���P�[�g
;;;;; short ems_alloc( unsigned page )
;;;;;	�A��l : �n���h���ԍ��@( == -1 : �G���[ )


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
;;;;; �n���h�����̃Z�b�g
;;;;; short ems_set_name( short handle , const char name[8] )
;;;;;	�A��l : EMM�X�e�C�^�X ( != 0 : �G���[ )
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

;;;; �n���h��������n���h���𓾂�B
;;;; short ems_find_handle(const char name[8]);
;;;;   �A��l : !=(-1) �n���h��
;;;;            ==(-1) ������Ȃ������B

ems_find_handle_::
	push	si
	push	dx

	mov	ax,5401h
	mov	si,bx
	int	67h
	and	ah,ah
	je	find_handle
;������Ȃ������B
	mov	ax,-1
	pop	dx
	pop	si
	ret
;���������B
find_handle:
	mov	ax,dx
	pop	dx
	pop	si
	ret
