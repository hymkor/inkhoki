#include <conio.h>
#include <ctype.h>
#include <direct.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "pe.h"
#include "mice.h"
#include "files.h"

static char old_current_directories[ 'Z'-'A' + 1 ][ 64 ];
static int old_current_drive;

static void load_current_paths( void )
{
	int i;
	for( i=0 ; i < numof(old_current_directories) ; i++ ){
		if( old_current_directories[i][0] != '\0'  
		 && ( !is_changable_drive(i) || !is_disk_changed(i) )
		 && set_current_drive( i ) != 0 ){
			chdir( old_current_directories[i] );
		}
	}
	set_current_drive( old_current_drive );
}

void save_current_paths( void )
{
	atexit( load_current_paths );
	
	memset( old_current_directories , 0 , sizeof( old_current_directories ) );
	old_current_drive = get_current_drive();
	getcwd( old_current_directories[old_current_drive] , 64 );
}

static int set_current_directory( const char *dir )
{
	int curdrv=get_current_drive();
	if( old_current_directories[ curdrv ][ 0 ] == 0   ||  is_disk_changed( curdrv )  ){
		if( get_current_directory( curdrv , old_current_directories[ curdrv ] ) != 0 ){
			old_current_directories[ curdrv ][ 0 ] = '\0';
		}
	}
	return chdir( dir );
}


/* 2 �~ 16 = 32 �� �t�@�C���E�B���h�E���J�� */

enum{
	NEXT_SWITCH = 30,	PREV_SWITCH = 31 , MAX = 32 ,
	SWITCH_LINE = 15,
};

static int file_selector( char *fname , const char *wild )
{
	int i;
	int result = 0;
	Files *top=FilesOpen(wild);
	
	for(;;){
		int x,y,s;
		Files *cur;
		int nfiles;
		int ndrives;
		
		int current_drive=get_current_drive();
		box_fill( 0,0 , ICON_RIGHT , BOTTOMS , 0 );
		
		cur=top;
		
		for( nfiles=0  ;  cur != NULL && nfiles < NEXT_SWITCH  ;  nfiles++  ){
			gputat( nfiles&1 ? 12 : 0  ,  nfiles>>1  ,  cur->name  ,  cur->attrib & _A_SUBDIR ? 6 : 7 );
			xor_box_fill( nfiles&1 ? 96 : 0 , (nfiles & ~1)<<2 , 12*8-1 , 7 , 8 );
			cur = FilesNext(cur);
		}

		if( cur != NULL ){
			gputat( 0 , SWITCH_LINE , " \x1f\x1f MORE \x1f\x1f" , 2 );
			xor_box_fill( 0 , SWITCH_LINE<<3 , 12*8-1 , 7 , 8 );
		}

		if( top == NULL ){
			gputat( 0 , 0 , "NO-FILES" , 12 );
		}else if( FilesPrev(top) != NULL ){
			gputat( 12 , SWITCH_LINE , " \x1e\x1e LESS \x1e\x1e" , 2 );
			xor_box_fill( 96 , SWITCH_LINE<<3 , 12*8-1 , 7 , 8 );
		}
		
		/* �f�B���N�g���ꗗ��\�� */
		for( ndrives=0 ; ; ndrives++ ){
			char buffer[4];
			
			switch( is_changable_drive( ndrives)  ){
				case 0: buffer[0] = '<';	buffer[2] = '>'; break;
				case 1: buffer[0] = '[';	buffer[2] = ']'; break;
				default:goto Break;
			}
			buffer[1] = 'A'+ndrives;
			buffer[3] = '\0';
			
			if( ndrives < 13 ){
				gputat( ndrives*3 , MESSAGE_LINE  , buffer , 7 );
				xor_box_fill( ndrives*24 , MESSAGE_LINE<<3 , 23 , 7 , 8 );
			}else{
				gputat( (ndrives-13)*3 , USAGE_LINE , buffer , 7 );
				xor_box_fill( (ndrives-13)*24 , USAGE_LINE<<3 , 23 , 7 , 8 );
			}
		}
		Break:
		/* �J�����g�h���C�u�̐F�����ς��� */
		if( current_drive < 13 )
			xor_box_fill( current_drive*24 , MESSAGE_LINE<<3 , 23 , 7 , 8  );
		else
			xor_box_fill( (current_drive-13)*24 , USAGE_LINE<<3 , 23 , 7 , 8 );
		
		mouse_on();
		while( mouse_alt(NULL,NULL)!=0 )
			;
		while( mouse_alt(&x,&y)==0 ){
			if( kbhit() && getch()=='\x1b' ){
				mouse_off();
				goto abort;
			}
		
		}
		mouse_off();
		
		s=(x<96 ? 0:1 ) + (y>>3)*2;
		
		/** �h���C�u�X�C�b�` **/
		if( y >= MESSAGE_LINE*8 ){
			int d = x / 24 + (y/8-MESSAGE_LINE)*13;
			if( d < ndrives ){
				set_current_drive( d );
				top = FilesOpen( wild );
			}
		
		/** �I�� **/
		}else if( x >= 192  ||  y >= 128 ){
			result = -1;
			break;
		
		/** MORE�X�C�b�` **/
		}else if( s == NEXT_SWITCH ){
			if( cur != NULL )
				top = cur;
		
		/** LESS�X�C�b�`**/
		}else if( s == PREV_SWITCH ){
			int i;
			for( i=0 ; i<NEXT_SWITCH && FilesPrev(top)!=NULL ; i++){
				top = FilesPrev(top);
			}
		
		/** �t�@�C���I�� **/
		}else{
			if( s < nfiles ){
				for( i=0 ; i<s ; i++){
					top = FilesNext(top);
				}
				if( top->attrib & _A_SUBDIR ){
					set_current_directory( top->name );
					top = FilesOpen( wild );
				}else{
					strcpy(fname,top->name);
					break;
				}
			}
		}
	}
abort:
	FilesClose();
	
	clear_message_line();
	clear_usage_line();
	
	return result;
}

static void remove_any_space(char *sp)
{
	char *dp=sp;
	while( *sp != '\0' ){
		if( !isspace(*sp) ){
			*dp++ = *sp++;
		}
	}
	*dp = '\0';
}
static int is_directory(const char *path)
{
	struct stat status;
	
	stat( path , &status );
	return (status.st_mode & S_IFMT) == S_IFDIR;
}

int input_fname(char *path)
{
	char wildcard[16];
	for(;;){
		int len;
		strcpy( wildcard , "*.*" );
	
		writes( 0 , USAGE_LINE , "ESC...CANCEL" , 1 );
		len=prompt_input( 0, MESSAGE_LINE,"File Name:" , path , 28 , 15);
		clear_message_line();
		clear_usage_line();
		
		/*** �t�@�C�����L�[�{�[�h���� ***/
		
		if( len < 0 ){  /* �L�����Z���Ȃ�ΏI�� */
			return -1;
		}else if( len > 0 ){
			char *directory = path;
			int	  drive = get_current_drive();
			remove_any_space( path );
			
			/* �h���C�u�����L�q����Ă���Ƃ� */
			if( path[1] == ':' ){
				if( isalpha( path[0] ) ){
					int _drive = path[0] - (islower(path[0]) ? 'a' : 'A');
					if( set_current_drive( _drive ) >= _drive ){
						drive = _drive;
					}else{
						writes( 20 , USAGE_LINE , "Bad Drive !!!" , 12 );
						continue;	/* ��ԊO���� for(;;){...}�̃g�b�v�ֈړ� */
					}
				}
				directory += 2;
			}
			
			if( strchr(path,'*') == NULL  &&  strchr(path,'?') == NULL ){
			/* ���C���h�J�[�h�������Ă��Ȃ��� */
				if( ! is_directory(path) ){	/* �f�B���N�g�����łȂ��Ƃ��́A������̗p */
					return len;
				}else{						/* �f�B���N�g�����̂Ƃ��́ACHDIR���� */
					if( set_current_directory( path ) ){
						writes( 20 , USAGE_LINE , "Bad Directory !!!" , 12 );
						continue;
					}
				}
			}else{
			/* ���C���h�J�[�h�������Ă��鎞,
			 * ���C���h�J�[�h�����ƃf�B���N�g�������ɕ�������
			 */
				char *lastroot = strrchr(path,'\\');
				
				if( lastroot == directory ){		/* ٰ��ިڸ�؂̎�,�ިڸ�ؕ����Ɂu\�v���c�� */
					strcpy( wildcard , lastroot+1 );
					lastroot[1] = '\0';
					if( set_current_directory( path ) ){
						writes( 20 , USAGE_LINE , "Bad Directory !!!" , 12 );
						continue;	/* ��ԊO���� for(;;){...}�̃g�b�v�ֈړ� */
					}
				}else if( lastroot != NULL ){		/* ��ٰ��ިڸ�؂̎�,�ިڸ�ؕ�������u\�v����� */
					strcpy( wildcard , lastroot+1 );
					lastroot[0] = '\0';
					if( set_current_directory( path ) ){
						writes( 20 , USAGE_LINE , "Bad Directory !!!" , 12 );
						continue;	/* ��ԊO���� for(;;){...}�̃g�b�v�ֈړ� */
					}
					
				}else{								/* �����ިڸ�ب�ɑ΂��鑀�� */
					strcpy( wildcard , directory );
				}
			}
		}
		/*** �t�@�C���Z���N�^�N�� ***/
		
		if( file_selector(path,wildcard) < 0 ){
			icon_init();
			edit_screen_flush();
			return -1;
		}
		icon_init();
		edit_screen_flush();
	}
}
