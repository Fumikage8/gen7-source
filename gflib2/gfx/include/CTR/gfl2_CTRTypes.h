#ifndef GFLIB2_GRX_CTR_CTRTYPES_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRTYPES_H_INCLUDED

#include <gfx/include/gfl2_types.h>
#include <util/include/gfl2_AutoComPtr.h>

#include <nn/gx.h>
#include <nn/gr.h>

namespace gfl2 { namespace gfx { namespace ctr {

	//-------------------------------------------------------------------------------------------------------------------------------------------------
  //! @brief テクスチャフォーマットの変換テーブル
  static const u8 s_ctrTextureFormatTable[ Format::NumberOf ] = {
		0xFF, // None = 0,

		//  surface		 [surface]	   [texture]
		//! 各色に 5 ビットを使用する 16 ビットのピクセル形式。
		0xFF, // X1R5G5B5,	   //  X1R5G5B5		X1R5G5B5
		//! R 5ビット、G 6ビット、B 5ビットをのピクセル形式。
		PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_SHORT_5_6_5, // R5G6B5,		 //  R5G6B5		  R5G6B5
		//! 各色に 8 ビットを使用する 32 ビットの RGB ピクセル形式。
		PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_BYTE, // X8R8G8B8,	   //  X8R8G8B8		X8R8G8B8
		//! アルファ付きの、チャネルごとに 8 ビットを使用する 32 ビットの ARGB ピクセル形式。 
		PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_BYTE, //A8R8G8B8,	   //  A8R8G8B8		A8R8G8B8		(PS3:displayable)
		//! ８ビットのBのみ。
		0xFF, //B8,			 //  B8			  B8
		//! ８ビットのGとBのみ。
		0xFF, // G8B8,		   //  G8B8			G8B8
		//! アルファ付きの、チャネルごとに 16 ビットを使用する 64　ビットのHalfFloatのARGBピクセル形式
		0xFF, // A16B16G16R16F,  //  A16B16G16R16F   A16B16G16R16F   (PS3:displayable)
		//! アルファ付きの、チャネルごとに 32 ビットを使用する 128　ビットのFloatのARGBピクセル形式
		0xFF, // A32B32G32R32F,  //  A32B32G32R32F   A32B32G32R32F
		//! 32ビットのFloat
		0xFF, // R32F,		   //  R32F			R32F
		//! 各色に 8 ビットを使用する 32 ビットの BGR ピクセル形式。
		0xFF, // X8B8G8R8,	   //  X8B8G8R8		(X8R8G8B8 + PS3:remap)
		//! アルファ付きの各色 8 ビットを使用する 32 ビットの ABGR ピクセル形式。
		0xFF, // A8B8G8R8,	   //  A8B8G8R8		(A8R8G8B8 + PS3:remap)

		//  depth stencil
		//! 16 ビットの Z バッファ
		0xFF, // D16,			//  D16			 D16
		//! 16 ビットFloatの Z バッファ
		0xFF, // D16F,		   //  D16F			D16F
		//! 深度チャネルに 24 ビットを使用する 32 ビットの z バッファ。
		0xFF, // D24X8,		  //  D24S8		   D24S8		   (PS3:Ｚステンシルバッファが３２ビットの場合ステンシルバッファを切れない)
		//! 深度チャネルに 24 ビット、ステンシル チャネルに 8 ビットを使用する 32 ビットの Z バッファ。 
		0xFF, // D24S8,		  //  D24S8		   D24S8
		//! 深度チャネルに 24 ビットFloat、ステンシル チャネルに 8 ビットを使用する 32 ビットの Z バッファ。 
		0xFF, // D24S8F,		 //  D24S8F		  D24S8F

		//  texture
		//! R 6ビット、G 5ビット、B 5ビットをのピクセル形式。
		0xFF, // R6G5B5,		 //  (none)		  R6G5B5
		//! G, Rに16ビットFloatを使用する32ビットのピクセル形式。
		0xFF, // G16R16F,		//  (none)		  G16R16F
		//! G, Rに16ビットを使用する32ビットのピクセル形式。
		0xFF, // G16R16,		 //  (none)		  G16R16
		//! Rに16ビットを使用するピクセル形式。
		0xFF, // R16,			//  (none)		  R16
		//! Aに1ビット、RGBに５ビットを使用する32ビットのピクセル形式。
		PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_SHORT_5_5_5_1, // A1R5G5B5,	   //  (none)		  A1R5G5B5
		//! アルファ付きの各色 4 ビットを使用する 16 ビットの ARGB ピクセル形式。
		PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_SHORT_4_4_4_4, // A4R4G4B4,	   //  (none)		  A4R4G4B4
		//! Aに1ビット、RGBに５ビットを使用する16ビットのRGBAピクセル形式。
		PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_SHORT_5_5_5_1, // R5G5B5A1,	   //  (none)		  R5G5B5A1
		//! DXT1圧縮フォーマット
		0xFF, // DXT1,		   //  (none)		  DXT1
		//! DXT3圧縮フォーマット
		0xFF, // DXT3,		   //  (none)		  DXT3
		//! DXT5圧縮フォーマット
		0xFF, // DXT5,		   //  (none)		  DXT5

		//! V8U8フォーマット(バンプマップに使用)
		0xFF, // V8U8,		   //  (none)		  V8U8

		0xFF, // X2R10G10B10,
		0xFF, // A2R10G10B10,
		0xFF, // X8R8G8B8_LE,
		0xFF, // A8R8G8B8_LE,
		0xFF, // X2R10G10B10_LE,
		0xFF, // A2R10G10B10_LE,

    // CTR用に追加
    PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_BYTE, // R8G8B8,
    PICA_DATA_TEXTURE_FORMAT_LUMINANCE_ALPHA_UNSIGNED_BYTE, // L8A8,
    PICA_DATA_TEXTURE_FORMAT_HILO8_DMP_UNSIGNED_BYTE, // H8L8,
    PICA_DATA_TEXTURE_FORMAT_LUMINANCE_UNSIGNED_BYTE, // L8,
    PICA_DATA_TEXTURE_FORMAT_ALPHA_UNSIGNED_BYTE, // A8,
    PICA_DATA_TEXTURE_FORMAT_LUMINANCE_ALPHA_UNSIGNED_BYTE_4_4_DMP, // L4A4,
    PICA_DATA_TEXTURE_FORMAT_LUMINANCE_UNSIGNED_4BITS_DMP, // L4,
    PICA_DATA_TEXTURE_FORMAT_ALPHA_UNSIGNED_4BITS_DMP, // A4,
    PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP, // ETC1,
    PICA_DATA_TEXTURE_FORMAT_ETC1_ALPHA_RGB8_A4_NATIVE_DMP, // ETC1_A4,
    PICA_DATA_TEXTURE_FORMAT_SHADOW_UNSIGNED_INT, // SHADOW,
    PICA_DATA_TEXTURE_FORMAT_GAS_DMP_UNSIGNED_SHORT, // GAS,
    0xFF, // REF,
  };

  //! @brief カラーフォーマットの変換テーブル
  static const u8 s_ctrColorFormatTable[ Format::NumberOf ] = {
		0xFF, // None = 0,

		//  surface		 [surface]	   [texture]
		//! 各色に 5 ビットを使用する 16 ビットのピクセル形式。
		0xFF, // X1R5G5B5,	   //  X1R5G5B5		X1R5G5B5
		//! R 5ビット、G 6ビット、B 5ビットをのピクセル形式。
		PICA_DATA_COLOR_RGB565, // R5G6B5,		 //  R5G6B5		  R5G6B5
		//! 各色に 8 ビットを使用する 32 ビットの RGB ピクセル形式。
		0xFF, // X8R8G8B8,	   //  X8R8G8B8		X8R8G8B8
		//! アルファ付きの、チャネルごとに 8 ビットを使用する 32 ビットの ARGB ピクセル形式。 
		PICA_DATA_COLOR_RGBA8_OES, //A8R8G8B8,	   //  A8R8G8B8		A8R8G8B8		(PS3:displayable)
		//! ８ビットのBのみ。
		0xFF, //B8,			 //  B8			  B8
		//! ８ビットのGとBのみ。
		0xFF, // G8B8,		   //  G8B8			G8B8
		//! アルファ付きの、チャネルごとに 16 ビットを使用する 64　ビットのHalfFloatのARGBピクセル形式
		0xFF, // A16B16G16R16F,  //  A16B16G16R16F   A16B16G16R16F   (PS3:displayable)
		//! アルファ付きの、チャネルごとに 32 ビットを使用する 128　ビットのFloatのARGBピクセル形式
		0xFF, // A32B32G32R32F,  //  A32B32G32R32F   A32B32G32R32F
		//! 32ビットのFloat
		0xFF, // R32F,		   //  R32F			R32F
		//! 各色に 8 ビットを使用する 32 ビットの BGR ピクセル形式。
		0xFF, // X8B8G8R8,	   //  X8B8G8R8		(X8R8G8B8 + PS3:remap)
		//! アルファ付きの各色 8 ビットを使用する 32 ビットの ABGR ピクセル形式。
		0xFF, // A8B8G8R8,	   //  A8B8G8R8		(A8R8G8B8 + PS3:remap)

		//  depth stencil
		//! 16 ビットの Z バッファ
		0xFF, // D16,			//  D16			 D16
		//! 16 ビットFloatの Z バッファ
		0xFF, // D16F,		   //  D16F			D16F
		//! 深度チャネルに 24 ビットを使用する 32 ビットの z バッファ。
		0xFF, // D24X8,		  //  D24S8		   D24S8		   (PS3:Ｚステンシルバッファが３２ビットの場合ステンシルバッファを切れない)
		//! 深度チャネルに 24 ビット、ステンシル チャネルに 8 ビットを使用する 32 ビットの Z バッファ。 
		0xFF, // D24S8,		  //  D24S8		   D24S8
		//! 深度チャネルに 24 ビットFloat、ステンシル チャネルに 8 ビットを使用する 32 ビットの Z バッファ。 
		0xFF, // D24S8F,		 //  D24S8F		  D24S8F

		//  texture
		//! R 6ビット、G 5ビット、B 5ビットをのピクセル形式。
		0xFF, // R6G5B5,		 //  (none)		  R6G5B5
		//! G, Rに16ビットFloatを使用する32ビットのピクセル形式。
		0xFF, // G16R16F,		//  (none)		  G16R16F
		//! G, Rに16ビットを使用する32ビットのピクセル形式。
		0xFF, // G16R16,		 //  (none)		  G16R16
		//! Rに16ビットを使用するピクセル形式。
		0xFF, // R16,			//  (none)		  R16
		//! Aに1ビット、RGBに５ビットを使用する32ビットのピクセル形式。
		PICA_DATA_COLOR_RGB5_A1, // A1R5G5B5,	   //  (none)		  A1R5G5B5
		//! アルファ付きの各色 4 ビットを使用する 16 ビットの ARGB ピクセル形式。
		PICA_DATA_COLOR_RGBA4, // A4R4G4B4,	   //  (none)		  A4R4G4B4
		//! Aに1ビット、RGBに５ビットを使用する16ビットのRGBAピクセル形式。
		PICA_DATA_COLOR_RGB5_A1, // R5G5B5A1,	   //  (none)		  R5G5B5A1
		//! DXT1圧縮フォーマット
		0xFF, // DXT1,		   //  (none)		  DXT1
		//! DXT3圧縮フォーマット
		0xFF, // DXT3,		   //  (none)		  DXT3
		//! DXT5圧縮フォーマット
		0xFF, // DXT5,		   //  (none)		  DXT5

		//! V8U8フォーマット(バンプマップに使用)
		0xFF, // V8U8,		   //  (none)		  V8U8

		0xFF, // X2R10G10B10,
		0xFF, // A2R10G10B10,
		0xFF, // X8R8G8B8_LE,
		0xFF, // A8R8G8B8_LE,
		0xFF, // X2R10G10B10_LE,
		0xFF, // A2R10G10B10_LE,

    // CTR用に追加
    0xFF, // R8G8B8,
    0xFF, // L8A8,
    0xFF, // H8L8,
    0xFF, // L8,
    0xFF, // A8,
    0xFF, // L4A4,
    0xFF, // L4,
    0xFF, // A4,
    0xFF, // ETC1,
    0xFF, // ETC1_A4,
    0xFF, // SHADOW,
    PICA_DATA_COLOR_GAS_DMP, // GAS,
    0xFF, // REF,
  };

  //! @brief カラーフォーマットの変換テーブル
  static const u8 s_ctrDepthFormatTable[ Format::NumberOf ] = {
		0xFF, // None = 0,

		//  surface		 [surface]	   [texture]
		//! 各色に 5 ビットを使用する 16 ビットのピクセル形式。
		0xFF, // X1R5G5B5,	   //  X1R5G5B5		X1R5G5B5
		//! R 5ビット、G 6ビット、B 5ビットをのピクセル形式。
		0xFF, // R5G6B5,		 //  R5G6B5		  R5G6B5
		//! 各色に 8 ビットを使用する 32 ビットの RGB ピクセル形式。
		0xFF, // X8R8G8B8,	   //  X8R8G8B8		X8R8G8B8
		//! アルファ付きの、チャネルごとに 8 ビットを使用する 32 ビットの ARGB ピクセル形式。 
		0xFF, //A8R8G8B8,	   //  A8R8G8B8		A8R8G8B8		(PS3:displayable)
		//! ８ビットのBのみ。
		0xFF, //B8,			 //  B8			  B8
		//! ８ビットのGとBのみ。
		0xFF, // G8B8,		   //  G8B8			G8B8
		//! アルファ付きの、チャネルごとに 16 ビットを使用する 64　ビットのHalfFloatのARGBピクセル形式
		0xFF, // A16B16G16R16F,  //  A16B16G16R16F   A16B16G16R16F   (PS3:displayable)
		//! アルファ付きの、チャネルごとに 32 ビットを使用する 128　ビットのFloatのARGBピクセル形式
		0xFF, // A32B32G32R32F,  //  A32B32G32R32F   A32B32G32R32F
		//! 32ビットのFloat
		0xFF, // R32F,		   //  R32F			R32F
		//! 各色に 8 ビットを使用する 32 ビットの BGR ピクセル形式。
		0xFF, // X8B8G8R8,	   //  X8B8G8R8		(X8R8G8B8 + PS3:remap)
		//! アルファ付きの各色 8 ビットを使用する 32 ビットの ABGR ピクセル形式。
		0xFF, // A8B8G8R8,	   //  A8B8G8R8		(A8R8G8B8 + PS3:remap)

		//  depth stencil
		//! 16 ビットの Z バッファ
		PICA_DATA_DEPTH_COMPONENT16, // D16,			//  D16			 D16
		//! 16 ビットFloatの Z バッファ
		PICA_DATA_DEPTH_COMPONENT16, // D16F,		   //  D16F			D16F
		//! 深度チャネルに 24 ビットを使用する 32 ビットの z バッファ。
		PICA_DATA_DEPTH24_STENCIL8_EXT, // D24X8,		  //  D24S8		   D24S8		   (PS3:Ｚステンシルバッファが３２ビットの場合ステンシルバッファを切れない)
		//! 深度チャネルに 24 ビット、ステンシル チャネルに 8 ビットを使用する 32 ビットの Z バッファ。 
		PICA_DATA_DEPTH24_STENCIL8_EXT, // D24S8,		  //  D24S8		   D24S8
		//! 深度チャネルに 24 ビットFloat、ステンシル チャネルに 8 ビットを使用する 32 ビットの Z バッファ。 
		PICA_DATA_DEPTH24_STENCIL8_EXT, // D24S8F,		 //  D24S8F		  D24S8F

		//  texture
		//! R 6ビット、G 5ビット、B 5ビットをのピクセル形式。
		0xFF, // R6G5B5,		 //  (none)		  R6G5B5
		//! G, Rに16ビットFloatを使用する32ビットのピクセル形式。
		0xFF, // G16R16F,		//  (none)		  G16R16F
		//! G, Rに16ビットを使用する32ビットのピクセル形式。
		0xFF, // G16R16,		 //  (none)		  G16R16
		//! Rに16ビットを使用するピクセル形式。
		0xFF, // R16,			//  (none)		  R16
		//! Aに1ビット、RGBに５ビットを使用する32ビットのピクセル形式。
		0xFF, // A1R5G5B5,	   //  (none)		  A1R5G5B5
		//! アルファ付きの各色 4 ビットを使用する 16 ビットの ARGB ピクセル形式。
		0xFF, // A4R4G4B4,	   //  (none)		  A4R4G4B4
		//! Aに1ビット、RGBに５ビットを使用する16ビットのRGBAピクセル形式。
		0xFF, // R5G5B5A1,	   //  (none)		  R5G5B5A1
		//! DXT1圧縮フォーマット
		0xFF, // DXT1,		   //  (none)		  DXT1
		//! DXT3圧縮フォーマット
		0xFF, // DXT3,		   //  (none)		  DXT3
		//! DXT5圧縮フォーマット
		0xFF, // DXT5,		   //  (none)		  DXT5

		//! V8U8フォーマット(バンプマップに使用)
		0xFF, // V8U8,		   //  (none)		  V8U8

		0xFF, // X2R10G10B10,
		0xFF, // A2R10G10B10,
		0xFF, // X8R8G8B8_LE,
		0xFF, // A8R8G8B8_LE,
		0xFF, // X2R10G10B10_LE,
		0xFF, // A2R10G10B10_LE,

    // CTR用に追加
    0xFF, // R8G8B8,
    0xFF, // L8A8,
    0xFF, // H8L8,
    0xFF, // L8,
    0xFF, // A8,
    0xFF, // L4A4,
    0xFF, // L4,
    0xFF, // A4,
    0xFF, // ETC1,
    0xFF, // ETC1_A4,
    0xFF, // SHADOW,
    0xFF, // GAS,
    0xFF, // REF,
  };

	//! @brief インデックスタイプテーブル
  static const GLuint s_openglIndexType[ IndexType::NumberOf ] = {
      GL_UNSIGNED_SHORT,						//Index16 = 0,
      GL_UNSIGNED_INT,							//Index32,
  };

	//! @brief プリミティブタイプテーブル
  static const GLuint s_openglPrimitiveType[ PrimitiveType::NumberOf ] = {
      GL_NONE,						//Points = 0,     //!< ポイント
      GL_NONE,							//LineStrip,      //!< ラインストリップ
      GL_NONE,        //Lines,          //!< ライン
      GL_TRIANGLES,					//Triangles,      //!< トライアングル
      GL_TRIANGLE_STRIP,    //TriangleStrip,  //!< トライアングルストリップ
  };

	//! @brief ブレンドオペレーション
  static const GLuint s_openglBlendFunc[ BlendFunc::NumberOf ] = {
      GL_ZERO,								//  Zero = 0,                   //!< ( R, G, B, A ) = ( 0, 0, 0, 0 )
      GL_ONE,									//  One,                        //!< ( R, G, B, A ) = ( 1, 1, 1, 1 )
      GL_SRC_COLOR,						//  SrcColor,                   //!< ( R, G, B, A ) = ( Rs, Gs, Bs, As )
      GL_ONE_MINUS_SRC_COLOR, //  InverseSrcColor,            //!< ( R, G, B, A ) = ( 1 - Rs, 1 - Gs, 1 - Bs, 1 - As )
      GL_DST_COLOR,						//  DsetColor,                  //!< ( R, G, B, A ) = ( Rd, Gd, Bd, Ad )
      GL_ONE_MINUS_DST_COLOR, //  InverseDestColor,           //!< ( R, G, B, A ) = ( 1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad )
      GL_SRC_ALPHA,						//  SrcAlpha,                   //!< ( R, G, B, A ) = ( As, As, As, As )
      GL_ONE_MINUS_SRC_ALPHA, //  InverseSrcAlpha,            //!< ( R, G, B, A ) = ( 1 - As, 1 - As, 1 - As, 1 - As )
      GL_DST_ALPHA,						//  DestAlpha,                  //!< ( R, G, B, A ) = ( Ad, Ad, Ad, Ad )
      GL_ONE_MINUS_DST_ALPHA, //  InverseDestAlpha,           //!< ( R, G, B, A ) = ( 1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad )
      GL_SRC_ALPHA_SATURATE,  //  SrcAlphaSaturate,           //!< ( R, G, B, A ) = ( min( As, 1 - Ad ), min( As, 1 - Ad ), min( As, 1 - Ad ), 1 )
  };

  //! @brief ブレンドオペレーション
  static const GLuint s_openglBlendOp[ BlendOp::NumberOf ] = {
      GL_FUNC_ADD,								//  Add = 0,                //!< ( R, G, B, A ) = ( Rs * Rx + Rd * Ry, Gs * Gx + Gd * Gy, Bs * Bx + Bd * By, As * Ax + Ad * Ay )
      GL_FUNC_SUBTRACT,						//  Subtract,               //!< ( R, G, B, A ) = ( Rs * Rx - Rd * Ry, Gs * Gx - Gd * Gy, Bs * Bx - Bd * By, As * Ax - Ad * Ay )
      GL_FUNC_REVERSE_SUBTRACT,   //  ReverseSubtract,        //!< ( R, G, B, A ) = ( Rd * Ry - Rs * Rx, Gd * Gy - Gs * Gx, Bd * By - Bs * Bx, Ad * Ay - As * Ax )
      GL_MIN,											//  Min,                    //!< ( R, G, B, A ) = ( min( Rs * Rd ), min( Gs * Gd ), min( Bs * Bd ), min( As * Ad ) )
      GL_MAX,											//  Max,                    //!< ( R, G, B, A ) = ( MAX( Rs * Rd ), MAX( Gs * Gd ), MAX( Bs * Bd ), MAX( As * Ad ) )
  };

	//! @brief　カリングモード
	static const GLuint s_openglCullMode[ CullMode::NumberOf ] = {
      GL_NONE,  //  None = 0,           //!< 背面カリングしない
      GL_FRONT,  //  CW,                 //!< 背面を右回りでカリング
      GL_BACK,	//  CCW,                //!< 背面を左回りでカリング
  };

	//! @brief　フィルモード
	static const GLuint s_openglFillMode[ FillMode::NumberOf ] = {
      GL_NONE,  //  Line,           //!< ライン
      GL_NONE,  //  Fill,           //!< フィル
  };

	//! @brief 比較関数テーブル
  static const GLuint s_openglCompareFunc[ CompareFunc::NumberOf ] = {
      GL_LESS,           //  Less,          //!< リファレンス値より小さい場合にのみパスさせる 
      GL_LEQUAL,				 //  LessEqual,     //!< リファレンス値と同じか小さい場合にパスさせる 
      GL_EQUAL,          //  Equal,         //!< リファレンス値と同じ場合にはパスさせる 
      GL_GEQUAL,				 //  GreaterEqual,  //!< リファレンス値と同じか大きい場合にパスさせる 
      GL_GREATER,        //  Greater,       //!< リファレンス値よりも大きい場合にパスさせる 
      GL_NOTEQUAL,       //  NotEqual,      //!< リファレンス値と同じではない場合にパスさせる 
      GL_ALWAYS,         //  Always,        //!< アルファテストを常にパスさせる 
      GL_NEVER,          //  Never = 0,     //!< アルファテストをパスさせない 
  };

	//! @brief　フィルモード
	//! @brief ステンシルオペレーションテーブル
  static const GLuint s_openglStencilOp[ StencilOp::NumberOf ] = {
      GL_KEEP,          //  Keep = 0,           //!< 更新しない
      GL_ZERO,          //  Zero,               //!< ０に設定
      GL_REPLACE,       //  Replace,            //!< 参照値で置き換える
      GL_INCR,					//  Increment,          //!< 最大値に達するまで増加
      GL_INCR_WRAP,     //  IncrementWrap,      //!< 増加させその値が最大値を超えると０に戻します。
      GL_DECR,					//  Decrement,          //!< ０になるまで減少
      GL_DECR_WRAP,     //  DecrementWrap,      //!< 減少させその値が０より小さくなった場合は最大値に戻します。
      GL_INVERT,        //  Invert,             //!< ビットを反転
  };

	//! @brief テクスチャアドレステーブル
  static PicaDataTextureWrap s_ctrTextureWrap[ TextureAddress::NumberOf ] =
  {
    PICA_DATA_TEXTURE_WRAP_REPEAT,							//  Wrap = 0,       //!< テクスチャサイズで割った余りを座標値としてとる。\nテクスチャを繰り返しパターンとして使用することになる
    PICA_DATA_TEXTURE_WRAP_MIRRORED_REPEAT,     //  Mirror,         //!< テクスチャサイズごとに折り返した座標値をとる。\nテクスチャを反転しながら繰り返すパターンとして使用することになる
    PICA_DATA_TEXTURE_WRAP_CLAMP_TO_BORDER,     //  Border,         //!< テクスチャ境界のテクセル値を読み出す
    PICA_DATA_TEXTURE_WRAP_CLAMP_TO_EDGE,       //  Clamp,          //!< [0, 1]の範囲にクランプした座標値をとる
  };

	static PicaDataTextureMinFilter s_ctrTextureMinFilterMipmap[TextureFilter::NumberOf][TextureFilter::NumberOf] =
	{
		{PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_NEAREST, PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_NEAREST},
		{PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_LINEAR, PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_LINEAR}
	};

  static PicaDataTextureMinFilter s_ctrTextureMinFilter[TextureFilter::NumberOf] = 
  {
    PICA_DATA_TEXTURE_MIN_FILTER_NEAREST,
    PICA_DATA_TEXTURE_MIN_FILTER_LINEAR,
  };

  static PicaDataTextureMagFilter s_ctrTextureMagFilter[TextureFilter::NumberOf] = 
  {
    PICA_DATA_TEXTURE_MAG_FILTER_NEAREST,
    PICA_DATA_TEXTURE_MAG_FILTER_LINEAR,
  };

}}}

#endif
