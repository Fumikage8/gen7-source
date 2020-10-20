// ============================================================================
/*
 * @file MiracleTradeStartConfirmLowerView.h
 * @brief ミラクル交換の開始確認を行う画面を表示するクラスです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADESTARTCONFIRMLOWERVIEW_H_INCLUDE )
#define MIRACLETRADESTARTCONFIRMLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/UI/NetAppCursorView.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"
#include "NetApp/MiracleTrade/source/StartConfirm/MiracleTradeStartConfirmLowerViewListener.h"
#include "NetStatic/NetAppLib/include/UI/NetAppCommonMessageWindowManipulator.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(StartConfirm)


class MiracleTradeStartConfirmLowerView :
  public NetAppLib::UI::NetAppCursorView,
  public NetAppLib::UI::NetAppCommonMessageWindowManipulator::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeStartConfirmLowerView );
public:
  MiracleTradeStartConfirmLowerView( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork );
  virtual~MiracleTradeStartConfirmLowerView();

  void SetListener( NetApp::MiracleTrade::StartConfirm::MiracleTradeStartConfirmLowerViewListener* pMiracleTradeStartConfirmLowerViewListener ) { m_pMiracleTradeStartConfirmLowerViewListener = pMiracleTradeStartConfirmLowerViewListener; }
  void RemoveListener() { m_pMiracleTradeStartConfirmLowerViewListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

  // NetAppCommonMessageWindowManipulator::IEventHandler
  virtual void CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID event );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_MIRACLE_TRADE,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum ButtonID
  {
    BUTTON_ID_BUTTON_BACK,
    BUTTON_ID_BUTTON_START,
    BUTTON_MAX,
  };

  void setupMessageWindowManipulator();
  void setupTextMessage();
  void setupButton();

private:

  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*                         m_pMiracleTradeWork;
  NetApp::MiracleTrade::StartConfirm::MiracleTradeStartConfirmLowerViewListener*     m_pMiracleTradeStartConfirmLowerViewListener;
  NetAppLib::UI::NetAppCommonMessageWindowManipulator                                m_MessageWindowManipulator;

};


GFL_NAMESPACE_END(StartConfirm)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADESTARTCONFIRMLOWERVIEW_H_INCLUDE
