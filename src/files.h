#ifndef	FILES_H
#define	FILES_H

/* ドライブ番号は
 *   0..`A:'  1..`B:'  2..`C:'   -1..ｶﾚﾝﾄﾄﾞﾗｲﾌﾞで統一
 */

short _asm_www(const char *,short,short);
#define is_changable_drive(drive)	\
	_asm_www("\n\tINT\t21h\n\tRCL\tBX,1\n\tSHL\tBX,1\n\tOR\tAX,BX\n\tAND\tAX,3",0x4408,drive)
/* drive...A:'1' , B:'2' , ｶﾚﾝﾄﾃﾞｨﾚｸﾄﾘｰ:'0'
 *	== 0 : 交換可能  1 交換不可能  2 無効なドライブ  -1 無効なファンクション
 */

short _asm_wwnnw(const char *,short,short (*)(),short (*)(),short);
#define	is_disk_changed(drive)	\
	_asm_wwnnw("\n\tINT\t13h\n\tRCL\tAX,1\n\tAND\tAX,1\n",0x1600,_asm_wwnnw,_asm_wwnnw,drive)
/* == 0 : 未交換  1:交換された */

typedef struct Files {
	struct Files	*next,*prev;
	char	name[ 16 ];
	int		attrib;
}Files;

Files	*FilesOpen ( const char *fname);
void 	 FilesClose( void );

Files	*FilesNext( Files *f );
#define	 FilesPrev(f)	((f)->prev)

int parse_path( const char *path , char *dir , char *wildcard );

enum{
	PARSED_ERROR		=-1,	/* そもそもパスがおかしい */

	PARSED_WILD_CARD	= 0x80,	/* ﾜｲﾙﾄﾞｶｰﾄﾞを持っている*/
	PARSED_CURRENT_DIR	= 0x40,	/* ｶﾚﾝﾄﾃﾞｨﾚｸﾄﾘｨｰ */
	PARSED_ROOT_DIR		= 0x20, /* ﾙｰﾄﾃﾞｨﾚｸﾄﾘｨｰ */

	PARSED_DRVNO_MASK	= 0x1F,
	/* ドライブ番号を得るためのマスク
	 *	0 = ｶﾚﾝﾄﾄﾞﾗｲﾌﾞ  1 = 'A:'  2 = 'B:' ... 
	 */
};

size_t fixpath( char *path );
int get_current_drive( void );
int set_current_drive( int no );
int get_current_directory( int drive, char *path );

#endif	/*** FILES_H ***/
