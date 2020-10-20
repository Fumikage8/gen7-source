#ifndef NIJI_PROJECT_APP_HERO_SELECT_UPPER_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_HERO_SELECT_UPPER_DRAW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file HeroSelectUpperDraw.h
 * @date 2015/08/26 12:07:37
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )
class HeroSelectDrawListener;
class HeroSelectCursor;
class YesNoWindow;
class DrawerBase;
class MenuCursor;
struct APP_PARAM;
GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_BEGIN( Tool )
class TalkWindow;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class KeyRepeatController;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( Savedata )
class MyStatus;
GFL_NAMESPACE_END( Savedata )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Fade )
class FadeManager;
GFL_NAMESPACE_END( Fade )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

//------------------------------------------------------------------------------
// @brief   容姿・性別選択上画面描画
//------------------------------------------------------------------------------
class HeroSelectUpperDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( HeroSelectUpperDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  HeroSelectUpperDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager, MenuCursor* pCursor );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~HeroSelectUpperDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam       外部設定パラメータ
   * @param   pAppLytBuff     レイアウトバイナリ
   * @param   pAppWndLytBuff  レイアウトバイナリ
   * @param   msgData         メッセージデータ
   * @param   wordSet         ワードセット
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* pAppParam, void* pAppLytBuff, void* pAppWndLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );

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
  void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @param   in    インアニメかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( bool in );

  //------------------------------------------------------------------
  /**
   * @brief	  このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( HeroSelectDrawListener* pListener );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pAppLytBuff     レイアウトバイナリ
   * @param   pAppWndLytBuff  レイアウトバイナリ
   */
  //------------------------------------------------------------------
  void Initialize2D( void* pAppLytBuff, void* pAppWndLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンス：準備
   */
  //------------------------------------------------------------------
  bool MainSequenceReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   サブシーケンス：初期化
   */
  //------------------------------------------------------------------
  void SubSequenceInit( void );

  //------------------------------------------------------------------
  /**
   * @brief   サブシーケンス：処理
   */
  //------------------------------------------------------------------
  void SubSequenceMain( void );

  //------------------------------------------------------------------
  /**
   * @brief   サブシーケンス：解放
   */
  //------------------------------------------------------------------
  void SubSequenceTerm( void );

  //------------------------------------------------------------------
  /**
   * @brief   着せ替えデータのセット
   * @param   index   容姿選択のインデックス
   */
  //------------------------------------------------------------------
  void SetDressUpParam( u32 index );

public:   //!< リスナー
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
  app::util::Heap*                  m_pHeap;        //!< ヒープ
  HeroSelectDrawListener*           m_uiListener;   //!< リスナー

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::Fade::FadeManager*          m_pFadeManager;

  gfl2::str::MsgData*               m_pMsgData;

  print::WordSet*                   m_pWordSet;

  APP_PARAM*                        m_pAppParam;

  u8    m_Seq;

private:
  DrawerBase*               m_pDrawerBase;

  YesNoWindow*              m_pYesNoWindow;

  MenuCursor*               m_pCursor;

  App::Tool::TalkWindow*    m_pTalkWindow;

  Savedata::MyStatus*       m_pMyStatus;

  u8  m_SubSeq;

  u8  m_DispMode;

  u32 m_CursorPos;

  bool  m_DecideTrigger;

  gfl2::lyt::LytPane*       m_pHeroAll;
  gfl2::lyt::LytPane*       m_pSelectHero;
  gfl2::lyt::LytPane*       m_pWindowPane;

  app::util::KeyRepeatController*   m_pKeyRepeat;
};

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_HERO_SELECT_UPPER_DRAW_H_INCLUDED