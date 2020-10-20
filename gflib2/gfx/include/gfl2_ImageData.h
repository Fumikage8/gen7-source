#ifndef GFLIB2_GRX_IMAGE_DATA_H_INCLUDED
#define GFLIB2_GRX_IMAGE_DATA_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GLMemory.h>

#if defined(NN_PLATFORM_CTR)
#include <nn/tpl.h>
#endif

namespace gfl2 { namespace gfx {

	namespace DDSInformation
	{
		class Information;
	}

class ImageData : public GLMemory
{
public:

	class TargaHeader{
	public:
		u8  idLength;            //  0: 識別子文字列長
		u8  colorMapType;        //  1:
		u8  imageType;           //  2: 0x02 = 無圧縮ＲＧＢタイプ / 0x0A = 圧縮ＲＧＢタイプ
		u8  colorMapOrigin[ 2 ]; //  3:
		u8  colorMapLength[ 2 ]; //  5:
		u8  colorMapEntrySize;   //  7: パレットのビット幅
		u16 xOrigin;             //  8:
		u16 yOrigin;             // 10:
		u16 width;               // 12: 画像の横幅
		u16 height;              // 14: 画像の縦幅
		u8  bpp;                 // 16: ビット深度
		union{
			struct{
				u8 attribute;
			};
#if defined( GFL2_LITTLE_ENDIAN )
			struct{
				u8 a        :4;
				u8 turnOverU:1;//格納方向（0=左から右、1=右から左）
				u8 turnOverV:1;//格納方向（0=下から上、1=上から下）
				u8 c        :2;
			};
#else
			struct{
				u8 c        :2;
				u8 turnOverV:1;//格納方向（0=下から上、1=上から下）
				u8 turnOverU:1;//格納方向（0=左から右、1=右から左）
				u8 a        :4;
			};
#endif
		};
	};

	class SwapType{
	public:
		enum {
			None = 0,
			OpenGL,
			CTR,
		};

	};

	ImageData( gfx::IGLAllocator* pAllocator = NULL );
	virtual ~ImageData();

#if defined(GF_PLATFORM_WIN32)
	b32 ReadFromFile( const char* path, u32 swapType = SwapType::None, bool rawDataRequest = false );  // NW4C_TGAの生データが欲しいならrawDataRequestをtrueに
#endif
	void ReadFromMemory( const u8* data, u32 swapType = SwapType::None, bool rawDataRequest = false );

	const void* GetDataTop() const { return m_pDataTop; }
	const void* GetImage( u32 depth = 0 ) const { return m_pImageData[depth]; }

	const u32 GetWidth() const { return m_Width; }
	const u32 GetHeight() const { return m_Height; }
	const u32 GetDepth() const { return m_Depth; }
	const Format GetFormat() const { return m_Format; }

	const b32 IsCubemap() const { return m_IsCubemap; }
	const b32 IsVolume() const { return m_IsVolume; }

protected:

#if defined(GF_PLATFORM_WIN32)
	void readTgaData(const void* buffer, u32 swapType = SwapType::None, bool rawDataRequest = false );
	void readNw4cTgaData(const void* buffer);
	void readDDSData(const void* buffer);

	void* DecodeRLE( const void* pDataTop, const TargaHeader* pHeader );
	template<class T>
	void SwapData( const TargaHeader *pHeader, u32 swapType = SwapType::None );

	static Format GetDDSInternalFormat(u32 format);
#elif defined(NN_PLATFORM_CTR)
  void readTplData(const void* buffer);
  static Format GetTlpInternalFormat(nn::tpl::CtrTexFormat format);
#endif


	static const s32				MIP_VOLUME_MAX = 32;//ボリュームテクスチャの最大値はあとで調べておこう。

	const u8*					m_pDataTop;
	void*							m_pImageData[MIP_VOLUME_MAX];
	void*							m_pAllocData[MIP_VOLUME_MAX];
	
	u32								m_Depth;
	u32								m_Width;
	u32								m_Height;
	Format						m_Format;
	b32								m_IsCubemap;
	b32								m_IsVolume;

	DDSInformation::Information		*m_pInformation;
  gfx::IGLAllocator		*m_pTemporaryAllocator;

#if defined(GF_PLATFORM_WIN32)
  b32               m_IsNw4cTga;
  u8*								m_pReadData;
#endif
};

}}

#endif
