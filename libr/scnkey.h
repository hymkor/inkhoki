#ifndef SCNKEY_H
#define	SCNKEY_H

enum{
	HENKAN_KEY	= 121,
	MUHENKAN_KEY= 123,
};

extern volatile unsigned char  scantable[128];
extern volatile unsigned char  lastkey;

void swap_int15h( void );
void restore_int15h( void );
void keybuf_clear( void );

int scankey_init(void);

#endif
