#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFORMORDER_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFORMORDER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFormOrder.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.03.26
 @brief   図鑑におけるフォルムの特別な並び順
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// pmlibのインクルード
#include <pml/include/pmlib.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include "ZukanType.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFormOrder
 @brief     図鑑におけるフォルムの特別な並び順
 */
//==============================================================================
class ZukanFormOrder
{
public:
  //! @brief  フォルム数の最大数
  static const u8 FORM_NUM_MAX = 5;  // このクラスで扱うポケモンにおけるフォルム数の最大数。全ポケモンにおけるフォルム数の最大数ではない。

  //! @brief  フォルムオーダーのタイプ
  enum FormOrderType
  {
    FORM_ORDER_TYPE_FORM_NO    = 0,   //!< フォルム番号
    FORM_ORDER_TYPE_ALOLA      = 1,   //!< アローラ種
    FORM_ORDER_TYPE_ISLAND     = 2,   //!< 島ごとに生息
    FORM_ORDER_TYPE_DIRECTION  = 3,   //!< 方角
    FORM_ORDER_TYPE_OTHER      = 4,   //!< その他
    FORM_ORDER_TYPE_NUM        = 5,   //!< 個数
  };

  //! @brief  フォルムオーダー
  struct FormOrder
  {
    u8  formNo[FORM_NUM_MAX];  // pml::FormNo  // [0]～[formNum-1]にフォルム番号が入っている
    u8  formOrderType;         // FormOrderType
    u8  formNum;               // フォルム数
    u8  specialFormNum;        // 特別扱いするフォルムの数。[0]～[specialFormNum-1]を特別扱いする。特別扱いがなければ0。
                               // nijiにおいては、nijiにしか出ないフォルムを持つポケモンにはnijiにしか出ないフォルム数を入れておく。
  };

  //! @brief  フォルムオーダーを取得する
  //! @param[in]  monsNo     フォルムオーダーを取得したいポケモン
  //! @param[out] formOrder  取得した値を代入するフォルムオーダー
  //! @return  trueのときformOrderが有効なので、この順番に出すこと。falseのときformOrderが無効なので、pml::FormNo順に出せばよい。
  //!          trueのときformOrder->formOrderType順。falseのときFORM_ORDER_TYPE_FORM_NO順。
  static bool GetFormOrder(const MonsNo monsNo, FormOrder* formOrder);
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFORMORDER_H_INCLUDED

