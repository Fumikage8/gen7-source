// ============================================================================
/*
 * @file P2PTradeSequenceBase.cpp
 * @brief P2P交換アプリで使用する通信モジュールです。
 * @date 2015.09.28
 * @author Akira endo
 */
// ============================================================================
#include "NetStatic/NetAppLib/source/P2PTrade/P2PTradeSequenceBase.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <util/include/gfl2_std_string.h>
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


P2PTradeSequenceBase::P2PTradeSequenceBase(
  gfl2::heap::HeapBase*                         pHeap,
  bool&                                         bOutCallZukanRegister,
  app::event::ZukanRegisterEventParam&          outZukanRegisterEventParam,
  bool&                                         bOutCallEvolution,
  NetEvent::Evolution::EvolutionEventParam&     outEvolutionEventParam ) :
  m_pHeap( pHeap ),
#if defined( GF_PLATFORM_CTR )
  m_NetAppPokemonValidationUtility( pHeap ),
#endif // defined( GF_PLATFORM_CTR )
  m_NetAppCommonSaveUtility(),
  m_bCallZukanRegister( bOutCallZukanRegister ),
  m_ZukanRegisterEventParam( outZukanRegisterEventParam ),
  m_bCallEvolution( bOutCallEvolution ),
  m_EvolutionEventParam( outEvolutionEventParam ),
  m_bInBox( false ),
  m_Tray( 0 ),
  m_Pos( 0 ),
  m_P2PTradeNet( *this ),
  m_SendPokemonBuffer(),
  m_RecvPokemonParam( pHeap ),
  m_RecvPokemonFlag( false ),
  m_eVerifyResult( VERIFY_RESULT_ERROR ),
  m_FirstTradeResult(),
  m_bCancelFirstTradeFlag( false ),
  m_bRecvCancelFirstTradeFlag( false ),
  m_bCancelFinalTradeFlag( false ),
  m_bRecvCancelFinalTradeFlag( false ),
  m_bRecvForceCancelFlag( false ),
  m_bExitTradeFlag( false ),
  m_bRecvExitTradeFlag( false ),
  m_bRecvExitResponseFlag( false ),
  m_bIsFinalTradeSuccess( false ),
  m_bIsFinalTradeComplete( false ),
  m_sendMonsNo(0),
  m_sendFormNo(0),
  m_bIsStartSyncSaveRequestFlag( false ),
  m_bCallExitReceivedHandler( false ),
  m_pP2pSyncSave( NULL ),
  m_eState( STATE_IDLE )
{
  resetFlag();

#if defined( GF_PLATFORM_CTR )
  m_NetAppPokemonValidationUtility.SetServerErrorListener( this );
#endif // defined( GF_PLATFORM_CTR )
}


P2PTradeSequenceBase::~P2PTradeSequenceBase()
{
#if defined( GF_PLATFORM_CTR )
  m_NetAppPokemonValidationUtility.SetServerErrorListener( NULL );//念のため
#endif // defined( GF_PLATFORM_CTR )
}


bool P2PTradeSequenceBase::Initialize()
{
  return m_P2PTradeNet.Initialize();
}


void P2PTradeSequenceBase::Finalize()
{
  m_P2PTradeNet.Finalize();
}


bool P2PTradeSequenceBase::CanDestroy()
{
  bool bCanDestroy = true;

#if defined( GF_PLATFORM_CTR )

  m_NetAppPokemonValidationUtility.SetServerErrorListener( NULL );
  bCanDestroy = m_NetAppPokemonValidationUtility.CanDestroy();

#endif // defined( GF_PLATFORM_CTR )

  // セーブが開始されてセーブが終わってなければ破棄できない
  if( m_NetAppCommonSaveUtility.IsExecute() )
  {
    if( !m_NetAppCommonSaveUtility.Save() )
    {
      bCanDestroy = false;
    }
  }

  return bCanDestroy;
}


NetAppLib::P2PTrade::P2PTradeSequenceBase::State P2PTradeSequenceBase::getState()
{
  return m_eState;
}


void P2PTradeSequenceBase::setState( State eState )
{
  m_eState = eState;
}


void P2PTradeSequenceBase::resetFlag()
{
  m_RecvPokemonFlag             = false;

  m_eVerifyResult               = VERIFY_RESULT_ERROR;
  gfl2::std::MemClear( &m_FirstTradeResult, sizeof( FirstTradeResult ) );

  m_bCancelFirstTradeFlag       = false;
  m_bRecvCancelFirstTradeFlag   = false;
  m_bCancelFinalTradeFlag       = false;
  m_bRecvCancelFinalTradeFlag   = false;
  m_bRecvForceCancelFlag        = false;
  m_bExitTradeFlag              = false;
  m_bRecvExitTradeFlag          = false;
  m_bRecvExitResponseFlag       = false;
  
  setState( STATE_IDLE );
}


void P2PTradeSequenceBase::StartSyncSaveRequest()
{
  m_bIsStartSyncSaveRequestFlag = true;
}


void P2PTradeSequenceBase::ResetFlag()
{
  m_bIsFinalTradeSuccess        = false;
  m_bIsFinalTradeComplete       = false;
  m_sendMonsNo                  = 0;
  m_sendFormNo                  = 0;
  m_bIsStartSyncSaveRequestFlag = false;

  resetFlag();
}


void P2PTradeSequenceBase::DebugPrint()
{
  GFL_PRINT( "m_eState                      = %d\n", m_eState );
  GFL_PRINT( "m_RecvPokemonFlag             = %d\n", m_RecvPokemonFlag );
  GFL_PRINT( "m_eVerifyResult               = %d\n", m_eVerifyResult );
  GFL_PRINT( "m_bCancelFirstTradeFlag       = %d\n", m_bCancelFirstTradeFlag );
  GFL_PRINT( "m_bRecvCancelFirstTradeFlag   = %d\n", m_bRecvCancelFirstTradeFlag );
  GFL_PRINT( "m_bCancelFinalTradeFlag       = %d\n", m_bCancelFinalTradeFlag );
  GFL_PRINT( "m_bRecvCancelFinalTradeFlag   = %d\n", m_bRecvCancelFinalTradeFlag );
  GFL_PRINT( "m_bRecvForceCancelFlag        = %d\n", m_bRecvForceCancelFlag );
}


void P2PTradeSequenceBase::startSyncSave()
{
  if( m_pP2pSyncSave == NULL )
  {
    NetLib::P2P::P2pSyncSave::PARAM param;
    param.gameData  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
#if defined( GF_PLATFORM_CTR )
    param.netGame   = m_P2PTradeNet.GetNetGame();
#endif // defiend( GF_PLATFORM_CTR )
    param.SyncNo1   = SYNC_NO_1;
    param.SyncNo2   = SYNC_NO_2;
    param.SyncNo3   = SYNC_NO_3;
    param.pHeap     = m_pHeap;
    param.StartWait = 100;
    param.sendMonsNo = m_sendMonsNo;
    param.sendFormNo = m_sendFormNo;
    param.recvMonsNo = m_RecvPokemonParam.GetMonsNo();
    param.recvFormNo = m_RecvPokemonParam.GetFormNo();
    m_pP2pSyncSave = GFL_NEW( m_pHeap ) NetLib::P2P::P2pSyncSave( &param );
  }
}


bool P2PTradeSequenceBase::updateSyncSave()
{
  bool bResult = true;

  if( m_pP2pSyncSave )
  {
    bResult = false;

    int nResult = m_pP2pSyncSave->UpdateSyncSave();

    if( nResult == 1 )
    {
      GFL_SAFE_DELETE( m_pP2pSyncSave );

      bResult = true;
    }
  }

  return bResult;
}


#if defined( GF_PLATFORM_CTR )
//サーバエラーが発生した瞬間に通知する為だけのイベント
void P2PTradeSequenceBase::OnServerError( void )
{
  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsConnectSuccess() )
  {//接続状態のときのみ呼ぶ
    GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->DisconnectStart( true );
  }
}
#endif // defined( GF_PLATFORM_CTR )

GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)

