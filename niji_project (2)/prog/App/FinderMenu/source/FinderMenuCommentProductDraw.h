#if !defined( __FINDER_MENU_COMMENT_PRODUCT_DRAW_H__ )
#define __FINDER_MENU_COMMENT_PRODUCT_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    FinderMenuCommentProductDraw.h
 * @date    2015/11/02 19:16:17
 * @author  fukushima_yuya
 * @brief   評価演出画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/FinderMenu/include/FinderMenuAppParam.h>

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)
class FinderMenuDrawListener;
class DrawerBase;
GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   写真選択画面の描画
//------------------------------------------------------------------------------
class FinderMenuCommentProductDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderMenuCommentProductDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderMenuCommentProductDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderMenuCommentProductDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( FinderMenuDrawListener* pListener );

  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   appParam      外部設定パラメータ
   * @param   pAppLytBuff   レイアウトバイナリ
   * @param   msgData       メッセージデータ
   * @param   wordSet       ワードセット
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* appParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  bool End( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   破棄チェック
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void );

  //------------------------------------------------------------------
  /**
   * @brief   オープン
   */
  //------------------------------------------------------------------
  void Open( u32 photoIdx );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void Close( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pHeap         ヒープ
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   */
  //------------------------------------------------------------------
  void Setup( u32 photoIdx );

  //------------------------------------------------------------------
  /**
   * @brief   コメントSEの再生判定
   */
  //------------------------------------------------------------------
  bool IsPlayCommentSE( void );

  //------------------------------------------------------------------
  /**
   * @brief   コメントSEの再生
   */
  //------------------------------------------------------------------
  void PlayCommentSE( void );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   キー入力通知用リスナ
   * @param   pKey
   * @param   pStick
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  //------------------------------------------------------------------
  /**
   * @brief   タッチパネルイベントの検知
   * @param   pTouchPanel
   * @param   isTouch
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

private:
  static const u32 COMMENT_MAX = 15;

private:
  app::util::Heap*                  m_pHeap;        //!< ヒープ

  FinderMenuDrawListener*           m_uiListener;   //!< UIイベントリスナー

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::str::MsgData*               m_pMsgData;

  print::WordSet*                   m_pWordSet;

  APP_PARAM*                        m_pAppParam;

  u8    m_Seq;

private:
  DrawerBase*     m_pDrawerBase;

  u32             m_CommentAnim;

  u32             m_CommentCount;

  gfl2::lyt::LytTextBox*      m_pScoreText;

  gfl2::lyt::LytTextBox*      m_pCommentText[COMMENT_MAX];
};

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

#endif // __FINDER_MENU_COMMENT_PRODUCT_DRAW_H__