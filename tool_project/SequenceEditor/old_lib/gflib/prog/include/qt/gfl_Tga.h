//==============================================================================
/**
 * @file	gfl_Tga.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/03/14, 15:33
 */
// =============================================================================

#if !defined( __GFL_TGA_H__ )
#define	__GFL_TGA_H__

#include <qt/gfl_QtGui.h>
#include <qt/gfl_QtFile.h>

namespace gfl {
namespace qt {

#pragma pack (push,1)

class Tga {
public:

	enum {
		IT_NONE,
		IT_INDEX_COLOR,
		IT_FULL_COLOR,
		IT_BLACK_AND_WHITE,
	};

	class Header {
	public:

		Header( s32 w = 0, s32 h = 0, s32 s = 32 ) :
		m_NoID( 0 ),
		m_CMapType( 0 ),
		m_ImageType( IT_FULL_COLOR ),
		m_Ox( 0 ),
		m_Oy( 0 ),
		m_Width( w ),
		m_Height( h ),
		m_PixelSize( s ),
		m_IDescType( 0x0 ) // (0x20)５ビット目は上から下
		{
			for( s32 i = 0; i < 5; ++i ){
				m_CMapSpec[i] = 0;
			}
		}

		u8 GetPixelSize( ) const {
			return m_PixelSize;
		}

		s16 GetHeight( ) const {
			return m_Height;
		}

		s16 GetWidth( ) const {
			return m_Width;
		}
	private:
		u8 m_NoID; /* 0=イメージＩＤ無し*/
		u8 m_CMapType; /* 0=カラーマップ無し */
		u8 m_ImageType; /* 2=無圧縮RGBタイプ 10=圧縮RGBタイプ */
		u8 m_CMapSpec[5]; /* スキップされる*/

		s16 m_Ox; /* 画面 左上 X 0固定 */
		s16 m_Oy; /* 画面 左上 Y 0固定 */

		s16 m_Width; /* 幅 (dot)*/
		s16 m_Height; /* 高さ (dot)*/

		u8 m_PixelSize; /* Pixel Size. 32=32bit */
		u8 m_IDescType; /* 8=TGA32 アトリビュート */

		friend class Tga;
	};

	Tga( ) : m_pImage( 0 ) {
	}

	~Tga( ) {
		delete [] m_pImage;
		m_pImage = 0;
	}

	c8* GetImage( void ) {
		return m_pImage;
	}

	const c8* const GetImage( void ) const {
		return m_pImage;
	}

	bool Load( const QString& fname ) {
		QByteArray dat( gfl::qt::File::Load( fname, QIODevice::NotOpen ) );
		if( dat.size( ) ){
			memcpy( &m_Header, dat.data( ), sizeof (Header) );

			if( m_Header.m_ImageType == IT_FULL_COLOR || m_Header.m_ImageType == IT_BLACK_AND_WHITE ){
				const size_t img_size = m_Header.m_Width * m_Header.m_Height * (m_Header.m_PixelSize >> 3);
				c8* img = DbgNew c8 [img_size];
				memcpy( img, &dat.data( )[sizeof (Header) + m_Header.m_NoID], img_size );
				m_pImage = img;
			} else { // 圧縮テクスチャー未対応
				return false;
			}
			return true;
		}
		return false;
	}

	bool Save( const QString& fname ) {
		QByteArray dat;
		m_Header.m_NoID = 0;
		dat.append( (c8*) & m_Header, sizeof (Header) );
		const s32 img_size = m_Header.m_Width * m_Header.m_Height * (m_Header.m_PixelSize >> 3);
		dat.append( m_pImage, img_size );
		return gfl::qt::File::Save( fname, dat, QIODevice::NotOpen );
	}

	void SetHeader( const s32 w, const s32 h, const s32 s = 32 ) {
		m_Header.m_Width = w;
		m_Header.m_Height = h;
		m_Header.m_PixelSize = s;
	}

	static bool Save( const QString& fname, const c8 * const img, s32 w, s32 h, s32 s = 32, s32 type = IT_FULL_COLOR ) {
		QByteArray dat;
		Header head;
		head.m_Width = w;
		head.m_Height = h;
		head.m_PixelSize = s;
		head.m_ImageType = type;
		dat.append( (c8*) & head, sizeof (Header) );
		const s32 img_size = w * h * (s >> 3);
		dat.append( img, img_size );
		return gfl::qt::File::Save( fname, dat, QIODevice::NotOpen );
	}

	const Header& GetHeader( ) const {
		return m_Header;
	}

	s32 GetHeight( ) const {
		return GetHeader( ).GetHeight( );
	}

	s32 GetWidth( ) const {
		return GetHeader( ).GetWidth( );
	}

	void SetImage( c8* Image ) {
		this->m_pImage = Image;
	}
private:
	c8* m_pImage; /* image data offset */
	Header m_Header;
};
#pragma pack (pop)

}
}

#endif	/* __GFL_TGA_H__ */

