#if !defined(GFLIB2_GRX_WINOpenGL_WINOpenGLTYPES_H_INCLUDED)
#define GFLIB2_GRX_WINOpenGL_WINOpenGLTYPES_H_INCLUDED

#include <gfx/include/gfl2_types.h>
#include <util/include/gfl2_AutoComPtr.h>

#include <gl/glew.h>
#include <gl/gl.h>


//$(NW4F_ROOT)\Library\WinExt\include
//$(NW4F_ROOT)\Library\WinExt\extlib

// 必要なライブラリをリンクする
#pragma comment( lib, "gdi32.lib" )
#pragma comment( lib, "user32.lib" )

#pragma comment( lib, "gl/glew32.lib" )
#pragma comment( lib, "opengl32.lib" )

namespace gfl2 { namespace gfx { namespace winOpenGL {

	//-------------------------------------------------------------------------------------------------------------------------------------------------
  //! @brief サーフェイスフォーマットの変換テーブル
  static const GLuint s_openglFormatTable[ Format::NumberOf ] = {
		GL_NONE,														//None = 0,

		//  surface
		GL_RGB5_A1,													//X1R5G5B5,       //  X1R5G5B5        X1R5G5B5
		GL_RGB,															//R5G6B5,         //  R5G6B5          R5G6B5
		GL_RGB,															//X8R8G8B8,       //  X8R8G8B8        X8R8G8B8
		GL_RGBA,														//A8R8G8B8,       //  A8R8G8B8        A8R8G8B8        (PS3:displayable)
		GL_ALPHA,														//B8,             //  B8              B8
		GL_NONE,														//G8B8,           //  G8B8            G8B8
		GL_RGBA16F,													//A16B16G16R16F,  //  A16B16G16R16F   A16B16G16R16F   (PS3:displayable)
		GL_RGBA32F,													//A32B32G32R32F,  //  A32B32G32R32F   A32B32G32R32F
		GL_R32F,														//R32F,           //  R32F            R32F
		GL_RGBA8,														//X8B8G8R8,       //  X8B8G8R8        (X8R8G8B8 + PS3:remap)
		GL_RGBA8,														//A8B8G8R8,       //  A8B8G8R8        (A8R8G8B8 + PS3:remap)

		//  depth stencil
		GL_INVALID_VALUE,										//D16,            //  D16             D16
		GL_INVALID_VALUE,										//D16F,           //  D16F            D16F
		GL_INVALID_VALUE,										//D24X8,          //  D24S8           D24S8           (PS3:Ｚステンシルバッファが３２ビットの場合ステンシルバッファを切れない)
		GL_DEPTH24_STENCIL8,								//D24S8,          //  D24S8           D24S8
		GL_INVALID_VALUE,										//D24S8F,         //  D24S8F          D24S8F

		//  texture
		GL_RGB5,														//R6G5B5,         //  (none)          R6G5B5
		GL_RG16F,														//G16R16F,        //  (none)          G16R16F
		GL_RG16,														//G16R16,         //  (none)          G16R16
		GL_R16UI,														//R16,            //  (none)          R16
		GL_RGB5,														//A1R5G5B5,       //  (none)          A1R5G5B5
		GL_NONE,														//A4R4G4B4,       //  (none)          A4R4G4B4
		GL_RGB5_A1,													//R5G5B5A1,       //  (none)          R5G5B5A1
		GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,   //DXT1,           //  (none)          DXT1
		GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,   //DXT3,           //  (none)          DXT3
		GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   //DXT5,           //  (none)          DXT5

		GL_NONE,														//V8U8           //  (none)           V8U8


		GL_NONE,														//X2R10G10B10,
		GL_RGB10_A2,												//A2R10G10B10,
		GL_NONE,														//X8R8G8B8_LE,
		GL_NONE,														//A8R8G8B8_LE,
		GL_NONE,														//X2R10G10B10_LE,
		GL_NONE,														//A2R10G10B10_LE,
  };

	//-------------------------------------------------------------------------------------------------------------------------------------------------
  //! @brief サーフェイスフォーマットバイトの変換テーブル
  static const GLuint s_openglByteTable[ Format::NumberOf ] = {
		GL_NONE,													//GL_NONE,														//None = 0,

		//  surface
		GL_UNSIGNED_SHORT_1_5_5_5_REV,		//GL_RGB5_A1,      //X1R5G5B5,       //  X1R5G5B5        X1R5G5B5
		GL_UNSIGNED_SHORT_5_6_5,				//GL_RGB5,          //R5G6B5,         //  R5G6B5          R5G6B5
		GL_UNSIGNED_BYTE,									//GL_RGB,															//X8R8G8B8,       //  X8R8G8B8        X8R8G8B8
		GL_UNSIGNED_BYTE,									//GL_RGBA,														//A8R8G8B8,       //  A8R8G8B8        A8R8G8B8        (PS3:displayable)
		GL_UNSIGNED_BYTE,									//GL_ALPHA,														//B8,             //  B8              B8
		GL_NONE,													//GL_NONE,														//G8B8,           //  G8B8            G8B8
		GL_HALF_FLOAT,										//GL_RGBA16F,													//A16B16G16R16F,  //  A16B16G16R16F   A16B16G16R16F   (PS3:displayable)
		GL_FLOAT,													//GL_RGBA32F,													//A32B32G32R32F,  //  A32B32G32R32F   A32B32G32R32F
		GL_FLOAT,													//GL_R32F,														//R32F,           //  R32F            R32F
		GL_UNSIGNED_BYTE,									//GL_RGBA8,														//X8B8G8R8,       //  X8B8G8R8        (X8R8G8B8 + PS3:remap)
		GL_UNSIGNED_BYTE,									//GL_RGBA8,														//A8B8G8R8,       //  A8B8G8R8        (A8R8G8B8 + PS3:remap)

		//  depth stencil
		GL_NONE,													//GL_INVALID_VALUE,										//D16,            //  D16             D16
		GL_NONE,													//GL_INVALID_VALUE,										//D16F,           //  D16F            D16F
		GL_NONE,													//GL_INVALID_VALUE,										//D24X8,          //  D24S8           D24S8           (PS3:Ｚステンシルバッファが３２ビットの場合ステンシルバッファを切れない)
		GL_UNSIGNED_INT_24_8,							//GL_DEPTH24_STENCIL8,								//D24S8,          //  D24S8           D24S8
		GL_NONE,													//GL_INVALID_VALUE,										//D24S8F,         //  D24S8F          D24S8F

		//  texture
		GL_UNSIGNED_SHORT_5_5_5_1,				//GL_RGB5,														//R6G5B5,         //  (none)          R6G5B5
		GL_SHORT,													//GL_RG16F,														//G16R16F,        //  (none)          G16R16F
		GL_UNSIGNED_SHORT,								//GL_RG16,														//G16R16,         //  (none)          G16R16
		GL_UNSIGNED_SHORT,								//GL_R16UI,														//R16,            //  (none)          R16
		GL_UNSIGNED_SHORT_5_5_5_1,				//GL_RGB5,          //A1R5G5B5,       //  (none)          A1R5G5B5
		GL_NONE,													//GL_NONE,														//A4R4G4B4,       //  (none)          A4R4G4B4
		GL_UNSIGNED_SHORT_1_5_5_5_REV,		//GL_RGB5_A1,      //R5G5B5A1,       //  (none)          R5G5B5A1
		GL_NONE,													//GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,   //DXT1,           //  (none)          DXT1
		GL_NONE,													//GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,   //DXT3,           //  (none)          DXT3
		GL_NONE,													//GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   //DXT5,           //  (none)          DXT5

		GL_NONE,													//GL_NONE,														//V8U8           //  (none)           V8U8

		GL_NONE,													//GL_NONE,														//X2R10G10B10,
		GL_UNSIGNED_INT_10_10_10_2,				//GL_RGB10_A2,												//A2R10G10B10,
		GL_NONE,													//GL_NONE,														//X8R8G8B8_LE,
		GL_NONE,													//GL_NONE,														//A8R8G8B8_LE,
		GL_NONE,													//GL_NONE,														//X2R10G10B10_LE,
		GL_NONE,													//GL_NONE,														//A2R10G10B10_LE,
  };

	//! @brief インデックスタイプテーブル
  static const GLuint s_openglIndexType[ IndexType::NumberOf ] = {
      GL_UNSIGNED_SHORT,						//Index16 = 0,
      GL_UNSIGNED_INT,							//Index32,
      GL_UNSIGNED_BYTE,							//Index8,
  };

	//! @brief プリミティブタイプテーブル
  static const GLuint s_openglPrimitiveType[ PrimitiveType::NumberOf ] = {
      GL_POINTS,						//Points = 0,     //!< ポイント
      GL_LINE_STRIP,        //Lines,          //!< ライン
      GL_LINES,							//LineStrip,      //!< ラインストリップ
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
			GL_CONSTANT_COLOR,
			GL_ONE_MINUS_CONSTANT_COLOR,
			GL_CONSTANT_ALPHA,
			GL_ONE_MINUS_CONSTANT_ALPHA,
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
  // !! caution !! //
  // GFではCWが表. CCWが裏.
  // OpenGLでは初期設定でCCWが表になっているので, 
  // それに合わせて, GL_BACKが表, GL_FRONTが裏!
	static const GLuint s_openglCullMode[ CullMode::NumberOf ] = {
      GL_NONE,  //  None = 0,        //!< 背面カリングしない
      GL_BACK,	//  CW,              //!< 背面を左回りでカリング
      GL_FRONT, // CCW,              //!< 背面を右回りでカリング
  };

	//! @brief　フィルモード
	static const GLuint s_openglFillMode[ FillMode::NumberOf ] = {
      GL_LINE,  //  Line,           //!< ライン
      GL_FILL,  //  Fill,           //!< フィル
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
  static const GLuint s_openglTextureAddress[ TextureAddress::NumberOf ] = {
      GL_REPEAT,							//  Wrap = 0,       //!< テクスチャサイズで割った余りを座標値としてとる。\nテクスチャを繰り返しパターンとして使用することになる
      GL_MIRRORED_REPEAT,     //  Mirror,         //!< テクスチャサイズごとに折り返した座標値をとる。\nテクスチャを反転しながら繰り返すパターンとして使用することになる
      GL_CLAMP_TO_BORDER,     //  Border,         //!< テクスチャ境界のテクセル値を読み出す
      GL_CLAMP_TO_EDGE,       //  Clamp,          //!< [0, 1]の範囲にクランプした座標値をとる
  };

	static const GLuint s_openglTextureFilter[TextureFilter::NumberOf][TextureFilter::NumberOf] =
	{
		{GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST},
		{GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR}
	};

}}}

#endif