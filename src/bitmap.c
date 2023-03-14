#include <stdio.h>
#include "video.h"
#include "pe.h"
#include "bitmap.h"

CmdStatus save_windows_bitmap(FILE *fp,int x1,int y1,int width,int height)
{
	int x2=x1+width , y2=y1+height;
	int x,y,i;
	long size;
	
	/**** ファイルヘッダ(14 bytes) ****/

	putc( 'B' , fp );
	putc( 'M' , fp );
	
	size = 54+256*4+(long)width*height;
	putw( (unsigned)size,fp);
	putw( (unsigned)(size>>16) , fp );	/* ファイル全体の大きさ */
	
	putw( 0 , fp );						/* 予約? */
	putw( 0 , fp );						/* 予約? */
	putw( 54+256*4,fp);putw( 0 , fp );	/* ビットマップの始りの位置 */
	
	/**** Windows 3.0形式ビットマップ情報(40 bytes) ****/

	putw( 40 , fp); putw( 0 , fp );	/* Windows' BMP-File */
	putw(width , fp);putw( 0 , fp );
	putw(height, fp);putw( 0 , fp );
	putw( 1 , fp );						/* プレーンの数 */
	putw( 8 , fp );						/* Bits per pixel */
	putw( 0 , fp );putw( 0 , fp );		/* 無圧縮 */
	
	/* 以下、ほとんどの場合、00000000hでよいらしい */
	putw(width*height,fp );putw(0,fp );	/* ビットマップデータの大きさ */
	putw( 0 , fp );putw( 0 , fp );		/* Pixels per x-meter */
	putw( 0 , fp );putw( 0 , fp );		/* Pixels per y-meter */
	putw(256, fp );putw( 0 , fp );		/* n-palette */
	putw(256, fp );putw( 0 , fp );		/* n-main palette */
	
	/**** パレット情報 ****/
	
	for( i=0 ; i<256 ; i++ ){
		int blue,red,green;
		pe_get_palette( i , &blue, &red , &green );
		putc( blue<<2 , fp );
		putc( green<<2, fp );
		putc( red<<2, fp );
		putc( 0 , fp );
	}

	/**** ビットマップデータ ****/
	
	i = width%4 ;
	for( y=y2-1 ; y >= y1 ; y-- ){
		for( x=x1 ; x<x2 ; x++ ){
			putc( real_color(x,y) , fp );
		}
		switch( i ){
			case 3: putc(0,fp);
			case 2: putc(0,fp);
			case 1: putc(0,fp);
		}
	}
	return CONTINUE;
}

static unsigned int buffer=0;
static int left=0;

static void bitflush(void)
{
	buffer=left=0;
}

static int getbit(int nbits,FILE *fp)
{
	
	unsigned int mask=(1<<nbits)-1;
	unsigned int result;
	
	if( nbits==8 )
		return getc(fp);
	
	if( left < nbits ){
		buffer <<= 8;
		buffer |= getc(fp);
		left += 8;
	}
	left -= nbits;
	result = buffer>>left;
	
	if( nbits == 1  && (result & 1) )
		return 15;
	
	return result & mask;
}

CmdStatus load_windows_bitmap(FILE *fp,int x1,int y1,int width,int height)
{
	struct bmp_head  bmphead;
	union{
		struct win_head  win;
		struct os2_head  os2;
	}bmphead2;
	int bmpwidth,bmpheight,bmpbits,bmpplanes,i,npalettes,bytes_per_line;
	
	if( fread(&bmphead,sizeof(bmphead),1,fp) == 0
			 || bmphead.id[0] != 'B' || bmphead.id[1] != 'M' )
	{	return ABORT;	}
	
	switch( bmphead.head_size ){
	case WIN_HEAD_SIZE:
		fread( &bmphead2.win , sizeof(struct win_head) , 1 , fp );
		
		bmpwidth  = (int)bmphead2.win.xpixels;
		bmpheight = (int)bmphead2.win.ypixels;
		bmpbits   = (int)bmphead2.win.bits_per_pixel ;
		bmpplanes = (int)bmphead2.win.nplanes;
		
		if( bmphead2.win.bits_per_pixel > 8 
		 || bmphead2.win.how_to_compress != 0 )
			return ABORT;
		
		if( bmphead2.win.main_palettes == 0 ){
			bmphead2.win.main_palettes = bmphead2.win.npalettes ;
		}
		
		for(i=0;i<bmphead2.win.main_palettes;i++){
			unsigned char rgb[4];
			fread( rgb , sizeof(rgb) , 1 , fp );
			pe_set_palette( i , rgb[0]>>2 , rgb[2]>>2 , rgb[1]>>2 );
		}
		break;
	
	case OS2_HEAD_SIZE:
		fread( &bmphead2.os2 , sizeof(struct os2_head) , 1 , fp );
		
		bmpwidth  = (int)bmphead2.os2.xpixels;
		bmpheight = (int)bmphead2.os2.ypixels;
		bmpbits   = (int)bmphead2.os2.bits_per_pixel ;
		bmpplanes = (int)bmphead2.os2.nplanes;
		
		if( bmphead2.os2.bits_per_pixel > 8 )
			return ABORT;
		
		npalettes = 1<<bmpbits;
		
		for(i=0;i<npalettes;i++){
			unsigned char rgb[3];
			fread( rgb , sizeof(rgb) , 1 , fp );
			pe_set_palette( i , rgb[0]>>2 , rgb[2]>>2 , rgb[1]>>2 );
		}
		break;
	
	default:
		return ABORT;
	}
	bytes_per_line = (bmpwidth*bmpbits+31)/32*4;
	
	fseek( fp , bmphead.start , SEEK_SET );
	if( bmpheight > height ){
		fseek( fp , bytes_per_line*(bmpheight-height) , SEEK_CUR );
	}else if( bmpheight < height ){
		height = bmpheight;
	}
	
	if( bmpwidth < width )
		width = bmpwidth;
	
	while( height-- > 0 ){
		int i;
		for(i=0;i<width;i++){
			int ch=getbit(bmpbits,fp);
			if( ch > 0 ){
				real_pset( x1+i , y1+height , ch );
			}else{
				real_preset( x1+i , y1+height );
			}
		}
		bitflush();
		fseek( fp , bytes_per_line-(width*bmpbits+7)/8 , SEEK_CUR );
	}
	return CONTINUE;
}
