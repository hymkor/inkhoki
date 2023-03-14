#ifndef GARBAGES_H
#define GARBAGES_H

/* int型の高速メモリ割り当てをしたい時は、
 *		
 *		GARBAGES gars=GARTYPE(int);
 *			:
 *		int *x=garmalloc(gars);
 *			:
 *		garfree(x);
 *	とする。
 */

#ifndef NULL
#	include <stddef.h>
#endif

typedef struct{
	char *frontier; /* N_ONCE個まるごとmallocで得た領域 */
	void *garbages; /* freeされたオブジェクトのリスト */
	int   left ;	/* frontierに残っているサイズ [回分] */
	int	  size ;	/* 一回あたりに返すサイズ [bytes] */
}garbage_t[ 1 ];

typedef struct{
	char far *frontier; /* N_ONCE個まるごとmallocで得た領域 */
	void far *farbages; /* freeされたオブジェクトのリスト */
	int       left ;	/* frontierに残っているサイズ [回分] */
	int       size ;	/* 一回あたりに返すサイズ [bytes] */
}farbage_t[ 1 ];

#define	GARTYPE(type)	{{ NULL , NULL , 0 , sizeof(type) }}

#define garbage_free(gars,obj)	(\
	*(void**)(obj)=(gars)->garbages,(gars)->garbages=(obj)\
)
#define farbage_free(gars,obj)	{\
	*(void far* far*)(obj)=(gars)->farbages,(gars)->farbages=(obj)\
)

void near *garbage_malloc( garbage_t gars );
void far  *farbage_malloc( farbage_t gars );

#endif /* GARBAGES_H */
