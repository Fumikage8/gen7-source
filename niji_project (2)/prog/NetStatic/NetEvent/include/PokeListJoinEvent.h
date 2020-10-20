// ============================================================================
/*
 * @file PokeListJoinEvent.h
 * @brief ポケリストを通信対戦で使用する参加選択モードで起動するイベント
 * @date 2015.08.28
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( POKELISTJOIN_EVENT_H_INCLUDE )
#define POKELISTJOIN_EVENT_H_INCLUDE
#pragma once

#include <pml/include/pmlib.h>
#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameManager.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase
#include "AppLib/include/Tool/AppToolTimeLimit.h"    // for TimeLimit
#include "Battle/include/battle_SetupParam.h"        // for BTL_CLIENT_ID

// ポケモンリストアプリ
#include "App/PokeList/include/PokeListAppParam.h"
#include "App/PokeList/include/PokeListSetupParam.h"
// ポケモンステータスアプリ
#include "App/Status/include/StatusAppParam.h"

// 通信系
#include <gflnet2/include/p2p/gflnet2_P2pDefine.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>


// 前方宣言
class Regulation;

GFL_NAMESPACE_BEGIN( Savedata )
class MyStatus;
GFL_NAMESPACE_END( Savedata )

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(NetworkBattle)
struct PersonalData;
GFL_NAMESPACE_END(NetworkBattle)
GFL_NAMESPACE_END(NetEvent)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(PokeListJoin)

//-------------------------------------
/// イベント呼び出しパラメータ構造体
//=====================================
struct EVENT_PARAM{
  // [in]
  Regulation*                               pRegulation;                      //! [in]レギュレーション
  NetEvent::NetworkBattle::PersonalData*    pPersonalData[ BTL_CLIENT_NUM ];  //! [in]参加者データ
  BTL_CLIENT_ID                             standingPos;                      //! [in]戦う立ち位置

  // [out]
  // 参加ポケモンのインデックス配列
  // 参加するポケモンのパーティ内インデックスが参加順に格納されます
  // 参加するポケモンがいない場合, -1 が格納されます
  s8 join_order[ BTL_CLIENT_NUM ][ pml::PokeParty::MAX_MEMBERS ]; //!< [out]参加ポケモンのインデックス配列

  // コンストラクタ
  EVENT_PARAM(void) :
    pRegulation( NULL ),
    pPersonalData(),
    standingPos()
  {
    for( u8 i=0; i<BTL_CLIENT_NUM; i++ )
    {
      for( u8 j=0; j<pml::PokeParty::MAX_MEMBERS; j++ )
      {
        join_order[i][j] = -1;
      }
    }
  }
};

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------
/**
 *	@brief  通信対戦受付アプリ起動クラス
 */
//----------------------------------------------------------------------
class PokeListJoinEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListJoinEvent );

public:
  //--------------------------------------------------------------
  /**
   * @brief  通信対戦受付アプリ呼び出し
   * 
   * @param   pGameManager  ゲームマネージャ
   * @param   pEventParam   イベント呼び出しパラメータ構造体
   */
  //--------------------------------------------------------------
  static void StartEvent( GameSys::GameManager* pGameManager, EVENT_PARAM* pEventParam );

  // コンストラクタ
  PokeListJoinEvent( gfl2::heap::HeapBase* pHeap );
  // デストラクタ
  virtual~PokeListJoinEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

private:

  void SetEventParam( EVENT_PARAM* pEventParam ){ m_pEventParam = pEventParam; };

private:

  /**
   * @enum Sequence
   *       シーケンス定義
   */
  enum Sequence{
    SEQ_NETGAME_INITIALIZE,       //! NetGame初期化
    SEQ_NETGAME_INITIALIZE_WAIT,  //! NetGame初期化待ち
    SEQ_CALL_APP,                 //! アプリ呼び出し
    SEQ_RUNNING_APP,              //! アプリ実行中
    SEQ_END,                      //! 終了
  };

  /**
   * @enum CallAppType
   *       呼び出しアプリ種類
   */
  enum CallAppType{
    CALL_POKELIST,          //! ポケモンリストアプリ呼び出し
    CALL_POKESTATUS,        //! ポケモンステータスアプリ呼び出し
  };

  //! 呼び出すアプリ設定
  void SetCallApp( CallAppType callAppType );
  //! アプリ呼び出し
  void CallApp( GameSys::GameManager* pGameManager );
  //! アプリ実行中
  bool RunningApp( GameSys::GameManager* pGameManager ); 
  //! アプリ戻り
  void ReturnApp();


  //! ポケモンリストアプリ呼び出しパラメータの初期化
  void InitializePokeListContext();
  //! ポケモンステータスアプリ呼び出しパラメータの初期化
  void InitializeStatusAppParam();


  //! バトルのPersonalData
  NetEvent::NetworkBattle::PersonalData*  GetPersonalData( BTL_CLIENT_ID btlClientId ) const;
  NetEvent::NetworkBattle::PersonalData*  GetMyPersonalData() const;
  NetEvent::NetworkBattle::PersonalData*  GetFriendPersonalData() const;
  NetEvent::NetworkBattle::PersonalData*  GetEnemy1PersonalData() const;
  NetEvent::NetworkBattle::PersonalData*  GetEnemy2PersonalData() const;

  //! バトル立ち位置から、関係を取得
  App::PokeList::PartyID GetPartyIdForBtlClientID( const BTL_CLIENT_ID btlClientId ) const;


  //=========================================
  // 通信系
  //=========================================
  /**
   * @enum NetGameCommand
   * NetGameに渡す通信コマンド
   */
  enum NetGameCommand
  {
    NET_GAME_COMMAND_JOIN_ORDER       = 80,   //! 参加ポケモンインデックス通知
  };

  /**
   * @struct PacketData
   * @brief  通信データ構造体
   */
  struct PacketData
  {
    s8 join_order[ pml::PokeParty::MAX_MEMBERS ]; //! 参加ポケモンのインデックス
    u8 my_btl_client_id;  //! 自分のBTL_CLIENT_ID
  };

  /**
   * @brief     参加ポケモンインデックス通知送信
   * @param[in] 参加ポケモンインデックス
   * @return  送信準備完了したらtrue
   */
  bool SendJoinOrder( const s8 join_order[ pml::PokeParty::MAX_MEMBERS ] );

  //! 全員の受信完了したか？
  bool IsRecvJoinOrderFinish() const;

  //! 受信更新関数
  void RecvDataUpdate();

  /**
   * @brief  受信したパケットデータ処理
   * @param[in] myNetId 自分の通信ID
   * @param[in] recvNetId 受信相手の通信ID
   * @param[in] packet  受信したパケットデータ
   * @param[in] command ユーザーコマンド
   */
  void RecvPacket( const int myNetId, const int recvNetId, const void* recvBuf, const u8 command );


private:

  EVENT_PARAM*                                  m_pEventParam;            //! Event呼び出しパラメータ
  Sequence                                      m_sequence;               //! シーケンス

  // 呼び出しアプリ
  CallAppType                                   m_callAppType;            //! 呼び出しアプリ種類

  // アプリ呼び出しパラメータ
  App::PokeList::CONTEXT_PARAM                  m_pokeListContext;        //! ポケモンリスト呼び出し情報
  App::Status::APP_PARAM                        m_statusAppParam; 
  
  // 制限時間用
  App::Tool::TimeLimit*                         m_pTimeLimit;

  // 通信用
  gflnet2::p2p::NetGame*                        m_pNetGame;               //! 送受信管理クラス
  u8                                            m_recvCount;              //! 受信カウント
  // ポケモンリストの通信用
  bool                                          m_isSendJoinOrder;        //! 参加ポケモンを送信したかどうか
};


GFL_NAMESPACE_END(PokeListJoin)
GFL_NAMESPACE_END(NetEvent)

#endif // POKELISTJOIN_EVENT_H_INCLUDE
