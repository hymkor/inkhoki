#ifndef	FILES_H
#define	FILES_H

/* �h���C�u�ԍ���
 *   0..`A:'  1..`B:'  2..`C:'   -1..������ײ�ނœ���
 */

short _asm_www(const char *,short,short);
#define is_changable_drive(drive)	\
	_asm_www("\n\tINT\t21h\n\tRCL\tBX,1\n\tSHL\tBX,1\n\tOR\tAX,BX\n\tAND\tAX,3",0x4408,drive)
/* drive...A:'1' , B:'2' , �����ިڸ�ذ:'0'
 *	== 0 : �����\  1 �����s�\  2 �����ȃh���C�u  -1 �����ȃt�@���N�V����
 */

short _asm_wwnnw(const char *,short,short (*)(),short (*)(),short);
#define	is_disk_changed(drive)	\
	_asm_wwnnw("\n\tINT\t13h\n\tRCL\tAX,1\n\tAND\tAX,1\n",0x1600,_asm_wwnnw,_asm_wwnnw,drive)
/* == 0 : ������  1:�������ꂽ */

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
	PARSED_ERROR		=-1,	/* ���������p�X���������� */

	PARSED_WILD_CARD	= 0x80,	/* ܲ��޶��ނ������Ă���*/
	PARSED_CURRENT_DIR	= 0x40,	/* �����ިڸ�ب� */
	PARSED_ROOT_DIR		= 0x20, /* ٰ��ިڸ�ب� */

	PARSED_DRVNO_MASK	= 0x1F,
	/* �h���C�u�ԍ��𓾂邽�߂̃}�X�N
	 *	0 = ������ײ��  1 = 'A:'  2 = 'B:' ... 
	 */
};

size_t fixpath( char *path );
int get_current_drive( void );
int set_current_drive( int no );
int get_current_directory( int drive, char *path );

#endif	/*** FILES_H ***/
