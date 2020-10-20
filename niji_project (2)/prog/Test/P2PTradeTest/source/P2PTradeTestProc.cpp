// ============================================================================
/*
 * @file P2PTradeTestProc.cpp
 * @brief P2PTradeRequestClientクラスのテストです。
 * @date 2015.09.25
 * @author Akira endo
 */
// ============================================================================
#if PM_DEBUG

#include "Test/P2PTradeTest/include/P2PTradeTestProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(P2PTradeTest)


P2PTradeTestProc::P2PTradeTestProc() :
  m_pRoModule( NULL ),
  m_pP2PTradeRequestClient( NULL ),
  m_eState( STATE_SELECT_POKEMON ),
  m_pSendPokemon( NULL ),
  m_pRecvPokemon( NULL )
{
}


P2PTradeTestProc::~P2PTradeTestProc()
{
}


#if defined( GF_PLATFORM_CTR )

gfl2::proc::Result P2PTradeTestProc::InitFunc( gfl2::proc::Manager* pManager )
{
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  switch( GetSubSeq() )
  {
    case 0:
    {
      NetLib::NijiNetworkSystem::InitializeLocalP2PNetwork();
      AddSubSeq();
    }
    break;

    case 1:
    {
      pP2pConnectionManager->Initialize( NetLib::P2P::NIJI_P2P_TRADE_L );
      AddSubSeq();
    }
    break;

    case 2:
    {
      if( pP2pConnectionManager->ConnectStart( gflnet2::p2p::AUTO ) )
      {
        AddSubSeq();
      }
    }
    break;

    case 3:
    {
      if( pP2pConnectionManager->IsConnectSuccess() )
      {
        AddSubSeq();
      }
    }
    break;

    case 4:
    {
      gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
//      gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
//      m_pRoModule = roMgr->LoadModule( pAsyncFileManager, "P2PTrade.cro" );
//      roMgr->StartModule( m_pRoModule );

      gfl2::heap::HeapBase *pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      m_pP2PTradeRequestClient = GFL_NEW( pDeviceHeap ) NetAppLib::P2PTrade::P2PTradeRequestClient( pDeviceHeap );
      m_pP2PTradeRequestClient->SetP2PTradeResponseListener( this );

      m_pSendPokemon = GFL_NEW( pDeviceHeap ) pml::pokepara::PokemonParam( pDeviceHeap, static_cast<MonsNo>( 1 ), static_cast<u16>( 1 ), static_cast<u64>( 1 ) );
      m_pRecvPokemon = GFL_NEW( pDeviceHeap ) pml::pokepara::PokemonParam( pDeviceHeap );

      AddSubSeq();
    }
    break;

    case 5:
    {
      if( m_pP2PTradeRequestClient->Initialize() )
      {
        return gfl2::proc::RES_FINISH;
      }
    }
    break;
  }

  return gfl2::proc::RES_CONTINUE;
}


gfl2::proc::Result P2PTradeTestProc::EndFunc( gfl2::proc::Manager* pManager )
{
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  switch( GetSubSeq() )
  {
    case 0:
    {
      GFL_SAFE_DELETE( m_pRecvPokemon );
      GFL_SAFE_DELETE( m_pSendPokemon );

      m_pP2PTradeRequestClient->RemoveP2PTradeResponseListener();
      GFL_SAFE_DELETE( m_pP2PTradeRequestClient );

//      gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
//      roMgr->DisposeModule( m_pRoModule );

      AddSubSeq();
    }
    break;

    case 1:
    {
      pP2pConnectionManager->DisconnectStart();
      AddSubSeq();
    }
    break;

    case 2:
    {
      if( pP2pConnectionManager->IsDisconnectSuccess() )
      {
        AddSubSeq();
      }
    }
    break;

    case 3:
    {
      pP2pConnectionManager->Terminate();
      AddSubSeq();
    }
    break;

    case 4:
    {
      NetLib::NijiNetworkSystem::TerminateNetwork();
      return gfl2::proc::RES_FINISH;
    }
    break;
  }

  return gfl2::proc::RES_CONTINUE;
}


gfl2::proc::Result P2PTradeTestProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();

  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

  if( m_pP2PTradeRequestClient )
  {
    m_pP2PTradeRequestClient->Update();
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    GFL_PRINT( "AddSendMessageIDRequest\n" );
    gfl2::heap::HeapBase *pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
    pml::pokepara::PokemonParam pokemonParam( pDeviceHeap );
    m_pP2PTradeRequestClient->AddSendMessageIDRequest( NetAppLib::P2PTrade::MESSAGE_ID_GOODBYE, MONSNO_HUSIGIDANE, pokemonParam );
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    GFL_PRINT( "AddSendPokemonParamRequest\n" );
    m_pP2PTradeRequestClient->AddSendPokemonParamRequest( *m_pSendPokemon );
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_R ) )
  {
    GFL_PRINT( "m_eState = %d\n", m_eState );
    if( m_pP2PTradeRequestClient )
    {
      m_pP2PTradeRequestClient->DebugPrint();
    }
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_L ) )
  {
    GFL_PRINT( "AddTradeExitRequest\n" );
    m_pP2PTradeRequestClient->AddTradeExitRequest();
  }

  switch( m_eState )
  {
    case STATE_SELECT_POKEMON:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        GFL_PRINT( "AddFirstTradeRequest\n" );
        m_pP2PTradeRequestClient->AddFirstTradeRequest( *m_pSendPokemon, 0, 0 );
        m_eState = STATE_WAIT_FIRST_TRADE_RESULT;
      }
      else
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        return gfl2::proc::RES_FINISH;
      }
    }
    break;

    case STATE_WAIT_FIRST_TRADE_RESULT:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        GFL_PRINT( "CancelFirstTradeRequest\n" );
        m_pP2PTradeRequestClient->CancelFirstTradeRequest();
      }
    }
    break;

    case STATE_TRADE_CONFIRM:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        GFL_PRINT( "AddFinalTradeRequest\n" );
        m_pP2PTradeRequestClient->AddFinalTradeRequest();
        m_eState = STATE_WAIT_FINAL_TRADE_RESULT;
      }
      else
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        GFL_PRINT( "AddSendTradeCancelRequest\n" );
        m_pP2PTradeRequestClient->AddSendTradeCancelRequest();
        m_eState = STATE_SELECT_POKEMON;
      }
    }
    break;

    case STATE_WAIT_FINAL_TRADE_RESULT:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        GFL_PRINT( "CancelFinalTradeRequest\n" );
        m_pP2PTradeRequestClient->CancelFinalTradeRequest();
      }
    }
    break;

    case STATE_TRADE_COMPLETE:
    {
      return gfl2::proc::RES_FINISH;
    }
    break;
  }

  return gfl2::proc::RES_CONTINUE;
}

#else

gfl2::proc::Result P2PTradeTestProc::InitFunc( gfl2::proc::Manager* pManager )
{
  return gfl2::proc::RES_FINISH;
}

gfl2::proc::Result P2PTradeTestProc::EndFunc( gfl2::proc::Manager* pManager )
{
  return gfl2::proc::RES_FINISH;
}

gfl2::proc::Result P2PTradeTestProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  return gfl2::proc::RES_FINISH;
}

#endif // defiend( GF_PLATFORM_CTR )


void P2PTradeTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{

}


//------------------------------------------------------------------
/**
  *  @brief    メッセージIDを受信した時に呼ばれるハンドラです
  *  @param    eMessageID 受信したメッセージID
  *  @param    monsno 欲しいポケモンの種族番号
  *  @note     関連関数 AddSendMessageIDRequest
  */
//------------------------------------------------------------------
void P2PTradeTestProc::OnMessageIDReceived( NetAppLib::P2PTrade::MessageID eMessageID, MonsNo monsno, void* pPokemonParamBuffer )
{
  GFL_PRINT( "OnMessageIDReceived\n" );
}


//------------------------------------------------------------------
/**
  *  @brief    ポケモンのパラメータを受信した時に呼ばれるハンドラです
  *  @param    pokemonParam 受信したポケモンのパラメータ
  *  @note     関連関数 AddSendPokemonParamRequest
  */
//------------------------------------------------------------------
void P2PTradeTestProc::OnPokemonParamReceived( const pml::pokepara::PokemonParam& pokemonParam )
{
  GFL_PRINT( "OnPokemonParamReceived\n" );
}


//------------------------------------------------------------------
/**
  *  @brief    最初の交換が成立した時に呼ばれるハンドラです
  *  @param    pokemonParam 交換相手のポケモンのパラメータ
  *  @note     関連関数 AddFirstTradeRequest
  */
//------------------------------------------------------------------
void P2PTradeTestProc::OnFirstTradeSuccess( const pml::pokepara::PokemonParam& pokemonParam )
{
  GFL_PRINT( "OnFirstTradeSuccess\n" );
  m_eState = STATE_TRADE_CONFIRM;
}


//------------------------------------------------------------------
/**
  *  @brief    交換相手のポケモンが不正だった時に呼ばれるハンドラです
  *  @param    bMyPokemonInvalid [ true : 自分のポケモンが不正 / false : 相手のポケモンが不正 ]
  *  @note     関連関数 AddFirstTradeRequest
  */
//------------------------------------------------------------------
void P2PTradeTestProc::OnFirstTradePokemonInvalid( bool bMyPokemonInvalid )
{
  GFL_PRINT( "OnFirstTradePokemonInvalid\n" );
  m_eState = STATE_SELECT_POKEMON;
}


//------------------------------------------------------------------
/**
  *  @brief    不正チェックサーバーへ接続出来なかった時などに呼ばれるハンドラです
  *  @param    bMyValidationError [ true : 自分の通信エラー / false : 相手の通信エラー ]
  *  @note     関連関数 AddFirstTradeRequest
  */
//------------------------------------------------------------------
void P2PTradeTestProc::OnFirstTradeValidationError( bool bMyValidationError )
{
  GFL_PRINT( "OnFirstTradeValidationError\n" );
  m_eState = STATE_SELECT_POKEMON;
}


//------------------------------------------------------------------
/**
  *  @brief    最初の交換リクエストがキャンセルされた時に呼ばれるハンドラです
  *  @note     関連関数 CancelFirstTradeRequest
  */
//------------------------------------------------------------------
void P2PTradeTestProc::OnFirstTradeCancel()
{
  GFL_PRINT( "OnFirstTradeCancel\n" );
  m_eState = STATE_SELECT_POKEMON;
}


//------------------------------------------------------------------
/**
  *  @brief    最後の交換が成立した時に呼ばれるハンドラです
  *  @note     関連関数 AddFinalTradeRequest
  */
//------------------------------------------------------------------
void P2PTradeTestProc::OnFinalTradeSuccess()
{
  GFL_PRINT( "OnFinalTradeSuccess\n" );
}


//------------------------------------------------------------------
/**
  *  @brief    最後の交換が完了した時に呼ばれるハンドラです
  *  @note     関連関数 AddFinalTradeRequest
  */
//------------------------------------------------------------------
void P2PTradeTestProc::OnFinalTradeComplete()
{
  GFL_PRINT( "OnFinalTradeComplete\n" );
  m_eState = STATE_TRADE_COMPLETE;
}


//------------------------------------------------------------------
/**
  *  @brief    最後の交換リクエストがキャンセルされた時に呼ばれるハンドラです
  *  @note     関連関数 CancelFinalTradeRequest
  */
//------------------------------------------------------------------
void P2PTradeTestProc::OnFinalTradeCancel()
{
  GFL_PRINT( "OnFinalTradeCancel\n" );
  m_eState = STATE_SELECT_POKEMON;
}


//------------------------------------------------------------------
/**
  *  @brief    交換キャンセルを受信した時に呼ばれるハンドラです
  *  @note     関連関数 AddSendTradeCancelRequest
  */
//------------------------------------------------------------------
void P2PTradeTestProc::OnTradeCancelReceived()
{
  GFL_PRINT( "OnTradeCancelReceived\n" );
  m_eState = STATE_SELECT_POKEMON;
}


void P2PTradeTestProc::OnTradeExitReceived()
{
  GFL_PRINT( "OnTradeExitReceived\n" );
  m_eState = STATE_SELECT_POKEMON;
}


void P2PTradeTestProc::OnTradeExitSuccess()
{
  GFL_PRINT( "OnTradeExitSuccess\n" );
  m_eState = STATE_SELECT_POKEMON;
}

  
GFL_NAMESPACE_END(P2PTradeTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
