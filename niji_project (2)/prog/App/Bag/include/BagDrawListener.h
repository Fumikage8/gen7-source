//=============================================================================
/**
 * @file    BagDrawListener.h
 * @brief   バッグ画面表示リスナー
 * @author  Hiroyuki Nakamura
 * @date    2015.03.04
 */
//=============================================================================
#if !defined( BAG_DRAW_LISTENER_H_INCLUDED )
#define BAG_DRAW_LISTENER_H_INCLUDED

#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)

//=============================================================================
/**
 * @class BagFrameListener
 * @brief バッグ画面表示リスナークラス
 * @date  2015.03.04
 */
//=============================================================================
class BagDrawListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @func    OnAction
   * @brief   入力リスナー
   * @date    2015.04.10
   *
   * @param   id  入力ID
   */
  //--------------------------------------------------------------------------
  virtual void OnAction( u32 id ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @func    UpdateCursorItem
   * @brief   カーソルの指すアイテムが変更されたときのリスナー
   * @date    2015.04.10
   *
   * @param   item  道具データ
   */
  //--------------------------------------------------------------------------
  virtual void UpdateCursorItem( const Savedata::MyItem::ITEM_ST * item ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @func    SelectItem
   * @brief   アイテムが選択されたときのリスナー
   * @date    2015.04.10
   *
   * @param   item  道具データ
   */
  //--------------------------------------------------------------------------
  virtual void SelectItem( Savedata::MyItem::ITEM_ST * item ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @func    CatchItem
   * @brief   ペインリストの項目キャッチ時のリスナー
   * @date    2015.05.12
   *
   * @param   item  道具データ
   */
  //--------------------------------------------------------------------------
  virtual void CatchItem( Savedata::MyItem::ITEM_ST * item ) = 0;
};

GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)

#endif  // BAG_DRAW_LISTENER_H_INCLUDED
