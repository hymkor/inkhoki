#include "sprite.h"
#include "bitmap.h"

beta_t betabuf;

int option_x		=0,
	option_y		=0,
	option_width	=128,
	option_height	=128,
	option_clear	=0 ;

int main(int argc,char **argv)
{
	struct bmp_head	bmphead;
	struct win_head winhead;
	struct os2_head os2head;
	int bmpwidth,bmpheight,bmpbits;
	FILE *bmpfile,*betafile;
	
	while( --argc > 0 && **++argv=='-' ){
		switch( *(*argv+1) ){
		case 'c':case 'C':
			option_clear = atoi( *argv+2 );
			break;
		case 'x':case 'X':
			option_x	= atoi( *argv+2 );
			break;
		case 'y':case 'Y':
			option_y	= atoi( *argv+2 );
			break;
		case 'w':case 'W':
			option_width= atoi( *argv+2 );
			break;
		case 'h':case 'H':
			option_height= atoi( *argv+2 );
			break;
		}
	}
	if( argc < 2 ){
		fputs(
			"説明 : BMP2BETA <オプション> <BMPファイル名> <BETAファイル名>\n"
			"option\n"
			"  -c??? : 透明色としたい色のカラーコード(default=0:黒)\n"
			"  -x??? : 変換したい範囲の左端の x 座標 (default=0)\n"
			"  -y??? :                 上端の y 座標 (default=8)\n"
			"  -w??? : 変換したい範囲の横幅 (default=128)\n"
			"  -h??? :                 縦幅 (default=128)\n"
		,stdout);
		return 1;
	}
	if( (bmpfile=fopen(argv[0],"rb"))==NULL ){
		perror(argv[0]);
		return 2;
	}
	if( (betafile=fopen(argv[1],"wb"))==NULL ){
		perror(argv[1]);
		return 3;
	}
	if( fread( &bmphead , sizeof(bmp_head) , 1 , bmpfile )==0 ){
		perror(argv[0]);
		return 3;
	}
	if( bmphead.id[0] != 'B'  ||  bmphead.id[1] != 'M' ){
		fputs(argv[0],stdout);
		fputs(" : not bitmap file\n");
		return 4;
	}
	switch( bmphead.head_size ){
	
	case WIN_HEAD_SIZE:
		fread(&os2head,sizeof(winhead),1,bmpfile);
		fseek( bmpfile , bmphead.start , SEEK_SET );
		
		break;
	case OS2_HEAD_SIZE:
		fread(&os2head.sizeof(os2head),1,bmpfile);
		if( xpixels < option_x  ||  ypixels < option_y ){
			printf("%s : Bitmapファイルが小さすぎる(横%ddot 縦%ddot)\n"
				, os2head.xpixels , os2head.ypixels );
			return 5;
		}
		
		break;
	}
	
	
	
	
	
	
	
	
	
	fcloseall();
}


