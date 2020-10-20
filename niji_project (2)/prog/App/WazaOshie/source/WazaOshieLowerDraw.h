#ifndef NIJI_PROJECT_APP_WAZAOSHIE_LOWER_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_WAZAOSHIE_LOWER_DRAW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file WazaOshieLowerDraw.h
 * @date 2015/07/01 15:37:49
 * @author fukushima_yuya
 * @brief 技教え・技忘れ下画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/WazaOshie/include/WazaOshieAppParam.h>

#include "WazaOshieConst.h"
#include "WazaOshieInfoWindow.h"

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )
class WazaOshieDrawListener;
class WazaOshieStatus;
class WazaOshieExplain;
class DrawerBase;
class MenuCursor;
GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class KeyRepeatController;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

//------------------------------------------------------------------------------
// @brief   技教え・技忘れ下画面描画
//------------------------------------------------------------------------------
class WazaOshieLowerDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public WazaOshieInfoWindowListener
{
  GFL_FORBID_COPY_AND_ASSIGN( WazaOshieLowerDraw );

public:
  struct InitParam
  {
    APP_PARAM*                        appParam;
    void*                             arcBuff;
    gfl2::str::MsgData*               msgData;
    print::WordSet*                   wordSet;
    gfl2::Fade::FadeManager*          fadeMan;
    App::PokeList::IntermediateData*  partyData;
    app::tool::AppCommonGrpIconData*  cmnIcon;
    bool                              createNumFont;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  WazaOshieLowerDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager, MenuCursor* pCursor );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~WazaOshieLowerDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Init( InitParam& param );

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
  void WazaOshieOpen( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void WazaOshieClose( void );

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
   * @brief   このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( WazaOshieDrawListener* pListener );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pAppLytBuff     レイアウトバイナリ
   */
  //------------------------------------------------------------------
  void Initialize2D( void* pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   画面モードの変更
   * @param   dispMode    画面モード
   */
  //------------------------------------------------------------------
  void SetDispMode( u8 dispMode );

  //------------------------------------------------------------------
  /**
   * @brief   技説明画面のセットアップ
   * @param   index   技プレートのインデックス
   */
  //------------------------------------------------------------------
  void SetupWazaExplain( u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   次の技へ
   */
  //------------------------------------------------------------------
  void SetupWazaNext( void );

  //------------------------------------------------------------------
  /**
   * @brief   前の技へ
   */
  //------------------------------------------------------------------
  void SetupWazaPrev( void );


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

  //------------------------------------------------------------------
  /**
   * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
   */
  //------------------------------------------------------------------
  virtual void OnPreLoadResource( void );

private:  //!< WazaOshieInfoWindowListener
  //------------------------------------------------------------------
  /**
   * @brief   閉じ終わった
   */
  //------------------------------------------------------------------
  virtual void OnClose( void );

private:   //!< ボタンマネージャ
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンのセット
   * @param   dispMode  技選択画面 or 技説明画面
   */
  //------------------------------------------------------------------
  void SetButtonEnable( u8 dispMode );

private:
  app::util::Heap*        m_pHeap;        //!< ヒープ
  WazaOshieDrawListener*  m_uiListener;   //!< UIイベントリスナー

  app::util::AppRenderingManager*     m_pAppRenderingManager;

  gfl2::fs::AsyncFileManager*         m_pAsyncFileManager;

  gfl2::Fade::FadeManager*            m_pFadeManager;

  gfl2::str::MsgData*                 m_pMsgData;

  print::WordSet*                     m_pWordSet;

  app::tool::AppCommonGrpIconData*    m_pCommonIcon;

  PokeList::IntermediateData*         m_pPartyData;

  bool                                m_CreateNumFont;

  APP_PARAM*  m_pAppParam;

  u8          m_Seq;

  u8          m_DispMode;

  u32         m_SelectPlate;

  DrawerBase*           m_pDrawerBase[RES_ID_NUM];

  WazaOshieStatus*      m_pStatus;

  WazaOshieExplain*     m_pExplain;

  MenuCursor*           m_pCursor;

  WazaOshieInfoWindow*  m_pInfoWindow;

  pml::pokepara::PokemonParam*  m_pPokeParam;

  u8    m_WazaCount;

  app::util::KeyRepeatController*   m_pKeyController[DISP_MODE_NUM];

  u8                                m_CursorPos[DISP_MODE_NUM];
};


GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_WAZAOSHIE_LOWER_DRAW_H_INCLUDED