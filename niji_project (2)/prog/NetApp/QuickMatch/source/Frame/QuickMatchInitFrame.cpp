// ============================================================================
/*
 * @file QuickMatchInitFrame.cpp
 * @brief クイックマッチアプリの初期化プロセスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/QuickMatch/source/Frame/QuickMatchInitFrame.h"
#include "NetApp/QuickMatch/source/QuickMatchFrameResult.h"

#include "NetApp/QuickMatch/source/View/QuickMatchUpperView.h"
#include "NetApp/QuickMatch/source/View/QuickMatchLowerView.h"

#include <Fade/include/gfl2_FadeManager.h>
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>
#include <niji_conv_header/message/msg_quickmatch.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(Frame)


QuickMatchInitFrame::QuickMatchInitFrame(
  NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork ) :
  NetAppLib::System::NetAppFrameBase( pQuickMatchWork ),
  m_pQuickMatchWork( pQuickMatchWork ),
  m_initializeCount( 0 )
{
}


QuickMatchInitFrame::~QuickMatchInitFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool QuickMatchInitFrame::startup()
{
  m_pQuickMatchWork->CreateNetGame( m_pQuickMatchWork->GetDeviceHeap() );
  m_pQuickMatchWork->GetNetGame( m_initializeCount )->Initialize();

  if( m_pQuickMatchWork->IsRequestChange() == false )
  {//初回起動
    m_pQuickMatchWork->GetUpperView()->HideTextBox();
    m_pQuickMatchWork->GetLowerView()->HideListLayout();
    //@view おまちください
    m_pQuickMatchWork->GetLowerView()->SetupMessageTimerMode( msg_quickmatch_win_01 , false );
  }

  return true;
}


void QuickMatchInitFrame::setListener()
{
}


void QuickMatchInitFrame::removeListener()
{
}

void QuickMatchInitFrame::updateSequence()
{
  if( m_pQuickMatchWork->GetNetGame( m_initializeCount )->IsInitialize() == false )
  {
    return;
  }

  m_initializeCount++;
  if( m_initializeCount < System::QuickMatchWork::NETGAME_NUM )
  {
    m_pQuickMatchWork->GetNetGame( m_initializeCount )->Initialize();
    return;
  }

  bool isHost = m_pQuickMatchWork->GetNetGame( 0 )->IsMaster();
  if( m_pQuickMatchWork->IsRequestChange() )
  {
    isHost = m_pQuickMatchWork->IsRequestChangeHost();
  }
    
  if( isHost )
  {
    exitFrame( NetApp::QuickMatch::RESULT_START_HOST );
  }
  else
  {
    exitFrame( NetApp::QuickMatch::RESULT_START_CLIENT );
  }
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
