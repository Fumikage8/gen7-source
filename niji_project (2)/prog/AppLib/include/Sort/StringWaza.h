//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		StringWaza.h
 *	@brief  技名ソート
 *	@author	Toru=Nagihashi
 *	@date		2012.09.27
 *
 *
 * ・使い方
 *    StringSortBinary.hを御覧ください
 *
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( STRING_WAZA_H_INCLUDED )
#define STRING_WAZA_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
// pmlib
#include <pml/include/pmlib.h>
// module
#include "StringBinary.h"

/*
#include <gflib.h>
#include <pmlib.h>
#include <system/sort_string/StringSortBinary.h>

#include <system/main.h>
*/

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(sort)

class StringWaza
  : public StringBinary
{
  GFL_FORBID_COPY_AND_ASSIGN(StringWaza);
public:
  //-----------------------------------------------------------------------
  // 生成、破棄
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  StringWaza();
  /**
   * @brief デストラクタ
   */
  virtual ~StringWaza();
  
  //-----------------------------------------------------------------------
  // 読み込み
  //-----------------------------------------------------------------------
  /**
   * @brief 同期読み込み
   *
   * @param heap  ヒープ
   */
  void Load( gfl2::heap::HeapBase * heap );

  /**
   * @brief 非同期読み込み設定
   *
   * @param heap  ヒープ
   * @param work_heap 作業用ヒープ
   *
   * @note  LoadMain()で読み込んでください
   */
  void LoadSetup( gfl2::heap::HeapBase * heap, gfl2::heap::HeapBase * work_heap );

  //-----------------------------------------------------------------------
  // 取得
  //  取得系関数はStringSortBinaryクラスを継承しているので
  //  以下のものが使えます。
  //  u32 GetSortOrder( u32 index ) const;
  //  u32 GetSortOrderMax( void ) const;
  //  u32 GetInitialIndex( u32 index ) const;
  //  u32 GetInitialIndexMax( void ) const;
  //  bool GetInitialIndexToSortHeaderIndex( u32 initial_index, u32 & start, u32 & end ) const;
  //-----------------------------------------------------------------------
  /**
   * @brief 隠す技かどうかチェック
   *
   * @param waza  技
   *
   * @return  trueならば隠す技  falseならば隠さない技
   */
  bool IsSecretWaza( WazaNo waza ) const;
  
private:
  static const int SECRET_WAZA_TABLE_MAX  = 13;
  static const WazaNo SECRET_WAZA_TABLE[ SECRET_WAZA_TABLE_MAX ];

  static const int SECRET_Z_WAZA_TABLE_MAX = 53;  // Z技テーブル
  static const WazaNo SECRET_Z_WAZA_TABLE[ SECRET_Z_WAZA_TABLE_MAX ];
};


GFL_NAMESPACE_END(sort)
GFL_NAMESPACE_END(app)

#endif		// STRING_WAZA_H_INCLUDED
