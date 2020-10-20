#if PM_DEBUG
//==============================================================================
/**
 * @file	P2pConnectionManagerTest.h
 * @brief	P2pConnectionManagerクラスのテスト
 * @author	ichiraku_katsuhiko
 * @date	2015/04/16 12:45:33
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

// nbr
#include <NetStatic/NetEvent/include/NBREvent.h>

// mine
#include "Test/P2pConnectionManagerTest/include/P2pConnectionManagerTestProc.h"
#include <system/include/GflUse.h>
#include <System/include/nijiAllocator.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(P2pConnectionManagerTestProc)


//コンストラクタ
Proc::Proc( void )
{
  m_pMemAllocator = NULL;
	m_pDevAllocator = NULL;
  m_pNijiAllocator = NULL;
	m_pRenderingPipeLine = NULL;
  m_sleepCount = 0;
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
    L"WIRELESS_AUTO",
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
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  switch( m_subseq ){
  case 0:
    {
      // WiFi接続
      switch( m_selectP2pType ){
      case WIFI_PARENT:
      case WIFI_CHILD:
      case WIFI_AUTO:
        {
          // Wifi通信の初期化
          NetLib::NijiNetworkSystem::InitializeWifiNetwork();

          gfl2::heap::HeapBase*  heapDev  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
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
      default:
        // ローカルネットワークの初期化
        NetLib::NijiNetworkSystem::InitializeLocalP2PNetwork();
        break;
      }
    }
    m_subseq++;
    break;

  case 1:
    {
      // 初期化
      switch (m_selectP2pType){
      case WIRELESS_PARENT:
      case WIRELESS_CHILD:
      case WIRELESS_AUTO:
        pP2pConnectionManager->Initialize( NetLib::P2P::NIJI_P2P_BATTLE_2_L );
        break;
      case WIFI_PARENT:
      case WIFI_CHILD:
      case WIFI_AUTO:
        // マッチング履歴クリア
        GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient)->SetClearCacheFlag( true );
        pP2pConnectionManager->Initialize( NetLib::P2P::NIJI_P2P_RNDRATE_D );
        break;
      case IR:
        pP2pConnectionManager->Initialize( NetLib::P2P::NIJI_P2P_LIVE_CUP );
        break;
      }
    }
    m_subseq++;
    break;

  case 2:
    {
      // スリープ値が設定されていたら何もしない
      if( m_sleepCount > 0 )
      {
        m_sleepCount--;
        break;
      }

      // WiFiマッチング用条件
      gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList list;
      gflnet2::nex::NexMatchMakingRule rule1( 0, 111, 111, 111 );
      list.push_back( &rule1 );

      bool ret = false;
      // 接続開始
      switch (m_selectP2pType){
      case WIRELESS_PARENT:
        ret = pP2pConnectionManager->ConnectStart( gflnet2::p2p::PARENT );
        break;
      case WIRELESS_CHILD:
        ret = pP2pConnectionManager->ConnectStart( gflnet2::p2p::CHILD, 2123352782994296365 );// 一楽緑箱
        break;
      case WIRELESS_AUTO:
        ret = pP2pConnectionManager->ConnectStart( gflnet2::p2p::AUTO );
        break;
      case WIFI_PARENT:
        ret = pP2pConnectionManager->ConnectStartWifi( gflnet2::p2p::PARENT, &list );// 一楽緑箱
        break;
      case WIFI_CHILD:
        ret = pP2pConnectionManager->ConnectStartWifi( gflnet2::p2p::CHILD, &list );// 一楽緑箱
        break;
      case WIFI_AUTO:
        ret = pP2pConnectionManager->ConnectStartWifi( gflnet2::p2p::AUTO, &list );// 一楽緑箱
        break;
      case IR:
        ret = pP2pConnectionManager->ConnectStart( gflnet2::p2p::AUTO );
        break;
      }

      if( ret == true )
      {
        m_subseq++;
      }
    }
    break;

  case 3:
    // 接続待ち
    {
      if( pP2pConnectionManager->IsConnectSuccess() )
      {
        GFL_PRINT("接続成功\n");
        m_subseq++;
      }
    }
    break;

  case 4:
    {
      pP2pConnectionManager->TimingStart( 111, true );
      m_subseq++;
    }
    break;

  case 5:
    {
      if( pP2pConnectionManager->IsTimingEnd( 111 ) )
      {
        m_subseq++;
      }
    }
    break;

  case 6:
    {
      if( m_selectP2pType == WIRELESS_AUTO )
      {
        // @todo クイックマッチの代わり
        static NetEvent::NBR::EVENT_PARAM param;
        param.hostNetID = 0;
        param.battleRule = Regulation::BATTLE_RULE_SINGLE;
        NetEvent::NBR::NBREvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), &param );
        this->SetMainSeq( MAIN_SEQ_END );
      }
      else
      {
        pP2pConnectionManager->DisconnectStart();
        m_subseq++;
      }
    }
    break;

  case 7:
    {
      if( pP2pConnectionManager->IsDisconnectSuccess() )
      {
        gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
        gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);
        if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
        {
          m_subseq++;
        }
        else
        {
          // ループさせる
          m_sleepCount = 180;
          m_subseq = 2;
        }
      }
    }
    break;

  case 8:
    {
      pP2pConnectionManager->Terminate();
      m_subseq++;
    }
    break;

  case 9:
    {
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

          NetLib::NijiNetworkSystem::TerminateNetwork();
        }
        break;
      default:
        NetLib::NijiNetworkSystem::TerminateNetwork();
        break;
      }
      m_subseq++;
    }
    break;
  case 11:
    {
      this->SetMainSeq( MAIN_SEQ_END );
    }
    break;
  }
}

GFL_NAMESPACE_END(P2pConnectionManagerTestProc)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_CTR

#endif //PM_DEBUG