#include	<direct.h>
#include	<stdlib.h>		/* for atexit */
#include	<stdio.h>
#include	<string.h>		/* for memcpy */
#include	<dos.h>
#include	"files.h"

int get_current_drive( void )
{
	unsigned drive;
	_dos_getdrive( &drive );
	return drive-1;
}
int set_current_drive( int no )
{
	unsigned ndrive;
	_dos_setdrive( no+1 , &ndrive );
	return ndrive;
}

int get_current_directory( int drive, char *path )
{
	int olddrive=get_current_drive();
	if( set_current_drive(drive) < drive )
		return 1;
	
	getcwd(path,64);
	set_current_drive(olddrive);
	return 0;
}

struct find_t	block;
char fullpath[ FILENAME_MAX ];
Files *first = NULL ;
int	endf = 1;

Files *FilesNext( Files *f )
{
	Files *result;
	if( f->next != NULL   ||   endf   ||   ( endf=_dos_findnext(&block) ) != 0   )
		return f->next;
	
	result = malloc(sizeof(struct Files));
	if( result == NULL ){
		endf = 1;
		return NULL;
	}
	
	strcpy( result->name , block.name );
	result->attrib = block.attrib;
	
	result->prev = f;
	result->next = NULL;
	f->next = result;
	
	return result;
}

Files *FilesOpen( const char *fname )
{
	static int atexit_flag = 1;
	Files *result;
	
	if( atexit_flag ){
		atexit( &FilesClose );
		atexit_flag = 0;
	}
	
	strcpy( fullpath , fname );
	
	endf = _dos_findfirst( fname , _A_RDONLY | _A_SUBDIR | _A_ARCH , &block );
	if( endf )
		return NULL;
	
	result = malloc( sizeof(struct Files) );
	if( result == NULL ){
		endf = 1;
		return NULL;
	}
	
	strcpy( result->name , block.name );
	result->attrib = block.attrib ;
	
	result->prev = NULL ;
	result->next = NULL ;
	
	return result;
}

void FilesClose( void )
{
	Files *pre;
	Files *cur = first ;
	
	if( endf )
		return;
	
	while( cur != NULL ){
		pre = cur;
		cur = cur->next;
		free(cur);
	}
	endf = 1;
}

int FilesMakePath(struct Files *f , char *path)
{
	char *lastroot;
	
	if( f->name[0] == '.' ){
		if( f->name[1] != '.' )
			return 1;
		else
			return 2;
	}
	strcpy( path , fullpath );
	lastroot=strrchr( path , '\\' );
	if( lastroot == NULL ){
		strcpy( path , f->name );
	}else{
		strcpy( lastroot+1 , f->name );
	}
}
