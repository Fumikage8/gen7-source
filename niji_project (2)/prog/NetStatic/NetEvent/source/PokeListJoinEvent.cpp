// ============================================================================
/*
 * @file PokeListJoinEvent.cpp
 * @brief ポケリストを通信対戦で使用する参加選択モードで起動するイベント
 * @date 2015.08.28
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include <util/include/gfl2_std_string.h>
#include "NetStatic/NetEvent/include/PokeListJoinEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// プロセス終了待ちイベント
#include "Field/FieldStatic/include/Event/EventProcessCall.h"
// レギュレーション
#include "Battle/Regulation/include/Regulation.h"
// ポケモンリストProc
#include <App/PokeList/include/PokeListProc.h>
// ポケモンステータスProc
#include <App/Status/include/StatusProc.h>
// レギュレーション検査
#include "Battle/Regulation/include/PokeRegulation.h"
// 通信対戦イベント PersonalData用
#include "NetStatic/NetEvent/include/NetworkBattleEvent.h"

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(PokeListJoin)


//--------------------------------------------------------------
/**
 * @brief  通信対戦受付アプリ呼び出し
 * 
 * @param   pGameManager  ゲームマネージャ
 * @param   pEventParam   イベント呼び出しパラメータ構造体
 */
//--------------------------------------------------------------
void PokeListJoinEvent::StartEvent( GameSys::GameManager* pGameManager, EVENT_PARAM* pEventParam )
{
  PokeListJoinEvent* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, PokeListJoinEvent>( pGameManager->GetGameEventManager() );

  // 本イベントの呼び出しパラメータのポインタセット
  p_event->SetEventParam( pEventParam );
}


// コンストラクタ
PokeListJoinEvent::PokeListJoinEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_pEventParam(NULL)
 ,m_sequence(SEQ_NETGAME_INITIALIZE)
 ,m_callAppType()
 ,m_pokeListContext()
 ,m_statusAppParam()
 ,m_pTimeLimit(NULL)
 ,m_pNetGame(NULL)
 ,m_recvCount(0)
 ,m_isSendJoinOrder(false)
{
  GFL_PRINT("PokeListJoinEvent::PokeListJoinEvent \n");
}

// デストラクタ
PokeListJoinEvent::~PokeListJoinEvent()
{
  GFL_PRINT("PokeListJoinEvent::~PokeListJoinEvent \n");
}


//--------------------------------------------------------------------------
bool PokeListJoinEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


//--------------------------------------------------------------------------
void PokeListJoinEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  // ポケモンリストアプリ呼び出しパラメータの初期化
  InitializePokeListContext();
  // ポケモンステータスアプリ呼び出しパラメータの初期化
  InitializeStatusAppParam();
}


//--------------------------------------------------------------------------
void PokeListJoinEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  m_pNetGame->Finalize();
  GFL_SAFE_DELETE( m_pNetGame );
  GFL_SAFE_DELETE( m_pTimeLimit );
}


//--------------------------------------------------------------------------
GameSys::GMEVENT_RESULT PokeListJoinEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  // シーケンス制御
  switch( m_sequence ){
  case SEQ_NETGAME_INITIALIZE:       //! NetGame初期化
    {
      m_pNetGame = GFL_NEW( GetWorkHeap() ) gflnet2::p2p::NetGame(); 
      m_pNetGame->Initialize();
      m_sequence = SEQ_NETGAME_INITIALIZE_WAIT;
    }
    break;
  case SEQ_NETGAME_INITIALIZE_WAIT:  //! NetGame初期化待ち
    {
      // 通信エラーが発生していたら即終了
      if( IsError() )
      {
        m_sequence = SEQ_END;
      }
      else if( m_pNetGame->IsInitialize() )
      {
        // 初期呼び出しアプリはNBR
        SetCallApp( CALL_POKELIST );
      }
    }
    break;
  case SEQ_CALL_APP:   //! アプリ呼び出し
    CallApp( pGameManager );
    break;
  case SEQ_RUNNING_APP://! アプリ実行中
    if( RunningApp( pGameManager ) )
    {
      // アプリ終了結果
      ReturnApp();
    }
    break;
  case SEQ_END:        //! 終了
    return GameSys::GMEVENT_RES_FINISH;
  default:
    GFL_ASSERT(0);
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * @brief  呼び出すアプリ設定
 * @param  callAppType アプリ種類
 */
//--------------------------------------------------------------------------
void PokeListJoinEvent::SetCallApp( CallAppType callAppType )
{
  m_callAppType = callAppType;
  ICHI_PRINT("呼び出しアプリ %d \n", callAppType);

  m_sequence = SEQ_CALL_APP;
}

//--------------------------------------------------------------------------
/**
 * @brief  アプリ呼び出し

 */
//--------------------------------------------------------------------------
void PokeListJoinEvent::CallApp( GameSys::GameManager* pGameManager )
{
  switch( m_callAppType ){
  case CALL_POKELIST:          //! ポケモンリストアプリ呼び出し
    {
      App::PokeList::Proc* pPokeListProc = NIJI_PROC_SETNEXT< GameSys::GameProcManager, App::PokeList::Proc >( pGameManager->GetProcManager() );
      pPokeListProc->SetAppParam( pGameManager, &m_pokeListContext, NULL, &m_statusAppParam ); 
      
      // このイベントのMainFuncを回す必要があるので呼ばない
      //GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventProcessCall>( pGameManager->GetGameEventManager() );
    }
    break;

  case CALL_POKESTATUS:        //! ポケモンステータスアプリ呼び出し
    {
      App::Status::Proc* pStatusProc;
      pStatusProc = GameSys::CallStatusProc();
      pStatusProc->SetAppParam( &m_statusAppParam );

      // ポケモンステータスアプリは、通信エラーで終了させたくないので通常イベントでアプリ終了待つ
      GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventProcessCall>( pGameManager->GetGameEventManager() );
    }
    break;
  }

  // 終了待ち
  m_sequence = SEQ_RUNNING_APP;
}

//--------------------------------------------------------------------------
/**
 * @brief  アプリ実行中
 * @return trueでアプリ終了
 */
//--------------------------------------------------------------------------
bool PokeListJoinEvent::RunningApp( GameSys::GameManager* pGameManager )
{
  if( m_isSendJoinOrder == false )
  {
    // ポケモンリストで参加ポケモンを選択して決定したらtrueになる
    if( m_pokeListContext.pokelist_wait == true )
    {
      // この時点でアプリは終了していないが、
      // どのポケモンを選択したかの結果は格納されている

      // バトルに出すポケモンを送信
      if( SendJoinOrder( m_pokeListContext.join_order ) )
      {
        m_isSendJoinOrder = true; // 送信終了フラグを立てる
      }
    }
  }
  else
  {
    // @fix NMCat[4197]：バトルスポットからの参加選択画面で通信エラーーが発生した時に、通知されない不具合修正用
    // バトルスポットの時は、P2P切断を通知しない設定なので、個別にハンドリングする
    if( NetLib::NijiNetworkSystem::IsErrorDetectForBattleSpot() )
    {
      if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsSessionLeft() )
      {
        m_pokeListContext.list_break = true;
      }
    }

    // 自分が送信済みで、全員分受信していたらポケモンリストを終了させる
    if( IsRecvJoinOrderFinish() )
    {
      m_pokeListContext.list_break = true;
    }
  }

  // 受信更新
  RecvDataUpdate();

  // プロセスがなくなったら終了
  if( pGameManager->IsProcessExists() == false )
  {
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief  アプリ戻り
 */
//--------------------------------------------------------------------------
void PokeListJoinEvent::ReturnApp()
{
  // 通信エラーが発生していたら即終了
  if( IsError() )
  {
    m_sequence = SEQ_END;
    return;
  }


  switch( m_callAppType ){
  case CALL_POKELIST:          //! ポケモンリストアプリ呼び出し
    {
      // 終了モード
      switch( m_pokeListContext.end_mode ){
      case App::PokeList::END_MODE_CALL_STATUS:
        // ポケモンステータスへ
        SetCallApp( CALL_POKESTATUS );
        break;
      default:
        // 終了
        m_sequence = SEQ_END;
      }
    }
    break;

  case CALL_POKESTATUS:        //! ポケモンステータスアプリ呼び出し
    {
      m_pokeListContext.poke_index = m_statusAppParam.poke_index;
      // ポケモンリスト呼び出し
      SetCallApp( CALL_POKELIST );
    }
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief  ポケモンリストアプリ呼び出しパラメータの初期化
 */
//--------------------------------------------------------------------------
void PokeListJoinEvent::InitializePokeListContext()
{
  // レギュレーションによる補正
  {
    // レギュレーション検査クラス作成
    PokeRegulation *pokeReg;
    pokeReg = GFL_NEW_LOW( GetWorkHeap() ) PokeRegulation( GetWorkHeap() );

    // レギュレーションセット
    pokeReg->SetRegulation( m_pEventParam->pRegulation );

    // レベル補正
    pokeReg->ModifyLevelPokeParty( GetMyPersonalData()->pPokeParty );
    pokeReg->ModifyLevelPokeParty( GetEnemy1PersonalData()->pPokeParty );
    // ニックネーム補正
    pokeReg->ModifyNickName( GetMyPersonalData()->pPokeParty );
    pokeReg->ModifyNickName( GetEnemy1PersonalData()->pPokeParty );

    // 4人の場合
    if( GetFriendPersonalData() != NULL && GetEnemy2PersonalData() != NULL )
    {
      // レベル補正
      pokeReg->ModifyLevelPokeParty( GetFriendPersonalData()->pPokeParty );
      pokeReg->ModifyLevelPokeParty( GetEnemy2PersonalData()->pPokeParty );
      // ニックネーム補正
      pokeReg->ModifyNickName( GetFriendPersonalData()->pPokeParty );
      pokeReg->ModifyNickName( GetEnemy2PersonalData()->pPokeParty );
    }

    GFL_DELETE pokeReg;
  }

  m_pokeListContext.mode = App::PokeList::MODE_JOIN; // 参加選択モード
  m_pokeListContext.cancel_flag = false;    // キャンセル出来ない
  
  // レギュレーション渡す
  m_pokeListContext.regulation = m_pEventParam->pRegulation;

  // 上画面の表示モード
  {
    switch( m_pEventParam->pRegulation->GetBattleRule() ){
    case Regulation::BATTLE_RULE_SINGLE:
    case Regulation::BATTLE_RULE_DOUBLE:
      m_pokeListContext.join_upp_mode = App::PokeList::JOIN_UPP_MODE_SINGLE;
      break;
    case Regulation::BATTLE_RULE_ROYAL:
      m_pokeListContext.join_upp_mode = App::PokeList::JOIN_UPP_MODE_ROYALE;
      break;
    case Regulation::BATTLE_RULE_MULTI:
      // マルチはダブル
      m_pokeListContext.join_upp_mode = App::PokeList::JOIN_UPP_MODE_TAG;
      break;
    }
  }

  // 対戦ルールによる参加ポケモン数
  {
    u8 join_count_min = m_pEventParam->pRegulation->GetValueParam( Regulation::SELECT_NUM_LO );
    u8 join_count_max = m_pEventParam->pRegulation->GetValueParam( Regulation::SELECT_NUM_HI );

    m_pokeListContext.join_count_min = join_count_min; 
    m_pokeListContext.join_count_max = join_count_max; 

    ICHI_PRINT("参加ポケモン HI:%d LO:%d\n", join_count_max, join_count_min );
  }

  // 制限時間
  {
    u32 select_time = m_pEventParam->pRegulation->GetValueParam( Regulation::SHOW_POKE_TIME );
    m_pTimeLimit = GFL_NEW( GetWorkHeap() ) App::Tool::TimeLimit( select_time ); 
    m_pokeListContext.time_limit = m_pTimeLimit;
  }

  // memo:ロイヤルの時はマルチと同じ並びになる（左がからfriend,enemy1,enemy2）

  // 自分の情報
  m_pokeListContext.my_party.SetParameter( GetMyPersonalData()->pPokeParty );
  m_pokeListContext.my_status = GetMyPersonalData()->pMyStatus;
  m_pokeListContext.qrcord[ App::PokeList::PARTY_SELF ] = GetMyPersonalData()->qrTeam;

  // ポケモンの見せ合いOn/Off
  if( m_pEventParam->pRegulation->GetBoolParam( Regulation::SHOW_POKE ) )
  {// ONの場合

    // 相手の情報入れる
    m_pokeListContext.enemy_party_0.SetParameter( GetEnemy1PersonalData()->pPokeParty );
    m_pokeListContext.enemy_status_0 = GetEnemy1PersonalData()->pMyStatus;
    m_pokeListContext.qrcord[ App::PokeList::PARTY_ENEMY0 ] = GetEnemy1PersonalData()->qrTeam;

    // 4人の場合は味方と敵2も入れる
    if( GetFriendPersonalData() != NULL && GetEnemy2PersonalData() != NULL )
    {
      m_pokeListContext.friend_party.SetParameter( GetFriendPersonalData()->pPokeParty );
      m_pokeListContext.friend_status = GetFriendPersonalData()->pMyStatus;
      m_pokeListContext.qrcord[ App::PokeList::PARTY_FRIEND ] = GetFriendPersonalData()->qrTeam;
      m_pokeListContext.enemy_party_1.SetParameter( GetEnemy2PersonalData()->pPokeParty );
      m_pokeListContext.enemy_status_1 = GetEnemy2PersonalData()->pMyStatus;
      m_pokeListContext.qrcord[ App::PokeList::PARTY_ENEMY1 ] = GetEnemy2PersonalData()->qrTeam;
    }
  }
  else {
    // OFFの場合
    m_pokeListContext.join_upp_mode = App::PokeList::JOIN_UPP_MODE_NONE;  // @fix NMCat[1042]
  }

  // 参加ポケモンを決定した後の待機メッセージは
  // 通信同期で終了させる
  // その為、パラメータを通信側が実体をもちチェックして、
  // ポケモンリスト側ではそれをみるようにする
  //
  // ポケモンリスト側で参加決定した -> 通信Updateでチェック
  //m_pokeListContext.pokelist_wait = 
  // ポケモンリストを終了させる -> 通信側でフラグを立てる
  //m_pokeListContext.listBreak
}

//--------------------------------------------------------------------------
/**
 * @brief  ポケモンステータスアプリ呼び出しパラメータの初期化
 */
//--------------------------------------------------------------------------
void PokeListJoinEvent::InitializeStatusAppParam()
{
  //m_statusAppParam.mode = App::Status::MODE_BATTLE_BOX;
}


//--------------------------------------------------------------------------
/**
 * @brief  バトルのPersonalData取得
 * @param[in] btlClientId 取得したいデータの立ち位置
 */
//--------------------------------------------------------------------------
NetEvent::NetworkBattle::PersonalData*  PokeListJoinEvent::GetPersonalData( BTL_CLIENT_ID btlClientId ) const
{
  return m_pEventParam->pPersonalData[ btlClientId ];
}

//--------------------------------------------------------------------------
/**
 * @brief  自分のPersonalData取得
 */
//--------------------------------------------------------------------------
NetEvent::NetworkBattle::PersonalData*  PokeListJoinEvent::GetMyPersonalData() const
{
  return GetPersonalData( m_pEventParam->standingPos );
}
//--------------------------------------------------------------------------
/**
 * @brief  味方のPersonalData取得
 */
//--------------------------------------------------------------------------
NetEvent::NetworkBattle::PersonalData*  PokeListJoinEvent::GetFriendPersonalData() const
{
  BTL_CLIENT_ID friendId = BTL_CLIENT_PLAYER;  // 相手立ち位置

  // 自分の立ち位置から敵2を求める
  switch( m_pEventParam->standingPos ){
  case BTL_CLIENT_PLAYER:   friendId = BTL_CLIENT_PARTNER; break;
  case BTL_CLIENT_ENEMY1:   friendId = BTL_CLIENT_ENEMY2;  break;
  case BTL_CLIENT_PARTNER:  friendId = BTL_CLIENT_PLAYER;  break;
  case BTL_CLIENT_ENEMY2:   friendId = BTL_CLIENT_ENEMY1;  break;
  default: GFL_ASSERT(0);
  }

  return GetPersonalData( friendId );
}
//--------------------------------------------------------------------------
/**
 * @brief  敵1のPersonalData取得
 */
//--------------------------------------------------------------------------
NetEvent::NetworkBattle::PersonalData*  PokeListJoinEvent::GetEnemy1PersonalData() const
{
  BTL_CLIENT_ID enemy1 = BTL_CLIENT_PLAYER;  // 相手立ち位置

  // 自分の立ち位置から敵2を求める
  switch( m_pEventParam->standingPos ){
  case BTL_CLIENT_PLAYER:   enemy1 = BTL_CLIENT_ENEMY1; break;
  case BTL_CLIENT_ENEMY1:   enemy1 = BTL_CLIENT_PLAYER;  break;
  case BTL_CLIENT_PARTNER:  enemy1 = BTL_CLIENT_ENEMY2;  break;
  case BTL_CLIENT_ENEMY2:   enemy1 = BTL_CLIENT_PARTNER;  break;
  default: GFL_ASSERT(0);
  }

  return GetPersonalData( enemy1 );
}
//--------------------------------------------------------------------------
/**
 * @brief  敵2のPersonalData取得
 */
//--------------------------------------------------------------------------
NetEvent::NetworkBattle::PersonalData*  PokeListJoinEvent::GetEnemy2PersonalData() const
{
  BTL_CLIENT_ID enemy2 = BTL_CLIENT_PLAYER;  // 相手立ち位置

  // 自分の立ち位置から敵2を求める
  switch( m_pEventParam->standingPos ){
  case BTL_CLIENT_PLAYER:   enemy2 = BTL_CLIENT_ENEMY2; break;
  case BTL_CLIENT_ENEMY1:   enemy2 = BTL_CLIENT_PARTNER;  break;
  case BTL_CLIENT_PARTNER:  enemy2 = BTL_CLIENT_ENEMY1;  break;
  case BTL_CLIENT_ENEMY2:   enemy2 = BTL_CLIENT_PLAYER;  break;
  default: GFL_ASSERT(0);
  }

  return GetPersonalData( enemy2 );
}

//--------------------------------------------------------------------------
/**
 * @brief     バトル立ち位置から関係を取得
 * @param[in] 相手の立ち位置
 * @return    敵とか味方とかの関係性
 */
//--------------------------------------------------------------------------
App::PokeList::PartyID PokeListJoinEvent::GetPartyIdForBtlClientID( const BTL_CLIENT_ID btlClientId ) const
{
  // 自分の立ち位置によって変わる
  switch( m_pEventParam->standingPos ){
  //=================================
  case BTL_CLIENT_PLAYER:
    switch( btlClientId ){
    case BTL_CLIENT_PLAYER: return App::PokeList::PARTY_SELF; 
    case BTL_CLIENT_ENEMY1: return App::PokeList::PARTY_ENEMY0; 
    case BTL_CLIENT_PARTNER:return App::PokeList::PARTY_FRIEND;
    case BTL_CLIENT_ENEMY2: return App::PokeList::PARTY_ENEMY1;
    }
    break;
  //=================================
  case BTL_CLIENT_ENEMY1:
    switch( btlClientId ){
    case BTL_CLIENT_PLAYER: return App::PokeList::PARTY_ENEMY0; 
    case BTL_CLIENT_PARTNER:return App::PokeList::PARTY_ENEMY1;
    case BTL_CLIENT_ENEMY1: return App::PokeList::PARTY_SELF; 
    case BTL_CLIENT_ENEMY2: return App::PokeList::PARTY_FRIEND;
    }
    break;
  //=================================
  case BTL_CLIENT_PARTNER:
    switch( btlClientId ){
    case BTL_CLIENT_PARTNER:return App::PokeList::PARTY_SELF; 
    case BTL_CLIENT_PLAYER: return App::PokeList::PARTY_FRIEND; 
    case BTL_CLIENT_ENEMY1: return App::PokeList::PARTY_ENEMY1;
    case BTL_CLIENT_ENEMY2: return App::PokeList::PARTY_ENEMY0;
    }
    break;
  //=================================
  case BTL_CLIENT_ENEMY2:
    switch( btlClientId ){
    case BTL_CLIENT_ENEMY2: return App::PokeList::PARTY_SELF; 
    case BTL_CLIENT_PLAYER: return App::PokeList::PARTY_ENEMY1; 
    case BTL_CLIENT_PARTNER:return App::PokeList::PARTY_ENEMY0;
    case BTL_CLIENT_ENEMY1: return App::PokeList::PARTY_FRIEND;
    }
    break;
  }
  GFL_ASSERT(0);
  return App::PokeList::PARTY_ENEMY0;
}

//--------------------------------------------------------------------------
/**
 * @brief     参加ポケモンインデックス通知送信
 * @param[in] 参加ポケモンインデックス
 * @return  送信準備完了したらtrue
 */
//--------------------------------------------------------------------------
bool PokeListJoinEvent::SendJoinOrder( const s8 join_order[ pml::PokeParty::MAX_MEMBERS ] )
{
  PacketData sendData;
  gfl2::std::MemCopy( join_order, sendData.join_order, pml::PokeParty::MAX_MEMBERS );
  sendData.my_btl_client_id = m_pEventParam->standingPos;

  // コマンド付き送信
  if( m_pNetGame->SendDataCommand( reinterpret_cast<char *>(&sendData), sizeof(PacketData), NET_GAME_COMMAND_JOIN_ORDER ) )
  {
    ICHI_PRINT("PokeListJoinEvent::SendJoinOrder \n");
    return true;
  }
  return false;
}
//--------------------------------------------------------------------------
/**
 * @brief   全員のコマンド受信完了したか？
 * @return  trueで受信完了
 */
//--------------------------------------------------------------------------
bool PokeListJoinEvent::IsRecvJoinOrderFinish() const
{
  // 自分は受信したことでカウントする
  return ( m_pNetGame->GetConnectionCount() == m_recvCount );
}

//--------------------------------------------------------------------------
/**
 * @brief   受信更新関数
 */
//--------------------------------------------------------------------------
void PokeListJoinEvent::RecvDataUpdate()
{
  int         id;       // 通信ID
  PacketData  recv_data;// 受信データ
  u8          command;  // 受信コマンド
  u16         size = 0; // 受け取ったデータサイズ

  // データがなくなるまで処理
  while( m_pNetGame->RecvDataCommand( &id, reinterpret_cast<char *>(&recv_data), sizeof(PacketData), &command, &size ) == true )
  {
    ICHI_PRINT("%d からデータ受信 コマンド %d\n", id, command );

    // パケットデータ処理
    RecvPacket( m_pNetGame->GetNetID(), id, &recv_data, command );
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
void PokeListJoinEvent::RecvPacket( const int myNetId, const int recvNetId, const void* recvBuf, const u8 command )
{
  // リスナー通知
  switch( command ){
  case NET_GAME_COMMAND_JOIN_ORDER:
    {
      const PacketData* packet = reinterpret_cast<const PacketData *>(recvBuf);

      // 範囲外チェック
      if( packet->my_btl_client_id >= BTL_CLIENT_NUM )
      {
        GFL_ASSERT_MSG( 0, "PokeListJoinEvent::RecvPacket %d\n", packet->my_btl_client_id );
        return;
      }

      // 直接戻り値に入れる
      s8* outJoinOrder = m_pEventParam->join_order[ packet->my_btl_client_id ];
      gfl2::std::MemCopy( packet->join_order, outJoinOrder, pml::PokeParty::MAX_MEMBERS );

      GFL_PRINT("--------------------\n");
      GFL_PRINT("参加ポケモンインデックス受信結果\n");
      GFL_PRINT(">NetID %d \n", recvNetId );
      GFL_PRINT(">BTL_CLIENT_ID %d \n", packet->my_btl_client_id );
      GFL_PRINT(">ORDER %d %d %d %d %d %d \n", packet->join_order[0], packet->join_order[1], packet->join_order[2]
                                             , packet->join_order[3], packet->join_order[4], packet->join_order[5]);
      // 待機中に変更
      App::PokeList::PartyID partyId = GetPartyIdForBtlClientID( static_cast<BTL_CLIENT_ID>(packet->my_btl_client_id) );
      m_pokeListContext.selected_wait[ partyId ] = App::PokeList::JOIN_SELECT_WAITING;
      ICHI_PRINT( "partyId:%dを待機中に変更\n", partyId );

      // 受信カウントインクリメント
      m_recvCount++;
    }
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}


GFL_NAMESPACE_END(PokeListJoin)
GFL_NAMESPACE_END(NetEvent)

