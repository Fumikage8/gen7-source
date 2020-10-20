// ============================================================================
/*
 * @file NetAppFrameBase.cpp
 * @brief 通信アプリ用フレーム基底クラスです。
 * @date 2015.06.17
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>
#include <Fade/include/gfl2_FadeManager.h>
#include "GameSys/include/GameData.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


NetAppFrameBase::NetAppFrameBase( NetAppLib::System::ApplicationWorkBase* pWork ) :
  m_pWork( pWork ),
  m_bFadeInRequest( false ),
  m_bFadeOutRequest( false ),
  m_isSaveRequest( false ),
  m_eWLANSwitchState( ( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() ) ? WLAN_SWITCH_STATE_ON : WLAN_SWITCH_STATE_OFF ),
  m_bFadeOutExecuted( false )
{
}


NetAppFrameBase::~NetAppFrameBase()
{
  gflnet2::ServerClient::ServerClientRequestManager::RemoveAllListener();
}


applib::frame::Result NetAppFrameBase::InitFunc( void )
{
  applib::frame::Result result = applib::frame::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    // ----------------------------------------------------------------------------
    case INIT_SEQUENCE_INITIALIZE_SERVER_CLIENT_REQUEST_MANAGER:
    {
      if( GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager) ) 
      {
        GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager)->Initialize( m_pWork->GetAppHeap()->GetDeviceHeap() );
      }

      SetSubSeq( INIT_SEQUENCE_STARTUP );
    }
    break;

    // ----------------------------------------------------------------------------
    case INIT_SEQUENCE_STARTUP:
    {
      if( startup() )
      {
        SetSubSeq( INIT_SEQUENCE_FADE_IN_START );
      }
    }
    break;

    // ----------------------------------------------------------------------------
    case INIT_SEQUENCE_FADE_IN_START:
    {
      if( m_bFadeInRequest && !m_bFadeOutExecuted )
      {
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
        SetSubSeq( INIT_SEQUENCE_FADE_IN_WAIT );
      }
      else
      {
        result = applib::frame::RES_FINISH;
      }
    }
    break;

    // ----------------------------------------------------------------------------
    case INIT_SEQUENCE_FADE_IN_WAIT:
    {
      if( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_DOUBLE ) )
      {
        result = applib::frame::RES_FINISH;
      }
    }
    break;
  }  

  if( result == applib::frame::RES_FINISH )
  {
    setListener();
  }

  return result;
}


applib::frame::Result NetAppFrameBase::EndFunc( void )
{
  applib::frame::Result result = applib::frame::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    // ----------------------------------------------------------------------------
    case END_SEQUENCE_FADE_OUT:
    {
      removeListener();

      if( m_bFadeOutRequest )
      {
        gfl2::math::Vector4 start_col( 0, 0, 0, 0.0f );
        gfl2::math::Vector4 end_col( 0, 0, 0, 255.0f );

        // Captureありでブラックフェードを開始
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col, gfl2::Fade::FADE_DEFAULT_SYNC, true );
      }

      if( m_isSaveRequest )
      {
        SetSubSeq( END_SEQUENCE_SAVE_WAIT );
      }
      else
      {
        SetSubSeq( END_SEQUENCE_REQUEST_CANCEL_CHECK );
      }
    }
    break;

    // ----------------------------------------------------------------------------
    case END_SEQUENCE_SAVE_WAIT:
    {
      if( UpdateSave() )
      {
        SetSubSeq( END_SEQUENCE_REQUEST_CANCEL_CHECK );
      }
      else
      {
        break;
      }
    }
    //break; fall

    // ----------------------------------------------------------------------------
    case END_SEQUENCE_REQUEST_CANCEL_CHECK:
    {
      if( !gflnet2::ServerClient::ServerClientRequestManager::IsRequestEmpty() )
      {
        gflnet2::ServerClient::ServerClientRequestManager::CancelAllRequest();
        SetSubSeq( END_SEQUENCE_REQUEST_CANCEL_WAIT );
      }
      else
      {
        SetSubSeq( END_SEQUENCE_CLEANUP );
      }
    }
    break;

    // ----------------------------------------------------------------------------
    case END_SEQUENCE_REQUEST_CANCEL_WAIT:
    {
      if( gflnet2::ServerClient::ServerClientRequestManager::IsRequestEmpty() )
      {
        SetSubSeq( END_SEQUENCE_CLEANUP );
      }
    }
    break;

    // ----------------------------------------------------------------------------
    case END_SEQUENCE_CLEANUP:
    {
      if( cleanup() )
      {
        SetSubSeq( END_SEQUENCE_FINALIZE_SERVER_CLIENT_REQUEST_MANAGER );
      }
    }
    break;

    // ----------------------------------------------------------------------------
    case END_SEQUENCE_FINALIZE_SERVER_CLIENT_REQUEST_MANAGER:
    {
      if( GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager) ) 
      {
        GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager)->Finalize();
      }
      result = applib::frame::RES_FINISH;
    }
    break;
  }

  return result;
}

applib::frame::Result NetAppFrameBase::UpdateFunc( void )
{
  updateSequence();

  if( m_eWLANSwitchState == WLAN_SWITCH_STATE_ON )
  {
    if( !NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
    {
      OnWLANSwitchDisable();
      m_eWLANSwitchState = WLAN_SWITCH_STATE_OFF;
    }
  }
  else
  if( m_eWLANSwitchState == WLAN_SWITCH_STATE_OFF )
  {
    if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
    {
      OnWLANSwitchEnable();
      m_eWLANSwitchState = WLAN_SWITCH_STATE_ON;
    }
  }

  if( m_pWork->GetFrameResult() != NetAppLib::System::FRAME_RESULT_INVALID )
  {
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}


void NetAppFrameBase::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
}


void NetAppFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  ICHI_PRINT(">ICHI exitFrame(%d)\n", result );
  removeListener();
  m_pWork->SetFrameResult( result );
}


bool NetAppFrameBase::UpdateSave( void )
{
  GameSys::GameData  *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();

  if( m_isSaveRequest )
  {
    if( game_data->SaveMain() == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
    {
      m_isSaveRequest = false;
      return true;
    }
  }
  else
  {
    game_data->GetPlayTime()->PushSaveTime();

    Savedata::GameTime gameTime;
    GameSys::DeviceDate date;
    gameTime.SetDate( date.GetDateConst() );
    game_data->GetPlayTime()->SetSaveTime( &gameTime );

    game_data->SaveStart( m_pWork->GetDeviceHeap()->GetLowerHandle() );
    m_isSaveRequest = true;
  }

  return false;
}

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)
