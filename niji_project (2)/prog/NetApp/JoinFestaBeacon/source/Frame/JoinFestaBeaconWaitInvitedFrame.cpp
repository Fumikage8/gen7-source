// ============================================================================
/*
 * @file JoinFestaBeaconWaitInvitedFrame.cpp
 * @brief ビーコンアプリの参加承諾プロセスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/JoinFestaBeacon/source/Frame/JoinFestaBeaconWaitInvitedFrame.h"
#include "NetApp/JoinFestaBeacon/source/JoinFestaBeaconFrameResult.h"

#include <niji_conv_header/message/msg_jf_menu.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaBeaconWaitInvitedFrame::JoinFestaBeaconWaitInvitedFrame(
  NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaBeaconWork ),
  m_pJoinFestaBeaconWork( pJoinFestaBeaconWork ),
  m_mainSeq(MAINSEQ_WAIT),
  m_subSeq(0),
  m_requestClient(this)
{
}


JoinFestaBeaconWaitInvitedFrame::~JoinFestaBeaconWaitInvitedFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaBeaconWaitInvitedFrame::startup()
{
  m_pJoinFestaBeaconWork->GetUpperView()->SetUpPlayerIcon();
  //@view Waitメッセージ表示
  m_pJoinFestaBeaconWork->GetLowerView()->SetupMessageTimerMode( jf_menu_win_32 , false );

  //@com 招待応答を送信
  JoinFestaBeaconRequestClient::REQUEST_PARAM requestParam;
  requestParam.m_pJoinFestaBeaconWork = m_pJoinFestaBeaconWork;
  m_requestClient.JoinRequest( requestParam );

  return true;
}


bool JoinFestaBeaconWaitInvitedFrame::cleanup()
{
  return true;
}


void JoinFestaBeaconWaitInvitedFrame::setListener()
{
}


void JoinFestaBeaconWaitInvitedFrame::removeListener()
{
}


void JoinFestaBeaconWaitInvitedFrame::updateSequence()
{
  u32 result = m_pJoinFestaBeaconWork->GetLowerView()->GetResult();

  switch( m_mainSeq )
  {
  case MAINSEQ_WAIT:
    {
      if( result == View::JoinFestaBeaconLowerView::RESULT_EXIT )
      {//Bボタンおした
        //@view メッセージ＆選択肢表示
        m_pJoinFestaBeaconWork->GetLowerView()->SetupYesNoMode();
      }
      else if( result == View::JoinFestaBeaconLowerView::RESULT_YES )
      {//はい
        //@view Waitメッセージ表示
        m_pJoinFestaBeaconWork->GetLowerView()->SetupMessageTimerMode( jf_menu_win_25 , false );
        m_requestClient.CancelConnectRequest();

        m_pJoinFestaBeaconWork->GetLowerView()->HideReturn();//これ以降はキャンセルできない

        SetMainSeq( MAINSEQ_CANCEL_REQUEST );
      }
      else if( result == View::JoinFestaBeaconLowerView::RESULT_NO )
      {//いいえ
        //@view Waitメッセージ表示
        m_pJoinFestaBeaconWork->GetLowerView()->SetupMessageTimerMode( jf_menu_win_32 , false );
      }
    }
  break;
  case MAINSEQ_CONNECT_SUCCESS:
    {
      SetMainSeq( MAINSEQ_RETURN_OK_WAIT );
    }
  break;
  case MAINSEQ_CONNECT_NG:
    {//NGが来た
      //@view メッセージ表示
      m_pJoinFestaBeaconWork->GetLowerView()->SetupMessageStreamMode( jf_menu_win_33 );
      SetMainSeq( MAINSEQ_RETURN_NG_MESSAGE );
    }
  break;
  case MAINSEQ_CONNECT_WLANOFF:
    {//LANOFF
      //@view メッセージ表示
      m_pJoinFestaBeaconWork->GetLowerView()->SetupMessageStreamMode( jf_menu_win_30 );
      SetMainSeq( MAINSEQ_RETURN_NG_MESSAGE );
    }
    break;
  case MAINSEQ_CANCEL_REQUEST:
    {//リスナー待ち
    }
  break;
  case MAINSEQ_RETURN_OK_WAIT:
    {
      //if( result == View::JoinFestaBeaconLowerView::RESULT_MESSAGE_END )
      {//タイマーメッセージ表示終了
        m_pJoinFestaBeaconWork->GetAppParam()->isConnectSuccess = true;
        exitFrame( NetApp::JoinFestaBeacon::RESULT_CONNECT_SUCCESS );
      }
    }
  break;
  case MAINSEQ_RETURN_NG_MESSAGE:
    {
      if( result == View::JoinFestaBeaconLowerView::RESULT_EOM )
      {//ストリームメッセージ表示終了
        exitFrame( NetApp::JoinFestaBeacon::RESULT_ERROR );
      }
    }
  break;
  case MAINSEQ_CANCEL_WAIT:
    {
      if( result == View::JoinFestaBeaconLowerView::RESULT_MESSAGE_END )
      {//タイマーメッセージ表示終了
        exitFrame( NetApp::JoinFestaBeacon::RESULT_CONNECT_CANCEL );
      }
    }
  break;
  case MAINSEQ_ERROR:
    exitFrame( NetApp::JoinFestaBeacon::RESULT_ERROR );
  break;
  }
}


void JoinFestaBeaconWaitInvitedFrame::SetMainSeq( u32 mainSeq )
{
  m_mainSeq = mainSeq;
  m_subSeq = 0;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   通信エラー
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconWaitInvitedFrame::OnError( void )
{
  m_mainSeq = MAINSEQ_ERROR;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   無線LANスイッチOFF
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconWaitInvitedFrame::OnWLANSwitchOFF( void )
{
  m_mainSeq = MAINSEQ_CONNECT_WLANOFF;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   接続成功
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconWaitInvitedFrame::OnConnectSuccess( void )
{
  m_mainSeq = MAINSEQ_CONNECT_SUCCESS;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   相手が断った
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconWaitInvitedFrame::OnConnectCanceled( u32 cancelPlayerIndex )
{
  m_mainSeq = MAINSEQ_CONNECT_NG;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   キャンセル成功
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconWaitInvitedFrame::OnCancelSuccess( void )
{
  m_mainSeq = MAINSEQ_CANCEL_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   プレイヤー参加を検知
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconWaitInvitedFrame::OnJoinPlayer( NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData )
{
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)
