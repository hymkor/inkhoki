#include <stdio.h>
#include "pe.h"

CmdStatus save_dotskip(FILE *fp,int x1,int y1,int width,int height)
{
	const int linesize=320;
	
	int x2=x1+width , y2=y1+height;
	int x,y;
	int lastx=x1,lasty=y1;
	
	char buffer[320];	/* ファイル出力前の文字通りのバッファ(緩衝領域) */
	unsigned countor=0;	/* DRAW-MODEの時、描画するドット数のカウント用 */
	
	enum{ SKIP , DRAW } mode = SKIP;
	
	for( y=y1; y<y2; y++){
		for( x=x1; x<x2; x++){
			switch( mode ){
			
			case SKIP:
				/* 非透明色が現れたら、DRAW-MODEに変更するだけ */
				if( ! is_bg( x , y )  ){
					/* スキップするバイト数　*/
					putw( (x-lastx)+linesize*(y-lasty) , fp );
					
					/* 1ドット目を書き込む */
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
	
	putw( 0 , fp );		/* Skip Byte なし */
	putw( 0 , fp );		/* Draw Byte なし => 描画終了マーク */
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