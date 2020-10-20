#if !defined( __BTL_SERVER_H__ )
#define __BTL_SERVER_H__
//=============================================================================================
/**
 * @file  btl_server.h
 * @brief ポケモンXY バトルシステム サーバモジュール
 * @author  taya
 *
 * @date  2010.12.17  作成
 */
//=============================================================================================
#pragma once

#include <str/include/gfl2_StrBuf.h>

#include "./btl_common.h"
#include "./btl_server_TimeLimit.h"
#include "./btl_pokecon.h"
#include "./btl_Adapter.h"
#include "./btl_ServerFlow.h"
#include "./btl_rec.h"
#include "./btl_SVCL_ACTION.h"


GFL_NAMESPACE_BEGIN(btl)


class MainModule;
class Adapter;
class SendData;
class SendDataContainer;
class ServerCommandCalculator;
class AdapterCommandGenerator;

//--------------------------------------------------------------
/**
 *
 */
//--------------------------------------------------------------
class BTL_SERVER {
  GFL_FORBID_COPY_AND_ASSIGN( BTL_SERVER );

public:
  BTL_SERVER(
        MainModule*        mainModule,
        const u32          randSeed,
        POKECON*           pokeCon,
        SendDataContainer* sendDataContainer,
        HeapHandle         pHeap,
        u32                clientLimitTime );

  ~BTL_SERVER();

  ServerFlow*      GetFlowWork( void );
  const ServerFlow* GetFlowWorkConst( void ) const;
  void             AttachClient( u8 clientID, Adapter* adapter );
  void             Startup( void );
  void             StartupAsNewServer( void );
  bool             MainLoop( void );
  void             CmdCheckMode( u8 clientID );
  void             GetEscapeInfo( EscapeInfo* dst );
  SVCL_WORK*       GetClientWork( u8 clientID );
  bool             IsClientEnable( u8 clientID ) const;
  bool             IsWaitingClientReply( void ) const;
  void             InitChangePokemonReq( void );
  void             RequestChangePokemon( BtlPokePos pos );
  bool             IsPokemonChangeRequestExist( void ) const;
  u8               GetChangeRequestPokeCount( BTL_CLIENT_ID clientID ) const;
  void             AddBonusMoney( u32 volume );
  void             SetMoneyDblUp( MoneyDblUpCause cause );
  void             NotifyPokemonCapture( const BTL_POKEPARAM* bpp );
  void             NotifyPokemonLevelUp( const BTL_POKEPARAM* bpp );

  void             CMDCHECK_RestoreActionData( const void* recData, u32 recDataSize );
  void             CMDCHECK_RestoreClientLimitTime( const SEND_DATA_CLIENT_LIMIT_TIME_SYNC& syncData );
  bool             CMDCHECK_Make( rec::Timing timingCode, const void* recvedCmd, u32 cmdSize );

private:

  // シーケンス制御関数型
  typedef ServerSequence ( BTL_SERVER::*SequenceFunc )( int* );

  ServerSequence            m_currentSequence;
  int                       m_seqStep;

  MainModule*               m_mainModule;
  POKECON*                  m_pokeCon;
  ServerFlow*               m_flowWork;
  SCQUE                     m_que;

  SVCL_WORK                 m_client[ BTL_CLIENT_MAX ];
  rec::SendTool             m_recTool;

  SendDataContainer*        m_sendDataContainer;  // 送信データの記録
  ServerCommandCalculator*  m_serverCommandCalculator;
  AdapterCommandGenerator*  m_adapterCommandGenerator;
  SendDataSerialNumber      m_adapterCommandSerialNumber; // アダプタコマンドの通し番号

  const EscapeInfo*         m_escapeInfo;
  BTL_RESULT_CONTEXT        m_btlResultContext;
  SVCL_ACTION               m_clientAction;
  TimeLimit                 m_timeLimit;         // 制限時間情報
  u8                        m_enemyPutPokeID;
  u8                        m_changePokePos[ BTL_POS_NUM ];
  u8                        m_changePokeCnt;
  STRBUF*                   m_strbuf;


  AdapterCommandGenerator* createAdapterCommandGenerator( HeapHandle heap );


  // シーケンス制御：戦闘開始
  ServerSequence serverMain_BATTLE_START_SETUP( int* seqStep );
  ServerSequence serverMain_BATTLE_START_SWITCH( int* seqStep );
  ServerSequence serverMain_BATTLE_START_TIMING( int* seqStep );
  ServerSequence serverMain_BATTLE_START_COMMAND( int* seqStep );
  ServerSequence serverMain_BATTLE_START_RECORD( int* seqStep );
  // シーケンス制御：行動選択
  ServerSequence serverMain_ACTION_SELECT_START( int* seqStep );
  ServerSequence serverMain_ACTION_SELECT_SWITCH( int* seqStep );
  ServerSequence serverMain_ACTION_SELECT_RECORD( int* seqStep );
  ServerSequence serverMain_ACTION_SELECT_COMMAND( int* seqStep );
  ServerSequence serverMain_ACTION_SELECT_COMMAND_SWITCH( int* seqStep );
  // シーケンス制御：逃げる、次を出す　選択
  ServerSequence serverMain_CHANGE_OR_ESCAPE_START( int* seqStep );
  ServerSequence serverMain_CHANGE_OR_ESCAPE_SWITCH( int* seqStep );
  ServerSequence serverMain_CHANGE_OR_ESCAPE_ESCAPE_SELECTED( int* seqStep );
  // シーケンス制御：カバーポケモン投入
  ServerSequence serverMain_COVER_SELECT_START( int* seqStep );
  ServerSequence serverMain_COVER_COMFIRM_PLAYER_POKECHANGE( int* seqStep );
  ServerSequence serverMain_COVER_SELECT_SWITCH( int* seqStep );
  ServerSequence serverMain_COVER_SELECT_RECORD( int* seqStep );
  ServerSequence serverMain_COVER_COMMAND( int* seqStep );
  ServerSequence serverMain_COVER_COMMAND_SWITCH( int* seqStep );
  // シーケンス制御：ターン途中のポケモン入れ替え
  ServerSequence serverMain_INTERVAL_POKECHANGE_POKESELECT( int* seqStep );
  ServerSequence serverMain_INTERVAL_POKECHANGE_POKESELECT_SWITCH( int* seqStep );
  ServerSequence serverMain_INTERVAL_POKECHANGE_RECORD( int* seqStep );
  ServerSequence serverMain_INTERVAL_POKECHANGE_COMMAND( int* seqStep );
  ServerSequence serverMain_INTERVAL_POKECHANGE_SWITCH( int* seqStep );
  // シーケンス制御：乱入
  ServerSequence serverMain_INTRUDE_PREPARE( int* seqStep );
  ServerSequence serverMain_INTRUDE_COMMAND( int* seqStep );
  // シーケンス制御：制限時間終了
  ServerSequence serverMain_TIMEUP_RECORD( int* seqStep );
  ServerSequence serverMain_TIMEUP_COMMAND( int* seqStep );
  // シーケンス制御：戦闘終了
  ServerSequence serverMain_EXIT_COMMON( int* seqStep );
  ServerSequence serverMain_EXIT_SWITCH( int* seqStep );
  ServerSequence serverMain_EXIT_NPC( int* seqStep );
  ServerSequence serverMain_EXIT_BATTLE_SPOT_TRAINER( int* seqStep );
  ServerSequence serverMain_EXIT_COMM( int* seqStep );
  ServerSequence serverMain_EXIT_WILD_WIN( int* seqStep );  
  ServerSequence serverMain_EXIT_WILD_LOSE( int* seqStep );
  ServerSequence serverMain_EXIT_WILD_CAPTURE( int* seqStep );
  ServerSequence serverMain_EXIT_KEY_WAIT( int* seqStep );
  ServerSequence serverMain_EXIT_FADEOUT( int* seqStep );
  ServerSequence serverMain_EXIT_QUIT( int* seqStep );
  // シーケンス制御：新サーバーとしてのスタートアップ
  ServerSequence serverMain_NEW_STARTUP_SEND_LATEST_REPLY( int* seqStep );
  ServerSequence serverMain_NEW_SEND_SWITCH( int* seqStep );


  void           initAllAdapter( void );

  void           Svcl_Clear( SVCL_WORK* clwk );
  bool           Svcl_IsEnable( const SVCL_WORK* clwk ) const;
  void           Svcl_Setup( SVCL_WORK* clwk, u8 clientID, Adapter* adapter );
  void           Svcl_SetParty( SVCL_WORK* client, BTL_PARTY* party );

  void           changeSequence( ServerSequence nextSeq );
  bool           callMainProc( void );
  bool           isPokeCoverEscapeMode( void );

  void           sendRequest( ServerRequest request );
  void           sendRequestAlone( ServerRequest request, u8 clientID );
  void           sendRequestCore( ServerRequest request, u8 clientID );
  void           sendAdapterCommand( AdapterCommand cmd, const void* sendData, u32 dataSize );
  void           sendAdapterCommandAlone( AdapterCommand cmd, u8 clientID, const void* sendData, u32 dataSize );
  void           storeSendDataInAllAdapter( SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand cmd, const void* sendData, u32 dataSize );
  void           storeSendDataInSingleAdapter( SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand cmd, const void* sendData, u32 dataSize, u8 clientId );
  void           registerSendData( SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand cmd, const void* sendData, u32 dataSize );
  bool           waitAllAdapterReply( void );
  void           resetAdapterCmd( void );
  BtlCompetitor  checkCompetitorExistType( void );

  void*          makeSelectActionRecord( rec::Timing timingCode, bool fChapter, u32* dataSize );
  void           storeClientAction( void );
  void           storeClientLimitTime( void );
  bool           isClientLimitTimeOver( void ) const;

  bool           irekae_IsNeedConfirm( void );
  u8             irekae_GetEnemyPutPokeID( void );

#if PM_DEBUG
  void           print_client_action( const SVCL_ACTION* clientAction ) const;
  void           print_que_info( SCQUE* que, const char* caption );
#endif // PM_DEBUG
  bool           checkPokeChangeReqClient( u8 clientID );

};
GFL_NAMESPACE_END(btl)
#endif // __BTL_SERVER_H__
