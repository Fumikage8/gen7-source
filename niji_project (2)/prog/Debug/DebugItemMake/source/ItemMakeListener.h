#if PM_DEBUG
//=============================================================================
/**
 * @file    ItemMakeListener.h
 * @brief   デバッグ用アイテム作成リスナー
 * @author  Hiroyuki Nakamura
 * @date    2015.05.26
 */
//=============================================================================
#if !defined( DEBUG_ITEM_MAKE_LISTENER_H_INCLUDED )
#define DEBUG_ITEM_MAKE_LISTENER_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(Debug)

//=============================================================================
/**
 * @class ItemMakeListener
 * @brief フィールドメニューリスナークラス
 * @date  2015.05.26
 */
//=============================================================================
class ItemMakeListener
{
public:
  //! 入力ID
  enum ActionID
  {
    ACTION_INPUT_NAME = 0,  //!< 道具名入力
    ACTION_INPUT_ITEM_ID,   //!< 道具ID入力
    ACTION_INPUT_NUM,       //!< 個数入力
    ACTION_INPUT_ENTER,     //!< 決定
    ACTION_ADD_ITEM_ONE,    //!< 道具１個追加
    ACTION_ADD_ITEM_MAX,    //!< 道具９９９個追加
    ACTION_EXIT,            //!< 終了
  };


public:
  //--------------------------------------------------------------------------
  /**
   * @func    OnAction
   * @brief   入力リスナー
   * @date    2015.05.26
   *
   * @param   id  入力ID
   */
  //--------------------------------------------------------------------------
  virtual void OnAction( u32 id ) = 0;
};

GFL_NAMESPACE_END(Debug)

#endif  // DEBUG_ITEM_MAKE_LISTENER_H_INCLUDED

#endif  // PM_DEBUG
