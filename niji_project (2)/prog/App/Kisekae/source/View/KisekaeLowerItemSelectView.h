// ============================================================================
/*
 * @file KisekaeLowerItemSelectView.h
 * @brief きせかえアプリのアイテム選択の下画面を表示するクラス
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_LOWER_ITEM_SELECT_VIEW_H_INCLUDE )
#define KISEKAE_LOWER_ITEM_SELECT_VIEW_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>

//  nijiのインクルード
#include "AppLib/include/Tool/app_tool_PaneListView.h"

#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class KeyRepeatController;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class CommonMessageObject;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)

  class ItemList;
  class ItemDataBase;
  class Work;

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(View)

  //  前方宣言
  class LowerItemSelectViewDelegate;

  class LowerItemSelectView :
    public NetAppLib::System::NetApplicationViewBase,
    public app::tool::PaneList::CallBack,
    public app::tool::PaneListView::Listener,
    public app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( LowerItemSelectView );

    struct CATEGORY_SETUP_DATA
    {
      u32       selectItemAnimeId;
      u32       nonSelectItemAnimeId;
      u32       selectBtnAnimeId;
      u32       nonSelecBtntAnimeId;
      u32       firstSelectAnimeId;

      u32       nonItemMsgId;
    };

  public:
    enum Mode
    {
      MODE_SHOP = 0,
      MODE_DRESS_UP,
      MODE_COLOR_DRESS_UP
    };

    //  ボタンのid種類
    enum ButtonId
    {
      //  Bキャンセル
      BUTTON_ID_CANCEL  = 0,

      //  すがたを見る
      BUTTON_ID_VIEWR,

      //  きがえる
      BUTTON_ID_DRESS_UP,

      //  カテゴリー
      BUTTON_ID_CATEGORY_00,
      BUTTON_ID_CATEGORY_01,
      BUTTON_ID_CATEGORY_02,
      BUTTON_ID_CATEGORY_03,
      BUTTON_ID_CATEGORY_04,
      BUTTON_ID_CATEGORY_05,
      BUTTON_ID_CATEGORY_06,
      BUTTON_ID_CATEGORY_07,

      BUTTON_ID_MAX,

      //  アイテムを押した
      BUTTON_ID_ITEM,

      BUTTON_ID_NONE
    };

    //  選択アイテムデータセットアップ
    struct SETUP_ITEM_DATA
    {
      u32 partsPaneId;
      u32 holdAnimeId;
      u32 releaseAnimeId;
      u32 cancelAnimeId;
      u32 selectAnimeId;
      u32 activeAnimeId;
      u32 passiveAnimeId;

      u32 newIconLoopAnimeId;
    };

    //  選択アイテムクラス
    class SelectItem
    {
      friend class LowerItemSelectView;

    public:
      SelectItem();
      virtual ~SelectItem();

      //  チェックマークをつける
      void SetEnableChkMark( const bool bFlg );

      //  newマークをつける
      void SetEnableNewMark( const bool bFlg );

      //  項目選択がアクティブか
      bool IsSelectActive() const { return m_bActive; }

    private:
      void _Clear()
      {
        m_mode  = MODE_SHOP;
        m_pItemNameTextPane   = NULL;
        m_pPriceNumTextPane   = NULL;
        m_pColorNameTextPane  = NULL;
        m_pBoughtNameTextPane = NULL;

        m_pChkMarkBGPane      = NULL;
        m_pChkMarkPane        = NULL;

        m_pNewIconPartsPane   = NULL;

        m_pBoughtMarkPane     = NULL;

        m_pNewIconPic         = NULL;

        m_pRootView   = NULL;

        m_bActive = true;
        m_bVisibleMoney = true;

        m_initSeq = 0;
        m_endSeq  = 0;
        m_pushSEid  = SEQ_SE_DECIDE1;

        gfl2::std::MemClear( &m_setupItemData, sizeof( m_setupItemData ) );
      }

      //  初回設定
      void Setup( LowerItemSelectView* pRootView, App::Kisekae::System::Work* pWork, const SETUP_ITEM_DATA& rSetup, const app::tool::PaneList::PANE_DATA& rPaneData, gfl2::heap::HeapBase* pHeap );

      //  初期化処理
      bool InitFunc();
      //  終了処理
      bool EndFunc();

      void UpdateFunc();

       //  モード指定
      void SetMode( const Mode mode );

      //  金額表示をするか
      void SetVisibleMoney( const bool bFlg ) { m_bVisibleMoney = bFlg; }

      //  項目データ設定
      void SetData( App::Kisekae::System::ItemDataBase* pData, NetAppLib::Message::MessageUtility* pMessageUtility );

      //  アクティブにするかどうか
      void SetActive( const bool bActive );

      //  プッシュSEを設定
      void SetPushSEId( const u32 seId ) { m_pushSEid = seId; }
      u32 GetPushSEId() const { return m_pushSEid; }

      Mode  m_mode;

      bool  m_bVisibleMoney;

      //  アイテム名
      gfl2::lyt::LytTextBox*  m_pItemNameTextPane;

      //  価格名
      gfl2::lyt::LytTextBox*  m_pPriceNumTextPane;

      //  色情報
      gfl2::lyt::LytTextBox*  m_pColorNameTextPane;

      //  購入済み名
      gfl2::lyt::LytTextBox*  m_pBoughtNameTextPane;
      //  購入済みマーク
      gfl2::lyt::LytPane*     m_pBoughtMarkPane;

      //  チェックボックス
      gfl2::lyt::LytPane*     m_pChkMarkBGPane;
      gfl2::lyt::LytPane*     m_pChkMarkPane;

      //  newアイコン
      gfl2::lyt::LytPane*     m_pNewIconPartsPane;
      //  newアイコン画像ペイン
      gfl2::lyt::LytPicture*  m_pNewIconPic;

      LowerItemSelectView*    m_pRootView;

      u32 m_initSeq;
      u32 m_endSeq;
      u32 m_pushSEid;

      SETUP_ITEM_DATA m_setupItemData;

      bool  m_bActive;
    };

    //  カテゴリーアイテムクラス
    class CategoryItem
    {
      friend class LowerItemSelectView;
    public:
      //  アイテムを選択できるかどうか
      void SelectItem( const bool bFlg );
      //  ボタンを選択しているかどうか
      void SelectBtn( const bool bFlg, const bool bFirst = false );

      //  アイテムがない時のエラーメッセージid
      u32 GetNonItemMsgId() const { return m_categorySetup.nonItemMsgId; }

    private:
      void Init( LowerItemSelectView* pRootView, const app::ui::ButtonInfoEx buttonInfo, const CATEGORY_SETUP_DATA& rSetupData );

      app::ui::ButtonInfoEx m_buttonInfo;
      CATEGORY_SETUP_DATA   m_categorySetup;

      LowerItemSelectView*  m_pRootView;
    };

    LowerItemSelectView( App::Kisekae::System::Work* pWork );
    virtual~LowerItemSelectView();

    //  初期化・終了処理
    bool InitFunc();
    bool EndFunc();

    //  開始イベント実行
    void StartEvent();

    // UIView
    virtual void Update();
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
    virtual bool IsDrawing(void) const;

    //  リスト作成
    void CreateList( App::Kisekae::System::ItemList* pItemList, const u32 listDrawPriority, const Mode mode, const bool bVisibleMoney, const bool bFirst );

    //  リストのカーソルスクロールと位置を取得
    f32 GetListScroll();
    u32 GetListPos();

    //  アイドリングを開始
    void StartIdling();

    //  画面のどこが押されたか
    ButtonId GetPushId() const { return m_pushId; }

    //  GetPushId() == BUTTON_ID_ITEMの場合値が入っている
    u32 GetPushItemSelectIdx() const { return m_pushItemIdx; }

    //  リストアイテムを取得
    SelectItem* GetListItem( const u32 paneListIdx );

    //  カテゴリアイテムを取得
    CategoryItem* GetCategoryItem( const u32 categoryListIdx );

    //  デリゲーターの設定
    void SetDelgate( LowerItemSelectViewDelegate* pDelgate )
    {
      m_pDelegate = pDelgate;
    }

    //  ボタンアクティブ設定
    void SetActionButton( const ButtonId buttonId, const bool bFlg );

    //  メッセージ表示
    void ShowMessageLower( gfl2::str::StrBuf* pStr, const bool bIsImmediateMessage = true );
    void HideMessageLower();

    //  画面リフレッシュ
    void Refresh();

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
    enum LayoutWorkID
    {
      LAYOUT_WORK_ID_LOWER  = 0
    };

    //  ステート
    enum State
    {
      STATE_START_EVENT = 0,
      STATE_IDLE,
      STATE_INPUT_EVENT_WAIT,
      STATE_INPUT_STOP,
    };

    enum
    {
      CATEGORY_ITEM_MAX  = BUTTON_ID_CATEGORY_07 - BUTTON_ID_CATEGORY_00 + 1,
      ITEM_LIST_MAX = 7
    };

    //  ステート設定
    void _SetState( const State newState );

    //  リスト作成
    void _CreatePaneList( const u32 listNum, const u32 listScroll, const f32 listPos, const u32 priority );

    void _Clear()
    {
      m_pWork         = NULL;
      m_pPaneListView = NULL;
      m_pPaneListViewHeap = NULL;
      m_pLowerCommonMessageObject = NULL;
      m_pErrorMessageTextPane     = NULL;
      m_pMenuCursor               = NULL;
      m_pItemList                 = NULL;
      m_pDelegate                 = NULL;
      m_pCategoryKeyRepatCtrl     = NULL;

      m_state     = STATE_INPUT_STOP;
      m_pushId    = BUTTON_ID_NONE;
      m_tmpPushId = BUTTON_ID_NONE;
      m_pushItemIdx     = 0;
      m_pushPaneListIdx = 0;
      m_acitveCategoryIdx = 0;
      m_listItemNum = 0;
      m_bRefresh    = false;
    }

    App::Kisekae::System::Work*     m_pWork;
    App::Kisekae::System::ItemList* m_pItemList;

    SelectItem    m_aSelectItem[ ITEM_LIST_MAX ];
    CategoryItem  m_aCategoryItem[ CATEGORY_ITEM_MAX ];

    app::tool::PaneList::PANE_DATA  m_aPaneListData[ ITEM_LIST_MAX ];
    app::tool::PaneListView*        m_pPaneListView;

    gfl2::lyt::LytTextBox*          m_pErrorMessageTextPane;

    NetAppLib::UI::NetAppMenuCursor* m_pMenuCursor;

    ButtonId                        m_pushId;
    ButtonId                        m_tmpPushId;

    u32                             m_pushItemIdx;
    u32                             m_pushPaneListIdx;
    u32                             m_listItemNum;

    app::util::Heap*  m_pPaneListViewHeap;
    app::tool::CommonMessageObject* m_pLowerCommonMessageObject;

    LowerItemSelectViewDelegate*    m_pDelegate;

    app::util::KeyRepeatController* m_pCategoryKeyRepatCtrl;
    u32                             m_acitveCategoryIdx;

    State                           m_state;

    bool                            m_bRefresh;
  };

  //  デリゲーター
  class LowerItemSelectViewDelegate
  {
  public:
    //  選択項目を押した
    virtual bool LowerItemSelectViewDelegate_OnPushItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView ) { return true; }

    //  選択項目にカーソルが設定された時に呼ばれる
    virtual void LowerItemSelectViewDelegate_OnCursorItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView ) = 0;

    //  選択項目にカーソルがヒットし続けている間はずっと呼ばれる
    virtual void LowerItemSelectViewDelegate_OnCursorHitItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView ) {};

    //  項目内のアイテムはきせかえ可能なのものか
    virtual bool LowerItemSelectViewDelegate_OnIsDressupOk( App::Kisekae::System::ItemDataBase* pItemData ) { return true; }

    //  項目更新
    virtual void LowerItemSelectViewDelegate_OnUpdateItem( App::Kisekae::System::ItemDataBase* pItemData, App::Kisekae::View::LowerItemSelectView::SelectItem* pItemView ) {}

    //  キー入力
    virtual void LowerItemSelectViewDelegate_OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) {}
  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // KISEKAE_LOWER_ITEM_SELECT_VIEW_H_INCLUDE
