// ============================================================================
/*
 * @file JoinFestaAlertLowerView.h
 * @brief アラート下画面
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAALERTLOWERVIEW_H_INCLUDE )
#define JOINFESTAALERTLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAlertLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaAlertLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAlertLowerView );
public:
  JoinFestaAlertLowerView( 
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::System::ResourceID layoutResourceID );
  virtual~JoinFestaAlertLowerView();

  void SetListener( NetAppLib::JoinFestaUI::JoinFestaAlertLowerViewListener* pJoinFestaAlertLowerViewListener ){ m_pJoinFestaAlertLowerViewListener = pJoinFestaAlertLowerViewListener; }
  void RemoveListener() { m_pJoinFestaAlertLowerViewListener = NULL; }

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  void SetMessage( u32 messageID );
  void SetMessage( u32 messageID, u32 waitTime );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_ID_ARROW_LEFT,
    BUTTON_ID_ARROW_RIGHT,
    BUTTON_MAX
  };

  void setupButton();

private:

  NetAppLib::System::ApplicationWorkBase*                       m_pWorkBase;
  NetAppLib::JoinFestaUI::JoinFestaAlertLowerViewListener*      m_pJoinFestaAlertLowerViewListener;
  u32                                                           m_MessageID;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAALERTLOWERVIEW_H_INCLUDE
