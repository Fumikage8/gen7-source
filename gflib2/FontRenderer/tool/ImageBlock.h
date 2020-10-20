#ifndef GFLIB2_IMAGE_BLOCK_H_INCLUDED
#define GFLIB2_IMAGE_BLOCK_H_INCLUDED
#include "tga_struct.h"
typedef enum _COLOR_TYPE
{
	COLOR_RGB	= 3,	// RGB[3Byte]
	COLOR_RGBA	= 4,	// RGBA[4Byte]
}COLOR_TYPE;

class ImageBlock
{
public:
	ImageBlock(
		unsigned char* colorData,
		const int width,
		const int height,
		const int sourceImageWidth,
		const int sourceImageHeight,
		const COLOR_TYPE color);
	~ImageBlock();
	ImageBlock& operator=(ImageBlock& imageBlock);
	void fill();
	unsigned char* getColorData() const;
	unsigned char* getLine(const int lineIndex) const;
	void setTrimRect(const int x, const int y, const int w, const int h);

	unsigned char* m_colorData;
	const int m_width;
	const int m_height;
	const int m_sourceImageWidth;
	const int m_sourceImageHeight;
	const COLOR_TYPE m_color;
	int m_trimX;
	int m_trimY;
	int m_trimWidth;
	int m_trimHeight;
};

class ImageBlockArray
{
public:
	ImageBlockArray(
		unsigned char* colorData,
		const int arrayWidth,
		const int arrayHeight,
		const int blockWidth,
		const int blockHeight,
		const COLOR_TYPE color);
	~ImageBlockArray();
	ImageBlock& operator()(const int idx);
	const ImageBlock& operator()(const int idx) const;
	ImageBlock& operator()(const int x, const int y);
	const ImageBlock& operator()(const int x, const int y) const;
	void setTrimRect(const int x, const int y, const int w, const int h);
	bool exportAsTga(
		const char* filename,
		int startHeightIndex,
		int numBlocksVertical);

	unsigned char* m_colorData;
	ImageBlock* m_imageBlocks;
	const int m_arrayWidth;
	const int m_arrayHeight;
	const int m_blockWidth;
	const int m_blockHeight;
	const COLOR_TYPE m_color;
	const int width();
	const int height();
	const int getImageWidth();
	const int getImageHeight();
	const int getImageSize();
};

#endif