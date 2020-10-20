// ============================================================================
/*
 * @file QuickMatchClientFrame.cpp
 * @brief クイックマッチアプリのクライアントプロセスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/QuickMatch/source/Frame/QuickMatchClientFrame.h"
#include "NetApp/QuickMatch/source/QuickMatchFrameResult.h"

#include "NetApp/QuickMatch/source/View/QuickMatchUpperView.h"
#include "NetApp/QuickMatch/source/View/QuickMatchLowerView.h"

#include <Fade/include/gfl2_FadeManager.h>
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>

#include <niji_conv_header/message/msg_quickmatch.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(Frame)


QuickMatchClientFrame::QuickMatchClientFrame(
  NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork ) :
  NetAppLib::System::NetAppFrameBase( pQuickMatchWork ),
  m_pQuickMatchWork( pQuickMatchWork ),
  m_isLastUpdate( false ),
  m_lastTimingNo(0)
{
}


QuickMatchClientFrame::~QuickMatchClientFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool QuickMatchClientFrame::startup()
{
  m_pQuickMatchWork->GetUpperView()->HideTextBox();
  m_pQuickMatchWork->GetLowerView()->HideListLayout();
  //@view ホストの選択をおまちください
  m_pQuickMatchWork->GetLowerView()->SetupMessageTimerMode( msg_quickmatch_win_07 , false );

  //@view XXXさんが選んでいます...
  m_pQuickMatchWork->GetUpperView()->ShowClientMessage();

  for( u32 index = 0 ; index < System::QuickMatchWork::NETGAME_NUM ; ++index )
  {
    u32 timingNo = 0;
    switch( index )
    {
    //case NETGAME_CLIENT_EXIT:
    case System::QuickMatchWork::NETGAME_HOST_EXIT:
      if( timingNo == 0 )
        timingNo = System::QuickMatchWork::TIMING_HOST_EXIT;
    case System::QuickMatchWork::NETGAME_ON_TRADE:
      if( timingNo == 0 )
        timingNo = System::QuickMatchWork::TIMING_ON_TRADE;
    case System::QuickMatchWork::NETGAME_ON_SINGLE:
      if( timingNo == 0 )
        timingNo = System::QuickMatchWork::TIMING_ON_SINGLE;
    case System::QuickMatchWork::NETGAME_ON_DOUBLE:
      if( timingNo == 0 )
        timingNo = System::QuickMatchWork::TIMING_ON_DOUBLE;
    case System::QuickMatchWork::NETGAME_ON_CHANGE:
      if( timingNo == 0 )
        timingNo = System::QuickMatchWork::TIMING_ON_CHANGE;

      {
        m_pQuickMatchWork->GetNetGame( index )->TimingStart( timingNo );
      }
      break;
    }
  }

  return true;
}


void QuickMatchClientFrame::setListener()
{
}


void QuickMatchClientFrame::removeListener()
{
}

void QuickMatchClientFrame::updateSequence()
{
  u32 result = m_pQuickMatchWork->GetLowerView()->GetResult();
  if( result == View::QuickMatchLowerView::RESULT_NONE )
    return;

  //通信制御
  s32 messageID = CheckTimingSeq();
  LastUpdateSeq();

  //Timingが来た
  if( messageID != INVALID_MESSAGE_ID )
  {
    m_pQuickMatchWork->GetLowerView()->SetupMessageTimerMode( messageID );//進行メッセージ表示
  }

  if( m_isLastUpdate )
    return;
  

  if( result == View::QuickMatchLowerView::RESULT_EXIT )//@view Bボタンがおされたら検出
  {//@view YesNo表示
    m_pQuickMatchWork->GetLowerView()->SetupYesNoMode();
  }
  else if( result == View::QuickMatchLowerView::RESULT_YES )//@view YesNo結果取得
  {//Yes
    m_pQuickMatchWork->GetLowerView()->SetupMessageTimerMode( msg_quickmatch_win_04 );//通信を終了しています
    m_pQuickMatchWork->GetNetGame( System::QuickMatchWork::NETGAME_CLIENT_EXIT )->TimingStart( System::QuickMatchWork::TIMING_CLIENT_EXIT );
  }
  else if( result == View::QuickMatchLowerView::RESULT_NO )
  {//No
    m_pQuickMatchWork->GetLowerView()->SetupMessageTimerMode( msg_quickmatch_win_07 , false );//ホストをお待ち下さい
  }
}

//通信同期確認
s32 QuickMatchClientFrame::CheckTimingSeq()
{
  s32 MESSAGE_ID_LIST[] = 
  {
    INVALID_MESSAGE_ID,                               //NETGAME_CLIENT_EXIT = 0,
    INVALID_MESSAGE_ID,                               //NETGAME_HOST_EXIT, 
    msg_quickmatch_win_11,//トレード  //NETGAME_ON_TRADE,
    msg_quickmatch_win_08,//シングル  //NETGAME_ON_SINGLE,
    msg_quickmatch_win_09,//ダブル    //NETGAME_ON_DOUBLE,
    msg_quickmatch_win_12,//交代      //NETGAME_ON_CHANGE,
  };

  s32 messageID = INVALID_MESSAGE_ID;
  for( u32 index = 0 ; index < System::QuickMatchWork::NETGAME_NUM ; ++index )
  {
    u32 timingNo = 0;
    switch( index )
    {
    case System::QuickMatchWork::NETGAME_CLIENT_EXIT:
      if( timingNo == 0 )
        timingNo = System::QuickMatchWork::TIMING_CLIENT_EXIT;
    case System::QuickMatchWork::NETGAME_HOST_EXIT:
      if( timingNo == 0 )
        timingNo = System::QuickMatchWork::TIMING_HOST_EXIT;
    case System::QuickMatchWork::NETGAME_ON_TRADE:
      if( timingNo == 0 )
        timingNo = System::QuickMatchWork::TIMING_ON_TRADE;
    case System::QuickMatchWork::NETGAME_ON_SINGLE:
      if( timingNo == 0 )
        timingNo = System::QuickMatchWork::TIMING_ON_SINGLE;
    case System::QuickMatchWork::NETGAME_ON_DOUBLE:
      if( timingNo == 0 )
        timingNo = System::QuickMatchWork::TIMING_ON_DOUBLE;
    case System::QuickMatchWork::NETGAME_ON_CHANGE:
      if( timingNo == 0 )
        timingNo = System::QuickMatchWork::TIMING_ON_CHANGE;

      {
        if( m_pQuickMatchWork->GetNetGame( index )->IsTimingEnd( timingNo ) )
        {
          GFL_PRINT("QuickMatchClientFrame::CheckTimingSeq : next[%d][%d]\n",index,timingNo);

          messageID = MESSAGE_ID_LIST[index];
          if( m_isLastUpdate )
          {
            if( m_lastTimingNo != timingNo )
            {//受信判定に齟齬がある
              exitFrame( NetApp::QuickMatch::RESULT_CANCEL_EXIT );//終了がえらばれました　へ
            }
            messageID = INVALID_MESSAGE_ID;
          }
          else
          {//再確認開始
            m_pQuickMatchWork->GetNetGame( System::QuickMatchWork::NETGAME_LAST_UPDATE )->TimingStart( timingNo );
            m_lastTimingNo = timingNo;
            m_isLastUpdate = true;
          }

          return messageID;
        }
      }
      break;
    }
  }

  return INVALID_MESSAGE_ID;
}

void QuickMatchClientFrame::LastUpdateSeq()
{
  if( m_isLastUpdate == false )
    return;

  for( u32 index = System::QuickMatchWork::TIMING_CLIENT_EXIT ; index < System::QuickMatchWork::TIMING_LAST_UPDATE ; ++index )
  {
    if( m_pQuickMatchWork->GetNetGame( System::QuickMatchWork::NETGAME_LAST_UPDATE )->IsTimingEnd( index ) )
    {
      switch( index )
      {
      case System::QuickMatchWork::TIMING_HOST_EXIT:
        exitFrame( NetApp::QuickMatch::RESULT_CANCEL_EXIT );//終了がえらばれました　へ
        return;
      case System::QuickMatchWork::TIMING_ON_CHANGE:
        m_pQuickMatchWork->SetRequestChange( true );
        exitFrame( NetApp::QuickMatch::RESULT_START_HOST );//選ぶ方に交代
        return;
      case System::QuickMatchWork::TIMING_CLIENT_EXIT://自分で終了
        m_pQuickMatchWork->SetAppResult( NetApp::QuickMatch::APP_RESULT_EXIT , Regulation::BATTLE_RULE_SINGLE , false );
      break;
      case System::QuickMatchWork::TIMING_ON_TRADE:
        m_pQuickMatchWork->SetAppResult( NetApp::QuickMatch::APP_RESULT_TRADE , Regulation::BATTLE_RULE_SINGLE , false );
      break;
      case System::QuickMatchWork::TIMING_ON_SINGLE:
        m_pQuickMatchWork->SetAppResult( NetApp::QuickMatch::APP_RESULT_BATTLE , Regulation::BATTLE_RULE_SINGLE , false );
      break;
      case System::QuickMatchWork::TIMING_ON_DOUBLE:
        m_pQuickMatchWork->SetAppResult( NetApp::QuickMatch::APP_RESULT_BATTLE , Regulation::BATTLE_RULE_DOUBLE , false );
      break;
      }

      exitFrame( NetApp::QuickMatch::RESULT_EXIT_APP );//アプリ終了
      return;
    }
  }
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
