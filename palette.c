#include <stdio.h>
#include "sprite.h"
#include "pe.h"

enum{ DEFAULT_PALETTE , NEW_PALETTE } static palette_mode = NEW_PALETTE;

const static char default_palettes[16][4]={ /* RGBÇÃèáÅAÇ†ÇËÇ·ÅH*/
	{ 0, 0, 0} , { 0, 0,42} , { 0,42, 0} , { 0,42,42},
	{42, 0, 0} , {42, 0,42} , {42,21, 0} , {42,42,42},
	{21,21,21} , {21,21,63} , {21,63,21} , {21,63,63},
	{63,21,21} , {63,21,63} , {63,63,21} , {63,63,63},
};
static char new_palettes[16][4]={
	{ 0, 0, 0} , { 0, 0,42} , { 0,42, 0} , { 0,42,42},
	{42, 0, 0} , {42, 0,42} , {42,21, 0} , {42,42,42},
	{21,21,21} , {21,21,63} , {21,63,21} , {21,63,63},
	{63,21,21} , {63,21,63} , {63,63,21} , {63,63,63},
};

void pe_set_palette(int i,int blue,int red,int green)
{
	if( i < 16 ){
		new_palettes[i][0] = red   & 63;
		new_palettes[i][1] = green & 63;
		new_palettes[i][2] = blue  & 63;
		if( palette_mode == DEFAULT_PALETTE )
			return;
	}
	set_palette(i,blue,red,green);
}

void pe_get_palette(int i,void *blue,void *red,void *green)
{
	typedef unsigned char *ucp;
	if( i < 16  ){
		*(ucp)red  = new_palettes[i][0] & 63;
		*(ucp)green= new_palettes[i][1] & 63;
		*(ucp)blue = new_palettes[i][2] & 63;
	}else{
		get_palette(i,(ucp)blue,(ucp)red,(ucp)green);
	}
}

void goto_default_palette( void )
{
	int i;
	for(i=0;i<numof(default_palettes);i++){
		set_palette( i , default_palettes[i][2] ,
					default_palettes[i][0] , default_palettes[i][1] );
	}
	palette_mode = DEFAULT_PALETTE;
}

void goto_new_palette( void )
{
	int i;
	for(i=0;i<numof(new_palettes);i++){
		set_palette( i , new_palettes[i][2] 
					,new_palettes[i][0] , new_palettes[i][1] );
	}
	palette_mode = NEW_PALETTE;
}

CmdStatus ChPalette( void )
{
	switch( palette_mode ){
		case DEFAULT_PALETTE:	goto_new_palette();		break;
		case NEW_PALETTE:		goto_default_palette();	break;
	}
	return CONTINUE;
}


unsigned char far bglogo[]={
	0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x01, 0x08, 0x00, 
	0x00, 0x0F, 0x0F, 0x00, 0x0B, 0x0B, 0x00, 0x00, 0x38, 0x01, 0x08, 0x00, 0x00, 0x0F, 0x0F, 0x07, 
	0x0B, 0x0B, 0x07, 0x00, 0x38, 0x01, 0x08, 0x00, 0x00, 0x00, 0x07, 0x07, 0x00, 0x07, 0x07, 0x00, 
	0x38, 0x01, 0x08, 0x00, 0x00, 0x0B, 0x0B, 0x00, 0x0B, 0x0B, 0x00, 0x00, 0x38, 0x01, 0x08, 0x00, 
	0x00, 0x0B, 0x0B, 0x07, 0x0B, 0x0B, 0x07, 0x00, 0x38, 0x01, 0x08, 0x00, 0x00, 0x00, 0x07, 0x07, 
	0x00, 0x07, 0x07, 0x00, 0x38, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
};

void palette_table_init()
{
	int i;
	
	for( i=0; i<256; i++){
		box_fill( 			(i%PALET_NUM_LINE)*PALET_DOT_SIZE,
			 	  PALET_TOP+(i/PALET_NUM_LINE)*PALET_DOT_SIZE,
			 	  PALET_DOT_SIZE-1,
			 	  PALET_DOT_SIZE-1,
			 	i );
	}
	sprite_putat( 0 , PALET_TOP + PALET_DOT_SIZE * 4 , bglogo);
}


CmdStatus click_palette_region(int x,int y,int trig)
{
	int c;
	c = x/PALET_DOT_SIZE+((y-PALET_TOP)/PALET_DOT_SIZE)*PALET_NUM_LINE;
	
	if( c >= 256 ){
		if( c < 258  ||  ( 320 <= c && c < 322 ) ){
			c = -1;
		}else{
			return CONTINUE;
		}
	}
	if( prevcmd != cmdid_box_forcus ){
		CurrentColor = c ;
		disp_current_color();
	}else{
		int x,y;
		
		for( y=0 ; y<forcus_height ; y++){
			for( x=0 ; x<forcus_width ; x++){
				dotset( forcus_x+x , forcus_y+y , c );
			}
		}
	}
	prevcmd = (cmdid_t) click_palette_region;
	return CONTINUE;
}

CmdStatus save_palette(FILE *fp,int x1,int y1,int width,int height)
{
	int i;
	for( i=0 ; i<256 ; i++ ){
		int blue,red,green;
		
		pe_get_palette(i,&blue,&red,&green);
		putc( blue , fp );
		putc( red  , fp );
		putc( green, fp );
	}
	return CONTINUE;
}

CmdStatus load_palette(FILE *fp,int x1,int y1,int width,int height)
{
	int i;
	for( i=0 ; i<256 ; i++ ){
		int blue,red,green;
		
		blue = getc( fp );
		red  = getc( fp );
		green= getc( fp );
		
		pe_set_palette(i,blue,red,green);
	}
	goto_new_palette();
	return CONTINUE;
}
