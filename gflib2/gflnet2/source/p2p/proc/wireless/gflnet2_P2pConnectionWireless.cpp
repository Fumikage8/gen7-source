#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pConnectionWireless.cpp
 * @brief	P2Pプロセス：Wireless接続
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 16:33:55
 */
// =============================================================================

#include <nex.h>
#include <util/include/gfl2_std_string.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include <gflnet2/include/p2p/gflnet2_P2pConfig.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include <gflnet2/include/beacon/gflnet2_BeaconCommand.h>
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"
#include "gflnet2/source/dev/gflnet2_PiaAdapter.h"
#include "gflnet2/source/p2p/gflnet2_P2pStaticManager.h"
#include "gflnet2/source/p2p/thread/gflnet2_P2pAutoScanThread.h"

// mine
#include "gflnet2/source/p2p/proc/wireless/gflnet2_P2pConnectionWireless.h"

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( p2p )
GFL_NAMESPACE_BEGIN( proc )

// コンストラクタ
P2pConnectionWireless::P2pConnectionWireless()
{
  m_pBeaconSender = NULL;
  m_context = NULL;
  m_thread = NULL;
  m_registerGatheringTryCount = REGIST_GATHERING_TRY_COUNT_MAX;
  m_contextFailureCount = 0;
  m_pAutoMatchingThread = NULL;
  m_pBeaconData = NULL;
  m_beaconSize = 0;
  m_beaconGameID = 0;
  m_beaconModeID = 0;
}

//------------------------------------------------------------------
/**
  * @brief   ビーコン設定（ローカルの親のみ）
  */
//------------------------------------------------------------------
#ifdef  GF_PLATFORM_CTR
void P2pConnectionWireless::SetLocalP2PMasterBeacon( const void* pBeaconData , u32 beaconSize , u8 gameID , u8 modeID )
{
  m_pBeaconData = pBeaconData;
  m_beaconSize = beaconSize;
  m_beaconGameID = gameID;
  m_beaconModeID = modeID;
}
#endif // GF_PLATFORM_CTR 

//---------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
*/
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pConnectionWireless::InitFunc(gfl2::proc::Manager* )
{
  ICHI_PRINT("P2pConnectionWireless::InitFunc\n");

  switch( GetSubSeq() ){
  case 0:
    m_pBeaconSender = GFL_NEW( GetInitParam()->GetHeapBase() ) gflnet2::wireless::PiaBeaconSender();
    GetBeaconManager()->SetBeaconSender( m_pBeaconSender );
    GetBeaconManager()->SetInitParam( GetInitParam() );
    m_context = gflnet2::nex::NexManager::CreateProtocolCallContext();
    AddSubSeq();
    break;
  case 1:
    GFL_PRINT("P2pInitProc::UdsAdapter\n");
    gflnet2::dev::PiaAdapter::CreateInstance(nn::uds::FORCE_DIRECT_BC, GetInitParam()->GetHeapBase() );
    AddSubSeq();
    break;
  case 2:
    gflnet2::p2p::StaticManager::InitializeSetting();
    gflnet2::dev::PiaAdapter::GetInstance()->CreateLocalCommunicationId( GetInitParam()->GetLocalCommunicationId() );

    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief   実行処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pConnectionWireless::UpdateFunc(gfl2::proc::Manager*)
{
  bool isFinish;

  // 外からの終了リクエスト（切断以外ないはず）
  if( mbEnd )
  {
    SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );
    return gfl2::proc::RES_FINISH;
  }

  // 親、子で処理分け
  if( this->GetConnectType() == CHILD )
  {
    isFinish = UpdateChild();
  }
  else if( this->GetConnectType() == PARENT )
  {
    isFinish = UpdateParent();
  }
  else if( this->GetConnectType() == AUTO )
  {
    isFinish = UpdateAutoMatching();
  }

  if( isFinish )
  {
    // nexの送受信機能を有効化
    DirectStreamEnable();

    SetProcReturn( P2pManager::E_PROC_RETURN_SUCCESS );
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief   終了処理
 * @param   Manager  管理クラス
 * @return  RES_FINISHで終了
 */
//---------------------------------------------------------------------------------
gfl2::proc::Result P2pConnectionWireless::EndFunc(gfl2::proc::Manager*)
{
  // 通信系のスレッド見直す。（killしていない箇所があったため）
  if( m_pAutoMatchingThread )
  {//オートマッチスレッドの終了待ち
    m_pAutoMatchingThread->KillNetThread();
    if( m_pAutoMatchingThread->IsAlive() == true )
    {
      return gfl2::proc::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( m_pAutoMatchingThread );
  }
  if( m_thread )
  {//ビーコンスキャンスレッドの終了待ち
    m_thread->KillNetThread();
    if( m_thread->IsAlive() == true )
    {
      return gfl2::proc::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( m_thread );
  }

  GFL_SAFE_DELETE( m_pBeaconSender );

  if( m_context != NULL )
  {
    if( m_context->GetState() == nn::nex::CallContext::CallInProgress ){
      m_context->Cancel();
      return gfl2::proc::RES_CONTINUE;
    }
    qDelete m_context;
    m_context = NULL;
  }
  return gfl2::proc::RES_FINISH;
}

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief   親更新
 * @return  trueで終了
 */
//------------------------------------------------------------------
bool P2pConnectionWireless::UpdateParent( void )
{
  enum{
    SEQ_CREATE_NETWORK,   // UDSネットワーク構築
    SEQ_CREATE_SESSION,   // セッション作成
    SEQ_BEACON_CREATE,    // ビーコン作成
    SEQ_BEACON_SEND,      // ギャザリング作成したビーコン送信
    SEQ_JOIN_WAIT,        // 相手Join待ち
    SEQ_JOIN_SUCCESS,     // 相手Join成功
  };

  switch( GetSubSeq() ){
  case SEQ_CREATE_NETWORK:   // UDSネットワーク構築
    {
      gflnet2::dev::PiaAdapter* piaAdapter = gflnet2::dev::PiaAdapter::GetInstance();
      if( !piaAdapter->RegisterGathering( UDS_MAX_ENTRY, UDS_NETWORK_PASSPHRASE, sizeof(UDS_NETWORK_PASSPHRASE), UDS_CHANNEL ))
      {
        ICHI_PRINT("ギャザリング失敗\n");
        m_registerGatheringTryCount--;
        if( m_registerGatheringTryCount < 0 )
        {
          GetP2pManager()->ErrorOccur();
          return true;
        }
        break;
      }
      ICHI_PRINT("P2pConnectionWireless::UpdateParent SEQ_CREATE_NETWORK \n");

      SetSubSeq( SEQ_CREATE_SESSION );
    }
    break;

  case SEQ_CREATE_SESSION:   // セッション作成
    {
      if( !nn::nex::Session::CreateSession( SESSION_TITLE ) ){
        GFL_ASSERT(0);  //@fix
      }
      ICHI_PRINT("P2pConnectionWireless::UpdateParent SEQ_CREATE_SESSION \n");
      SetSubSeq( SEQ_BEACON_CREATE );
    }
    break;

  case SEQ_BEACON_CREATE:    // ビーコン作成
    {
      // PiaBeaconSender用ビーコン実データ作成
      //  ReloadBeaconDataのSetBeaconで設定される。（SetBeaconはPiaBeaconSenderのものが使用される）
      GetBeaconManager()->CreateBeaconData();
      if( m_pBeaconData )
      {
        GetBeaconManager()->AddBeaconData( m_beaconGameID, m_beaconModeID, static_cast<const u8*>( m_pBeaconData ) , m_beaconSize );
      }
      else
      {
        u8 sendBeacon[ 4 ];
        gfl2::std::MemFill( sendBeacon, 0, 4 );
        GetBeaconManager()->AddBeaconData( GFLNET_COMMAND_P2P, 0, sendBeacon, 4 );
      }

      SetSubSeq( SEQ_BEACON_SEND );
    }
    break;

  case SEQ_BEACON_SEND:      // ギャザリング作成したビーコン送信
    {
      if( GetBeaconManager()->ReloadBeaconData())
      {
        ICHI_PRINT("P2pConnectionWireless::UpdateParent SEQ_BEACON_SEND \n");
        SetSubSeq( SEQ_JOIN_WAIT );
      }
    }
    break;

  case SEQ_JOIN_WAIT:        // 相手Join待ち
    {
      // メモ： 相手がnexのsessionに参加するとNetGameBaseにデータが入る

      //ICHI_PRINT("P2pConnectionWireless::GetConnectionCount %d \n",NetGame::GetNetGameBase()->GetConnectionCount());
      // 接続人数確認
      int connectMax = GetInitParam()->GetLimitNum();
      if( connectMax == NetGame::GetNetGameBase()->GetConnectionCount() )
      {
        ICHI_PRINT("P2pConnectionWireless::UpdateParent SEQ_JOIN_WAIT \n");
        SetSubSeq( SEQ_JOIN_SUCCESS );
      }
    }
    break;

  case SEQ_JOIN_SUCCESS:     // 相手Join成功
    return true;

  default:
    GFL_ASSERT(0);
    return true;
  }

  return false;
}

//------------------------------------------------------------------
/**
 * @brief   子更新
 * @return  trueで終了
 */
//------------------------------------------------------------------
bool P2pConnectionWireless::UpdateChild( void )
{
  enum{
    SEQ_BEACON_SCAN_THREAD_START, // ビーコンスキャンスレッド作成
    SEQ_BEACON_SCAN_THREAD_WAIT,  // ビーコンスキャンスレッド終了待ち
    SEQ_JOIN_SESSION_START,       // セッションJoin開始
    SEQ_JOIN_SESSION_WAIT,        // セッションJoin待ち
  };

  switch( GetSubSeq() ){
  case SEQ_BEACON_SCAN_THREAD_START: // ビーコンスキャンスレッド作成
    {
      BeaconScanThreadStart();
      SetSubSeq( SEQ_BEACON_SCAN_THREAD_WAIT );
      ICHI_PRINT("P2pConnectionWireless::UpdateChild SEQ_BEACON_SCAN_THREAD_START \n");
    }
    break;

  case SEQ_BEACON_SCAN_THREAD_WAIT:  // ビーコンスキャンスレッド終了待ち
    {
      if( BeaconScanThreadEndCheck() == true )
      {
        SetSubSeq( SEQ_JOIN_SESSION_START );
        ICHI_PRINT("P2pConnectionWireless::UpdateChild SEQ_BEACON_SCAN_THREAD_WAIT \n");
      }
    }
    break;

  case SEQ_JOIN_SESSION_START:               // セッションJoin開始
    {
      JoinSessionStart();
      SetSubSeq( SEQ_JOIN_SESSION_WAIT );
      ICHI_PRINT("P2pConnectionWireless::UpdateChild SEQ_JOIN_SESSION_START \n");
    }
    break;

  case SEQ_JOIN_SESSION_WAIT:                // セッションJoin待ち
    {
      if( CheckJoinSession() )
      {
        ICHI_PRINT("P2pConnectionWireless::UpdateChild SEQ_JOIN_SESSION_WAIT \n");
        return true;
      }
    }
    break;

  default:
    GFL_ASSERT(0);
    return true;
  }

  return false;
}

//------------------------------------------------------------------
/**
  * @brief   自動マッチング更新
  * @return  trueで終了
  */
//------------------------------------------------------------------
bool P2pConnectionWireless::UpdateAutoMatching( void )
{
  enum{
    SEQ_AUTO_MATCH_THREAD_START,  // 自動マッチスレッド作成
    SEQ_JOIN_SESSION_START,       // セッションJoin開始
    SEQ_JOIN_SESSION_WAIT_MASTER,       // セッションJoin待ち(親)
    SEQ_JOIN_SESSION_WAIT_CLIENT,       // セッションJoin待ち(親)
  };

  switch( GetSubSeq() )
  {
  case SEQ_AUTO_MATCH_THREAD_START:
    {
      int connectMax = GetInitParam()->GetLimitNum();
      m_pAutoMatchingThread = GFL_NEW( GetInitParam()->GetHeapBase() ) gflnet2::p2p::LocalAutoMatchingThread( GetInitParam()->GetHeapBase() , connectMax );
      m_pAutoMatchingThread->SetP2pManager( GetP2pManager() );
      m_pAutoMatchingThread->Start( gfl2::thread::Thread::THREAD_PRI_NORMAL - 1 );//メインスレッドより優先する
      SetSubSeq( SEQ_JOIN_SESSION_START );
    }
  break;
  case SEQ_JOIN_SESSION_START:
    {
      if( m_pAutoMatchingThread->IsAlive() == false )
      {
        bool isConnectSuccess = m_pAutoMatchingThread->GetIsConnectSuccess();
        bool isMaster = m_pAutoMatchingThread->IsMaster();
        GFL_SAFE_DELETE( m_pAutoMatchingThread );

        if( isConnectSuccess )
        {//PIA接続完了

          bool masterClientError = (gflnet2::dev::PiaAdapter::GetInstance()->IsMaster() == false) && 
            (gflnet2::dev::PiaAdapter::GetInstance()->IsClient() == false);
          if( masterClientError == true )
          {//何らかの通信エラー
            GFL_PRINT("master == false && client == false\n");
            GFL_PRINT("retry automatch\n");
            m_context->Reset();
            gflnet2::dev::PiaAdapter::GetInstance()->Terminate();
            gflnet2::p2p::StaticManager::DestroyNetZ();
            gflnet2::p2p::StaticManager::InitializeSetting();
            SetSubSeq( SEQ_AUTO_MATCH_THREAD_START );

            return false;
          }

          GFL_PRINT("IsMaster[%d]IsClient[%d]\n",gflnet2::dev::PiaAdapter::GetInstance()->IsMaster(),gflnet2::dev::PiaAdapter::GetInstance()->IsClient());

          if( isMaster )
          {//親なのでセッション作成

            NetGame::GetNetGameBase()->SetMaster( true );

            if( !nn::nex::Session::CreateSession( SESSION_TITLE ) )
            {
              GFL_ASSERT(0);  //@fix
              GFL_PRINT("CreateSession failed retry automatch\n");
              m_context->Reset();
              gflnet2::dev::PiaAdapter::GetInstance()->Terminate();
              gflnet2::p2p::StaticManager::DestroyNetZ();
              gflnet2::p2p::StaticManager::InitializeSetting();
              SetSubSeq( SEQ_AUTO_MATCH_THREAD_START );

              return false;
            }
            GFL_PRINT("CreateSession OK\n");

            SetSubSeq( SEQ_JOIN_SESSION_WAIT_MASTER );
          }
          else
          {//子なので参加
            NetGame::GetNetGameBase()->SetMaster( false );

            JoinSessionStart();
            m_contextFailureCount = CALLFAILURE_DOWNCOUNT_AUTOMATCH;//オートマッチ用のカウンタ
            SetSubSeq( SEQ_JOIN_SESSION_WAIT_CLIENT );
          }
        }
        else
        {
          //キャンセル終了
          return true;
        }
      }
    }
  break;
  case SEQ_JOIN_SESSION_WAIT_MASTER://親
    {
      int connectMax = GetInitParam()->GetLimitNum();
      if( connectMax == NetGame::GetNetGameBase()->GetConnectionCount() )
      {
        //これ以降の参加を受け付けないようにする
        u32 data = 0;
        data = LocalAutoMatchingThread::ID_START_GAME;
        gflnet2::dev::PiaAdapter::GetInstance()->SetDataToBeacon( (char*)&data , sizeof(data) );

        //成功
        return true;
      }
    }
  break;
  case SEQ_JOIN_SESSION_WAIT_CLIENT://子
    {
      if( CheckJoinSession() )
      {
        // piaLocalエラーチェック
        if(gflnet2::dev::PiaAdapter::GetInstance())
        {
          gflnet2::dev::ERROR_CHECK_RESULT isError = gflnet2::dev::PiaAdapter::GetInstance()->ErrorCheck();
          if( isError != gflnet2::dev::RESULT_NONE || m_contextFailureCount == 0 )//JoinSessionで失敗している
          {//リトライ
            GFL_PRINT("retry automatch\n");
            m_context->Reset();
            gflnet2::dev::PiaAdapter::GetInstance()->Terminate();
            gflnet2::p2p::StaticManager::DestroyNetZ();
            gflnet2::p2p::StaticManager::InitializeSetting();
            SetSubSeq( SEQ_AUTO_MATCH_THREAD_START );

            return false;
          }
        }

        //成功
        return true;
      }
    }
  break;
  default:
    GFL_ASSERT(0);
    return true;
  }


  return false;
}

//------------------------------------------------------------------
/**
 * @brief   ビーコンスキャンスレッド作成
 */
//------------------------------------------------------------------
void P2pConnectionWireless::BeaconScanThreadStart( void )
{
  m_thread = GFL_NEW( GetInitParam()->GetHeapBase() ) gflnet2::p2p::P2pAutoScanThread( GetInitParam()->GetHeapBase() );
  if( m_thread ){
    m_thread->SetP2pManager( GetP2pManager() );
    m_thread->Start( GetP2pManager()->GetThreadPriority() );
  }
  else{
    GFL_ASSERT(0); //@fix  動きはしないが飛びもしない
  }
}

//------------------------------------------------------------------
/**
 * @brief   ビーコンスキャンスレッド終了待ち
 * @return  trueでスレッド終了
*/
//------------------------------------------------------------------
bool P2pConnectionWireless::BeaconScanThreadEndCheck( void )
{
  // スレッドがないと動かない
  if( m_thread == NULL )
  {
    return false;
  }

  if( m_thread->IsAlive() == false )
  {
    bool isSuccess = m_thread->IsSuccess();
    GFL_SAFE_DELETE( m_thread );
    if( isSuccess )
    {//成功
      return true;
    }
    else
    {//失敗
      GetP2pManager()->ErrorOccur();
    }
  }

  return false;
}


//------------------------------------------------------------------
/**
 * @brief   相手のSessionにJoin開始
 */
//------------------------------------------------------------------
void P2pConnectionWireless::JoinSessionStart( void )
{
  nn::nex::qBool ret = nn::nex::Session::JoinSession( m_context, *gflnet2::dev::PiaAdapter::GetInstance()->GetMasterUrl());
  if( !ret ) {
    ICHI_PRINT("P2pConnectionWireless JoinSessionFailed\n");
    GetP2pManager()->ErrorOccur();
  }
  else
  {
    m_contextFailureCount = CALLFAILURE_DOWNCOUNT;
    ICHI_PRINT("P2pWirelessProc: JoinSessionSuccess\n");
  }
}

//------------------------------------------------------------------
/**
 * @brief   相手のSessionにJoin待ち
 * @return  trueで終了
 */
//------------------------------------------------------------------
bool P2pConnectionWireless::CheckJoinSession( void )
{
  // Contextの状態検査
  switch( m_context->GetState() ){
  case nn::nex::CallContext::CallInProgress:
    break;
  case nn::nex::CallContext::CallFailure:
    m_contextFailureCount--;
    if( m_contextFailureCount == 0 ){
      GetP2pManager()->ErrorOccur();
      return true;  //失敗の場合
    }
    ICHI_PRINT("CallFailure--\n");
    break;
  case nn::nex::CallContext::CallCancelled:
    ICHI_PRINT("CallCancelled\n");
    GetP2pManager()->ErrorOccur();
    return true;
  case nn::nex::CallContext::Available:
  case nn::nex::CallContext::CallSuccess:
    GFL_PRINT("CallSuccess\n");
    return true;
  }
  return false;
}

GFL_NAMESPACE_END( proc )
GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )

#endif // GF_PLATFORM_CTR
