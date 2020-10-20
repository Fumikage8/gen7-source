// ============================================================================
/*
 * @file RegulationDLLoadFrame.cpp
 * @brief レギュレーション選択アプリのSDカードからレギュレーションデータをロードするプロセスです。
 * @date 2015.06.30
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/Frame/RegulationDLLoadFrame.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectBGUpperView.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectFrameResult.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectViewObjects.h"

#include <niji_conv_header/message/msg_regulation.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)


RegulationDLLoadFrame::RegulationDLLoadFrame(
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects ) :
  NetApp::RegulationSelect::Frame::RegulationSelectSDCardLoadFramBase( pRegulationSelectWork ),
  m_pRegulationSelectViewObjects( pRegulationSelectViewObjects ),
  m_RegulationSelectMessageMenuLowerViewListener()
{
}


RegulationDLLoadFrame::~RegulationDLLoadFrame()
{
}


//-----------------------------------------------------------------------------
// RegulationSelectFrameBase
//-----------------------------------------------------------------------------
bool RegulationDLLoadFrame::startup()
{
  m_pRegulationSelectViewObjects->InputDisableAll();
  m_pRegulationSelectWork->GetRootUpperView()->RemoveAllSubView();
  m_pRegulationSelectWork->GetRootLowerView()->RemoveAllSubView();

  addUpperViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationSelectBGUpperView() );
  addLowerViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView() );
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageBlackFilter( msg_reg_win_01 );

  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->BackgroundAndBackButtonVisible( true );

  loadRegulationData( false );

  return true;
}


void RegulationDLLoadFrame::setListener()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetListener( &m_RegulationSelectMessageMenuLowerViewListener );
}


void RegulationDLLoadFrame::removeListener()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->RemoveListener();

  m_pRegulationSelectViewObjects->InputDisableAll();
}


void RegulationDLLoadFrame::updateSequence()
{
  switch( m_RegulationSelectMessageMenuLowerViewListener.GetInputEventID() )
  {
    case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
    {
      switch( m_RegulationSelectMessageMenuLowerViewListener.GetMessageID() )
      {
        case msg_reg_win_02:
          exitFrame( NetApp::RegulationSelect::DL_LOAD_RESULT_SUCCESS_TO_MAIN );
        break;
      }
    }
    break;
  }

  m_RegulationSelectMessageMenuLowerViewListener.ResetInputEvent();
}


//-----------------------------------------------------------------------------
// RegulationSelectSDCardLoadFramBase
//-----------------------------------------------------------------------------
void RegulationDLLoadFrame::onLoadRegulationDataSuccess()
{
  exitFrame( NetApp::RegulationSelect::DL_LOAD_RESULT_SUCCESS_TO_MAIN );
}


void RegulationDLLoadFrame::onLoadRegulationDataError()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_reg_win_02 );
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetInputEnabled( true );
}


void RegulationDLLoadFrame::onLoadRegulationDataFatalError()
{
  m_pRegulationSelectWork->SetFatalError( true );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
