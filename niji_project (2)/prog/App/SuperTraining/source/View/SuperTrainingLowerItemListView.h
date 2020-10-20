// ============================================================================
/*
 * @file SuperTrainingLowerItemListView.h
 * @brief すごい特訓アプリの下画面選択リストクラスです。
 * @date 2015.12.07
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINING_LOWER_ITEM_LIST_VIEW_H_INCLUDE )
#define SUPER_TRAINING_LOWER_ITEM_LIST_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"

#include "App/SuperTraining/source/System/SuperTrainingWork.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class KeyRepeatController;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(View)

  class LowerItemListView :
    public NetAppLib::System::NetApplicationViewBase,
    public app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( LowerItemListView );
  public:
    enum ButtonId
    {
      BUTTON_ID_CANCEL  = 0,
      BUTTON_ID_HP,
      BUTTON_ID_ATK,
      BUTTON_ID_DEF,
      BUTTON_ID_SP_ATK,
      BUTTON_ID_SP_DEF,
      BUTTON_ID_AGI,
      BUTTON_ID_START_TRAINIG,
      BUTTON_ID_MAX,

      BUTTON_ID_NONE,

      BUTTON_ID_SELECT_TYPE_MAX = BUTTON_ID_START_TRAINIG - BUTTON_ID_HP + 1,
    };


    LowerItemListView( App::SuperTraining::System::SuperTrainingWork* pWork );
    virtual~LowerItemListView();

    //  開始(Viewを使う前に必ず呼び出し。)
    void Start();

    //  すごい特訓ボタンにカーソル固定
    void LockCursorTrainigButton();

    //  押したボタンidを返す
    ButtonId GetPushButtonId() const { return m_pushButtonId; }

    //  チェックボックスの選択の設定を有効にするか
    void SetEnableChkBoxSelect( const ButtonId buttonId, const bool bFlg );

    //  チェックボックスの選択が有効化
    bool IsEnableChkBoxSelect( const ButtonId buttonId );

    //  選択項目のチェックの切り替え
    bool ChkChangeSelect( const ButtonId buttonid );

    //  選択項目のチェック設定
    void SetChkSelect( const ButtonId buttonId, const bool bFlg );

    //  選択項目にチェックがついているか
    bool IsChkSelect( const ButtonId buttonId );

    //  とっくん開始ボタンを表示
    void SetVisibleTrainingButton( const bool bFlg );

    //  アイテムインフォ表示設定
    void SetVisibleItemInfo( const bool bFlg );

    //  アイテムインフォの情報設定
    void SetItemInfoNumber( const u32 nowNum, const u32 maxNum );

    //  アイテムインフォの情報設定
    void SetItemInfoNumberItemLeftPos( const u32 num );

    //  アイテムインフォの情報に設定している所持数の取得
    u32 GetItemInfoNowNumber();

    // UIView
    virtual void Update(void);
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
    virtual bool IsDrawing(void) const;

  public:   //!< リスナー

    // ペインタッチ時を検知
    // 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
    virtual void OnLayoutPaneTouchTriggerEvent( const u32 buttonId );

    //------------------------------------------------------------------
    /**
      * @brief   ペインタッチ時を検知
      * @param[in] buttonId ボタンid
      * @return 入力処理を続けるかを返す
      */
    //------------------------------------------------------------------
    virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 buttonId );

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
      LAYOUT_WORK_ID  = 0,
    };

    //  更新シーケンス
    enum UpdateSeq
    {
      UPDATE_SEQ_IDLE = 0,
      UPDATE_SEQ_INPUT_ANIME,
      UPDATE_SEQ_PAUSE,
    };

    //  選択アイテムデータセットアップ
    struct SETUP_ITEM_DATA
    {
      u32 titleStrId;
      app::ui::ButtonInfoEx buttonInfo;
    };

    //  選択アイテム基本クラス
    class SelectItemBase
    {
    public:
      SelectItemBase();
      virtual ~SelectItemBase();

      //  初回設定
      virtual void Setup( LowerItemListView* pRootView, App::SuperTraining::System::SuperTrainingWork* pWork, const SETUP_ITEM_DATA& rSetup, gfl2::heap::HeapBase* pHeap );

      //  初期化処理
      void InitFunc();
      //  終了処理
      void EndFunc();

      void UpdateFunc();

      //  チェックマーク切り替え(派生先で実装しないといけない)
      virtual void ChangeMark() { GFL_ASSERT( 0 ); }
      virtual bool IsMark() { GFL_ASSERT( 0 ); return false; }
      virtual void SetMark( const bool bFlg ) { GFL_ASSERT( 0 ); }

      //  チェックマークそのもの有効
      virtual void SetEnableChkBox( const bool bFlg ) { GFL_ASSERT( 0 ); }
      virtual bool IsChkBox() { GFL_ASSERT( 0 ); return false; }

      //  アクティブにするかどうか
      void SetActive( const bool bActive );

      //  表示設定
      void SetVisible( const bool bFlg );

      //  表示しているかどうか
      bool IsVisible();

      //  項目選択がアクティブか
      bool IsSelectActive() const { return m_bActive; }

      //  ボタンid
      ButtonId  GetButtonId() const { return (ButtonId)m_setupItemData.buttonInfo.button_id; }

      //  カーソル位置ペイン
      gfl2::lyt::LytPane* GetCursorPosPane() const { return m_pCursorPosPane; }

    protected:
      LowerItemListView*  m_pRootView;
      gfl2::lyt::LytPane* m_pCursorPosPane;

    private:
      void _Clear()
      {
        m_pItemNameTextPane   = NULL;
        m_pCursorPosPane      = NULL;
        m_pRootView           = NULL;
        m_initSeq             = 0;
        m_endSeq              = 0;

        m_bActive = m_bOldActive  = true;

        gfl2::std::MemClear( &m_setupItemData, sizeof( m_setupItemData ) );
      }

      //  アイテム名
      gfl2::lyt::LytTextBox*  m_pItemNameTextPane;


      u32 m_initSeq;
      u32 m_endSeq;

      SETUP_ITEM_DATA m_setupItemData;

      bool  m_bActive, m_bOldActive;
    };

    //  チェックボックス選択アイテム
    class SelectCheckItem : public SelectItemBase
    {
    public:
      SelectCheckItem();
      virtual ~SelectCheckItem();

      virtual void Setup( LowerItemListView* pRootView, App::SuperTraining::System::SuperTrainingWork* pWork, const SETUP_ITEM_DATA& rSetup, gfl2::heap::HeapBase* pHeap );

      // マーク切り替え
      virtual void ChangeMark();
      virtual bool IsMark();
      virtual void SetMark( const bool bFlg );

      //  チェックマークそのもの有効
      virtual void SetEnableChkBox( const bool bFlg );
      virtual bool IsChkBox();

    private:
      void  _Clear()
      {
        m_pChkMarkPane        = NULL;
        m_pChkBoxPane         = NULL;
      }

      //  チェックボックス
      gfl2::lyt::LytPane*     m_pChkMarkPane;
      gfl2::lyt::LytPane*     m_pChkBoxPane;
    };

    //  とっくん開始選択アイテム
    class SelectTrainingItem : public SelectItemBase
    {
    public:
      SelectTrainingItem();
      virtual ~SelectTrainingItem();

      virtual void Setup( LowerItemListView* pRootView, App::SuperTraining::System::SuperTrainingWork* pWork, const SETUP_ITEM_DATA& rSetup, gfl2::heap::HeapBase* pHeap );
    };

    //  アイテムインフォ
    class ItemInfoObject
    {
    public:
      ItemInfoObject();
      virtual ~ItemInfoObject();

      void Setup( LowerItemListView* pRootView, App::SuperTraining::System::SuperTrainingWork* pWork );

      void SetNowNumber( const u32 num );
      u32 GetNowNumber() const { return m_nowNum; }

      void SetMaxNumber( const u32 num );

      void SetViaible( const bool bFlg );
    private:
      void _Clear()
      {
        m_pItemNameTextPane     = NULL;
        m_pNowItemNumTextPane   = NULL;
        m_pMaxItemNumTextPane   = NULL;
        m_pItemNumSlashTextPane = NULL;

        m_nowNum    = 0;
        m_pRootView = NULL;
      }

      gfl2::lyt::LytTextBox*  m_pItemNameTextPane;
      gfl2::lyt::LytTextBox*  m_pNowItemNumTextPane;
      gfl2::lyt::LytTextBox*  m_pMaxItemNumTextPane;
      gfl2::lyt::LytTextBox*  m_pItemNumSlashTextPane;

      u32                 m_nowNum;
      LowerItemListView*  m_pRootView;
    };

    //  更新シーケンス設定
    void _SetUpdateSeq( const UpdateSeq newSeq );

    //  チェックマーク選択ボタンか
    bool _IsSelectCheckItemButtonId( const ButtonId buttonId );


    //  カーソル移動
    //  カーソル移動したら true
    void _SetCursorMove( const s32 move );

    //  カーソル位置設定
    bool _SetCursorPos( const u32 pos );

    //  カーソル位置設定(ボタンid版)
    void _SetCursorButtonId( const ButtonId buttonId );

    void _Clear()
    {
      m_updateSeq               = UPDATE_SEQ_IDLE;
      m_tmpPushButtonId         = BUTTON_ID_NONE;
      m_pushButtonId            = BUTTON_ID_NONE;
      m_pWork                   = NULL;
      m_pMenuCursor             = NULL;
      m_pSelectItemKeyRepatCtrl = NULL;

      m_bLockCursor   = false;
      m_menuCursorPos = 0;

      gfl2::std::MemClear( m_pSelectItem, sizeof( m_pSelectItem ) );
      
    }

    UpdateSeq                                           m_updateSeq;
    ButtonId                                            m_pushButtonId;
    ButtonId                                            m_tmpPushButtonId;
    App::SuperTraining::System::SuperTrainingWork*      m_pWork;
    NetAppLib::UI::NetAppMenuCursor*                    m_pMenuCursor;
    u32                                                 m_menuCursorPos;
    u32                                                 m_menuCursorMax;

    bool                                                m_bLockCursor;
    app::util::KeyRepeatController*                     m_pSelectItemKeyRepatCtrl;

    ItemInfoObject                                      m_itemInfoObject;
    SelectItemBase*                                     m_pSelectItem[ BUTTON_ID_SELECT_TYPE_MAX ];
  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)

#endif // SUPER_TRAINING_LOWER_ITEM_LIST_VIEW_H_INCLUDE
