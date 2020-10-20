//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		StringTokusei.h
 *	@brief  特性名名ソート
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
#if !defined( STRING_TOKUSEI_H_INCLUDED )
#define STRING_TOKUSEI_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
// pmlib
#include <pml/include/pmlib.h>
// module
#include "StringBinary.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(sort)

class StringTokusei
  : public StringBinary
{
  GFL_FORBID_COPY_AND_ASSIGN(StringTokusei);
public:
  //-----------------------------------------------------------------------
  // 生成、破棄
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  StringTokusei();
  /**
   * @brief デストラクタ
   */
  virtual ~StringTokusei();
  
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
   * @brief 隠す特性かどうかチェック
   *
   * @param tokusei  特性
   *
   * @return  trueならば隠す特性  falseならば隠さない特性
   */
  bool IsSecretTokusei( TokuseiNo tokusei ) const;
  
private:
  static const int SECRET_TOKUSEI_TABLE_MAX  = 8;
  static const TokuseiNo SECRET_TOKUSEI_TABLE[ SECRET_TOKUSEI_TABLE_MAX ];
};


GFL_NAMESPACE_END(sort)
GFL_NAMESPACE_END(app)

#endif		// STRING_TOKUSEI_H_INCLUDED
