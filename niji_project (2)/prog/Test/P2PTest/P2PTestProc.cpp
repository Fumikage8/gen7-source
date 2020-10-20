#if PM_DEBUG
//==============================================================================
/**
 * @file	P2PTestProc.cpp
 * @brief	P2Pテスト用Proc
 * @author	ichiraku_katsuhiko
 * @date	2015/03/10 15:59:52
 */
// =============================================================================

#ifdef  GF_PLATFORM_CTR

// font
#include <print/include/SystemFont.h>
// HEAP
#include "System/include/HeapDefine.h"
// Version
#include "System/include/PokemonVersion.h"

#include <util/include/gfl2_List.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"
#include "gflnet2/source/p2p/gflnet2_P2pStaticManager.h"
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// mine
#include "Test/P2PTest/P2PTestProc.h"
#include <system/include/GflUse.h>
#include <System/include/nijiAllocator.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(P2PTestProc)


//コンストラクタ
Proc::Proc( void )
{
  m_pMemAllocator = NULL;
	m_pDevAllocator = NULL;
  m_pNijiAllocator = NULL;
	m_pRenderingPipeLine = NULL;
  m_pP2pManager = NULL;
  m_pBeaconManager = NULL;
  m_pNetGame = NULL;
}


//デストラクタ
Proc::~Proc( void )
{
}

//------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::InitFunc(gfl2::proc::Manager* pManager)
{
  //HEAPID_APP  本体に関するものはAPPメモリから確保します
  gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // font
  {
 	  m_pMemAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pMemHeap );
	  m_pDevAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pDevHeap );
    m_pNijiAllocator = GFL_NEW(pDevHeap) System::nijiAllocator( pDevHeap );
    m_pRenderingPipeLine = GFL_NEW( pManager->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine( m_pNijiAllocator, 512, 256 );
		m_pDrawUtilText = GFL_NEW(pManager->GetHeap()) gfl2::util::DrawUtilText(m_pMemAllocator, m_pDevAllocator); 
    m_pDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
	  //m_pDrawUtilText->SetFont( print::SystemFont_GetFont() );
  }

  GFL_PRINT("TransferdID:%llu\n", nn::cfg::GetTransferableId(GameSys::GameManager::APLICATION_UNIQUE_ID) );

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  // プロセス実行
  bool isRunning = this->DoSeqProcess();

  // 文字描画
  this->DrawText();

  if( isRunning == true )
  {
    return gfl2::proc::RES_CONTINUE;
  }
  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void Proc::DrawFunc(gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
    case gfl2::gfx::CtrDisplayNo::LEFT:
    case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();
    }
    break;
  }

}

//------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::EndFunc(gfl2::proc::Manager* pManager)
{
  m_pDrawUtilText->Terminate();
	GFL_SAFE_DELETE( m_pDrawUtilText );
  GFL_SAFE_DELETE( m_pRenderingPipeLine );
  GFL_SAFE_DELETE( m_pNijiAllocator );
  GFL_SAFE_DELETE( m_pDevAllocator );
  GFL_SAFE_DELETE( m_pMemAllocator );
  return gfl2::proc::RES_FINISH;
}

//=============================================================================
/**
*  P2pManagerリスナー
*/
//=============================================================================
//------------------------------------------------------------------
/**
* @brief   P2P接続成功通知
*/
//------------------------------------------------------------------
void Proc::OnP2pManagerConnectSuccess()
{
  GFL_PRINT("Proc::OnP2pManagerConnectSuccess\n");
  m_isConnectSuccess = true;
}

//------------------------------------------------------------------
/**
* @brief   P2P切断成功通知
*/
//------------------------------------------------------------------
void Proc::OnP2pManagerDisconnectSuccess()
{
  GFL_PRINT("Proc::OnP2pManagerDisconnectSuccess\n");
  m_isDisconnectSuccess = true;
}

//------------------------------------------------------------------
/**
* @brief P2pManager通信のエラー通知
*/
//------------------------------------------------------------------
void Proc::OnP2pManagerError()
{
  GFL_PRINT("Proc::OnP2pManagerError\n");

}

//------------------------------------------------------------------
/**
* @brief P2pManager通信のエラー通知
*
* @param[in] result     実行結果
* @param[in] errorCode  エラーアプレット用エラーコード
*/
//------------------------------------------------------------------
void Proc::OnP2pManagerError( const nn::nex::qResult& result, u32 errorCode )
{
  GFL_PRINT("Proc::OnP2pManagerError\n");
  GFL_PRINT("result:%d\n", result.IsError() );
  GFL_PRINT("errorCode:%d\n", errorCode );

}

//------------------------------------------------------------------
/**
* @brief P2P切断通知
*/
//------------------------------------------------------------------
void Proc::OnP2pManagerSessionLeft()
{
  GFL_PRINT("Proc::OnP2pManagerSessionLeft\n");

}

//------------------------------------------------------------------
/**
 * @brief マッチメイクエラー通知（独自ハンドリング版）
 */
//------------------------------------------------------------------
void Proc::OnP2pManagerMatchMakeFailed()
{
  GFL_PRINT("Proc::OnP2pManagerMatchMakeFailed\n");
}

/************************************************************************/
/************************************************************************/

//------------------------------------------------------------------
/**
* @brief 文字描画
*/
//------------------------------------------------------------------
void Proc::DrawText( void )
{
  static wchar_t *text[NET_MAX] = 
  {
    L"WIRELESS_PARENT",
    L"WIRELESS_CHILD",
    L"WIFI_PARENT",
    L"WIFI_CHILD",
    L"WIFI_AUTO",
    L"IR",
  };

  gfl2::math::Vector4 color( 1.0f, 1.0f, 1.0f, 1.0f );
  m_pDrawUtilText->ClearDrawText();
  m_pDrawUtilText->SetTextScale( 1.0f, 1.0f );
  m_pDrawUtilText->DrawText( 0, 100, L"P2P MODE", color, print::SystemFont_GetFont() );
  m_pDrawUtilText->DrawText( 0, 130, text[m_selectP2pType], color, print::SystemFont_GetFont() );
  m_pDrawUtilText->DrawText( 0, 170, L"A Start", color, print::SystemFont_GetFont() );
  m_pDrawUtilText->DrawText( 0, 200, L"B Exit", color, print::SystemFont_GetFont() );
}

//--------------------------------------------------------------------------
/**
 * @brief   シーケンス別プロセス処理
 *
 * @retval  true:処理中
 * @retval  false:それ以外
 */
//--------------------------------------------------------------------------
bool Proc::DoSeqProcess( void )
{
  switch( m_mainSeq ){
  case MAIN_SEQ_KEYWAIT:MainSeq_KeyWait();  break;
  case MAIN_SEQ_P2P:    MainSeq_P2P();   break;
  case MAIN_SEQ_END:
    return false;
  }
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   メインシーケンス：キー待ち
 */
//------------------------------------------------------------------
void Proc::MainSeq_KeyWait()
{
  // ボタン検知
  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);


  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_selectP2pType>0 ){m_selectP2pType--;}
  }
  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_selectP2pType<(NET_MAX-1) ){m_selectP2pType++;}
  }
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    this->SetMainSeq( MAIN_SEQ_END );
  }
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    this->SetMainSeq( MAIN_SEQ_P2P );
  }
}
//------------------------------------------------------------------
/**
 * @brief   メインシーケンス：P2P
 */
//------------------------------------------------------------------
void Proc::MainSeq_P2P()
{
  if( m_pP2pManager != NULL )
  {
    m_pP2pManager->NetMain();
  }

  switch( m_subseq ){
  case 0:
    {
      // WiFi接続
      switch( m_selectP2pType ){
      case WIFI_PARENT:
      case WIFI_CHILD:
      case WIFI_AUTO:
        {
          gfl2::heap::HeapBase*  heapDev  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
          NetLib::NijiNetworkSystem::InitializeWifiNetwork();
          NetLib::Wifi::WifiConnectRunner::E_EXEC_MODE mode = NetLib::Wifi::WifiConnectRunner::E_MODE_CONNECT_AND_CREATE_NEXID;
          NetLib::Wifi::WifiConnectRunner* pWifiConnect = GFL_NEW( heapDev ) NetLib::Wifi::WifiConnectRunner( mode );

          while(true)
          {
            NetLib::Wifi::WifiConnectRunner::E_RET_CODE ret = pWifiConnect->Update();
            if( ret != NetLib::Wifi::WifiConnectRunner::E_RET_CONTINUE )
            {
              break;
            }
            GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
            gfl2::thread::ThreadStatic::CurrentSleep(1);
          }
        }
        break;
      }

      // エラーテスト
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->RegistListener( this );

      NetLib::Error::NijiNetworkError error;
      error.SetResultErrorCode( 1005 );
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );

      error.SetResult( NetLib::Error::NijiNetworkError::ERROR_TYPE_IR_BROKEN );
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
      error.SetResult( NetLib::Error::NijiNetworkError::ERROR_TYPE_SESSION_LEFT );
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
      nn::nex::qResult result(false);
      error.SetResultNetZFatal( result );
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
    }
    m_subseq++;
    break;

  case 1:
    // P2P初期化
    P2pInitialize();
    m_pNetGame->Initialize();
    m_subseq++;
    break;

  case 2:
    // NetGameの初期化終了待ち
    if( m_pNetGame->IsInitialize() )
    {
      m_subseq++;
    }
    break;

  case 3:
    // 接続
    {
      if( P2pConnectStart() )
      {
        m_subseq++;
      }
    }
    break;

  case 4:
    // ビーコンを収集
    m_pBeaconManager->MainCheckRecvBeacon();
    if( m_isConnectSuccess )
    {
      GFL_PRINT("接続成功\n");
      m_subseq++;
    }
    break;

  case 5:
    {
      m_pNetGame->TimingStart(11);
      m_subseq++;
    }
    break;

  case 6:
    if( m_pNetGame->IsTimingEnd(11) )
    {
      m_subseq++;
    }
    break;

  case 7:
    m_pNetGame->Finalize();
    m_subseq++;
    break;

  case 8:
    if( m_pP2pManager->DisconnectStart() )
    {
      m_subseq++;
    }
    break;

  case 9:
    if( m_isDisconnectSuccess )
    {
      P2pFinalize();
      m_subseq++;
    }
  case 10:
    {
      // WiFi切断
      switch( m_selectP2pType ){
      case WIFI_PARENT:
      case WIFI_CHILD:
      case WIFI_AUTO:
        {
          gfl2::heap::HeapBase*  heapDev  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
          NetLib::Wifi::WifiConnectRunner::E_EXEC_MODE mode = NetLib::Wifi::WifiConnectRunner::E_MODE_DISCONNECT_SYNC;
          NetLib::Wifi::WifiConnectRunner* pWifiConnect = GFL_NEW( heapDev ) NetLib::Wifi::WifiConnectRunner( mode );

          while(true)
          {
            NetLib::Wifi::WifiConnectRunner::E_RET_CODE ret = pWifiConnect->Update();
            if( ret != NetLib::Wifi::WifiConnectRunner::E_RET_CONTINUE )
            {
              break;
            }
            GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
            gfl2::thread::ThreadStatic::CurrentSleep(1);
          }
        }
        break;
      }
      m_subseq++;
    }
    break;
  case 11:
    {
      NetLib::NijiNetworkSystem::TerminateNetwork();
      this->SetMainSeq( MAIN_SEQ_END );
    }
    break;
  }
}

 //------------------------------------------------------------------
 /**
  * @brief   P2P初期化
  */
 //------------------------------------------------------------------
void Proc::P2pInitialize()
{
  gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

 gflnet2::EN_NET_TYPE netType;
 switch (m_selectP2pType){
 case WIRELESS_PARENT:
 case WIRELESS_CHILD:
   netType = gflnet2::TYPE_P2P_WIRELESS;
   break;
 case WIFI_PARENT:
 case WIFI_CHILD:
 case WIFI_AUTO:
   netType = gflnet2::TYPE_P2P_WIFI;
   break;
 case IR:
   netType = gflnet2::TYPE_P2P_IR;
   break;
 }

  //-------------------------------------
  /// 通信初期化パラメータ（アプリ固有）
  //=====================================
  gflnet2::InitParameter netInitParam = {
    nn::cfg::GetTransferableId(GameSys::GameManager::APLICATION_UNIQUE_ID), // 自分のTransferdID
    pMemHeap, // システムメモリ
    pDevHeap, // デバイスメモリ
    netType, // ネットワーク種別
    0,        // 通信別のID
    2,        // 最大接続人数
    false,    // マイグレーション有効無効
  };


  //-------------------------------------
  /// 通信初期化パラメータ（アプリ共通）
  //=====================================
  gflnet2::InitParameterFixed netInitParamFixed = {
    NIJI_PRODUCT_CODE,///< 任天堂からソフトごとに割り当ててくれるID
    1,      ///< ＲＯＭバージョン番号
    0,      ///< 言語番号
    0,      ///< 通信バージョン
  };

  // ビーコンマネージャが必要なので作る
  m_pBeaconManager = GFL_NEW( pMemHeap ) gflnet2::base::BeaconManager();

  m_pP2pManager = GFL_NEW( pMemHeap ) gflnet2::p2p::P2pManager( pMemHeap, pDevHeap );
  m_pP2pManager->Initialize( &netInitParam, &netInitParamFixed );
  m_pP2pManager->RegistListener( this );
  //m_pP2pManager->Initialize( &netInitParam, &netInitParamFixed, m_pBeaconManager );
  m_isConnectSuccess = false;
  m_isDisconnectSuccess = false;

  // 送受信管理クラス（ないとNetGameBaseがないので、P2PManagerが動かない）
  m_pNetGame = GFL_NEW( pMemHeap ) gflnet2::p2p::NetGame( m_pP2pManager->GetInitParam() );
}

//------------------------------------------------------------------
/**
 * @brief   P2P初期化
 */
//------------------------------------------------------------------
bool Proc::P2pConnectStart()
{
  bool ret = true;

  // WiFiマッチング用条件
  gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList list;
  gflnet2::nex::NexMatchMakingRule rule1( 0, 111, 111, 111 );
  gflnet2::nex::NexMatchMakingRule rule2( 1, 222, 222, 222 );
  list.push_back( &rule1 );
  list.push_back( &rule2 );

  switch (m_selectP2pType){
  case WIRELESS_PARENT:
    ret = m_pP2pManager->ConnectStart( gflnet2::p2p::PARENT, NULL, m_pBeaconManager );
    break;;
  case WIRELESS_CHILD:
    {
      gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      gfl2::util::List<bit64> transferdID( pMemHeap, 2 );
      transferdID.PushBack( 2123352782994296365 ); // 一楽緑箱
      ret = m_pP2pManager->ConnectStart( gflnet2::p2p::CHILD, &transferdID, m_pBeaconManager );
      transferdID.Clear();
    }
    break;
  case WIFI_PARENT:
    ret = m_pP2pManager->ConnectStartWifi( gflnet2::p2p::PARENT, &list );
    break;
  case WIFI_CHILD:
    ret = m_pP2pManager->ConnectStartWifi( gflnet2::p2p::CHILD, &list );
    break;
  case WIFI_AUTO:
    ret = m_pP2pManager->ConnectStartWifi( gflnet2::p2p::AUTO, &list );
    break;
  case IR:
    // 赤外線は自動接続飲み
    ret = m_pP2pManager->ConnectStart( gflnet2::p2p::AUTO, NULL, NULL );
    break;
  }

  return ret;
}

//------------------------------------------------------------------
/**
 * @brief   P2P初期化
 */
//------------------------------------------------------------------
void Proc::P2pFinalize()
{
  GFL_SAFE_DELETE( m_pNetGame );
  GFL_SAFE_DELETE( m_pP2pManager );
  GFL_SAFE_DELETE( m_pBeaconManager );
}

GFL_NAMESPACE_END(P2PTestProc)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_CTR
#endif //PM_DEBUG
