// ============================================================================
/*
 * @file P2PTradeSequenceGuest.cpp
 * @brief P2P交換アプリで使用する通信モジュールです。
 * @date 2015.09.28
 * @author Akira endo
 */
// ============================================================================
#include "NetStatic/NetAppLib/source/P2PTrade/P2PTradeSequenceGuest.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonTradeUtility.h"
#include "App/Box/include/BoxAppParam.h"
#include <util/include/gfl2_std_string.h>
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// P2P切断用
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"

#include <pml/include/item/item.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


P2PTradeSequenceGuest::P2PTradeSequenceGuest(
  gfl2::heap::HeapBase*                         pHeap,
  bool&                                         bOutCallZukanRegister,
  app::event::ZukanRegisterEventParam&          outZukanRegisterEventParam,
  bool&                                         bOutCallEvolution,
  NetEvent::Evolution::EvolutionEventParam&     outEvolutionEventParam ) :
  NetAppLib::P2PTrade::P2PTradeSequenceBase( pHeap, bOutCallZukanRegister, outZukanRegisterEventParam, bOutCallEvolution, outEvolutionEventParam ),
  m_bRecvVerifyStartFlag( false ),
  m_bRecvFirstTradeResultFlag( false ),
  m_bSendCancelFirstTradeFlag( false ),
  m_bRecvFinalTradeResultFlag( false ),
  m_bSendCancelFinalTradeFlag( false ),
  m_bSendFinalSyncCommandFlag( false ),
  m_bRecvFinalSyncCompleteCommandFlag( false ),
  m_bSendFinalSyncCancelCommandFlag( false ),
  m_bRecvFinalSyncCancelCompleteCommandFlag( false )
{
}


P2PTradeSequenceGuest::~P2PTradeSequenceGuest()
{
}


void P2PTradeSequenceGuest::Update( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener )
{
  m_P2PTradeNet.Update();

  switch( getState() )
  {
    // --------------------------------------------------------------------------------------------
    case STATE_SEND_POKEMON:
    {
      if( m_P2PTradeNet.SecureSendDataCommand( m_SendPokemonBuffer, sizeof( m_SendPokemonBuffer ), COMMAND_ID_POKEMON_PARAM ) )
      {
        setState( STATE_RECV_POKEMON );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_RECV_POKEMON:
    {
      if( m_bRecvVerifyStartFlag )
      {
        setState( STATE_VERIFY );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_VERIFY:
    {
#if defined( GF_PLATFORM_CTR )

      if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
      {
        GFL_PRINT( "NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI\n" );

        // 不正チェックサーバmomiji対応:相手がnijiの場合はnijiのモード
        NetLib::Validation::PokemonValidation::GameMode gameMode = NetLib::Validation::PokemonValidation::E_GAME_MODE_TRADE_M;
        if( GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager )->IsNijiConnect() )
        {
          gameMode = NetLib::Validation::PokemonValidation::E_GAME_MODE_TRADE;
        }

        if( m_NetAppPokemonValidationUtility.Verify( gameMode, &m_RecvPokemonParam ) )
        {
          NetAppLib::Util::NetAppPokemonValidationUtility::VerifyResult eVerifyResult = m_NetAppPokemonValidationUtility.GetVerifyResult();

          switch( eVerifyResult )
          {
            case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_OK:
            {
              GFL_PRINT( "NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_OK\n" );

              m_eVerifyResult = VERIFY_RESULT_SUCCESS;
              setState( STATE_SEND_VERIFY_RESULT );
            }
            break;

            case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NG:
            {
              GFL_PRINT( "NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NG\n" );

              m_eVerifyResult = VERIFY_RESULT_POKEMON_INVALID;
              setState( STATE_SEND_VERIFY_RESULT );
            }
            break;

//            case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NET_ERROR:
            default:
            {
              GFL_PRINT( "NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NET_ERROR\n" );

              m_eVerifyResult = VERIFY_RESULT_ERROR;
              setState( STATE_SEND_VERIFY_RESULT );
            }
            break;
          }
        }
      }
      else
      {
        GFL_PRINT( "NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI\n" );

        m_eVerifyResult = VERIFY_RESULT_SUCCESS;
        setState( STATE_SEND_VERIFY_RESULT );
      }

#elif defined( GF_PLATFORM_WIN32 )

      m_eVerifyResult = VERIFY_RESULT_ERROR;
      setState( STATE_NOTIFICATION_FIRST_TRADE_RESULT );

#endif // defined( GF_PLATFORM_CTR )
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_SEND_VERIFY_RESULT:
    {
      u32 verifyResult = static_cast<u32>( m_eVerifyResult );
      if( m_P2PTradeNet.SecureSendDataCommand( &verifyResult, sizeof( u32 ), COMMAND_ID_VERIFY_RESULT ) )
      {
        setState( STATE_RECV_FIRST_TRADE_RESULT );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_RECV_FIRST_TRADE_RESULT:
    {
      if( m_bRecvFirstTradeResultFlag )
      {
        setState( STATE_NOTIFICATION_FIRST_TRADE_RESULT );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_NOTIFICATION_FIRST_TRADE_RESULT:
    {
      bool bTradeSuccess = false;

      if( pP2PTradeResponseListener )
      {
        GFL_PRINT( "---------------------------------------------\n" );
        GFL_PRINT( "------------------- Guest -------------------\n" );
        GFL_PRINT( "--- STATE_NOTIFICATION_FIRST_TRADE_RESULT ---\n" );

        if( VERIFY_RESULT_ERROR == static_cast<VerifyResult>( m_eVerifyResult ) )
        {
          // 自分の通信エラー(エラーアプレットが表示される)
          GFL_PRINT( "pP2PTradeResponseListener->OnFirstTradeValidationError( true );\n" );
          pP2PTradeResponseListener->OnFirstTradeValidationError( true );
        }
        else
        if( VERIFY_RESULT_ERROR == static_cast<VerifyResult>( m_FirstTradeResult.verifyResultHost ) )
        {
          // 相手の通信エラー(アプリ側でエラーメッセージを表示する)
          GFL_PRINT( "pP2PTradeResponseListener->OnFirstTradeValidationError( false );\n" );
          pP2PTradeResponseListener->OnFirstTradeValidationError( false );
        }
        else
        if( VERIFY_RESULT_POKEMON_INVALID == static_cast<VerifyResult>( m_FirstTradeResult.verifyResultHost ) )
        {
          // 自分のポケモンが不正(※相手が自分のポケモンを不正チェックしている為、ここは自分のポケモンが不正を通知しています)
          GFL_PRINT( "pP2PTradeResponseListener->OnFirstTradePokemonInvalid( true );\n" );
          pP2PTradeResponseListener->OnFirstTradePokemonInvalid( true );
        }
        else
        if( VERIFY_RESULT_POKEMON_INVALID == static_cast<VerifyResult>( m_eVerifyResult ) )
        {
          // 相手のポケモンが不正(※自分は相手のポケモンを不正チェックしている為、ここは相手のポケモンが不正を通知しています)
          GFL_PRINT( "pP2PTradeResponseListener->OnFirstTradePokemonInvalid( false );\n" );
          pP2PTradeResponseListener->OnFirstTradePokemonInvalid( false );
        }
        else
        if( m_FirstTradeResult.tradeSuccess )
        {
          // 交換成立
          GFL_PRINT( "pP2PTradeResponseListener->OnFirstTradeSuccess;\n" );
          pP2PTradeResponseListener->OnFirstTradeSuccess( m_RecvPokemonParam );
          bTradeSuccess = true;
        }
        else
        {
          GFL_PRINT( "pP2PTradeResponseListener->OnFirstTradeValidationError( false );\n" );
          GFL_ASSERT( NULL ); // @fix : ここには来ないはず、もしも来た時は相手の通信エラーとする
          pP2PTradeResponseListener->OnFirstTradeValidationError( false );
        }
      }

      if( bTradeSuccess )
      {
        GFL_PRINT( "setState( STATE_WAIT_FINAL_TRADE_REQUEST );\n" );
        setState( STATE_WAIT_FINAL_TRADE_REQUEST );
      }
      else
      {
        resetFlag();
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_SEND_FINAL_TRADE:
    {
      u8 commandID = static_cast<u8>( COMMAND_ID_FINAL_TRADE );
      if( m_P2PTradeNet.SecureSendDataCommand( &commandID, sizeof( u8 ), commandID ) )
      {
        GFL_PRINT( "setState( STATE_RECV_FINAL_TRADE_RESULT );\n" );
        setState( STATE_RECV_FINAL_TRADE_RESULT );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_RECV_FINAL_TRADE_RESULT:
    {
      if( m_bRecvFinalTradeResultFlag )
      {
        GFL_PRINT( "setState( STATE_NOTIFICATION_FINAL_TRADE_SUCCESS );\n" );
        setState( STATE_NOTIFICATION_FINAL_TRADE_SUCCESS );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_NOTIFICATION_FINAL_TRADE_SUCCESS:
    {
      m_bIsFinalTradeSuccess = true;

      if( pP2PTradeResponseListener )
      {
        GFL_PRINT( "pP2PTradeResponseListener->OnFinalTradeSuccess;\n" );
        pP2PTradeResponseListener->OnFinalTradeSuccess();
      }

      GFL_PRINT( "setState( STATE_SAVE_PREPARE );\n" );
      setState( STATE_SAVE_PREPARE );
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_SAVE_PREPARE:
    {
      if( m_bIsStartSyncSaveRequestFlag )
      {
        GFL_PRINT( "setState( STATE_SAVE_START );\n" );
        setState( STATE_SAVE_START );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_SAVE_START:
    {
      NetAppLib::Util::NetAppCommonSaveUtility::SetDisconnectDetectedFlag( true );

      GFL_PRINT( "setState( STATE_SAVE_WAIT );\n" );
      setState( STATE_SAVE_WAIT );
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_SAVE_WAIT:
    {
      if( m_NetAppCommonSaveUtility.Save() )
      {
        NetAppLib::Util::NetAppCommonSaveUtility::SetDisconnectDetectedFlag( false );

        GFL_PRINT( "setState( STATE_SYNC_SAVE_PREPARE_SYNC_START );\n" );
        setState( STATE_SYNC_SAVE_PREPARE_SYNC_START );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_SYNC_SAVE_PREPARE_SYNC_START:
    {
#if defined( GF_PLATFORM_CTR )

      m_P2PTradeNet.GetNetGame()->TimingStart( SYNC_NO_4 );

      GFL_PRINT( "setState( STATE_SYNC_SAVE_PREPARE_SYNC_WAIT );\n" );
      setState( STATE_SYNC_SAVE_PREPARE_SYNC_WAIT );

#endif // defiend( GF_PLATFORM_CTR )
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_SYNC_SAVE_PREPARE_SYNC_WAIT:
    {
#if defined( GF_PLATFORM_CTR )

      if( m_P2PTradeNet.GetNetGame()->IsTimingEnd( SYNC_NO_4 ) )
      {
        GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SaveCriticalSpanOn();

        GFL_PRINT( "setState( STATE_SYNC_SAVE_START );\n" );
        setState( STATE_SYNC_SAVE_START );
      }

#endif // defiend( GF_PLATFORM_CTR )
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_SYNC_SAVE_START:
    {
      pml::pokepara::PokemonParam sendPokemon( m_pHeap );
      sendPokemon.Deserialize_Core( m_SendPokemonBuffer );
      if( item::ITEM_CheckPiece( m_RecvPokemonParam.GetItem() ) )
      {
        m_RecvPokemonParam.RemoveItem();
      }
      NetAppLib::Util::NetAppCommonTradeUtility::UpdateSaveData( m_pHeap, NetAppLib::Util::TRADE_ID_P2P, m_bInBox, m_Tray, m_Pos, &m_RecvPokemonParam, &sendPokemon, m_bCallZukanRegister, m_ZukanRegisterEventParam, m_bCallEvolution, m_EvolutionEventParam );
      startSyncSave();
      GFL_PRINT( "setState( STATE_SYNC_SAVE_WAIT );\n" );
      setState( STATE_SYNC_SAVE_WAIT );
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_SYNC_SAVE_WAIT:
    {
      if( updateSyncSave() )
      {
        GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SaveCriticalSpanOff();

        GFL_PRINT( "setState( STATE_NOTIFICATION_FINAL_TRADE_COMPLETE );\n" );
        setState( STATE_NOTIFICATION_FINAL_TRADE_COMPLETE );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_NOTIFICATION_FINAL_TRADE_COMPLETE:
    {
      m_bIsFinalTradeComplete = true;

      if( pP2PTradeResponseListener )
      {
        GFL_PRINT( "pP2PTradeResponseListener->OnFinalTradeComplete;\n" );
        pP2PTradeResponseListener->OnFinalTradeComplete();
      }

      GFL_PRINT( "setState( STATE_FINAL_SYNC );\n" );
      setState( STATE_FINAL_SYNC );
    }
    break;

    // --------------------------------------------------------------------------------------------
    case STATE_SEND_FORCE_CANCEL:
    {
      u8 commandID = static_cast<u8>( COMMAND_ID_FORCE_CANCEL );
      if( m_P2PTradeNet.SecureSendDataCommand( &commandID, sizeof( u8 ), commandID ) )
      {
        resetFlag();
      }
    }
    break;
  }

  tradeCancel( pP2PTradeResponseListener );
  tradeExit( pP2PTradeResponseListener );
  finalSync( pP2PTradeResponseListener );

}


void P2PTradeSequenceGuest::tradeCancel( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener )
{
  // --------------------------------------------------------------------------------------------
  // キャンセル処理
  {
    if( m_bRecvCancelFirstTradeFlag )
    {
      if( pP2PTradeResponseListener )
      {
        pP2PTradeResponseListener->OnFirstTradeCancel();
      }
      resetFlag();
    }

    if( m_bCancelFirstTradeFlag )
    {
      if( !m_bSendCancelFirstTradeFlag )
      {
        u8 commandID = static_cast<u8>( COMMAND_ID_CANCEL_FIRST_TRADE );
        if( m_P2PTradeNet.SecureSendDataCommand( &commandID, sizeof( u8 ), commandID ) )
        {
          m_bSendCancelFirstTradeFlag = true;
        }
      }
    }
  }

  // --------------------------------------------------------------------------------------------
  // キャンセル処理
  {
    if( m_bRecvCancelFinalTradeFlag )
    {
      if( pP2PTradeResponseListener )
      {
        pP2PTradeResponseListener->OnFinalTradeCancel();
      }
      resetFlag();
    }

    if( m_bCancelFinalTradeFlag )
    {
      if( !m_bSendCancelFinalTradeFlag )
      {
        u8 commandID = static_cast<u8>( COMMAND_ID_CANCEL_FINAL_TRADE );
        if( m_P2PTradeNet.SecureSendDataCommand( &commandID, sizeof( u8 ), commandID ) )
        {
          m_bSendCancelFinalTradeFlag = true;
        }
      }
    }
  }

  // --------------------------------------------------------------------------------------------
  // キャンセル処理
  {
    if( m_bRecvForceCancelFlag )
    {
      if( pP2PTradeResponseListener )
      {
        pP2PTradeResponseListener->OnTradeCancelReceived();
      }
      resetFlag();
    }
  }
}


void P2PTradeSequenceGuest::tradeExit( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener )
{
  // --------------------------------------------------------------------------------------------
  // 終了処理
  {
    if( m_bExitTradeFlag )
    {
      u8 commandID = static_cast<u8>( COMMAND_ID_EXIT );
      if( m_P2PTradeNet.SecureSendDataCommand( &commandID, sizeof( u8 ), commandID ) )
      {
        m_bExitTradeFlag = false;
      }
    }

    if( m_bRecvExitTradeFlag )
    {
      if( pP2PTradeResponseListener )
      {
        GFL_PRINT( "Guest : pP2PTradeResponseListener->OnTradeExitReceived(); [1] \n" );

        pP2PTradeResponseListener->OnTradeExitReceived();
      }
      else
      {
        m_bCallExitReceivedHandler = true;
      }

      // 裏で切断を呼んでおく
      // ここで呼ばないと、この後にストリームメッセージ表示でキー待ちがあるところで待つことになる。
      // そうすると、相手側は真っ暗画面で切断タイムアウトを待つ事になる。
      // ここで切断しておくことで、相手の切断がスムーズに終了する
      GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->DisconnectStart();

      resetFlag();
    }

    if( m_bRecvExitResponseFlag )
    {
      if( pP2PTradeResponseListener )
      {
        GFL_PRINT( "Guest : pP2PTradeResponseListener->OnTradeExitSuccess();\n" );

        pP2PTradeResponseListener->OnTradeExitSuccess();
      }
      resetFlag();
    }

    if( m_bCallExitReceivedHandler )
    {
      if( pP2PTradeResponseListener )
      {
        GFL_PRINT( "Guest : pP2PTradeResponseListener->OnTradeExitReceived(); [2] \n" );

        pP2PTradeResponseListener->OnTradeExitReceived();

        m_bCallExitReceivedHandler = false;
      }
    }
  }
}


void P2PTradeSequenceGuest::finalSync( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener )
{
  if( m_bSendFinalSyncCommandFlag )
  {
    u8 commandID = static_cast<u8>( COMMAND_ID_FINAL_SYNC );
    if( m_P2PTradeNet.SecureSendDataCommand( &commandID, sizeof( u8 ), commandID ) )
    {
      m_bSendFinalSyncCommandFlag = false;
    }
  }

  if( m_bRecvFinalSyncCompleteCommandFlag )
  {
    if( pP2PTradeResponseListener )
    {
      GFL_PRINT( "pP2PTradeResponseListener->OnFinalSyncComplete();\n" );
      pP2PTradeResponseListener->OnFinalSyncComplete();
    }

    resetFlag();

    // 同期完了の通知をしたのでここで終了
    return;
  }

  if( m_bSendFinalSyncCancelCommandFlag )
  {
    u8 commandID = static_cast<u8>( COMMAND_ID_CANCEL_FINAL_SYNC );
    if( m_P2PTradeNet.SecureSendDataCommand( &commandID, sizeof( u8 ), commandID ) )
    {
      m_bSendFinalSyncCancelCommandFlag = false;
    }
  }

  if( m_bRecvFinalSyncCancelCompleteCommandFlag )
  {
    if( pP2PTradeResponseListener )
    {
      GFL_PRINT( "pP2PTradeResponseListener->OnFinalSyncCancelComplete();\n" );
      pP2PTradeResponseListener->OnFinalSyncCancelComplete();

      resetFlag();
    }

    // 裏で切断を呼んでおく
    // ここで呼ばないと、この後にストリームメッセージ表示でキー待ちがあるところで待つことになる。
    // そうすると、相手側は真っ暗画面で切断タイムアウトを待つ事になる。
    // ここで切断しておくことで、相手の切断がスムーズに終了する
    GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->DisconnectStart();
  }

}


void P2PTradeSequenceGuest::OnRecvDataCommand( void* pRecvData, u8 commandID )
{
  switch( commandID )
  {
    case COMMAND_ID_POKEMON_PARAM:
    {
      if( getState() <= STATE_RECV_POKEMON )
      {
        m_RecvPokemonParam.Deserialize_Core( pRecvData );
        m_RecvPokemonFlag = true;

#if defined(GF_PLATFORM_CTR)
        NetLib::Validation::PokemonValidation::ReplaceMonsName( &m_RecvPokemonParam );
#endif // defined(GF_PLATFORM_CTR)

        GFL_PRINT( "Guest Recv pokemon mons no = %d\n", m_RecvPokemonParam.GetMonsNo() );
      }
      else
      {
        GFL_ASSERT_STOP( NULL ); // @fix : ここに来たらプログラム改ざんの可能性があるので停止させる
      }
    }
    break;

    case COMMAND_ID_VERIFY_START:
    {
      m_bRecvVerifyStartFlag = true;
    }
    break;

    case COMMAND_ID_FIRST_TRADE_RESULT:
    {
      gfl2::std::MemCopy( pRecvData, &m_FirstTradeResult, sizeof( FirstTradeResult ) );
      m_bRecvFirstTradeResultFlag = true;
    }
    break;

    case COMMAND_ID_CANCEL_FIRST_TRADE:
    {
      m_bRecvCancelFirstTradeFlag = true;
    }
    break;

    case COMMAND_ID_FINAL_TRADE_RESULT:
    {
      m_bRecvFinalTradeResultFlag = true;
    }
    break;

    case COMMAND_ID_CANCEL_FINAL_TRADE:
    {
      m_bRecvCancelFinalTradeFlag = true;
    }
    break;

    case COMMAND_ID_FORCE_CANCEL:
    {
      m_bRecvForceCancelFlag = true;
    }
    break;

    case COMMAND_ID_EXIT:
    {
      m_bRecvExitTradeFlag = true;
    }
    break;

    case COMMAND_ID_EXIT_RESPONSE:
    {
      m_bRecvExitResponseFlag = true;
    }
    break;

    case COMMAND_ID_FINAL_SYNC_COMPLETE:
    {
      m_bRecvFinalSyncCompleteCommandFlag = true;
    }
    break;

    case COMMAND_ID_CANCEL_FINAL_SYNC_COMPLETE:
    {
      m_bRecvFinalSyncCancelCompleteCommandFlag = true;
    }
    break;
  }
}


void P2PTradeSequenceGuest::AddFirstTradeRequest( const pml::pokepara::PokemonParam& pokemonParam, u8 tray, u8 pos )
{
  GFL_ASSERT( getState() == STATE_IDLE );

  m_bInBox = ( tray == App::Box::RETURN_TEMOTI ) ? false : true;
  m_Tray   = static_cast<u32>( tray );
  m_Pos    = static_cast<u32>( pos );

  m_sendMonsNo = pokemonParam.GetMonsNo();
  m_sendFormNo = pokemonParam.GetFormNo();

  GFL_PRINT( "Guest m_sendMonsNo = %d\n", m_sendMonsNo );

  pokemonParam.Serialize_Core( m_SendPokemonBuffer );
  setState( STATE_SEND_POKEMON );
}


void P2PTradeSequenceGuest::CancelFirstTradeRequest()
{
  m_bCancelFirstTradeFlag = true;
}


void P2PTradeSequenceGuest::AddFinalTradeRequest()
{
  GFL_ASSERT( getState() == STATE_WAIT_FINAL_TRADE_REQUEST );
  setState( STATE_SEND_FINAL_TRADE );
}


void P2PTradeSequenceGuest::CancelFinalTradeRequest()
{
  m_bCancelFinalTradeFlag = true;
}


void P2PTradeSequenceGuest::AddSendTradeCancelRequest()
{
  GFL_ASSERT( getState() == STATE_WAIT_FINAL_TRADE_REQUEST );
  setState( STATE_SEND_FORCE_CANCEL );
}


void P2PTradeSequenceGuest::AddTradeExitRequest()
{
  m_bExitTradeFlag = true;
}


void P2PTradeSequenceGuest::AddFinalSyncRequest()
{
  if( getState() == STATE_FINAL_SYNC )
  {
    m_bSendFinalSyncCommandFlag = true;
  }
}


void P2PTradeSequenceGuest::AddFinalSyncCancelRequest()
{
  if( getState() == STATE_FINAL_SYNC )
  {
    m_bSendFinalSyncCancelCommandFlag = true;
  }
}


void P2PTradeSequenceGuest::resetFlag()
{
  P2PTradeSequenceBase::resetFlag();

  m_bRecvVerifyStartFlag                    = false;
  m_bRecvFirstTradeResultFlag               = false;
  m_bSendCancelFirstTradeFlag               = false;
  m_bRecvFinalTradeResultFlag               = false;
  m_bSendCancelFinalTradeFlag               = false;
  m_bSendFinalSyncCommandFlag               = false;
  m_bRecvFinalSyncCompleteCommandFlag       = false;
  m_bSendFinalSyncCancelCommandFlag         = false;
  m_bRecvFinalSyncCancelCompleteCommandFlag = false;
}


void P2PTradeSequenceGuest::DebugPrint()
{
  GFL_PRINT( "Guest -----------------------------------------------\n" );
  P2PTradeSequenceBase::DebugPrint();
  GFL_PRINT( "m_bRecvVerifyStartFlag                      = %d\n", m_bRecvVerifyStartFlag );
  GFL_PRINT( "m_bRecvFirstTradeResultFlag                 = %d\n", m_bRecvFirstTradeResultFlag );
  GFL_PRINT( "m_bSendCancelFirstTradeFlag                 = %d\n", m_bSendCancelFirstTradeFlag );
  GFL_PRINT( "m_bRecvFinalTradeResultFlag                 = %d\n", m_bRecvFinalTradeResultFlag );
  GFL_PRINT( "m_bSendCancelFinalTradeFlag                 = %d\n", m_bSendCancelFinalTradeFlag );
  GFL_PRINT( "m_bSendFinalSyncCommandFlag                 = %d\n", m_bSendFinalSyncCommandFlag );
  GFL_PRINT( "m_bRecvFinalSyncCompleteCommandFlag         = %d\n", m_bRecvFinalSyncCompleteCommandFlag );
  GFL_PRINT( "m_bSendFinalSyncCancelCommandFlag           = %d\n", m_bSendFinalSyncCancelCommandFlag );
  GFL_PRINT( "m_bRecvFinalSyncCancelCompleteCommandFlag   = %d\n", m_bRecvFinalSyncCancelCompleteCommandFlag );
}


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)

