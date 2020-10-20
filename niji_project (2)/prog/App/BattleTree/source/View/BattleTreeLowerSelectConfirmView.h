// ============================================================================
/*
 * @file LowerSelectConfirmView.h
 * @brief バトルツリーアプリの下画面選択確認表示するクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_LOWER_SELECT_CONFIRM_VIEW_H_INCLUDE )
#define BATTLE_TREE_LOWER_SELECT_CONFIRM_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"

#include "App/BattleTree/source/System/BattleTreeWork.h"

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class CommonMessageObject;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN( util )

  class  KeyRepeatController;

GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(View)

  class LowerSelectConfirmView :
    public NetAppLib::System::NetApplicationViewBase, app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( LowerSelectConfirmView );
  public:

    enum ButtonId
    {
      BUTTON_ID_NO  = 0,
      BUTTON_ID_YES,
      BUTTON_ID_CANCEL,
      BUTTON_ID_MAX,

      BUTTON_ID_NONE,
    };

    LowerSelectConfirmView( App::BattleTree::System::BattleTreeWork* pWork );
    virtual~LowerSelectConfirmView();

    // UIView
    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update(void);

    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
    virtual bool IsDrawing(void) const;

    //------------------------------------------------------------------
    /**
     * @brief   処理開始
     * @note    ポーズ状態から復帰する
     */
    //------------------------------------------------------------------
    void Start( const App::BattleTree::NPC_SELECT_ITEM* pItem );

    //  押したボタンid
    ButtonId  GetPushId();

    //------------------------------------------------------------------
    /**
      * @brief   ペインタッチ時を検知
      * @param[in] buttonId ボタンid
      * @return 入力処理を続けるかを返す
      */
    //------------------------------------------------------------------
    virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 id );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief ペインタッチ時を検知
    * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
    */
    //--------------------------------------------------------------------------------------------
    virtual void OnLayoutPaneTouchTriggerEvent( const u32 buttonId );

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
      LAYOUT_WORK_ID,
    };

    enum UpdateSeq
    {
      UPDATE_SEQ_IDLE = 0,
      UPDATE_SEQ_BTN_ANIM_WAIT,
      UPDATE_SEQ_PAUSE,
    };

    enum
    {
      MENU_CURSOR_POS_MAX = 2,
    };

    //  カーソル位置データ
    struct MENU_CURSOR_POS_DATA
    {
      gfl2::lyt::LytPane* pPosPane;
      ButtonId            buttonId;
    };

    //  カーソル移動
    void _SetCursorMove( const s32 move );

    //  カーソル位置設定
    void _SetCursorPos( const u32 pos );

    //  カーソル位置設定(ボタンid版)
    void _SetCursorButtonId( const ButtonId buttonId );

    //  決定
    void _PushEntry();

    //  更新シーケンス設定
    void  _SetUpdateSeq( const UpdateSeq newSeq );

    void _Clear()
    {
      m_pWork                     = NULL;
      m_pMenuCursor               = NULL;
      m_pLowerCommonMessageObject = NULL;
      m_pKeyRepeatCtrl            = NULL;
      m_pTrainerNameTextBox       = NULL;
      m_menuCursorPos             = 0;
      m_trainerLoadIconId         = 0;
      m_pushId                    = BUTTON_ID_NONE;
      m_updateSeq                 = UPDATE_SEQ_IDLE;
    }

    App::BattleTree::System::BattleTreeWork*      m_pWork;

    ButtonId                                      m_pushId;

    UpdateSeq                                     m_updateSeq;
    
    gfl2::lyt::LytTextBox*                        m_pTrainerNameTextBox;

    u32                                           m_trainerLoadIconId;

    u32                               m_menuCursorPos;
    MENU_CURSOR_POS_DATA              m_aMenuCursorPosData[ MENU_CURSOR_POS_MAX ];
    
    app::util::KeyRepeatController*   m_pKeyRepeatCtrl;

    //  共通ウィンドウ
    app::tool::CommonMessageObject*   m_pLowerCommonMessageObject;

    NetAppLib::UI::NetAppMenuCursor*  m_pMenuCursor;

    app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA m_trainerIconObjData;

  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

#endif // BATTLE_TREE_LOWER_SELECT_CONFIRM_VIEW_H_INCLUDE
