#include "ImageBlock.h"
#include <iostream>
#include <new>

ImageBlock::ImageBlock(
	unsigned char* colorData,
	const int width,
	const int height,
	const int sourceImageWidth,
	const int sourceImageHeight,
	const COLOR_TYPE color) : 
	m_colorData(colorData),
	m_width(width),
	m_height(height),
	m_sourceImageWidth(sourceImageWidth),
	m_sourceImageHeight(m_sourceImageHeight),
	m_color(color),
	m_trimX(0),
	m_trimY(0),
	m_trimWidth(width),
	m_trimHeight(height)
{
}

ImageBlock::~ImageBlock()
{
}

ImageBlockArray::ImageBlockArray(
	unsigned char* colorData,
	const int arrayWidth,
	const int arrayHeight,
	const int blockWidth,
	const int blockHeight,
	const COLOR_TYPE color) :
	m_colorData(colorData),
	m_arrayWidth(arrayWidth),
	m_arrayHeight(arrayHeight),
	m_blockWidth(blockWidth),
	m_blockHeight(blockHeight),
	m_color(color)
{
	// 指定された分だけ色情報の配列を作る
	const int imageWidth = arrayWidth * blockWidth;
	const int imageHeight = arrayHeight * blockHeight;
	m_imageBlocks = (ImageBlock*)calloc(arrayWidth * arrayHeight, sizeof(ImageBlock));
	for(int h = 0; h < arrayHeight; ++h)
	{
		for(int w = 0; w < arrayWidth; ++w)
		{
			unsigned char* idx = 
				&m_colorData[color * blockWidth * (w + blockHeight * h * arrayWidth)];
			// placement newを使う
			new(m_imageBlocks + w + h * arrayWidth) 
				ImageBlock(idx, blockWidth, blockHeight, imageWidth, imageHeight, color);
		}
	}
}

ImageBlockArray::~ImageBlockArray()
{
	free(m_imageBlocks);
}

ImageBlock& ImageBlockArray::operator()(const int idx)
{
	return m_imageBlocks[idx];
}

const ImageBlock& ImageBlockArray::operator()(const int idx) const
{
	return m_imageBlocks[idx];
}

ImageBlock& ImageBlockArray::operator()(const int x, const int y)
{
	return m_imageBlocks[x + y * m_arrayWidth];
}

const ImageBlock& ImageBlockArray::operator()(const int x, const int y) const
{
	return m_imageBlocks[x + y * m_arrayWidth];
}

ImageBlock& ImageBlock::operator=(ImageBlock& imageBlock)
{
	for(int h = 0; h < m_trimHeight; ++h)
	{
		unsigned char* dst = this->getLine(h);
		unsigned char* src = imageBlock.getLine(h);
		memcpy(	this->getLine(h),
				imageBlock.getLine(h),
				m_color * m_trimWidth);
	}
	return (*this);
}

void ImageBlock::fill()
{
	for(int h = 0; h < m_height; ++h)
	{
		// ブロックの領域を全て白で塗りつぶす
		memset(	getLine(h),
				0xFFFFFFFF,
				m_color * m_width);
	}
}

unsigned char* ImageBlock::getColorData() const
{
	return m_colorData;
}

unsigned char* ImageBlock::getLine(const int lineIndex) const
{
	return &m_colorData[m_color * (m_trimX + m_sourceImageWidth * (lineIndex + m_trimY))];
}

void ImageBlock::setTrimRect(const int x, const int y, const int w, const int h)
{
	m_trimX = x;
	m_trimY = y;
	m_trimWidth = w;
	m_trimHeight = h;
}

const int ImageBlockArray::width()
{
	return m_arrayWidth;
}

const int ImageBlockArray::height()
{
	return m_arrayHeight;
}

const int ImageBlockArray::getImageWidth()
{
	return m_arrayWidth * m_blockWidth;
}

const int ImageBlockArray::getImageHeight()
{
	return m_arrayHeight * m_blockHeight;
}

const int ImageBlockArray::getImageSize()
{
	return m_arrayWidth * m_arrayHeight * m_blockWidth * m_blockHeight * m_color;
}

void ImageBlockArray::setTrimRect(const int x, const int y, const int w, const int h)
{
	for(int height = 0; height < m_arrayHeight; ++height)
	{
		for(int width = 0; width < m_arrayWidth; ++width)
		{
			m_imageBlocks[width + height * m_arrayWidth].setTrimRect(x, y, w, h);
		}
	}
}

bool ImageBlockArray::exportAsTga(
	const char* filename,
	int startHeightIndex,
	int numBlocksVertical)
{
	TGA_HEADER tgaHeader;
	TGA_FOOTER tgaFooter;
	unsigned char sig[] = {0x54, 0x52, 0x55, 0x45, 0x56, 0x49, 0x53, 0x49,
						   0x4F, 0x4E, 0x2D, 0x58, 0x46, 0x49, 0x4C, 0x45,
						   0x2E, 0x00};
	FILE* pFile;
	fopen_s(&pFile, filename, "wb");
	if(!pFile)
	{
		return false;
	}
	tgaHeader.idFieldLength = 0;
	tgaHeader.colorMapType = 0;
	tgaHeader.imageType = 0x02;
	tgaHeader.colorMapIndex[0] = 0;
	tgaHeader.colorMapIndex[1] = 0;
	tgaHeader.colorMapLength[0] = 0;
	tgaHeader.colorMapLength[1] = 0;
	tgaHeader.colorMapSize = 0;
	tgaHeader.imageOriginX = 0;
	tgaHeader.imageOriginY = m_blockHeight * numBlocksVertical;
	tgaHeader.imageWidth = getImageWidth();
	tgaHeader.imageHeight = m_blockHeight * numBlocksVertical;
	tgaHeader.bitPerPixel = 0x20;
	tgaHeader.discriptor = 0x28;

	tgaFooter.developerAreaOffset = 0x00000000;
	tgaFooter.extensionOffset = 0x00000000;
	memcpy(tgaFooter.signature, sig, sizeof(sig));

	unsigned char* idx = 
			&m_colorData[m_color * m_blockWidth * (m_blockHeight * startHeightIndex * m_arrayWidth)];
	size_t size = m_color * tgaHeader.imageWidth * tgaHeader.imageHeight;
	fwrite(&tgaHeader, sizeof(TGA_HEADER), 1, pFile);
	fwrite(idx, size , 1, pFile);
	fwrite(&tgaFooter, sizeof(TGA_FOOTER), 1, pFile);
	fclose(pFile);

	return true;
}