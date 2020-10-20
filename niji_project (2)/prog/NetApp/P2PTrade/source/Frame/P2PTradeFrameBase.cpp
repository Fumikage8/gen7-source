// ============================================================================
/*
 * @file P2PTradeFrameBase.cpp
 * @brief P2P交換アプリのフレームの基底クラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/Frame/P2PTradeFrameBase.h"
#include "NetApp/P2PTrade/source/System/P2PTradeViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(Frame)


P2PTradeFrameBase::P2PTradeFrameBase(
  NetApp::P2PTrade::System::P2PTradeWork* pP2PTradeWork,
  NetApp::P2PTrade::System::P2PTradeViewSystemAccessor* pP2PTradeViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pP2PTradeWork ),
  m_pP2PTradeWork( pP2PTradeWork ),
  m_pP2PTradeViewList( pP2PTradeViewSystemAccessor->GetP2PTradeViewList() ),
  m_pP2PTradeViewObjects( pP2PTradeViewSystemAccessor->GetP2PTradeViewObjects() )
{
}


P2PTradeFrameBase::~P2PTradeFrameBase()
{
}


void P2PTradeFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pP2PTradeViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)
