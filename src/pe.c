#include <assert.h>

#include <stdio.h>
#include <string.h>

#include "console.h"
#include "sprite.h"
#include "mice.h"
#include "keyb.h"
#include "pe.h"

#define VER "0.25"

region_t which_region(int x,int y,int *X,int *Y)
{
	int dummy;
	if( X==NULL ) X=&dummy;
	if( Y==NULL ) Y=&dummy;

	if(  y  <  128  ){
		*Y = y;
		if(  x  <  128  ){
			*X = x;
			return EDIT_REGION;
		}else if( x < 192 ){
			*X = x - 128 ;
			return ICON_REGION;
		}else{
			*X = x - 192 ;
			return REAL_REGION;
		}
	}else if( y < 152 ){
		*X = x;
		*Y = y - 128;
		return COLOR_REGION;
	}else{
		*X = x;
		*Y = y - 152;
		return PALETTE_REGION;
	}
}


int main(int argc,char **argv)
{
	int x=0,y=0,trig=0;
	int old_video_mode = get_dspx();
	FILE *cfg;
	int i;
	
	save_current_paths();
	
	set_video_mode(0x13);
	if( mouse_init() ){
		set_dspx(old_video_mode);
		fputs( isdosv()
			? "マウスドライバが常駐されていません。\n"
			: "No mouse driver.\n"
		, stderr );
		return 1;
	}
	
	real_screen_init();
	
	/* "_INK" の評価 */
	if( (cfg=fopen("_ink","rt")) != NULL ){
		char buffer[80];
		while( fgets(buffer,sizeof(buffer),cfg) != NULL ){
			char *token=strtok(buffer," \t\n\r");
			while( token != NULL ){
				const char *errstr=do_option( token );
				if( errstr != NULL ){
					set_dspx(old_video_mode);
					printf("\aIn \"_ink\"\n%s : %s\n",token,errstr);
					return -1;
				}
				token=strtok(NULL," \t\n\r");
			}
		}
		fclose(cfg);
	}
	/* コマンドラインの評価 */
	for( i=1 ; i<argc ; i++ ){
		const char *errstr=do_option( argv[i] );
		if( errstr != NULL ){
			set_dspx(old_video_mode);
			printf("\a%s : %s\n",argv[i],errstr);
			return -1;
		}
	}
	
	icon_init();
	palette_table_init();
	edit_screen_flush();
	
	writes( 11 , TITLE_LINE , "Ver." VER " 1994,95 (C) K.HAYAMA" , 4  );
	writes( 2 , TITLE_LINE , "InkHoki!" , 14 );
	
	boxdrawx(REAL_LEFT+CurrentPattern.x,CurrentPattern.y,EditSize,EditSize,15);
	writes( 0 , USAGE_LINE , std_usage ,1);
	disp_current_color();
	disp_current_pattern_position();
	
	mouse_on();
	
	for(;;){
		int _x,_y,_trig;
		for(;;){
			_x=x; _y=y; _trig=trig;
			trig=mouse_alt(&x,&y);
			
			/***** キーボードによるコマンド起動 *****/
			if( kbdhit() ){
				mouse_off();
					boxdrawx( REAL_LEFT+CurrentPattern.x, CurrentPattern.y, 
								EditSize, EditSize,15);
					clear_usage_line();
					if( prevcmd == cmdid_box_forcus )
						forcus_off();
					if( short_cut_key( getkey() ) == ABORT )
						goto exit;
					writes( 0,USAGE_LINE,"[LEFT]..PSET/SELECT  [RIGHT]..SPOIT",1);
					boxdrawx( REAL_LEFT+CurrentPattern.x, CurrentPattern.y,
							EditSize, EditSize, 15);
				mouse_on();
			}
			
			/***** マウスによるコマンド起動 *****/
			if( ( x!=_x || y!=_y || trig!=_trig ) && trig != 0 ){
				mouse_off();
					if( prevcmd == cmdid_box_forcus )
						forcus_off();
					if( raise_command(x,y,trig) == ABORT )
						goto exit;
				mouse_on();
			}
		}
	}
	
exit:
	if( old_video_mode != 0x13 ){
		/* 元のモードが 13h の時は、何等かのアプリケーションから呼ばれていると
		 * 判断。迅速に移行するため、下手なメッセージは出さない。
		 */
		set_dspx( old_video_mode );
		fputs(isdosv() ?
			"VGA Pattern Editor \"InkHoki\"「いんくほーき」 Version " VER "\n"
			"\n"
			"Compiled on " __DATE__ " with LSI C-86 Ver.3.30c 試食版\n"
			"Copyright (C) 1994,95 by K.HaYaMa (葉山 薫)\n"
			"Copyleft for private uses.\n"
			"Bug reports to hay@kmc.kyoto-u.ac.jp\n"
			"\n"
			"Thank you for using InkHoki!\n"
		:
			"VGA Pattern Editor \"InkHoki\" Version " VER "\n"
			"\n"
			"Compiled on " __DATE__ " with LSI C-86 Ver.3.30c SISHOKU-BAN\n"
			"Copyright (C) 1994,95 by Kaoru Hayama \n"
			"Copyleft for private uses.\n"
			"Bug reports to hay@kmc.kyoto-u.ac.jp\n"
			"\n"
			"Thank you for using InkHoki!\n"
		, stderr );
	}
	return 0;
}
cmdid_t prevcmd = (cmdid_t)main;
