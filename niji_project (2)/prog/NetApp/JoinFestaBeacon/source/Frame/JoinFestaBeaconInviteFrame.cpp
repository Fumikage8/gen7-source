// ============================================================================
/*
 * @file JoinFestaBeaconInviteFrame.cpp
 * @brief ビーコンアプリの一人申し込みプロセスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/JoinFestaBeacon/source/Frame/JoinFestaBeaconInviteFrame.h"
#include "NetApp/JoinFestaBeacon/source/JoinFestaBeaconFrameResult.h"

#include <niji_conv_header/message/msg_jf_menu.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaBeaconInviteFrame::JoinFestaBeaconInviteFrame(
  NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaBeaconWork ),
  m_pJoinFestaBeaconWork( pJoinFestaBeaconWork ),
  m_mainSeq(MAINSEQ_WAIT),
  m_subSeq(0),
  m_requestClient(this),
  m_cancelIndex(0),
  m_waitMessage(0),
  m_finalWaitCount(0)
{
}


JoinFestaBeaconInviteFrame::~JoinFestaBeaconInviteFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaBeaconInviteFrame::startup()
{
  m_pJoinFestaBeaconWork->GetUpperView()->SetUpPlayerIcon();

  //待機メッセージ
  m_waitMessage = jf_menu_win_39;
  if( m_pJoinFestaBeaconWork->GetAppParam()->type == NetApp::JoinFestaBeacon::TYPE_TRADE )
  {//交換
    m_waitMessage = jf_menu_win_39;
  }
  else
  {//対戦
    switch( m_pJoinFestaBeaconWork->GetAppParam()->type )
    {
    case NetApp::JoinFestaBeacon::TYPE_BATTLE_S:
      m_waitMessage = jf_menu_win_14;
    break;
    case NetApp::JoinFestaBeacon::TYPE_BATTLE_D:
      m_waitMessage = jf_menu_win_15;
    break;
    case NetApp::JoinFestaBeacon::TYPE_BATTLE_M:
      m_waitMessage = jf_menu_win_16;
    break;
    case NetApp::JoinFestaBeacon::TYPE_BATTLE_R:
      m_waitMessage = jf_menu_win_17;
    break;
    }
  }

  //@view Waitメッセージ表示
  m_pJoinFestaBeaconWork->GetLowerView()->SetupMessageTimerMode( m_waitMessage , false );


  //@com 招待を送信
  JoinFestaBeaconRequestClient::REQUEST_PARAM requestParam;
  requestParam.m_pJoinFestaBeaconWork = m_pJoinFestaBeaconWork;
  m_requestClient.InviteRequest( requestParam );

  return true;
}


bool JoinFestaBeaconInviteFrame::cleanup()
{
  if( m_pJoinFestaBeaconWork->GetAppParam()->isConnectSuccess == false )
  {
    m_pJoinFestaBeaconWork->GetAppParam()->isCancel = true;
  }
  else
  {
    m_pJoinFestaBeaconWork->GetAppParam()->isCancel = false;
  }

  return true;
}


void JoinFestaBeaconInviteFrame::setListener()
{
}


void JoinFestaBeaconInviteFrame::removeListener()
{
}


void JoinFestaBeaconInviteFrame::updateSequence()
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
        m_pJoinFestaBeaconWork->GetLowerView()->SetupMessageTimerMode( m_waitMessage , false );
      }
    }
  break;
  case MAINSEQ_CONNECT_SUCCESS:
    {//OKが来た
      //@view Waitメッセージ表示
      bool isMulti = false;
      switch( m_pJoinFestaBeaconWork->GetAppParam()->type )
      {
      case NetApp::JoinFestaBeacon::TYPE_BATTLE_M:
      case NetApp::JoinFestaBeacon::TYPE_BATTLE_R:
        isMulti = true;
        break;
      }

      if( isMulti == false )
      {//二人用
        m_pJoinFestaBeaconWork->GetLowerView()->SetRegisterPlayerName( 0 );
        m_pJoinFestaBeaconWork->GetLowerView()->SetupMessageTimerMode( jf_menu_win_19 , false );
      }
      else
      {//4人用
        m_pJoinFestaBeaconWork->GetLowerView()->SetupMessageTimerMode( jf_menu_win_18 , false );
      }
      SetMainSeq( MAINSEQ_RETURN_OK_WAIT );
    }
  break;
  case MAINSEQ_CONNECT_NG:
    {//NGが来た
      //@view メッセージ表示
      m_pJoinFestaBeaconWork->GetLowerView()->SetRegisterPlayerName( m_cancelIndex );
      m_pJoinFestaBeaconWork->GetLowerView()->SetupMessageStreamMode( jf_menu_win_20 );
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
    {//リスナーを待つ
    }
  break;
  case MAINSEQ_RETURN_OK_WAIT:
    {
      if( m_finalWaitCount++ < FINAL_WAIT_COUNT )
      {//最後のウェイト
        break;
      }

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
        exitFrame( NetApp::JoinFestaBeacon::RESULT_CONNECT_NG );
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


void JoinFestaBeaconInviteFrame::SetMainSeq( u32 mainSeq )
{
  m_mainSeq = mainSeq;
  m_subSeq = 0;
}


//--------------------------------------------------------------------------------------------
/**
  * @brief   通信エラー
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconInviteFrame::OnError( void )
{
  m_mainSeq = MAINSEQ_ERROR;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   無線LANスイッチOFF
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconInviteFrame::OnWLANSwitchOFF( void )
{
  m_mainSeq = MAINSEQ_CONNECT_WLANOFF;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   接続成功
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconInviteFrame::OnConnectSuccess( void )
{
  m_pJoinFestaBeaconWork->GetLowerView()->HideReturn();//これ以降はキャンセルできない
  m_mainSeq = MAINSEQ_CONNECT_SUCCESS;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   相手が断った
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconInviteFrame::OnConnectCanceled( u32 cancelPlayerIndex )
{
  m_mainSeq = MAINSEQ_CONNECT_NG;
  m_cancelIndex = cancelPlayerIndex;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   キャンセル成功
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconInviteFrame::OnCancelSuccess( void )
{
  m_mainSeq = MAINSEQ_CANCEL_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   プレイヤー参加を検知
  */
//--------------------------------------------------------------------------------------------
void JoinFestaBeaconInviteFrame::OnJoinPlayer( NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData )
{
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)
