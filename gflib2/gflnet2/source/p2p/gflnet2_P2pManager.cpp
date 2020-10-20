//=============================================================================
/**
 * @file	 gflnet_P2pManager.cpp
 * @brief	 通信管理クラス P2PNex用
 * @author k.ohno -> ichiraku（niji移植 大幅改造）
 * @date	 2011.5.13
 */
//=============================================================================

#include "gflnet2/include/p2p/gflnet2_P2pManager.h"
#include "gflnet2/include/p2p/gflnet2_NetGame.h"
#include <gflnet2/source/p2p/gflnet2_NetGameBase.h>
#include "gflnet2/include/p2p/gflnet2_NetPlayer.h"

#include <thread/include/gfl2_Thread.h>
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// P2P PROC
#include <gflnet2/source/p2p/proc/ir/gflnet2_P2pConnectionIr.h>
#include <gflnet2/source/p2p/proc/ir/gflnet2_P2pMainIr.h>
#include <gflnet2/source/p2p/proc/ir/gflnet2_P2pDisconnectionIr.h>
#include <gflnet2/source/p2p/proc/wireless/gflnet2_P2pConnectionWireless.h>
#include <gflnet2/source/p2p/proc/wireless/gflnet2_P2pMainWireless.h>
#include <gflnet2/source/p2p/proc/wireless/gflnet2_P2pDisconnectionWireless.h>
#include <gflnet2/source/p2p/proc/wifi/gflnet2_P2pConnectionWifi.h>
#include <gflnet2/source/p2p/proc/wifi/gflnet2_P2pSimpleConnectionWifi.h>
#include <gflnet2/source/p2p/proc/wifi/gflnet2_P2pMainWifi.h>
#include <gflnet2/source/p2p/proc/wifi/gflnet2_P2pDisconnectionWifi.h>
#include <gflnet2/source/p2p/proc/wifi/gflnet2_P2pSimpleDisconnectionWifi.h>

#include  <debug/include/gfl2_DebugLogWindow.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)


gfl2::heap::HeapBase* P2pManager::m_pHeapSys=NULL;
gfl2::heap::HeapBase* P2pManager::m_pHeapDev=NULL;
P2pManagerEventListener* P2pManager::m_listener=NULL;

void* P2pManager::MallocFunction(size_t stSize)
{
  void* pData = NULL;
  if( m_pHeapDev->GetTotalAllocatableSize() > static_cast<int>(stSize)){
    pData = GflHeapAllocMemory( m_pHeapDev, stSize);
  }
  if(pData==NULL){
    if( m_listener != NULL )
    {
      m_listener->OnP2pManagerErrorOutOfMemory();
    }
    return GflHeapAllocMemory( m_pHeapSys, stSize);
  }
  return pData;
}

void P2pManager::FreeFunction(void* pMemory)
{
  GflHeapFreeMemory(pMemory);
}


//---------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   pHeapSys  システムヒープ
 * @param   pHeapDev  デバイスヒープ
 */
//---------------------------------------------------------------------------------
P2pManager::P2pManager( gfl2::heap::HeapBase* pHeapSys, gfl2::heap::HeapBase* pHeapDev )
     : gfl2::proc::Manager(pHeapSys)
     , m_requestDisconnect(false)
     , m_threadPriority(gfl2::thread::Thread::THREAD_PRI_NORMAL + 1)
     , m_useSimpleMatchmake(false)
     , m_pBeaconData( NULL )
     , m_beaconSize( 0 )
     , m_beaconGameID( 0 )
     , m_beaconModeID( 0 )
{
  GFL_ASSERT_STOP(m_pHeapSys==NULL);  //@fix
  GFL_ASSERT_STOP(m_pHeapDev==NULL);  //@fix

  m_pHeapSys = pHeapSys;
  m_pHeapDev = pHeapDev;
  m_procReturn = E_PROC_RETURN_SUCCESS;

  m_state = E_STATE_NONE;
}

//---------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//---------------------------------------------------------------------------------
P2pManager::~P2pManager()
{
  m_pHeapSys = NULL;
  m_pHeapDev = NULL;
}

//------------------------------------------------------------------
/**
 * @brief   イベントリスナー登録
 * @param   listener   通知を受け取るP2pManagerEventListener
 */
//------------------------------------------------------------------
void P2pManager::RegistListener( P2pManagerEventListener* listener )
{
  m_listener = listener;
}

//------------------------------------------------------------------
/**
 * @brief   イベントリスナー解除
 */
//------------------------------------------------------------------
void P2pManager::UnregistListener( void )
{
  m_listener = NULL;
}

//---------------------------------------------------------------------------------
/**
 * @brief   通信の初期化
 * @param   pIParam     動的初期化パラメータ
 * @param   pIParamFix  静的初期化パラメータ
 * @return  初期化に成功 true 失敗 false
 */
//---------------------------------------------------------------------------------
bool P2pManager::Initialize( const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix )
{
  m_InitParam.Initialize(pIParam, pIParamFix);
  m_state = E_STATE_INITIALIZE; // 初期化済み

  return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief   シンプルマッチメイク使用するかどうか
 * @attention ミラクル交換専用です。
 */
//---------------------------------------------------------------------------------
void P2pManager::UseSimpleMatchmake()
{
  GFL_PRINT("P2pManager::UseSimpleMatchmake \n");
  m_useSimpleMatchmake = true;
}

//---------------------------------------------------------------------------------
/**
 * @brief   通信のメインループ この中でproc::Mainを呼んでいる  NetZ Terminate 
 */
//---------------------------------------------------------------------------------
void P2pManager::NetMain(void)
{
#if GFL_DEBUG
  {
    static E_STATE debugState;
    if( debugState != m_state )
    {
      debugState = m_state;
      GFL_PRINT("P2pManager::NetMain m_state %d\n", m_state );
      gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"P2pManager : state[%d]\n",m_state);
    }

    if( GetBaseProcess() )
    {
      static int debugProcSubSeq = -1;
      if( debugProcSubSeq != GetBaseProcess()->GetSubSeq() )
      {
        debugProcSubSeq = GetBaseProcess()->GetSubSeq();
        gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"P2pSubSeq : SubSeq[%d]\n",debugProcSubSeq);
      }
    }
  }
#endif

  switch ( m_state ){
  case E_STATE_NONE:          ///< なし
  case E_STATE_INITIALIZE:    ///< 初期化済み
    // none
    break;

  case E_STATE_CONNECT_WAIT:  ///< 接続待ち
    {
      // 接続PROC更新
      gfl2::proc::MainStatus ms = Main();
      if( ms == gfl2::proc:: MAINSTATUS_NULL )
      {
        // 接続成功しているか確認
        m_state = ( m_procReturn == E_PROC_RETURN_SUCCESS ) ? E_STATE_CONNECT_START : E_STATE_DISCONNECT_IDLING;
      }
    }
    break;

  case E_STATE_CONNECT_START:///< 接続開始
    {
      if( this->SendConnectionID() )
      {
        m_state = E_STATE_CONNECT_PARENT_WAIT;
      }
    }
    break;
  case E_STATE_CONNECT_PARENT_WAIT:///< 親からの開始号令待ち
    {
      if( this->IsEnableStartGame() )
      {
        // 接続中PROCをコールする
        P2pMainProcCall();
        // P2P接続成功処理
        P2pConnectSuccess();
        m_state = E_STATE_MAIN;
      }
    }
    break;
  case E_STATE_MAIN:          ///< 接続中
    {
      // 接続中Proc更新
      gfl2::proc::MainStatus ms = Main();

      // 接続中に勝手にProcが終わるのはエラー以外存在しない。（切断は別ステート）
      if( ms == gfl2::proc:: MAINSTATUS_NULL )
      {
        if( m_procReturn != E_PROC_RETURN_ERROR ){ GFL_ASSERT(0); }
        m_state = E_STATE_DISCONNECT_IDLING;
      }
    }
    break;

  case E_STATE_DISCONNECT_IDLING:///< 切断リクエスト待機
    {
      // 切断リクエスト待ち
      if( m_requestDisconnect == true )
      {
        // 今動いているPROCがある場合は、終了させる
        if( GetBaseProcess() )
        {
          GetBaseProcess()->SetProcEnd();
        }

        m_state = E_STATE_MAIN_PROC_FINISH_WAIT;
      }
    }
    break;
  case E_STATE_MAIN_PROC_FINISH_WAIT://< メインプロック終了待ち
    {
      // 接続中Proc更新
      gfl2::proc::MainStatus ms = Main();
      if( ms == gfl2::proc:: MAINSTATUS_NULL )
      {
        // 切断PROCをコールする
        P2pDisconnectProcCall();
        m_state = E_STATE_DISCONNECT_WAIT;
      }
    }
  break;
  case E_STATE_DISCONNECT_WAIT:  ///< 切断待ち
    // 切断PROC終了待ち
    {
      gfl2::proc::MainStatus ms = Main();
      if( ms == gfl2::proc:: MAINSTATUS_NULL )
      {
        // 切断成功時処理
        P2pDisconnectEnd();
        m_state = E_STATE_DISCONNECT_END;
      }
    }
    break;

  case E_STATE_DISCONNECT_END:   ///< 切断完了
    // none
  	break;
  default:
    GFL_ASSERT(0);
    break;
  }

  // NetGame更新
  if( NetGame::GetNetGameBase() ){
#ifdef  GF_PLATFORM_CTR
    NetGame::GetNetGameBase()->Func();
#endif // GF_PLATFORM_CTR 
  }
}

//------------------------------------------------------------------
/**
  * @brief   ビーコン設定（ローカルの親のみ）
  */
//------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
void P2pManager::SetLocalP2PMasterBeacon( const void* pBeaconData , u32 beaconSize , u8 gameID , u8 modeID )
{
  m_pBeaconData = pBeaconData;
  m_beaconSize = beaconSize;
  m_beaconGameID = gameID;
  m_beaconModeID = modeID;
}
#endif // GF_PLATFORM_CTR 


#ifdef  GF_PLATFORM_CTR
//------------------------------------------------------------------
/**
 * @brief   接続開始
 * @param   connectType     接続種類（自動、親、子）
 * @param   transferdIdList 接続相手のTransferdIDリスト（自動ならNULL）
 * @param   pBeaconManager  ビーコンマネージャークラスポインタ ゲームで1つもっているはずなのでそれを渡す
 * @return  trueで開始成功
 */
//------------------------------------------------------------------
bool P2pManager::ConnectStart( const CONNECT_TYPE connectType, gfl2::util::List<bit64>* transferdIdList, gflnet2::base::BeaconManager* pBeaconManager )
{
  ICHI_PRINT("P2pManager::ConnectStart Start…\n");

  // 初期化済み or 切断済み の状態のみ接続開始OK
  if( !(m_state == E_STATE_INITIALIZE || m_state == E_STATE_DISCONNECT_END) )
  {
    GFL_ASSERT_MSG(0,"P2pManager未初期化");
    return false;
  }

  P2pConnectionInterface * proc;

  // 通信種別に応じてPROCを作る
  switch(m_InitParam.GetType()){
  case TYPE_P2P_IR:        ///< P2P:赤外線
    proc = GFLIB_PROC_CALL<P2pManager, proc::P2pConnectionIr>(this);
    break;
  case TYPE_P2P_WIRELESS:  ///< P2P:ワイヤレス
    {
      ICHI_PRINT("P2pManager::ConnectStart Wireless \n");
      nn::nex::Core::UseThreadAutoStack(false);  //使用しない設定 島岡さんサーバP2Pで使用量が増えるため 
      nn::nex::MemoryManager::SetBasicMemoryFunctions(&MallocFunction, &FreeFunction);
      nn::nex::GlobalVariables::AcquireInstance();
      nn::nex::Core::SetThreadMode(nn::nex::Core::ThreadModeUnsafeTransportBuffer);
      nn::nex::PacketBufferManager::EnablePacketBufferPool(false);
      proc::P2pConnectionWireless* pWirelessProc = GFLIB_PROC_CALL<P2pManager, proc::P2pConnectionWireless>(this);
      pWirelessProc->SetLocalP2PMasterBeacon( m_pBeaconData , m_beaconSize , m_beaconGameID , m_beaconModeID );

      proc = pWirelessProc;
    }
    break;
  case TYPE_P2P_WIFI:      ///< P2P:WIFI
    GFL_ASSERT_MSG(0, "Wifi接続は別関数です");
    return false;
    //break;
  default:
    GFL_ASSERT(0);
    return false;
  }

  proc->SetInitParam( &m_InitParam );
  proc->SetP2pManager( this );
  proc->SetConnectType( connectType ); // 接続時のみ接続種類を渡す
  proc->SetBeaconManager( pBeaconManager ); // 接続時のみビーコンマネージャを渡す

  // 接続相手のTransferdIDを初期化
  this->ClearCallTransferedID();

  // 呼び出し相手のTransferdIDセット
  if( transferdIdList != NULL )
  {
    for( gfl2::util::List<bit64>::Iterator it = transferdIdList->Begin(); it != transferdIdList->End(); ++it )
    {
      this->SetCallTransferedID( (*it) );
    }
  }

  // 状態を接続待ちに変更
  m_state = E_STATE_CONNECT_WAIT;
  m_procReturn = E_PROC_RETURN_SUCCESS;
  m_requestDisconnect = false; // 切断リクエストフラグは落とす
  ICHI_PRINT("P2pManager::ConnectStart End\n");
  return true;
}
#endif // GF_PLATFORM_CTR 


#ifdef  GF_PLATFORM_CTR
//------------------------------------------------------------------
/**
 * @brief   接続開始(Wifi接続時のランダムマッチング専用）
 * @param   connectType     接続種類（自動、親、子）
 * @param   ruleLest        マッチング条件リスト
 * @return  trueで開始成功
 */
//------------------------------------------------------------------
bool P2pManager::ConnectStartWifi( const CONNECT_TYPE connectType, gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList* ruleList )
{
  // 初期化済み or 切断済み の状態のみ接続開始OK
  if( !(m_state == E_STATE_INITIALIZE || m_state == E_STATE_DISCONNECT_END) )
  {
    GFL_ASSERT_MSG(0,"P2pManager未初期化");
    return false;
  }

  // Wifi以外は対象外
  if( m_InitParam.GetType() != TYPE_P2P_WIFI )
  {
    GFL_ASSERT_MSG(0, "Wifi以外の設定で呼ばれた");
    return false;
  }

  // 通常マッチング
  if( m_useSimpleMatchmake == false )
  {
    proc::P2pConnectionWifi* proc;
    proc = GFLIB_PROC_CALL<P2pManager, proc::P2pConnectionWifi>(this);
    proc->SetInitParam( &m_InitParam );
    proc->SetP2pManager( this );
    proc->SetConnectType( connectType ); // 接続時のみ接続種類を渡す
    proc->SetBeaconManager( NULL );
    // 条件リストを渡す
    proc->SetMatchRuleList( ruleList );
  }
  // シンプルマッチメイク
  else
  {
    proc::P2pSimpleConnectionWifi* proc;
    proc = GFLIB_PROC_CALL<P2pManager, proc::P2pSimpleConnectionWifi>(this);
    proc->SetInitParam( &m_InitParam );
    proc->SetP2pManager( this );
  }

  // 状態を接続待ちに変更
  m_state = E_STATE_CONNECT_WAIT;
  m_procReturn = E_PROC_RETURN_SUCCESS;
  m_requestDisconnect = false; // 切断リクエストフラグは落とす
  ICHI_PRINT("P2pManager::ConnectStartWifi\n");
  return true;
}
#endif // GF_PLATFORM_CTR 

//------------------------------------------------------------------
/**
 * @brief   切断開始
 * @return  trueで開始成功
 */
//------------------------------------------------------------------
bool P2pManager::DisconnectStart()
{
  switch( m_state ){
  case E_STATE_CONNECT_WAIT: // 接続待ち
  case E_STATE_CONNECT_START:// 接続開始
  case E_STATE_CONNECT_PARENT_WAIT:// 親からの開始号令待ち
  case E_STATE_MAIN:         // 接続中
  case E_STATE_DISCONNECT_IDLING:// エラーが既に起きていて切断待ち
    m_requestDisconnect = true;
    m_state = E_STATE_DISCONNECT_IDLING;
    break;
  default:
    GFL_ASSERT_MSG(0,"適切でないタイミングで切断開始が呼ばれた");
    return false;
  }
  return true;
}

//------------------------------------------------------------------
/**
* @brief P2pManager通信のエラー発生
*/
//------------------------------------------------------------------
void P2pManager::ErrorOccur( void )
{
  if( m_listener )
  {
    m_listener->OnP2pManagerError();
  }
}

//------------------------------------------------------------------
/**
* @brief P2pManager通信のエラー発生
* @param[in] result     実行結果
* @param[in] errorCode  エラーアプレット用エラーコード
*/
//------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
void P2pManager::ErrorOccur( const nn::nex::qResult& result, u32 errorCode )
{
  if( m_listener )
  {
    m_listener->OnP2pManagerError( result, errorCode );
  }
}
#endif // GF_PLATFORM_CTR 

//------------------------------------------------------------------
/**
* @brief P2P切断発生
*/
//------------------------------------------------------------------
void P2pManager::SessionLeftOccur( void )
{
  if( m_listener )
  {
    m_listener->OnP2pManagerSessionLeft();
  }
}

//------------------------------------------------------------------
/**
 * @brief マッチメイクエラー（独自ハンドリング版）
 */
//------------------------------------------------------------------
void P2pManager::MatchMakeFailedOccur( void )
{
  if( m_listener )
  {
    m_listener->OnP2pManagerMatchMakeFailed();
  }
}

//------------------------------------------------------------------
/**
  * @brief   自分のTransferdIDを取得
  * @return  TransferdID
  */
//------------------------------------------------------------------
bit64 P2pManager::GetTransferedID( void )
{
  return m_InitParam.GetTransferedID();
}


//------------------------------------------------------------------
/**
  * @brief   呼び出し相手のTransferdIDが設定されているか
  * @return  trueで設定されている
  */
//------------------------------------------------------------------
bool P2pManager::IsCallTransferdIdIn( void )
{
  for( int i=0; i<CONNECT_MAX_FOUR; i++ )
  {
    if( m_callTransferedID[i] != 0 ){
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------
/**
  * @brief   呼び出し相手のTransferdIDか判定
  * @param   判定するTransferdID
  * @retval  true:呼び出し相手だった
  * @retval  false:呼び出し相手ではない
  */
//------------------------------------------------------------------
bool P2pManager::CheckCallTransferedID( const bit64 checkTransferedID )
{
  for( int i=0; i<CONNECT_MAX_FOUR; i++ )
  {
    if( m_callTransferedID[i] != 0 ){
      if( m_callTransferedID[i] == checkTransferedID ){
        return true;
      }
    }
  }
  return false;
}


//------------------------------------------------------------------
/**
  * @brief   ThreadPriorityを取得
  * @return   ThreadPriority
  */
//------------------------------------------------------------------
int P2pManager::GetThreadPriority( void ) const
{
  return m_threadPriority;
}

//------------------------------------------------------------------
/**
  * @brief   ThreadPriorityを設定
  * @param   ThreadPriority
  */
//------------------------------------------------------------------
void P2pManager::SetThreadPriority( int threadPriority )
{
  m_threadPriority = threadPriority;
}

//=============================================================================
//------------------------------------------------------------------
/**
  * @brief マイグレーション成功
  */
//------------------------------------------------------------------
void P2pManager::OnMigrationSuccess()
{
  GFL_PRINT("P2pManager::OnP2pManagerMigrationStart\n");
  if( m_listener )
  {
    m_listener->OnP2pManagerMigrationSuccess();
  }
}

//=============================================================================
/**
 *  マッチメイクセッション作成成功リスナー
 */
//=============================================================================
#ifdef GF_PLATFORM_CTR
//------------------------------------------------------------------
/**
 * @brief マッチメイクセッション作成成功
 */
//------------------------------------------------------------------
void P2pManager::MatchmakingCreateSuccess( const nn::nex::GatheringID& gatheringId )
{
  GFL_PRINT("P2pManager::OnMatchmakingCreateSuccess\n");
  if( m_listener )
  {
    m_listener->OnMatchmakingCreateSuccess( gatheringId );
  }
}
#endif // GF_PLATFORM_CTR



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
/**
  * @brief   呼び出し相手のTransferdIDを設定
  * @param   相手のTransferdID
  */
//------------------------------------------------------------------
void P2pManager::SetCallTransferedID( const bit64 transferedID )
{
  for( int i=0; i<CONNECT_MAX_FOUR; i++ )
  {
    if( m_callTransferedID[i] == 0 ){
      m_callTransferedID[i] = transferedID;
      break;
    }
  }
}

//------------------------------------------------------------------
/**
  * @brief   呼び出し相手のTransferdIDをクリア
  */
//------------------------------------------------------------------
void P2pManager::ClearCallTransferedID( void )
{
  for( int i=0; i<CONNECT_MAX_FOUR; i++ )
  {
    m_callTransferedID[i] = 0;
  }
}

//------------------------------------------------------------------
/**
 * @brief   接続用IDを送信
 * @return  trueでOK
 */
//------------------------------------------------------------------
bool P2pManager::SendConnectionID( void )
{
#ifdef  GF_PLATFORM_CTR
  if( NetGame::GetNetGameBase() == NULL )
    return false;

  if( NetGame::GetNetGameBase()->IsMaster() )
  {
    return NetGame::GetNetGameBase()->SendConnectionID();
  }
#endif // GF_PLATFORM_CTR 
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   親からの接続開始号令待ち
 * @return  trueでOK
 */
//------------------------------------------------------------------
bool P2pManager::IsEnableStartGame( void )
{
#ifdef  GF_PLATFORM_CTR
  if( NetGame::GetNetGameBase() == NULL )
    return false;

  return NetGame::GetNetGameBase()->IsEnableStartGame();
#else
  return true;
#endif // GF_PLATFORM_CTR 
}



//------------------------------------------------------------------
/**
 * @brief   P2P接続成功時処理
 */
//------------------------------------------------------------------
void P2pManager::P2pConnectSuccess()
{
  // リスナー通知
  if( m_listener )
  {
    m_listener->OnP2pManagerConnectSuccess();
  }
}

//------------------------------------------------------------------
/**
 * @brief   P2P接続中のProcをコール
 */
//------------------------------------------------------------------
void P2pManager::P2pMainProcCall()
{
#ifdef  GF_PLATFORM_CTR
  gflnet2::p2p::P2pBaseProcess* baseProc;

  // 通信種別に応じてPROCを作る
  switch(m_InitParam.GetType()){
  case TYPE_P2P_IR:        ///< P2P:赤外線
    baseProc = GFLIB_PROC_CALL<P2pManager, proc::P2pMainIr>(this);
    break;
  case TYPE_P2P_WIRELESS:  ///< P2P:ワイヤレス
    baseProc = GFLIB_PROC_CALL<P2pManager, proc::P2pMainWireless>(this);
    break;
  case TYPE_P2P_WIFI:      ///< P2P:WIFI
    baseProc = GFLIB_PROC_CALL<P2pManager, proc::P2pMainWifi>(this);
    break;
  default:
    GFL_ASSERT(0);
    return;
  }

  baseProc->SetInitParam( &m_InitParam );
  baseProc->SetP2pManager( this );
#endif // GF_PLATFORM_CTR 

  m_procReturn = E_PROC_RETURN_SUCCESS;

  GFL_PRINT("P2P 接続成功!!!\n");
}

//------------------------------------------------------------------
/**
 * @brief   P2P切断のProcをコール
 */
//------------------------------------------------------------------
void P2pManager::P2pDisconnectProcCall()
{
  GFL_PRINT("P2pManager::P2pDisconnectProcCall\n");
#ifdef  GF_PLATFORM_CTR
  gflnet2::p2p::P2pDisconnectionInterface* baseProc;

  // 通信種別に応じてPROCを作る
  switch(m_InitParam.GetType()){
  case TYPE_P2P_IR:        ///< P2P:赤外線
    baseProc = GFLIB_PROC_CALL<P2pManager, proc::P2pDisconnectionIr>(this);
    break;
  case TYPE_P2P_WIRELESS:  ///< P2P:ワイヤレス
    baseProc = GFLIB_PROC_CALL<P2pManager, proc::P2pDisconnectionWireless>(this);
    break;
  case TYPE_P2P_WIFI:      ///< P2P:WIFI
    {
      if( m_useSimpleMatchmake == false )
      {
        baseProc = GFLIB_PROC_CALL<P2pManager, proc::P2pDisconnectionWifi>(this);
      }
      else
      {
        baseProc = GFLIB_PROC_CALL<P2pManager, proc::P2pSimpleDisconnectionWifi>(this);
      }
    }
    break;
  default:
    GFL_ASSERT(0);
    return;
  }

  baseProc->SetInitParam( &m_InitParam );
  baseProc->SetP2pManager( this );
#endif // GF_PLATFORM_CTR 

  m_procReturn = E_PROC_RETURN_SUCCESS;
}
 

//------------------------------------------------------------------
/**
 * @brief   P2P切断成功時処理
 */
//------------------------------------------------------------------
void P2pManager::P2pDisconnectEnd()
{
#ifdef  GF_PLATFORM_CTR
  switch(m_InitParam.GetType()){
  case TYPE_P2P_WIRELESS:  ///< P2P:ワイヤレス
    ICHI_PRINT("P2pManager::P2pDisconnectEnd Wireless \n");
    nn::nex::GlobalVariables::ReleaseInstance();
    nn::nex::MemoryManager::SetBasicMemoryFunctions(NULL, NULL);
    break;
  }
#endif // GF_PLATFORM_CTR 

  m_requestDisconnect = false; // 切断出来たので切断リクエストフラグは落とす

  // リスナー通知
  if( m_listener )
  {
    m_listener->OnP2pManagerDisconnectSuccess();
  }
}

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)
