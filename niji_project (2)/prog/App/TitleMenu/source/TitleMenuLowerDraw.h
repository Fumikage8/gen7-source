#ifndef NIJI_PROJECT_APP_TITLE_MENU_LOWER_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_TITLE_MENU_LOWER_DRAW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file    TitleMenuLowerDraw.h
 * @date    2015/07/21 14:52:13
 * @author  fukushima_yuya
 * @brief   タイトルメニュー下画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/TitleMenu/include/TitleMenuAppParam.h"
#include "App/TitleMenu/source/TitleMenuConst.h"

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )
class DrawerBase;
GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )

//------------------------------------------------------------------------------
// @brief   タイトルメニュー下画面
//------------------------------------------------------------------------------
class TitleMenuLowerDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( TitleMenuLowerDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  TitleMenuLowerDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TitleMenuLowerDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam
   * @param   pAppLytBuff     レイアウトバイナリ
   * @param   msgData         メッセージデータ
   * @param   wordSet         ワードセット
   * @param   wordSetLoader   ワードセットローダー
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet, WordSetLoader* wordSetLoader );

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
  void TitleMenuOpen( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void TitleMenuClose( void );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @param   in    インアニメかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( bool in );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   appLytBuff    レイアウトバイナリ
   * @param   msgData       メッセージデータ
   * @param   wordSet       ワードセット
   */
  //------------------------------------------------------------------
  void Initialize2D( void* appLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );

  //------------------------------------------------------------------
  /**
   * @brief   ペインの初期化
   */
  //------------------------------------------------------------------
  void InitializePane( void );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージのセット
   *
   * @param   msgID   メッセージID
   */
  //------------------------------------------------------------------
  void SetMessageID( u32 msgID );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウの表示切替
   *
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisibleMsgWindow( bool visible );

private:
  //------------------------------------------------------------------
  /**
   * @brief   タイマーアイコンのアニメを再生
   */
  //------------------------------------------------------------------
  void StartTimeIcon( void );

  //------------------------------------------------------------------
  /**
   * @brief   タイマーアイコンのアニメの停止
   */
  //------------------------------------------------------------------
  void EndTimeIcon( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   セーブデータフォーマット用の表示
   */
  //------------------------------------------------------------------
  void VisibleSavedataFormat( void );


private:
  app::util::Heap*                  m_pHeap;        //!< ヒープ

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::Fade::FadeManager*          m_pFadeManager;

  APP_PARAM*                        m_pAppParam;

private:
  DrawerBase*   m_pDrawerBase;

private:
  u8    m_Seq;

private:
  gfl2::lyt::LytParts*      m_pPartsMsgWindow;

  gfl2::lyt::LytTextBox*    m_pTextMsgWindow;
};

GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_TITLE_MENU_LOWER_DRAW_H_INCLUDED