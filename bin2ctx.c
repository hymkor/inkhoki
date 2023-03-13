#include <stdio.h>

void main( int argc , char **argv )
{
	int column=0;
	int ch;
	FILE *fp;
	
	if( argc < 2  ||  (fp=fopen(argv[1],"rb"))==NULL ){
		return ;
	}
	
	putc( '\t' , stdout );
	while( (ch=getc(fp)) != EOF ){
		printf( "0x%02X, " , ch );
		if( ++column >= 16 ){
			printf( "\n\t" , stdout );
			column=0;
		}
	}
	fclose(fp);
}
