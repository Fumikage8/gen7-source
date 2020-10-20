//=============================================================================
/**
 * @file    ShopDrawListener.h
 * @brief   ショップ購入画面描画リスナー
 * @author  Hiroyuki Nakamura
 * @date    2015.10.22
 */
//=============================================================================
#if !defined( SHOP_DRAW_LISTENER_H_INCLUDED )
#define SHOP_DRAW_LISTENER_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Shop)


//=============================================================================
/**
 * @class ShopDrawListener
 * @brief ショップ購入画面描画リスナークラス
 * @date  2015.10.02
 */
//=============================================================================
class ShopDrawListener
{
public:
  //! 動作ID
  enum ActionID
  {
    ACTION_ID_CANCEL = 0,     //!< キャンセル
    ACTION_ID_SELECT,         //!< 選択

    ACTION_ID_KEY_UP,         //!< 上キー
    ACTION_ID_KEY_DOWN,       //!< 下キー
    ACTION_ID_KEY_LEFT,       //!< 左キー
    ACTION_ID_KEY_RIGHT,      //!< 右キー

    ACTION_ID_KEY_UP_TRG,     //!< 上キー（トリガー入力）
    ACTION_ID_KEY_DOWN_TRG,   //!< 下キー（トリガー入力）
    ACTION_ID_KEY_LEFT_TRG,   //!< 左キー（トリガー入力）
    ACTION_ID_KEY_RIGHT_TRG,  //!< 右キー（トリガー入力）
  };

public:
  //-----------------------------------------------------------------------------
  /**
   * @func    OnAction
   * @brief   入力時のリスナー
   * @date    2015.10.02
   *
   * @param   id    動作ID
   */
  //-----------------------------------------------------------------------------
  virtual void OnAction( ActionID id ) = 0;
};


GFL_NAMESPACE_END(Shop)
GFL_NAMESPACE_END(App)

#endif  // SHOP_DRAW_LISTENER_H_INCLUDED
