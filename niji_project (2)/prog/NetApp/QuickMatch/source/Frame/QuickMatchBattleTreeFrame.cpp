// ============================================================================
/*
 * @file QuickMatchBattleTreeFrame.cpp
 * @brief クイックマッチアプリの初期化プロセスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/QuickMatch/source/Frame/QuickMatchBattleTreeFrame.h"
#include "NetApp/QuickMatch/source/QuickMatchFrameResult.h"

#include "NetApp/QuickMatch/source/View/QuickMatchUpperView.h"
#include "NetApp/QuickMatch/source/View/QuickMatchLowerView.h"

#include <Fade/include/gfl2_FadeManager.h>
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>
#include <niji_conv_header/message/msg_quickmatch.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(Frame)


QuickMatchBattleTreeFrame::QuickMatchBattleTreeFrame(
  NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork ) :
  NetAppLib::System::NetAppFrameBase( pQuickMatchWork ),
  m_pQuickMatchWork( pQuickMatchWork ),
  m_pQuickMatchDraw( NULL ),
  m_isEnd(false)
{
  m_pQuickMatchDraw = GFL_NEW( m_pQuickMatchWork->GetDeviceHeap() ) App::FieldMenu::QuickMatchDraw( m_pQuickMatchWork->GetAppHeap() , m_pQuickMatchWork->GetAppRenderingManager() );
}


QuickMatchBattleTreeFrame::~QuickMatchBattleTreeFrame()
{
  GFL_SAFE_DELETE( m_pQuickMatchDraw );
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool QuickMatchBattleTreeFrame::startup()
{
  if( m_pQuickMatchDraw->Initialize() == false )
  {
    return false;
  }
  m_pQuickMatchDraw->SetListener( this );

  m_pQuickMatchWork->GetLowerView()->AddSubView( m_pQuickMatchDraw );

  return true;
}

bool QuickMatchBattleTreeFrame::cleanup()
{
  m_pQuickMatchDraw->SetInputEnabled( false );
  m_pQuickMatchDraw->RemoveFromSuperView();
  if( m_pQuickMatchDraw->Terminate() == false )
  {
    return false;
  }
  GFL_SAFE_DELETE( m_pQuickMatchDraw );

  return true;
}

void QuickMatchBattleTreeFrame::setListener()
{
}


void QuickMatchBattleTreeFrame::removeListener()
{
}

void QuickMatchBattleTreeFrame::updateSequence()
{
  if( m_isEnd )
  {
    exitFrame( NetApp::QuickMatch::RESULT_EXIT_APP );
  }
}

void QuickMatchBattleTreeFrame::OnQuickMatchDrawExit(void)
{
  m_isEnd = true;
  m_pQuickMatchWork->m_pAppParam->m_isBattleTreeConnected = false;
  m_pQuickMatchDraw->SetInputEnabled( false );
}

void QuickMatchBattleTreeFrame::OnQuickMatchDrawNext(void)
{
  m_isEnd = true;
  m_pQuickMatchWork->m_pAppParam->m_isBattleTreeConnected = true;
  m_pQuickMatchDraw->SetInputEnabled( false );
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
