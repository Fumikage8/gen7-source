#if !defined( __BTL_NET_H__ )
#define __BTL_NET_H__
//=============================================================================================
/**
 * @file    btl_net.h
 * @brief   XY バトルシステム  通信処理管理クラス
 * @author  taya
 *
 * @date  2010.12.21  作成
 */
//=============================================================================================
#pragma once

#include "./btl_common.h"
#include <Battle/include/battle_SetupParam.h>
#include <debug/include/gfl2_assert.h>

#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include "./btl_client.h"


GFL_NAMESPACE_BEGIN(btl)
namespace net {

/**
 *  定数
 */
enum {
  COMM_TRAINERNAME_MAX = 32,    ///< 通信で送受信可能なトレーナー名文字数
};

/**
 *  サーバーになったマシンが各マシンに対し、
 *  統一してもらうパラメータを送るのに使用する
 */
typedef struct {

  u32                  randomSeed;
  u16                  debugFlagBit;
  u16                  LimitTimeGame;
  u16                  LimitTimeClient;
  u16                  LimitTimeCommand;
  u8                   msgSpeed;
  u8                   fWazaEffectEnable : 4;
  u8                   fCommTag          : 4; ///< 通信タッグモード

}SERVER_NOTIFY_PARAM;

//---------------------------------------------------
/**
 *  トレーナー通信用データ
 */
//---------------------------------------------------
typedef struct {
  BSP_TRAINER_DATA::SERIALIZE_DATA  base_data;

  // トレーナー名実データ
  STRCODE           trainer_name[COMM_TRAINERNAME_MAX];
  u16               trainer_name_length;
  // ClientID
  u8                clientID;
} TRAINER_SEND_DATA;


/**
 *  同期タイミングID
 */
typedef enum {

  TIMING_NULL = 0,    ///< 無効値

  TIMING_INITIALIZE,
  TIMING_NOTIFY_PARTY_DATA,
  TIMING_NOTIFY_PERAPP_VOICE,
  TIMING_NOTIFY_PLAYER_DATA,
  TIMING_SETUP_END,

  TIMING_NOTIFY_AI_PARTY_1,
  TIMING_NOTIFY_AI_PARTY_2,
  TIMING_NOTIFY_AI_PARTY_3,
  TIMING_NOTIFY_AI_TRAINER_1,
  TIMING_NOTIFY_AI_TRAINER_2,

  TIMING_MAX,

}TimingID;


/**
 *  サーバマシンからAI用データ（パーティ、トレーナー）を送信するためのコンテナ型定義
 */
typedef struct _AIDCon  AIDCon;

AIDCon*  AIDC_Create( u32 size, HeapHandle heap );
void     AIDC_Delete( AIDCon* container );
void*    AIDC_GetBuffer( AIDCon* container );
void     AIDC_SetClientID( AIDCon* container, u8 clientID );




extern void InitSystem( u16 commNetIDBit, HeapHandle heapHandle , BATTLE_SETUP_PARAM* sp );
extern void QuitSystem( void );

extern bool CheckError( void );
extern void Initialize( void );
extern void InitializeListener( void );
extern bool IsInitialized( void );

extern bool DetermineServer( u8 clientID );
extern bool IsServerDetermained( void );
extern bool CheckImServer( void );
extern bool CheckConnectedClient( u8 clientID );//指定したクライアントが切断されていないか確認(true:接続している,false:切断している)
extern u8   GetServerVersion( void );
extern bool NotifyServerParam( const SERVER_NOTIFY_PARAM* sendParam );
extern bool IsServerParamReceived( SERVER_NOTIFY_PARAM* dst );

extern void TimingSyncStart( TimingID timingID );
extern bool IsTimingSync( TimingID timingID );

extern bool StartNotifyPartyData( const PokeParty* party );
extern bool IsCompleteNotifyPartyData( void );

extern bool StartNotify_AI_PartyData( const AIDCon* container );
extern bool IsRecved_AI_PartyData( u8 clientID );

extern const void* GetPartySerializedData( u8 clientID , const BATTLE_SETUP_PARAM* sp );
extern void  EndNotifyPartyData( void );

extern bool  StartNotifyPlayerData( const Savedata::MyStatus* playerData );
extern bool  IsCompleteNotifyPlayerData( void );
extern const void* GetPlayerSerialData( u8 clientID );

extern bool StartNotify_AI_TrainerData( const BSP_TRAINER_DATA* tr_data, u8 clientID );
extern bool IsRecved_AI_TrainerData( u8 clientID );
extern const TRAINER_SEND_DATA*  Get_AI_TrainerData( u8 clientID );
extern void EndNotifyPlayerData( void );


extern void ClearRecvData( void );
extern bool SendToClient( u8 clientID, const void* adrs, u32 size );
extern bool CheckReturnFromClient( void );
extern u32  GetRecvClientData( u8 clientID, const void** pptr );


extern bool IsServerCmdReceived( void );
extern u32  GetReceivedCmdData( const void** ppDest );
extern bool ReturnToServer( const void* data, u32 size );

extern void Update( void );
extern void SetNetworkError( void );
extern void ReloadRecSender( BTL_CLIENT* myClient );

inline TimingID TimingID_Add( TimingID startID, u32 addValue )
{
  GFL_ASSERT( startID != TIMING_NULL );
  GFL_ASSERT( startID < TIMING_MAX );

  startID = static_cast<TimingID>( startID + addValue );

  GFL_ASSERT( startID < TIMING_MAX );
  return startID;
}

void DebugPartySerialDump( const char* caption, u8* serial );


} // end of namespace 'net'
GFL_NAMESPACE_END(btl)



#endif // __BTL_NET_H__
