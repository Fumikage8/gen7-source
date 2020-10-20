#ifndef __GFL_MACROS_H__
#define __GFL_MACROS_H__
//===========================================================================
/**
 * @file    gfl_Macros.h
 * @brief   GAMEFREAK Library / Utility Macros
 * @author  GAMEFREAK inc.
 * @date    2010.10.29
 */
//===========================================================================
#pragma once

//-----------------------------------------------------------------------------
/**
 *          マクロ定義
*/
//-----------------------------------------------------------------------------

/// 使用しない引き数に対して使うマクロ（#pragma unusedの代わり）
#define GFL_UNUSED(a) static_cast<void>(a)

///コピーコンストラクタ＋代入演算子を禁止する
#define GFL_FORBID_COPY_AND_ASSIGN(TypeName)  \
  private: \
    TypeName( const TypeName& ); \
    void operator= (const TypeName&)

/// 配列の要素数
#define GFL_NELEMS(ary) (sizeof(ary)/sizeof(ary[0]))
#define elementof(ary) (sizeof(ary)/sizeof(ary[0]))

/// enumのサイズを DWORD （４バイト）に統一する
#define  ENUM_FORCE_DWORD(name) name##_FORCE_DWORD = 0x7FFFFFFF

/// bool型はコンパイラによって大きさが変わる為 true falseを厳格に扱うために定義
typedef unsigned char b8;    ///< 小さくする場合はこちらを使用してください
typedef signed long b32;     ///< NITROARMのboolと同じですので 受ける場合にはこちらを使ってください


/// スタティックアサート
#define STATIC_ASSERT(expr) { extern char dummy[(expr) ? 1 : -1]; }




#endif // __GFL_MACROS_H__
