#if !defined( __APP_BOX_SEARCH_DRAW_LISTENER_H_INCLUDED__ )
#define __APP_BOX_SEARCH_DRAW_LISTENER_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    BoxSearchDrawListener.h
 * @date    2015/07/27 12:57:54
 * @author  fukushima_yuya
 * @brief   ボックス検索：描画リスナー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/BoxSearch/include/BoxSearchAppParam.h>

#include "BoxSearchConst.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//------------------------------------------------------------------------------
// @brief   DrawListenerクラス
//------------------------------------------------------------------------------
class BoxSearchDrawListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @brief   画面の終了通知
   */
  //--------------------------------------------------------------------------
  virtual void EndBoxSearch( void ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   終了モードのセット
   * @param   endMode   終了モード
   */
  //--------------------------------------------------------------------------
  virtual void SetEndMode( EndMode endMode ) = 0;

public:
  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(検索項目リスト用)
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_ItemList( void ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(頭文字選択用)
   *
   * @param   listID    リストの種類
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_Initial( u32 listID ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(ガイド用)
   *
   * @param   listID    リストの種類
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_Guide( u32 listID ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(タイプ用)
   *
   * @param   listID  リストの種類
   * @param   type1   タイプ１
   * @param   type2   タイプ２
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_Type( u32 listID, u8 type1, u8 type2 ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(特性用)
   *
   * @param   tokusei   特性No
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_Tokusei( TokuseiNo tokusei ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(性格用)
   *
   * @param   seikaku   性格ID
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_Seikaku( pml::pokepara::Seikaku seikaku ) { ; }
};

GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)


#endif // __APP_BOX_SEARCH_DRAW_LISTENER_H_INCLUDED__