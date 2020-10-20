//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//
#if defined( GF_PLATFORM_CTR )

#include <string.h>

#include <qr/source/ctr/gfl2_CTR_TextureConverter.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)

/*
     *  Definitions
     */    
    const u32 TEXTURE_BLOCK_SIZE = 8;
    const u32 TEXTURE_PIXELS_IN_BLOCK = TEXTURE_BLOCK_SIZE * TEXTURE_BLOCK_SIZE;
    #define SWAP(A, B)		tmp = (A); (A) = (B); (B) = tmp

    /*
     *  Type definitions
     */

    enum TextureMode
    {
	    TEXTURE_LINE_TO_BLOCK_MODE = 0,
	    TEXTURE_BLOCK_TO_LINE_MODE = 1
    };

    typedef struct
    {	    
        GLenum format;        
	    s32 blocksInRow;
	    s32	blocksInCol;
	    u8*	src;
	    u8*	dst;
	    u32	width;
	    u32	height;
	    u32	yflip;
	    u32	mode;
    } TextureInformation;

    typedef s32 (*textureConvertFunction)(TextureInformation* textureInformation);

    /*
     *  Local Function Declaration
     */
    static s32 GetPixelByte(const GLenum format);

    static s32 ReadBlock(u32 no, u8* buf, TextureInformation* textureInformation);
    static s32 WriteBlock(u32 no, const u8* pSrc, TextureInformation* textureInformation);
    static s32 DeBlock(const u8* pSrc, u8* pDst, s32 blocksize, TextureInformation* textureInformation);
    static s32 EnBlock(const u8* pSrc, u8* pDst, s32 blocksize, TextureInformation* textureInformation);
    static s32 SwapColorComponents(TextureInformation* textureInformation);

    static s32 ConvertFormat8888Function(TextureInformation* textureInformation);
    static s32 ConvertFormat888Function(TextureInformation* textureInformation);
    static s32 ConvertFormat88Function(TextureInformation* textureInformation);
    static s32 ConvertFormatXXXXFunction(TextureInformation* textureInformation);
        
    static textureConvertFunction convertFunctionArray[] =
    {
	    ConvertFormat8888Function,  /* GL_RGBA_NATIVE_DMP R8G8B8A8 */
	    ConvertFormat888Function,   /* GL_RGB_NATIVE_DMP R8G8B8 */
        ConvertFormat88Function,    /* GL_LUMINANCE_ALPHA_NATIVE_DMP L8A8 */
        ConvertFormatXXXXFunction,  /* else     */
	    ConvertFormatXXXXFunction,  /* R5G6B5   */
	    ConvertFormatXXXXFunction,  /* R5G5B5A1 */
	    ConvertFormatXXXXFunction,  /* R4G4B4A4 */	    
	    ConvertFormatXXXXFunction,  /* L8       */
	    ConvertFormatXXXXFunction   /* A8       */
    };

    /*
     *	Export functions
     */

    /* Block <--> Linear conversion */
    bool ConvertGLTextureToNative(const GLenum format, const u32 width, const u32 height,
        void* glData, void* dmpData)
    {
        bool yflip = true;
        TextureMode mode = TEXTURE_LINE_TO_BLOCK_MODE;

	    u8 blockBuf[TEXTURE_PIXELS_IN_BLOCK * 4];
	    TextureInformation textureInformation;

	    if (width & 0x00000003 || height & 0x00000003)
	    {
		    return false;
	    }

	    textureInformation.format = format;
	    textureInformation.width = width;
	    textureInformation.height = height;
	    textureInformation.blocksInRow = (textureInformation.width / TEXTURE_BLOCK_SIZE);
	    textureInformation.blocksInCol = (textureInformation.height / TEXTURE_BLOCK_SIZE);
	    textureInformation.src = (u8*)glData;
	    textureInformation.dst = (u8*)dmpData;
        if ( yflip )
        {
	        textureInformation.yflip = 1;
        }
        else
        {
            textureInformation.yflip = 0;
        }
        textureInformation.mode = mode;

	    /* Convert color format */
	    SwapColorComponents(&textureInformation);
	    /* for each row */
	    for (s32 row = 0; row < textureInformation.blocksInCol; row++)
	    {
		    /* for each column */
		    for (s32 col = 0; col < textureInformation.blocksInRow; col++)
		    {
			    /* Read one unit from source buffer. Read in source addressing and color format */
			    ReadBlock(row * textureInformation.blocksInRow + col, blockBuf, &textureInformation);
			    /* Write one unit to destination buffer. written with converting addressing format */
			    WriteBlock(row * textureInformation.blocksInRow + col, blockBuf, &textureInformation);
		    }
	    }

	    return true;
    }

    /*
     *	Local functions
     */
    static s32 GetPixelByte(const GLenum format)
    {
        /* GL_RGBA_NATIVE_DMP R8G8B8A8 */
        if ( format == GL_RGBA_NATIVE_DMP )
        {
            return 4;
        }
        /* GL_RGB_NATIVE_DMP R8G8B8 */
        else if ( format == GL_RGB_NATIVE_DMP )
        {
	        return 3;
        }
        /* GL_LUMINANCE_ALPHA_NATIVE_DMP L8A8 */
        else if ( format == GL_LUMINANCE_ALPHA_NATIVE_DMP )
        {
            return 2;
        }
        else
        {
            NN_TLOG_("Unsupported texture format.");
            NN_TASSERT_(0);

            return 1;
        }
    }

    static s32 ReadBlock(u32 no, u8* buf, TextureInformation* textureInformation)
    {
        u32 row = (no / textureInformation->blocksInRow);
        u32 column = (no % textureInformation->blocksInRow);
        u32 pixelByte = GetPixelByte(textureInformation->format);

	    /* Read src us32 */
	    if (textureInformation->mode == TEXTURE_BLOCK_TO_LINE_MODE)
	    {
		    /* Get source buffer pointer */
		    const u8* pSrc = textureInformation->src + 
                no * pixelByte * TEXTURE_PIXELS_IN_BLOCK;

		    /* read uint image from block format buffer */
		    memcpy(buf, pSrc, (TEXTURE_PIXELS_IN_BLOCK * pixelByte) );
	    }
	    else // if (mode == L2B)
	    {
		    /* Get source buffer pointer */
		    const u8* pSrc = textureInformation->src + 
                (row * TEXTURE_PIXELS_IN_BLOCK * pixelByte * textureInformation->blocksInRow) + 
                (column * pixelByte * TEXTURE_BLOCK_SIZE);

		    /* read uint image from linear format buffer */
		    for (s32 i = 0; i < TEXTURE_BLOCK_SIZE; i++)
		    {
			    memcpy(buf, pSrc, pixelByte * TEXTURE_BLOCK_SIZE);
			    buf += pixelByte * TEXTURE_BLOCK_SIZE;
			    pSrc += pixelByte * textureInformation->width;
		    }
        }
        
        return 1;
    }

    static s32 WriteBlock(u32 no, const u8* pSrc, TextureInformation* textureInformation)
    {
        u32 row = (no / textureInformation->blocksInRow);
        u32 column = (no % textureInformation->blocksInRow);
        u32 pixelByte = GetPixelByte(textureInformation->format);

	    /* Read src uint */
	    if (textureInformation->mode == TEXTURE_BLOCK_TO_LINE_MODE)
	    {
		    /* Get destination buffer pointer */
		    u8*	pDst;

		    /* Get destination pointer */
		    if (textureInformation->yflip)
            {
			    pDst = textureInformation->dst + 
                    ((textureInformation->blocksInCol - row - 1) * TEXTURE_PIXELS_IN_BLOCK * pixelByte * 
                    textureInformation->blocksInRow) + column * pixelByte * TEXTURE_BLOCK_SIZE;
            }
		    else
            {
			    pDst = textureInformation->dst + 
                    (row * TEXTURE_PIXELS_IN_BLOCK * pixelByte * textureInformation->blocksInRow) + 
                    column * pixelByte * TEXTURE_BLOCK_SIZE;
            }

		    /* write uint image to linear format buffer */
		    DeBlock(pSrc, pDst, TEXTURE_BLOCK_SIZE, textureInformation);
	    }
	    else // if (mode == L2B)
	    {
		    /* Get destination buffer pointer */
		    u8* pDst;

		    if (textureInformation->yflip)
            {
			    pDst = textureInformation->dst + 
                    ((textureInformation->blocksInCol - row - 1) * textureInformation->blocksInRow + column) * 
                    TEXTURE_PIXELS_IN_BLOCK * pixelByte;
            }
		    else
            {
			    pDst = textureInformation->dst + 
                    no * TEXTURE_PIXELS_IN_BLOCK * pixelByte;
            }

		    /* write uint image to block format buffer */
		    EnBlock(pSrc, pDst, TEXTURE_BLOCK_SIZE, textureInformation);
	    }
	    
        return 1;
    }

    static s32 DeBlock(const u8* pSrc, u8* pDst, s32 blocksize, TextureInformation* textureInformation)
    {
        u32 pixelByte = GetPixelByte(textureInformation->format);

	    if (blocksize != 1)
	    {
		    s32 pixels;
		    blocksize >>= 1;
		    pixels = blocksize * blocksize;            
		    //s32 dstWidth = textureInformation->width;
            
		    if (!textureInformation->yflip)
		    {
			    DeBlock(pSrc + (pixelByte * pixels * 0), 
                    pDst, blocksize, textureInformation);
			    
                DeBlock(pSrc + (pixelByte * pixels * 1), 
                    pDst + (pixelByte * blocksize),
                    blocksize, textureInformation);

			    DeBlock(pSrc + (pixelByte * pixels * 2),
                    pDst + (pixelByte * textureInformation->width * blocksize),
                    blocksize, textureInformation);
			    
                DeBlock(pSrc + (pixelByte * pixels * 3), 
                    pDst + (pixelByte * (blocksize + (textureInformation->width * blocksize))), 
                    blocksize, textureInformation);
		    }
		    else
		    {
			    DeBlock(pSrc + (pixelByte * pixels * 0), 
                    pDst + (pixelByte * textureInformation->width*blocksize),
                    blocksize, textureInformation);

			    DeBlock(pSrc + (pixelByte * pixels * 1),
                    pDst + (pixelByte * (blocksize + (textureInformation->width * blocksize))), 
                    blocksize, textureInformation);

			    DeBlock(pSrc + (pixelByte * pixels * 2), pDst,       
                    blocksize, textureInformation);

			    DeBlock(pSrc + (pixelByte * pixels * 3), 
                    pDst+(pixelByte * blocksize),
                    blocksize, textureInformation);
		    }
	    }
	    else
		    memcpy((void*)pDst, pSrc, pixelByte);

	    return 1;
    }

    static s32 EnBlock(const u8* pSrc, u8* pDst, s32 blocksize, TextureInformation* textureInformation)
    {    
        u32 pixelByte = GetPixelByte(textureInformation->format);

	    if (blocksize != 1)
	    {            
		    s32 pixels;
		    blocksize >>= 1;
		    pixels = blocksize * blocksize;

		    if (! textureInformation->yflip )
		    {
			    EnBlock(pSrc,
                    pDst + (pixelByte * pixels * 0), 
                    blocksize, textureInformation);

			    EnBlock(pSrc + (pixelByte * blocksize),                  
                    pDst + (pixelByte * pixels * 1),
                    blocksize, textureInformation);

			    EnBlock(pSrc + (pixelByte * blocksize * TEXTURE_BLOCK_SIZE),
                    pDst + (pixelByte * pixels * 2),
                    blocksize, textureInformation);

			    EnBlock(pSrc + (pixelByte * blocksize * (TEXTURE_BLOCK_SIZE + 1)),
                    pDst + (pixelByte * pixels * 3),
                    blocksize, textureInformation);
		    }
		    else
		    {
			    EnBlock(pSrc,
                    pDst + (pixelByte * pixels * 2),
                    blocksize, textureInformation);

			    EnBlock(pSrc + (pixelByte * blocksize),
                    pDst + (pixelByte * pixels * 3),
                    blocksize, textureInformation);

			    EnBlock(pSrc + (pixelByte * blocksize * TEXTURE_BLOCK_SIZE),
                    pDst + (pixelByte * pixels * 0),                    
                    blocksize, textureInformation);

			    EnBlock(pSrc + (pixelByte * blocksize * (TEXTURE_BLOCK_SIZE + 1)),
                    pDst + (pixelByte * pixels * 1),
                    blocksize, textureInformation);
		    }

	    }
	    else
        {
		    memcpy((void*)pDst, pSrc, pixelByte);
        }

	    return 1;
    }

    static s32 SwapColorComponents(TextureInformation* textureInformation)
    {
        if ( textureInformation->format == GL_RGBA_NATIVE_DMP )
        {
	        return convertFunctionArray[0](textureInformation);
        }
        else if ( textureInformation->format == GL_RGB_NATIVE_DMP )
        {
            return convertFunctionArray[1](textureInformation);
        }
        else if ( textureInformation->format == GL_LUMINANCE_ALPHA_NATIVE_DMP )
        {
            return convertFunctionArray[2](textureInformation);
        }
        else
        {
            return convertFunctionArray[3](textureInformation);
        }
    }

    static s32 ConvertFormat8888Function(TextureInformation* textureInformation)
    {
    	u8 tmp;
        u32 pixelByte = GetPixelByte(textureInformation->format);
        s32 total = static_cast<s32>(textureInformation->width * textureInformation->height * pixelByte);

	    for (s32 i = 0; i< total; i+= pixelByte)
	    {
		    SWAP(textureInformation->src[i + 0], textureInformation->src[i + 3]);
		    SWAP(textureInformation->src[i + 1], textureInformation->src[i + 2]);
	    }

	    return 1;
    }

    static s32 ConvertFormat888Function(TextureInformation* textureInformation)
    {
	    u8 tmp;
        u32 pixelByte = GetPixelByte(textureInformation->format);
        s32 total = static_cast<s32>(textureInformation->width * textureInformation->height * pixelByte);

	    for (s32 i = 0; i < total; i += pixelByte)
	    {
		    SWAP(textureInformation->src[i + 0], textureInformation->src[i + 2]);
        }

	    return 1;
    }

    static s32 ConvertFormat88Function(TextureInformation* textureInformation)
    {
	    u8 tmp;
        u32 pixelByte = GetPixelByte(textureInformation->format);
        s32 total = static_cast<s32>(textureInformation->width * textureInformation->height * pixelByte);
        
	    for (s32 i = 0; i < total; i += pixelByte)
	    {
		    SWAP(textureInformation->src[i+0], textureInformation->src[i+1]);
	    }

	    return 1;
    }

    static s32 ConvertFormatXXXXFunction(TextureInformation* textureInformation)
    {
        (void)textureInformation;    
        NN_TLOG_("Unsupported texture format.");
        NN_TASSERT_(0);

	    return 1;
    }

GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // defined( GF_PLATFORM_CTR )
