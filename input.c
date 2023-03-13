#include <jctype.h>
#include <stdlib.h>
#include <string.h>

#include "keyb.h"
#include "macros.h"
#include "video.h"
#include "sprite.h"
#include "console.h"

static void insert( char *ptr , int size , int spc )
{
	char *srcptr = ptr + size;
	char *dstptr = ptr + size + spc;
	
	while( size-- >= 0 ){
		*dstptr-- = *srcptr--;
	}
}

static void delete( char *ptr , int spc )
{
	char *srcptr = ptr+spc;
	char *dstptr = ptr;
	
	while( (*dstptr++ = *srcptr++) != '\0' )
		;
}

static void cursor( int x,int y )
{
	int i,j;
	
	x <<= 3;
	y <<= 3;
	
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			GRAM[ i+y ][ j+x ] ^= 1;
		}
	}
}

int input( int x , int y , char *buffer , int size , int color )
{
	char *attrib = malloc( size );
	int vm=get_video_mode();
	
	int	 pos=0 ,  len=0; /* Œ…”‚Å”‚¦‚½ê‡(bytes) */
	int jpos=0 , jlen=0; /* •¶Žš‚Å”‚¦‚½ê‡ */
	
	char *bufptr=buffer;
	char *atrptr=attrib;
	
	if( attrib==NULL )
		return -2;
	
	while( *bufptr != '\0' ){
		if( iskanji( *bufptr ) ){
			*atrptr++ = 2;
			bufptr += 2;
			len += 2;
		}else{
			*atrptr++ = 1;
			++bufptr;
			++len;
		}
		++jlen;
	}
	 pos =  len;
	jpos = jlen;
	*atrptr = 0;
	
	for(;;){
		int ch;
		
		gputat( x , y , buffer , color );
		gputat( x+len , y , "  " , color );
		
		locate( x+pos , y );
		
		if( vm==0x13 )	cursor(x+pos,y);
			ch = getkey() & 0xFF;
		if( vm==0x13 )	cursor(x+pos,y);
		
		switch( ch ){
		
		default:
			if(  len >= size-1  ) break;
			
			if(  iskanji(ch)  ){ /*** Š¿ Žš ***/
				
				insert( &buffer[ pos ] , len-pos , 2 );
				buffer[ pos++ ] = ch;
				buffer[ pos++ ] = getkey() & 0xFF;
				len += 2;
				
				insert( &attrib[ jpos ] , jlen-jpos , 1 );
				attrib[ jpos++ ] = 2;
				jlen++;
				
			}else if( isprint(ch) ){ /*** ‚`‚m‚j•¶Žš ***/
				
				insert( &buffer[  pos ] ,  len- pos , 1 );
				buffer[  pos++ ] = ch;
				len++;
				
				insert( &attrib[ jpos ] , jlen-jpos , 1 );
				attrib[ jpos++ ] = 1;
				jlen++;
			}
			break;
		
		case CTRL('B'):	/* backward */
			if( pos <= 0 ) break;
			
			if( attrib[ --jpos ]==2 ){
				pos -= 2;
			}else{
				pos--;
			}
			break;
			
		case CTRL('F'): /* forward */
			if( pos >= len ) break;
			
			if( attrib[ jpos++ ]==2 ){
				pos += 2;
			}else{
				pos ++;
			}
			break;
		
		case CTRL('H'): /* back-space */
			if( pos <= 0 ) break;
			
			if( attrib[ --jpos ]==2 ){
				pos -= 2;
			}else{
				pos--;
			}
			/* to next case */
		
		case CTRL('D'): /* delete */
			if( pos >= len ) break;
			
			if( attrib[ jpos ] == 2 ){
				delete( &buffer[  pos ] , 2 );
				len -= 2;
			}else{
				delete( &buffer[  pos ] , 1 );
				len--;
			}
			delete( &attrib[ jpos ] , 1 );
			jlen--;
			
			break;
			
		case '\r':
		case '\n':
			free( attrib );
			return len; /* “ü—Í */
		
		case '\x1B':
		case CTRL('G'):
			free( attrib );
			return -1; /* ^G , [ESC]‚É‚æ‚éabort */
		
		}/* switch */
	
	}/* for(;;) */
}
int prompt_input(int x,int y,const char *prompt,char *buffer,int size,int color)
{
	gputat( x, y , prompt , 15 );
	x += strlen( prompt );
	return input(x,y,buffer,size,color);
}
