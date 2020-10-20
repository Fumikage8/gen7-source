//======================================================================
/**
 * @file P2pCommonData.cpp
 * @date 2015/07/06 14:32:28
 * @author ichiraku_katsuhiko
 * @brief P2P共通データクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <gflnet2/include/nex/gflnet2_NexManager.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

// mine
#include "P2pCommonData.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)

// コンストラクタ
P2pCommonData::P2pCommonData( gfl2::heap::HeapBase* pHeap, gflnet2::p2p::NetGame* pNetGame, JoinFestaScript::CommRouteType commRouteType ) :
  m_pHeap(pHeap)
  ,m_pNetGame(pNetGame)
  ,m_recvCnt(0)
  ,m_timingCommand(FINISH_TIMING_NO_MYSTATUS)
  ,m_state(STATE_NULL)
  ,m_commRouteType(commRouteType)
{
  for( int i=0; i<CONNET_TARGET_MAX; i++ )
  {
    m_recvData[i].m_pMyStatus = NULL;
    m_recvData[i].m_pPokeParty = NULL;
    m_isValidationOk[i] = false;
  }
}

// デストラクタ
P2pCommonData::~P2pCommonData()
{
  for( int i=0; i<CONNET_TARGET_MAX; i++ )
  {
    GFL_SAFE_DELETE(m_recvData[i].m_pMyStatus);
    GFL_SAFE_DELETE(m_recvData[i].m_pPokeParty);
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief MyStatusデータ送受信開始
 * @return  送信準備完了したらtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool P2pCommonData::SendMyStatus()
{
  PacketData sendData; // 送信データ
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );

  const Savedata::MyStatus* myStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

  // マイステータス　シリアライズ
  myStatus->Serialize( sendData.my_status_core );

  // 送受信完了同期番号
  m_timingCommand = FINISH_TIMING_NO_MYSTATUS;
  return SendPacket( &sendData, sizeof(PacketData), NET_COMMANDO_MYSTATUS );
}

//------------------------------------------------------------------
/**
 * @brief PokePartyデータ送受信開始
 * @param[in] pPokeParty    ポケパーティ
 * @param[in] pCerdData     署名データ
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool P2pCommonData::SendPokeParty( const pml::PokeParty* pPokeParty, const void* pCerdData )
{
  PacketData sendData; // 送信データ
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );

  // ポケパーティのシリアライズ
  pPokeParty->SerializeCore( &sendData.poke_party_core );

  // 署名ある時のみ
  if( pCerdData != NULL )
  {
    // 署名コピー
    gfl2::std::MemCopy( pCerdData, sendData.cert_data, SIGNATURE_SIZE );
    sendData.cert_data_size = SIGNATURE_SIZE;
    sendData.cert_data_enable = true;
    ICHI_PRINT(">ICHI 署名チェック送信 size %d \n", sendData.cert_data_size );
    for( int i=0; i<sendData.cert_data_size; i++ )
    {
      ICHI_PRINT("%d ", sendData.cert_data[i] );
      if( i%32 == 0 ){ ICHI_PRINT("\n"); }
    }
  }
  else
  {
    sendData.cert_data_enable = false;
  }

  // 送受信完了同期番号
  m_timingCommand = FINISH_TIMING_NO_POKEPARTY;
  return SendPacket( &sendData, sizeof(PacketData), NET_COMMANDO_POKEPARTY );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief JoinFestaPersonalDataデータ送受信開始
 * @return  送信準備完了したらtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool P2pCommonData::SendJoinFestaPersonalData()
{
  PacketData sendData; // 送信データ
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );

  NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
  pMyData->SerializeCoreData( sendData.join_festa_personal_data );

  // 送受信完了同期番号
  m_timingCommand = FINISH_TIMING_NO_JF_PERSONAL;
  return SendPacket( &sendData, sizeof(PacketData), NET_COMMANDO_JF_PERSONAL );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 共通データ送受信終了したか
 * @return 終了していたらtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool P2pCommonData::IsRecvEnd()
{
  return (m_state == STATE_RECV_END);
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 更新
 */
/* ----------------------------------------------------------------------------------------- */
void P2pCommonData::Update()
{
  // 受信完了するまでRecvData回す
  if( m_state == STATE_RECV_WAIT )
  {
    RecvData();
  }
  else if( m_state == STATE_TIMING )
  {
    // @fix NMCat[4097]：NBRでP2pConnectionManagerの同期をすると、自分から切断した場合の切断同期と被る
    if( m_timingCommand == FINISH_TIMING_NO_POKEPARTY )
    {
      GFL_RELEASE_PRINT("NMCat[4097]確認用：この後終了シーケンスに行くと発生していた\n");
      m_state = STATE_RECV_END; // 受信完了
    }
    // 最終同期待ち
    else if( m_pNetGame->IsTimingLoopCheck( m_timingCommand ) )
    {
      m_state = STATE_RECV_END; // 受信完了
    }
  }
}

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  パケットデータ送信処理
 * @param[in] packet      送信したいパケットデータ
 * @param[in] packetSize  送信したいパケットデータサイズ
 * @param[in] command     ユーザーコマンド
 * @return  送信準備完了したらtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool P2pCommonData::SendPacket( void* packet, u32 packetSize, const u8 command )
{
#ifdef  GF_PLATFORM_CTR
  // コマンド付き送信
  if( m_pNetGame->SendDataCommand( reinterpret_cast<char *>(packet), packetSize, command ) )
  {
    ICHI_PRINT("P2pCommonData::SendPacket:コマンド %d \n", command );

    m_state = STATE_RECV_WAIT; // 受信中
    m_recvCnt = 0;
    return true;
  }
  return false;
#else
  PacketData*  recv_data = (PacketData*)packet;// 受信データ
  // Win版は通信動かないので受信した体で。
  for( int i=0; i<CONNET_TARGET_MAX; i++ )
  {
    RecvPacket( i, *recv_data, command );
  }
  m_state = STATE_RECV_END; // 受信完了
  return true;
#endif
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  データ受信
 */
/* ----------------------------------------------------------------------------------------- */
void P2pCommonData::RecvData()
{
  int         id;       // 通信ID
  PacketData  recv_data;// 受信データ
  u8          command;  // 受信コマンド
  u16         size = 0; // 受け取ったデータサイズ

  // データがなくなるまで処理
  while( m_pNetGame->RecvDataCommand( &id, reinterpret_cast<char *>(&recv_data), sizeof(PacketData), &command, &size ) == true )
  {
    if( size != sizeof(PacketData) )
    {
      GFL_ASSERT_MSG( 0, "受信データのサイズ不一致! %d \n", size ); // @check
    }
    ICHI_PRINT("%d からデータ受信 コマンド %d\n", id, command );

    // パケットデータ処理
    RecvPacket( id, recv_data, command );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  受信したパケットデータ処理
 * @param[in] netId   通信ID
 * @param[in] packet  受信したパケットデータ
 * @param[in] command ユーザーコマンド
 */
/* ----------------------------------------------------------------------------------------- */
void P2pCommonData::RecvPacket( const int netId, const P2pCommonData::PacketData& packet, const u8 command )
{
  
  switch( command ){
  case NET_COMMANDO_MYSTATUS:
    {
      GFL_ASSERT( m_recvData[ netId ].m_pMyStatus == NULL );

      // 受信データ作成
      m_recvData[ netId ].m_pMyStatus = GFL_NEW( m_pHeap ) Savedata::MyStatus();
      m_recvData[ netId ].m_pMyStatus->Deserialize( packet.my_status_core );

      // @fix MMCat[337]：相手がnijiの場合フラグをたてる。
      if( m_recvData[netId].m_pMyStatus->GetRomCode() == VERSION_SUN ||
          m_recvData[netId].m_pMyStatus->GetRomCode() == VERSION_MOON )
      {
        m_recvData[netId].m_pMyStatus->SetDressUpParamRomVersionNiji();
      }

      ICHI_PRINT("%d のマイステータス受信 ", netId );
      ICHI_PRINT("ID:%d\n", m_recvData[ netId ].m_pMyStatus->GetID() );
    }
    break;

  case NET_COMMANDO_POKEPARTY:
    {
      // なかったら作る
      if( m_recvData[ netId ].m_pPokeParty == NULL )
      {
        m_recvData[ netId ].m_pPokeParty = GFL_NEW( m_pHeap ) pml::PokeParty( m_pHeap );
      }
      m_recvData[ netId ].m_pPokeParty->Clear();

      // デシリアライズ
      m_recvData[ netId ].m_pPokeParty->DeserializeCore( &packet.poke_party_core );

      // 署名チェック
      if( packet.cert_data_enable == true )
      {
        ICHI_PRINT(">ICHI [%d]の署名チェック開始 size %d \n", netId, packet.cert_data_size );
        for( int i=0; i<packet.cert_data_size; i++ )
        {
          ICHI_PRINT("%d ", packet.cert_data[i] );
          if( i%32 == 0 ){ ICHI_PRINT("\n"); }
        }

        m_isValidationOk[ netId ] = CheckRsaValidation( m_recvData[ netId ].m_pPokeParty, packet.cert_data, packet.cert_data_size );
        ICHI_PRINT(">ICHI 結果 %d \n", m_isValidationOk[ netId ] );
      }
    }
    break;

  case NET_COMMANDO_JF_PERSONAL:
    {
      // LIVE大会とふしぎなおくりものは無視する
      if( m_commRouteType >= JoinFestaScript::COMM_ROUTE_MAX )
      {
        break;
      }

#if defined(GF_PLATFORM_CTR)
      // 自分のデータは無視する
      if( m_pNetGame->GetNetID() != netId )
      {
        // 受信したCoreDataからJoinFestaPersonalData作成
        NetAppLib::JoinFesta::JoinFestaPersonalData recvPersonal;
        recvPersonal.DeserializeCoreData( packet.join_festa_personal_data );

        // @fix NMCat[4004]：一度パケット経由で更新をかける事で、受信拒否処理が通る
        NetAppLib::JoinFesta::JoinFestaPacketManager* pJoinFestaPacketManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager );

        // 通信拒否設定で弾かれなかったら、改めてフルデータ更新
        if( pJoinFestaPacketManager->IsRejectPersonal( recvPersonal ) != true )
        {
          // 通信経路を上書き
          recvPersonal.SetCommRoute( m_commRouteType );
          GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->UpdatePersonalDataWithPersonal( recvPersonal );
          GFL_PRINT("P2Pによるパーソナルデータ更新成功!!!\n");
        }
        else
        {
          GFL_PRINT("P2Pによるパーソナルデータ更新失敗!!!\n");
        }
      }
#endif // GF_PLATFORM_CTR
    }
    break;
  }

  // 受信カウンタ
  m_recvCnt++;

  // 全員分受け取ったらOK(自分含む）
  if( m_recvCnt == (m_pNetGame->GetConnectionCount()) )
  {
    m_state = STATE_TIMING; // 同期
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ポケモンの改ざんチェック
 * @param[in]   pCheckParty     チェックするポケパーティ
 * @param[in]   pCertData       証明書データ
 * @param[in]   pCertDataSize   証明書データサイズ
 * @return  改ざんが行われていないならtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool P2pCommonData::CheckRsaValidation( const pml::PokeParty* pCheckParty, const u8* pCerdData, const u32 cerdDataSize )
{

#if defined(GF_PLATFORM_CTR)
  NetLib::Validation::PokemonValidation* pValidation = GFL_NEW_LOW( m_pHeap ) NetLib::Validation::PokemonValidation();

  // 初期化
  if( !pValidation->Initialize(
        m_pHeap, m_pHeap,
        GFL_SINGLETON_INSTANCE( gflnet2::nex::NexManager )->GetAuthenticationToken(),
        NetLib::Validation::PokemonValidation::E_GAME_MODE_BATTLE_M ) ) // チェック関数なのでモードはnijiでもmomijiでも良い
  {
    GFL_ASSERT(0);
    GFL_SAFE_DELETE( pValidation );
    return false;
  }

  // ポケモンセット
  u8 pokeNum = 0;

  for( int i=0; i<pCheckParty->GetMemberCount(); i++ )
  {
    const pml::pokepara::PokemonParam* pp = pCheckParty->GetMemberPointerConst( i );
    
    // コアデータシリアライズ
    NetApp::GTS::GtsSerializePokemonParam gtsSerializePokemonParam;
    gfl2::std::MemClear( &gtsSerializePokemonParam, sizeof( NetApp::GTS::GtsSerializePokemonParam ) );
    pp->Serialize_Core( gtsSerializePokemonParam.data );

    // 不正チェックで処理するデータのセット
    pValidation->SetPokemon( i, NULL, &gtsSerializePokemonParam );
    ICHI_PRINT(">ICHI 改ざんチェックPokemon[%d] %d \n", i, pp->GetMonsNo() );
  }

  // 証明書を使用して改ざんチェック
  bool ret = pValidation->CheckRsaValidation( pCheckParty->GetMemberCount(), pCerdData, cerdDataSize );

  // 破棄
  pValidation->Finalize();
  GFL_SAFE_DELETE( pValidation );

  return ret;
#else
  return true;
#endif // GF_PLATFORM_CTR
}

GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)
