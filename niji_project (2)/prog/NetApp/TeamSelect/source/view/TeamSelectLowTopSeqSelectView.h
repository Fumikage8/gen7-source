//======================================================================
/**
 * @file TeamSelectLowTopSeqSelectView.h
 * @date 2015/06/30 19:29:18
 * @author uchida_yuto
 * @brief トップ画面のシーケンス選択下画面View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_LOW_TOP_SEQ_SELECT_VIEW_H_INCLUDED__
#define __TEAM_SELECT_LOW_TOP_SEQ_SELECT_VIEW_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/ui/UIView.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  
  class MenuCursor;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  /**
   * @class TeamSelectLowTopSeqSelectView
   */
  class TeamSelectLowTopSeqSelectView : public app::ui::UIView, public app::ui::UIInputListener
  {
      GFL_FORBID_COPY_AND_ASSIGN(TeamSelectLowTopSeqSelectView);

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
      BUTTON_ID_TEAM_SETTING  = 0,
      BUTTON_ID_MAX,

      BUTTON_ID_NONE
    };

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      * @param[in] pHeap 使用するメモリ
      */
    //------------------------------------------------------------------
    TeamSelectLowTopSeqSelectView( app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderManager );

    virtual ~TeamSelectLowTopSeqSelectView();

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    void Initialize( void* pLytBinData, void* pLytMenuCursorBinData, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet );

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
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

    //------------------------------------------------------------------
    /**
      * @brief   ペインタッチ時を検知
      * @param[in] id ボタンid
      * @return 入力処理を続けるかを返す
      */
    //------------------------------------------------------------------
    virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 id );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief ペインタッチ時を検知
    *       【ボタンマネージャー関係】
    *
    * @param[in] id ボタンid
    *
    * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
    * 
    *                 TRIGER判定として使いたい場合は、ここを使わずに
    *                 ボタンマネージャーモードをTRIGGERにして
    *                 OnLayoutPaneEventを使うべき。
    */
    //--------------------------------------------------------------------------------------------
    void OnLayoutPaneTouchTriggerEvent( const u32 id );

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
      * @brief   状態取得
      */
    //------------------------------------------------------------------
    State GetState() const { return m_state; }

    //------------------------------------------------------------------
    /**
      * @brief   ボタンを押したid
      */
    //------------------------------------------------------------------
    ButtonId GetPushBtnId() const { return m_lastPushButtonId; }

    //------------------------------------------------------------------
    /**
      * @brief   指定したボタン有効化
      */
    //------------------------------------------------------------------
    void SetEnableActiveButton( const ButtonId buttonId, const bool bFlg );

  private:
    //  作成するレイアウトワークの種類
    enum
    {
      LYT_WORK_NORMAL  = 0,
      LYT_WORK_MAX
    };

    struct ButtonData
    {
      gfl2::lyt::LytParts*  pItemBtnPartsPane;
      u32                   textId;
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
      * @brief   カーソル位置設定
      */
    //------------------------------------------------------------------
    void _SetCursorPos( const ButtonId id );

    //------------------------------------------------------------------
    /**
      * @brief   変数の初期化はここに
      */
    //------------------------------------------------------------------
    void _Clear()
    {
      m_pHeap           = NULL;
      m_pRenderManager  = NULL;
      m_pMenuCursor     = NULL;
      
      m_state           = STATE_IDLE;
      m_bPushBackBtn    = false;
      m_pushButtonId    = BUTTON_ID_NONE;
      m_lastPushButtonId  = BUTTON_ID_NONE;
      m_selectIdx         = BUTTON_ID_TEAM_SETTING;
      gfl2::std::MemClear( m_aButtonData, sizeof( m_aButtonData ) );
    }

    app::util::Heap*                m_pHeap;
    app::util::AppRenderingManager* m_pRenderManager;
    app::tool::MenuCursor*          m_pMenuCursor;

    State                           m_state;

    ButtonId                        m_pushButtonId;
    ButtonId                        m_lastPushButtonId;

    struct ButtonData               m_aButtonData[ BUTTON_ID_MAX ];
    s32                             m_selectIdx;

    bool                            m_bPushBackBtn;
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_LOW_TOP_SEQ_SELECT_VIEW_H_INCLUDED__