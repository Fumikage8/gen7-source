// ============================================================================
/*
 * @file MiracleTradeStartConfirmFrame.cpp
 * @brief ミラクル交換を開始するか確認するプロセスです。
 * @date 2015.05.25
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/StartConfirm/MiracleTradeStartConfirmFrame.h"
#include "NetApp/MiracleTrade/source/StartConfirm/MiracleTradeStartConfirmUpperView.h"
#include "NetApp/MiracleTrade/source/StartConfirm/MiracleTradeStartConfirmLowerView.h"
#include "NetApp/MiracleTrade/source/MiracleTradeFrameResult.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeViewObjects.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(StartConfirm)


MiracleTradeStartConfirmFrame::MiracleTradeStartConfirmFrame(
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork,
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects* pMiracleTradeViewObjects ) :
  NetApp::MiracleTrade::MiracleTradeFrameBase( pMiracleTradeWork ),
  m_pMiracleTradeWork( pMiracleTradeWork ),
  m_pMiracleTradeViewObjects( pMiracleTradeViewObjects )
{
}


MiracleTradeStartConfirmFrame::~MiracleTradeStartConfirmFrame()
{
}


//-----------------------------------------------------------------------------
// MiracleTradeFrameBase
//-----------------------------------------------------------------------------
bool MiracleTradeStartConfirmFrame::startup()
{
  m_pMiracleTradeViewObjects->InputDisableAll();
  m_pMiracleTradeWork->GetRootUpperView()->RemoveAllSubView();
  m_pMiracleTradeWork->GetRootLowerView()->RemoveAllSubView();

  addUpperViewInputDisable( m_pMiracleTradeViewObjects->GetMiracleTradeStartConfirmUpperView() );
  addLowerViewInputDisable( m_pMiracleTradeViewObjects->GetMiracleTradeStartConfirmLowerView() );

  return true;
}


void MiracleTradeStartConfirmFrame::setListener()
{
  m_pMiracleTradeViewObjects->GetMiracleTradeStartConfirmLowerView()->SetListener( this );
  m_pMiracleTradeViewObjects->GetMiracleTradeStartConfirmLowerView()->SetInputEnabled( true );
}


void MiracleTradeStartConfirmFrame::removeListener()
{
  m_pMiracleTradeViewObjects->GetMiracleTradeStartConfirmLowerView()->RemoveListener();

  m_pMiracleTradeViewObjects->InputDisableAll();
}


//-----------------------------------------------------------------------------
// MiracleTradeStartConfirmLowerViewListener
//-----------------------------------------------------------------------------
void MiracleTradeStartConfirmFrame::OnTouchBackButtonFromMiracleTradeStartConfirmLower()
{
  exitFrame( NetApp::MiracleTrade::START_CONFIRM_RESULT_BACK );
}


void MiracleTradeStartConfirmFrame::OnTouchStartButton()
{
  m_pMiracleTradeWork->SetBootProcID( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork::BOOT_PROC_ID_BOX );

  exitFrame( NetApp::MiracleTrade::START_CONFIRM_RESULT_START );
}


GFL_NAMESPACE_END(StartConfirm)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)
