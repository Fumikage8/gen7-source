// ============================================================================
/*
 * @file JoinFestaGlobalAttractionMessageMenuLowerView.h
 * @brief メッセージメニュー表示
 * @date 2015.10.14
 * @author endo_akira
 */
// ============================================================================
#if !defined( JoinFestaGlobalAttractionMessageMenuLowerView_H_INCLUDE )
#define JoinFestaGlobalAttractionMessageMenuLowerView_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionMessageMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaGlobalAttractionMessageMenuLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public NetAppLib::UI::NetAppMessageMenu::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionMessageMenuLowerView );

public:

  JoinFestaGlobalAttractionMessageMenuLowerView(
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu );
  virtual~JoinFestaGlobalAttractionMessageMenuLowerView();

  void SetListener( NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionMessageMenuLowerViewListener* pJoinFestaGlobalAttractionMessageMenuLowerViewListener ) { m_pJoinFestaGlobalAttractionMessageMenuLowerViewListener = pJoinFestaGlobalAttractionMessageMenuLowerViewListener; }
  void RemoveListener() { m_pJoinFestaGlobalAttractionMessageMenuLowerViewListener = NULL; }

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // NetAppMessageMenu::IEventHandler
  virtual void MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode );

  void SetMessage( u32 messageID, bool isShowTimerIcon = true );
  void SetStreamMessage( u32 messageID, bool isShowTimerIcon = false );
  void SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon = false );
  void SetMessageYesNoBlackFilter( u32 messageID );

private:

  void showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG );

private:

  NetAppLib::System::ApplicationWorkBase*                                                                m_pWorkBase;
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionMessageMenuLowerViewListener*        m_pJoinFestaGlobalAttractionMessageMenuLowerViewListener;
  NetAppLib::UI::NetAppMessageMenu&                                                                      m_NetAppMessageMenu;
  u32                                                                                                    m_MessageID;


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JoinFestaGlobalAttractionMessageMenuLowerView_H_INCLUDE
