#ifndef GFLIB2_TGA_STRUCT_H_INCLUDED
#define GFLIB2_TGA_STRUCT_H_INCLUDED

typedef struct _TGA_HEADER
{
	unsigned char	idFieldLength;
	unsigned char	colorMapType;
	unsigned char	imageType;
	unsigned char	colorMapIndex[2];
	unsigned char	colorMapLength[2];
	unsigned char	colorMapSize;
	unsigned short	imageOriginX;
	unsigned short	imageOriginY;
	unsigned short	imageWidth;
	unsigned short	imageHeight;
	unsigned char	bitPerPixel;
	union
	{
		struct
		{
			unsigned char	discriptor;
		};
		// Windows環境で実行するツールなので, リトルエンディアン以外は想定しない
		struct
		{
			unsigned char attribute	:4;		// Bit 0-3 属性
			unsigned char turnOverU	:1;		// Bit 4 画素データの左右の格納方向(0 : 左→右, 1 : 右→左)
			unsigned char turnOverV	:1;		// Bit 5 画素データの上下の格納方向(0 : 下→上, 1 : 上→下)
			unsigned char reserved	:2;		// Bit 7-8 予約領域
		};
	};
}TGA_HEADER;

typedef struct _TGA_FOOTER
{
	unsigned int	extensionOffset;
	unsigned int	developerAreaOffset;
	unsigned char	signature[18];			// "TRUEVISION-XFILE."
}TGA_FOOTER;
#endif