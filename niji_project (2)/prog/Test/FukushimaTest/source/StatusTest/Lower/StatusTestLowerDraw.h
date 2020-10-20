#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_TEST_STATUS_LOWER_DRAW_H__
#define __TEST_FUKUSHIMA_TEST_STATUS_LOWER_DRAW_H__
#pragma once
//==============================================================================
/**
 * @file    StatusTestLowerDraw.h
 * @author  fukushima_yuya
 * @date    2015.06.26
 * @brief   ステータス下画面描画
 */
//==============================================================================

// niji
#include <App/Status/include/StatusAppParam.h>
//#include <App/Status/source/StatusConst.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Status )
class StatusDrawListener;
class DrawerBase;
GFL_NAMESPACE_END( Status )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   ステータス下画面描画クラス
//------------------------------------------------------------------------------
class TestStatusLowerDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( TestStatusLowerDraw );

public:
  enum Sequence {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  TestStatusLowerDraw( app::util::Heap * heap, app::util::AppRenderingManager * renderingManager );

  //------------------------------------------------------------------
  /**
   * @brief	  デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TestStatusLowerDraw( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( App::Status::StatusDrawListener* pListener );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam
   * @param   pAppLytBuff   レイアウトバイナリ
   */
  //------------------------------------------------------------------
  void Init( App::Status::APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet );

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
   * @brief   ステータス画面のオープン
   */
  //------------------------------------------------------------------
  void StatusOpen( void );

  //------------------------------------------------------------------
  /**
   * @brief   ステータス画面のクローズ
   */
  //------------------------------------------------------------------
  void StatusClose( void );

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
   * @param   pHeap         ヒープ
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   タブの変更
   * @param   tabID   変更後のタブID
   * @param   swFlg   切り替えフラグ
   */
  //------------------------------------------------------------------
  void ChangeTab( u32 tabID, bool swFlg = true );


private:  //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
   */
  //------------------------------------------------------------------
  virtual void OnPreLoadResource( void );

  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   * @param   paneID
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneID );

  //------------------------------------------------------------------
  /**
   * @brief   キー入力通知用リスナ
   * @param   pKey
   * @param   pStick
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );


private:
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );


private:
  app::util::Heap*        m_pHeap;        //!< ヒープ
  App::Status::StatusDrawListener*   m_uiListener;     //!< UIイベントリスナー

  App::Status::APP_PARAM*  m_pAppParam;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::Fade::FadeManager*    m_pFadeManager;

  u32   m_LayoutID;
  u8    m_Seq;

  App::Status::DrawerBase*   m_pDrawerBase;

  u32   m_TabID;
};


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif  // #ifndef __TEST_FUKUSHIMA_TEST_STATUS_LOWER_DRAW_H__

#endif // PM_DEBUG