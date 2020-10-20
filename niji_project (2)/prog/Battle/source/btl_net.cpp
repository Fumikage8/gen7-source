//=============================================================================================
/**
 * @file    btl_net.cpp
 * @brief   XY バトルシステム  通信処理管理クラス
 * @author  taya
 *
 * @date  2010.12.21  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#include "Savedata/include/MyStatus.h"

#include "./btl_common.h"
#include "./btl_server_const.h"
#include "./btl_net.h"

#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorListener.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkError.h"

// P2pConnectionMigrationListener
#include "NetStatic/NetLib/include/P2P/P2pConnectionMigrationListener.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include "gflnet2/include/p2p/gflnet2_NetGameMigrationListener.h"
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"

#include  <NetStatic/NetLib/include/Delivery/DeliveryManager.h>
#include  <System/include/HeapDefine.h>
#include "ExtSavedata/include/BattleRecorderSaveData.h"
#include  <System/include/ThreadPriority.h>
#include <NetStatic/NetLib/include/NijiNetworkSystem.h>
#include <NetStatic/NetEvent/include/BattleSpotEvent.h>

#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

#if defined(GF_PLATFORM_CTR)
#if PM_DEBUG
#include  <debug/include/gfl2_DebugLogWindow.h>
#endif
#endif

GFL_NAMESPACE_BEGIN(btl)
namespace net {

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  BTL_NET_CONNECT_MACHINE_MAX = 4,  ///< 同時接続するマシン台数最大

  NETID_NULL = 40,  ///< GFL/Net系のNetIDとしてあり得ない値

  PRINT_FLG = 0,
};

/**
 *  バトル用通信コマンドID
 */
enum Command {
  CMD_SERVER_VERSION = 0x01,
  CMD_NOTIFY_SERVER_PARAM,
  CMD_NOTIFY_PARTY_DATA,
  CMD_NOTIFY_AI_PARTY_DATA,
  CMD_NOTIFY_PLAYER_DATA,
  CMD_NOTIFY_AI_TRAINER_DATA,
  CMD_TO_CLIENT,
  CMD_TO_SERVER,
};

/**
 *  現在送受信中のデータ種類
 */
enum {
  DATAKIND_NULL = 0,
  DATAKIND_PARTY,
  DATAKIND_PLAYER,
  DATAKIND_TRAINER,
};

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/

//---------------------------------------------------
/**
 *  一時的送信バッファ
 */
//---------------------------------------------------
typedef struct {

  union {
    u32   val32;
    u16   val16[2];
    u8    val8[4];
  };

}TMP_SEND_BUFFER;


//---------------------------------------------------
/**
 *  サーババージョン相互通知処理用ワーク
 */
//---------------------------------------------------
class SVVER_WORK{

  struct {
    u8  clientID;
    u8  version;
    u8  recvedFlag;
  }recvTable[ BTL_NET_CONNECT_MACHINE_MAX ];

  u8  recvedCount;
  u8  maxVersion;
  u8  serverClientID;

  public:
  SVVER_WORK()
  {
    Initialize();
  }

  void Initialize( void )
  {
    recvedCount = 0;
    maxVersion = 0;
    serverClientID = BTL_CLIENTID_NULL;
    for(u32 i=0; i<GFL_NELEMS(recvTable); ++i){
      recvTable[i].recvedFlag = false;
    }
  }

  // １件追加
  u8 AddRecord( int netID, u8 clientID, u8 sv_ver )
  {
    if( (netID < BTL_NET_CONNECT_MACHINE_MAX)
    &&  (!recvTable[netID].recvedFlag)
    ){
      recvTable[netID].version    = sv_ver;
      recvTable[netID].clientID   = clientID;
      recvTable[netID].recvedFlag = true;
      BTL_PRINT("[BTLNET] NetID:%d = clientID:%d\n", netID, clientID);
      if( sv_ver > maxVersion ){
        maxVersion = sv_ver;
      }
      ++recvedCount;
    }
    return recvedCount;
  }
  // 件数取得
  u8  RecordCount( void ) const { return recvedCount; }
  // 最高バージョン取得
  u8  MaxVersion( void ) const { return maxVersion; }

  // サーバーになるClientIDを取得
  u8  GetServerClientID( void )
  {
    if( serverClientID == BTL_CLIENTID_NULL )
    {
      for(u32 i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
      {
        if( recvTable[i].recvedFlag && (maxVersion == recvTable[i].version) ){
          serverClientID = recvTable[i].clientID;
          GFL_PRINT("GetServerClientID : serverClientID[%d] netID[%d]\n",serverClientID,i);
          break;
        }
      }
    }
    return serverClientID;
  }

  bool CheckNewServerClientID( u8 checkID , bool* pConnectArray )
  {
    for(u32 i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
    {
      if( pConnectArray[i] && (maxVersion == recvTable[i].version) ){
        if( checkID == recvTable[i].clientID )
        {
          GFL_PRINT("CheckNewServerClientID : true\n");
          return true;
        }
        else
        {
          GFL_PRINT("CheckNewServerClientID : false\n");
          return false;
        }
      }
    }

    GFL_PRINT("CheckNewServerClientID : not exists\n");

    return false;
  }

  bool IsConnectNetID( int netID )
  {
    if( netID < GFL_NELEMS(recvTable) )
    {
      return recvTable[netID].recvedFlag;
    }
    return false;
  }

  u32 GetConnectPlayerNum( void )
  {
    u32 count = 0;
    for(u32 i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
    {
      if( recvTable[i].recvedFlag )
      {
        count++;
      }
    }

    return count;
  }

  // netID -> clientID
  u8 NetIDtoClientID( int netID )
  {
    if( netID < BTL_NET_CONNECT_MACHINE_MAX )
    {
      if( recvTable[netID].recvedFlag ){
//        BTL_PRINT("[BTLNET] netID(%d)->clientID(%d)\n", netID, recvTable[netID].clientID);
        return recvTable[netID].clientID;
      }
    }
    return BTL_CLIENTID_NULL;
  }

  int ClientIDtoNetID( u8 clientID )
  {
    for(u32 i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
    {
      if( recvTable[i].recvedFlag && (recvTable[i].clientID == clientID) ){

//        BTL_PRINT("[BTLNET] clientID(%d)->netID(%d)\n", clientID, i);

        return i;
      }
    }
    return NETID_NULL;
  }

  u8 ResetAndGetServerClientID(void)
  {
    serverClientID = BTL_CLIENTID_NULL;
    return GetServerClientID();
  }

  void SetDisconnectPlayer( int netID )
  {
    GFL_PRINT("SetDisconnectPlayer : netID[%d]\n",netID);
    if( netID >= 0 && netID < BTL_NET_CONNECT_MACHINE_MAX )
    {
      recvTable[netID].recvedFlag = false;
      GFL_PRINT("SetDisconnectPlayer : clientID[%d]\n",recvTable[netID].clientID);
    }
  }
};


/*--------------------------------------------------------------------------*/
/* AIData Container                                                         */
/*--------------------------------------------------------------------------*/

/**
 *  サーバマシンからAI用データ（パーティ、トレーナー）を送信するためのコンテナ
 */
struct _AIDCon{

  u32     clientID :  8;
  u32     dataSize : 24;
  u8      data[1];  //移植中に０だとワーニングが出たので１にしておく。

};

AIDCon*  AIDC_Create( u32 size, HeapHandle heap )
{
  AIDCon* container;

  container = reinterpret_cast<AIDCon*>( GflHeapAllocMemory(heap, size+sizeof(AIDCon)) );
  container->dataSize = size;
  container->clientID = BTL_CLIENTID_NULL;

  return container;
}
u32 AIDC_GetSendSize( const AIDCon* container )
{
  return sizeof(AIDCon) + container->dataSize;
}
void AIDC_Delete( AIDCon* container )
{
  GflHeapFreeMemory( container );
}
void AIDC_SetClientID( AIDCon* container, u8 clientID )
{
  container->clientID = clientID;
}
void* AIDC_GetBuffer( AIDCon* container )
{
  return container->data;
}

//シーケンス保存用ヘッダ
struct CMD_BUFFER_DATA_HEADER
{
  CMD_BUFFER_DATA_HEADER()
    : netID(NETID_NULL)
    , cmdSeqNo(0)
  {
  };

  u8 netID;
  u8 cmdSeqNo;
  u8 padding[2];
};

//シーケンス保存用バッファ
struct CMD_TMP_BUFFER
{
  CMD_TMP_BUFFER()
  {
    ::std::memset( dataBody , 0 , sizeof(dataBody) );
  };

  CMD_BUFFER_DATA_HEADER dataHeader;
  u8    dataBody[ BTL_SERVERCMD_QUE_SIZE ];
};

struct SYSWORK;//前方宣言

//---------------------------------------------------
/**
 *  通常受信用バッファ(RecvBufferの配列のIndexはNetIDではないので注意)
 */
//---------------------------------------------------
class RecvBuffer {

  public:
  u8    m_data[ BTL_SERVERCMD_QUE_SIZE ];
  u16   m_size;
  u16   m_cmdID;
  bool   m_bRecved;
  int   m_netID;
  u8    m_AIClientID;
  SYSWORK* Sys;

  public:
    RecvBuffer();

    void Initialize( SYSWORK* sys );

    // データ受信
    bool  Recv( GflNet* netHandle, Command cmd );

    // 受信したデータのポインタ取得
    const void* GetData( void ) const;

    // 受信したデータのサイズを取得
    u32  GetDataSize( void ) const;

    // 受信したデータの送信者 netID 取得
    int         GetNetID( void ) const ;

    // 受信したデータのAIClientIDを取得
    int         GetAIClientID( void ) const ;

    // 空のバッファか判定
    bool        IsEmpty( void ) const ;

    // バッファクリア（再受信可能に）
    void        Clear( void );

    //データヘッダを受信データから削除
    void RemoveDataHeader( bool isServer );

    //システムワークをセット
    void SetSysWork( SYSWORK* sys );
};



//通信イベント受信クラス
class BattleNetworkEventReceiver : public NetLib::Error::NijiNetworkErrorListener
#if defined(GF_PLATFORM_CTR)
  , public NetLib::P2P::P2pConnectionMigrationListener , public gflnet2::p2p::NetGameMigrationListener
#endif
{
private:
  enum
  {
    DISCONNECT_TIMING_NO = 54,
    SYNC_TIMEOUT = 1000 * 60,//タイムアウト10秒
  };

  enum
  {
    SYNC_SEQ_INIT = 0,
    SYNC_SEQ_REQUEST,
    SYNC_SEQ_WAIT,
  };

public:
  BattleNetworkEventReceiver()
    :m_isError(false)
    ,m_pSVVER_WORK(NULL)
    ,m_pSys(NULL)
    ,m_pSyncNetGame(NULL)
    ,m_syncSeq(0)
    ,m_disconnectCount(0)
    ,m_disconnectTimingCount(0)
    ,m_syncWaitCount(0)
    ,m_syncTimeoutStart(0)
    ,m_dummySendCount(0)
  {};
  virtual ~BattleNetworkEventReceiver()
  {
    Finalize();
  };

  void Initialize(void);


  bool IsInitialized(void)
  {
    if( m_pSyncNetGame == NULL )
    {
      return false;
    }

    return m_pSyncNetGame->IsInitialize();
  }

  void Finalize(void)
  {
    if( m_pSyncNetGame )
    {
      m_pSyncNetGame->Finalize();
      GFL_SAFE_DELETE( m_pSyncNetGame );
    }
  }

  bool IsError(void) const { return m_isError; };

  void SetSVVER_WORK( SVVER_WORK* work ){ m_pSVVER_WORK = work; }
  void SetSYSWORK( SYSWORK* sys ){ m_pSys = sys; }

  // NijiNetworkErrorListener
  //------------------------------------------------------------------
  /**
  * @brief 通信エラー通知
  *
  * @param error_level    エラーレベル
  * @param error_type     エラー種類
  */
  //------------------------------------------------------------------
  virtual void OnNijiNetworkError();

  //更新
  void Update( void );

#if defined(GF_PLATFORM_CTR)
  //------------------------------------------------------------------
  /**
   * @brief マイグレーション発生通知
   */
  //------------------------------------------------------------------
  virtual void OnP2PConnectionMigrationSuccess();

  //------------------------------------------------------------------
  /**
   * @brief 切断通知
   */
  //------------------------------------------------------------------
  virtual void OnNetGameDisconnectPlayer();
#endif
private:
  bool m_isError;
  SVVER_WORK* m_pSVVER_WORK;
  SYSWORK* m_pSys;
  GflNet* m_pSyncNetGame;
  u32 m_syncSeq;
  u32 m_disconnectCount;
  u32 m_disconnectTimingCount;
  u32 m_syncWaitCount;
  s64 m_syncTimeoutStart;
  u32 m_dummySendCount;
};





struct SYSWORK
{

  GflNet*     netHandle;
  HeapHandle  heapHandle;

  u8      myNetID;
  u8      serverNetID;
  u8      myClientID;
  u8      serverClientID;

  u16   netIDBit;
  u8    fServerParamReceived;
  u8    memberCount;
  u8    recvCount;
  u8    bServerCmdReceived;
  u8    clientDataReturned;
  u8    timingSyncStartFlag;
  u8    initializeSeq;
  u8    bPertySerialized;
  u8    bPlayerDataSerialized;
  u8    currentDataKind;
  u8    clientID[ BTL_NET_CONNECT_MACHINE_MAX ];
  u32   timingID;

  TRAINER_SEND_DATA  trainerSendBuf[ BTL_NET_CONNECT_MACHINE_MAX ];
  bool               trainerSendMark[ BTL_NET_CONNECT_MACHINE_MAX ];

  SERVER_NOTIFY_PARAM  serverNotifyParam;

  // 常駐受信バッファ
  RecvBuffer   recvServer;
  RecvBuffer   recvClient[ BTL_NET_CONNECT_MACHINE_MAX ];//このIndexはNetIDとは対応していないので注意

  // 一時受信バッファ
  void* tmpRecvBuf[ BTL_NET_CONNECT_MACHINE_MAX  ];
  u32   tmpLargeBufUsedSize[ BTL_NET_CONNECT_MACHINE_MAX ];

  void* AIPartyBuffer[ BTL_CLIENT_MAX ];

  void* tmpExBuffer;
  u32   tmpExBufferUsedSize;

  // 常駐送信バッファ
  TMP_SEND_BUFFER sendBuf;

  SVVER_WORK    svverWork;

  BattleNetworkEventReceiver networkEventReceiver;

  u8* partySerializeBuffer[BTL_CLIENT_MAX];
  bool serialBufferEnable[BTL_CLIENT_MAX];

  u8 totalCmdSeqNo;
  CMD_TMP_BUFFER* cmdTmpBufferClient;
  CMD_TMP_BUFFER* cmdTmpBufferServer;

  BATTLE_SETUP_PARAM* battleSetupParam;
};

RecvBuffer::RecvBuffer()
{
  Initialize( Sys );
}

void RecvBuffer::Initialize( SYSWORK* sys )
{
  m_bRecved = false;
  m_AIClientID = BTL_CLIENTID_NULL;
  m_netID = NETID_NULL;
  m_size = 0;
  m_cmdID = 0;

  SetSysWork( sys );
}

// データ受信
bool  RecvBuffer::Recv( GflNet* netHandle, Command cmd )
{
  if( !m_bRecved )
  {
//        BTL_PRINT(" cmd=%d, 未受信です...\n", cmd);
    if( netHandle->RecvDataCommandLocalCommand(&m_netID, (char*)m_data, sizeof(m_data), cmd, &m_size) )
    {
      m_cmdID = cmd;
      m_bRecved = true;
      GFL_PRINT("[BtlNet] NetID:%d の Command:%d 受信, size=%d\n", m_netID, m_cmdID, m_size);
      if( cmd == CMD_NOTIFY_AI_PARTY_DATA )
      {
        AIDCon* pCon = (AIDCon*)(m_data);
        m_AIClientID = pCon->clientID;
//            BTL_PRINT("  AIParty, ClientID=%d, buf=%p\n", m_AIClientID, AIDC_GetBuffer(pCon));
//            DebugPartySerialDump( "Net Recv", (u8*)(AIDC_GetBuffer(pCon)));
      }
      else if( cmd == CMD_NOTIFY_AI_TRAINER_DATA )
      {
        const TRAINER_SEND_DATA* pData = (const TRAINER_SEND_DATA*)(m_data);
        m_AIClientID = pData->clientID;
//            BTL_PRINT("  AITrainer, ClientID=%d\n", m_AIClientID);
      }
      else if( cmd == CMD_TO_CLIENT )
      {//シーケンス番号を保存
        GFL_ASSERT( Sys->cmdTmpBufferClient->dataHeader.netID == NETID_NULL ); //@fix NULLになっていないと上書きの恐れがある

        CMD_BUFFER_DATA_HEADER* pHeader = reinterpret_cast<CMD_BUFFER_DATA_HEADER*>( m_data );
        Sys->cmdTmpBufferClient->dataHeader.cmdSeqNo = pHeader->cmdSeqNo;
        Sys->cmdTmpBufferClient->dataHeader.netID = pHeader->netID;

        RemoveDataHeader( false );
      }
      else if( cmd == CMD_TO_SERVER )
      {//シーケンス番号をチェック
        CMD_BUFFER_DATA_HEADER* pHeader = reinterpret_cast<CMD_BUFFER_DATA_HEADER*>( m_data );
        if( pHeader->cmdSeqNo == Sys->totalCmdSeqNo &&
            pHeader->netID == Sys->myNetID )
        {//受信すべきデータ
          RemoveDataHeader( true );
        }
        else
        {//破棄すべきデータ
          GFL_PRINT("自分が発行していないコマンドへの返答なので破棄する seq[%d]netID[%d] != m_totalCmdSeqNo[%d]m_myNetID[%d]\n",pHeader->cmdSeqNo,pHeader->netID,Sys->totalCmdSeqNo,Sys->myNetID);
          Clear();
        }
      }
    }
  }
//      else{      BTL_PRINT(" cmd=%d, 受信済みです\n", cmd); }

  return m_bRecved;
}

// 受信したデータのポインタ取得
const void* RecvBuffer::GetData( void ) const
{
  if( m_bRecved ){
    return (const void*)m_data;
  }
  return NULL;
}

// 受信したデータのサイズを取得
u32  RecvBuffer::GetDataSize( void ) const
{
  if( m_bRecved ){
    return m_size;
  }
  return 0;
}

// 受信したデータの送信者 netID 取得
int         RecvBuffer::GetNetID( void ) const { return m_netID; }

// 受信したデータのAIClientIDを取得
int         RecvBuffer::GetAIClientID( void ) const { return m_AIClientID; }

// 空のバッファか判定
bool        RecvBuffer::IsEmpty( void ) const { return !m_bRecved; }

// バッファクリア（再受信可能に）
void        RecvBuffer::Clear( void )
{
  GFL_PRINT("ClearBuffer : netID[%d]\n",m_netID);
  m_bRecved = false;
  m_netID = NETID_NULL;
  m_AIClientID = BTL_CLIENTID_NULL;
}

//データヘッダを受信データから削除
void RecvBuffer::RemoveDataHeader( bool isServer )
{
  CMD_BUFFER_DATA_HEADER* pHeader = reinterpret_cast<CMD_BUFFER_DATA_HEADER*>( m_data );

  CMD_TMP_BUFFER* pCmdTmpBuffer = NULL;
  if( isServer )
  {
    pCmdTmpBuffer = Sys->cmdTmpBufferServer;
  }
  else
  {
    pCmdTmpBuffer = Sys->cmdTmpBufferClient;
  }

  m_size -= sizeof(pCmdTmpBuffer->dataHeader);
  ::std::memcpy( pCmdTmpBuffer->dataBody , &(pHeader[1]) , m_size );
  ::std::memset( m_data , 0 , sizeof(m_data) );
  ::std::memcpy( m_data , pCmdTmpBuffer->dataBody , m_size );
}

//システムワークをセット
void RecvBuffer::SetSysWork( SYSWORK* sys )
{
  Sys = sys;
}


//------------------------------------------------------------------
/**
* @brief 初期化
*/
//------------------------------------------------------------------
void BattleNetworkEventReceiver::Initialize( void )
{
  if( m_pSyncNetGame == NULL && m_pSys != NULL )
  {
    m_pSyncNetGame = GFL_NEW( m_pSys->heapHandle ) GflNet();
    m_pSyncNetGame->Initialize();
  }
}


//------------------------------------------------------------------
/**
* @brief 通信エラー通知
*
* @param error_level    エラーレベル
* @param error_type     エラー種類
*/
//------------------------------------------------------------------
void BattleNetworkEventReceiver::OnNijiNetworkError( void )
{
  GFL_PRINT("BattleNetworkEventReceiver::OnNijiNetworkError\n");
  m_isError = true; 
  m_pSys->battleSetupParam->isDisconnectOccur = true;//一応こっちでも設定
}

//更新
void BattleNetworkEventReceiver::Update( void )
{
#if defined(GF_PLATFORM_CTR)
  switch( m_syncSeq )
  {
  case SYNC_SEQ_INIT:
    if( m_disconnectTimingCount != m_disconnectCount )
    {//切断同期する
      m_syncSeq = SYNC_SEQ_REQUEST;
    }
  break;
  case SYNC_SEQ_REQUEST:
    if( m_syncWaitCount++ > 100 )
    {
      m_syncWaitCount = 0;
      m_syncSeq = SYNC_SEQ_WAIT;
      
      m_pSyncNetGame->TimingStart( DISCONNECT_TIMING_NO + m_disconnectTimingCount );

      //@fix [#4132 バトルロイヤルでバトル開始時の画面がホワイトアウトした直後に切断するとゲームが進行しなくなる　【ホストマイグレーション】]
      m_syncTimeoutStart = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
    }
  break;
  case SYNC_SEQ_WAIT:
    //@fix[#5721 インターネット通信の「バトルロイヤル」で通信を切断すると、高確率でホストマイグレーションに失敗する]
    if( m_dummySendCount != m_disconnectCount )
    {//切断毎に1回実行する
      m_pSyncNetGame->GetNetGameBase()->SetDummySend();
      m_dummySendCount++;
    }
    
    if( m_pSyncNetGame->IsTimingEnd( DISCONNECT_TIMING_NO + m_disconnectTimingCount ) )//切断後同期中
    {//切断後同期終了
      m_disconnectTimingCount++;
      m_syncSeq = SYNC_SEQ_INIT;
      //切断同期後の処理
      {
        for(u32 i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
        {
          if( m_pSys->netHandle->IsDisconnectNetID(i) )
          {//切断した人を登録
            m_pSVVER_WORK->SetDisconnectPlayer(i);
          }
        }

        m_pSys->serverClientID = m_pSVVER_WORK->ResetAndGetServerClientID();
        GFL_PRINT("OnP2PConnectionMigrationSuccess : new serverClientID[%d],myID[%d]\n",m_pSys->serverClientID , m_pSys->myClientID );
        if( m_pSys->serverClientID == BTL_CLIENTID_NULL )
        {//サーバになれる人がいない（サーババージョンが維持できない）
          m_isError = true;
          GFL_PRINT("OnP2PConnectionMigrationSuccess : server is not exists\n" );
        }
      }
    }
    else
    {
      //@fix [#4132 バトルロイヤルでバトル開始時の画面がホワイトアウトした直後に切断するとゲームが進行しなくなる　【ホストマイグレーション】]
      if( nn::os::Tick::GetSystemCurrent().ToMilliSeconds() - m_syncTimeoutStart > SYNC_TIMEOUT )
      {
        GFL_PRINT("BattleNetworkEventReceiver : sync time out\n");
        SetNetworkError();
      }
    }
  break;
  }
#endif
}

#if defined(GF_PLATFORM_CTR)
//------------------------------------------------------------------
/**
  * @brief マイグレーション発生通知(サーバから通知)
  */
//------------------------------------------------------------------
void BattleNetworkEventReceiver::OnP2PConnectionMigrationSuccess()
{
  //特にすること無し
}

//------------------------------------------------------------------
/**
  * @brief 切断発生通知（P2Pライブラリから通知）
  */
//------------------------------------------------------------------
void BattleNetworkEventReceiver::OnNetGameDisconnectPlayer()
{
  if( m_pSVVER_WORK && m_pSys )
  {
    //bool connectArray[BTL_NET_CONNECT_MACHINE_MAX];
    u32 aliveCount = 0;
    for(u32 i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
    {
      if( m_pSys->netHandle->IsDisconnectNetID(i) )
      {//切断
        //connectArray[i] = false;
        GFL_PRINT("%d : dead\n",i);
      }
      else
      {//切断していない
        //connectArray[i] = true;
        aliveCount++;
        GFL_PRINT("%d : alive\n",i);
      }
    }

    if( aliveCount <= 1 )
    {//他のプレイヤー全員切断している
      if( gflnet2::p2p::NetGame::GetNetGameBase() )
      {//必ず親になる
        gflnet2::p2p::NetGame::GetNetGameBase()->SetMaster(true);
      }
    }

    m_disconnectCount++;
    m_pSys->battleSetupParam->isDisconnectOccur = true;
  }
};



#endif
static SYSWORK  Sys = {};

//======================================================================================================
// 全ての受信用バッファをクリア（再利用可能にする）
void clearAllRecvData( void )
{
  for(u32 i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
  {
    Sys.recvClient[i].Clear();
  }
  Sys.recvServer.Clear();

  Sys.currentDataKind = DATAKIND_NULL;
  Sys.bServerCmdReceived = false;//受信データを削除したので受信フラグもfalseにする
}
// 全てのトレーナーデータ送信バッファをクリア（再利用可能にする）
void clearTrainerSendBuffer( void )
{
  for(u32 i=0; i<GFL_NELEMS(Sys.trainerSendMark); ++i)
  {
    Sys.trainerSendMark[ i ] = false;
  }
}
// トレーナーデータを、送信できる情報として構造体にセット
TRAINER_SEND_DATA* setupTrainerSendData( const BSP_TRAINER_DATA* trData, u8 clientID )
{
  if( !Sys.trainerSendMark[ clientID ] )
  {
    TRAINER_SEND_DATA* sendData = &(Sys.trainerSendBuf[ clientID ]);

    sendData->clientID = clientID;
    trData->Serialize( &sendData->base_data );
#if 0
    sendData->base_data = *trData;
    sendData->base_data.name = NULL;
    sendData->base_data.tr_type_name = NULL;
    sendData->trainer_name_length = trData->name->PutStr( sendData->trainer_name, GFL_NELEMS(sendData->trainer_name) );
#endif

    Sys.trainerSendMark[ clientID ] = true;
  }
  return &(Sys.trainerSendBuf[ clientID ]);
}

//======================================================================================================

/**
 *  システム初期化
 */
void InitSystem( u16 commNetIDBit, HeapHandle heapHandle , BATTLE_SETUP_PARAM* sp )
{
//  gfl2::std::MemClear( &Sys, sizeof(Sys) );

  
  Sys.heapHandle = heapHandle;
#if defined(GF_PLATFORM_CTR)
  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager) ) 
  {
    if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsConnectSuccess() )
    {
      Sys.netHandle  = GFL_NEW( Sys.heapHandle ) GflNet();
      Sys.memberCount = Sys.netHandle->GetConnectionCount();
    }
  }
#endif
  Sys.netIDBit   = commNetIDBit;
  Sys.timingSyncStartFlag   = false;
  Sys.initializeSeq         = 0;
  Sys.fServerParamReceived  = false;
  Sys.bPertySerialized      = false;
  Sys.bPlayerDataSerialized = false;
  Sys.bServerCmdReceived    = false;
  Sys.timingID = 0xffffffff;
  Sys.totalCmdSeqNo = 0;

  Sys.currentDataKind = DATAKIND_NULL;
  Sys.svverWork.Initialize();
  Sys.recvServer.Initialize( &Sys );
  for(u32 i=0; i<GFL_NELEMS(Sys.recvClient); ++i){
    Sys.recvClient[i].Initialize( &Sys );
    Sys.partySerializeBuffer[i] = GFL_NEW_ARRAY( Sys.heapHandle ) u8[pml::PokeParty::SERIALIZE_DATA_SIZE];
  }

  Sys.cmdTmpBufferClient = GFL_NEW( Sys.heapHandle ) CMD_TMP_BUFFER();
  Sys.cmdTmpBufferServer = GFL_NEW( Sys.heapHandle ) CMD_TMP_BUFFER();
  

  clearTrainerSendBuffer();

  

  if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) )
  {
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->RegistListener( &Sys.networkEventReceiver );
  }


  Sys.networkEventReceiver.SetSYSWORK( &Sys );
  Sys.networkEventReceiver.SetSVVER_WORK( &Sys.svverWork );

  //バトルセットアップパラムのパーティーデータを保存
  u32 enableCount = 0;
  for(int i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
  {
    if( sp->party[i] && sp->party[i]->GetMemberCount() > 0 )
    {
      enableCount++;
      sp->party[i]->SerializeToShortStruct( (Sys.partySerializeBuffer[i]) );//バトルセットアップパラムから取得する
      Sys.serialBufferEnable[i] = true;
    }
    else
    {
      Sys.serialBufferEnable[i] = false;
    }
  }

  Sys.battleSetupParam = sp;


#if PM_DEBUG
  if( enableCount == 1 )
  {//デバッグ通信バトル時の設
    GFL_PRINT("btl_net::ClearSerialBuffer\n");
    ::std::memset( Sys.serialBufferEnable , 0 , sizeof( Sys.serialBufferEnable ) );
  }
#endif
}

void InitializeListener( void )
{
#if defined(GF_PLATFORM_CTR)

  if( Sys.battleSetupParam->rule == BTL_RULE_ROYAL && NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
  {//Wifiのバトルロイヤルのみホストマイグレーションが有効になる
    // P2pConnectionManager
    if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager) ) 
    {
      GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->RegistMigrationListener( &Sys.networkEventReceiver );
    }

    gflnet2::p2p::NetGameBase::RegistMigrationListener( &Sys.networkEventReceiver );

#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    //この瞬間からホストマイグレーション有効になる
    gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"Host Migration Enable\n");
#endif
#endif

  }
#endif
}

void QuitSystem( void )
{
#if defined(GF_PLATFORM_CTR)
  gflnet2::p2p::NetGameBase::UnregistMigrationListener();

  // P2pConnectionManager
  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager) ) 
  {
    GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->UnregistMigrationListener();
  }
#endif
  if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) )
  {
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->UnregistListener();
  }

  if( Sys.netHandle )
  {
    Sys.netHandle->Finalize();
    GFL_SAFE_DELETE( Sys.netHandle );
  }

  Sys.networkEventReceiver.Finalize();

  for(u32 i=0; i<GFL_NELEMS(Sys.partySerializeBuffer); ++i)
  {
    GFL_SAFE_DELETE_ARRAY( Sys.partySerializeBuffer[i] );
  }

  GFL_SAFE_DELETE( Sys.cmdTmpBufferClient );
  GFL_SAFE_DELETE( Sys.cmdTmpBufferServer );
}
bool CheckError( void )
{
  return GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() || 
         Sys.networkEventReceiver.IsError() ||
         GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsSessionLeft();
}

void Initialize( void )
{
  if( Sys.netHandle )
  {
    Sys.netHandle->Initialize();
  }
}

bool IsInitialized( void )
{
  if( Sys.netHandle )
  {
    BTL_N_Printf( DBGSTR_line, __LINE__ );
    if( Sys.netHandle->IsInitialize() )
    {
      Sys.networkEventReceiver.Initialize();

      if( Sys.networkEventReceiver.IsInitialized() )
      {
        return true;
      }
    }

    return false;
  }

  return true;
}

// 自分のクライアントIDとサーババージョンを全マシンで相互に送受信
bool DetermineServer( u8 clientID )
{
  #if 1
  if( Sys.netHandle )
  {
    TMP_SEND_BUFFER* tsb = &Sys.sendBuf;
    tsb->val16[0] = clientID;
    tsb->val16[1] = BTL_NET_SERVER_VERSION;

    Sys.myClientID = clientID;
    BTL_PRINT("[BTLNET]自分の ClientID %d を送信\n", tsb->val16[0]);

    bool result = Sys.netHandle->SendDataCommand( (char*)tsb, sizeof(*tsb), CMD_SERVER_VERSION );
    if( !result ){
      BTL_N_Printf( DBGSTR_line, __LINE__ );
    }
    return result;
  }
  return true;
  #else
  GFL_UNUSED( clientID );
  return true;
  #endif
}
bool IsServerDetermained( void )
{
  #if 1
  if( Sys.netHandle )
  {
    if( Sys.svverWork.RecordCount() == Sys.memberCount ){
      return true;
    }

    RecvBuffer* recv = &( Sys.recvClient[Sys.myClientID] );
    bool  bDetermined = false;

    if( recv->Recv(Sys.netHandle, CMD_SERVER_VERSION) )
    {
      const TMP_SEND_BUFFER* tsb = (const TMP_SEND_BUFFER*)(recv->GetData());
      int netID = recv->GetNetID();

      Sys.clientID[ netID ] = static_cast<u8>(tsb->val16[0]);
      if( Sys.clientID[ netID ] == Sys.myClientID ){
        Sys.myNetID = netID;
      }
      BTL_PRINT("[BTLNET] NetID:%d -> ClientID:%d です\n", netID, Sys.clientID[netID]);

      if( Sys.svverWork.AddRecord(netID, static_cast<u8>(tsb->val16[0]), static_cast<u8>(tsb->val16[1])) == Sys.memberCount )
      {
        Sys.serverClientID = Sys.svverWork.GetServerClientID();
        BTL_PRINT("[BTLNET] %d台のバージョン送受信完了／サーバーは Client_%d に決まりました（自分は Client_%d , NetID=%d です）\n",
                Sys.memberCount, Sys.serverClientID, Sys.myClientID, Sys.myNetID);
        bDetermined = true;
      }
      recv->Clear();
    }
    return bDetermined;
  }
  return true;
  #else
  return true;
  #endif
}
// 自分がサーバかどうか判定
bool CheckImServer( void )
{
  #if 1
  return ( Sys.serverClientID == Sys.myClientID );
  #else
  return true;
  #endif
}

//指定したクライアントが切断されていないか確認(true:接続している,false:切断している)
bool CheckConnectedClient( u8 clientID )
{
  return ( Sys.svverWork.ClientIDtoNetID( clientID ) != NETID_NULL );
}

// サーバになるマシンのサーババージョンを取得
u8 GetServerVersion( void )
{
  if( Sys.netHandle )
  {
    return Sys.svverWork.MaxVersion();
  }
  return BTL_NET_SERVER_VERSION;
}

// 全クライアントにサーバパラメータを通知する（サーバからのみ呼び出し）
bool NotifyServerParam( const SERVER_NOTIFY_PARAM* sendParam )
{
  #if 1
  if( Sys.netHandle )
  {
    bool result = Sys.netHandle->SendDataCommand( (char*)sendParam, sizeof(SERVER_NOTIFY_PARAM), CMD_NOTIFY_SERVER_PARAM );

    if( !result ){
      BTL_PRINT("[btlnet] サーバーからコマンド %d おくったよ\n");
      BTL_N_Printf( DBGSTR_line, __LINE__ );
    }

    return result;
  }
  return true;
  #else
  GFL_UNUSED( sendParam );
  return true;
  #endif
}
// サーバパラメータ受信完了したか？
bool IsServerParamReceived( SERVER_NOTIFY_PARAM* dst )
{
  #if 1
  if( Sys.netHandle )
  {
    if( !Sys.fServerParamReceived )
    {
      int netID;
      u8 cmd;
      if( !(Sys.netHandle->RecvDataCommand(&netID, (char*)(&Sys.serverNotifyParam), sizeof(Sys.serverNotifyParam), &cmd)) )
      {
//        BTL_PRINT("サーバパラメータ受信まち…\n");
        return false;
      }
      if( cmd != CMD_NOTIFY_SERVER_PARAM ){
        BTL_PRINT("なんか違うコマンド(%d)送られてきた...\n", cmd);
        return false;
      }
      Sys.fServerParamReceived = true;
    }

    *dst = Sys.serverNotifyParam;
    return  true;
  }

  return true;
  #else
  GFL_UNUSED( dst );
  return true;
  #endif
}
//================================================================================================
//================================================================================================
/**
 *  同期開始
 */
void TimingSyncStart( TimingID timingID )
{
  #if 1
  if( Sys.netHandle )
  {
    Sys.timingID = timingID;
    Sys.timingSyncStartFlag = true;
    Sys.netHandle->TimingStart( timingID );
//    BTL_PRINT("タイミングシンクロスタート (timingID=%d)\n", timingID);
  }
  #else
  GFL_UNUSED( timingID );
  #endif
}
/**
 *  同期終了待ち
 */
bool IsTimingSync( TimingID timingID )
{
  #if 1
  if( Sys.netHandle )
  {
    if( Sys.timingSyncStartFlag )
    {
      bool is_sync = Sys.netHandle->IsTimingEnd( timingID );
      if( is_sync )
      {
        Sys.timingID = TIMING_NULL;
        Sys.timingSyncStartFlag = false;
        return true;
      }
      BTL_N_Printf( DBGSTR_line, __LINE__ );
    }
    else
    {
      // netIDBitを利用…?
//      GFL_NET_HANDLE_TimeSyncBitStart( Sys.netHandle, timingID, WB_NET_BATTLE_SERVICEID, Sys.netIDBit );
    }
    return false;
  }
  else{
    return true;
  }
  #else
  GFL_UNUSED( timingID );
  return true;
  #endif
}

/**
 *  指定クライアントから受信したデータバッファを返す
 */
RecvBuffer* GetClientRecvBuffer( u8 clientID )
{
  // 先にAIClientを探す
  for(int i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
  {
    RecvBuffer* recv = &( Sys.recvClient[i] );
    if( recv->GetAIClientID() == clientID ){
      return recv;
    }
  }

  // でなければプレイヤーClientなので netID から判定
  int netID = Sys.svverWork.ClientIDtoNetID( clientID );

  for(int i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
  {
    RecvBuffer* recv = &( Sys.recvClient[i] );
    if( recv->GetNetID() == netID ){
      return recv;
    }
  }

  GFL_PRINT("RecvedData from client_%d not exist : netID[%d]\n", clientID,netID);

  return NULL;
}

RecvBuffer* GetClientRecvBufferForAI( u8 clientID )
{
  for(int i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
  {
    RecvBuffer* recv = &( Sys.recvClient[i] );
    if( recv->GetAIClientID() == clientID ){
      return recv;
    }
  }

  return NULL;
}
RecvBuffer* GetEmptyRecvBuffer( void )
{
  for(int i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
  {
    RecvBuffer* recv = &( Sys.recvClient[i] );
    if( recv->IsEmpty() ){
      return recv;
    }
  }
  return NULL;
}


/*----------------------------------------------------------------------------------------------------*/
/* パーティデータ                                                                                     */
/*----------------------------------------------------------------------------------------------------*/

// パーティデータを連絡する（全マシン相互）
bool StartNotifyPartyData( const PokeParty* party )
{
  #if 1
  GFL_ASSERT((Sys.currentDataKind==DATAKIND_NULL)||(Sys.currentDataKind==DATAKIND_PARTY));

  if( Sys.netHandle )
  {
    u32 size = party->GetSerializeShortStructSize();
    if( size <= sizeof(Sys.recvServer.m_data) )
    {
      if( !(Sys.bPertySerialized) )
      {
        party->SerializeToShortStruct( (Sys.recvServer.m_data) );
        Sys.bPertySerialized = true;
      }

      bool result = Sys.netHandle->SendDataCommand( (char*)(Sys.recvServer.m_data), size, CMD_NOTIFY_PARTY_DATA );
      if( result ){
        Sys.currentDataKind = DATAKIND_PARTY;
      }
      else{
        BTL_N_Printf( DBGSTR_line, __LINE__ );
      }
      return result;
    }
    else
    {
      // 用意されたバッファサイズが小さすぎてパーティデータを送信できない
      GFL_ASSERT_MSG(0, "bufSize=%d, partySize=%d\n", sizeof(Sys.recvServer.m_data), size);
      return false;
    }
  }
  return true;

  #else
  GFL_UNUSED( party );
  return true;
  #endif
}
// パーティデータの相互受信が完了したか？
bool IsCompleteNotifyPartyData( void )
{
  #if 1
  if( Sys.currentDataKind != DATAKIND_PARTY ){
    GFL_ASSERT_MSG(0, "curDataKind=%d\n", Sys.currentDataKind);
    return false;
  }

  if( Sys.netHandle )
  {
    bool result = true;

    for(u32 i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
    {
      if( Sys.svverWork.IsConnectNetID(i) )
      {

        RecvBuffer* recv = &( Sys.recvClient[i] );
        if( !(recv->Recv(Sys.netHandle, CMD_NOTIFY_PARTY_DATA)) ){  // １件でも未受信のClientが居たら false
          result = false;
        }
      }
    }

    return result;
  }
  return true;
  #else
  return true;
  #endif
}
/**
 *  相互受信により受け取ったパーティデータのシリアライズデータを受け取る
 */
const void* GetPartySerializedData( u8 clientID , const BATTLE_SETUP_PARAM* sp )
{
  if( Sys.currentDataKind == DATAKIND_PARTY )
  {
    RecvBuffer* recv = GetClientRecvBuffer( clientID );
    if( recv )
    {
      if( recv->GetAIClientID() == BTL_CLIENTID_NULL )
      {
        if( Sys.serialBufferEnable[clientID] )
        {
          GFL_PRINT("btl_net:use serialize buffer[%d]\n",clientID);
          return Sys.partySerializeBuffer[clientID];
        }
        else
        {
#if PM_DEBUG
          if( !Sys.battleSetupParam->btlvDebugParam.isDebugBattle )
          {
            GFL_ASSERT(0);//デバッグで無いのに、ポケモンを渡していない
          }
#endif

          return recv->GetData();
        }
      }
      else
      {
        AIDCon* p_con = (AIDCon*)(recv->GetData());
        void* buf = AIDC_GetBuffer( p_con );
        {
          DebugPartySerialDump( "GetPartySerial", (u8*)buf);
        }
        return buf;
      }
    }
    //GFL_ASSERT(0);
    return NULL;
  }
  else{
    GFL_ASSERT(0);
    return NULL;
  }
}

/**
 *  パーティデータ相互受信を終了する
 */
void EndNotifyPartyData( void )
{
  Sys.currentDataKind = DATAKIND_NULL;
  clearAllRecvData();
}



/*----------------------------------------------------------------------------------------------------*/
/* プレイヤーデータ                                                                                   */
/*----------------------------------------------------------------------------------------------------*/

// プレイヤーデータを連絡する（全マシン相互）
bool  StartNotifyPlayerData( const Savedata::MyStatus* playerData )
{
  #if 1
  GFL_ASSERT((Sys.currentDataKind==DATAKIND_NULL)||(Sys.currentDataKind==DATAKIND_PLAYER));
  if( Sys.netHandle )
  {
    u32 size = playerData->GetSerializeDataSize();
    if( size <= sizeof(Sys.recvServer.m_data) )
    {
      if( !(Sys.bPlayerDataSerialized) )
      {
        playerData->Serialize( (void*)(Sys.recvServer.m_data) );
        Sys.bPlayerDataSerialized = true;
      }

      bool result = Sys.netHandle->SendDataCommand( (char*)(Sys.recvServer.m_data), size, CMD_NOTIFY_PLAYER_DATA );
      if(result){
        Sys.currentDataKind = DATAKIND_PLAYER;
      }
      else{
        BTL_N_Printf( DBGSTR_line, __LINE__ );
      }
      return result;
    }
  }
  return true;
  #else
  GFL_UNUSED( playerData );
  return true;
  #endif
}
#if 0
static u8* getbuf_playerData( int netID, void* pWork, int size )
{
  GFL_ASSERT(Sys->tmpRecvBuf[ netID ] == NULL);

  Sys->tmpRecvBuf[ netID ] = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(Sys->heapID), size );
  return Sys->tmpRecvBuf[ netID ];
}

static void recv_playerData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  Sys->tmpLargeBufUsedSize[ netID ] = size;
  BTL_Printf("netID=%dのプレイヤーデータ受信完了, pData=%p, buf=%p\n",
      netID, pData, Sys->tmpRecvBuf[netID] );
}
#endif
// プレイヤーデータの相互受信が完了したか？
bool  IsCompleteNotifyPlayerData( void )
{
  #if 1
  if( Sys.currentDataKind != DATAKIND_PLAYER ){
    GFL_ASSERT_MSG(0, "curDataKind=%d\n", Sys.currentDataKind);
    return false;
  }

  if( Sys.netHandle )
  {
    bool result = true;

    for(u32 i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
    {
      if( Sys.svverWork.IsConnectNetID(i) )
      {
        RecvBuffer* recv = &( Sys.recvClient[i] );
        if( !(recv->Recv(Sys.netHandle, CMD_NOTIFY_PLAYER_DATA)) ){  // １件でも未受信のClientが居たら false
          result = false;
        }
      }
    }

    return result;
  }
  return true;
  #else
  return true;
  #endif
}
// 受信したトレーナーデータを取得
const void* GetPlayerSerialData( u8 clientID )
{
  #if 1
  if( Sys.currentDataKind == DATAKIND_PLAYER )
  {
    RecvBuffer* recv = GetClientRecvBuffer( clientID );
    if( recv )
    {
      return recv->GetData();
    }
    else
    {
      return NULL;
    }
  }
  else{
    GFL_ASSERT(0);
    return NULL;
  }
  #else
  GFL_UNUSED( clientID );
  return NULL;
  #endif
}

/*----------------------------------------------------------------------------------------------------*/
/* AI用トレーナーデータ                                                                               */
/*----------------------------------------------------------------------------------------------------*/

// AI用トレーナーデータを連絡する（サーバ→全マシン）
bool StartNotify_AI_TrainerData( const BSP_TRAINER_DATA* tr_data, u8 clientID )
{
  GFL_ASSERT((Sys.currentDataKind==DATAKIND_NULL)||(Sys.currentDataKind==DATAKIND_PLAYER)||((Sys.currentDataKind==DATAKIND_TRAINER)));
  if( Sys.netHandle )
  {
    u32 size = sizeof(TRAINER_SEND_DATA);
    if( size <= sizeof(Sys.recvServer.m_data) )
    {
      TRAINER_SEND_DATA* trSendData = setupTrainerSendData( tr_data, clientID );
      bool result = Sys.netHandle->SendDataCommand( (char*)(trSendData), size, CMD_NOTIFY_AI_TRAINER_DATA );
      if(result){
        Sys.currentDataKind = DATAKIND_TRAINER;
      }
      else{
        BTL_N_Printf( DBGSTR_line, __LINE__ );
      }
      return result;
    }
    GFL_ASSERT(0);
    return false;
  }
  return true;
}
// AIトレーナーデータ受信完了したか
bool IsRecved_AI_TrainerData( u8 clientID )
{
  RecvBuffer* recBuf = GetClientRecvBufferForAI( clientID );
  if( recBuf )
  {
    return true;
  }
  recBuf = GetEmptyRecvBuffer();
  if( recBuf )
  {
    if( recBuf->Recv(Sys.netHandle, CMD_NOTIFY_AI_TRAINER_DATA) )
    {
      if( recBuf->GetAIClientID() == clientID ){
        return true;
      }
    }
    return false;
  }
  GFL_ASSERT(0);
  return false;
}

// 受信したAIトレーナーデータを取得
const TRAINER_SEND_DATA* Get_AI_TrainerData( u8 clientID )
{
  RecvBuffer* recBuf = GetClientRecvBufferForAI( clientID );
  if( recBuf )
  {
    return (const TRAINER_SEND_DATA*)(recBuf->GetData());
  }
  GFL_ASSERT(0);
  return NULL;
}

// プレイヤー・トレーナーデータ受信処理を完了する
void EndNotifyPlayerData( void )
{
  clearAllRecvData();
  clearTrainerSendBuffer();
}


/*----------------------------------------------------------------------------------------------------*/
/* 通信タッグ用AIのパーティデータ                                                                     */
/*----------------------------------------------------------------------------------------------------*/

// 通信タッグ用のAIパーティデータを送信（サーバ→全マシン）
bool StartNotify_AI_PartyData( const AIDCon* container )
{
  GFL_ASSERT((Sys.currentDataKind==DATAKIND_NULL)||(Sys.currentDataKind==DATAKIND_PARTY));

  if( Sys.netHandle )
  {
    u32 size = AIDC_GetSendSize( container );
    if( size <= sizeof(Sys.recvServer.m_data) )
    {
      bool result = Sys.netHandle->SendDataCommand( (char*)(const_cast<AIDCon*>(container)), size, CMD_NOTIFY_AI_PARTY_DATA );
      if( result ){
        Sys.currentDataKind = DATAKIND_PARTY;
        {
//          u8* pbuf = (u8*)(AIDC_GetBuffer( const_cast<AIDCon*>(container) ));
//          BTL_PRINT("AI clientID=%d party data  送信成功..size=%d\n", container->clientID, size);
//          DebugPartySerialDump( "send by net", pbuf );
        }
      }
      else{
        BTL_N_Printf( DBGSTR_line, __LINE__ );
      }
      return result;
    }
    else
    {
      // 用意されたバッファサイズが小さすぎてパーティデータを送信できない
      GFL_ASSERT_MSG(0, "bufSize=%d, partySize=%d\n", sizeof(Sys.recvServer.m_data), size);
      return false;
    }
  }
  return true;
}
// AIパーティデータを受信したかチェック
bool  IsRecved_AI_PartyData( u8 clientID )
{
  RecvBuffer* recBuf = GetClientRecvBufferForAI( clientID );
  if( recBuf )
  {
    return true;
  }
  recBuf = GetEmptyRecvBuffer();
  if( recBuf )
  {
    if( recBuf->Recv(Sys.netHandle, CMD_NOTIFY_AI_PARTY_DATA) )
    {
      if( recBuf->GetAIClientID() == clientID ){
        return true;
      }
    }
    return false;
  }
  GFL_ASSERT(0);
  return false;
}


/**
 *  受信済みデータバッファを全クリア
 */
void ClearRecvData( void )
{
  clearAllRecvData();
}



//=============================================================================================
/**
 * サーバから全ライアントにデータ送信
 *
 * @param   netID
 * @param   adrs
 * @param   size
 *
 */
//=============================================================================================
bool SendToClient( u8 clientID, const void* adrs, u32 size )
{
#if 1
  if( Sys.netHandle )
  {
    int netID = Sys.svverWork.ClientIDtoNetID( clientID );
    if( netID == NETID_NULL )
    {//切断済みの人には送信できたことにする
      return true;
    }

    //シーケンス番号をセット
    Sys.cmdTmpBufferServer->dataHeader.cmdSeqNo = Sys.totalCmdSeqNo;
    Sys.cmdTmpBufferServer->dataHeader.netID = Sys.myNetID;
    ::std::memset( Sys.cmdTmpBufferServer->dataBody , 0 , sizeof(Sys.cmdTmpBufferServer->dataBody) );
    ::std::memcpy( Sys.cmdTmpBufferServer->dataBody , (char*)adrs , size );

    bool result = Sys.netHandle->SendDataCommandSendBit( (char*)Sys.cmdTmpBufferServer, size+sizeof(Sys.cmdTmpBufferServer->dataHeader), CMD_TO_CLIENT, (1 << netID) );
    if( result )
    {
      //BTL_N_PrintfEx( PRINT_FLG, DBGSTR_NET_SendCmdDone, netID, size);
      GFL_PRINT("SendToClient::クライアント %d (=netID:%d)へコマンド送信しました。cmdSeq[%d]myNetID[%d]\n", clientID, netID,Sys.cmdTmpBufferServer->dataHeader.cmdSeqNo,Sys.cmdTmpBufferServer->dataHeader.netID);
    }
    return result;
  }
  else
  {
    return true;
  }
#else
  GFL_UNUSED( clientID );
  GFL_UNUSED( adrs );
  GFL_UNUSED( size );
  return true;
#endif
}

//=============================================================================================
/**
 * 全クライアントから返信があったかチェック
 *
 * @retval  BOOL    あったらtrue
 */
//=============================================================================================
bool CheckReturnFromClient( void )
{
#if 1
  u32 count = 0;
  for(u32 i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
  {
    RecvBuffer* recv = &( Sys.recvClient[i] );
//      BTL_PRINT("[BtlNet] netID:%d Cmd:%dについて..",i,CMD_TO_SERVER);
    if( recv->Recv(Sys.netHandle, CMD_TO_SERVER) )
    {
      if( Sys.svverWork.IsConnectNetID( recv->GetNetID() ) )
      {
        count++;
      }
    }
  }

#if PM_DEBUG
  //受信バッファ内の整合性チェック
  for(u32 i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
  {
    RecvBuffer* recv = &( Sys.recvClient[i] );
    for(u32 j=i+1; j<BTL_NET_CONNECT_MACHINE_MAX; j++)
    {
      RecvBuffer* recv2 = &( Sys.recvClient[j] );

      if( recv->GetNetID() != NETID_NULL )
      {
        GFL_ASSERT_MSG( recv->GetNetID() != recv2->GetNetID() ,
          "同一クライアントから連続でデータが届いている？netID[%d]clientID[%d]\n",
          recv->GetNetID(),Sys.svverWork.NetIDtoClientID(recv->GetNetID()));//@fix 通信バッファ内でNetIDが重複しているのはNG（同一クライアントから連続でデータが届いている可能性あり）
      }
    }
  }
#endif

  if( count >= Sys.svverWork.GetConnectPlayerNum() )
  {
    GFL_PRINT("[BtlNet]全クライアントから返信があった : count[%d]\n",count);
    Sys.totalCmdSeqNo++;//シーケンス番号を進める
    return true;
  }
  else
  {
    return false;
  }

#else
  return true;
#endif
}

u32 GetRecvClientData( u8 clientID, const void** pptr )
{
#if 1
  RecvBuffer* recv = GetClientRecvBuffer( clientID );
  if( recv )
  {
    *pptr = recv->GetData();
    return recv->GetDataSize();
  }
  else
  {
    return 0;
  }
#else
  GFL_UNUSED( clientID );
  GFL_UNUSED( pptr );
  return 10;
#endif
}


//---------------------------------

bool IsServerCmdReceived( void )
{
#if 1
  if( !(Sys.bServerCmdReceived) )
  {
//  BTL_PRINT("[BtlNet] Server受信バッファ Cmd:%dについて..",CMD_TO_CLIENT);
    Sys.bServerCmdReceived = Sys.recvServer.Recv( Sys.netHandle, CMD_TO_CLIENT );
    if( Sys.bServerCmdReceived ){
      BTL_PRINT("サーバーから受信\n");
    }
  }
  return Sys.bServerCmdReceived;
#else
  return true;
#endif
}

u32 GetReceivedCmdData( const void** ppDest )
{
#if 1
  GFL_ASSERT( Sys.bServerCmdReceived );

  RecvBuffer* recv = &(Sys.recvServer);

  *ppDest = recv->GetData();
  return recv->GetDataSize();

#else
//  GFL_UNUSED( ppDest );
  *ppDest = NULL;
  return 0;
#endif
}

bool ReturnToServer( const void* data, u32 size )
{
#if 1
  if( Sys.netHandle )
  {
    uint32_t serverNetID = Sys.svverWork.ClientIDtoNetID( Sys.serverClientID );
    if( serverNetID == NETID_NULL )
    {//ホストが切断済みだったら新しいホストが立つまで待たないといけない
      return false;
    }

    //シーケンス番号をセット
    ::std::memset( Sys.cmdTmpBufferClient->dataBody , 0 , sizeof(Sys.cmdTmpBufferClient->dataBody) );
    ::std::memcpy( Sys.cmdTmpBufferClient->dataBody , (char*)data , size );

    bool result = Sys.netHandle->SendDataCommandSendBit( (char*)Sys.cmdTmpBufferClient, size + sizeof(Sys.cmdTmpBufferServer->dataHeader), CMD_TO_SERVER, (1 << serverNetID) );
    if( result )
    {
      GFL_PRINT("ReturnToServer::サーバー (netID=%d, clientID=%d) へ返信, CmdID=%d, bServerCmdReceived = false : cmdSeq[%d]netID[%d]\n", 
                  serverNetID, Sys.serverClientID, CMD_TO_SERVER,Sys.cmdTmpBufferClient->dataHeader.cmdSeqNo,Sys.cmdTmpBufferClient->dataHeader.netID);
      Sys.recvServer.Clear();
      Sys.bServerCmdReceived = false;

      //シーケンスデータを使用済みにする
      Sys.cmdTmpBufferClient->dataHeader.netID = NETID_NULL;
    }
    else
    {
      BTL_PRINT("サーバー( netID=%d, clientID=%d )への返信できてない..\n", serverNetID, Sys.serverClientID);
      BTL_N_Printf( DBGSTR_line, __LINE__ );
    }
    return result;
  }
  return false;
#else
  GFL_UNUSED( data );
  GFL_UNUSED( size );
  return true;
#endif
}

void Update( void )
{
  Sys.networkEventReceiver.Update();
#if PM_DEBUG
  if( Sys.battleSetupParam->isLiveRecPlayMode )
  {//観戦モード
#if defined(GF_PLATFORM_CTR)
    GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager)->Update();
#endif
  }
#endif
}

//Wifiのときは不整合がおきたら強制的に通信エラーにする
void SetNetworkError( void )
{
#if defined(GF_PLATFORM_CTR)
  if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
  {
    GFL_PRINT("SetNetworkError : 不整合orタイムアウトエラー発生\n");
    if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) )
    {
      NetLib::Error::NijiNetworkError error;
      error.SetResult( NetLib::Error::NijiNetworkError::ERROR_TYPE_SESSION_LEFT );
      if( NetEvent::BattleSpot::BattleSpotEvent::IsEnterBattleSpot() == true )
      {//P2P切断はバトルスポットだとエラーにしない
        error.SetNotError();
      }
      
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
    }
  }
  else
  {
    GFL_PRINT("SetNetworkError : 不整合orタイムアウトエラーを無視\n");
  }
#endif
}

void ReloadRecSender( BTL_CLIENT* myClient )
{
  if( Sys.battleSetupParam->isLiveRecSendEnable )
  {
    const void* recData;
    u32 dataSize;

    recData = myClient->GetRecordData( &dataSize );
    if( recData )
    {
      gfl2::std::MemCopy( recData, Sys.battleSetupParam->recBuffer, dataSize );
      Sys.battleSetupParam->recDataSize = dataSize;
    }
    //@fix [#5742 ライブ大会：観戦機能にて観戦者と対戦者で乱数に差異が生じます]
    //if( Sys.battleSetupParam->recDataSize > 1 )
    {
      NetLib::NijiNetworkSystem::ReloadLiveRecSender();
    }
  }
}

void DebugPartySerialDump( const char* caption, u8* serial )
{
  #if 0
  BTL_PRINT("*** Serial Party Dump  : %s\n", caption);

  u32 serialSize = pml::PokeParty::GetSerializeDataSize();
  nn::util::Crc32  calculator;

  calculator.InitializeContext();
  calculator.Update( serial, serialSize );
  u32  hash = calculator.GetHash();
  BTL_PRINT("hash=%08x\n", hash);

  if( serialSize > 32 ){
    serialSize = 32;
  }
  for(u32 i=0; i<serialSize; ++i)
  {
    BTL_PRINT("%02x,", serial[i]);
    if( (i+1)%16==0 ){
      BTL_PRINT("\n");
    }
  }
  #endif
}





} // end of namespace 'net'
GFL_NAMESPACE_END(btl)

#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif
