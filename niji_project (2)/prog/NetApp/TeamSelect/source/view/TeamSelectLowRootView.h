//======================================================================
/**
 * @file TeamSelectLowRootView.h
 * @date 2015/06/30 19:29:18
 * @author uchida_yuto
 * @brief チーム選択の下画面ルートView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_LOW_ROOT_VIEW_H_INCLUDED__
#define __TEAM_SELECT_LOW_ROOT_VIEW_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/ui/UIView.h"

#include "AppLib/include/Tool/CommonMessageWindowManipulator.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

  class CommonMessageWindowManipulator;

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  /**
   * @class TeamSelectLowRootView
   * @brief チーム選択の下画面ルートView
   */
  class TeamSelectLowRootView : public app::ui::UIView, public app::ui::UIInputListener, public App::Tool::CommonMessageWindowManipulator::IEventHandler
  {
      GFL_FORBID_COPY_AND_ASSIGN(TeamSelectLowRootView);

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

    /**
     * @fn
     * ここに関数の説明を書く
     * @brief 要約説明
     * @param[in] var1 var1の説明
     * @param[out] var2 var2の説明
     * @param[in,out] var3 var3の説明
     * @return 戻り値の説明
     * @sa 参照すべき関数を書けばリンクが貼れる
     * @detail 詳細な説明
     */

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      * @param[in] pHeap 使用するメモリ
      */
    //------------------------------------------------------------------
    TeamSelectLowRootView( app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderManager );

    virtual ~TeamSelectLowRootView();

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    void Initialize( void* pLytBinData, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet );

    //------------------------------------------------------------------
    /**
      * @brief   終了処理
      */
    //------------------------------------------------------------------
    void Terminate();

    //------------------------------------------------------------------
    /**
      * @brief   破棄可能か
      */
    //------------------------------------------------------------------
    bool IsDelete();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update();

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
      * @brief   ボタン表示設定
      * @param[in] id   ボタンid
      * @param[in] bFlg 表示設定
      */
    //------------------------------------------------------------------
    void SetBtnVisible( const ButtonId id, const bool bFlg );

    //------------------------------------------------------------------
    /**
      * @brief   状態設定
      * @param[in] newStaet 設定する状態
      */
    //------------------------------------------------------------------
    void SetState( const State newStaet );

    //------------------------------------------------------------------
    /**
      * @brief   メッセージイベント中か
      */
    //------------------------------------------------------------------
    bool IsMessageEvent() const { return m_bMessageEvent; }

    //------------------------------------------------------------------
    /**
      * @brief   バックボタンを押した
      */
    //------------------------------------------------------------------
    bool IsPushBackBtn() const { return m_bPushBackBtn; }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      メッセージ表示
    * @param[in]  pStrBuf             文字列
    * @param[in]  bUserInput          ユーザー入力でメッセージ送りをするか
    * @param[in]  bImmediateMessage   true:即時表示 / false:ストリーム（タイピング）表示
    * @param[in]  bShowTimerIcon      タイマーアイコンを表示する（SetupでpaneID_TimerIconを指定していること）
    */
    //--------------------------------------------------------------------------------------------
    void ShowMessageWindow( gfl2::str::StrBuf* pStrBuf, const bool bUserInput, const bool bImmediateMessage, const bool bShowTimerIcon );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  タイトルメッセージ表示
    * @param[in]  pStrBuf             文字列
    * @param[in]  bImmediateMessage   true:即時表示 / false:ストリーム（タイピング）表示
    */
    //--------------------------------------------------------------------------------------------
    void ShowTitleMessageWindow( gfl2::str::StrBuf* pStrBuf, const bool bImmediateMessage );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  タイトルメッセージ非表示
    */
    //--------------------------------------------------------------------------------------------
    void HideTitleMessageWindow();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  メッセージ非表示
    */
    //--------------------------------------------------------------------------------------------
    void HideMesssageWindow();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  メッセージイベント通知
    */
    //--------------------------------------------------------------------------------------------
    virtual void  CommonMessageWindowManipulator_OnEvent( const App::Tool::CommonMessageWindowManipulator::EventID event );

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
      m_pHeap                   = NULL;
      m_pRenderManager          = NULL;
      m_state                   = STATE_IDLE;
      m_bPushBackBtn            = false;
      m_bMessageEvent           = false;
      m_bMessageFinishUserInput = false;
      m_pushButtonId            = BUTTON_ID_NONE;

      m_pTitleMessageWindowPartsPane  = NULL;
      m_pTitleMessageTextBoxPane      = NULL;
      m_pTitleMessageWindow           = NULL;
      m_pCommonMessageWindow          = NULL;
    }

    app::util::Heap*                m_pHeap;
    app::util::AppRenderingManager* m_pRenderManager;
    State                           m_state;

    ButtonId                        m_pushButtonId;

    bool                            m_bPushBackBtn;
    bool                            m_bMessageEvent;
    bool                            m_bMessageFinishUserInput;

    gfl2::lyt::LytParts*            m_pTitleMessageWindowPartsPane;
    gfl2::lyt::LytTextBox*          m_pTitleMessageTextBoxPane;
    print::MessageWindow*           m_pTitleMessageWindow;

    App::Tool::CommonMessageWindowManipulator*  m_pCommonMessageWindow;
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_LOW_ROOT_VIEW_H_INCLUDED__