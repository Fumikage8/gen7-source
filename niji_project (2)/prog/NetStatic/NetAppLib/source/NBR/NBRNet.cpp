//======================================================================
/**
 * @file NBRNet.cpp
 * @date 2015/07/08 15:02:53
 * @author ichiraku_katsuhiko
 * @brief 通信対戦受付アプリの通信モジュール
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_DebugPrint.h>

// NetGame
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
// P2P
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
// Regulation
#include "Battle/Regulation/include/Regulation.h"

// Listener
#include "NetStatic/NetAppLib/include/NBR/NBRNetRecvListener.h"
// mine
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)


// コンストラクタ
NBRNet::NBRNet( gfl2::heap::HeapBase* pHeap ) :
   m_pHeap(pHeap)
  ,m_pRecvListener(NULL)
  ,m_pP2pPacketTransfer(NULL)
  ,m_sendRegulationPacket()
  ,m_recvPoolBufferList()
{
  m_pNetGame = GFL_NEW( pHeap ) gflnet2::p2p::NetGame();
  m_pP2pPacketTransfer = GFL_NEW( pHeap ) NetLib::P2P::P2pPacketTransfer( pHeap, this );
  m_recvPoolBufferList.CreateBuffer( pHeap, RECV_POOL_BUFFER_NUM );
  ClearRecvPacketEnableCommand();
}

// デストラクタ
NBRNet::~NBRNet()
{
  m_recvPoolBufferList.Clear();
  GFL_SAFE_DELETE( m_pP2pPacketTransfer );
  GFL_SAFE_DELETE( m_pNetGame );
}

//------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------
void NBRNet::Initialize()
{
  m_pNetGame->Initialize();
}

//------------------------------------------------------------------
/**
 * @brief   初期化終了待ち
 */
//------------------------------------------------------------------
bool NBRNet::IsInitialize()
{
  return m_pNetGame->IsInitialize();
}

//------------------------------------------------------------------
/**
 * @brief   終了
 */
//------------------------------------------------------------------
void NBRNet::Terminate()
{
  m_pNetGame->Finalize();
}

//------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------
void NBRNet::Update()
{
#ifdef  GF_PLATFORM_CTR
  // P2P接続していない場合何もしない
  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsConnectSuccess() == false )
  {
    return;
  }

  // プールバッファ更新
  PoolBufferUpdate();

  // 受信更新
  RecvDataUpdate();

  // 分割送信更新
  m_pP2pPacketTransfer->Update();
#else
  // Win版は何もしない
#endif // GF_PLATFORM_CTR
}

//------------------------------------------------------------------
/**
 * @brief   親かどうか
 * @return  trueで親
 */
//------------------------------------------------------------------
bool NBRNet::IsMaster()
{
  return m_pNetGame->IsMaster();
}

//------------------------------------------------------------------
/**
 * @brief   接続順番 = NetIDをかえす
 * @return  NetID
 */
//------------------------------------------------------------------
int NBRNet::GetNetID()
{
  return m_pNetGame->GetNetID();
}

//------------------------------------------------------------------
/**
 * @brief   Wifi接続かどうか
 * @return  trueでWifi接続
 */
//------------------------------------------------------------------
bool NBRNet::IsWifi()
{
#ifdef  GF_PLATFORM_CTR
  return GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsWifi();
#else
  return false;
#endif
}

//------------------------------------------------------------------
/**
 * @brief   接続人数を取得
 * @return  接続人数（自分含む）
 */
//------------------------------------------------------------------
u8 NBRNet::GetConnectionNum()
{
#ifdef  GF_PLATFORM_CTR
  return static_cast<u8>(m_pNetGame->GetConnectionCount());
#else
  return 4;
#endif
}

//------------------------------------------------------------------
/**
 * @brief   プールしている受信バッファをクリアする
 */
//------------------------------------------------------------------
void NBRNet::ClearRecvPoolBuffer()
{
  // 全要素を削除
  for( gfl2::util::ListIterator<RecvBuffer> it=m_recvPoolBufferList.Begin(); it!=m_recvPoolBufferList.End();  )
  {
    it = m_recvPoolBufferList.Erase( it );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief  タイミングをそろえるコマンドを発行確認まで内部でやってくれる
 * @param  timingUniqueNo    タイミングをそろえるためのユニーク番号 ０以外
 * @return bool              到着したタイミングから次のスタートを行うまでずっとtrue
 */
//---------------------------------------------------------------------------------
bool NBRNet::IsTimingLoopCheck( const NBRNetDefine::TimingCommand timing )
{
  return m_pNetGame->IsTimingLoopCheck( timing );
}

//=============================================================================
/**
 *  送信
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief  通信終了通知送信
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendEnd()
{
  PacketData sendData;
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_END );
}

//------------------------------------------------------------------
/**
 * @brief     ルール選ぶ人の候補者優先順位通知送信（親のみ）
 * @param[in] ルール選ぶ人の候補者優先順位（NetIdが要素で優先度が格納されている）
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendPriority( const u8 priority[ NBRNetDefine::NET_NUM_MAX ] )
{
  PacketData sendData;
  gfl2::std::MemCopy( priority, sendData.priority, NBRNetDefine::NET_NUM_MAX );
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_PRIORITY );
}

//------------------------------------------------------------------
/**
 * @brief     ルール選択者通知送信
 * @param[in] ルール選択者通知コマンド
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendRuleSelector( const NBRNetDefine::RuleSelectorCommand command )
{
  PacketData sendData;
  sendData.rule_selector_command = static_cast<u8>(command);
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_RULE_SELECTOR );
}

//------------------------------------------------------------------
/**
 * @brief  ルール選択を他の人に任せる通知送信
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendRuleOther()
{
  PacketData sendData;
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_RULE_OTHER );
}

//------------------------------------------------------------------
/**
 * @brief  ルール選択者が自分になった通知送信
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendRuleSelectorIsMe()
{
  PacketData sendData;
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_SELECTOR_SELF );
}

//------------------------------------------------------------------
/**
 * @brief  ルール選択者が自分になった通知送信
 * @param[in] 送信するレギュレーションがプリセットならtrue。配信ならfalse。
 * @return  送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendRegulationReady( const bool isPreset )
{
  ICHI_PRINT("NBRNet::SendRegulationReady isPreset=%d\n", isPreset);
  PacketData sendData;
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );
  sendData.is_preset = static_cast<b8>(isPreset);
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_REGULATION_READY );
}

//------------------------------------------------------------------
/**
 * @brief  レギュレーション送信（プリセット版）
 * @param[in]   presetIndex          送信するプリセットレギュレーションの番号
 * @return  送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendRegulation( Regulation::PRESET presetIndex )
{
  PacketData sendData;
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );
  sendData.regulation_preset = static_cast<u8>(presetIndex);
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_REGULATION_P );
}

//------------------------------------------------------------------
/**
 * @brief  レギュレーション受信通知送信
 * @param[in]   regulation          送信するレギュレーション
 * @param[in]   pCerdData           署名データ
 * @param[in]   cerdDataSize        署名データサイズ
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendRegulation( Regulation& regulation,
                             const void* pCerdData, const u32 cerdDataSize )
{
  gfl2::std::MemClear( &m_sendRegulationPacket, sizeof(PacketDataForRegulation) );

  // レギュレーションシリアライズ
  regulation.Serialize( m_sendRegulationPacket.regulation_buffer );
  m_sendRegulationPacket.regulation_size = regulation.GetSerializeDataSize();
  // 署名コピー
  gfl2::std::MemCopy( pCerdData, m_sendRegulationPacket.cert_data, cerdDataSize );
  m_sendRegulationPacket.cert_data_size = cerdDataSize;

  // 分割送信クラス使う
  m_pP2pPacketTransfer->SendDataCommand( &m_sendRegulationPacket, sizeof(PacketDataForRegulation), NET_GAME_COMMAND_REGULATION_D );

  return true;
}

//------------------------------------------------------------------
/**
 * @brief     レギュレーション受信結果通知送信
 * @param[in] レギュレーション受信結果通知コマンド
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendRegulationRet( const NBRNetDefine::RegulationRetCommand command )
{
  ICHI_PRINT("NBRNet::SendRegulationRet command=%d\n", command);
  PacketData sendData;
  sendData.regulation_ret_command = static_cast<u8>(command);
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_REGULATION_RET );
}

//------------------------------------------------------------------
/**
 * @brief     勝負開始通知送信
 * @param[in] bgm     選択されたBGM（ローカルで親のみ設定）
 * @param[in] isQrTeam QRバトルチームかどうか
 * @return  送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendBattleStart( const u32 bgm, bool isQrTeam )
{
  PacketData sendData;
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );

  sendData.selectBgm = bgm;
  sendData.is_qr_team = isQrTeam;
  ICHI_PRINT("NBRNet::SendBattleStart bgm=%d isQrTeam=%d\n", bgm, isQrTeam);

  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_BATTLE_START );
}

//------------------------------------------------------------------
/**
 * @brief PokePartyデータ送受信開始
 * @param[in] pPokeParty    ポケパーティ
 * @param[in] pCerdData     署名データ
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendPokeParty( const pml::PokeParty* pPokeParty, const void* pCerdData )
{
  return GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->SendPokeParty( pPokeParty, pCerdData ); 
}

//------------------------------------------------------------------
/**
 * @brief PokeParty送受信終了したか
 * @return 終了していたらtrue
 */
//------------------------------------------------------------------
bool NBRNet::IsRecvPokePartyEnd()
{
  return GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsRecvPokePartyEnd();
}

//------------------------------------------------------------------
/**
 * @brief   ポケモン改ざんチェックの結果取得
 * @param[in] netId   通信ID（NetGame::GetNetID)
 * @return  改ざんが行われていないならtrueを返却する
 */
//------------------------------------------------------------------
bool NBRNet::IsValidationCheckOk( const int netId )
{
  return GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsValidationCheckOk( netId );
}

//------------------------------------------------------------------
/**
 * @brief  参加出来ない受信通知送信
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendCantParticipate()
{
  PacketData sendData;
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_CANT_PARTICIPATE );
}

//------------------------------------------------------------------
/**
 * @brief     ポケパーティ検査結果通知コマンド送信
 * @param[in] ポケパーティ検査結果通知コマンド
 * @return  送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendPokePartyRet( const NBRNetDefine::PokePartyRetCommand command[ NBRNetDefine::NET_NUM_MAX ] )
{
  PacketData sendData;
  for( int i=0; i<NBRNetDefine::NET_NUM_MAX; i++ ){
    sendData.pokeparty_ret_command[i] = static_cast<u8>(command[i]);
  }
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_POKEPARTY_RESULT );
}

//------------------------------------------------------------------
/**
 * @brief  マルチ振り分け決定通知送信
 * @param[in] NetIdが要素で戦う位置が格納されている
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendMultiDecide( const u8 place[ NBRNetDefine::NET_NUM_MAX ] )
{
  PacketData sendData;
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );
  gfl2::std::MemCopy( place, sendData.place, NBRNetDefine::NET_NUM_MAX );
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_MULTI_DECIDE );
}

//------------------------------------------------------------------
/**
 * @brief  マルチ振り分け通知送信
 * @param[in] netId   決めた人のNetId
 * @param[in] place   立ち位置（BTL_CLIETN_ID）
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendMultiSelect( int netId, const u8 place )
{
  PacketData sendData;
  gfl2::std::MemClear( &sendData, sizeof(PacketData) );
  sendData.multiValue[0] = static_cast<u8>(netId);
  sendData.multiValue[1] = place;
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_MULTI_SELECT );
}

//------------------------------------------------------------------
/**
 * @brief     続けて遊ぶ受信結果通知送信
 * @param[in] 続けて遊ぶ受信結果通知コマンド
 * @return 送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendContinue( const NBRNetDefine::ContinueCommand command )
{
  PacketData sendData;
  sendData.continue_command = static_cast<u8>(command);
  return SendPacket( &sendData, sizeof(PacketData), NET_GAME_COMMAND_CONTINUE );
}

//=============================================================================
/**
 *  受信
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief  受信結果通知リスナー設定
 */
//------------------------------------------------------------------
void NBRNet::RegistRecvListener( NBRNetRecvListener* pListener )
{
  GFL_PRINT("NBRNet::RegistRecvListener\n");
  GFL_PRINT("PollBufferSize %d \n", m_recvPoolBufferList.Size() );
  m_pRecvListener = pListener;
}

//------------------------------------------------------------------
/**
 * @brief  受信結果通知リスナー解除
 */
//------------------------------------------------------------------
void NBRNet::UnregistRecvListener()
{
  GFL_PRINT("NBRNet::UnregistRecvListener \n" );
  GFL_PRINT("PollBufferSize %d \n", m_recvPoolBufferList.Size() );
  m_pRecvListener = NULL;
}

// ------------------------------------------------------------------
/**
 * @brief  受信パケットを処理可能なコマンドかどうか
 * @return 可能ならtrue
 */
// ------------------------------------------------------------------
bool NBRNet::IsRecvPacketEnableCommand( const u8 command )
{
  for( int i=0; i<COMMAND_MAX; i++ )
  {
    if( m_enableCommand[i] == command )
    {
      return true;
    }
  }
  return false;
}

// ------------------------------------------------------------------
/**
 * @brief  受信パケットを処理可能なコマンド追加
 *         コレに登録しないとそのコマンドは処理されない
 */
// ------------------------------------------------------------------
void NBRNet::AddRecvPacketEnableCommand( const u8 command )
{
  GFL_PRINT("処理可能コマンド登録 command=%d \n", command);
  for( int i=0; i<COMMAND_MAX; i++ )
  {
    // 空いてる所に登録する
    if( m_enableCommand[i] == 0 )
    {
      m_enableCommand[i] = command;
      GFL_PRINT("登録成功 index=%d \n", i);
      return;
    }
  }
  GFL_ASSERT_MSG( 0,"clear 忘れ\n" );
}

// ------------------------------------------------------------------
/**
 * @brief  受信パケットを処理可能なコマンドをクリア
 */
// ------------------------------------------------------------------
void NBRNet::ClearRecvPacketEnableCommand()
{
  gfl2::std::MemClear( m_enableCommand, (sizeof(u8)*COMMAND_MAX) );
}

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief  パケットデータ送信処理
 * @param[in] packet      送信したいパケットデータ
 * @param[in] packetSize  送信したいパケットデータサイズ
 * @param[in] command     ユーザーコマンド
 * @return  送信準備完了したらtrue
 */
//------------------------------------------------------------------
bool NBRNet::SendPacket( void* packet, u32 packetSize, const u8 command )
{
#ifdef  GF_PLATFORM_CTR
  // コマンド付き送信
  if( m_pNetGame->SendDataCommand( reinterpret_cast<char *>(packet), packetSize, command ) )
  {
    ICHI_PRINT("NBRNet::SendPacket:コマンド %d \n", command );
    return true;
  }
  return false;
#else
  // 自分のデータは無視する
  if( IsIgnoreMyRecvData( command ) == true )
  {
    return true;
  }
  // Win版は通信動かないので受信した体で。
  RecvPacket( 0, 1, packet, command );
  return true;
#endif
}

//------------------------------------------------------------------
/**
 * @brief   受信更新関数
 */
//------------------------------------------------------------------
void NBRNet::RecvDataUpdate()
{
  RecvBuffer recvBuffer; // 受信バッファ
  gfl2::std::MemClear( &recvBuffer, sizeof(RecvBuffer) );

  // データがなくなるまで処理
  while( m_pNetGame->RecvDataCommand( &recvBuffer.id, reinterpret_cast<char *>(&recvBuffer.recv_data), PACKET_MAX_SIZE, &recvBuffer.command, &recvBuffer.size ) == true )
  {
    ICHI_PRINT("%d からデータ受信 コマンド %d サイズ %d\n", recvBuffer.id, recvBuffer.command, recvBuffer.size );

    // 受信パケットサイズチェック
    {
      if( recvBuffer.size != sizeof(PacketData) )
      {
        GFL_ASSERT_MSG(0,"recv packet size error %d %d\n", recvBuffer.size, sizeof(PacketData));
        break;
      }
    }

    // 自分のデータは無視する
    if( IsIgnoreMyRecvData( recvBuffer.command ) == true && m_pNetGame->GetNetID() == recvBuffer.id )
    {
      GFL_PRINT("自分のデータなので無視する\n");
      break;
    }

    // リスナー未設定ならばプールしておく
    // かつ 受信パケットを処理出来ないならプールしておく
    if( m_pRecvListener == NULL ||
        IsRecvPacketEnableCommand( recvBuffer.command ) == false )
    {
      if( m_recvPoolBufferList.Size() >= RECV_POOL_BUFFER_NUM )
      {
        GFL_ASSERT_MSG(0,"list max!");
        // 製品版で吹っ飛ばないようにケア
        ClearRecvPacketEnableCommand();
      }
      m_recvPoolBufferList.PushBack( recvBuffer );
      ICHI_PRINT("受信バッファ処理出来ないのでプールした\n");
    }
    // リスナー設定時は即時処理
    else
    { // パケットデータ処理
      RecvPacket( m_pNetGame->GetNetID(), recvBuffer.id, recvBuffer.recv_data, recvBuffer.command );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   プールしてあるデータ処理関数
 */
//------------------------------------------------------------------
void NBRNet::PoolBufferUpdate()
{
  // プールしてあるデータがあり、リスナーが設定されていたら処理するだけ
  if( m_recvPoolBufferList.Size() > 0 && m_pRecvListener != NULL )
  {
    for( gfl2::util::ListIterator<RecvBuffer> it=m_recvPoolBufferList.Begin(); it!=m_recvPoolBufferList.End(); )
    {
      RecvBuffer* pRecvBuffer = &(*it);
      // 処理出来るコマンドとして登録されていたら処理する
      if( IsRecvPacketEnableCommand( pRecvBuffer->command ) == true )
      {
        ICHI_PRINT("プールしていたバッファを処理 %d \n", pRecvBuffer->command );
        // パケットデータ処理
        RecvPacket( m_pNetGame->GetNetID(), pRecvBuffer->id, pRecvBuffer->recv_data, pRecvBuffer->command );
        // 要素を削除
        it = m_recvPoolBufferList.Erase( it );
      }
      else
      {
        it++;
      }
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  受信したパケットデータ処理
 * @param[in] myNetId 自分の通信ID
 * @param[in] recvNetId 受信相手の通信ID
 * @param[in] packet  受信したパケットデータ
 * @param[in] command ユーザーコマンド
 */
//------------------------------------------------------------------
void NBRNet::RecvPacket( const int myNetId, const int recvNetId, const void* recvBuf, const u8 command )
{
  // リスナー通知
  switch( command ){
  case NET_GAME_COMMAND_END:            //! 終了通知
    {
      m_pRecvListener->OnNBRNetRecvEnd();
    }
    break;
  case NET_GAME_COMMAND_PRIORITY:       //! ルール候補者の優先順位通知
    {
      // 親が送信・自分が子なら処理
      if( recvNetId == 0 && myNetId != 0 )
      {
        const PacketData* packet = reinterpret_cast<const PacketData *>(recvBuf);
        m_pRecvListener->OnNBRNetRecvPriority( packet->priority );
      }
    }
    break;
  case NET_GAME_COMMAND_RULE_SELECTOR:  //! ルール選択者通知
    {
      const PacketData* packet = reinterpret_cast<const PacketData *>(recvBuf);
      m_pRecvListener->OnNBRNetRecvRuleSelector( static_cast<NBRNetDefine::RuleSelectorCommand>(packet->rule_selector_command) );
    }
    break;
  case NET_GAME_COMMAND_RULE_OTHER:     //! ルール選択を他の人に任せる通知
    {
      m_pRecvListener->OnNBRNetRecvRuleOther();
    }
    break;
  case NET_GAME_COMMAND_SELECTOR_SELF:   //! ルール選択者が自分になった通知
    {
      m_pRecvListener->OnNBRNetRecvRuleSelectorIsMe( recvNetId );
    }
    break;
  case NET_GAME_COMMAND_REGULATION_READY:   //! レギュレーション送信準備
    {
      const PacketData* packet = reinterpret_cast<const PacketData *>(recvBuf);
      m_pRecvListener->OnNBRNetRecvRegulationReady( packet->is_preset );
    }
    break;
  case NET_GAME_COMMAND_REGULATION_P:     //! レギュレーション送信
    {
      const PacketData* packet = reinterpret_cast<const PacketData *>(recvBuf);
      m_pRecvListener->OnNBRNetRecvRegulation( packet->regulation_preset );
    }
    break;
  case NET_GAME_COMMAND_REGULATION_RET: //! レギュレーション受信結果送信
    {
      const PacketData* packet = reinterpret_cast<const PacketData *>(recvBuf);
      m_pRecvListener->OnNBRNetRecvRegulationRet( static_cast<NBRNetDefine::RegulationRetCommand>(packet->regulation_ret_command) );
    }
    break;
  case NET_GAME_COMMAND_BATTLE_START:   //! 勝負開始通知
    {
      const PacketData* packet = reinterpret_cast<const PacketData *>(recvBuf);
      m_pRecvListener->OnNBRNetRecvBattleStart( recvNetId, packet->selectBgm, packet->is_qr_team );
    }
    break;
  case NET_GAME_COMMAND_CANT_PARTICIPATE:   //! 参加出来ない通知
    {
      m_pRecvListener->OnNBRNetRecvCantParticipate();
    }
    break;
  case NET_GAME_COMMAND_POKEPARTY_RESULT:   //! ポケパーティ検査結果通知
    {
      const PacketData* packet = reinterpret_cast<const PacketData *>(recvBuf);
      m_pRecvListener->OnNBRNetRecvPokePartyCheckRet( packet->pokeparty_ret_command );
    }
    break;
  case NET_GAME_COMMAND_MULTI_DECIDE:   //! マルチ振り分け決定通知
    {
      const PacketData* packet = reinterpret_cast<const PacketData *>(recvBuf);
      m_pRecvListener->OnNBRNetRecvMultiDecide( packet->place );
    }
    break;
  case NET_GAME_COMMAND_MULTI_SELECT:   //! マルチ振り分け通知
    {
      const PacketData* packet = reinterpret_cast<const PacketData *>(recvBuf);
      m_pRecvListener->OnNBRNetRecvMultiSelect( packet->multiValue[0], packet->multiValue[1] );
    }
    break;
  case NET_GAME_COMMAND_CONTINUE:       //! 続けて遊ぶ通知
    {
      const PacketData* packet = reinterpret_cast<const PacketData *>(recvBuf);
      m_pRecvListener->OnNBRNetRecvContinue( static_cast<NBRNetDefine::ContinueCommand>(packet->continue_command) );
    }
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}

// ------------------------------------------------------------------
/**
 * @brief   自分の受信パケットを無視するコマンドかどうか
 * @return  trueで無視する
 */
// ------------------------------------------------------------------
bool NBRNet::IsIgnoreMyRecvData( const u8 command ) const
{
  // 自分のデータは無視するコマンド
  switch( command ){
  case NET_GAME_COMMAND_END:            //! 終了通知
  case NET_GAME_COMMAND_PRIORITY:       //! ルール候補者の優先順位通知
  case NET_GAME_COMMAND_RULE_SELECTOR:  //! ルール選択者通知
  case NET_GAME_COMMAND_RULE_OTHER:     //! ルール選択を他の人に任せる通知
  case NET_GAME_COMMAND_SELECTOR_SELF:   //! ルール選択者が自分になった通知
  case NET_GAME_COMMAND_REGULATION_READY: //! レギュレーション送信準備
  case NET_GAME_COMMAND_REGULATION_P:     //! レギュレーション送信
  case NET_GAME_COMMAND_REGULATION_D:     //! レギュレーション送信
  case NET_GAME_COMMAND_REGULATION_RET: //! レギュレーション受信結果送信
  case NET_GAME_COMMAND_CANT_PARTICIPATE:   //! 参加出来ない通知
  case NET_GAME_COMMAND_MULTI_DECIDE:   //! マルチ振り分け決定通知
  case NET_GAME_COMMAND_MULTI_SELECT:   //! マルチ振り分け通知
    return true;
  }

  //case NET_GAME_COMMAND_BATTLE_START:   //! 勝負開始通知
  //case NET_GAME_COMMAND_CONTINUE:       //! 続けて遊ぶ通知
  //case NET_GAME_COMMAND_POKEPARTY_RESULT:   //! ポケパーティ検査結果通知
  return false;
}

// ------------------------------------------------------------------
/**
 * @brief   パケット分割受信通知リスナー
 */
// ------------------------------------------------------------------
void NBRNet::OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer )
{
  const u32 receivedDataCount = pP2pPacketTransfer->GetReceivedDataCount();
  GFL_PRINT("NBRNet::OnPacketTransferSuccess : data count[%d]\n",receivedDataCount);

  PacketDataForRegulation* pRecvBuffer = GFL_NEW( m_pHeap->GetLowerHandle() ) PacketDataForRegulation;

  for( u32 i = 0; i < receivedDataCount; ++i )
  {
    int netID = 0;
    
    bool bResult = pP2pPacketTransfer->GetReceivedData( i, NET_GAME_COMMAND_REGULATION_D, netID, pRecvBuffer, sizeof(PacketDataForRegulation) );
    
    if( bResult )
    {
      // 自分のデータは無視する
      if( IsIgnoreMyRecvData( NET_GAME_COMMAND_REGULATION_D ) == true && m_pNetGame->GetNetID() == netID )
      {
        GFL_PRINT("自分のデータなので無視する\n");
        continue;
      }

      // 受信リスナーで通知
      // @fix GFNMCat[5780]：配信レギュレーション受信完了時に終了フレームにいっているとリスナーが解除されていてNULLアクセス
      if( m_pRecvListener )
      {
        m_pRecvListener->OnNBRNetRecvRegulation( netID,
                                               pRecvBuffer->regulation_buffer, pRecvBuffer->regulation_size,
                                               pRecvBuffer->cert_data, pRecvBuffer->cert_data_size );
      }
      else
      {
        GFL_RELEASE_PRINT("GFNMCat[5780]確認用\n");
      }
    }
  }

  GFL_SAFE_DELETE( pRecvBuffer );
}

GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
