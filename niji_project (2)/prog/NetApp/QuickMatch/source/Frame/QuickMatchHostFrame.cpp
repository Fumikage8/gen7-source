// ============================================================================
/*
 * @file QuickMatchHostFrame.cpp
 * @brief クイックマッチアプリのホストプロセスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/QuickMatch/source/Frame/QuickMatchHostFrame.h"
#include "NetApp/QuickMatch/source/QuickMatchFrameResult.h"

#include "NetApp/QuickMatch/source/View/QuickMatchUpperView.h"
#include "NetApp/QuickMatch/source/View/QuickMatchLowerView.h"

#include <Fade/include/gfl2_FadeManager.h>
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>

#include <niji_conv_header/message/msg_quickmatch.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(Frame)


QuickMatchHostFrame::QuickMatchHostFrame(
  NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork ) :
  NetAppLib::System::NetAppFrameBase( pQuickMatchWork ),
  m_pQuickMatchWork( pQuickMatchWork ),
  m_mainSeq( SELECT_MODE ),
  m_subSeq( 0 ),
  m_messageNextSeq( 0 ),
  m_messageId( 0 ),
  m_isTimerEnd( false ),
  m_isLastUpdate( false ),
  m_timingNetGameIndex( 0 ),
  m_timingNo( 0 ),
  m_tradeNGMessageNo( -1 ),
  m_tradeNGPlayerIndex( 0 ),
  m_doubleNGMessageNo( -1 ),
  m_doubleNGPlayerIndex( 0 ),
  m_lastTimingNo(0)
{
}


QuickMatchHostFrame::~QuickMatchHostFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool QuickMatchHostFrame::startup()
{
  //クライアントの終了受付
  m_pQuickMatchWork->GetNetGame( System::QuickMatchWork::NETGAME_CLIENT_EXIT )->TimingStart( System::QuickMatchWork::TIMING_CLIENT_EXIT );

  bool isMyPenalty = false;
  bool isMyTradeOK = false;
  bool isMyDoubleOK = false;

  bool isOtherPenalty = false;
  bool isOtherTradeOK = false;
  bool isOtherDoubleOK = false;


  u32 myNetId = m_pQuickMatchWork->GetNetGame( 0 )->GetNetID();
  u32 otherNetId = 1 - m_pQuickMatchWork->GetNetGame( 0 )->GetNetID();

  m_pQuickMatchWork->GetMyStatus( myNetId )->GetQuickMatchInfo( &isMyPenalty , &isMyTradeOK , &isMyDoubleOK );
  m_pQuickMatchWork->GetMyStatus( otherNetId )->GetQuickMatchInfo( &isOtherPenalty , &isOtherTradeOK , &isOtherDoubleOK );

  //ダブルバトルアラート決定
  if( isMyDoubleOK == false && isOtherDoubleOK == false )
  {
    m_doubleNGMessageNo = msg_quickmatch_win_18;
  }
  else if( isMyDoubleOK == false || isOtherDoubleOK == false )
  {
    m_doubleNGMessageNo = msg_quickmatch_win_17;
    if( isMyDoubleOK == false )
    {
      m_doubleNGPlayerIndex = myNetId;
    }
    else if( isOtherDoubleOK == false )
    {
      m_doubleNGPlayerIndex = otherNetId;
    }
  }

  //交換アラート決定
  if( isMyPenalty == true && isOtherPenalty == true )
  {//お互いペナルティ
    m_tradeNGMessageNo = msg_quickmatch_win_14;
  }
  else if( isMyPenalty == true || isOtherPenalty == true )
  {//どちらかペナルティ
    m_tradeNGMessageNo = msg_quickmatch_win_13;
    if( isMyPenalty == true )
    {
      m_tradeNGPlayerIndex = myNetId;
    }
    else if( isOtherPenalty == true )
    {
      m_tradeNGPlayerIndex = otherNetId;
    }
  }
  else if( isMyTradeOK == false && isOtherTradeOK == false )
  {//お互いトレードできない
    m_tradeNGMessageNo = msg_quickmatch_win_16;
  }
  else if( isMyTradeOK == false || isOtherTradeOK == false )
  {//どちらかトレードできない
    m_tradeNGMessageNo = msg_quickmatch_win_15;
    if( isMyTradeOK == false )
    {
      m_tradeNGPlayerIndex = myNetId;
    }
    else if( isOtherTradeOK == false )
    {
      m_tradeNGPlayerIndex = otherNetId;
    }
  }

  return true;
}


void QuickMatchHostFrame::setListener()
{
}


void QuickMatchHostFrame::removeListener()
{
}


void QuickMatchHostFrame::updateSequence()
{
  //表示、入力制御
  switch( m_mainSeq )
  {
  case SELECT_MODE:
    UpdateSelectMode();
  break;
  case SELECT_EXIT:
    UpdateSelectExit();
  break;
  case MESSAGE_STREAM_WAIT:
    MessageStreamWait();
  break;
  case MESSAGE_TIMER_WAIT:
    MessageTimerWait();
  break;
  case EXIT_SEQ:
  break;
  }

  //通信制御
  CheckTimingSeq();
  LastUpdateSeq();
}

void QuickMatchHostFrame::UpdateSelectMode()
{
  enum SELECT_MODE_SEQ
  {
    VIEW_INIT = 0,
    WAIT_VIEW_RESULT
  };

  switch( m_subSeq )
  {
  case VIEW_INIT:
    {
      //@view なにをしますか？
      //@view 選択肢表示
      m_pQuickMatchWork->GetLowerView()->SetupSelectMode();
      m_subSeq++;
    }
  break;
  case WAIT_VIEW_RESULT:
    {
      //@view 選択肢結果取得
      u32 result = m_pQuickMatchWork->GetLowerView()->GetResult();
      switch( result )
      {
      case View::QuickMatchLowerView::RESULT_NONE://未選択
        return;
      case View::QuickMatchLowerView::RESULT_SINGLE://シングル
        {
          m_messageId = msg_quickmatch_win_08;
          m_messageNextSeq = EXIT_SEQ;
          m_timingNetGameIndex = System::QuickMatchWork::NETGAME_ON_SINGLE;
          m_timingNo = System::QuickMatchWork::TIMING_ON_SINGLE;
        }
      break;
      case View::QuickMatchLowerView::RESULT_DOUBLE://ダブル
        {
          if( m_doubleNGMessageNo >= 0 )
          {
            SetMainSeq( MESSAGE_STREAM_WAIT );
            m_messageId = m_doubleNGMessageNo;
            m_messageNextSeq = SELECT_MODE;
            //@view RegisterPlayerName
            m_pQuickMatchWork->GetLowerView()->SetPlayerNetIDforName( m_doubleNGPlayerIndex );
            return;
          }
          m_messageId = msg_quickmatch_win_09;
          m_messageNextSeq = EXIT_SEQ;
          m_timingNetGameIndex = System::QuickMatchWork::NETGAME_ON_DOUBLE;
          m_timingNo = System::QuickMatchWork::TIMING_ON_DOUBLE;
        }
      break;
      case View::QuickMatchLowerView::RESULT_TRADE://交換
        {
          if( m_tradeNGMessageNo >= 0 )
          {
            SetMainSeq( MESSAGE_STREAM_WAIT );
            m_messageId = m_tradeNGMessageNo;
            m_messageNextSeq = SELECT_MODE;
            //@view RegisterPlayerName
            m_pQuickMatchWork->GetLowerView()->SetPlayerNetIDforName( m_tradeNGPlayerIndex );
            return;
          }
          m_messageId = msg_quickmatch_win_11;
          m_messageNextSeq = EXIT_SEQ;
          m_timingNetGameIndex = System::QuickMatchWork::NETGAME_ON_TRADE;
          m_timingNo = System::QuickMatchWork::TIMING_ON_TRADE;
        }
      break;
      case View::QuickMatchLowerView::RESULT_CHANGE://交代
        {
          m_messageId = msg_quickmatch_win_05;
          m_messageNextSeq = EXIT_SEQ;
          m_timingNetGameIndex = System::QuickMatchWork::NETGAME_ON_CHANGE;
          m_timingNo = System::QuickMatchWork::TIMING_ON_CHANGE;
        }
      break;
      case View::QuickMatchLowerView::RESULT_EXIT://終了
        SetMainSeq( SELECT_EXIT );
      return;
      default:
      return;
      }

      SetMainSeq( MESSAGE_TIMER_WAIT );
    }
  }
}
void QuickMatchHostFrame::UpdateSelectExit()
{
  enum EXIT_MODE_SEQ
  {
    VIEW_INIT = 0,
    WAIT_VIEW_RESULT
  };

  switch( m_subSeq )
  {
    //あそぶのをやめますか？
    //@view YesNo表示
  case VIEW_INIT:
    {
      m_pQuickMatchWork->GetLowerView()->SetupYesNoMode();
      m_subSeq++;
    }
  break;
  case WAIT_VIEW_RESULT:
    {
      //@view YesNo結果取得
      u32 result = m_pQuickMatchWork->GetLowerView()->GetResult();

      if( result == View::QuickMatchLowerView::RESULT_YES )
      {//Yes
        m_messageId = msg_quickmatch_win_04;
        m_messageNextSeq = EXIT_SEQ;
        m_timingNetGameIndex = System::QuickMatchWork::NETGAME_HOST_EXIT;
        m_timingNo = System::QuickMatchWork::TIMING_HOST_EXIT;
        SetMainSeq( MESSAGE_TIMER_WAIT );
      }
      else if( result == View::QuickMatchLowerView::RESULT_NO )
      {//No
        SetMainSeq( SELECT_MODE );
      }
    }
  break;
  }
}

void QuickMatchHostFrame::MessageStreamWait()
{
  //ストリームメッセージ表示
  enum STREAM_MSG_MODE_SEQ
  {
    VIEW_INIT = 0,
    WAIT_VIEW_RESULT
  };

  switch( m_subSeq )
  {
  case VIEW_INIT:
    {
      m_pQuickMatchWork->GetLowerView()->SetupMessageStreamMode( m_messageId );
      m_subSeq++;
    }
  break;
  case WAIT_VIEW_RESULT:
    {
      u32 result = m_pQuickMatchWork->GetLowerView()->GetResult();
      if( result != View::QuickMatchLowerView::RESULT_EOM )
        break;

      SetMainSeq( m_messageNextSeq );
    }
  break;
  }
}
void QuickMatchHostFrame::MessageTimerWait()
{
  enum TIMER_MSG_MODE_SEQ
  {
    VIEW_INIT = 0,
    WAIT_VIEW_RESULT
  };

  switch( m_subSeq )
  {
  case VIEW_INIT:
    {
      //タイマーメッセージ表示
      m_pQuickMatchWork->GetLowerView()->SetupMessageTimerMode( m_messageId );
      m_subSeq++;
    }
  break;
  case WAIT_VIEW_RESULT:
    {
      u32 result = m_pQuickMatchWork->GetLowerView()->GetResult();
      if( result != View::QuickMatchLowerView::RESULT_MESSAGE_END )
        break;

      m_isTimerEnd = false;
      SetMainSeq( m_messageNextSeq );
      m_pQuickMatchWork->GetNetGame( m_timingNetGameIndex )->TimingStart( m_timingNo );
    }
  break;
  }
}

void QuickMatchHostFrame::CheckTimingSeq()
{
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
          GFL_PRINT("QuickMatchHostFrame::CheckTimingSeq : next[%d][%d]\n",index,timingNo);

          if( m_isLastUpdate )
          {
            if( timingNo != m_lastTimingNo )
            {//受信判定に齟齬がある
              exitFrame( NetApp::QuickMatch::RESULT_CANCEL_EXIT );//終了がえらばれました　へ
            }
          }
          else
          {//再確認開始
            m_pQuickMatchWork->GetNetGame( System::QuickMatchWork::NETGAME_LAST_UPDATE )->TimingStart( timingNo );
            m_lastTimingNo = timingNo;
            m_isLastUpdate = true;
          }

          return;
        }
      }
      break;
    }
  }
}

void QuickMatchHostFrame::LastUpdateSeq()
{
  if( m_isLastUpdate == false )
    return;

  for( u32 index = System::QuickMatchWork::TIMING_CLIENT_EXIT ; index < System::QuickMatchWork::TIMING_LAST_UPDATE ; ++index )
  {
    if( m_pQuickMatchWork->GetNetGame( System::QuickMatchWork::NETGAME_LAST_UPDATE )->IsTimingEnd( index ) )
    {
      switch( index )
      {
      case System::QuickMatchWork::TIMING_CLIENT_EXIT:
        exitFrame( NetApp::QuickMatch::RESULT_CANCEL_EXIT );//終了がえらばれました　へ
        return;
      case System::QuickMatchWork::TIMING_ON_CHANGE:
        m_pQuickMatchWork->SetRequestChange( false );
        exitFrame( NetApp::QuickMatch::RESULT_START_CLIENT );//待つ方に交代
        return;
      case System::QuickMatchWork::TIMING_HOST_EXIT://自分で終了
        m_pQuickMatchWork->SetAppResult( NetApp::QuickMatch::APP_RESULT_EXIT , Regulation::BATTLE_RULE_SINGLE , true );
      break;
      case System::QuickMatchWork::TIMING_ON_TRADE:
        m_pQuickMatchWork->SetAppResult( NetApp::QuickMatch::APP_RESULT_TRADE , Regulation::BATTLE_RULE_SINGLE , true );
      break;
      case System::QuickMatchWork::TIMING_ON_SINGLE:
        m_pQuickMatchWork->SetAppResult( NetApp::QuickMatch::APP_RESULT_BATTLE , Regulation::BATTLE_RULE_SINGLE , true );
      break;
      case System::QuickMatchWork::TIMING_ON_DOUBLE:
        m_pQuickMatchWork->SetAppResult( NetApp::QuickMatch::APP_RESULT_BATTLE , Regulation::BATTLE_RULE_DOUBLE , true );
      break;
      }

      exitFrame( NetApp::QuickMatch::RESULT_EXIT_APP );//アプリ終了
      return;
    }
  }
}

void QuickMatchHostFrame::SetMainSeq( u32 mainSeq )
{
  m_mainSeq = mainSeq;
  m_subSeq = 0;
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
