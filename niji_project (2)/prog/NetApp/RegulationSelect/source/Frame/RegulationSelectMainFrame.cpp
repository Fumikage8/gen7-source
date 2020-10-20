// ============================================================================
/*
 * @file RegulationSelectMainFrame.cpp
 * @brief レギュレーション選択アプリのレギュレーションデータを選択するプロセスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/Frame/RegulationSelectMainFrame.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainUpperView.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainLowerView.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectFrameResult.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectViewObjects.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include "Savedata/include/MyStatus.h"

#include <niji_conv_header/message/msg_regulation.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)


RegulationSelectMainFrame::RegulationSelectMainFrame(
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects ) :
  NetApp::RegulationSelect::RegulationSelectFrameBase( pRegulationSelectWork ),
  m_pRegulationSelectViewObjects( pRegulationSelectViewObjects ),
  m_RegulationSelectMainLowerViewListener(),
  m_RegulationSelectMessageMenuLowerViewListener(),
  m_RegulationList( m_pRegulationSelectWork->GetAppHeap()->GetDeviceHeap(), NetApp::RegulationSelect::PRESET_LIST_MAX + NetApp::RegulationSelect::DOWNLOADED_LIST_MAX )
{
}


RegulationSelectMainFrame::~RegulationSelectMainFrame()
{
}


//-----------------------------------------------------------------------------
// RegulationSelectFrameBase
//-----------------------------------------------------------------------------
bool RegulationSelectMainFrame::startup()
{
  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList = NULL;
  switch( m_pRegulationSelectWork->GetBootMode() )
  {
    case NetApp::RegulationSelect::BOOT_MODE_SELECT_P2P:
    {
      gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pPresetList = m_pRegulationSelectWork->GetPresetList();
      gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pDownloadedList = m_pRegulationSelectWork->GetDownloadedList();

      m_RegulationList.Erase();
      for( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>::Iterator it = pPresetList->Begin(); it != pPresetList->End(); ++it )
      {
        m_RegulationList.PushBack( *it );
      }

      Regulation::BATTLE_RULE eBattleRule = m_pRegulationSelectWork->GetBattleRule();

      for( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>::Iterator it = pDownloadedList->Begin(); it != pDownloadedList->End(); ++it )
      {
        if( eBattleRule == (*it)->GetRegulation().GetBattleRule() )
        {
          m_RegulationList.PushBack( *it );
        }
      }

      pRegulationList = &m_RegulationList;
    }
    break;

    default:
    {
      pRegulationList = m_pRegulationSelectWork->GetPresetList();
    }
    break;
  }

  m_pRegulationSelectViewObjects->InputDisableAll();
  m_pRegulationSelectWork->GetRootUpperView()->RemoveAllSubView();
  m_pRegulationSelectWork->GetRootLowerView()->RemoveAllSubView();

  addUpperViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView() );
  addLowerViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView() );

  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->CheckBoxDisable();
  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->SetRegulationList( pRegulationList, false, NetApp::RegulationSelect::Draw::RegulationSelectMainLowerView::LIST_TYPE_PRESET_DOWNLOADED );

  NetAppLib::UI::RegulationDrawInfo* pActiveSelectRegulationDrawInfo = m_pRegulationSelectWork->GetActiveSelectRegulationDrawInfo();

  if( pActiveSelectRegulationDrawInfo )
  {
    m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView()->SetRegulationDrawInfo( pActiveSelectRegulationDrawInfo );
  }
  else
  {
    NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = *pRegulationList->Begin();
    m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView()->SetRegulationDrawInfo( pRegulationDrawInfo );
    m_pRegulationSelectWork->SetActiveSelectRegulationDrawInfo( pRegulationDrawInfo );
  }

  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->BackgroundAndBackButtonVisible( false );

  return true;
}


void RegulationSelectMainFrame::setListener()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->SetListener( &m_RegulationSelectMainLowerViewListener );
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetListener( &m_RegulationSelectMessageMenuLowerViewListener );

  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->SetInputEnabled( true );
}


void RegulationSelectMainFrame::removeListener()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->RemoveListener();
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->RemoveListener();

  m_pRegulationSelectViewObjects->InputDisableAll();
}


void RegulationSelectMainFrame::updateSequence()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView()->SetRegulationDrawInfo( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );
  m_pRegulationSelectWork->SetActiveSelectRegulationDrawInfo( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );

  switch( m_RegulationSelectMainLowerViewListener.GetInputEventID() )
  {
    case NetApp::RegulationSelect::Draw::RegulationSelectMainLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      addLowerViewInputEnable( m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView() );

      m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageYesNoBlackFilter( msg_reg_win_18 );
    }
    break;

    case NetApp::RegulationSelect::Draw::RegulationSelectMainLowerViewListener::INPUT_EVENT_ID_SELECT_REGULATION:
    {
      addLowerViewInputEnable( m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView() );
      
      m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageMenuBlackFilter( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo(), msg_reg_win_17, msg_reg_sel_07, msg_reg_sel_08, msg_reg_sel_05 );
    }
    break;
  }

  switch( m_RegulationSelectMessageMenuLowerViewListener.GetInputEventID() )
  {
    case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON:
    {
      m_pRegulationSelectWork->SetSelectRegulationDrawInfo( NULL );

      exitFrame( NetApp::RegulationSelect::SELECT_MAIN_RESULT_BACK );
    }
    break;

    case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON:
    {
      removeLowerView();
    }
    break;

    case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
    {
      switch( m_RegulationSelectMessageMenuLowerViewListener.GetMessageID() )
      {
        case msg_reg_win_19:
        {
          removeLowerView();
        }
        break;
      }
    }
    break;

    case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_SELECT_MENU:
    {
      switch( m_RegulationSelectMessageMenuLowerViewListener.GetMessageID() )
      {
        case msg_reg_win_17:
        {
          switch( m_RegulationSelectMessageMenuLowerViewListener.GetSelectMenuID() )
          {
            // 決定する
            case msg_reg_sel_07:
            {
              switch( m_pRegulationSelectWork->GetBootMode() )
              {
                case NetApp::RegulationSelect::BOOT_MODE_SELECT_P2P:
                {
                  if( isEntry( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() ) )
                  {
                    m_pRegulationSelectWork->SetSelectRegulationDrawInfo( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );

                    exitFrame( NetApp::RegulationSelect::SELECT_MAIN_RESULT_SELECT_REGULATION );
                  }
                  else
                  {
                    removeLowerView();

                    addLowerViewInputEnable( m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView() );
      
                    m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_reg_win_19 );
                  }
                }
                break;
                
                default:
                {
                  m_pRegulationSelectWork->SetSelectRegulationDrawInfo( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );

                  exitFrame( NetApp::RegulationSelect::SELECT_MAIN_RESULT_SELECT_REGULATION );
                }
                break;
              }
            }
            break;

            // 詳しく見る
            case msg_reg_sel_08:
            {
              removeLowerView();

              m_pRegulationSelectWork->SetDetailBootInfo( NetApp::RegulationSelect::DETAIL_BOOT_FRAME_SELECT_MAIN, m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );

              exitFrame( NetApp::RegulationSelect::SELECT_MAIN_RESULT_TO_DETAIL );
            }
            break;

            // 戻る
            case msg_reg_sel_05:
            {
              removeLowerView();
            }
            break;
          }
        }
        break;
      }
    }
    break;
  }

  m_RegulationSelectMainLowerViewListener.ResetInputEvent();
  m_RegulationSelectMessageMenuLowerViewListener.ResetInputEvent();

}


bool RegulationSelectMainFrame::isEntry( NetAppLib::UI::RegulationDrawInfo* pSelectRegulationDrawInfo )
{
  bool bIsEntry = true;

  if( pSelectRegulationDrawInfo )
  {
    NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

    for( int i = 0; i < 4; ++i )
    {
      Savedata::MyStatus* pMyStatus = pP2pConnectionManager->GetMyStatus(i);
    
      if( pMyStatus )
      {
        if( !pSelectRegulationDrawInfo->GetRegulation().CheckParamRom( static_cast<int>( pMyStatus->GetRomCode() ) ) )
        {
          bIsEntry = false;
        }
      }
    }
  }
  else
  {
    bIsEntry = false;
  }

  return bIsEntry;
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
