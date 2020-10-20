#ifndef __GFL_GRAPHICSTYPE_H__
#define __GFL_GRAPHICSTYPE_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_GraphicsType.h
 *	@brief  グラフィックス環境　共通型、定数定義
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>
#include <nw/ut.h>

#include <gfl_Macros.h>

namespace gfl {
namespace grp {

//-----------------------------------------------------------------------------
/**
 *					デバッグ用
*/
//-----------------------------------------------------------------------------
// OPENGL系コマンドのエラーチェック
#if GFL_DEBUG
  
#define GFL_GRPGL_ERROR_CHECK() {\
  GLenum error = glGetError();\
  GFL_ASSERT_MSG( error == GL_NO_ERROR, "GLError code = 0x%x", error );\
}
  
#else
  
#define GFL_GRPGL_ERROR_CHECK() ((void)0)
  
#endif
  

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
// メモリタイプ
enum MemoryArea
{
  MEMORY_AREA_NONE,
  MEMORY_AREA_FCRAM = NN_GX_MEM_FCRAM,                      //!< FCRAMの領域を表します。
  MEMORY_AREA_VRAMA = NN_GX_MEM_VRAMA,                      //!< VRAMのAチャンネルの領域を表します。
  MEMORY_AREA_VRAMB = NN_GX_MEM_VRAMB,                      //!< VRAMのBチャンネルの領域を表します。
  ENUM_FORCE_DWORD(MemoryArea)
} ;
 
// メモリ転送タイプ
enum MemoryTransfer
{
  MEMORY_TRANSFER_COPY_FCRAM_DMP      = GL_COPY_FCRAM_DMP,    //< データのコピーをメインメモリに生成。
  MEMORY_TRANSFER_NO_COPY_FCRAM_DMP   = GL_NO_COPY_FCRAM_DMP, //< データのコピーをメインメモリに生成しない。
  ENUM_FORCE_DWORD(MemoryTransfer)
};

// メモリ使用用途
typedef enum
{
  MEM_SYSTEM        = NN_GX_MEM_SYSTEM,               // システムに使用。
  MEM_TEXTURE       = NN_GX_MEM_TEXTURE,              // テクスチャに使用。
  MEM_VERTEXBUFFER  = NN_GX_MEM_VERTEXBUFFER,         // 頂点バッファに使用。
  MEM_DISPLAYBUFFER = NN_GX_MEM_DISPLAYBUFFER,        // ディスプレイバッファに使用。
  MEM_COMMANDBUFFER = NN_GX_MEM_COMMANDBUFFER,        // コマンドバッファに使用。
  MEM_RENDERBUFFER  = NN_GX_MEM_RENDERBUFFER,         // レンダーバッファに使用。


  ENUM_FORCE_DWORD(MemoryUseType)
} MemoryUseType;

// 表示面定義
typedef enum
{
  DISPLAY_UPPER = 0,                      // 上画面　or 左画面
  DISPLAY_LOWER,                          // 下画面
  DISPLAY_UPPER_RIGHT,                    // 右画面

  DISPLAY_BOTH_MAX,                       // 全画面の数
  DISPLAY_NONE,                           // 対応する画面なし。 

  DISPLAY_NORMAL_MAX = DISPLAY_UPPER_RIGHT, // 立体視なしの画面数。


  ENUM_FORCE_DWORD(DisplayType)
} DisplayType;
  

//-------------------------------------
///	ディスプレイ　種類
//=====================================
typedef enum{
  DISPLAY_MSK_UPPER       = 1<<DISPLAY_UPPER,       // 上画面/上画面左
  DISPLAY_MSK_LOWER       = 1<<DISPLAY_LOWER,       // 下画面
  DISPLAY_MSK_UPPER_RIGHT = 1<<DISPLAY_UPPER_RIGHT, // 右画面

  DISPLAY_MSK_NORMAL  = DISPLAY_MSK_UPPER | DISPLAY_MSK_LOWER,                      // 通常時
  DISPLAY_MSK_BOTH    = DISPLAY_MSK_UPPER | DISPLAY_MSK_UPPER_RIGHT | DISPLAY_MSK_LOWER, // 立体視時

  DISPLAY_MSK_STEREO  = DISPLAY_MSK_UPPER | DISPLAY_MSK_UPPER_RIGHT, // 立体視上画面

  ENUM_FORCE_DWORD(DisplayMsk)
} DisplayMsk;


//-------------------------------------
///	レンダラーカラーフォーマット
//=====================================
enum RenderColorFormat
{
  RENDER_COLOR_FORMAT_NONE = 0,
  RENDER_COLOR_FORMAT_RGBA8 = GL_RGBA8_OES,
  RENDER_COLOR_FORMAT_RGB8 = GL_RGB8_OES,
  RENDER_COLOR_FORMAT_RGBA4 = GL_RGBA4,
  RENDER_COLOR_FORMAT_RGB5_A1 = GL_RGB5_A1,
  RENDER_COLOR_FORMAT_RGB565 = GL_RGB565,

  RENDER_COLOR_FORMAT_COUNT = 6,

  ENUM_FORCE_DWORD(RenderColorFormat)
};

//-------------------------------------
///	レンダラー 深度バッファフォーマット
//=====================================
enum RenderDepthFormat
{
  RENDER_DEPTH_FORMAT_NONE = 0,
  RENDER_DEPTH_FORMAT_16 = GL_DEPTH_COMPONENT16,
  RENDER_DEPTH_FORMAT_24 = GL_DEPTH_COMPONENT24_OES,
  RENDER_DEPTH_FORMAT_24_STENCIL8 = GL_DEPTH24_STENCIL8_EXT,

  RENDER_DEPTH_FORMAT_COUNT = 4,

  ENUM_FORCE_DWORD(RenderDepthFormat)
};

// シャドウの種別です。
enum ShadowType
{
  SHADOW_KIND_NONE = 0,
  SHADOW_KIND_TEXTURE,
  SHADOW_KIND_CUBE,

  ENUM_FORCE_DWORD(ShadowType)
};

//-----------------------------------------------------------------------------
/**
 *        ブロックイメージサイズ
*/
//-----------------------------------------------------------------------------
static const s32 DEFAULT_BLOCK_IMAGE_SIZE = (8);

//-----------------------------------------------------------------------------
/**
 *        画面サイズ定義
*/
//-----------------------------------------------------------------------------
#define   DISPLAY_UPPER_WIDTH   (400)
#define   DISPLAY_UPPER_HEIGHT  (240)
#define   DISPLAY_LOWER_WIDTH   (320)
#define   DISPLAY_LOWER_HEIGHT  (240)


//-----------------------------------------------------------------------------
/**
 *					型宣言
*/
//-----------------------------------------------------------------------------
typedef nw::ut::Color8      ColorU8;   // u8のときは0が黒&透明、255が白&不透明
typedef nw::ut::FloatColor  ColorF32;  // f32のときは0.0fが黒&透明、1.0fが白&不透明

extern ColorF32 ConvertColorU8ToColorF32(const ColorU8& c);
extern ColorU8  ConvertColorF32ToColorU8(const ColorF32& c);

// 線形補間を計算する
extern ColorF32* ColorF32Lerp(    // pOutを返す
    ColorF32*       pOut,         // 計算結果を受け取る。p1, p2と同じでも構わない。
    const ColorF32* p1,
    const ColorF32* p2,
    f32             t             // 0.0fであればp1が、1.0fであればp2が結果となる。
);

// スカラー倍を計算する
extern ColorF32* ColorF32Scale(    // pOutを返す
    ColorF32*       pOut,          // 計算結果を受け取る。pと同じでも構わない。
    const ColorF32* p,
    f32             scale          // 掛ける数
);

} /* end namespace grp */
} /* end namespace gfl */
#endif // __GFL_GRAPHICSTYPE_H__
