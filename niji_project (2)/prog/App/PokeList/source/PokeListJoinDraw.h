#ifndef NIJI_PROJECT_APP_POKELIST_JOIN_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_JOIN_DRAW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListJoinDraw.h
 * @author  fukushima_yuya
 * @date    2015.03.25
 * @brief   ポケモンリスト画面参加選択描画
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListBaseDraw.h"
#include "App/PokeList/source/PokeListConst.h"

#include <Battle/Regulation/include/PokeRegulation.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   ポケモンリスト描画クラス
//------------------------------------------------------------------------------
class PokeListJoinDraw
  : public PokeListBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListJoinDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeListJoinDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListJoinDraw( void );
  
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
   * @brief   UIのセットアップ完了待ち
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  virtual bool IsSetupUIReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   メニューIDの取得
   * @param   cpos    プレート位置
   * @return  MENU_JOIN           参加可で未参加
   *          MENU_JOIN_ALREADY   参加可で参加済み
   *          MENU_JOIN_DISABLE   参加不可
   */
  //------------------------------------------------------------------
  MenuID GetMenuID( CursorPos cpos );

  //------------------------------------------------------------------
  /**
   * @brief   レギュレーションNGメッセージの取得
   *
   * @param   result    レギュレーション結果
   *
   * @return  メッセージID
   */
  //------------------------------------------------------------------
  s32 GetRegulationNGMsgID( PokeRegulation::PokelistCheckReturn result );

  //------------------------------------------------------------------
  /**
   * @brief   参加ポケモン数、参加可能ポケモン数の表示
   */
  //------------------------------------------------------------------
  void UpdateJoinCount( void );

  //------------------------------------------------------------------
  /**
   * @brief   キーアクション
   * @param   pButton
   */
  //------------------------------------------------------------------
  void KeyActionNormal( gfl2::ui::Button* pButton );

  //------------------------------------------------------------------
  /**
   * @brief   レギュレーションチェック
   *
   * @return  "true  == 問題なし"
   * @return  "false == レギュレーション違反"
   */
  //------------------------------------------------------------------
  bool CheckRegulation( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   参加ポケモンの決定
   */
  //------------------------------------------------------------------
  void DecideJoinPoke( void );

  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンスかどうか
   *
   * @return  "true  = メインシーケンス"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsSeqMain( void ) const { return (m_Seq == SEQ_MAIN); }

public:   //!< リスナー処理
  //------------------------------------------------------------------
  /**
   * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
   */
  //------------------------------------------------------------------
  virtual void OnPreLoadResource( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   タッチパネルイベントの検知
   * @param   pTouchPanel
   * @param   isTouch
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

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

public:   //!< ボタンマネージャ関連
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );

private:
  u8    m_JoinedCount;

  gfl2::lyt::LytParts*      m_pDecideBtnParts;

  gfl2::lyt::LytTextBox*    m_pJoinCountText;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_JOIN_DRAW_H_INCLUDED
