#ifndef GFLIB2_GRX_WINDX11_WINDX11TYPES_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11TYPES_H_INCLUDED

#include <d3d11.h>
#include <d3dcompiler.h>
//#include <dxerr.h>
#include <gfx/include/gfl2_types.h>
#include <util/include/gfl2_AutoComPtr.h>

// 必要なライブラリをリンクする
#pragma comment( lib, "gdi32.lib" )
#pragma comment( lib, "user32.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "d3dcompiler.lib" )


namespace gfl2 { namespace gfx { namespace windx11 {

	//-------------------------------------------------------------------------------------------------------------------------------------------------
  //! @brief サーフェイスフォーマットの変換テーブル
  static const DXGI_FORMAT s_d3d11FormatTable[ Format::NumberOf ] = {
		DXGI_FORMAT_UNKNOWN,                //None = 0,

		//  surface
		DXGI_FORMAT_B5G5R5A1_UNORM,         //X1R5G5B5,       //  X1R5G5B5        X1R5G5B5
		DXGI_FORMAT_B5G6R5_UNORM,           //R5G6B5,         //  R5G6B5          R5G6B5
		DXGI_FORMAT_UNKNOWN,								//X8R8G8B8,       //  X8R8G8B8        X8R8G8B8
		DXGI_FORMAT_B8G8R8A8_UNORM,         //A8R8G8B8,       //  A8R8G8B8        A8R8G8B8        (PS3:displayable)
		DXGI_FORMAT_A8_UNORM,								//B8,             //  B8              B8
		DXGI_FORMAT_UNKNOWN,                //G8B8,           //  G8B8            G8B8
		DXGI_FORMAT_R16G16B16A16_FLOAT,     //A16B16G16R16F,  //  A16B16G16R16F   A16B16G16R16F   (PS3:displayable)
		DXGI_FORMAT_R32G32B32A32_FLOAT,     //A32B32G32R32F,  //  A32B32G32R32F   A32B32G32R32F
		DXGI_FORMAT_R32_FLOAT,              //R32F,           //  R32F            R32F
		DXGI_FORMAT_R8G8B8A8_UNORM,         //X8B8G8R8,       //  X8B8G8R8        (X8R8G8B8 + PS3:remap)
		DXGI_FORMAT_R8G8B8A8_UNORM,         //A8B8G8R8,       //  A8B8G8R8        (A8R8G8B8 + PS3:remap)

		//  depth stencil
		DXGI_FORMAT_D16_UNORM,              //D16,            //  D16             D16
		DXGI_FORMAT_D16_UNORM,              //D16F,           //  D16F            D16F
		DXGI_FORMAT_UNKNOWN,                //D24X8,          //  D24S8           D24S8           (PS3:Ｚステンシルバッファが３２ビットの場合ステンシルバッファを切れない)
		DXGI_FORMAT_D24_UNORM_S8_UINT,      //D24S8,          //  D24S8           D24S8
		DXGI_FORMAT_UNKNOWN,                //D24S8F,         //  D24S8F          D24S8F

		//  texture
		DXGI_FORMAT_B5G6R5_UNORM,           //R6G5B5,         //  (none)          R6G5B5
		DXGI_FORMAT_R16G16_FLOAT,           //G16R16F,        //  (none)          G16R16F
		DXGI_FORMAT_R16G16_UNORM,           //G16R16,         //  (none)          G16R16
		DXGI_FORMAT_R16_UINT,               //R16,            //  (none)          R16
		DXGI_FORMAT_B5G5R5A1_UNORM,         //A1R5G5B5,       //  (none)          A1R5G5B5
		DXGI_FORMAT_UNKNOWN,                //A4R4G4B4,       //  (none)          A4R4G4B4
		DXGI_FORMAT_B5G5R5A1_UNORM,         //R5G5B5A1,       //  (none)          R5G5B5A1
		DXGI_FORMAT_BC1_UNORM,              //DXT1,           //  (none)          DXT1
		DXGI_FORMAT_BC2_UNORM,              //DXT3,           //  (none)          DXT3
		DXGI_FORMAT_BC3_UNORM,              //DXT5,           //  (none)          DXT5

		DXGI_FORMAT_UNKNOWN,                //V8U8           //  (none)           V8U8


		DXGI_FORMAT_UNKNOWN,								//X2R10G10B10,
		DXGI_FORMAT_R10G10B10A2_UNORM,			//A2R10G10B10,
		DXGI_FORMAT_UNKNOWN,								//X8R8G8B8_LE,
		DXGI_FORMAT_UNKNOWN,								//A8R8G8B8_LE,
		DXGI_FORMAT_UNKNOWN,								//X2R10G10B10_LE,
		DXGI_FORMAT_UNKNOWN,								//A2R10G10B10_LE,
  };

	//! @brief プリミティブタイプテーブル
  static const D3D_PRIMITIVE_TOPOLOGY s_d3d11PrimitiveType[ PrimitiveType::NumberOf ] = {
      D3D_PRIMITIVE_TOPOLOGY_POINTLIST,        //Points = 0,     //!< ポイント
      D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,        //LineStrip,      //!< ラインストリップ
      D3D_PRIMITIVE_TOPOLOGY_LINELIST,         //Lines,          //!< ライン
      D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,     //Triangles,      //!< トライアングル
      D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,    //TriangleStrip,  //!< トライアングルストリップ
  };

	//! @brief ブレンドオペレーション
  static const D3D11_BLEND s_d3d11BlendFunc[ BlendFunc::NumberOf ] = {
      D3D11_BLEND_ZERO,           //  Zero = 0,                   //!< ( R, G, B, A ) = ( 0, 0, 0, 0 )
      D3D11_BLEND_ONE,            //  One,                        //!< ( R, G, B, A ) = ( 1, 1, 1, 1 )
      D3D11_BLEND_SRC_COLOR,      //  SrcColor,                   //!< ( R, G, B, A ) = ( Rs, Gs, Bs, As )
      D3D11_BLEND_INV_SRC_COLOR,  //  InverseSrcColor,            //!< ( R, G, B, A ) = ( 1 - Rs, 1 - Gs, 1 - Bs, 1 - As )
      D3D11_BLEND_DEST_COLOR,     //  DsetColor,                  //!< ( R, G, B, A ) = ( Rd, Gd, Bd, Ad )
      D3D11_BLEND_INV_DEST_COLOR, //  InverseDestColor,           //!< ( R, G, B, A ) = ( 1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad )
      D3D11_BLEND_SRC_ALPHA,      //  SrcAlpha,                   //!< ( R, G, B, A ) = ( As, As, As, As )
      D3D11_BLEND_INV_SRC_ALPHA,  //  InverseSrcAlpha,            //!< ( R, G, B, A ) = ( 1 - As, 1 - As, 1 - As, 1 - As )
      D3D11_BLEND_DEST_ALPHA,     //  DestAlpha,                  //!< ( R, G, B, A ) = ( Ad, Ad, Ad, Ad )
      D3D11_BLEND_INV_DEST_ALPHA, //  InverseDestAlpha,           //!< ( R, G, B, A ) = ( 1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad )
      D3D11_BLEND_SRC_ALPHA_SAT,  //  SrcAlphaSaturate,           //!< ( R, G, B, A ) = ( min( As, 1 - Ad ), min( As, 1 - Ad ), min( As, 1 - Ad ), 1 )
  };

  //! @brief ブレンドオペレーション
  static const D3D11_BLEND_OP s_d3d11BlendOp[ BlendOp::NumberOf ] = {
      D3D11_BLEND_OP_ADD,             //  Add = 0,                //!< ( R, G, B, A ) = ( Rs * Rx + Rd * Ry, Gs * Gx + Gd * Gy, Bs * Bx + Bd * By, As * Ax + Ad * Ay )
      D3D11_BLEND_OP_SUBTRACT,        //  Subtract,               //!< ( R, G, B, A ) = ( Rs * Rx - Rd * Ry, Gs * Gx - Gd * Gy, Bs * Bx - Bd * By, As * Ax - Ad * Ay )
      D3D11_BLEND_OP_REV_SUBTRACT,    //  ReverseSubtract,        //!< ( R, G, B, A ) = ( Rd * Ry - Rs * Rx, Gd * Gy - Gs * Gx, Bd * By - Bs * Bx, Ad * Ay - As * Ax )
      D3D11_BLEND_OP_MIN,             //  Min,                    //!< ( R, G, B, A ) = ( min( Rs * Rd ), min( Gs * Gd ), min( Bs * Bd ), min( As * Ad ) )
      D3D11_BLEND_OP_MAX,             //  Max,                    //!< ( R, G, B, A ) = ( MAX( Rs * Rd ), MAX( Gs * Gd ), MAX( Bs * Bd ), MAX( As * Ad ) )
  };

	//! @brief　カリングモード
	static const D3D11_CULL_MODE s_d3d11CullMode[ CullMode::NumberOf ] = {
      D3D11_CULL_NONE,   //  None = 0,           //!< 背面カリングしない
      D3D11_CULL_FRONT,  //  CW,                 //!< 背面を右回りでカリング
      D3D11_CULL_BACK,   //  CCW,                //!< 背面を左回りでカリング
  };

	//! @brief　フィルモード
	static const D3D11_FILL_MODE s_d3d11FillMode[ FillMode::NumberOf ] = {
      D3D11_FILL_WIREFRAME,  //  Line,           //!< ライン
      D3D11_FILL_SOLID,      //  Fill,           //!< フィル
  };

	//! @brief 比較関数テーブル
  static const D3D11_COMPARISON_FUNC s_d3d11CompareFunc[ CompareFunc::NumberOf ] = {
      D3D11_COMPARISON_LESS,            //  Less,          //!< リファレンス値より小さい場合にのみパスさせる 
      D3D11_COMPARISON_LESS_EQUAL,      //  LessEqual,     //!< リファレンス値と同じか小さい場合にパスさせる 
      D3D11_COMPARISON_EQUAL,           //  Equal,         //!< リファレンス値と同じ場合にはパスさせる 
      D3D11_COMPARISON_GREATER_EQUAL,   //  GreaterEqual,  //!< リファレンス値と同じか大きい場合にパスさせる 
      D3D11_COMPARISON_GREATER,         //  Greater,       //!< リファレンス値よりも大きい場合にパスさせる 
      D3D11_COMPARISON_NOT_EQUAL,       //  NotEqual,      //!< リファレンス値と同じではない場合にパスさせる 
      D3D11_COMPARISON_ALWAYS,          //  Always,        //!< アルファテストを常にパスさせる 
      D3D11_COMPARISON_NEVER,           //  Never = 0,     //!< アルファテストをパスさせない 
  };

	//! @brief　フィルモード
	//! @brief ステンシルオペレーションテーブル
  static const D3D11_STENCIL_OP s_d3d11StencilOp[ StencilOp::NumberOf ] = {
      D3D11_STENCIL_OP_KEEP,          //  Keep = 0,           //!< 更新しない
      D3D11_STENCIL_OP_ZERO,          //  Zero,               //!< ０に設定
      D3D11_STENCIL_OP_REPLACE,       //  Replace,            //!< 参照値で置き換える
      D3D11_STENCIL_OP_INCR_SAT,      //  Increment,          //!< 最大値に達するまで増加
      D3D11_STENCIL_OP_INCR,          //  IncrementWrap,      //!< 増加させその値が最大値を超えると０に戻します。
      D3D11_STENCIL_OP_DECR_SAT,      //  Decrement,          //!< ０になるまで減少
      D3D11_STENCIL_OP_DECR,          //  DecrementWrap,      //!< 減少させその値が０より小さくなった場合は最大値に戻します。
      D3D11_STENCIL_OP_INVERT,        //  Invert,             //!< ビットを反転
  };

	//! @brief テクスチャアドレステーブル
  static const D3D11_TEXTURE_ADDRESS_MODE s_d3d11TextureAddress[ TextureAddress::NumberOf ] = {
      D3D11_TEXTURE_ADDRESS_WRAP,       //  Wrap = 0,       //!< テクスチャサイズで割った余りを座標値としてとる。\nテクスチャを繰り返しパターンとして使用することになる
      D3D11_TEXTURE_ADDRESS_MIRROR,     //  Mirror,         //!< テクスチャサイズごとに折り返した座標値をとる。\nテクスチャを反転しながら繰り返すパターンとして使用することになる
      D3D11_TEXTURE_ADDRESS_BORDER,     //  Border,         //!< テクスチャ境界のテクセル値を読み出す
      D3D11_TEXTURE_ADDRESS_CLAMP,      //  Clamp,          //!< [0, 1]の範囲にクランプした座標値をとる
  };

	static const D3D11_FILTER s_d3d11TextureFilter[TextureFilter::NumberOf][TextureFilter::NumberOf][TextureFilter::NumberOf] =
	{
		{{D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR},{D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT, D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR}},
		{{D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT, D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR},{D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_FILTER_MIN_MAG_MIP_LINEAR}}
	};

}}}

#endif