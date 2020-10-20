//======================================================================
/**
 * @file BattleVideoPlayerLowDataListView.h
 * @date 2015/09/24 16:23:10
 * @author uchida_yuto
 * @brief アプリの下画面データリストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __BATTLE_VIDEO_PLAYER_LOW_DATA_LIST_VIEW_H_INCLUDED__
#define __BATTLE_VIDEO_PLAYER_LOW_DATA_LIST_VIEW_H_INCLUDED__
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>

//  nijiのインクルード
#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"

#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerProcAppParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

    class VideoDataManager;

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  /** コールバックデリゲータ */
  class LowDataListViewDelegate
  {
  public:
    enum ActiveItemResult
    {
      ACTIVE_ITEM_RESULT_INFO_OFF  = 0,
      ACTIVE_ITEM_RESULT_INFO_ON,
    };

    LowDataListViewDelegate() {}

    //  リストのアクティブ位置
    virtual ActiveItemResult LowDataListViewDelegate_OnActiveCursor( const u32 idx ) = 0;
  };

  /**
   * @class LowDataListView
   * @brief アプリの下画面データリストView
   */
  class LowDataListView : 
  public app::ui::UIView,
  public app::tool::PaneList::CallBack,
  public app::tool::PaneListView::Listener,
  public app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN(LowDataListView);

  public:
    //  状態一覧
    enum State
    {
      STATE_IDLE  = 0,
      STATE_INPUT_EVENT_WAIT,
      STATE_INPUT_STOP,
    };

    //  削除モード
    enum DelMode
    {
      DEL_MODE_NONE = 0,
      DEL_MODE_SELECT,
    };

    //  情報モード
    enum InfoMode
    {
      INFO_MODE_DETAIL  = 0,
      INFO_MODE_RESULT
    };

    //  ボタンid
    enum ButtonId
    {
      BUTTON_ID_DATA_CHANGE = 0,
      BUTTON_ID_DATA_DEL,

      BUTTON_ID_MAX,
      BUTTON_ID_NONE  = BUTTON_ID_MAX,

      BUTTON_ID_DATA_SELECT,
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
        pMenuCursorBinData  = NULL;
      }

      app::util::Heap*                pHeap;
      app::util::AppRenderingManager* pRenderManager;
      void*                           pLytBinData;
      void*                           pMenuCursorBinData;
      gfl2::str::MsgData*             pAppMsgData;
      print::WordSet*                 pWordSet;
    };

    //  リストスクロールデータ
    struct LIST_SCROLL_DATA
    {
      u32 listPos;
      f32 listScroll;

      LIST_SCROLL_DATA()
      {
        listPos     = 0;
        listScroll  = 0.0f;
      }
    };

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      * @param[in] pHeap 使用するメモリ
      */
    //------------------------------------------------------------------
    LowDataListView( INIT_PARAM& rInitParam );

    virtual ~LowDataListView();

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
      * @brief   Viewの入力終了か
      */
    //------------------------------------------------------------------
    bool IsEndInput() const { return ( m_state == STATE_INPUT_STOP ); }

    //------------------------------------------------------------------
    /**
      * @brief   押したボタンidを返す
      */
    //------------------------------------------------------------------
    ButtonId  GetPushBtnId() const { return m_pushButtonId; }

    //------------------------------------------------------------------
    /**
      * @brief   状態設定
      * @param[in] newStaet 設定する状態
      */
    //------------------------------------------------------------------
    void SetState( const State newStaet );

    //------------------------------------------------------------------
    /**
      * @brief   削除モード設定
      * @note
      *   pLinkVideoDataListで設定したポインタの削除フラグ変数を初期化している
      */
    //------------------------------------------------------------------
    void SetDelMode( const DelMode newMode );

    //------------------------------------------------------------------
    /**
      * @brief   情報モード設定
      */
    //------------------------------------------------------------------
    void SetInfoMode( const InfoMode newMode );

    //------------------------------------------------------------------
    /**
      * @brief      リストデータ設定
      * @param[in]  pLinkVideoDataManager
      * @note
      *   pLinkVideoDataListで設定したポインタはクラス内部で値変更するケースがある
      */
    //------------------------------------------------------------------
    void SetListData( NetAppLib::BattleVideoPlayer::VideoDataManager* pLinkVideoDataManager, const LIST_SCROLL_DATA& rScrollData );

    //------------------------------------------------------------------
    /**
      * @brief   リストのスクロールデータ取得
      */
    //------------------------------------------------------------------
    const LIST_SCROLL_DATA GetListScrollData();

    //  デリゲータ設定
    void SetDelegate( LowDataListViewDelegate* pDelegate )
    {
      //  外部ポインタ設定なので注意!
      m_pDelegate = pDelegate;
    }

    // PaneList::CallBack
    virtual void CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos );
    virtual void CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
    virtual void CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
    virtual void CallBack_ItemChange( void * work, u32 pos1, u32 pos2 );

    // PaneListView::Listener
    virtual void PaneListItemSelectListener( u32 pos );

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

  private:
    //  作成するレイアウトワークの種類
    enum
    {
      LYT_WORK_NORMAL  = 0,
      LYT_WORK_MAX
    };

    enum
    {
      LIST_ITEM_PANE_MAX  = 8,
    };

    struct SELECT_ITEM
    {
      gfl2::lyt::LytTextBox*  pTextBoxPane;
      gfl2::lyt::LytPane*     pChkMarkBGPane;
      gfl2::lyt::LytPane*     pChkMarkPane;
      gfl2::lyt::LytPane*     pUploadMarkPane;

      bool  bUploadMarkFlg;

      //  初期化
      SELECT_ITEM()
      {
        pTextBoxPane  = NULL;
        pChkMarkBGPane  = NULL;
        pChkMarkPane    = NULL;
        pUploadMarkPane = NULL;
        bUploadMarkFlg  = false;
      }
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
      * @brief    データの更新
      * @@param   現在選択している選択pos
      */
    //------------------------------------------------------------------
    void _UpdateData( const u32 currentPos );

    //------------------------------------------------------------------
    /**
      * @brief   変数の初期化はここに
      */
    //------------------------------------------------------------------
    void _Clear()
    {
      m_state           = STATE_IDLE;
      m_initSeq         = 0;
      m_endSeq          = 0;

      m_pushButtonId      = BUTTON_ID_NONE;
      m_pPaneListView     = NULL;
      m_pPaneListViewHeap = NULL;
      m_pMenuCursor       = NULL;
      m_pPaneListData     = NULL;

      m_pYBtnTextPane     = NULL;
      m_pXBtnTextPane     = NULL;
      m_pSelectListNumTextPane  = NULL;
      m_pListMaxNumTextPane     = NULL;
      m_pListNumSlashTextPane   = NULL;

      m_pVideoDataListManager = NULL;
      m_pDelegate             = NULL;

      m_delMode           = DEL_MODE_NONE;
      m_infoMode          = INFO_MODE_DETAIL;
    }

    DelMode                         m_delMode;
    InfoMode                        m_infoMode;

    State                           m_state;

    u32                             m_initSeq;
    u32                             m_endSeq;

    ButtonId                        m_pushButtonId;
    INIT_PARAM                      m_initParam;

    app::tool::MenuCursor*          m_pMenuCursor;

    app::tool::PaneList::PANE_DATA* m_pPaneListData;
    app::tool::PaneListView*        m_pPaneListView;
    app::util::Heap*                m_pPaneListViewHeap;

    gfl2::lyt::LytTextBox*          m_pYBtnTextPane;
    gfl2::lyt::LytTextBox*          m_pXBtnTextPane;

    gfl2::lyt::LytTextBox*          m_pSelectListNumTextPane;
    gfl2::lyt::LytTextBox*          m_pListNumSlashTextPane;
    gfl2::lyt::LytTextBox*          m_pListMaxNumTextPane;

    SELECT_ITEM                     m_aSelectItem[ LIST_ITEM_PANE_MAX ];

    LowDataListViewDelegate*        m_pDelegate;

    //  リストが参照するデータ
    NetAppLib::BattleVideoPlayer::VideoDataManager*               m_pVideoDataListManager;
  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)

#endif // __BATTLE_VIDEO_PLAYER_LOW_DATA_LIST_VIEW_H_INCLUDED__