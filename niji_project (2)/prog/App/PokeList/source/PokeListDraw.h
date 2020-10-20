#ifndef NIJI_PROJECT_APP_POKELIST_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_DRAW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListDraw.h
 * @author  fukushima_yuya
 * @date    2015.03.03
 * @brief   ポケモンリスト画面(フィールド用)
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListBaseDraw.h"
#include "App/PokeList/source/PokeListConst.h"


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class PokeExchange;     //!< ポケモン並び替え
class ItemExchange;     //!< アイテム並び替え
class FormChange;       //!< フォルムチェンジ
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_BEGIN(Tool)
class ItemEffect;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   ポケモンリスト描画クラス
//------------------------------------------------------------------------------
class PokeListDraw
  : public PokeListBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeListDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRanderingManager, App::Tool::ItemEffect* itemEffect );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListDraw( void );
  
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
   * @brief   キャンセルボタンの入力判定処理
   *
   * @param   touchPanel    タッチパネル
   */
  //------------------------------------------------------------------
  bool InputCancelBtn( gfl2::ui::TouchPanel* touchPanel );

  //------------------------------------------------------------------
  /**
   * @brief   キャンセルボタンの入力終了処理
   */
  //------------------------------------------------------------------
  void InputedCancelBtn( void );

  //------------------------------------------------------------------
  /**
   * @brief   キャンセルボタンの更新処理
   */
  //------------------------------------------------------------------
  void UpdateCancelBtn( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション(通常時)
   * @param   paneId  ペインID
   */
  //------------------------------------------------------------------
  void PaneEventNormal( const u32 paneId );

  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション(ポケモン入れ替え)
   * @param   paneId  ペインID
   */
  //------------------------------------------------------------------
  void PaneEventPokeExchange( const u32 paneId );

  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション(アイテム入れ替え)
   * @param   paneId  ペインID
   */
  //------------------------------------------------------------------
  void PaneEventItemExchange( const u32 paneId );

  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション(ウィンドウ)
   * @param   paneId  ペインID
   */
  //------------------------------------------------------------------
  void PaneEventWindow( const u32 paneId );


private:
  //------------------------------------------------------------------
  /**
   * @brief   タッチイベント(ポケモン入れ替え)
   * @param   pTouchPanel   タッチパネル
   */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult TouchEventNormal( gfl2::ui::TouchPanel* );

  //------------------------------------------------------------------
  /**
   * @brief   タッチイベント(ポケモン入れ替え)
   * @param   pTouchPanel   タッチパネル
   */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult TouchEventPokeExchange( gfl2::ui::TouchPanel* pTouchPanel );

  //------------------------------------------------------------------
  /**
   * @brief   タッチイベント(アイテム入れ替え)
   * @param   pTouchPanel   タッチパネル
   */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult TouchEventItemExchange( gfl2::ui::TouchPanel* pTouchPanel );

  //------------------------------------------------------------------
  /**
   * @brief   タッチイベント(ウィンドウ)
   * @param   pTouchPanel   タッチパネル
   */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult TouchEventWindow( gfl2::ui::TouchPanel* pTouchPanel );


private:   //!< リスナー処理
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
   * @brief   ペイン選択アニメーションの開始の検知
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual void OnLayoutPaneStartingAnimeEvent( const u32 paneId );

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


private:
  // ポケモン並び替え
  PokeExchange*             m_pPokeExchange;

  // アイテム並び替え
  ItemExchange*             m_pItemExchange;

  // フォルムチェンジ
  FormChange*               m_pFormChange;

  // フォルムチェンジエフェクト
  App::Tool::ItemEffect*    m_pItemEffect;

  gfl2::lyt::LytPane*       m_pVisiblePane[2];


  u8    m_CancelBtnState;

  bool  m_TouchEnable;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_DRAW_H_INCLUDED
