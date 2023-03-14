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
			"���� : BMP2BETA <�I�v�V����> <BMP�t�@�C����> <BETA�t�@�C����>\n"
			"option\n"
			"  -c??? : �����F�Ƃ������F�̃J���[�R�[�h(default=0:��)\n"
			"  -x??? : �ϊ��������͈͂̍��[�� x ���W (default=0)\n"
			"  -y??? :                 ��[�� y ���W (default=8)\n"
			"  -w??? : �ϊ��������͈͂̉��� (default=128)\n"
			"  -h??? :                 �c�� (default=128)\n"
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
			printf("%s : Bitmap�t�@�C��������������(��%ddot �c%ddot)\n"
				, os2head.xpixels , os2head.ypixels );
			return 5;
		}
		
		break;
	}
	
	
	
	
	
	
	
	
	
	fcloseall();
}


