//======================================================================
/**
 * @file BattleVideoPlayerLowRootView.h
 * @date 2015/09/02 16:23:10
 * @author uchida_yuto
 * @brief アプリの下画面ルートview
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __BATTLE_VIDEO_PLAYER_LOW_ROOT_VIEW_H_INCLUDED__
#define __BATTLE_VIDEO_PLAYER_LOW_ROOT_VIEW_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

//  nijiのインクルード
#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/CommonMessageWindowManipulator.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  //  キー入力結果を外部から呼び出すデリゲータ
  class LowRootViewInputDelegate
  {
  public:
    //------------------------------------------------------------------
    /**
      * @brief   キー入力通知用リスナ
      * @param[in] pButton  ボタン入力の情報
      * @param[in] pKey     キー入力の情報
      * @param[in] pStick   スティック入力の情報
      * @return 入力処理を続けるかを返す
      */
    //------------------------------------------------------------------
    virtual void OnTrainerPhotoLowRootViewKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;
  };

  /**
   * @class LowRootView
   * @brief 下画面ルートView
   */
  class LowRootView : 
  public app::ui::UIView, 
  public app::ui::UIInputListener,
  public App::Tool::CommonMessageWindowManipulator::IEventHandler
  {
      GFL_FORBID_COPY_AND_ASSIGN(LowRootView);

  public:
    //  状態一覧
    enum State
    {
      STATE_IDLE  = 0,
      STATE_INPUT_EVENT_WAIT,
      STATE_INPUT_STOP
    };

    /**
     * @enum ButtonId
     * 入力ボタンid一覧
     */
    enum ButtonId
    {
      BUTTON_ID_BACK  = 0,
      BUTTON_ID_MAX,

      BUTTON_ID_NONE
    };

    enum ShowDialogMessageLine
    {
      SHOW_DIALOG_MESSAGE_LINE_NONE = 0,
      SHOW_DIALOG_MESSAGE_LINE_ONE,
      SHOW_DIALOG_MESSAGE_LINE_TWO,
    };

    //  初期化パラメータ
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        pHeap           = NULL;
        pRenderManager  = NULL;
        pLytBinData     = NULL;
        pAppMsgData     = NULL;
        pWordSet        = NULL;
      }

      app::util::Heap*                      pHeap;
      app::util::AppRenderingManager*       pRenderManager;
      void*                                 pLytBinData;
      gfl2::str::MsgData*                   pAppMsgData;
      print::WordSet*                       pWordSet;
    };

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      */
    //------------------------------------------------------------------
    LowRootView( INIT_PARAM& rInitParam );

    virtual ~LowRootView();

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    bool InitFunc();

    //------------------------------------------------------------------
    /**
      * @brief   終了処理
      */
    //------------------------------------------------------------------
    bool EndFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw(  gfl2::gfx::CtrDisplayNo displayNo );

    //------------------------------------------------------------------
    /**
      * @brief   ペインタッチ時を検知
      * @param[in] buttonId ボタンid
      * @return 入力処理を続けるかを返す
      */
    //------------------------------------------------------------------
    virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 id );

    //------------------------------------------------------------------
    /**
      * @brief   キー入力通知用リスナ
      * @param[in] pButton  ボタン入力の情報
      * @param[in] pKey     キー入力の情報
      * @param[in] pStick   スティック入力の情報
      * @return 入力処理を続けるかを返す
      */
    //------------------------------------------------------------------
    virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    //------------------------------------------------------------------
    /**
      * @brief   状態設定
      * @param[in] newStaet 設定する状態
      */
    //------------------------------------------------------------------
    void SetState( const State newStaet );

    //------------------------------------------------------------------
    /**
      * @brief   押したボタンを返す
      */
    //------------------------------------------------------------------
    ButtonId GetPushBtn() const { return m_pushButtonId; }

    //------------------------------------------------------------------
    /**
      * @brief   指定したボタンを表示設定
      */
    //------------------------------------------------------------------
    void SetVisiblePushBtn( const ButtonId id, const bool bVisible );

    //------------------------------------------------------------------
    /**
      * @brief   Viewの入力終了か
      */
    //------------------------------------------------------------------
    bool IsEndInput() const { return ( m_state == STATE_INPUT_STOP ); }

    //  下画面のシステムウィンドウは App::Tool::MessageMenuView クラスを使っているのでもう使わない
#if 0
    //------------------------------------------------------------------
    /**
      * @brief   入力のデリゲータを設定
      */
    //------------------------------------------------------------------
    void SetInputDelegate( LowRootViewInputDelegate* pDelegate )
    {
      m_pInputDelegate  = pDelegate;
    }

    //------------------------------------------------------------------
    /**
      * @brief   メッセージダイアログ表示
      */
    //------------------------------------------------------------------
    void ShowMessageDialog( const ShowDialogMessageLine messageLine, gfl2::str::StrBuf* pStrBuf, const bool bUseInput, const bool bShowTimerIcon );

    //------------------------------------------------------------------
    /**
      * @brief   メッセージダイアログ非表示
      */
    //------------------------------------------------------------------
    void HideMessageDialog();

    //------------------------------------------------------------------
    /**
      * @brief   メッセージダイアログ更新が終わっているか
      */
    //------------------------------------------------------------------
    bool IsEndUpdateMessageDialog() const { return m_bEndUpdateMessageDialog; }
#endif
    //------------------------------------------------------------------
    /**
      * @brief   メッセージダイアログのコールバック
      */
    //------------------------------------------------------------------
    void  CommonMessageWindowManipulator_OnEvent( const App::Tool::CommonMessageWindowManipulator::EventID event );
  protected:
    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update();

  private:
    //  作成するレイアウトワークの種類
    enum
    {
      LYT_WORK_NORMAL  = 0,
      LYT_WORK_MAX
    };

    //------------------------------------------------------------------
    /**
      * @brief   2D画面の作成
      * @param   pAppLytBuff レイアウトデータのバイナリポインタ
      */
    //------------------------------------------------------------------
    void _Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet );

    //------------------------------------------------------------------
    /**
      * @brief   変数の初期化はここに
      */
    //------------------------------------------------------------------
    void _Clear()
    {
      m_state                   = STATE_IDLE;
      m_pushButtonId            = BUTTON_ID_NONE;
      m_pInputDelegate          = NULL;
      m_pEnterButtonTextPane    = NULL;
      m_pMessageWindowTextPane  = NULL;
      m_bEndUpdateMessageDialog = false;
      m_bShowMessageDialog      = false;

      m_pCommonMessageDialogOneLine = NULL;
      m_pCommonMessageDialogTwoLine = NULL;

      m_pMessageDialogOneLinePartPane = NULL;
      m_pMessageDialogTwoLinePartPane = NULL;

      m_initSeq                 = 0;
      m_endSeq                  = 0;
    }

    State                           m_state;

    ButtonId                        m_pushButtonId;
    
    INIT_PARAM                      m_initParam;

    u32                             m_initSeq;
    u32                             m_endSeq;

    bool                            m_bEndUpdateMessageDialog;
    bool                            m_bShowMessageDialog;

    gfl2::lyt::LytParts*            m_pMessageDialogTwoLinePartPane;
    gfl2::lyt::LytParts*            m_pMessageDialogOneLinePartPane;

    gfl2::lyt::LytTextBox*          m_pEnterButtonTextPane;
    gfl2::lyt::LytTextBox*          m_pMessageWindowTextPane;

    //  汎用ウィンドウダイアログ(2行用)
    App::Tool::CommonMessageWindowManipulator*  m_pCommonMessageDialogTwoLine;
    //  汎用ウィンドウダイアログ(1行用)
    App::Tool::CommonMessageWindowManipulator*  m_pCommonMessageDialogOneLine;

    LowRootViewInputDelegate*       m_pInputDelegate;
  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)

#endif // __BATTLE_VIDEO_PLAYER_LOW_ROOT_VIEW_H_INCLUDED__