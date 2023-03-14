#ifndef	EMS_H
#define	EMS_H

#include "macros.h"

short  ems_init( void ); 
	/* 帰り値 : EMSページフレイムアドレス ( == -1  : EMSは存在しない/エラー )*/

short  ems_alloc( unsigned page );
	/* 帰り値 : ハンドル番号　( == -1 : エラー ) */

short  ems_set_name( short handle, const char name[8] );
	/* 帰り値 : EMMステイタス ( != 0 : エラー )
	 * nameは８文字で、満たない部分は\0で埋める。
	 */

short  ems_find_handle(const char name[8]);
	/* 帰り値: !=(-1) ハンドル
	 *         ==(-1) 見つからなかった。
	 */

WORD _asm_WWWNW(SOURCE,WORD,WORD,WORD(*)(),WORD);
WORD _asm_WWNNW(SOURCE,WORD,WORD (*)(),WORD (*)(),WORD);
WORD _asm_WW   (SOURCE,WORD);

#define	ems_map(handle,log,phy)		\
	(_asm_WWWNW("\n\tINT\t67h\n",0x4400|(log),(phy),_asm_WWWNW,(handle))>>8)
	/* 帰り値 : EMSステイタス */

#define ems_free(handle)	\
	(_asm_WWNNW("\n\tINT\t67h\n",0x4500,_asm_WWNNW,_asm_WWNNW,(handle))>>8)
	/* 帰り値 : EMSステイタス */

#define ems_get_size()	\
	_asm_WW("\n\tPUSH\tDX\n\tINT\t67h\n\tPOP\tDX\n\tMOV\tAX,BX\n",0x4200)
	/* 帰り値 : 未割り当てページ数 */





/* EMSsbrk.C */

#define PAGE_SIZE	0x4000
typedef short PageMap[ 4 ];

#ifndef NULL
#include <stddef.h>
#endif

extern unsigned default_ems_pages;
void far *emssbrk( short *_page , size_t size );
void back_emssbrk( size_t size );
void kill_emssbrk( void );
short call_sbrk_area( short page );
void set_emssbrk_handle_name( const char *name );
void save_sbrk_page_map( PageMap map );
void load_sbrk_page_map( const PageMap map );


#endif /* EMS_H */
