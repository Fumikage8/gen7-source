#ifndef NIJI_PROJECT_APP_POKELIST_SIMPLE_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_SIMPLE_DRAW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListSimpleDraw.h
 * @author  fukushima_yuya
 * @date    2015.03.23
 * @brief   ポケモンリスト画面シンプル描画(バトル・対象選択)
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListBaseDraw.h"
#include "App/PokeList/source/PokeListConst.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   ポケモンリスト描画クラス
//------------------------------------------------------------------------------
class PokeListSimpleDraw
  : public PokeListBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListSimpleDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeListSimpleDraw( app::util::Heap * heap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListSimpleDraw( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  virtual bool EndFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   */
  //------------------------------------------------------------------
  virtual void UpdateFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画処理
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   UIのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupUI( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupButton( app::tool::ButtonManager* pBtnMan );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   */
  //------------------------------------------------------------------
  void UpdateFunc_Simple( void );             //!< MODE_SIMPLE, MODE_SIMPLE2
  void UpdateFunc_Battle( void );             //!< MODE_BATTLE

  void UpdateFunc_BattleFromTop( void );      //!< バトルトップ -> ポケモンリスト
  void UpdateFunc_BattleFromStatus( void );   //!< ステータス -> ポケモンリスト

  bool UpdateFunc_SeqReady( void );           //!< 準備シーケンス
  bool UpdateFunc_SeqFadeIn( void );          //!< フェードインシーケンス
  bool UpdateFunc_SeqFadeOut( void );         //!< フェードアウトシーケンス
  bool UpdateFunc_SeqEndReady( void );        //!< 終了準備シーケンス

private:
  //------------------------------------------------------------------
  /**
   * @brief   キー入力
   *
   * @param   pButton
   */
  //------------------------------------------------------------------
  void KeyActionNormal( gfl2::ui::Button* pButton );

public:
  //------------------------------------------------------------------
  /**
   * @brief   シーケンスがメイン状態かどうか
   *
   * @return  "true  = メイン"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsSeqMain( void ) const;

public:   //!< リスナー処理
  //------------------------------------------------------------------
  /**
   * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
   */
  //------------------------------------------------------------------
  virtual void OnPreLoadResource( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

  //------------------------------------------------------------------
  /**
   * @brief   キー入力通知用リスナ
   * @param   pKey
   * @param   pStick
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_SIMPLE_DRAW_H_INCLUDED
