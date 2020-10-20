#ifndef NIJI_PROJECT_FIELD_FINDER_LOSER_DRAW_H_INCLUDED
#define NIJI_PROJECT_FIELD_FINDER_LOSER_DRAW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    FinderLowerDraw.h
 * @author  fukushima_yuya
 * @date    2015.05.26
 * @brief   ポケファインダー下画面
 */
//==============================================================================

// niji
#include "FinderWindow.h"

#include <App/Finder/include/FinderAppParam.h>

#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetTypeListener.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )
class FinderDrawListener;
class DrawerBase;
class FinderInfoWindow;
class FinderConfirmWindow;
GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN(Savedata)
class PokeFinderSave;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

//------------------------------------------------------------------------------
// @brief   ポケファインダー上画面の描画環境
//------------------------------------------------------------------------------
class FinderLowerDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public FinerConfirmWindowListenr
  , public app::tool::InfoWindowSheetTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderLowerDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderLowerDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager, Savedata::PokeFinderSave* finderSave );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderLowerDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( FinderDrawListener* pListener );

  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam
   * @param   pAppLytBuff   レイアウトバイナリ
   * @param   grpFontBuff   グラフィックフォント
   * @param   msgData       メッセージデータ
   * @param   wordSet       ワードセット
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* pAppParam, void* pAppLytBuff, void* grpFontBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );

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
   * @brief   準備完了か
   */
  //------------------------------------------------------------------
  bool IsOpened( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pHeap         ヒープ
   * @param   pAppLytBuff   レイアウトバイナリデータ
   * @param   grpFontBuff   グラフィックフォント
   */
  //------------------------------------------------------------------
  void Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff, void* grpFontBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   カメラアプリのバージョン設定
   */
  //------------------------------------------------------------------
  void SetAppVersion( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ピントの設定
   * @param   pintDir    ピント方向
   */
  //------------------------------------------------------------------
  void SetPintMode( u8 pintDir );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ジャイロの切り替え
   */
  //------------------------------------------------------------------
  void SwitchGyroMode( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   ジャイロの設定
   *
   * @param   mode    ジャイロモード
   */
  //------------------------------------------------------------------
  void SetGyroMode( const u8 mode );
  
  //------------------------------------------------------------------
  /**
   * @brief   ジャイロのSEを設定
   *
   * @param   mode    ジャイロモード
   */
  //------------------------------------------------------------------
  void SetGyroBtnSE( const u8 mode );

private:
  //------------------------------------------------------------------
  /**
   * @brief   コールボタンのプッシュ
   */
  //------------------------------------------------------------------
  void PushCallBtn( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   コールボタンの有効化
   */
  //------------------------------------------------------------------
  void ActiveCallBtn( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   インフォボタンのプッシュ
   */
  //------------------------------------------------------------------
  void PushInfoBtn( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   待機モードの開始
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  void StartWaitMode( EndMode endMode );


public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   ペインタッチアクション通知用リスナ
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 paneId );

  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

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
   * @brief   キー入力通知用リスナ
   * @param   pKey
   * @param   pStick
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

public:   //!< ファインダーウィンドウリスナー
  //------------------------------------------------------------------
  /**
   * @brief   項目選択時のリスナー
   *
   * @param   kind    ウィンドウの種類
   * @param   id      選択項目ID
   */
  //------------------------------------------------------------------
  virtual void SelectMenu( WndKind  kind, u32 id );

public:   //!< app::tool::InfoWindow::InfoWindowSheetTypeListener
  //------------------------------------------------------------------
  /**
   * @brief   インフォウィンドウを閉じ終わった
   */
  //------------------------------------------------------------------
  virtual void OnClose( void );

public:   //!< ボタンマネージャ関連
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateFinderButtonManager( void );

private:
  app::util::Heap*                  m_pHeap;        //!< ヒープ

  FinderDrawListener*               m_uiListener;   //!< UIイベントリスナー

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  Savedata::PokeFinderSave*         m_pFinderSave;

  APP_PARAM*                        m_pAppParam;

  gfl2::str::MsgData*               m_pMsgData;

  print::WordSet*                   m_pWordSet;

  u8    m_Seq;
  
private:
  DrawerBase*             m_pDrawerBase;

  FinderInfoWindow*       m_pInfoWindow;

  FinderConfirmWindow*    m_pConfWindow;

private:
  u8      m_Version;

  u8      m_PintMode;

  u8      m_GyroMode;

  bool    m_bWaitMode;

  EndMode m_EndMode;

private:
  gfl2::lyt::LytTextBox*      m_pGyroOnMsg;

  gfl2::lyt::LytTextBox*      m_pGyroOffMsg;
};

GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )


#endif  //#ifndef NIJI_PROJECT_FIELD_FINDER_LOSER_DRAW_H_INCLUDED
