// ============================================================================
/*
 * @file JoinFestaPlayerDetailFrame.cpp
 * @brief 詳細アプリの○○プロセスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerDetail/source/Frame/JoinFestaPlayerDetailFrame.h"
#include "NetApp/JoinFestaPlayerDetail/source/JoinFestaPlayerDetailFrameResult.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordSelectLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordDetailLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyLowerView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaPlayerDetailFrame::JoinFestaPlayerDetailFrame(
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork,
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewSystemAccessor* pJoinFestaPlayerDetailViewSystemAccessor ) :
  NetApp::JoinFestaPlayerDetail::Frame::JoinFestaPlayerDetailFrameBase( pJoinFestaPlayerDetailWork, pJoinFestaPlayerDetailViewSystemAccessor ),
  m_JoinFestaRecordSelectLowerViewListener(),
  m_JoinFestaRecordDetailLowerViewListener(),
  m_JoinFestaSurveyLowerViewListener()
{
}


JoinFestaPlayerDetailFrame::~JoinFestaPlayerDetailFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaPlayerDetailFrame::startup()
{
  NetAppLib::JoinFestaUI::JoinFestaRecordListManager& joinFestaRecordListManager = m_pJoinFestaPlayerDetailWork->GetJoinFestaRecordListManager();
  NetAppLib::JoinFestaUI::JoinFestaSurveyListManager& joinFestaSurveyListManager = m_pJoinFestaPlayerDetailWork->GetJoinFestaSurveyListManager();
  NetAppLib::JoinFestaUI::JoinFestaPlayerData& selectPlayer = m_pJoinFestaPlayerDetailWork->GetSelectPlayer();

  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaPlayerDetailWork->GetDeviceHeap();

  NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager* pJoinFestaRecordRankingManager = GFL_NEW( pDeviceHeap->GetLowerHandle() ) NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager();
  pJoinFestaRecordRankingManager->UpdateRankingList( &selectPlayer );

  if( selectPlayer.IsRelatonVip() )
  {
    selectPlayer.SetType( NetAppLib::JoinFestaUI::JoinFestaPlayerData::TYPE_VIP );
  }
  else
  {
    selectPlayer.SetType( NetAppLib::JoinFestaUI::JoinFestaPlayerData::TYPE_GUEST );
  }

  if( selectPlayer.IsVip() )
  {
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_BATTLE,  pJoinFestaRecordRankingManager->GetVipRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_BATTLE,  selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_TRADE,   pJoinFestaRecordRankingManager->GetVipRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_TRADE,   selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_GREED,   pJoinFestaRecordRankingManager->GetVipRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_GREED,   selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_POKEMON, pJoinFestaRecordRankingManager->GetVipRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_POKEMON, selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_DAILY,   pJoinFestaRecordRankingManager->GetVipRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_DAILY,   selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_CIRCLE,  pJoinFestaRecordRankingManager->GetVipRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_CIRCLE,  selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_VARIETY, pJoinFestaRecordRankingManager->GetVipRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_VARIETY, selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_TOTAL,   pJoinFestaRecordRankingManager->GetVipRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_TOTAL,   selectPlayer.GetJoinFestaFriendKey() ) );
  }
  else
  if( selectPlayer.IsGuest() )
  {
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_BATTLE,  pJoinFestaRecordRankingManager->GetGuestRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_BATTLE,  selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_TRADE,   pJoinFestaRecordRankingManager->GetGuestRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_TRADE,   selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_GREED,   pJoinFestaRecordRankingManager->GetGuestRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_GREED,   selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_POKEMON, pJoinFestaRecordRankingManager->GetGuestRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_POKEMON, selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_DAILY,   pJoinFestaRecordRankingManager->GetGuestRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_DAILY,   selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_CIRCLE,  pJoinFestaRecordRankingManager->GetGuestRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_CIRCLE,  selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_VARIETY, pJoinFestaRecordRankingManager->GetGuestRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_VARIETY, selectPlayer.GetJoinFestaFriendKey() ) );
    selectPlayer.SetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_TOTAL,   pJoinFestaRecordRankingManager->GetGuestRank( NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RECORD_CATEGORY_TOTAL,   selectPlayer.GetJoinFestaFriendKey() ) );
  }

  joinFestaRecordListManager.SetupRecordDataList( &selectPlayer );
  joinFestaSurveyListManager.SetupSurveyDataList( &selectPlayer );

  m_pJoinFestaPlayerDetailViewObjects->InputDisableAll();
  m_pJoinFestaPlayerDetailViewList->RemoveAllView();

  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordSelectLowerView()->SetVisibleUpdateButton( false );
  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordSelectLowerView()->SetVisibleDecideButton( false );
  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordSelectLowerView()->SetVisibleMenuCursor( false );

  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaSurveyLowerView()->SetVisibleUpdateButton( false );
  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaSurveyLowerView()->SetVisibleDecideButton( false );
  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaSurveyLowerView()->SetVisibleMenuCursor( false );

  NetAppLib::JoinFestaUI::JoinFestaRecordDataList& recordList = joinFestaRecordListManager.GetJoinFestaRecordDataList();
  NetAppLib::JoinFestaUI::JoinFestaSurveyDataList& surveyList = joinFestaSurveyListManager.GetJoinFestaSurveyDataList();

  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( &selectPlayer );

  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordSelectLowerView()->SetList( &recordList, selectPlayer.IsVip() );
  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaSurveyLowerView()->SetList( &surveyList, selectPlayer.IsVip() );

  m_pJoinFestaPlayerDetailViewList->AddUpperView( m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaPlayerDetailViewList->AddUpperView( m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView() );

  m_pJoinFestaPlayerDetailViewList->AddLowerView( m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaPlayerListBGLowerView() );
  m_pJoinFestaPlayerDetailViewList->AddLowerView( m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordSelectLowerView() );

  GFL_SAFE_DELETE( pJoinFestaRecordRankingManager );

  return true;
}


void JoinFestaPlayerDetailFrame::setListener()
{
  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordSelectLowerView()->SetListener( &m_JoinFestaRecordSelectLowerViewListener );
  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordSelectLowerView()->SetInputEnabled( true );

  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordDetailLowerView()->SetListener( &m_JoinFestaRecordDetailLowerViewListener );
  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaSurveyLowerView()->SetListener( &m_JoinFestaSurveyLowerViewListener );
}


void JoinFestaPlayerDetailFrame::removeListener()
{
  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordSelectLowerView()->RemoveListener();
  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordDetailLowerView()->RemoveListener();
  m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaSurveyLowerView()->RemoveListener();
}


void JoinFestaPlayerDetailFrame::updateSequence()
{
  recordSelectEventProcedure();
  recordDetailEventProcedure();
  surveyEventProcedure();
}


void JoinFestaPlayerDetailFrame::recordSelectEventProcedure()
{
  u32 inputEventID         = m_JoinFestaRecordSelectLowerViewListener.GetInputEventID();
  bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
  bool bPushLButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
  bool bPushRButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;
  bool bSelectRecord       = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaRecordSelectLowerViewListener::INPUT_EVENT_ID_SELECT_RECORD ) ? true : false;

  if( bTouchBackButton )
  {
    exitFrame( NetApp::JoinFestaPlayerDetail::DETAIL_FRAME_RESULT_BACK );
  }
  else
  if( bPushLButton || bPushRButton )
  {
    m_pJoinFestaPlayerDetailViewList->RemoveLowerView();
    m_pJoinFestaPlayerDetailViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaSurveyLowerView() );
  }
  else
  if( bSelectRecord )
  {
    m_pJoinFestaPlayerDetailViewList->RemoveLowerView();
    m_pJoinFestaPlayerDetailViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordDetailLowerView() );

    NetAppLib::JoinFestaUI::JoinFestaRecordData* pSelectRecord = m_JoinFestaRecordSelectLowerViewListener.GetSelectRecord();
    if( pSelectRecord )
    {
      m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordDetailLowerView()->SetRecordData( *pSelectRecord );
    }
  }

  m_JoinFestaRecordSelectLowerViewListener.ResetInputEvent();
}


void JoinFestaPlayerDetailFrame::recordDetailEventProcedure()
{
  u32 inputEventID         = m_JoinFestaRecordDetailLowerViewListener.GetInputEventID();
  bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;

  if( bTouchBackButton )
  {
    m_pJoinFestaPlayerDetailViewList->RemoveLowerView();
    m_pJoinFestaPlayerDetailViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordSelectLowerView() );
  }

  m_JoinFestaRecordDetailLowerViewListener.ResetInputEvent();
}


void JoinFestaPlayerDetailFrame::surveyEventProcedure()
{
  u32 inputEventID         = m_JoinFestaSurveyLowerViewListener.GetInputEventID();
  bool bTouchBackButton    = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
  bool bPushLButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
  bool bPushRButton        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;

  if( bTouchBackButton )
  {
    exitFrame( NetApp::JoinFestaPlayerDetail::DETAIL_FRAME_RESULT_BACK );
  }
  else
  if( bPushLButton || bPushRButton )
  {
    m_pJoinFestaPlayerDetailViewList->RemoveLowerView();
    m_pJoinFestaPlayerDetailViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerDetailViewObjects->GetJoinFestaRecordSelectLowerView() );
  }

  m_JoinFestaSurveyLowerViewListener.ResetInputEvent();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)
