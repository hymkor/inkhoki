#include <stdio.h>
#include "pe.h"

CmdStatus save_dotskip(FILE *fp,int x1,int y1,int width,int height)
{
	const int linesize=320;
	
	int x2=x1+width , y2=y1+height;
	int x,y;
	int lastx=x1,lasty=y1;
	
	char buffer[320];	/* �t�@�C���o�͑O�̕����ʂ�̃o�b�t�@(�ɏ՗̈�) */
	unsigned countor=0;	/* DRAW-MODE�̎��A�`�悷��h�b�g���̃J�E���g�p */
	
	enum{ SKIP , DRAW } mode = SKIP;
	
	for( y=y1; y<y2; y++){
		for( x=x1; x<x2; x++){
			switch( mode ){
			
			case SKIP:
				/* �񓧖��F�����ꂽ��ADRAW-MODE�ɕύX���邾�� */
				if( ! is_bg( x , y )  ){
					/* �X�L�b�v����o�C�g���@*/
					putw( (x-lastx)+linesize*(y-lasty) , fp );
					
					/* 1�h�b�g�ڂ��������� */
					buffer[ 0 ] = real_color(x,y);
					countor=1;
					mode=DRAW;
				}
				break;
			
			case DRAW:
				if( is_bg( x , y )  ){
					putw( countor , fp );
					fwrite( buffer , countor , 1 , fp );
					
					lastx=x; lasty=y;
					mode=SKIP;
				}else{
					buffer[ countor++ ] = real_color( x , y );
				}
				break;
			}
		}/* end of X loop */
		
		if( mode == DRAW ){
			putw( countor , fp );
			fwrite( buffer , countor , 1 , fp );
			
			countor = 0;
			lastx=x; lasty=y;
			mode=SKIP;
		}
	}/* end of Y loop */
	
	if( mode== DRAW ){
		putw( countor , fp );
		fwrite( buffer , countor , 1 , fp );
	}
	
	putw( 0 , fp );		/* Skip Byte �Ȃ� */
	putw( 0 , fp );		/* Draw Byte �Ȃ� => �`��I���}�[�N */
}

CmdStatus load_dotskip(FILE *fp,int x1,int y1,int width,int height)
{
	int x2=x1+width , y2=y1+height ;
	int x=x1,y=y1,n,dx,dy;
	
	dx=getw(fp);
	for(;;){
		n = getw(fp);
		if( feof(fp) || n==0  )
			return CONTINUE;
		
		dy=0;
		while( dx > 160 ){
			dy++;
			dx -= 320;
		}
		x += dx;
		y += dy;
		
		if( y >= y2 )
			return CONTINUE;
		
		while( n-- ){
			int c=getc(fp);
			if( feof(fp) )
				return CONTINUE;
			if( x1 <= x && x < x2 )
				real_pset(x,y,c);
			x++;
		}
		dx=getw(fp);
		if( feof(fp) || dx==0 )
			return CONTINUE;
	}
}
