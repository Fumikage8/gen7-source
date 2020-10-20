#ifndef __GFL2_GRAPHICSTYPE_H__
#define __GFL2_GRAPHICSTYPE_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl2_GraphicsType.h
 *  @brief  グラフィックス環境　共通型、定数定義
 *  @date   2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#if defined(GF_PLATFORM_WIN32)
#include <nw/ut.h>
#elif defined(GF_PLATFORM_CTR)
#include <nn.h>
#include <nw/ut.h>
#endif

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)

//-----------------------------------------------------------------------------
/**
 *          デバッグ用
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
 *          定数宣言
 */
//-----------------------------------------------------------------------------
// メモリタイプ
enum MemoryArea
{
  MEMORY_AREA_NONE,
#if defined(GF_PLATFORM_WIN32)
  MEMORY_AREA_FCRAM = 0x00010000,   //NN_GX_MEM_FCRAM,  //!< FCRAMの領域を表します。
  MEMORY_AREA_VRAMA = 0x00020000,   //NN_GX_MEM_VRAMA,  //!< VRAMのAチャンネルの領域を表します。
  MEMORY_AREA_VRAMB = 0x00030000,   //NN_GX_MEM_VRAMB,  //!< VRAMのBチャンネルの領域を表します。
#elif defined(GF_PLATFORM_CTR)
  MEMORY_AREA_FCRAM = NN_GX_MEM_FCRAM,                  //!< FCRAMの領域を表します。
  MEMORY_AREA_VRAMA = NN_GX_MEM_VRAMA,                  //!< VRAMのAチャンネルの領域を表します。
  MEMORY_AREA_VRAMB = NN_GX_MEM_VRAMB,                  //!< VRAMのBチャンネルの領域を表します。
#endif

  ENUM_FORCE_DWORD(MemoryArea)
} ;

// 表示面定義
typedef enum
{
  DISPLAY_UPPER = 0,                      // 上画面　or 左画面
  DISPLAY_LOWER,                          // 下画面
  DISPLAY_UPPER_RIGHT,                    // 右画面

  DISPLAY_BOTH_MAX,                       // 全画面の数
  DISPLAY_NONE,                           // 対応する画面なし。 
  DISPLAY_BOTH,   //上下両方

  DISPLAY_NORMAL_MAX = DISPLAY_UPPER_RIGHT, // 立体視なしの画面数。


  ENUM_FORCE_DWORD(DisplayType)
} DisplayType;

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
 *          型宣言
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

GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)
#endif // __GFL2_GRAPHICSTYPE_H__
