--
#include <stdlib.h>	/* for NULL,malloc */
#include "garbages.h"

#define	N_ONCE	16

- garbages.c

void *garbage_malloc( garbage_t gars )
{
	if( gars->garbages != NULL ){
		void *result=gars->garbages;
		gars->garbages = *(void**)gars->garbages;
		return result;
	}
	
	if( gars->left <= 0 ){
		gars->frontier = malloc( gars->size*N_ONCE );
		if( gars->frontier == NULL )
			return NULL;
		gars->left = N_ONCE;
	}
	
	return &gars->frontier[ --gars->left * gars->size ];
}

- farbages.c
#include <dos.h>	/* for farmalloc */

void far *farbage_malloc( farbage_t gars )
{
	if( gars->farbages != NULL ){
		void far *result=gars->farbages;
		gars->farbages = *(void far* far*)gars->farbages;
		return result;
	}
	
	if( gars->left <= 0 ){
		gars->frontier = farmalloc( gars->size*N_ONCE );
		if( gars->frontier == NULL )
			return NULL;
		gars->left = N_ONCE;
	}
	
	return &gars->frontier[ --gars->left * gars->size ];
}

--
#include <stddef.h>
- intsqrt.c

/* numer = (帰り値)**2 + amari */

unsigned intsqrt( unsigned long numer , unsigned short *amari )
{
	unsigned result=0;	/* 平方根(結果) */
	unsigned denom=0;	/* 除数 */
	
	int numbit=32;
	
	int bit=16;
	while( bit-- > 0 ){
		numbit  -= 2;
		result <<= 1;
		denom  <<= 1;
		
		/* 十進数だと、(denox+x)*x (x=0..9)となる */
		if( (numer>>numbit) >= denom+1  ){	
			numer -= (denom+1)<<numbit;
			
			result ++;
			denom += 2;
		}
	}
	if( amari != NULL )
		*amari = (unsigned short)numer;
	return result;
}
