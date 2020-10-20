// ============================================================================
/*
 * @file P2PTradeRequestClient.cpp
 * @brief P2P交換アプリで使用する通信リクエストを投げるクラスです。
 * @date 2015.07.24
 * @author Akira endo
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeRequestClient.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeResponseListener.h"
#include "NetStatic/NetAppLib/source/P2PTrade/P2PTradeSequenceHost.h"
#include "NetStatic/NetAppLib/source/P2PTrade/P2PTradeSequenceGuest.h"
#include <util/include/gfl2_std_string.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


P2PTradeRequestClient::P2PTradeRequestClient( gfl2::heap::HeapBase* pHeap ) :
  m_pHeap( pHeap ),
  m_pP2PTradeResponseListener( NULL ),
  m_MessageNet( *this ),
  m_PokemonParamNet( *this ),
  m_LastRecvMessageInfo(),
  m_LastRecvPokemonParam( pHeap ),
  m_pTradeSequence( NULL ),
  m_InitializeSequence( 0 ),
  m_bCallZukanRegister( false ),
  m_ZukanRegisterEventParam(),
  m_bCallEvolution( false ),
  m_EvolutionEventParam()
{
}


P2PTradeRequestClient::~P2PTradeRequestClient()
{
  if( m_pTradeSequence )
  {
    m_pTradeSequence->Finalize();
    GFL_SAFE_DELETE( m_pTradeSequence );
  }
  m_PokemonParamNet.Finalize();
  m_MessageNet.Finalize();
}


//------------------------------------------------------------------
/**
  *  @brief    初期化処理
  *  @retval   true  初期化終了
  *  @retval   false 初期化中
  */
//------------------------------------------------------------------
bool P2PTradeRequestClient::Initialize()
{
  bool bResult = false;

  switch( m_InitializeSequence )
  {
    case 0:
    {
      if( m_MessageNet.Initialize() )
      {
        m_InitializeSequence = 1;
      }
    }
    break;

    case 1:
    {
      if( m_PokemonParamNet.Initialize() )
      {
        m_InitializeSequence = 2;
      }
    }
    break;

    case 2:
    {
      if( gflnet2::p2p::NetGame::IsMaster() )
      {
        m_pTradeSequence = GFL_NEW( m_pHeap ) NetAppLib::P2PTrade::P2PTradeSequenceHost( m_pHeap, m_bCallZukanRegister, m_ZukanRegisterEventParam, m_bCallEvolution, m_EvolutionEventParam );
      }
      else
      {
        m_pTradeSequence = GFL_NEW( m_pHeap ) NetAppLib::P2PTrade::P2PTradeSequenceGuest( m_pHeap, m_bCallZukanRegister, m_ZukanRegisterEventParam, m_bCallEvolution, m_EvolutionEventParam );
      }
      m_InitializeSequence = 3;
    }
    break;

    case 3:
    {
      if( m_pTradeSequence->Initialize() )
      {
        bResult = true;
      }
    }
    break;
  }

  return bResult;
}


//------------------------------------------------------------------
/**
  *  @brief    更新処理
  *  @note     P2PTradeEvent の Update で呼び出します
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::Update()
{
  m_MessageNet.Update();
  m_PokemonParamNet.Update();
  if( m_pTradeSequence )
  {
    m_pTradeSequence->Update( m_pP2PTradeResponseListener );
  }
}


//------------------------------------------------------------------
/**
  *  @brief    破棄できるか
  *  @note     P2PTradeEvent の MainFunc の SEQUENCE_FINISH の時に呼び出します
  */
//------------------------------------------------------------------
bool P2PTradeRequestClient::CanDestroy()
{
  bool bCanDestroy = true;
  if( m_pTradeSequence )
  {
    bCanDestroy = m_pTradeSequence->CanDestroy();
  }
  return bCanDestroy;
}


//------------------------------------------------------------------
/**
  *  @brief    P2P交換の通信レスポンスリスナーの登録を行います
  *  @param    pListener P2P交換の通信レスポンスリスナー
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::SetP2PTradeResponseListener( NetAppLib::P2PTrade::P2PTradeResponseListener* pListener )
{
  m_pP2PTradeResponseListener = pListener;
}


//------------------------------------------------------------------
/**
  *  @brief    P2P交換の通信レスポンスリスナーの解除を行います
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::RemoveP2PTradeResponseListener()
{
  m_pP2PTradeResponseListener = NULL;
}


//------------------------------------------------------------------
/**
  *  @brief    メッセージIDを送信するリクエストの追加を行います
  *  @param    eMessageID 送信するメッセージID
  *  @param    monsno 欲しいポケモンの種族番号
  *  @note     自分が AddSendMessageIDRequest をコールすると
  *            相手の OnMessageIDReceived がコールされます
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::AddSendMessageIDRequest( NetAppLib::P2PTrade::MessageID eMessageID, MonsNo monsno, const pml::pokepara::PokemonParam& pokemonParam )
{
  NetAppLib::P2PTrade::MessageInfo messageInfo;
  messageInfo.messageID = static_cast<u32>( eMessageID );
  messageInfo.monsno    = static_cast<u32>( monsno );
  pokemonParam.Serialize_Full( messageInfo.pokemonParamBuffer );
  m_MessageNet.SendDataCommand( &messageInfo, sizeof( NetAppLib::P2PTrade::MessageInfo ), COMMAND_ID_MESSAGE_INFO );
}


//------------------------------------------------------------------
/**
  *  @brief    最後に受信したメッセージIDを取得します
  *  @return   最後に受信したメッセージID
  */
//------------------------------------------------------------------
NetAppLib::P2PTrade::MessageID P2PTradeRequestClient::GetLastRecvMessageID()
{
  return static_cast<NetAppLib::P2PTrade::MessageID>( m_LastRecvMessageInfo.messageID );
}


//------------------------------------------------------------------
/**
  *  @brief    最後に受信した欲しいポケモンの種族番号を取得します
  *  @return   最後に受信した欲しいポケモンの種族番号
  */
//------------------------------------------------------------------
MonsNo P2PTradeRequestClient::GetLastRecvMonsNo()
{
  return static_cast<MonsNo>( m_LastRecvMessageInfo.monsno );
}


//------------------------------------------------------------------
/**
  *  @brief    最後に受信した欲しいポケモンのPokemonParamのバッファを取得します
  *  @return   最後に受信した欲しいポケモンのPokemonParamのバッファ
  */
//------------------------------------------------------------------
void* P2PTradeRequestClient::GetLastRecvPokemonParamBuffer()
{
  return m_LastRecvMessageInfo.pokemonParamBuffer;
}


//------------------------------------------------------------------
/**
  *  @brief    ポケモンのパラメータを送信するリクエストの追加を行います
  *  @param    pokemonParam 送信するポケモンのパラメータ
  *  @note     自分が AddSendPokemonParamRequest をコールすると
  *            相手の OnPokemonParamReceived がコールされます
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::AddSendPokemonParamRequest( const pml::pokepara::PokemonParam& pokemonParam )
{
  u8 buffer[ pml::pokepara::PokemonParam::DATASIZE ];
  pokemonParam.Serialize_Core( buffer );
  m_PokemonParamNet.SendDataCommand( buffer, sizeof( buffer ), COMMAND_ID_POKEMON_PARAM );
}


//------------------------------------------------------------------
/**
  *  @brief    最後に受信したポケモンのパラメータを取得します
  *  @return   最後に受信したポケモンのパラメータ
  */
//------------------------------------------------------------------
const pml::pokepara::PokemonParam& P2PTradeRequestClient::GetLastRecvPokemonParam()
{
  return m_LastRecvPokemonParam;
}


//------------------------------------------------------------------
/**
  *  @brief    最初の交換リクエストの追加を行います
  *  @param    pokemonParam 交換するポケモンのパラメータ
  *  @param    tray         ボックスのトレイ番号
  *  @param    pos          ボックス内の位置
  *  @note     最初のコマンドメニューで交換を選んだ時に使用します
  *            受信した相手のポケモンの不正チェックも行います
  *            自分が AddFirstTradeRequest をコールすると
  *            自分の OnFirstTradeSuccess
  *                   OnFirstTradePokemonInvalid
  *                   OnFirstTradeValidationError のどれかがコールされます
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::AddFirstTradeRequest( const pml::pokepara::PokemonParam& pokemonParam, u8 tray, u8 pos )
{
  GFL_PRINT( "P2PTradeRequestClient::AddFirstTradeRequest\n" );

  if( m_pTradeSequence )
  {
    m_pTradeSequence->AddFirstTradeRequest( pokemonParam, tray, pos );
  }
}


//------------------------------------------------------------------
/**
  *  @brief    最初の交換リクエストのキャンセルを行います
  *  @note     最初の交換リクエストを投げた後の待機画面で、
  *            Bボタンを押した時に使用します
  *            自分が CancelFirstTradeRequest をコールすると
  *            自分の OnFirstTradeCancel
  *                   OnFirstTradeSuccess
  *                   OnFirstTradePokemonInvalid
  *                   OnFirstTradeValidationError のどれかがコールされます
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::CancelFirstTradeRequest()
{
  GFL_PRINT( "P2PTradeRequestClient::CancelFirstTradeRequest\n" );

  if( m_pTradeSequence )
  {
    m_pTradeSequence->CancelFirstTradeRequest();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    最後の交換リクエストの追加を行います
  *  @note     2回目のコマンドメニューで交換を選んだ時に使用します
  *            自分が AddFinalTradeRequest をコールすると
  *            自分の OnFinalTradeSuccess
  *                   OnFinalTradeComplete
  *                   OnTradeCancelReceived のどれかがコールされます
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::AddFinalTradeRequest()
{
  GFL_PRINT( "P2PTradeRequestClient::AddFinalTradeRequest\n" );

  if( m_pTradeSequence )
  {
    m_pTradeSequence->AddFinalTradeRequest();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    最後の交換リクエストのキャンセルを行います
  *  @note     最後の交換リクエストを投げた後の待機画面で、
  *            Bボタンを押した時に使用します
  *            自分が CancelFinalTradeRequest をコールすると
  *            自分の OnFinalTradeCancel
  *                   OnFinalTradeSuccess
  *                   OnTradeCancelReceived のどれかがコールされます
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::CancelFinalTradeRequest()
{
  GFL_PRINT( "P2PTradeRequestClient::CancelFinalTradeRequest\n" );

  if( m_pTradeSequence )
  {
    m_pTradeSequence->CancelFinalTradeRequest();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    交換キャンセルを送信するリクエストの追加を行います
  *  @note     2回目のコマンドメニューでやめるを選んだ時に使用します
  *            自分が AddSendTradeCancelRequest をコールすると
  *            相手の OnTradeCancelReceived がコールされます
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::AddSendTradeCancelRequest()
{
  GFL_PRINT( "P2PTradeRequestClient::AddSendTradeCancelRequest\n" );

  if( m_pTradeSequence )
  {
    m_pTradeSequence->AddSendTradeCancelRequest();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    交換終了を送信するリクエストの追加を行います
  *  @note     自分が AddTradeExitRequest をコールすると
  *            相手の OnTradeExitReceived がコールされ
  *            自分の OnTradeExitSuccess がコールされます
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::AddTradeExitRequest()
{
  GFL_PRINT( "P2PTradeRequestClient::AddTradeExitRequest\n" );

  if( m_pTradeSequence )
  {
    m_pTradeSequence->AddTradeExitRequest();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    同期セーブを開始のリクエストを投げます
  *  @note     P2PTradeEvent で呼び出す
  */
//------------------------------------------------------------------
void P2PTradeRequestClient::StartSyncSaveRequest()
{
  GFL_PRINT( "P2PTradeRequestClient::StartSyncSaveRequest\n" );

  if( m_pTradeSequence )
  {
    m_pTradeSequence->StartSyncSaveRequest();
  }
}


void P2PTradeRequestClient::AddFinalSyncRequest()
{
  if( m_pTradeSequence )
  {
    m_pTradeSequence->AddFinalSyncRequest();
  }
}


void P2PTradeRequestClient::AddFinalSyncCancelRequest()
{
  if( m_pTradeSequence )
  {
    m_pTradeSequence->AddFinalSyncCancelRequest();
  }
}


void P2PTradeRequestClient::ResetFlag()
{
  if( m_pTradeSequence )
  {
    m_pTradeSequence->ResetFlag();
  }

  gfl2::std::MemClear( &m_LastRecvMessageInfo, sizeof( NetAppLib::P2PTrade::MessageInfo ) );
  m_LastRecvPokemonParam.Clear();
}


//------------------------------------------------------------------
/**
  *  @brief    最後の交換が成立したか判定します
  *  @retval   true 最後の交換が成立した
  *  @retval   false 最後の交換が成立していない
  *  @note     P2PTradeResponseListener の OnFinalTradeSuccess のフラグチェック版の関数です
  */
//------------------------------------------------------------------
bool P2PTradeRequestClient::IsFinalTradeSuccess()
{
  bool bResult = false;

  if( m_pTradeSequence )
  {
    bResult = m_pTradeSequence->IsFinalTradeSuccess();
  }

  return bResult;
}


//------------------------------------------------------------------
/**
  *  @brief    最後の交換が完了したか判定します
  *  @retval   true 最後の交換が完了した
  *  @retval   false 最後の交換が完了していない
  *  @note     P2PTradeResponseListener の OnFinalTradeComplete のフラグチェック版の関数です
  */
//------------------------------------------------------------------
bool P2PTradeRequestClient::IsFinalTradeComplete()
{
  bool bResult = false;

  if( m_pTradeSequence )
  {
    bResult = m_pTradeSequence->IsFinalTradeComplete();
  }

  return bResult;
}


void P2PTradeRequestClient::OnRecvDataCommand( void* pRecvData, u8 commandID )
{
  switch( commandID )
  {
    // -------------------------------------------------------------
    case COMMAND_ID_MESSAGE_INFO:
    {
      gfl2::std::MemCopy( pRecvData, &m_LastRecvMessageInfo, sizeof( NetAppLib::P2PTrade::MessageInfo ) );
      if( m_pP2PTradeResponseListener )
      {
        m_pP2PTradeResponseListener->OnMessageIDReceived( static_cast<NetAppLib::P2PTrade::MessageID>( m_LastRecvMessageInfo.messageID ), static_cast<MonsNo>( m_LastRecvMessageInfo.monsno ), m_LastRecvMessageInfo.pokemonParamBuffer );
      }
    }
    break;

    // -------------------------------------------------------------
    case COMMAND_ID_POKEMON_PARAM:
    {
      m_LastRecvPokemonParam.Deserialize_Core( pRecvData );

#if defined(GF_PLATFORM_CTR)
      NetLib::Validation::PokemonValidation::ReplaceMonsName( &m_LastRecvPokemonParam );
#endif // defined(GF_PLATFORM_CTR)

      if( m_pP2PTradeResponseListener )
      {
        m_pP2PTradeResponseListener->OnPokemonParamReceived( m_LastRecvPokemonParam );
      }
    }
    break;
  }
}


void P2PTradeRequestClient::DebugPrint()
{
  if( m_pTradeSequence )
  {
    m_pTradeSequence->DebugPrint();
  }
}


pml::pokepara::PokemonParam& P2PTradeRequestClient::GetRecvPokemonParam()
{
  return m_pTradeSequence->GetRecvPokemonParam();
}


u8* P2PTradeRequestClient::GetSendPokemonBuffer()
{
  return m_pTradeSequence->GetSendPokemonBuffer();
}


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)

