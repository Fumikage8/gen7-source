// ============================================================================
/*
 * @file KisekaeLowerInfoView.h
 * @brief きせかえアプリの下画面を表示するクラスです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_LOWER_INFO_VIEW_H_INCLUDE )
#define KISEKAE_LOWER_INFO_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"

#include "App/Kisekae/source/System/KisekaeWork.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class CommonMessageObject;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(View)

  //  デリゲーター
  class LowerInfoViewDelegata
  {
  public:
    //  キー入力取得
    virtual bool LowerInfoViewDelegata_OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;
  };

  class LowerInfoView :
    public NetAppLib::System::NetApplicationViewBase,
    public app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( LowerInfoView );
  public:
    enum ButtonId
    {
      BUTTON_ID_CANCEL  = 0,
      BUTTON_ID_MAX,

      BUTTON_ID_NONE,
    };

    LowerInfoView( App::Kisekae::System::Work* pWork );
    virtual~LowerInfoView();

    //  アイドリングを開始
    void StartIdling();

    // UIView
    virtual void Update();
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
    virtual bool IsDrawing(void) const;

    //  デリゲータ設定
    void SetDelegate( LowerInfoViewDelegata* pDelegate ) { m_pDelegae = pDelegate; }

    //  押したボタン
    const ButtonId GetPushButtonId() const { return m_pushId; }

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

    enum LayoutResourceID
    {
      LAYOUT_RESOURCE_ID_CONNECTION,
    };

    enum LayoutWorkID
    {
      LAYOUT_WORK_ID_LOWER,
    };

  private:
    //  ステート
    enum State
    {
      STATE_IDLE  = 0,
      STATE_INPUT_EVENT_WAIT,
      STATE_INPUT_STOP
    };

    //  ステート設定
    void _SetState( const State newState );

    void _Clear()
    {
      m_pWork                     = NULL;
      m_pLowerCommonMessageObject = NULL;
      m_pDelegae  = NULL;

      m_state = STATE_IDLE;

      m_pushId  = m_tmpPushId = BUTTON_ID_NONE;
    }

    App::Kisekae::System::Work*     m_pWork;
    app::tool::CommonMessageObject* m_pLowerCommonMessageObject;

    State                           m_state;

    ButtonId  m_pushId, m_tmpPushId;
    LowerInfoViewDelegata*  m_pDelegae;
  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // KISEKAE_LOWER_INFO_VIEW_H_INCLUDE
