--
CGROUP	GROUP	TEXT
TEXT	CSEG

- crtc_reg.a86
; void set_crtc_register(unsigned short address,unsigned short data)
;	address ... AX	data ... BX

CRTC_AD	equ	03d4h
CRTC_DT	equ	03d5h

set_crtc_register_::
	push	dx
	mov	dx,CRTC_AD
	out	dx,al
	
	mov	dx,CRTC_DT
	mov	al,bl
	out	dx,al
	pop	dx
	ret

- horiplan.a86

; void set_horizontal_planning( unsigned short npixel )
;	npixel ... AX

STATUS_REG	equ	03DAh
ATTR_REG	equ	03C0h

set_horizontal_planning_::
	push	dx
	mov	bx,ax
; set address-mode
	mov	dx,STATUS_REG
	in	al,dx
; set address
	mov	dx,ATTR_REG
	in	al,dx
	and	al,0E0h
	or	al,013h
	out	dx,al
; set data
	in	al,dx	; at dx==ATTR_REG
	and	al,0F0h
	or	al,bl
	out	dx,al
	
	pop	dx
	ret

--
/* CŒ¾Œêƒ‰ƒCƒuƒ‰ƒŠ */
#include "sprite.h"

- vramoffs.c

unsigned short vram_offset=0;

- startadd.c

void set_start_address(unsigned short address)
{
	set_crtc_register( 0xC , address >> 8  );
	set_crtc_register( 0xD , address & 0xFF);
}

- addoff13.c

void add_vram_offset_13h( signed int delta )
{
	vram_offset += delta;
	
	set_start_address( vram_offset >> 2 );
	set_horizontal_planning( (vram_offset & 3) << 1 );
}

--
- dspx.c
#include "video.h"
#include "console.h"

int get_dspx(void)
{
	int mode=get_video_mode();
	if( (mode==3 || mode==73)  &&  isdosv()  &&  get_text_height() > 25 ){
		return -mode;
	}else{
		return +mode;
	}
}

void set_dspx(int mode)
{
	if( mode == get_dspx() )
		return;
	
	if( mode < 0 ){
		set_video_mode( -mode );
		set_longtext_mode();
	}else{
		set_video_mode( mode );
	}
}
