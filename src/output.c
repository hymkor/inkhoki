#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "mice.h"
#include "pe.h"

char default_fname[ FILENAME_MAX ];
CmdStatus (*default_saver)(FILE *fp,int,int,int,int);

int select_2nd_icon( struct icontable *table , int n )
{
	int i,x,y,no;
	
	box_fill( ICON_COLUMN*8 , 0 , 8*8 , 128 , 0 );
	for( i=0 ; i<n ; i++ ){
		gputat( ICON_COLUMN , i , table[ i ].name , 7 );
		xor_box_fill( ICON_COLUMN<<3 , i<<3 , 8*8-1 , 7 , 8 );
	}
	while( mouse_alt(NULL,NULL) != 0 )
		;
	mouse_on();
	while( mouse_alt(&x,&y) == 0 ){
		if( kbhit() ){
			int ch=getch();
			
			if( ch == '\x1b' ){
				mouse_off();
				return -1;
			}else{
				if( isupper(ch) )
					ch = _tolower(ch);
				for( i=0 ; i<n ; i++ ){
					if( table[i].shortcut == ch ){
						no=i;
						goto exit;
					}
				}
			}
		}
	}
	while( mouse_alt(NULL,NULL) != 0 )
		;
	no = y / 8;
	if( x<ICON_COLUMN*8 || x>(ICON_COLUMN+8)*8 || no>=n )
		return -1 ;
exit:
	mouse_off();
	xor_box_fill( ICON_COLUMN<<3 , no<<3 , (8*8-1) , 7 , 8 );
	return no;
}

static void SaveLoad(const char *mode,struct icontable *format,int n,int x0,int y0,int width,int height){
	int no;
	char fname[ FILENAME_MAX ] = "\0";
	FILE *fp;
	
	no = select_2nd_icon( format , n );
	
	if( no<0 || input_fname(fname)<=0 )
		return;
	
	clear_message_line();
	clear_usage_line();
	
	fp=fopen( fname , mode );
	if( fp==NULL){
		writes(0,MESSAGE_LINE,"Cannot open the file !",15);
		getch();
		return;
	}
	
	if( (*format[no].func)(fp,x0,y0,width,height) != CONTINUE ){
		writes(0,MESSAGE_LINE,"Bad format !",15);
		getch();
	}
	
	fclose(fp);
}

static CmdStatus WinSaveLoad( const char *mode , struct icontable *format , int n )
{
#if 0 /* 非正方形SAVE/LOADサポート */
	if( prevcmd == cmdid_box_forcus ){
		forcus_off();
		SaveLoad( mode,format,n , CurrentPattern.x+forcus_x , CurrentPattern.y+forcus_y
			,forcus_width,forcus_height );
		forcus_off();
	}else{
		SaveLoad( mode , format,n , CurrentPattern.x , CurrentPattern.y
					, EditSize, EditSize );
	}
#else /* LOAD時のサイズ自動計算サポート*/
	SaveLoad( mode , format,n , CurrentPattern.x , CurrentPattern.y
				, EditSize, EditSize );
#endif
	icon_init();
	clear_message_line();
	return CONTINUE;
}
static CmdStatus AllSaveLoad( const char *mode , struct icontable *format , int n )
{
	SaveLoad( mode , format , n , 0 , 0 , 128 , 128 );
	
	icon_init();
	clear_message_line();
	return CONTINUE;
}

/***** SAVE ROUTINE ****/
CmdStatus save_dotskip(FILE *fp,int x,int y,int width,int height);
CmdStatus save_standard(FILE *fp,int x,int y,int width,int height);
CmdStatus save_plane16(FILE *fp,int x,int y,int width,int height);
CmdStatus save_windows_bitmap(FILE *fp,int x,int y,int width,int height);
CmdStatus save_palette(FILE *fp,int x1,int y1,int width,int height);

struct icontable saveformat[] = {
	{ "Skip-Dot",save_dotskip			,'s' },
	{ "  BETA  ",save_standard			,'b' },
	{ "16colors",save_plane16			,'6' },
	{ "DIB/BMP ",save_windows_bitmap	,'d' },
	{ "Palette ",save_palette			,'p' },
};

CmdStatus WinSaver( void )
{
	return WinSaveLoad( "wb" , saveformat , numof(saveformat) );
}

CmdStatus AllSaver( void )
{
	return AllSaveLoad( "wb" , saveformat , numof(saveformat) );
}

/**** LOAD ROUTINE ****/
CmdStatus load_dotskip(FILE *fp,int x1,int y1,int width,int height);
CmdStatus load_standard(FILE *fp,int x1,int y1,int width,int height);
CmdStatus load_plane16(FILE *fp,int x1,int y1,int width,int height);
CmdStatus load_windows_bitmap(FILE *fp,int x1,int y1,int width,int height);
CmdStatus load_palette(FILE *fp,int x1,int y1,int width,int height);

struct icontable loadformat[]={
	{ "Skip-Dot",load_dotskip		,'s' },
	{ "  BETA  ",load_standard		,'b' },
	{ "16colors",load_plane16		,'6' },
	{ "DIB/BMP ",load_windows_bitmap,'d' },
	{ "Palette ",load_palette		,'p' },
};

CmdStatus WinLoader( void )
{
	WinSaveLoad( "rb" , loadformat , numof(loadformat) );
	edit_screen_flush();
	return CONTINUE;
}

CmdStatus AllLoader( void )
{
	AllSaveLoad( "rb" , loadformat , numof(loadformat) );
	edit_screen_flush();
	return CONTINUE;
}

struct suffix {
	const char suffix[4];
	CmdStatus (*loader)(FILE *fp,int,int,int,int);
	CmdStatus (*saver )(FILE *fp,int,int,int,int);
} suffix_table[] = {
	{ "PAL"	, load_palette			, save_palette	},
	{ "PE"	, load_standard			, save_standard	},
	{ "SPR" , load_dotskip			, save_dotskip	},
	{ "BMP" , load_windows_bitmap	, save_windows_bitmap },
};


const char *do_option( const char *fname )
{
	const char *suffix=fname;
	FILE *fp;
	int i;
	
	while( *suffix != '.' ){
		if( *suffix++ == '\0' ){
			return	"No Suffix.\n"
					"       .PAL ... PALETTE\n"
					"       .PE  ... BETA\n"
					"       .SPR ... SKIPDOT\n"
					"       .BMP ... Windows Bitmap\n" ;
		}
	}
	if( (fp=fopen(fname,"rb"))==NULL ){
		return "Can't Open.\n" ;
	}
	for( i=0 ; i<numof(suffix_table) ; i++ ){
		if( stricmp( suffix+1 , suffix_table[i].suffix )==0 ){
			(*suffix_table[i].loader)(fp,0,0,128,128);
			fclose(fp);
			default_saver = suffix_table[i].saver;
			strcpy( default_fname , fname );
			return NULL;
		}
	}
	fclose(fp);
	return	"Bad Suffix.\n"
			"       .PAL ... PALETTE\n"
			"       .PE  ... BETA\n"
			"       .SPR ... SKIPDOT\n"
			"       .BMP ... Windows Bitmap\n";
}

CmdStatus Quit( void )
{
	if( default_fname[0] != '\0' ){
		char message[40];
		sprintf( message , "Save All as \"%s\"?" , default_fname );
		if( AreYouSure(message)==CONTINUE ){
			FILE *fp=fopen(default_fname,"wb");
			if( fp == NULL ){
				return AreYouSure("Save failure! Quit sure ?")==CONTINUE 
					? ABORT : CONTINUE ;
			}
			(*default_saver)(fp,0,0,128,128);
			fclose(fp);
		}
	}else{
		if( AreYouSure("Save All ?")==ABORT ||  AllSaver() == ABORT ){
			return AreYouSure("Not Saved. Quit sure ?")==CONTINUE
				? ABORT : CONTINUE ;
		}
	}
	return AreYouSure("Quit Sure ?")==CONTINUE ? ABORT : CONTINUE;
}
