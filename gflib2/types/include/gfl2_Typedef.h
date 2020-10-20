//===========================================================================
/**
 * @file    gfl2_Typedef.h
 * @brief   各種基本型定義
 * @author  GAMEFREAK inc.
 * @date    2014

 * 2010 kujiraプロジェクトで原型作成(gfl_Typedef.h)
 * 2014 Win/Platform対応のためnijiプロジェクトで整形
 * 2016.01.22 tamada  niji完成に向けて整理

 * 3DSとWin32（VisualStudio）以外は挙動確認されていない。
 * 移植時にはwchar_tが4バイトの処理系などもあるので注意すること。
 */
//===========================================================================

 //////////////// 次の型を使えるようにしておく ////////////////

 //// 符号あり整数 ////
 // s8  s16  s32  s64

 //// 符号なし整数 ////
 // u8  u16  u32  u64

 //// 浮動小数 ////
 // f16    u16
 // f32    float
 // f64    double

 //// ビット ////
 // bit8   u8
 // bit16  u16
 // bit32  u32
 // bit64  u64

 //// bool型 ////
 // b8     bool型はコンパイラによって大きさが変わる為 true falseを厳格に扱うために定義    s8
 // b32    NITROARMのboolと同じですので 受ける場合にはこちらを使ってください              s32

 //// 文字 ////
 // c8     8ビットの文字変数    char
 // c16    16ビットの文字変数   wchar_t

 //// アドレス ////
 // uptr   符号なしアドレス整数値型です。

 ////////////////  ////////////////

#pragma once
#ifndef GFL2_TYPEDEF_H
#define GFL2_TYPEDEF_H

#ifdef GF_PLATFORM_CTR
//-------------------------------------------------------------
// 3DSの場合
//-------------------------------------------------------------

///< new[]で、余分に確保されるメモリサイズ
const int NEW_ARRAY_MEMORYCONTORL_SECTION_SIZE= 8;

#include <nn/types.h>

//typedef u16                 f16;  //niji未使用

typedef s8                  b8;
typedef s32                 b32;

typedef char                c8;
typedef wchar_t             c16;


#elif defined( GF_PLATFORM_WIN32 )
//-------------------------------------------------------------
// WIN32の場合
//-------------------------------------------------------------

///< new[]で、余分に確保されるメモリサイズ
const int NEW_ARRAY_MEMORYCONTORL_SECTION_SIZE=4;

#include <crtdefs.h>

typedef __int8              s8;
typedef __int16             s16;
typedef __int32             s32;
typedef __int64             s64;
typedef unsigned __int8     u8;
typedef unsigned __int16    u16;
typedef unsigned __int32    u32;
typedef unsigned __int64    u64;
typedef float               f32;
typedef double              f64;


typedef s8                  b8;
typedef s32                 b32;

typedef u8                  bit8;
//typedef u16                 bit16;  //niji未使用
typedef u32                 bit32;
typedef u64                 bit64;

//typedef unsigned short      f16;    //niji未使用
typedef char                c8;
typedef wchar_t             c16;

//! 符号なしアドレス整数値型です。
typedef uintptr_t           uptr;



#else   /* !defined(GF_PLATFORM_*) */
//-------------------------------------------------------------
// プラットフォーム定義がない場合はエラーにする
//-------------------------------------------------------------
#error GF_PLATFORM IS NOT DEFINED!!

#endif  /* GF_PLATFORM_* */

#endif // !GFL2_TYPEDEF_H

