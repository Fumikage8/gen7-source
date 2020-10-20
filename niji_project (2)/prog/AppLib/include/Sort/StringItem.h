//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		StringItem.h
 *	@brief  道具名ソート
 *	@author	Toru=Nagihashi
 *	@date		2012.09.27
 *
 *
 * ・使い方
 *    StringSortBinary.hを御覧ください
 *
 *
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( STRING_ITEM_H_INCLUDED )
#define STRING_ITEM_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
// module
#include "StringBinary.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(sort)

class StringItem
  : public StringBinary
{
  GFL_FORBID_COPY_AND_ASSIGN(StringItem);
public:
  //-----------------------------------------------------------------------
  // 生成、破棄
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  StringItem();
  /**
   * @brief デストラクタ
   */
  virtual ~StringItem();
  
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
};


GFL_NAMESPACE_END(sort)
GFL_NAMESPACE_END(app)

#endif		// STRING_ITEM_H_INCLUDED
