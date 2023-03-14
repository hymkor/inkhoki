#ifndef BITMAP_H
#define BITMAP_H

/* ヘッダ形式
 *	0..15 : struct bmp_head (全BMPファイル共通ヘッダ)
 * Windows(ヘッダサイズ==28h)
 *  16..53 : struct win_head (Windows用ヘッダ)
 *  53..   : char [][4]      (Windows用パレット情報)
 * OS/2 (ヘッダサイズ==0Ch)
 *	16..24 : struct os2_head (OS/2用ヘッダ)
 *	25..   : char [][3]      (OS/2用パレット情報)
 * ビットマップ
 *  14,15 : unsigned long	(ヘッダサイズ)
 */

struct bmp_head {
	unsigned char	id[2];
	unsigned long	size;
	unsigned short	zero[2];
	unsigned long	start;
	unsigned long	head_size;
};

struct win_head {
	unsigned long	xpixels ,
					ypixels ;
	unsigned short	nplanes ,
					bits_per_pixel ;
	unsigned long	how_to_compress ,
					size ,
					xpixels_per_meter ,
					ypixels_per_meter ,
					npalettes ,
					main_palettes;
};

struct os2_head {
	unsigned short	xpixels ,
					ypixels ,
					nplanes ,
					bits_per_pixel ;
};

enum{
	WIN_HEAD_SIZE=0x28,
	OS2_HEAD_SIZE=0x0C,
};

union bmp_head2 {
	struct win_head win;
	struct os2_head os2;
};

#endif
