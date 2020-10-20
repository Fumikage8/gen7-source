// ============================================================================
/*
 * @file JoinFestaPlayerListShowDetailFrame.cpp
 * @brief 閲覧アプリのSDカード読み込みプロセスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListShow/source/Frame/JoinFestaPlayerListShowDetailFrame.h"
#include "NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowFrameResult.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordSelectLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordDetailLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyLowerView.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(Frame)



JoinFestaPlayerListShowDetailFrame::JoinFestaPlayerListShowDetailFrame(
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork,
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor* pJoinFestaPlayerListShowViewSystemAccessor ) :
  NetApp::JoinFestaPlayerListShow::Frame::JoinFestaPlayerListShowFrameBase( pJoinFestaPlayerListShowWork, pJoinFestaPlayerListShowViewSystemAccessor ),
  m_JoinFestaRecordSelectLowerViewListener(),
  m_JoinFestaRecordDetailLowerViewListener(),
  m_JoinFestaSurveyLowerViewListener()
{
}


JoinFestaPlayerListShowDetailFrame::~JoinFestaPlayerListShowDetailFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaPlayerListShowDetailFrame::startup()
{
  NetAppLib::JoinFestaUI::JoinFestaRecordListManager& joinFestaRecordListManager = m_pJoinFestaPlayerListShowWork->GetJoinFestaRecordListManager();
  NetAppLib::JoinFestaUI::JoinFestaSurveyListManager& joinFestaSurveyListManager = m_pJoinFestaPlayerListShowWork->GetJoinFestaSurveyListManager();
  NetAppLib::JoinFestaUI::JoinFestaPlayerData& selectPlayer = m_pJoinFestaPlayerListShowWork->GetSelectPlayer();

  joinFestaRecordListManager.SetupRecordDataList( &selectPlayer );
  joinFestaSurveyListManager.SetupSurveyDataList( &selectPlayer );

  m_pJoinFestaPlayerListShowViewObjects->InputDisableAll();
  m_pJoinFestaPlayerListShowViewList->RemoveAllView();

  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordSelectLowerView()->SetVisibleUpdateButton( false );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordSelectLowerView()->SetVisibleDecideButton( false );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordSelectLowerView()->SetVisibleMenuCursor( false );

  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaSurveyLowerView()->SetVisibleUpdateButton( false );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaSurveyLowerView()->SetVisibleDecideButton( false );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaSurveyLowerView()->SetVisibleMenuCursor( false );

  NetAppLib::JoinFestaUI::JoinFestaRecordDataList& recordList = joinFestaRecordListManager.GetJoinFestaRecordDataList();
  NetAppLib::JoinFestaUI::JoinFestaSurveyDataList& surveyList = joinFestaSurveyListManager.GetJoinFestaSurveyDataList();

  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( &selectPlayer );

  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordSelectLowerView()->SetList( &recordList, selectPlayer.IsVip() );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaSurveyLowerView()->SetList( &surveyList, selectPlayer.IsVip() );

  m_pJoinFestaPlayerListShowViewList->AddUpperView( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaPlayerListShowViewList->AddUpperView( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView() );

  m_pJoinFestaPlayerListShowViewList->AddLowerView( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListBGLowerView() );
  m_pJoinFestaPlayerListShowViewList->AddLowerView( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordSelectLowerView() );

  return true;
}


void JoinFestaPlayerListShowDetailFrame::setListener()
{
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordSelectLowerView()->SetListener( &m_JoinFestaRecordSelectLowerViewListener );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordSelectLowerView()->SetInputEnabled( true );

  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordDetailLowerView()->SetListener( &m_JoinFestaRecordDetailLowerViewListener );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaSurveyLowerView()->SetListener( &m_JoinFestaSurveyLowerViewListener );
}


void JoinFestaPlayerListShowDetailFrame::removeListener()
{
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordSelectLowerView()->RemoveListener();
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordDetailLowerView()->RemoveListener();
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaSurveyLowerView()->RemoveListener();
}


void JoinFestaPlayerListShowDetailFrame::updateSequence()
{
  recordSelectEventProcedure();
  recordDetailEventProcedure();
  surveyEventProcedure();

  m_JoinFestaRecordSelectLowerViewListener.ResetInputEvent();
  m_JoinFestaRecordDetailLowerViewListener.ResetInputEvent();
  m_JoinFestaSurveyLowerViewListener.ResetInputEvent();
}


void JoinFestaPlayerListShowDetailFrame::recordSelectEventProcedure()
{
  u32 inputEventID         = m_JoinFestaRecordSelectLowerViewListener.GetInputEventID();
  bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
  bool bPushLButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
  bool bPushRButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;
  bool bSelectRecord       = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerViewListener::INPUT_EVENT_ID_SELECT_RECORD ) ? true : false;

  if( bTouchBackButton )
  {
    exitFrame( NetApp::JoinFestaPlayerListShow::DETAIL_FRAME_RESULT_BACK );
  }
  else
  if( bPushLButton || bPushRButton )
  {
    m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
    m_pJoinFestaPlayerListShowViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaSurveyLowerView() );
  }
  else
  if( bSelectRecord )
  {
    m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
    m_pJoinFestaPlayerListShowViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordDetailLowerView() );

    NetAppLib::JoinFestaUI::JoinFestaRecordData* pSelectRecord = m_JoinFestaRecordSelectLowerViewListener.GetSelectRecord();
    if( pSelectRecord )
    {
      m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordDetailLowerView()->SetRecordData( *pSelectRecord );
    }
  }
}


void JoinFestaPlayerListShowDetailFrame::recordDetailEventProcedure()
{
  u32 inputEventID         = m_JoinFestaRecordDetailLowerViewListener.GetInputEventID();
  bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;

  if( bTouchBackButton )
  {
    m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
    m_pJoinFestaPlayerListShowViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordSelectLowerView() );
  }
}


void JoinFestaPlayerListShowDetailFrame::surveyEventProcedure()
{
  u32 inputEventID         = m_JoinFestaSurveyLowerViewListener.GetInputEventID();
  bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
  bool bPushLButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
  bool bPushRButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;

  if( bTouchBackButton )
  {
    exitFrame( NetApp::JoinFestaPlayerListShow::DETAIL_FRAME_RESULT_BACK );
  }
  else
  if( bPushLButton || bPushRButton )
  {
    m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
    m_pJoinFestaPlayerListShowViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaRecordSelectLowerView() );
  }
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)
