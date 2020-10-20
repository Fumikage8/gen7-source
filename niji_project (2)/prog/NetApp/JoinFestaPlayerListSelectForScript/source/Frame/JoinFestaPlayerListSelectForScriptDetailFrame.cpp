// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptDetailFrame.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)の○○プロセスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectForScript/source/Frame/JoinFestaPlayerListSelectForScriptDetailFrame.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptFrameResult.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordSelectLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordDetailLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyLowerView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaPlayerListSelectForScriptDetailFrame::JoinFestaPlayerListSelectForScriptDetailFrame(
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork,
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewSystemAccessor* pJoinFestaPlayerListSelectForScriptViewSystemAccessor ) :
  NetApp::JoinFestaPlayerListSelectForScript::Frame::JoinFestaPlayerListSelectForScriptFrameBase( pJoinFestaPlayerListSelectForScriptWork, pJoinFestaPlayerListSelectForScriptViewSystemAccessor ),
  m_JoinFestaRecordSelectLowerViewListener(),
  m_JoinFestaRecordDetailLowerViewListener(),
  m_JoinFestaSurveyLowerViewListener()
{
}


JoinFestaPlayerListSelectForScriptDetailFrame::~JoinFestaPlayerListSelectForScriptDetailFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaPlayerListSelectForScriptDetailFrame::startup()
{
  NetAppLib::JoinFestaUI::JoinFestaRecordListManager& joinFestaRecordListManager = m_pJoinFestaPlayerListSelectForScriptWork->GetJoinFestaRecordListManager();
  NetAppLib::JoinFestaUI::JoinFestaSurveyListManager& joinFestaSurveyListManager = m_pJoinFestaPlayerListSelectForScriptWork->GetJoinFestaSurveyListManager();
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_pJoinFestaPlayerListSelectForScriptWork->GetSelectPlayer();

  joinFestaRecordListManager.SetupRecordDataList( pSelectPlayer );
  joinFestaSurveyListManager.SetupSurveyDataList( pSelectPlayer );

  m_pJoinFestaPlayerListSelectForScriptViewObjects->InputDisableAll();
  m_pJoinFestaPlayerListSelectForScriptViewList->RemoveAllView();

  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordSelectLowerView()->SetVisibleUpdateButton( false );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordSelectLowerView()->SetVisibleDecideButton( false );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordSelectLowerView()->SetVisibleMenuCursor( false );

  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaSurveyLowerView()->SetVisibleUpdateButton( false );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaSurveyLowerView()->SetVisibleDecideButton( false );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaSurveyLowerView()->SetVisibleMenuCursor( false );

  NetAppLib::JoinFestaUI::JoinFestaRecordDataList& recordList = joinFestaRecordListManager.GetJoinFestaRecordDataList();
  NetAppLib::JoinFestaUI::JoinFestaSurveyDataList& surveyList = joinFestaSurveyListManager.GetJoinFestaSurveyDataList();

  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pSelectPlayer );

  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordSelectLowerView()->SetList( &recordList, pSelectPlayer->IsVip() );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaSurveyLowerView()->SetList( &surveyList, pSelectPlayer->IsVip() );

  m_pJoinFestaPlayerListSelectForScriptViewList->AddUpperView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaPlayerListSelectForScriptViewList->AddUpperView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView() );

  m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListBGLowerView() );
  m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordSelectLowerView() );

  return true;
}


void JoinFestaPlayerListSelectForScriptDetailFrame::setListener()
{
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordSelectLowerView()->SetListener( &m_JoinFestaRecordSelectLowerViewListener );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordSelectLowerView()->SetInputEnabled( true );

  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordDetailLowerView()->SetListener( &m_JoinFestaRecordDetailLowerViewListener );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaSurveyLowerView()->SetListener( &m_JoinFestaSurveyLowerViewListener );
}


void JoinFestaPlayerListSelectForScriptDetailFrame::removeListener()
{
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordSelectLowerView()->RemoveListener();
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordDetailLowerView()->RemoveListener();
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaSurveyLowerView()->RemoveListener();
}


void JoinFestaPlayerListSelectForScriptDetailFrame::updateSequence()
{
  recordSelectEventProcedure();
  recordDetailEventProcedure();
  surveyEventProcedure();
}


void JoinFestaPlayerListSelectForScriptDetailFrame::recordSelectEventProcedure()
{
  u32 inputEventID         = m_JoinFestaRecordSelectLowerViewListener.GetInputEventID();
  bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
  bool bPushLButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
  bool bPushRButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;
  bool bSelectRecord       = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerViewListener::INPUT_EVENT_ID_SELECT_RECORD ) ? true : false;

  if( bTouchBackButton )
  {
    exitFrame( NetApp::JoinFestaPlayerListSelectForScript::DETAIL_FRAME_RESULT_BACK );
  }
  else
  if( bPushLButton || bPushRButton )
  {
    m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();
    m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaSurveyLowerView() );
  }
  else
  if( bSelectRecord )
  {
    m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();
    m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordDetailLowerView() );

    NetAppLib::JoinFestaUI::JoinFestaRecordData* pSelectRecord = m_JoinFestaRecordSelectLowerViewListener.GetSelectRecord();
    if( pSelectRecord )
    {
      m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordDetailLowerView()->SetRecordData( *pSelectRecord );
    }
  }

  m_JoinFestaRecordSelectLowerViewListener.ResetInputEvent();
}


void JoinFestaPlayerListSelectForScriptDetailFrame::recordDetailEventProcedure()
{
  u32 inputEventID         = m_JoinFestaRecordDetailLowerViewListener.GetInputEventID();
  bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;

  if( bTouchBackButton )
  {
    m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();
    m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordSelectLowerView() );
  }

  m_JoinFestaRecordDetailLowerViewListener.ResetInputEvent();
}


void JoinFestaPlayerListSelectForScriptDetailFrame::surveyEventProcedure()
{
  u32 inputEventID         = m_JoinFestaSurveyLowerViewListener.GetInputEventID();
  bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
  bool bPushLButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
  bool bPushRButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;

  if( bTouchBackButton )
  {
    exitFrame( NetApp::JoinFestaPlayerListSelectForScript::DETAIL_FRAME_RESULT_BACK );
  }
  else
  if( bPushLButton || bPushRButton )
  {
    m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();
    m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaRecordSelectLowerView() );
  }

  m_JoinFestaSurveyLowerViewListener.ResetInputEvent();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)
