#ifndef NIJI_PROJECT_APP_POKELIST_FRAME_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_FRAME_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListFrame.h
 * @author  fukushima_yuya
 * @date    2015.03.03
 * @brief   ポケモンリスト画面テストフレーム
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListBaseFrame.h"

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class PokeListDraw;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_BEGIN( Tool )
class ItemEffect;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class AppCamera;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   フィールド用ポケモンリストフレーム
//------------------------------------------------------------------------------
class PokeListFrame
  : public PokeListBaseFrame
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeListFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListFrame();

public:
  //------------------------------------------------------------------
  /**
   * @brief   PokeListのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupPokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの生成
   */
  //------------------------------------------------------------------
  virtual void CreatePokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの破棄
   */
  //------------------------------------------------------------------
  virtual bool EndPokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの更新
   */
  //------------------------------------------------------------------
  virtual void UpdatePokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの描画
   */
  //------------------------------------------------------------------
  virtual void DrawPokeList( gfl2::gfx::CtrDisplayNo no );

private:
  //------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジエフェクトの生成
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CreateItemEffect( void );

  //------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジエフェクトの破棄
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool DeleteItemEffect( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   AppCameraの生成
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CreateAppCamera( void );

  //------------------------------------------------------------------
  /**
   * @brief   AppCameraの破棄
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool DeleteAppCamera( void );

public: // PokeListDrawListener
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリストの終了
   */
  //------------------------------------------------------------------
  virtual void PokeListEnd( void );

  //------------------------------------------------------------------
  /**
   * @brief   外部設定パラメータの設定
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void SetupOutData( EndMode endMode );


private:
  PokeListDraw*             m_pPokeListDraw;      //!< フィールド用ポケモンリストドロー

  App::Tool::ItemEffect*    m_pItemEffect;        //!< フォルムチェンジエフェクト

  app::util::AppCamera*     m_pAppCamera;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_FRAME_H_INCLUDED
