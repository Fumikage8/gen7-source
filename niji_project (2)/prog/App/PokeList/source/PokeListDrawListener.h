#ifndef NIJI_PROJECT_APP_POKELIST_DRAW_LISTENER_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_DRAW_LISTENER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListDrawListener.h
 * @author  fukushima_yuya
 * @date    2015.03.03
 * @brief   描画からファンクションへリスナー
 */
//==============================================================================

// niji
#include "App/PokeList/include/PokeListSetupParam.h"
#include "App/PokeList/source/PokeListConst.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class CursorDraw;
class MenuWindowDraw;
class StaticWork;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   ポケモンリスト用のリスナー
//------------------------------------------------------------------------------
class PokeListDrawListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンリストの終了
   */
  //--------------------------------------------------------------------------
  virtual void PokeListEnd( void ) { ; }

  //--------------------------------------------------------------------------
  /**
   * @brief   外部設定パラメータの設定
   * @param   endMode   終了モード
   */
  //--------------------------------------------------------------------------
  virtual void SetupOutData( EndMode endMode ) { ; }

  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンステータスの呼び出し
   * @param   pokeIndex   ポケモンのインデックス
   */
  //--------------------------------------------------------------------------
  virtual void CallPokeStatus( u8 pokeIndex ) { ; }

  //--------------------------------------------------------------------------
  /**
   * @brief   バッグの呼び出し
   * @param   pokeIndex   ポケモンのインデックス
   */
  //--------------------------------------------------------------------------
  virtual void CallBag( u8 pokeIndex ) { ; }

  //--------------------------------------------------------------------------
  /**
   * @brief   バッグ(かいふく)の呼び出し
   * @param   pokeIndex   ポケモンのインデックス
   */
  //--------------------------------------------------------------------------
  virtual void CallBagRepair( u8 pokeIndex ) { ; }

  //--------------------------------------------------------------------------
  /**
   * @brief   参加選択で自分の選択を終えた
   */
  //--------------------------------------------------------------------------
  virtual void FinishJoinSelect( void ) { ; }

public:
  //--------------------------------------------------------------------------
  /**
   * @brief   描画許可フラグの有効化
   */
  //--------------------------------------------------------------------------
  virtual void SetEnableFlag_Draw( void ) { ; }

  //--------------------------------------------------------------------------
  /**
   * @brief   描画許可フラグの取得
   *
   * @return  "true  == 有効"
   * @return  "false == 無効"
   */
  //--------------------------------------------------------------------------
  virtual bool GetEnableFlag_Draw( void ) { return true; }
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_DRAW_LISTENER_H_INCLUDED
