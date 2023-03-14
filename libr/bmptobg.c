#include <stdio.h>
#include "bitmap.h"
#include "sprite.h"

static unsigned getdot( FILE *fp , unsigned short bit )
{
	if( bit > 8 ){
		int result=getc(fp);
		fseek(fp, bit/8-1 , SEEK_CUR );
		return result;
	}else if( bit == 8 ){
		return getc(fp);
	}else{
		const static unsigned char mask[]={ 0,1,3,7,15 , 31,63,127,255 };
		static unsigned leftbit=0;
		static unsigned buffer =0;
		
		if( leftbit < bit ){
			buffer = ( (buffer<<8) | getc(fp) );
			leftbit += 8;
		}
		leftbit -= bit;
		
		return (buffer>>leftbit) & mask[bit];
	}
}

int bmp_bgload( const char *fname , int palette_load_flag )
{
	FILE *fp;
	struct bmp_head  bmp_head;
	unsigned short width,height,n_palette;
	unsigned bytes_per_palette , bits_per_pixel ;
	unsigned y;
	
	if( (fp=fopen(fname,"rb"))==NULL )
		return 1;
	
	fread( &bmp_head , sizeof(bmp_head) , 1 , fp );
	
	switch( bmp_head.head_size ){
		case WIN_HEAD_SIZE:{
			struct win_head  win_head;
			fread( &win_head , sizeof(struct win_head) , 1 , fp );
			width  = (unsigned short)win_head.xpixels;
			height = (unsigned short)win_head.ypixels;
			bytes_per_palette = 4;
			bits_per_pixel = win_head.bits_per_pixel;
			n_palette = win_head.npalettes ? win_head.npalettes : (1<<bits_per_pixel) ;
			
			break;
		}
		case OS2_HEAD_SIZE:{
			struct os2_head  os2_head;
			fread( &os2_head , sizeof(struct os2_head) , 1 , fp );
			bytes_per_palette = 3;
			bits_per_pixel = os2_head.bits_per_pixel;
			n_palette = 1 << bits_per_pixel;
			
			break;
		}
	}
	if( palette_load_flag ){
		int i;
		if( n_palette > 256 )
			n_palette = 256;
		
		for( i=0 ; i< n_palette ; i++ ){
			unsigned char rgb[4];
			fread( rgb , bytes_per_palette , 1 , fp );
			set_palette( i , rgb[0]>>2 , rgb[2]>>2 , rgb[1]>>2 );
		}
	}
	fseek( fp , bmp_head.start , SEEK_SET );
	
	if( height > 200 ){
		fseek( fp , (height-200)*width , SEEK_CUR );
	}
	
	for( y=199 ; y >= 0 ; y-- ){
		int x;
		
		for( x=0 ; x<width ; x++ ){
			if( x >= 320 ){
				fseek( fp , width-320 , SEEK_CUR );
				break;
			}
			GRAM[ y ][ x ] = BGRAM[ y ][ x ] = getdot( fp , bits_per_pixel );
			if( feof(fp) )
				goto exit;
		}
	}
exit:
	fclose(fp);
}
