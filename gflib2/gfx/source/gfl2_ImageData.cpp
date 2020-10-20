#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_ImageData.h>
#include <gfx/include/gfl2_GFGL.h>
#include "gfl2_DDSInformation.h"


namespace gfl2 { namespace gfx {

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
ImageData::ImageData(gfx::IGLAllocator* pAllocator) :
  m_pDataTop(NULL),
  m_Depth(0),
  m_Width(0),
  m_Height(0),
  m_Format(Format::None),
  m_IsCubemap(false),
  m_IsVolume(false),
  m_pInformation(NULL)
{
  if ( pAllocator )
		m_pTemporaryAllocator = pAllocator;
	else
		m_pTemporaryAllocator = this->GetInstanceAllocator();

#if defined(GF_PLATFORM_WIN32)
  m_pReadData = NULL;
  m_IsNw4cTga = false;
#endif
	for(s32 i = 0; i < MIP_VOLUME_MAX; ++i )
	{
		m_pImageData[i] = NULL;
		m_pAllocData[i] = NULL;		
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
ImageData::~ImageData()
{
	for(s32 i = 0; i < MIP_VOLUME_MAX; ++i )
	{
    m_pImageData[i] = NULL;

		if ( m_pAllocData[i] )
		{
			m_pTemporaryAllocator->SystemMemoryFree( m_pAllocData[i] );
			m_pAllocData[i] = NULL;
		}		
	}

#if defined(GF_PLATFORM_WIN32)
	if ( m_pInformation )
		delete m_pInformation;

	if ( m_pReadData )
		m_pTemporaryAllocator->SystemMemoryFree( m_pReadData );

#endif
}

#if defined(GF_PLATFORM_WIN32)
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
b32 ImageData::ReadFromFile( const char* path, u32 swapType, bool rawDataRequest)
{
	s32				size;

	m_pReadData = (u8*)GFGL::ReadFile(path, &size, 1, m_pTemporaryAllocator);
	m_pDataTop = m_pReadData;

  if ( m_pDataTop )
	  this->ReadFromMemory( reinterpret_cast<u8*>(m_pReadData), swapType, rawDataRequest );

  return m_pDataTop ? true : false;
}
#endif

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ImageData::ReadFromMemory( const u8* data, u32 swapType, bool rawDataRequest)
{
#if defined(GF_PLATFORM_WIN32)
	m_pDataTop = data;

	if (::strncmp((char*)data, "DDS ", 4) == 0)
	{
		readDDSData(data);
	}
	else
	{
		readTgaData(data, swapType, rawDataRequest);
	}
#elif defined(NN_PLATFORM_CTR)
  readTplData(data);
#endif	
}

#if defined(GF_PLATFORM_WIN32)
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
template<class T>
void ImageData::SwapData( const TargaHeader *pHeader, u32 swapType )
{
#if 0
	{
		m_pSwapData[0] = GLMemory::Malloc( m_Width * m_Height * sizeof(T) );

		u8				*pSwapBuffer = reinterpret_cast<u8*>(m_pSwapData[0]);
		u8				*pImage = reinterpret_cast<u8*>(m_pImageData[0]);
		T					*pSwapBufferT = reinterpret_cast<T*>(pSwapBuffer);
		T					*pImageT = reinterpret_cast<T*>(pImage);
		u32				pitchSize = m_Width * sizeof(T);

		if ( pHeader->turnOverU == 1 && pHeader->turnOverV == 0 )
		{
			u32				tailPos = (m_Width * m_Height) - 1;

			for( u32 pos = 0; pos < m_Width * m_Height; ++pos )
			{
				pSwapBufferT[pos] = pImageT[tailPos - pos];
			}
		}
		else if ( pHeader->turnOverU == 1 )
		{
			u32				tailPos = m_Width - 1;

			for( u32 v_pos = 0; v_pos < m_Height; ++v_pos )
			{
				for( u32 u_pos = 0; u_pos < m_Width; ++u_pos )
				{
					pSwapBufferT[u_pos] = pImageT[tailPos - u_pos];
				}

				pSwapBufferT += m_Width;
				pImageT += m_Width;
			}
		}
		else if ( pHeader->turnOverV == 0 )
		{
			pImage += pitchSize * (m_Height - 1);//末端行

			for( u32 h = 0; h < m_Height; ++h )
			{
				::memcpy( pSwapBuffer, pImage, pitchSize );
				pSwapBuffer += pitchSize;
				pImage -= pitchSize;
			}
		}
		m_pImageData[0] = m_pSwapData[0];
	}
#endif

	if ( swapType == SwapType::OpenGL )
	{//OpenGL
		T				*pImageT = reinterpret_cast<T*>(m_pImageData[0]);

		for( u32 x = 0; x < m_Width; ++x )
		{
			for( u32 y = 0; y < m_Height; ++y )
			{
				u8				*pImage = reinterpret_cast<u8*>(pImageT);
				u8				r = pImage[2];
				u8				b = pImage[0];

				pImage[0] = r;
				pImage[2] = b;

				++pImageT;
			}

		}
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void* ImageData::DecodeRLE( const void* pDataTop, const ImageData::TargaHeader* pHeader )
{
	static const u8				s_CheckBit = (1<<7);

	u32				pixelSize = pHeader->bpp / 8;
	u32				imageSize = m_Width * m_Height * pixelSize;

	u8*				pDecodeData = reinterpret_cast<u8*>( m_pTemporaryAllocator->SystemMemoryMalloc( imageSize ) );
	u32				readPos = 0;
	const u8*	pSeek = reinterpret_cast<const u8*>( pDataTop );

	while( readPos < imageSize )
	{
		u8			head = *pSeek;
		u8			length = (head & ~s_CheckBit) + 1;

		pSeek++;

		if ( head & s_CheckBit )
		{//圧縮
			for( u32 i = 0; i < length; ++i )
			{
			/// OPTIMIZED (REMOVED SLOW MEMCPY)
			///		memcpy( &pDecodeData[readPos], pSeek, pixelSize );
				
				switch(pixelSize)
				{
					case 4:
						{											
							u32 *ptr = (u32*)((u8*)(pDecodeData+readPos));
							u32 *ptrS = (u32*)pSeek;
							*ptr = *ptrS;
						}
						break;

					case 3:
						{				
							u16 *ptr = (u16*)((u8*)(pDecodeData+readPos));
							u16 *ptrS = (u16*)pSeek;
							*ptr = *ptrS;
							pDecodeData[readPos+2] = pSeek[2]; 
						}					
						break;
					case 2:
						{
							u16 *ptr = (u16*)((u8*)(pDecodeData+readPos));
							u16 *ptrS = (u16*)pSeek;
							*ptr = *ptrS;
						}					
						break;
					case 1:
						pDecodeData[readPos] = pSeek[0]; break;

					default:
						memcpy( &pDecodeData[readPos], pSeek, pixelSize ); break;
					
				}
				
				readPos += pixelSize;
			}
			pSeek += pixelSize;
		}
		else
		{
			memcpy( &pDecodeData[readPos], pSeek, length * pixelSize );
			pSeek += length * pixelSize;
			readPos += length * pixelSize;
		}
	}

	return pDecodeData;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ImageData::readTgaData(const void* buffer, u32 swapType, bool rawDataRequest)
{
	const TargaHeader* const		pHeader = reinterpret_cast< const TargaHeader* >( buffer );
	u32													isNw4c = true;
	
	{
		const char	*nw4cHeader = "NW4C_Tga Ver1.0";
		char*				nw4cCheck = (char*)(pHeader + 1);

		for( u32 i = 0; i < strlen(nw4cHeader); ++i )
		{
			if ( nw4cCheck[i] != nw4cHeader[i] )
			{
				isNw4c = false;
				break;
			}
		}
	}
#if defined(GF_PLATFORM_WIN32)
  m_IsNw4cTga = isNw4c;
#endif

	if( pHeader->bpp != 32 && pHeader->bpp != 24 )
		GFL_ASSERT( 0 );

	m_Width  = pHeader->width;
	m_Height = pHeader->height;

	if( pHeader->bpp == 32 )		m_Format = Format::A8R8G8B8;
	if( pHeader->bpp == 24 )		m_Format = Format::X8R8G8B8;

	m_Depth = 1;

	if ( isNw4c )
	{
    if (rawDataRequest)
    {
      // 生画像部分を使用する
      u8*					pSeek = (u8*)(pHeader + 1);
      pSeek += pHeader->idLength;
      m_pImageData[0] = (void*)(pSeek);
    }
    else
    {
      // 圧縮データ部分を使用する
      u8*					pSeek = (u8*)buffer;
      pSeek += sizeof(TargaHeader);
      pSeek += 16; // "NW4C_Tga Ver1.0"

      u32 nw4cOffset = *(u32*)pSeek;
      pSeek = (u8*)buffer;
      pSeek += nw4cOffset;

      this->readNw4cTgaData(pSeek);
      return;
    }
	}
	else
		m_pImageData[0] = (void*)(pHeader + 1);

	if ( pHeader->imageType == 0x0A )
	{
		m_pAllocData[0] = DecodeRLE( m_pImageData[0], pHeader );
		m_pImageData[0] = m_pAllocData[0];
	}
	
	if ( pHeader->turnOverU == 1 || pHeader->turnOverV == 0 )
	{//turnOverV == 0で反転するのは、DirectXＵＶにあわせるため。
		if( pHeader->bpp == 32 )
		{
			class rgba32{
			public:
				u8			r;
				u8			g;
				u8			b;
				u8			a;
			};

			this->SwapData<rgba32>( pHeader, swapType );
		}
		else if( pHeader->bpp == 24 )
		{
			class rgb24{
			public:
				u8			r;
				u8			g;
				u8			b;
			};

			this->SwapData<rgb24>( pHeader, swapType );
		}
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ImageData::readDDSData(const void* buffer)
{
	m_pInformation = new DDSInformation::Information();

	m_pInformation->SetSource( buffer );

	const u32			sourceFormat = static_cast<u32>(m_pInformation->GetFormat());
	const size_t	numberOfMipmap = m_pInformation->GetTexture().GetNumberOfMipmap();

	m_Width = static_cast<u32>(m_pInformation->GetWidth());
	m_Height = static_cast<u32>(m_pInformation->GetHeight());
	m_Depth = static_cast<u32>(numberOfMipmap);
	m_Format = GetDDSInternalFormat(sourceFormat);
	m_IsCubemap = m_pInformation->IsCubemap();
	m_IsVolume = m_pInformation->IsVolume();

	for( u32 i = 0; i < m_Depth; ++i )
	{
		m_pImageData[i] = m_pInformation->GetTexture().GetMipmap(i).GetPixel();
	}

}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
Format ImageData::GetDDSInternalFormat(u32 format)
{
	switch(format){
		case GL_BGRA:
			return Format::A8R8G8B8;
		case GL_BGR:
			return Format::X8R8G8B8;
		case GL_LUMINANCE:
			return Format::B8;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			return Format::DXT1;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			return Format::DXT3;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			return Format::DXT5;
		case GL_RGBA16:
			return Format::A4R4G4B4;
		case GL_LUMINANCE8_ALPHA8:
			return Format::V8U8;
		default:{
			GFL_ASSERT(false);
			return Format::Unknown;
		}
	}
}
#endif

#if defined(NN_PLATFORM_CTR)
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ImageData::readTplData(const void* buffer)
{
  s32 mipLevel = 0;
  u32 mipmapSize = 0;
  void* texBuf = nn::tpl::GetTexture(&mipLevel, &mipmapSize, buffer, 0);

  nn::tpl::CtrTextureInfo texInfo;
  nn::tpl::GetTextureInfo(&texInfo, buffer, 0);

  m_Width = texInfo.width;
  m_Height = texInfo.height;
  m_Depth = mipLevel;
  m_Format = GetTlpInternalFormat(texInfo.texFormat);
  m_IsCubemap = (texInfo.type == 0);
  m_IsVolume = false;

  m_pImageData[0] = texBuf;
}

Format ImageData::GetTlpInternalFormat(nn::tpl::CtrTexFormat format)
{
  switch (format)
  {
  case nn::tpl::TEXFMT_RGBA8888:        //!< R:8bit / G:8bit / B:8bit / A:8bit
    return Format::A8R8G8B8;
  case nn::tpl::TEXFMT_RGB888:          //!< R:8bit / G:8bit / B:8bit
    return Format::R8G8B8;
  case nn::tpl::TEXFMT_RGBA5551:        //!< R:5bit / G:5bit / B:5bit / A:1bit
    return Format::A1R5G5B5;
  case nn::tpl::TEXFMT_RGB565:          //!< R:5bit / G:6bit / B:5bit
    return Format::R5G6B5;
  case nn::tpl::TEXFMT_RGBA4444:        //!< R:4bit / G:4bit / B:4bit / A:4bit
    return Format::A4R4G4B4;
  case nn::tpl::TEXFMT_LA88:            //!< Luminance:8bit / Alpha:8bit
    return Format::L8A8;
  case nn::tpl::TEXFMT_HL8:             //!< R:8bit / G:8bit
    return Format::H8L8;
  case nn::tpl::TEXFMT_L8:              //!< Luminance:8bit
    return Format::L8;
  case nn::tpl::TEXFMT_A8:              //!< Alpha:8bit
    return Format::A8;
  case nn::tpl::TEXFMT_LA44:            //!< Luminance:4bit / Alpha:4bit
    return Format::L4A4;
  case nn::tpl::TEXFMT_L4:              //!< Luminance:4bit
    return Format::L4;
  case nn::tpl::TEXFMT_A4:              //!< Alpha:4bit
    return Format::A4;
  case nn::tpl::TEXFMT_ETC1:            //!< ETC1圧縮フォーマット
    return Format::ETC1;
  case nn::tpl::TEXFMT_ETC1_A4:         //!< Alpha付きETC1圧縮フォーマット
    return Format::ETC1_A4;

  case nn::tpl::TEXFMT_SHADOW:          //!< shadowテクスチャ(unsigned int)
    return Format::SHADOW;
  case nn::tpl::TEXFMT_GAS:             //!< gas用テクスチャ(unsigned short)
    return Format::GAS;
  case nn::tpl::TEXFMT_REF:             //!< 1次元テクスチャ(float)
    return Format::REF;
  }

  return Format::Unknown;
}

#endif


}}

