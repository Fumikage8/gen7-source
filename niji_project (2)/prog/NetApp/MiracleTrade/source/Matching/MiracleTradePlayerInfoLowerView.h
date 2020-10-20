// ============================================================================
/*
 * @file MiracleTradePlayerInfoLowerView.h
 * @brief ミラクル交換の交換相手の情報を表示する画面です。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEPLAYERINFOLOWERVIEW_H_INCLUDE )
#define MIRACLETRADEPLAYERINFOLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "NetApp/MiracleTrade/source/Matching/MiracleTradePlayerInfoLowerViewListener.h"
#include "NetStatic/NetAppLib/include/UI/NetAppCommonMessageWindowManipulator.h"

#ifdef GF_PLATFORM_CTR
#include <nn/cfg.h>
#include "System/include/CTR/CTR_CountryNameCache.h"
#include "System/include/CTR/CTR_RegionNameCache.h"
#endif // GF_PLATFORM_CTR

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(Matching)


class MiracleTradePlayerInfoLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener,
  public NetAppLib::UI::NetAppCommonMessageWindowManipulator::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradePlayerInfoLowerView );
public:
  MiracleTradePlayerInfoLowerView( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork );
  virtual~MiracleTradePlayerInfoLowerView();

  void SetListener( NetApp::MiracleTrade::Matching::MiracleTradePlayerInfoLowerViewListener* pMiracleTradePlayerInfoLowerViewListener ) { m_pMiracleTradePlayerInfoLowerViewListener = pMiracleTradePlayerInfoLowerViewListener; }
  void RemoveListener() { m_pMiracleTradePlayerInfoLowerViewListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  // NetAppCommonMessageWindowManipulator::IEventHandler
  virtual void CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID event );

  void SetupTextMessage();

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
  void setVisibleBackButton( bool bVisible );

private:

#ifdef GF_PLATFORM_CTR
  u8*                                                                               m_pSimpleAddressWorkMemory;
  System::CTR::CountryNameCache*                                                    m_pCountryNameCache;
  System::CTR::RegionNameCache*                                                     m_pRegionNameCache;
#endif // GF_PLATFORM_CTR
  gfl2::str::STRCODE                                                                m_TradePersonName[ System::STRLEN_PLAYER_NAME + System::EOM_LEN ];
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*                        m_pMiracleTradeWork;
  NetApp::MiracleTrade::Matching::MiracleTradePlayerInfoLowerViewListener*          m_pMiracleTradePlayerInfoLowerViewListener;
  NetAppLib::UI::NetAppCommonMessageWindowManipulator                               m_MessageWindowManipulator;

};


GFL_NAMESPACE_END(Matching)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEPLAYERINFOLOWERVIEW_H_INCLUDE
