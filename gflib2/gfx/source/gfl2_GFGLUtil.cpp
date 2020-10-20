#include <gfx/include/gfl2_GFGLUtil.h>

namespace gfl2 { namespace gfx {

//  ディスプレイモード情報テーブル
const GFGLUtil::DisplayModeInfo GFGLUtil::s_displayModeInfoTable[ DisplayMode::NumberOf ] = 
{
    { 0,    0    }, //  Default = 0,        //!< 指定なし

#if defined( GFL2_X360 )
#if defined( GFL2_PAL )
	{ 640,  576  }, //  SDTV,               //!< XBOX360用640x480(NTSC) or 640x576(PAL) or 480p(HDMI)
#else
	{ 640,  480  }, //  SDTV,               //!< XBOX360用640x480(NTSC) or 640x576(PAL) or 480p(HDMI)
#endif
#else
#if defined( GFL2_PAL )
    { 720,  576  }, //  SDTV,               //!< 720x480(NTSC) or 720x576(PAL) or 480p(HDMI)
#else
    { 720,  480  }, //  SDTV,               //!< 720x480(NTSC) or 720x576(PAL) or 480p(HDMI)
#endif
#endif
    { 1280, 720  }, //  HD720,              //!< 720p  1280x720  D4
    { 1920, 1080 }, //  HD1080,             //!< 1080p 1920x1080 D5

    { 320,  240  }, //  QVGA,               //!< VGA   320x240
    { 320,  240  }, //  FullScreenQVGA,     //!< VGA   320x240(FullScreen)
    { 640,  480  }, //  VGA,                //!< VGA   640x480
    { 640,  480  }, //  FullScreenVGA,      //!< VGA   640x480(FullScreen)
    { 800,  600  }, //  SVGA,               //!< SVGA  800x600
    { 800,  600  }, //  FullScreenSVGA,     //!< SVGA  800x600(FullScreen)
    { 1024, 768  }, //  XGA,                //!< XGA   1024x768
    { 1024, 768  }, //  FullScreenXGA,      //!< XGA   1024x768(FullScreen)
    { 1280, 1024 }, //  SXGA,               //!< SXGA  1280x1024
    { 1280, 1024 }, //  FullScreenSXGA,     //!< SXGA  1280x1024(FullScreen)
    { 1600, 1200 }, //  UXGA,               //!< UXGA  1600x1200
    { 1600, 1200 }, //  FullScreenUXGA,     //!< UXGA  1600x1200(FullScreen)
    { 1920, 1200 }, //  WUXGA,              //!< WUXGA 1920x1200
    { 1920, 1200 }, //  FullScreenWUXGA,    //!< WUXGA 1920x1200(FullScreen)
    { 2048, 1536 }, //  QXGA,               //!< QXGA  2048x1536
    { 2048, 1536 }, //  FullScreenQXGA,     //!< QXGA  2048x1536(FullScreen)
    { 2560, 1600 }, //  WQXGA,              //!< WQXGA 2560x1600
    { 2560, 1600 }, //  FullScreenWQXGA,    //!< WQXGA 2560x1600(FullScreen)
    { 0, 0 },       //  UserSetting,        //!< UserSettingSize

    // CTR用に追加
    { 400, 240 },   //  CTR_LEFT,           //!< 3DSの左画面 400x240
    { 400, 240 },   //  CTR_RIGHT,          //!< 3DSの右画面 400x240
    { 320, 240 },   //  CTR_DOWN,           //!< 3DSの下画面 320x240
};

GFGLUtil::DisplayModeInfo GFGLUtil::s_displayModeInfoUserSetting = {0, 0};

//  フォーマット情報テーブル
const GFGLUtil::SurfaceFormatInfo GFGLUtil::s_formatInfoTable[ Format::NumberOf ] = {
    { "none",            0,  0,  0,  0,  0,  0,  0,  0, false, false, },     //Format::UNKNOWN = 0,    //  [surface]       [texture]

    //  surface
    { "X1R5G5B5",       16,  5,  5,  5,  1,  0,  0,  0, false, false, },     //Format::X1R5G5B5,       //  X1R5G5B5        X1R5G5B5
    { "R5G6B5",         16,  5,  6,  5,  0,  0,  0,  0, false, false, },     //Format::R5G6B5,         //  R5G6B5          R5G6B5
    { "X8R8G8B8",       32,  8,  8,  8,  0,  0,  0,  0, false, false, },     //Format::X8R8G8B8,       //  X8R8G8B8        X8R8G8B8
    { "A8R8G8B8",       32,  8,  8,  8,  8,  0,  0,  0, false, false, },     //Format::A8R8G8B8,       //  A8R8G8B8        A8R8G8B8        (displayable)
    { "B8",              8,  0,  0,  8,  0,  0,  0,  0, false, false, },     //Format::B8,             //  B8              B8
    { "G8B8",           16,  0,  8,  8,  0,  0,  0,  0, false, false, },     //Format::G8B8,           //  G8B8            G8B8
    { "A16B16G16R16F",  64, 16, 16, 16, 16,  0,  0,  0, true,  false, },     //Format::A16B16G16R16F,  //  A16B16G16R16F   A16B16G16R16F   (displayable)
    { "A32B32G32R32F", 128, 32, 32, 32, 32,  0,  0,  0, true,  false, },     //Format::A32B32G32R32F,  //  A32B32G32R32F   A32B32G32R32F
    { "R32F",           32, 32,  0,  0,  0,  0,  0,  0, true,  false, },     //Format::R32F,           //  R32F            R32F
    { "X8B8G8R8",       32,  8,  8,  8,  0,  0,  0,  0, false, false, },     //Format::X8B8G8R8,       //  X8B8G8R8        (X8R8G8B8 + remap)
    { "A8B8G8R8",       32,  8,  8,  8,  8,  0,  0,  0, false, false, },     //Format::A8B8G8R8,       //  A8B8G8R8        (A8R8G8B8 + remap)

    //  depth stencil
    { "D16",             0,  0,  0,  0,  0, 16, 16,  0, false, false, },     //Format::D16,            //  Z16             Z16
    { "D16F",            0,  0,  0,  0,  0, 16, 16,  0, false, false, },     //Format::D16F,           //  Z16F            Z16F
    { "D24X8",           0,  0,  0,  0,  0, 32, 24,  0, true,  false, },     //Format::D24X8,          //  Z24S8           Z24S8
    { "D24S8",           0,  0,  0,  0,  0, 32, 24,  8, true,  false, },     //Format::D24S8,          //  Z24S8           Z24S8
    { "D24S8F",          0,  0,  0,  0,  0, 32, 24,  8, true,  false, },     //Format::D24S8F,         //  Z24S8F          Z24S8F

    //  texture
    { "R6G5B5",         16,  5,  6,  5,  0,  0,  0,  0, false, false, },     //Format::R6G5B5,         //  (none)          R6G5B5
    { "G16R16F",        32,  0, 16, 16,  0,  0,  0,  0, true,  false, },     //Format::G16R16F,        //  (none)          G16R16F
    { "G16R16",         32,  0, 16, 16,  0,  0,  0,  0, false, false, },     //Format::G16R16,         //  (none)          G16R16
    { "R16",            16, 16,  0,  0,  0,  0,  0,  0, false, false, },     //Format::R16,            //  (none)          R16
    { "A1R5G5B5",       16,  5,  5,  5,  1,  0,  0,  0, false, false, },     //Format::A1R5G5B5,       //  (none)          A1R5G5B5
    { "A4R4G4B4",       16,  4,  4,  4,  4,  0,  0,  0, false, false, },     //Format::A4R4G4B4,       //  (none)          A4R4G4B4
    { "R5G5B5A1",       16,  5,  5,  5,  1,  0,  0,  0, false, false, },     //Format::R5G5B5A1,       //  (none)          R5G5B5A1
    { "DXT1",            4,  0,  0,  0,  0,  0,  0,  0, false, true,  },     //Format::DXT1,           //  (none)          DXT1
    { "DXT3",            8,  0,  0,  0,  0,  0,  0,  0, false, true,  },     //Format::DXT3,           //  (none)          DXT3
    { "DXT5",            8,  0,  0,  0,  0,  0,  0,  0, false, true,  },     //Format::DXT5,           //  (none)          DXT5

		{ "V8U8",           16,  0,  8,  8,  0,  0,  0,  0, false, false, },     //Format::V8U8,           //  V8U8            V8U8

		{ "X2R10G10B10",	  32, 10, 10, 10,  0,  0,  0,  0, false, false, },     //Format::X2R10G10B10,
		{ "A2R10G10B10",	  32, 10, 10, 10,  2,  0,  0,  0, false, false, },     //Format::A2R10G10B10,
    { "X8R8G8B8_LE",    32,  8,  8,  8,  0,  0,  0,  0, false, false, },	 //Format::X8R8G8B8_LE,
    { "A8R8G8B8_LE",    32,  8,  8,  8,  8,  0,  0,  0, false, false, },	 //Format::X8R8G8B8_LE,
		{ "X2R10G10B10_LE",	32, 10, 10, 10,  0,  0,  0,  0, false, false, },     //Format::X2R10G10B10_LE,
		{ "A2R10G10B10_LE",	32, 10, 10, 10,  2,  0,  0,  0, false, false, },     //Format::A2R10G10B10_LE,

    // CTR用に追加
    { "R8G8B8",         24,  8,  8,  8,  0,  0,  0,  0, false, false, },
    { "L8A8",           16,  8,  0,  0,  8,  0,  0,  0, false, false, },
    { "H8L8",           16,  8,  8,  0,  0,  0,  0,  0, false, false, },
    { "L8",             8,   8,  0,  0,  0,  0,  0,  0, false, false, },
    { "A8",             8,   0,  0,  0,  8,  0,  0,  0, false, false, },
    { "L4A4",           8,   4,  0,  0,  4,  0,  0,  0, false, false, },
    { "L4",             4,   4,  0,  0,  0,  0,  0,  0, false, false, },
    { "A4",             4,   0,  0,  0,  4,  0,  0,  0, false, false, },
    { "ETC1",           4,   0,  0,  0,  0,  0,  0,  0, false, true, },
    { "ETC1_A4",        4,   0,  0,  0,  0,  0,  0,  0, false, true, },
    { "SHADOW",         32,  0,  0,  0,  0,  0, 32,  0, false, false, },
    { "GAS",            32,  0,  0,  0,  0,  0, 32,  0, false, false, },
    { "REF",            32,  0,  0,  0,  0,  0,  0,  0, false, false, },
};

//----------------------------------------------------------------------------
/**
  *	@brief ディスプレイモードがフルスクリーンかどうか
	* @note プラットフォームがWindowsでなければ必ずtrueを返します
  */
//-----------------------------------------------------------------------------
b32 GFGLUtil::IsFullScreenDisplayMode( DisplayMode displayMode )
{
	switch( displayMode ){
	case DisplayMode::FullScreenQVGA:
	case DisplayMode::FullScreenVGA:
	case DisplayMode::FullScreenSVGA:
	case DisplayMode::FullScreenXGA:
	case DisplayMode::FullScreenSXGA:
	case DisplayMode::FullScreenUXGA:
	case DisplayMode::FullScreenWUXGA:
	case DisplayMode::FullScreenQXGA:
	case DisplayMode::FullScreenWQXGA:
		return true;
	default:
		return false;
	}
}

//----------------------------------------------------------------------------
/*
 *	@brief 各ディスプレイモードの解像度の幅を取得する
* @note プラットフォームがWindowsでなければ必ずtrueを返します
 */
//-----------------------------------------------------------------------------
u32 GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode displayMode )
{
  if( displayMode == DisplayMode::UserSetting )
  {
    return s_displayModeInfoUserSetting.width;
  }

	return s_displayModeInfoTable[ displayMode ].width;
}

//----------------------------------------------------------------------------
/*
 *	@brief 各ディスプレイモードの解像度の高さを取得する
 */
//-----------------------------------------------------------------------------
u32 GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode displayMode )
{
  if( displayMode == DisplayMode::UserSetting )
  {
    return s_displayModeInfoUserSetting.height;
  }

	return s_displayModeInfoTable[ displayMode ].height;
}

//----------------------------------------------------------------------------
/*
 *	@brief ユーザー設定用ディスプレイモードの解像度の幅を設定する
 */
//-----------------------------------------------------------------------------
void GFGLUtil::SetDisplayWidthForUserSetting( u32 size )
{
  s_displayModeInfoUserSetting.width = size;
}

//----------------------------------------------------------------------------
/*
 *	@brief ユーザー設定用ディスプレイモードの解像度の高さを取得する
 */
//-----------------------------------------------------------------------------
void GFGLUtil::SetDisplayHeightForUserSetting( u32 size )
{
  s_displayModeInfoUserSetting.height = size;
}

//----------------------------------------------------------------------------
/*
 *	@brief ピクセルフォーマットを文字列に変換する
 */
//-----------------------------------------------------------------------------
const char* GFGLUtil::GetFormatString( Format format )
{
    return s_formatInfoTable[ format ].name;
}

//----------------------------------------------------------------------------
/*
 *	@brief ピクセルフォーマットのビットサイズを取得する
 */
//-----------------------------------------------------------------------------
int GFGLUtil::GetPixelBitSizeByFormat( Format format )
{
    return s_formatInfoTable[ format ].color + s_formatInfoTable[ format ].depthStencil;
}

//----------------------------------------------------------------------------
/*
 *	@brief カラーフォーマットのビットサイズを取得する
 */
//-----------------------------------------------------------------------------
int GFGLUtil::GetColorBitSizeByFormat( Format format )
{
    return s_formatInfoTable[ format ].color;
}

//----------------------------------------------------------------------------
/*
 *	@brief フォーマットのＲ成分のビット数
 */
//-----------------------------------------------------------------------------
int GFGLUtil::GetRedBitSizeByFormat( Format format )
{
    return s_formatInfoTable[ format ].red;
}

//----------------------------------------------------------------------------
/*
 *	@brief フォーマットのＧ成分のビット数
 */
//-----------------------------------------------------------------------------
int GFGLUtil::GetGreenBitSizeByFormat( Format format )
{
    return s_formatInfoTable[ format ].green;
}

//----------------------------------------------------------------------------
/*
 *	@brief フォーマットのＢ成分のビット数
 */
//-----------------------------------------------------------------------------
int GFGLUtil::GetBlueBitSizeByFormat( Format format )
{
    return s_formatInfoTable[ format ].blue;
}

//----------------------------------------------------------------------------
/*
 *	@brief フォーマットのα成分のビット数
 */
//-----------------------------------------------------------------------------
int GFGLUtil::GetAlphaBitSizeByFormat( Format format )
{
    return s_formatInfoTable[ format ].alpha;
}

//----------------------------------------------------------------------------
/*
 *	@brief フォーマットのステンシル成分のビット数
 */
//-----------------------------------------------------------------------------
int GFGLUtil::GetDepthStencilBitSizeByFormat( Format format )
{
    return s_formatInfoTable[ format ].depthStencil;
}

//----------------------------------------------------------------------------
/*
 *	@brief フォーマットの深度成分のビット数
 */
//-----------------------------------------------------------------------------
int GFGLUtil::GetDepthBitSizeByFormat( Format format )
{
    return s_formatInfoTable[ format ].depth;
}

//----------------------------------------------------------------------------
/*
 *	@brief フォーマットのステンシル成分のビット数
 */
//-----------------------------------------------------------------------------
int GFGLUtil::GetStencilBitSizeByFormat( Format format )
{
    return s_formatInfoTable[ format ].stencil;
}

//----------------------------------------------------------------------------
/*
 *	@brief 圧縮テクスチャかどうか
 */
//-----------------------------------------------------------------------------
b32 GFGLUtil::IsFormatCompressed( Format format )
{
    return s_formatInfoTable[ format ].isCompressed;
}

//----------------------------------------------------------------------------
/*
 *	@brief ＦＰテクスチャかどうか
 */
//-----------------------------------------------------------------------------
b32 GFGLUtil::IsFormatFloatingPointer( Format format )
{
    return s_formatInfoTable[ format ].isFloat;
}

//----------------------------------------------------------------------------
/*
 *	@brief ビット幅を指定して、最適なサーフェイスフォーマットを取得する
 */
//-----------------------------------------------------------------------------
Format GFGLUtil::GetSuitedSurfaceFormat( u32 red, u32 green, u32 blue, u32 alpha, b32 fp )
{
    if( red == 8 && green == 8 && blue == 8 && alpha == 8 && !fp ){
        return Format::A8R8G8B8;
    }
    if( red == 8 && green == 8 && blue == 8 && alpha == 0 && !fp ){
        return Format::X8R8G8B8;
    }
    if( red == 16 && green == 16 && blue == 16 && alpha == 16 && fp ){
        return Format::A16B16G16R16F;
    }
    return Format::Unknown;     //  対応していないフォーマット
}

//----------------------------------------------------------------------------
/*
 *	@brief ビット幅を指定して、最適なテクスチャフォーマットを取得する
 */
//-----------------------------------------------------------------------------
Format GFGLUtil::GetSuitedTextureFormat( u32 red, u32 green, u32 blue, u32 alpha, b32 fp )
{
    return GetSuitedSurfaceFormat( red, green, blue, alpha, fp );//サーフェイスと同じ
}

//----------------------------------------------------------------------------
/*
 *	@brief ビット幅を指定して、最適な深度ステンシルフォーマットを取得する
 */
//-----------------------------------------------------------------------------
Format GFGLUtil::GetSuitedDepthStencilFormat( u32 depth, u32 stencil )
{
    if( depth == 24 && stencil == 8 ){
        return Format::D24S8;
    }
    if( depth == 24 && stencil == 0 ){
        return Format::D24X8;
    }
    if( depth == 16 && stencil == 0 ){
        return Format::D16;
    }
    return Format::Unknown;     //  対応していないフォーマット
}

//----------------------------------------------------------------------------
/*
 *	@brief 指定フォーマットでの画像のバッファサイズを求める
 */
//-----------------------------------------------------------------------------
u32 GFGLUtil::GetImageSizeByFormat( u32 width, u32 height, Format format )
{
    int imageSize = 0;

    if( GFGLUtil::IsFormatCompressed( format ) ) {
        const int blockCount = ((width + 3) / 4) * ((height + 3) / 4);
        if( format == Format::DXT1 ) {
            imageSize = blockCount * 8;       // DXT1
        } else {
            imageSize = blockCount * 16;      // DXT2 - 5
        }
    } else {
        imageSize = width * height * GFGLUtil::GetColorBitSizeByFormat( format )/8;
    }
    return imageSize;
}

}}
