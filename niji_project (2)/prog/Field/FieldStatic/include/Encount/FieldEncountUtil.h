//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file FieldEncountUtil.h
 *  @brief エンカウント関連の便利関数
 *  @author saita_kazuki
 *  @date 2015.1.14
 */
//=============================================================================

#if !defined( FIELD_ENCOUNT_UTIL_H_INCLUDED )
#define FIELD_ENCOUNT_UTIL_H_INCLUDED
#pragma once

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// field
#include "Field/FieldRo/include/Encount/FieldEncountData.h"


// 前方参照
namespace GameSys {
  class GameManager;
}

namespace gfl2 {
  namespace fs {
    class AsyncFileManager;
  }
}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
 */
//-----------------------------------------------------------------------------

// エンカウントチェック
class EncountUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(EncountUtil); // コピーコンストラクタ＋代入禁止

public:

  //----------------------------------------------------------------------------
  /**
   * @brief シンボルエンカウントデータの同期読み込み
   * @param pHeap garc読み込みに使用するヒープ
   * @param pAsyncFileManager ファイル読み込み管理クラス
   * @param tableID シンボルエンカウントテーブルID
   * @param pOut パラメータの格納先
   */
  //-----------------------------------------------------------------------------
  static void SyncLoadSymbolEncountParam( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 tableID, SymbolEncountParam* pOut );
};

GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )

#endif  // FIELD_ENCOUNT_UTIL_H_INCLUDED

