// ============================================================================
/*
 * @file ConnectionInternetConnectionFrame.cpp
 * @brief インターネット接続アプリのインターネット接続を行うプロセスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/Frame/ConnectionInternetConnectionFrame.h"
#include "NetApp/Connection/source/View/ConnectionUpperView.h"
#include "NetApp/Connection/source/View/ConnectionLowerView.h"
#include "NetApp/Connection/source/ConnectionFrameResult.h"
#include "NetApp/Connection/source/RequestSequence/ConnectionRequestFacade.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncRequestFacade.h"
#include "NetStatic/NetAppLib/include/GlobalAttraction/GlobalAttractionRequestFacade.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Sound/include/Sound.h"
#include <niji_conv_header/message/msg_netconnect.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(Frame)


static const int SEQUENCE_DEFAULT                                      = 0;
static const int SEQUENCE_FIRST_SAVE                                   = 1;
static const int SEQUENCE_CONNECTION_WAIT                              = 2;
static const int SEQUENCE_CONNECTION_SUCCESS_MESSAGE_START             = 3;
static const int SEQUENCE_SECOND_SAVE                                  = 4;
static const int SEQUENCE_GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST         = 5;
static const int SEQUENCE_GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST_WAIT    = 6;
static const int SEQUENCE_GAME_SYNC_CREATE_REQUEST                     = 7;
static const int SEQUENCE_GAME_SYNC_CREATE_REQUEST_WAIT                = 8;
static const int SEQUENCE_GAME_SYNC_CREATE_SUCCESS_SAVE                = 9;


ConnectionInternetConnectionFrame::ConnectionInternetConnectionFrame(
  NetApp::Connection::System::ConnectionWork* pConnectionWork,
  NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor ) :
  NetApp::Connection::Frame::ConnectionFrameBase( pConnectionWork, pConnectionViewSystemAccessor ),
  m_ConnectionLowerViewListener(),
  m_NetAppCommonSaveUtility(),
  m_NetAppTimeout(),
  m_bIsValidNexUniqueID( false ),
  m_bIsTimeOutStart( false )
{
}


ConnectionInternetConnectionFrame::~ConnectionInternetConnectionFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool ConnectionInternetConnectionFrame::startup()
{
  m_pConnectionViewObjects->InputDisableAll();
  m_pConnectionViewList->RemoveAllView();

  m_pConnectionViewList->AddUpperView( m_pConnectionViewObjects->GetConnectionUpperView() );
  m_pConnectionViewList->AddLowerView( m_pConnectionViewObjects->GetConnectionLowerView() );

  m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessageYesNo( msg_netconnect_win_01 );

  m_pConnectionWork->SetConnect( false );

  return true;
}


void ConnectionInternetConnectionFrame::setListener()
{
  m_pConnectionViewObjects->GetConnectionLowerView()->SetListener( &m_ConnectionLowerViewListener );
  m_pConnectionViewObjects->GetConnectionLowerView()->SetInputEnabled( true );
}


void ConnectionInternetConnectionFrame::removeListener()
{
  m_pConnectionViewObjects->GetConnectionLowerView()->RemoveListener();
}


NetAppLib::System::FrameResult ConnectionInternetConnectionFrame::getExitFrameResult()
{
#if 0

  NetAppLib::System::FrameResult result = NetApp::Connection::INTERNET_CONNECTION_RESULT_BACK;

  if( m_pConnectionWork->GetBootMode() == NetApp::Connection::BOOT_MODE_CHANGE_INTERNET_MODE )
  {
    result = NetApp::Connection::INTERNET_CONNECTION_RESULT_CHANGE_INTERNET_MODE;
  }

  return result;

#else

  return NetApp::Connection::INTERNET_CONNECTION_RESULT_BACK;

#endif
}


void ConnectionInternetConnectionFrame::initializeNijiNetworkSystem()
{

#if defined( GF_PLATFORM_CTR )

  // ネットワークモードをWiFiに切り替える 
  if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI ) 
  { 
    NetLib::NijiNetworkSystem::TerminateNetwork(); 
    NetLib::NijiNetworkSystem::InitializeWifiNetwork(); 
  } 
  else 
  { 
    GFL_ASSERT_MSG(0,"NetworkMode Error!!!\n"); 
  }

#endif // defined( GF_PLATFORM_CTR )

}


void ConnectionInternetConnectionFrame::updateSequence()
{
  switch( GetSubSeq() )
  {
    // ----------------------------------------------------------------------
    case SEQUENCE_DEFAULT:
    {
      defaultProcedure();
    }
    break;

    // ----------------------------------------------------------------------
    case SEQUENCE_FIRST_SAVE:
    {
      bool bSaveComplete = m_NetAppCommonSaveUtility.Save();
      
      if( bSaveComplete )
      {
        m_pConnectionViewObjects->GetConnectionLowerView()->SetMessage( msg_netconnect_win_02 );

        initializeNijiNetworkSystem();
        NetApp::Connection::RequestSequence::ConnectionRequestFacade::AddConnectionRequest( this, m_pConnectionWork );

        SetSubSeq( SEQUENCE_CONNECTION_WAIT );
      }
    }
    break;

    // ----------------------------------------------------------------------
    case SEQUENCE_CONNECTION_SUCCESS_MESSAGE_START:
    {
      m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessage( msg_netconnect_win_09 );

      Sound::PlaySE( SEQ_SE_SAVE );

      SetSubSeq( SEQUENCE_DEFAULT );
    }
    break;

    // ----------------------------------------------------------------------
    case SEQUENCE_SECOND_SAVE:
    {
      bool bSaveComplete = m_NetAppCommonSaveUtility.Save();
      
      if( bSaveComplete )
      {
        SetSubSeq( SEQUENCE_CONNECTION_SUCCESS_MESSAGE_START );
      }
    }
    break;

    // ----------------------------------------------------------------------
    case SEQUENCE_GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST:
    {
      m_pConnectionViewObjects->GetConnectionLowerView()->SetMessage( msg_netconnect_win_22 );

      NetAppLib::GlobalAttraction::GlobalAttractionRequestFacade::AddOpenCheckRequest( this, m_pConnectionWork );

      SetSubSeq( SEQUENCE_GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST_WAIT );
    }
    break;

    // ----------------------------------------------------------------------
    case SEQUENCE_GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST_WAIT:
    {
      bool bIsPushButtonA = m_pConnectionViewObjects->GetConnectionLowerView()->IsPushButtonA();

      bool bIsTimeOut = false;

      if( m_bIsTimeOutStart )
      {
        bIsTimeOut = m_NetAppTimeout.IsTimeOut();
      }

      if( bIsPushButtonA || bIsTimeOut )
      {
        exitFrame( getExitFrameResult() );
      }
    }
    break;

    // ----------------------------------------------------------------------
    case SEQUENCE_GAME_SYNC_CREATE_REQUEST:
    {
      m_pConnectionViewObjects->GetConnectionLowerView()->SetMessage( msg_netconnect_win_20 );

      NetAppLib::GameSync::GAMESYNC_PARAM param;
      std::memset( &param , 0 , sizeof(param) );
      param.pWork = m_pConnectionWork;
      param.pTrainerIconUtility = m_pConnectionWork->GetTrainerIconUtility();
      param.pReplacePane = m_pConnectionViewObjects->GetConnectionUpperView()->GetIconPane();
      param.isErrorSkip = true;

      NetAppLib::GameSync::GameSyncRequestFacade::AddCreateSaveDataRequest( this, &param );

      SetSubSeq( SEQUENCE_GAME_SYNC_CREATE_REQUEST_WAIT );
    }
    break;

    // ----------------------------------------------------------------------
    case SEQUENCE_GAME_SYNC_CREATE_SUCCESS_SAVE:
    {
      bool bSaveComplete = m_NetAppCommonSaveUtility.Save();
      
      if( bSaveComplete )
      {
        SetSubSeq( SEQUENCE_GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST );
      }
    }
    break;
  }

  m_ConnectionLowerViewListener.ResetInputEvent();
}


void ConnectionInternetConnectionFrame::defaultProcedure()
{
  u32 messageID          = m_ConnectionLowerViewListener.GetMessageID();
  u32 inputEventID       = m_ConnectionLowerViewListener.GetInputEventID();
  bool bTouchYesButton   = ( inputEventID == NetApp::Connection::View::ConnectionLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON ) ? true : false;
  bool bTouchNoButton    = ( inputEventID == NetApp::Connection::View::ConnectionLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON ) ? true : false;
  bool bEndStreamMessage = ( inputEventID == NetApp::Connection::View::ConnectionLowerViewListener::INPUT_EVENT_ID_END_STREAM_MESSAGE ) ? true : false;

  switch( messageID )
  {
    // -----------------------------------------------------------------------------
    // インターネットに接続します。よろしいですか？
    case msg_netconnect_win_01:
    {
      if( bTouchYesButton )
      {
        if( NetLib::NijiNetworkSystem::IsParentalModeOnlyInternet() )
        {
          m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessage( msg_netconnect_win_23 );
        }
        else
        {
          Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

          m_bIsValidNexUniqueID = pMyStatus->IsValidNexUniqueID();

          if( m_bIsValidNexUniqueID )
          {
            m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessageYesNo( msg_netconnect_win_17 );
          }
          else
          {
            m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessageYesNo( msg_netconnect_win_05 );
          }
        }
      }
      else
      if( bTouchNoButton )
      {
        m_pConnectionWork->SetCancel( true );
        exitFrame( getExitFrameResult() );
      }
    }
    break;

    // -----------------------------------------------------------------------------
    // レポートを書きます。よろしいですか？
    case msg_netconnect_win_17:
    {
      if( bTouchYesButton )
      {
        m_pConnectionViewObjects->GetConnectionLowerView()->SetMessage( msg_netconnect_win_18 );
        
        SetSubSeq( SEQUENCE_FIRST_SAVE );
      }
      else
      if( bTouchNoButton )
      {
        m_pConnectionWork->SetCancel( true );
        exitFrame( getExitFrameResult() );
      }
    }
    break;

    // -----------------------------------------------------------------------------
    // インターネット接続…　レポート書き込みます。よろしいですか？
    case msg_netconnect_win_05:
    {
      if( bTouchYesButton )
      {
        m_pConnectionViewObjects->GetConnectionLowerView()->SetMessage( msg_netconnect_win_06 );

        initializeNijiNetworkSystem();
        NetApp::Connection::RequestSequence::ConnectionRequestFacade::AddConnectionRequest( this, m_pConnectionWork );
      }
      else
      if( bTouchNoButton )
      {
        m_pConnectionWork->SetCancel( true );
        exitFrame( getExitFrameResult() );
      }
    }
    break;

    // -----------------------------------------------------------------------------
    // インターネット通信に切り替えました。
    case msg_netconnect_win_09:
    {
      Savedata::GameSyncSave* pGameSyncSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync();

      if( pGameSyncSave->GetGameSyncFirstConnected() )
      {
        if( pGameSyncSave->GetIsCreateGameSyncSaveID() && !pGameSyncSave->CheckLockGameSyncSaveFor5xxError() )
        {
          m_pConnectionViewObjects->GetConnectionLowerView()->SetMessage( msg_netconnect_win_13 );

          NetAppLib::GameSync::GETPLAYSTATUS_PARAM param;
          std::memset( &param , 0 , sizeof(param) );
          param.pWork = m_pConnectionWork;
          param.mode = NetAppLib::GameSync::GETPLAYSTATUS_MODE_GAMESYNC;
          param.isErrorSkip = true;

          NetAppLib::GameSync::GameSyncRequestFacade::AddConfirmPlayStatusRequest( this, &param );
        }
        else
        {
          SetSubSeq( SEQUENCE_GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST );
        }
      }
      else
      {
        SetSubSeq( SEQUENCE_GAME_SYNC_CREATE_REQUEST );
      }
    }
    break;

    // -----------------------------------------------------------------------------
    // インターネット接続に必要な情報を取得しました。
    case msg_netconnect_win_07:
    {
      if( bEndStreamMessage )
      {
        m_pConnectionViewObjects->GetConnectionLowerView()->SetMessage( msg_netconnect_win_18 );
        
        SetSubSeq( SEQUENCE_SECOND_SAVE );
      }
    }
    break;

    // -----------------------------------------------------------------------------
    // 
    case msg_netconnect_win_14:
    case msg_netconnect_win_16:
    case msg_netconnect_win_21:
    {
      if( bEndStreamMessage )
      {
        SetSubSeq( SEQUENCE_GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST );
      }
    }
    break;

    // -----------------------------------------------------------------------------
    // 
    case msg_netconnect_win_23:
    {
      if( bEndStreamMessage )
      {
        m_pConnectionWork->SetCancel( true );
        exitFrame( getExitFrameResult() );
      }
    }
    break;

  }
}


//-----------------------------------------------------------------------------
// ConnectionResponseListener
//-----------------------------------------------------------------------------
void ConnectionInternetConnectionFrame::OnConnectionSuccess()
{
  m_pConnectionWork->SetConnect( true );

  if( m_bIsValidNexUniqueID )
  {
    SetSubSeq( SEQUENCE_CONNECTION_SUCCESS_MESSAGE_START );
  }
  else
  {
    m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessage( msg_netconnect_win_07 );
  }
}


void ConnectionInternetConnectionFrame::OnConnectionError()
{
  m_pConnectionWork->SetCancel( true );
  exitFrame( getExitFrameResult() );
}


//-----------------------------------------------------------------------------
// GameSyncConfirmPlayStatusResponseListener
//-----------------------------------------------------------------------------
void ConnectionInternetConnectionFrame::OnConfirmPlayStatusSuccess()
{
  const Savedata::GameSyncSave::GameSyncSaveData& gameSyncSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveData();

  bool isSigned = gameSyncSaveData.m_registInf.m_isSigned;

  if( isSigned == false )
  {
    m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessage( msg_netconnect_win_14 );
  }
  else
  {
    m_pConnectionViewObjects->GetConnectionLowerView()->SetMessage( msg_netconnect_win_19 );

    NetAppLib::GameSync::GAMESYNC_PARAM param;
    std::memset( &param , 0 , sizeof(param) );
    param.pWork = m_pConnectionWork;
    param.pTrainerIconUtility = m_pConnectionWork->GetTrainerIconUtility();
    param.pReplacePane = m_pConnectionViewObjects->GetConnectionUpperView()->GetIconPane();
    param.isErrorSkip = true;

    NetAppLib::GameSync::GameSyncRequestFacade::AddSaveDataSyncRequest( this, &param );
  }
}


void ConnectionInternetConnectionFrame::OnConfirmPlayStatusError( u32 errorCode )
{
  m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessage( msg_netconnect_win_21 );
}

#if defined(GF_PLATFORM_CTR)
void ConnectionInternetConnectionFrame::OnConfirmPlayStatusError( u32 errorCode, NetLib::Pgl::PGL_COMMON_RESULT ePglCommonResult )
{
  if( ePglCommonResult == NetLib::Pgl::E_RET_SAVEDATA_RECIVE_LIMIT )
  {
    m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessage( msg_netconnect_win_16 );
  }
  else
  {
    m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessage( msg_netconnect_win_21 );
  }
}
#endif // defined(GF_PLATFORM_CTR)


//-----------------------------------------------------------------------------
// GameSyncSaveDataSyncResponseListener
//-----------------------------------------------------------------------------
void ConnectionInternetConnectionFrame::OnSaveDataSyncSuccess()
{
  SetSubSeq( SEQUENCE_GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST );
}


void ConnectionInternetConnectionFrame::OnSaveDataSyncError( u32 errorCode )
{
  m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessage( msg_netconnect_win_21 );
}

#if defined(GF_PLATFORM_CTR)
void ConnectionInternetConnectionFrame::OnSaveDataSyncError( u32 errorCode, NetLib::Pgl::PGL_COMMON_RESULT ePglCommonResult )
{
  if( ePglCommonResult == NetLib::Pgl::E_RET_SAVEDATA_RECIVE_LIMIT )
  {
    m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessage( msg_netconnect_win_16 );
  }
  else
  {
    m_pConnectionViewObjects->GetConnectionLowerView()->SetStreamMessage( msg_netconnect_win_21 );
  }
}
#endif // defined(GF_PLATFORM_CTR)


//-----------------------------------------------------------------------------
// GameSyncCreateSaveDataResponseListener
//-----------------------------------------------------------------------------
void ConnectionInternetConnectionFrame::OnCreateSaveDataSuccess()
{
  Savedata::GameSyncSave* pGameSyncSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync();

  pGameSyncSave->SetGameSyncFirstConnected( true );

  SetSubSeq( SEQUENCE_GAME_SYNC_CREATE_SUCCESS_SAVE );
}


void ConnectionInternetConnectionFrame::OnCreateSaveDataError()
{
  SetSubSeq( SEQUENCE_GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST );
}


//-----------------------------------------------------------------------------
// GlobalAttractionOpenCheckResponseListener
//-----------------------------------------------------------------------------
void ConnectionInternetConnectionFrame::OnOpenCheckSuccess( bool bIsOpen )
{
  if( bIsOpen )
  {
    m_pConnectionViewObjects->GetConnectionLowerView()->SetAlertMessage( msg_netconnect_exp_01, true );
    m_NetAppTimeout.TimeOutStart( 5000 );
    m_bIsTimeOutStart = true;
  }
  else
  {
    exitFrame( getExitFrameResult() );
  }
}


void ConnectionInternetConnectionFrame::OnOpenCheckError()
{
  exitFrame( getExitFrameResult() );
}


void ConnectionInternetConnectionFrame::OnOpenCheckCancel()
{
  exitFrame( getExitFrameResult() );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)
