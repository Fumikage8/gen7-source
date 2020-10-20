// ============================================================================
/*
 * @file QuickMatchCancelExitFrame.cpp
 * @brief クイックマッチアプリのキャンセル終了プロセスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/QuickMatch/source/Frame/QuickMatchCancelExitFrame.h"
#include "NetApp/QuickMatch/source/QuickMatchFrameResult.h"

#include "NetApp/QuickMatch/source/View/QuickMatchUpperView.h"
#include "NetApp/QuickMatch/source/View/QuickMatchLowerView.h"

#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

#include <Fade/include/gfl2_FadeManager.h>
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>

#include <niji_conv_header/message/msg_quickmatch.h>




GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(Frame)


QuickMatchCancelExitFrame::QuickMatchCancelExitFrame(
  NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork ) :
  NetAppLib::System::NetAppFrameBase( pQuickMatchWork ),
  m_pQuickMatchWork( pQuickMatchWork )
{
}


QuickMatchCancelExitFrame::~QuickMatchCancelExitFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool QuickMatchCancelExitFrame::startup()
{
#ifdef GF_PLATFORM_CTR
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  pP2pConnectionManager->DisconnectStart();
#endif

  //@view終了が選ばれました
  m_pQuickMatchWork->GetLowerView()->SetupMessageStreamMode( msg_quickmatch_win_06 );
  return true;
}


void QuickMatchCancelExitFrame::setListener()
{
}


void QuickMatchCancelExitFrame::removeListener()
{
}

void QuickMatchCancelExitFrame::updateSequence()
{
  //@view メッセージ終了待ち
  u32 result = m_pQuickMatchWork->GetLowerView()->GetResult();
  if( result != View::QuickMatchLowerView::RESULT_EOM )
    return;

  m_pQuickMatchWork->SetAppResult( NetApp::QuickMatch::APP_RESULT_EXIT , Regulation::BATTLE_RULE_SINGLE , true );
  exitFrame( NetApp::QuickMatch::RESULT_EXIT_APP );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
