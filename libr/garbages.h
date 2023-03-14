#ifndef GARBAGES_H
#define GARBAGES_H

/* int�^�̍������������蓖�Ă����������́A
 *		
 *		GARBAGES gars=GARTYPE(int);
 *			:
 *		int *x=garmalloc(gars);
 *			:
 *		garfree(x);
 *	�Ƃ���B
 */

#ifndef NULL
#	include <stddef.h>
#endif

typedef struct{
	char *frontier; /* N_ONCE�܂邲��malloc�œ����̈� */
	void *garbages; /* free���ꂽ�I�u�W�F�N�g�̃��X�g */
	int   left ;	/* frontier�Ɏc���Ă���T�C�Y [��] */
	int	  size ;	/* ��񂠂���ɕԂ��T�C�Y [bytes] */
}garbage_t[ 1 ];

typedef struct{
	char far *frontier; /* N_ONCE�܂邲��malloc�œ����̈� */
	void far *farbages; /* free���ꂽ�I�u�W�F�N�g�̃��X�g */
	int       left ;	/* frontier�Ɏc���Ă���T�C�Y [��] */
	int       size ;	/* ��񂠂���ɕԂ��T�C�Y [bytes] */
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