﻿#ifndef NIJI_PROJECT_FIELD_FINDER_DRAW_LISTENER_H_INCLUDED
#define NIJI_PROJECT_FIELD_FINDER_DRAW_LISTENER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    FinderDrawListener.h
 * @author  fukushima_yuya
 * @date    2015.05.26
 * @brief   描画からファンクションへリスナー
 */
//==============================================================================

// niji
#include <App/Finder/include/FinderAppParam.h>
#include <AppLib/include/Frame/AppFrameCellSuper.h>

// gflib2
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const u32 INIT_SHUTTER_COUNT = 6;

//------------------------------------------------------------------------------
// @brief   ポケファインダー画面用のリスナー
//------------------------------------------------------------------------------
class FinderDrawListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   終了モードの設定
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void SetEndMode( EndMode endMode ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   ポケファインダーUI画面の操作開始
   */
  //------------------------------------------------------------------
  virtual void FinderStart( void ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   ポケファインダーUI画面を閉じる
   */
  //------------------------------------------------------------------
  virtual void FinderClose( void ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   シャッターアニメの開始
   */
  //------------------------------------------------------------------
  virtual void StartShutterAnim( void ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   シャッターアニメの終了
   */
  //------------------------------------------------------------------
  virtual void EndShutterAnim( void ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   照準の変更
   */
  //------------------------------------------------------------------
  virtual void ChangeSight( void ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   ピントの変更
   * @param   mode    ピントのモード
   */
  //------------------------------------------------------------------
  virtual void SetPintMode( u8 mode ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   システムメッセージの表示
   * @param   msgID     メッセージID
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void OpenSystemMessage( u32 msgID, EndMode endMode ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   ファインダーの終了
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void FinishFinder( EndMode endMode ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   情報表示の切り替え
   */
  //------------------------------------------------------------------
  virtual void ChangeVisibleInfo( void ) { ; }
};

GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )

#endif  //#ifndef NIJI_PROJECT_FIELD_FINDER_DRAW_LISTENER_H_INCLUDED
