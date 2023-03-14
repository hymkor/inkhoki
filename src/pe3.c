#include "pe.h"
struct realbuffer_t RealBuffer[ HEIGHT ][ WIDTH ];

void dotset(int x,int y,int color)
{
	int X=x<<ScaleBit,
		Y=y<<ScaleBit;
	
	if( color < 0 ){
		box_fill(X,Y,Scale,Scale,0);
		GRAM[ Y+Scale/2 ][ X+Scale/2 ]	= (x%8==7 || y%8==7) ? 3:7;
	}else{
		box_fill(X,Y,Scale,Scale,color);
	}
	
	real_pset( CurrentPattern.x + x , CurrentPattern.y + y , color );
}

void edit_screen_flush( void )
{
	int y;
	for( y=0; y<EditSize; y++){
		int x;
		for( x=0; x<EditSize; x++){
			dotset(x,y, real_attr( CurrentPattern.x+x , CurrentPattern.y+y )  );
		}
	}
}

void edit_screen_init( void )
{
	int y;
	for( y=0; y<EditSize; y++){
		int x;
		for( x=0; x<EditSize; x++){
			dotset(x,y,-1);
		}
	}
}


void real_screen_init( void )
{
	int y;
	for( y=0 ; y < HEIGHT ; y++){
		int x;
		for( x=0 ;  x < WIDTH ; x++){
			RealBuffer[ y ][ x ].color =  0;
			RealBuffer[ y ][ x ].attr  =  1;
		}
	}
}

int real_pset(int x,int y,int color)
{
	if( color >= 0 ){
		GRAM[ Y0 + y ][ X0 + x ] = color;
		RealBuffer[ y ][ x ].color = color;
		RealBuffer[ y ][ x ].attr  = 0;
	}else{
		GRAM[ Y0 + y ][ X0 + x ] = 0;
		RealBuffer[ y ][ x ].color = 0;
		RealBuffer[ y ][ x ].attr  = 1;
	}
	return 0;
}

int real_preset(int x,int y)
{
	GRAM[ Y0 + y ][ X0 + x ] = 0;
	RealBuffer[ y ][ x ].color = 0;
	RealBuffer[ y ][ x ].attr  = 1;
	return 0;
}

int real_repaint( void )
{
	int y;
	for( y=0 ; y < HEIGHT ; y++ ){
		int x;
		for( x=0 ; x < WIDTH ; x++ ){
			GRAM[ Y0 + y ][ X0 + x ] = RealBuffer[y][x].attr ? 0 : RealBuffer[y][x].color  ;
		}
	}
	return 0;
}
