#ifndef	MACROS_H
#define	MACROS_H

#define	numof(A)	(sizeof(A)/sizeof((A)[0]))
#define	CTRL(A)		((A) & 0x1F)
#define ESC			"\x1B"
#define FNULL		((void far *)0L)

typedef enum{ OK , NG } status_t;

/* インラインアセンブリ関数 プロトタイプ宣言用 型定義 */
typedef const char	   *SOURCE;
typedef unsigned char	BYTE;
typedef unsigned short	WORD;

unsigned intsqrt( unsigned long numer , unsigned short *amari );
BYTE _asm_BW(SOURCE,WORD);
#define idling()	_asm_BW("\n\tINT\t2Fh\n",0x1680)

#endif /* MACROS.H */
