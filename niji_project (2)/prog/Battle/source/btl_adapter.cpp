//=============================================================================================
/**
 * @file  btl_adapter.cpp
 * @brief ポケモンXY バトルシステム サーバ-クライアント間アダプター
 * @author  taya
 *
 * @date  2010.12.22  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#include "btl_common.h"
#include "btl_net.h"

#include "btl_adapter.h"

#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

GFL_NAMESPACE_BEGIN(btl)

enum {
  ADAPTER_PRINT_FLG = true,
};

/*============================================================================================*/
/*                                                                                            */
/* Adapter                                                                                    */
/*                                                                                            */
/*============================================================================================*/
AdapterFactory* AdapterFactory::s_pSingleton = NULL;
BtlCommMode Adapter::s_commMode = BTL_COMM_NONE;


/**
 *  Constructor
 */
Adapter::Adapter( u8 clientID, bool fCommType )
 :
    m_processingCmd( ACMD_NONE ),
    m_myID( clientID ),
    m_myState( AS_FREE ),
    m_fRetDataPrepared( false ),
    m_fCommType( fCommType )
{
}
/**
 *  Destructor
 */
Adapter::~Adapter()
{
}






/*============================================================================================*/
/*                                                                                            */
/* AdapterFactory                                                                             */
/*                                                                                            */
/*============================================================================================*/

AdapterFactory*  AdapterFactory::GetInstance( void )
{
  return s_pSingleton;
}

void   AdapterFactory::InitSystem( BtlCommMode commMode, gfl2::heap::HeapBase* pHeap )
{
  if( s_pSingleton == NULL )
  {
    s_pSingleton = GFL_NEW(pHeap) AdapterFactory();

    Adapter::s_commMode = commMode;

    for(u32 i=0; i<GFL_NELEMS(s_pSingleton->m_set); ++i)
    {
      s_pSingleton->m_set[ i ] = NULL;
    }
  }
}
void  AdapterFactory::QuitSystem( void )
{
  if( s_pSingleton )
  {
    for(u32 i=0; i<GFL_NELEMS(s_pSingleton->m_set); ++i)
    {
      if( s_pSingleton->m_set[i] ){
        GFL_DELETE  s_pSingleton->m_set[i];
        s_pSingleton->m_set[i] = NULL;
      }
    }
    GFL_DELETE( s_pSingleton );
    s_pSingleton = NULL;
  }
}

//=============================================================================================
/**
 *
 *
 * @param   netHandle
 * @param   clientID
 * @param   fCommType
 * @param   heapHandle
 *
 * @retval  Adapter*
 */
//=============================================================================================
Adapter*  AdapterFactory::Create( BtlCommMode commMode, u8 clientID, HeapHandle heapHandle )
{
  GFL_ASSERT_STOP(clientID<BTL_CLIENT_MAX);

  Adapter* wk = NULL;
  bool     fComm = (commMode != BTL_COMM_NONE);

  if( m_set[clientID] == NULL )
  {
    wk = GFL_NEW(heapHandle) Adapter( clientID, fComm );
    BTL_N_PrintfEx( ADAPTER_PRINT_FLG, DBGSTR_ADAPTER_Create, clientID, commMode );
  }

  BTL_PRINT("[AdapterFactory] Create CommMode:%d for Client_%d\n", fComm, clientID);

  // 同一IDは１つだけ作ってサーバ・クライアント双方から使い回し
  if( m_set[clientID] == NULL )
  {
    m_set[clientID] = wk;
  }

  return m_set[ clientID ];
}




//--------------------------------------------------------------------------------------
/**
 *
 */
//--------------------------------------------------------------------------------------
void Adapter::Init( void )
{
  m_myState = AS_FREE;
  m_processingCmd = ACMD_NONE;
  m_fRetDataPrepared = false;
}

//--------------------------------------------------------------------------------------
/**
 * @brief アダプタの通信タイプを変更する
 * @param fCommType  設定する通信タイプ
 */
//--------------------------------------------------------------------------------------
void Adapter::ChangeCommType( bool fCommType )
{
  m_fCommType = fCommType;
}

//--------------------------------------------------------------------------------------
/**
 * @brief クライアントからの返信待ち状態か？
 * @retval true   返信待ち状態である
 * @retval false  返信待ち状態でない
 */
//--------------------------------------------------------------------------------------
bool Adapter::IsWaitingClientReply( void ) const
{
  if( m_myState == AS_WAIT_RECV_FROM_CLIENT ) 
  {
    return true;
  }

  return false;
}

//--------------------------------------------------------------------------------------
/**
 * [SERVER用] BTL_ADAPTER_SetCmd の前に呼び出し
 */
//--------------------------------------------------------------------------------------
void Adapter::BeginSetCmd( void )
{
  if( s_commMode != BTL_COMM_NONE )
  {
    net::ClearRecvData();
  }
}

//--------------------------------------------------------------------------------------
/**
 * [SERVER用] BTL_ADAPTER_SetCmd の後に呼び出し
 */
//--------------------------------------------------------------------------------------
void Adapter::EndSetCmd( void )
{
  if( s_commMode != BTL_COMM_NONE )
  {

  }
}
//--------------------------------------------------------------------------------------
/**
 * [SERVER用] クライアントに対するコマンド送信を開始する
 *
 * @param serialNumber  送信データの通し番号
 * @param serverSeq     送信時のサーバーシーケンス
 * @param cmd           送信するコマンド
 * @param sendData      送信データ本体
 * @param sendDataSize  送信データ本体のサイズ
 *
 */
//--------------------------------------------------------------------------------------
void Adapter::SetCmd( SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand cmd, const void* sendData, u32 sendDataSize )
{
  GFL_ASSERT(m_myState == AS_FREE); // @check

  m_myState = AS_CMD_RECEIVED;
  m_processingCmd = cmd;

  m_sendData.Store( serialNumber, serverSeq, static_cast<AdapterCommand>(cmd), sendData, sendDataSize );
  BTL_PRINT( "[Adapter_%d] コマンドをバッファにセット( serverSeq=%d, cmd=%d, sendDataSize=%d )\n", m_myID, serverSeq, cmd, sendDataSize );

  m_returnData.Clear();
  m_fRetDataPrepared = false;

  BTL_N_PrintfEx( ADAPTER_PRINT_FLG, DBGSTR_ADAPTER_SEND_START, m_myID, m_processingCmd );
}
//--------------------------------------------------------------------------------------
/**
 * [SERVER用] 送信したコマンドに対するクライアントからの返信を待つ
 *
 * @retval  bool    クライアントからの返信を受け取ったらtrue
 */
//--------------------------------------------------------------------------------------
bool Adapter::WaitCmd( void )
{
  switch( m_myState ){
  case AS_CMD_RECEIVED:
    BTL_PRINT("[Adapter_%d]コマンド送信トライ\n", m_myID);
    if( !startToReception() ){
      break;
    }
    m_myState = AS_WAIT_RECV_FROM_CLIENT;
    /* fallthru */
  case AS_WAIT_RECV_FROM_CLIENT:
    if( !receptionClient() )
    {
      //BTL_PRINT("[Adapter_%d]コマンド終了待ちへ移行できん\n", m_myID);
      break;
    }
    BTL_N_PrintfEx(ADAPTER_PRINT_FLG, DBGSTR_ADAPTER_RECV_DONE );
    BTL_PRINT("[Adapter_%d]コマンド終了待ちへ移行する\n", m_myID);
    m_myState = AS_DONE;
    /*fallthru*/;
  case AS_DONE:
    //BTL_PRINT("[Adapter_%d]コマンド終了待ち...myStateAdrs=%p\n", m_myID, &m_myState);
    return true;
  default:
    BTL_PRINT("[Adapter_%d] 不正な状態: %d\n", m_myID, m_myState);
      GFL_ASSERT(0);
  }
  return false;
}
//--------------------------------------------------------------------------------------
/**
 * [SERVER用] クライアントから帰ってきたデータへのポインタを取得
 *
 * @param   wk        アダプタモジュール
 * @param   size      [out] データサイズを取得する変数ポインタ（不要ならNULL）
 *
 * @retval  const void*   データポインタ
 */
//--------------------------------------------------------------------------------------
const SendData& Adapter::GetReturnData( void ) const
{
  GFL_ASSERT(m_fRetDataPrepared); // @check
  return m_returnData;
}

//--------------------------------------------------------------------------------------
/**
 * [SERVER用] クライアントに送信するコマンドをリセットする
 */
//--------------------------------------------------------------------------------------
void Adapter::ResetCmd( void )
{
  GFL_ASSERT(m_myState == AS_FREE || m_myState == AS_DONE); // @check

  m_myState = AS_FREE;

//
//  if( CommMode != BTL_COMM_NONE )
  if( m_fCommType )
  {
    net::ClearRecvData();
  }

}


//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------

bool Adapter::startToReception( void )
{
  // スタンドアロンなら処理不要
  if( m_fCommType )
  {
    // WBでは特定クライアントにのみ送信することが可能だったが、XYでは全通信対象に送信してしまう（2011.12.12 現在）
    // WB同様の状態に近づけるため、ClientID=0 と接続しているAdapterのみ、送信を行うようにしてみる
//    if( m_myID == 0 )
    {
      const SEND_DATA_BUFFER* sendBuf = &( m_sendData.GetBuffer() );
      u32 sendBufSize = m_sendData.GetTotalSize();
      return net::SendToClient( m_myID, sendBuf, sendBufSize );
    }
  }
  else{
    return true;
  }
}

bool Adapter::receptionClient( void )
{
//  if( CommMode != BTL_COMM_NONE )
  if( m_fCommType )
  {
    if( net::CheckReturnFromClient() )
    {
      const void* returnDataPtr;
      net::GetRecvClientData( m_myID, &returnDataPtr );

      const SEND_DATA_BUFFER* returnDataBuffer = reinterpret_cast<const SEND_DATA_BUFFER*>( returnDataPtr );
      m_returnData.Store( *returnDataBuffer );
      m_fRetDataPrepared = true;
      BTL_N_PrintfEx(ADAPTER_PRINT_FLG, DBGSTR_ADAPTER_RECVED_A_CLIENT, m_myID );
      BTL_PRINT("[Adapter_%d] クライアントからの返信がありました\n", m_myID );
      return true;
    }
    return false;
  }
  else
  {
    return m_fRetDataPrepared;
  }
}

//--------------------------------------------------------------------------------------
/**
 *
 */
//--------------------------------------------------------------------------------------
void Adapter::ResetRecvBuffer( void )
{
  m_sendData.Clear();
}
//--------------------------------------------------------------------------------------
/**
 * [CLIENT用] アダプタコマンド受信チェック
 *
 * @param[out] command             受信したコマンドの格納先( 受信していない場合, ACMD_NONE )
 * @param[out] commandSerialNumber 受信したコマンドの通し番号の格納先
 * @param[out] serverSeq           受信したコマンドのサーバーシーケンス格納先
 */
//--------------------------------------------------------------------------------------
void Adapter::RecvCmd( AdapterCommand* command, SendDataSerialNumber* commandSerialNumber, ServerSequence* serverSeq )
{
  // 通信時
//  if( CommMode != BTL_COMM_NONE )
  if( m_fCommType )
  {
    if( net::IsServerCmdReceived() )
    {
      SEND_DATA_BUFFER* sendBuf;
      net::GetReceivedCmdData( (const void**)&sendBuf );
      *command = SEND_DATA_BUFFER_GetAdapterCommand( sendBuf );
      *commandSerialNumber = SEND_DATA_BUFFER_GetSerialNumber( sendBuf );
      *serverSeq = SEND_DATA_BUFFER_GetServerSequence( sendBuf );
      return;
    }
    else
    {
      *command = ACMD_NONE;
      *commandSerialNumber = SEND_DATA_SERIAL_NUMBER_NULL;
      *serverSeq = SERVER_SEQ_NULL;
      return;
    }
  }
  // 非通信時
  else
  {
    if( m_fRetDataPrepared == false )
    {
      *command = m_sendData.GetAdapterCommand();
      *commandSerialNumber = m_sendData.GetSerialNumber();
      *serverSeq = m_sendData.GetServerSequence();
      return;
    }
    else
    {
      *command = ACMD_NONE;
      *commandSerialNumber = SEND_DATA_SERIAL_NUMBER_NULL;
      *serverSeq = SERVER_SEQ_NULL;
      return;
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 * [CLIENT用] アダプタコマンド実データ部受信
 *
 * @param   wk
 * @param   ppRecv [out] データアドレスを受け取るためのポインタアドレス
 *
 * @retval  u32   データサイズ
 */
//--------------------------------------------------------------------------------------
u32 Adapter::GetRecvData( const void** ppRecv ) const
{
  // 通信時
//  if( CommMode != BTL_COMM_NONE )
  if( m_fCommType )
  {
    SEND_DATA_BUFFER* sendBuf;
    net::GetReceivedCmdData( (const void**)&sendBuf );
    if( sendBuf == NULL )
    {//まだ受信していない
      return 0;
    }

    *ppRecv = SEND_DATA_BUFFER_GetData( sendBuf );
    return SEND_DATA_BUFFER_GetDataSize( sendBuf );
  }
  // 非通信時
  else
  {
    *ppRecv = m_sendData.GetData();
    return m_sendData.GetDataSize();
  }
}
//--------------------------------------------------------------------------------------
/**
 * [CLIENT用] サーバへコマンド返信
 *
 * @param   wk
 * @param   data    返信データアドレス
 * @param   size    返信データサイズ
 *
 * @retval  bool    成功時true（成功するまで毎フレーム呼ぶこと）
 */
//--------------------------------------------------------------------------------------
bool Adapter::ReturnCmd( const SendData& returnData )
{
  // 通信時
  if( m_fCommType )
  {
    BTL_PRINT("[Adapter_%d] サーバーに返信します\n", m_myID );
    const SEND_DATA_BUFFER& buffer = returnData.GetBuffer();
    u32 size = returnData.GetTotalSize();
    return net::ReturnToServer( &buffer, size );
  }
  // 非通信時
  else
  {
    if( m_fRetDataPrepared == false )
    {
      m_returnData.CopyFrom( returnData );
      m_fRetDataPrepared = true;
      BTL_PRINT("[Adapter_%d] ローカルクライアントからの返信がありました\n", m_myID );
    }
    return true;
  }
}


GFL_NAMESPACE_END(btl)

#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif