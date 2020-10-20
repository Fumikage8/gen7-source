// ============================================================================
/*
 * @file RegulationDLMainFrame.cpp
 * @brief レギュレーション選択アプリのレギュレーションデータをダウンロードするプロセスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/Frame/RegulationDLMainFrame.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectBGUpperView.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectFrameResult.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectViewObjects.h"
#include "NetApp/RegulationSelect/source/RequestSequence/RegulationDLRequestFacade.h"
#include <niji_conv_header/message/msg_regulation.h>


#if PM_DEBUG
// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <debug/include/gfl2_DebugWinSystem.h>
// niji
#include "Debug/DebugWin/include/DebugWinSystem.h"

#include "NetApp/RegulationSelect/source/RequestSequence/RegulationDLSearchDataRequestSequence.h"

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"
#endif // #if PM_DEBUG

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)

#if PM_DEBUG
static void UpdateTestDebug(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    GFL_PRINT("push button A \n");
  }
}
#endif // #if PM_DEBUG


RegulationDLMainFrame::RegulationDLMainFrame(
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects ) :
  NetApp::RegulationSelect::RegulationSelectFrameBase( pRegulationSelectWork ),
  m_pRegulationSelectViewObjects( pRegulationSelectViewObjects ),
  m_RegulationSelectMessageMenuLowerViewListener(),
  m_RequestHandle( gflnet2::ServerClient::INVALID_HANDLE ),
  m_bRecvSearchDataSuccess( false ),
  m_bRecvSearchDataError( false ),
  m_bRecvSearchDataError2( false ),
  m_bRecvSearchDataCancel( false )
{
#if PM_DEBUG
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();
  gfl2::debug::DebugWinGroup* pGroup = gfl2::debug::DebugWin_AddGroup( m_pRegulationSelectWork->GetAppHeap()->GetDeviceHeap(), pMsgData, msg_dmenu_regulation_main_top, NULL );   // "RegulationDLMainFrame"
  gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, m_pRegulationSelectWork->GetAppHeap()->GetDeviceHeap(), pMsgData, msg_dmenu_regulation_main_test , this, UpdateTestDebug, NULL );    // "Test"

#endif // #if PM_DEBUG
}


RegulationDLMainFrame::~RegulationDLMainFrame()
{
#if PM_DEBUG
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE( gfl2::debug::DebugWin)->GetWorkStrBuf();
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();

  pMsgData->GetString( msg_dmenu_regulation_main_top, *workStrBuf );  // "RegulationDLMainFrame"
  gfl2::debug::DebugWin_RemoveGroupByName( workStrBuf->GetPtr() );
#endif // #if PM_DEBUG
}


//-----------------------------------------------------------------------------
// RegulationSelectFrameBase
//-----------------------------------------------------------------------------
bool RegulationDLMainFrame::startup()
{
  m_pRegulationSelectViewObjects->InputDisableAll();
  m_pRegulationSelectWork->GetRootUpperView()->RemoveAllSubView();
  m_pRegulationSelectWork->GetRootLowerView()->RemoveAllSubView();

  addUpperViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationSelectBGUpperView() );
  addLowerViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView() );

  // データを探しています。しばらくお待ち下さい。
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessage( msg_reg_win_03 );

  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->BackgroundAndBackButtonVisible( true );

  m_RequestHandle = NetApp::RegulationSelect::RequestSequence::RegulationDLRequestFacade::AddSearchDataRequest( this, m_pRegulationSelectWork );

  return true;
}


void RegulationDLMainFrame::setListener()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetListener( &m_RegulationSelectMessageMenuLowerViewListener );

  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetInputEnabled( true );
}


void RegulationDLMainFrame::removeListener()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->RemoveListener();

  m_pRegulationSelectViewObjects->InputDisableAll();
}


void RegulationDLMainFrame::updateSequence()
{
  u32 inputEventID = m_RegulationSelectMessageMenuLowerViewListener.GetInputEventID();
  u32 messageID = m_RegulationSelectMessageMenuLowerViewListener.GetMessageID();

  switch( GetSubSeq() )
  {
    // ---------------------------------------------------------------------------------------------
    case UPDATE_SEQUENCE_DATA_DOWNLOAD:
    {
      if( inputEventID == NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON )
      {
        // データを探しています。しばらくお待ち下さい。
        if( messageID == msg_reg_win_03 )
        {
          // データを探すのをやめますか？
          m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageYesNoBlackFilter( msg_reg_win_06 );
          SetSubSeq( UPDATE_SEQUENCE_CANCEL_CONFIRM );
        }
      }
      else
      if( m_bRecvSearchDataSuccess )
      {
        exitFrame( NetApp::RegulationSelect::DL_MAIN_RESULT_SUCCESS );
      }
      else
      if( m_bRecvSearchDataError )
      {
        // データが見つかりませんでした。
        m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_reg_win_05 );
        SetSubSeq( UPDATE_SEQUENCE_NOT_FOUND_DATA );
      }
      else
      if( m_bRecvSearchDataError2 )
      {
        exitFrame( NetApp::RegulationSelect::DL_MAIN_RESULT_BACK );
      }
    }
    break;

    // ---------------------------------------------------------------------------------------------
    case UPDATE_SEQUENCE_NOT_FOUND_DATA:
    {
      if( inputEventID == NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN )
      {
        // データが見つかりませんでした。
        if( messageID == msg_reg_win_05 )
        {
          removeListener();
          exitFrame( NetApp::RegulationSelect::DL_MAIN_RESULT_BACK );
        }
      }
    }
    break;

    // ---------------------------------------------------------------------------------------------
    case UPDATE_SEQUENCE_CANCEL_CONFIRM:
    {
      if( inputEventID == NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON )
      {
        // データを探すのをやめますか？
        if( messageID == msg_reg_win_06 )
        {
          removeLowerView();
          if( m_RequestHandle != gflnet2::ServerClient::INVALID_HANDLE )
          {
            NetApp::RegulationSelect::RequestSequence::RegulationDLRequestFacade::CancelSearchDataRequest( m_RequestHandle );
            SetSubSeq( UPDATE_SEQUENCE_CANCEL_WAIT );
          }
          else
          {
            exitFrame( NetApp::RegulationSelect::DL_MAIN_RESULT_BACK );
          }
        }
      }
      else
      if( inputEventID == NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON )
      {
        // データを探すのをやめますか？
        if( messageID == msg_reg_win_06 )
        {
          m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetInputEnabled( true );

          // データを探しています。しばらくお待ち下さい。
          m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessage( msg_reg_win_03 );
          SetSubSeq( UPDATE_SEQUENCE_DATA_DOWNLOAD );
        }
      }
    }
    break;

    // ---------------------------------------------------------------------------------------------
    case UPDATE_SEQUENCE_CANCEL_WAIT:
    {
      if( m_bRecvSearchDataCancel )
      {
        exitFrame( NetApp::RegulationSelect::DL_MAIN_RESULT_BACK );
      }
    }
    break;
  }

  m_RegulationSelectMessageMenuLowerViewListener.ResetInputEvent();

}


//-----------------------------------------------------------------------------
// RegulationDLSearchDataResponseListener
//-----------------------------------------------------------------------------
void RegulationDLMainFrame::OnSearchDataSuccess()
{
  m_bRecvSearchDataSuccess = true;
  m_RequestHandle = gflnet2::ServerClient::INVALID_HANDLE;
}


void RegulationDLMainFrame::OnSearchDataError()
{
  m_bRecvSearchDataError = true;
  m_RequestHandle = gflnet2::ServerClient::INVALID_HANDLE;
}


void RegulationDLMainFrame::OnSearchDataError2()
{
  m_bRecvSearchDataError2 = true;
  m_RequestHandle = gflnet2::ServerClient::INVALID_HANDLE;
}


void RegulationDLMainFrame::OnSearchDataCancel()
{
  m_bRecvSearchDataCancel = true;
  m_RequestHandle = gflnet2::ServerClient::INVALID_HANDLE;
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
