#if !defined( __APP_STATUS_LOWER_DRAW_H_INCLUDED__ )
#define __APP_STATUS_LOWER_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusLowerDraw.h
 * @date    2015/11/27 17:03:50
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：メイン画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/Status/source/lowerParts/StatusBallPlate.h>
#include <App/Status/source/lowerParts/StatusInfoWindow.h>
#include <App/Status/source/lowerParts/StatusInformation.h>
#include <App/Status/source/lowerParts/StatusRibbon.h>
#include <App/Status/source/lowerParts/StatusWazaExchange.h>

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>
#include <fs/include/gfl2_Fs.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
struct APP_PARAM;
class DrawerBase;
class StatusDrawListener;
class MenuCursor;
class PokeModel;
class InfoWindow;
class Waza;
class WazaCursor;
class MsgWindow;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   メイン画面(下)
//------------------------------------------------------------------------------
class LowerMainDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public InformationListener
  , public BallPlateListener
  , public WazaExchangeExplainListener
  , public app::tool::InfoWindowSheetTypeListener
  , public RibbonListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerMainDraw );

public:
  //! @brief  シーケンスID
  enum {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IDLE,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END,
    SEQ_NUM,
  };

public:
  //! @brief    生成パラメータ
  struct CreateParam
  {
    app::util::Heap*                  heap;
    app::util::AppRenderingManager*   renderMan;
    gfl2::Fade::FadeManager*          fadeMan;
    app::tool::AppCommonGrpIconData*  cmnIcon;
    MenuCursor*                       wazaCursor;
    MenuCursor*                       ribbonCursor;
  };

  //! @brief    初期化パラメータ
  struct InitParam
  {
    APP_PARAM*                  appParam;
    void*                       appLytBuff;
    gfl2::str::MsgData*         baseMsgData;
    gfl2::str::MsgData*         itemMsgData;
    gfl2::str::MsgData*         tokuseiMsgData;
    print::WordSet*             wordSet;
    gfl2::fs::AsyncFileManager* fileMan;
    gfl2::ui::DeviceManager*    devMan;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  LowerMainDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LowerMainDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   このビュー専用のUIリスナー設定
   *
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( StatusDrawListener* listener ) { m_uiListener = listener; }

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
   *
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void ) { return true; }

  //------------------------------------------------------------------
  /**
   * @brief   準備完了かどうか
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsReady( void ) const { return m_Seq == SEQ_IDLE; }

  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンスかどうか
   *
   * @return  "true  = メインシーケンス"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsMainSeq( void ) const { return m_Seq == SEQ_MAIN; }

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータのセット
   *
   * @param   pp      表示する対象のポケモンパラメータ
   * @param   party   ポケパーティ
   * @param   index   ポケモンインデックス
   */
  //------------------------------------------------------------------
  void SetPokemonParam( pml::pokepara::PokemonParam* pp, pml::PokeParty* party, u32 index );

public:
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
   * @brief   オープン中？
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsOpen( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Initialize2D( InitParam& param );

private:
  //------------------------------------------------------------------
  /**
   * @brief   入力フラグを有効
   */
  //------------------------------------------------------------------
  void SetInputFlagEnable( void ) { m_bInputFlag = true; }

  //------------------------------------------------------------------
  /**
   * @brief   入力フラグを無効
   */
  //------------------------------------------------------------------
  void SetInputFlagDisable( void ) { m_bInputFlag = false; }

  //------------------------------------------------------------------
  /**
   * @brief   入力フラグを取得
   *
   * @return  "true  == 有効"
   * @return  "false == 無効"
   */
  //------------------------------------------------------------------
  bool GetInputFlag( void ) const { return m_bInputFlag; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   更新禁止フラグの更新
   *
   * @param   flag    更新禁止フラグ
   */
  //------------------------------------------------------------------
  void SetUpdateProhibition( bool flag ) { m_UpdateProhibition = flag; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   タブのアクティブ
   */
  //------------------------------------------------------------------
  void ActiveTab( void );

  //------------------------------------------------------------------
  /**
   * @brief   タブのパッシブ
   */
  //------------------------------------------------------------------
  void PassiveTab( void );

  //------------------------------------------------------------------
  /**
   * @brief   BattleFesCircle用のTab設定にする
   */
  //------------------------------------------------------------------
  void InitializeTabForBFC( void );

  //------------------------------------------------------------------
  /**
   * @brief   技タブを選択
   */
  //------------------------------------------------------------------
  void SelectWazaTab( void );

  //------------------------------------------------------------------
  /**
   * @brief   メモタブを選択
   */
  //------------------------------------------------------------------
  void SelectMemoTab( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   タブの変更処理
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void ChangeTab( const pml::pokepara::PokemonParam* pp );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンインデックスのセット
   *
   * @param   index   ポケモンインデックス
   */
  //------------------------------------------------------------------
  void SetPokeIndex( u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   タマゴ専用モードの設定
   *
   * @param   flag    タマゴ専用モードフラグ(true = タマゴ専用、false = 通常モード)
   */
  //------------------------------------------------------------------
  void SetEggMode( bool flag );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ボックスマークの更新
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void UpdateBoxMark( const pml::pokepara::PokemonParam* pp );

public:
  //------------------------------------------------------------------
  /**
   * @brief   技カーソルの位置を設定
   *
   * @param   pos       カーソル位置
   * @param   visFlag   表示切替を行うかどうか
   */
  //------------------------------------------------------------------
  void SetWazaCursorPos( u8 pos, bool visFlag = true );

  //------------------------------------------------------------------
  /**
   * @brief   次の技へ
   */
  //------------------------------------------------------------------
  void NextWazaCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   前の技へ
   */
  //------------------------------------------------------------------
  void PrevWazaCursor( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   技入れ替えの開始
   */
  //------------------------------------------------------------------
  void StartWazaExchange( void );

  //------------------------------------------------------------------
  /**
   * @brief   技入れ替えの終了
   */
  //------------------------------------------------------------------
  void EndWazaExchange( void );

  //------------------------------------------------------------------
  /**
   * @brief   キャンセルボタンの更新
   */
  //------------------------------------------------------------------
  void UpdateCancelBtn( void );   // @fix GFNMCat[2100]

public:
  //------------------------------------------------------------------
  /**
   * @brief   リボンリストを表示
   */
  //------------------------------------------------------------------
  void OpenRibbonList( void );

  //------------------------------------------------------------------
  /**
   * @brief   リボンリストを非表示
   */
  //------------------------------------------------------------------
  void CloseRibbonList( void );

  //------------------------------------------------------------------
  /**
   * @brief   リボンリストのアナログキーの入力設定
   *
   * @param   enable    入力フラグ
   */
  //------------------------------------------------------------------
  void InputRibbonKeyEnabled( bool enable );

  //------------------------------------------------------------------
  /**
   * @brief   リボンIDの取得
   *
   * @param   index   インデックス
   *
   * @return  リボンID
   */
  //------------------------------------------------------------------
  u32 GetRibbonID( u32 index ) const;

  //------------------------------------------------------------------
  /**
   * @brief   最後に選択していたリボンIDの取得
   *
   * @return  リボンID
   */
  //------------------------------------------------------------------
  u32 GetLastRibbonID( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウを開く(特性)
   */
  //------------------------------------------------------------------
  void OpenInfoWindow_Tokusei( void );

  //------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウを開く(アイテム)
   */
  //------------------------------------------------------------------
  void OpenInfoWindow_Item( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウを開く
   *
   * @param   msgID         メッセージID
   * @param   inputEnalbe   入力フラグを変更するかどうか(true = 変更、false = 変更しない)
   */
  //------------------------------------------------------------------
  void OpenMessageWindow( u32 msgID, bool inputEnable = true );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウを閉じる
   */
  //------------------------------------------------------------------
  void CloseMessageWindow( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウが閉じきったかどうか
   *
   * @return  "true  == 閉じきった"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsClosedMessageWindow( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   準備シーケンス
   *
   * @return  "true  = 準備完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool Sequence_Ready( void );

  //------------------------------------------------------------------
  /**
   * @brief   フェードインシーケンス
   *
   * @return  "true  = フェードイン完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool Sequence_FadeIn( void );

  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンス
   */
  //------------------------------------------------------------------
  void Sequence_Main( void );

private:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
   */
  //------------------------------------------------------------------
  virtual void OnPreLoadResource( void );

  //------------------------------------------------------------------
  /**
   * @brief   ペインタッチ時通知用リスナ
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

  //------------------------------------------------------------------
  /**
   * @brief   ペイン選択アニメーションの開始の検知
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual void OnLayoutPaneStartingAnimeEvent( const u32 paneId );

public:   //!< 情報(下)リスナー
  //------------------------------------------------------------------
  /**
   * @brief   道具インフォメーションボタンの入力切替
   *
   * @param   enable    入力フラグ(true = 有効、false = 無効)
   */
  //------------------------------------------------------------------
  virtual void SetInputEnableItemInfoBtn( bool enable );

public:   //!< ボールプレート(下)リスナー
  //------------------------------------------------------------------
  /**
   * @brief   タッチされたボールのインデックスを設定
   *
   * @param   index   タッチされたボールのインデックス
   */
  //------------------------------------------------------------------
  virtual void SetTouchedBallIndex( u32 index );

public:   //!< 技入れ替えリスナー
  //------------------------------------------------------------------
  /**
   * @brief   説明画面の表示を更新
   *
   * @param   wazaNo    技No
   */
  //------------------------------------------------------------------
  virtual void SetWazaExplain( WazaNo wazaNo );

public:   //!< インフォメーションウィンドウ
  //------------------------------------------------------------------
  /**
   * @brief   閉じ終わった
   */
  //------------------------------------------------------------------
  virtual void OnClose( void );

public:   //!< RibbonListener
  //------------------------------------------------------------------
  /**
   * @brief   タブのパッシブ
   */
  //------------------------------------------------------------------
  virtual void TabPassive( void );

  //------------------------------------------------------------------
  /**
   * @brief   キャンセルボタンの有効設定
   *
   * @param   enable    有効フラグ
   */
  //------------------------------------------------------------------
  virtual void SetCancelBtnEnable( bool enable );

private:   //!< ボタンマネージャ関連
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ボタンの設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetButtonEnable( const pml::pokepara::PokemonParam* pp );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ボタンの設定：タブボタン
   *
   * @param   btnMan    ボタンマネージャ
   */
  //------------------------------------------------------------------
  void SetButtonEnable_Tab( app::tool::ButtonManager* btnMan );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンの設定：技ボタン
   *
   * @param   btnMan    ボタンマネージャ
   * @param   pp        ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetButtonEnable_Waza( app::tool::ButtonManager* btnMan, const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンの設定：インフォボタン
   *
   * @param   btnMan    ボタンマネージャ
   */
  //------------------------------------------------------------------
  void SetButtonEnable_Info( app::tool::ButtonManager* btnMan, const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンの設定：ボックスマークボタン
   *
   * @param   btnMan    ボタンマネージャ
   */
  //------------------------------------------------------------------
  void SetButtonEnable_BoxMark( app::tool::ButtonManager* btnMan );


private:
  app::util::Heap*                    m_pHeap;

  app::util::AppRenderingManager*     m_pAppRenderingManager;

  gfl2::Fade::FadeManager*            m_pFadeManager;

  app::tool::AppCommonGrpIconData*    m_pCommonIcon;

  MenuCursor*                         m_pCursorWaza;

  MenuCursor*                         m_pCursorRibbon;

  StatusDrawListener*                 m_uiListener;

  pml::pokepara::PokemonParam*        m_pPokeParam;

private:
  APP_PARAM*        m_pAppParam;

  DrawerBase*       m_pDrawerBase;

  Information*      m_pLowInfo;

  BallPlate*        m_pBallPlate;

  PokeModel*        m_pPokeModel;

  InfoWindow*       m_pInfoWindow;

  Ribbon*           m_pRibbon;

  MsgWindow*        m_pMsgWindow;

  Waza*             m_pWaza;

  WazaCursor*       m_pWazaCursor;

  WazaExchange*     m_pWazaExchange;

  u8                m_Seq;

  u8                m_PrevTab;

  bool              m_PrevEggTab;

  bool              m_UpdateProhibition;

  bool              m_bInputFlag;      //!< 入力が可能か？(true = 可、false = 不可)

  bool              m_bBFCMode;        //!< BattleFesCircle用であるか

  bool              m_createNumFont;

private:
  gfl2::lyt::LytPane*     m_pPaneVisibleMark;

  gfl2::lyt::LytPane*     m_pPaneVisibleInfo;

  gfl2::lyt::LytPane*     m_pPaneVisibleRibbon;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_LOWER_DRAW_H_INCLUDED__
