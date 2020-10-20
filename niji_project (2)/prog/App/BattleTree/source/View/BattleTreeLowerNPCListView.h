#ifndef BATTLE_TREE_NPC_LIST_VIEW_H_INCLUDED
#define BATTLE_TREE_NPC_LIST_VIEW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    BattleTreeNPCListView.h
 * @author  yuto_uchida
 * @date    2015.11.30
 * @brief   NPCトレーナーリストView
 */
//==============================================================================

// niji
#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"

#include "App/BattleTree/source/System/BattleTreeWork.h"
#include "AppLib/include/Tool/app_tool_ScrollPaneManager.h"
#include "AppLib/include/Util/app_util_ScrollBar.h"

#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class CommonMessageObject;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(View)

  //  delegate
  class LowerNPCTrainerListView_Delegate
  {
  public:
    virtual void LowerNPCTrainerListView_OnCursor( const u32 dataID ) = 0;
  };

  //------------------------------------------------------------------------------
  // @brief   NPCトレーナーリストView
  //------------------------------------------------------------------------------
  class LowerNPCTrainerListView :  public 
  NetAppLib::System::NetApplicationViewBase, 
  app::tool::ScrollPaneManager::Listener,
  app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( LowerNPCTrainerListView );
  public:
    enum ButtonId
    {
      BUTTON_ID_CANCEL  = 0,
      BUTTON_ID_MAX,
      BUTTON_ID_NONE,
      BUTTON_ID_SELECT,
    };

  public:
    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    LowerNPCTrainerListView( App::BattleTree::System::BattleTreeWork* pWork );

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    ~LowerNPCTrainerListView( void );

  public:
    //------------------------------------------------------------------
    /**
     * @brief   セットアップ
     */
    //------------------------------------------------------------------
    void Setup( void );

    //------------------------------------------------------------------
    /**
     * @brief   スクロールペインマネージャの破棄
     */
    //------------------------------------------------------------------
    void DeleteManager( void );

    //------------------------------------------------------------------
    /**
     * @brief   処理開始
     * @note    ポーズ状態から復帰する
     */
    //------------------------------------------------------------------
    void Start();

    //------------------------------------------------------------------
    /**
     * @brief   メニューカーソルの表示切替
     * @param   visible   表示フラグ
     */
    //------------------------------------------------------------------
    void SetVisibleCursor( bool visible );

    //  delegateを設定
    void SetDelegate( LowerNPCTrainerListView_Delegate* pDelegate ) { m_pDelegate = pDelegate; }

    //  押したボタンidを取得
    ButtonId  GetPushId();

    //  選択したdataID
    u32 GetSelectDataID() const { return m_selectDataId; }

    // UIView
    virtual void Update(void);
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
    virtual bool IsDrawing(void) const;

  public:   //!< リスナー
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

    /**
     * @brief リストの項目を描画するコールバック関数
     *
     * @param pane_index  カーソルがあっているペインのインデックス
     * @param dataID  データID
     * @param is_end_async  true非同期ではないor非同期で読み込みが終わっている
     *                      false非同期読み込み終わっていない
     */
    virtual void WriteTexture( u32 pane_index, u32 dataID, bool is_end_async );

    //------------------------------------------------------------------
    /**
      * @brief   決定ボタンの押下通知
      * @param   pane_index  選択されたボタンのインデックス
      * @param   dataID      選択されたデータのインデックス
      */
    //------------------------------------------------------------------
    virtual void Decide( u32 pane_index, u32 dataID );

    //------------------------------------------------------------------
    /**
      * @brief   カーソルの選択通知
      * @param   pane_index  選択されたボタンのインデックス
      * @param   dataID      選択されたデータのインデックス
      */
    //------------------------------------------------------------------
    virtual void OnCursor( u32 pane_index, u32 dataID );

  private:
    enum
    {
      LYT_WORK_ID = 0,
    };

    enum UpdateSeq
    {
      UPDATE_SEQ_IDLE = 0,
      UPDATE_SEQ_BTN_ANIM_WAIT,
      UPDATE_SEQ_PAUSE,
    };

    struct PANE_SETUP_DATA
    {
      gfl2::lyt::LytPaneIndex     paneIndex;
      u32                         hold_anime_index;     //トリガ時のアニメ
      u32                         release_anime_index;  //タッチで選んだ時のアニメ
      u32                         cancel_anime_index;   //ホールドしながら枠外へスライドしたときのアニメ
      u32                         select_anime_index;   //キーで選んだ時のアニメ
    };

    struct PANE_DATA
    {
      gfl2::lyt::LytPicture*  pTrainerIcon;
      gfl2::lyt::LytTextBox*  pTrainerNameTexBox;

      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA trainerIconObjData;
      u32                     trainerLoadId;
    };

    void _SetUpdateSeq( const UpdateSeq newSeq );

    void _Clear()
    {
      m_pWork                     = NULL;
      m_pScrollPaneManager        = NULL;
      m_pScrollBar                = NULL;
      m_pInfo                     = NULL;
      m_pMenuCursor               = NULL;
      m_pPaneDataList             = NULL;
      m_pLowerCommonMessageObject = NULL;
      m_pDelegate                 = NULL;

      m_selectDataId            = 0;
      m_pushId                  = BUTTON_ID_NONE;
      m_updateSeq               = UPDATE_SEQ_IDLE;
    }

    ButtonId                        m_pushId;

    App::BattleTree::System::BattleTreeWork*     m_pWork;

    // スクロールペインマネージャ
    app::tool::ScrollPaneManager*   m_pScrollPaneManager;
    //!< スクロールバー
    app::util::ScrollBar * m_pScrollBar;

    // スクロールペイン情報
    app::tool::ScrollPaneManager::PaneInfo*    m_pInfo;

    //  共通ウィンドウ
    app::tool::CommonMessageObject*   m_pLowerCommonMessageObject;

    PANE_DATA*                        m_pPaneDataList;

    NetAppLib::UI::NetAppMenuCursor*  m_pMenuCursor;

    UpdateSeq                         m_updateSeq;
    
    u32                               m_selectDataId;

    LowerNPCTrainerListView_Delegate* m_pDelegate;

    //  リストペインセットアップデータ
    static const PANE_SETUP_DATA  ma_sPaneSetupData[];
  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END( App )

#endif  // #ifndef BATTLE_TREE_NPC_LIST_VIEW_H_INCLUDED
