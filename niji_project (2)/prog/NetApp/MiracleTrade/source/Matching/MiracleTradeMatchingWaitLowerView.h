// ============================================================================
/*
 * @file MiracleTradeMatchingWaitLowerView.h
 * @brief ミラクル交換のマッチング待機画面です。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEMATCHINGWAITLOWERVIEW_H_INCLUDE )
#define MIRACLETRADEMATCHINGWAITLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"
#include "NetStatic/NetAppLib/include/UI/NetAppCommonMessageWindowManipulator.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(Matching)


class MiracleTradeMatchingWaitLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public NetAppLib::UI::NetAppCommonMessageWindowManipulator::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeMatchingWaitLowerView );
public:
  MiracleTradeMatchingWaitLowerView( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork );
  virtual~MiracleTradeMatchingWaitLowerView();

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // NetAppCommonMessageWindowManipulator::IEventHandler
  virtual void CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID eEventID );

  void SetTime( u32 second );
  void SetStreamMessage( u32 messageID );
  bool IsEndStreamMessage() const { return m_bIsEndStreamMessage; }

  void SetVisibleTimeOutCounter( bool bVisible );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_MIRACLE_TRADE,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  void setupMessageWindowManipulator();
  void setupTextMessage();
  void setVisibleBackButton( bool bVisible );
  void showMessage( u32 messageID, bool isImmediateMessage, bool isShowTimerIcon );

private:

  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*                         m_pMiracleTradeWork;
  u32                                                                                m_MessageID;
  NetAppLib::UI::NetAppCommonMessageWindowManipulator                                m_MessageWindowManipulator;
  bool                                                                               m_bIsEndStreamMessage;

};


GFL_NAMESPACE_END(Matching)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEMATCHINGWAITLOWERVIEW_H_INCLUDE
