// ============================================================================
/*
 * @file BattleRoyalResultLowerBGView.h
 * @brief バトルロイヤル勝敗アプリの背景下画面を表示するクラス
 * @date 2015.10.26
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( BATTLE_ROYAL_RESULT_LOWER_BG_VIEW_H_INCLUDE )
#define BATTLE_ROYAL_RESULT_LOWER_BG_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)

  class BattleRoyalResultWork;

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(View)

  class BattleRoyalResultLowerBGView :
    public NetAppLib::System::NetApplicationViewBase,
    public app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleRoyalResultLowerBGView );
  public:
    /**
     * @enum ButtonId
     * 入力ボタンid一覧
     */
    enum ButtonId
    {
      BUTTON_ID_NEXT  = 0,
      BUTTON_ID_MAX,

      BUTTON_ID_NONE
    };

    BattleRoyalResultLowerBGView( App::BattleRoyalResult::System::BattleRoyalResultWork* pWork );
    virtual~BattleRoyalResultLowerBGView();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update(void);

    // UIView
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

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

    //  戻るボタン登場イベント
    void InEventBackButton();

    //  押したボタンを取得
    ButtonId  GetPushButtonId() const { return m_pushButtonId; }

  private:
    enum LayoutWorkID
    {
      LAYOUT_WORK_ID_LOWER  = 0,
    };

    enum StateType
    {
      STATE_TYPE_IDLE = 0,
      STATE_TYPE_IN_NEXT_BUTTON
    };

    void _Clear()
    {
      m_pWork         = NULL;
      m_state         = STATE_TYPE_IDLE;
      m_pushButtonId  = BUTTON_ID_NONE;
    }

    void _SetState( const StateType newState );

    App::BattleRoyalResult::System::BattleRoyalResultWork*     m_pWork;

    StateType m_state;
    ButtonId  m_pushButtonId;
  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

#endif // BATTLE_ROYAL_RESULT_LOWER_BG_VIEW_H_INCLUDE
