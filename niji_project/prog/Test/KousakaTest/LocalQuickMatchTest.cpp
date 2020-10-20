#if PM_DEBUG
//==============================================================================
/**
 * @file	LocalQuickMatchTest.cpp
 * @brief	ローカルクイックマッチテスト用Proc
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

// mine
#include "Test/KousakaTest/LocalQuickMatchTest.h"
#include <system/include/GflUse.h>
#include <System/include/nijiAllocator.h>

#include <ExtSavedata/include/NijiExtSaveSystem.h>
#include <ExtSavedata/include/NijiExtSaveData.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(LocalQuickMatchTest)



//コンストラクタ
LocalQuickMatchTest::LocalQuickMatchTest( void )
{
  m_pMemAllocator = NULL;
	m_pDevAllocator = NULL;
  m_pNijiAllocator = NULL;
	m_pRenderingPipeLine = NULL;
  m_pDrawUtilText = NULL;

  m_pP2pManager = NULL;
  m_pBeaconManager = NULL;
  m_pNetGame = NULL;

  m_isConnectSuccess = false;
  m_isDisconnectSuccess = false;
  m_mode = 0;
  m_waitTime = 0;
  m_DispTime = 0;
}


//デストラクタ
LocalQuickMatchTest::~LocalQuickMatchTest( void )
{
}

//------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result LocalQuickMatchTest::InitFunc(gfl2::proc::Manager* pManager)
{
  //HEAPID_APP  本体に関するものはAPPメモリから確保します
  gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // font
  {
 	  m_pMemAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pMemHeap );
	  m_pDevAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pDevHeap );
    m_pNijiAllocator = GFL_NEW(pDevHeap) System::nijiAllocator( pDevHeap );
    m_pRenderingPipeLine = GFL_NEW( pManager->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine( m_pNijiAllocator , 512, 256 );
		m_pDrawUtilText = GFL_NEW(pManager->GetHeap()) gfl2::util::DrawUtilText(m_pMemAllocator, m_pDevAllocator); 
    m_pDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
  }

  GFL_PRINT("BATTLE_REC_DATA[%d]\n",sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA));
  GFL_PRINT("BATTLE_REC_HEADER[%d]\n",sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_HEADER));
  GFL_PRINT("BATTLE_REC_UPLOAD_DATA[%d]\n",sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA));
  GFL_PRINT("BATTLE_REC_BODY[%d]\n",sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_BODY));
  GFL_PRINT("REC_DATE[%d]\n",sizeof(ExtSavedata::BattleRecorderSaveData::REC_DATE));
  

  ExtSavedata::NijiExtSaveSystem * p_extsvld = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem();
  GFL_PRINT("CreateExtFile[%d]\n",p_extsvld->CreateExtFile( ExtSavedata::EXTID_BATTLEVIDEO , 0 , sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA) ) );

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result LocalQuickMatchTest::UpdateFunc(gfl2::proc::Manager* pManager)
{


  // 文字描画
  this->DrawText();

  // ボタン検知
  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    if( m_mode == MODE_NONE )
    {//終了
      if( m_pNetGame )
      {
        m_pNetGame->Finalize();
      }
      GFL_SAFE_DELETE( m_pNetGame );
      GFL_SAFE_DELETE( m_pP2pManager );
      GFL_SAFE_DELETE( m_pBeaconManager );

      return gfl2::proc::RES_FINISH;
    }
    else if( m_mode == MODE_AUTO_MATCH_CONNECTED )
    {//切断
      if( m_pNetGame )
      {
        m_pNetGame->Finalize();
      }
      GFL_SAFE_DELETE( m_pNetGame );

      GFL_ASSERT( m_pP2pManager->DisconnectStart() );
      m_isDisconnectSuccess = false;
      m_mode = MODE_AUTO_DISCONNECT;
    }
  }
  else if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
  {//ボタンを押し続けている
    if( m_mode == MODE_NONE )
    {//接続
      P2pInitialize( 2 );
      m_pNetGame->Initialize();
      GFL_ASSERT( m_pNetGame->IsInitialize() );
      GFL_ASSERT( m_pP2pManager->ConnectStart( gflnet2::p2p::AUTO, NULL, m_pBeaconManager ) );

      m_isConnectSuccess = false;
      m_mode = MODE_AUTO_MATCH;
      m_waitTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
    }
  }
  else
  {//ボタンを離している場合
    if( m_mode == MODE_AUTO_MATCH )
    {//切断
      if( m_pNetGame )
      {
        m_pNetGame->Finalize();
      }
      GFL_SAFE_DELETE( m_pNetGame );

      GFL_ASSERT( m_pP2pManager->DisconnectStart() );
      m_isDisconnectSuccess = false;
      m_mode = MODE_AUTO_DISCONNECT;
    }
  }

  if( m_pP2pManager )
  {
    m_pP2pManager->NetMain();
  }

  switch( m_mode )
  {
  case MODE_NONE:
    {
      m_waitTime = 0;
    }
    break;
  case MODE_AUTO_MATCH:
    {
      if( m_isConnectSuccess )
      {
        m_mode = MODE_AUTO_MATCH_CONNECTED;
      }
    }
  break;
  case MODE_AUTO_MATCH_CONNECTED:
    {
      m_waitTime = 0;
    }
  break;
  case MODE_AUTO_DISCONNECT:
    {
      m_waitTime = 0;
      if( m_isDisconnectSuccess )
      {
        GFL_SAFE_DELETE( m_pP2pManager );
        m_mode = MODE_NONE;
      }
    }
  break;
  };

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void LocalQuickMatchTest::DrawFunc(gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo displayNo)
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
gfl2::proc::Result LocalQuickMatchTest::EndFunc(gfl2::proc::Manager* pManager)
{
  m_pDrawUtilText->Terminate();
	GFL_SAFE_DELETE( m_pDrawUtilText );
  GFL_SAFE_DELETE( m_pRenderingPipeLine );
  GFL_SAFE_DELETE( m_pNijiAllocator );
  GFL_SAFE_DELETE( m_pDevAllocator );
  GFL_SAFE_DELETE( m_pMemAllocator );
  return gfl2::proc::RES_FINISH;
}


//------------------------------------------------------------------
/**
  * @brief   P2P接続成功通知
  */
//------------------------------------------------------------------
void LocalQuickMatchTest::OnP2pManagerConnectSuccess()
{
  m_isConnectSuccess = true;
  GFL_PRINT("LocalQuickMatchTest::OnP2pManagerConnectSuccess\n");
}

//------------------------------------------------------------------
/**
  * @brief   P2P切断成功通知
  */
//------------------------------------------------------------------
void LocalQuickMatchTest::OnP2pManagerDisconnectSuccess()
{
  m_isDisconnectSuccess = true;
  GFL_PRINT("LocalQuickMatchTest::OnP2pManagerDisconnectSuccess\n");
}

//------------------------------------------------------------------
/**
* @brief 文字描画
*/
//------------------------------------------------------------------
void LocalQuickMatchTest::DrawText( void )
{
  gfl2::math::Vector4 color( 1.0f, 1.0f, 1.0f, 1.0f );
  m_pDrawUtilText->ClearDrawText();
  m_pDrawUtilText->SetTextScale( 1.0f, 1.0f );
  //m_pDrawUtilText->DrawText( 0, 70, L"seq %d", color, m_subseq, print::SystemFont_GetFont() );

  switch( m_mode )
  {
  case MODE_NONE:
    m_pDrawUtilText->DrawText( 0, 100, L"STOP", color, print::SystemFont_GetFont() );
  break;
  case MODE_AUTO_MATCH_CONNECTED:
    color.Set( 1.0f , 0.0f , 0.0f , 1.0f );
    m_pDrawUtilText->DrawText( 0, 100, L"CONNECTED", color, print::SystemFont_GetFont() );
  break;
  case MODE_AUTO_MATCH:
    color.Set( 0.0f , 0.0f , 1.0f , 1.0f );
    m_pDrawUtilText->DrawText( 0, 100, L"WAIT MATCHING...", color, print::SystemFont_GetFont() );
  break;
  };

  if( m_waitTime != 0 )
  {
    u64 nowTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
    m_DispTime = nowTime - m_waitTime;
  }
    

  wchar_t timeText[32] = { 0 };
  swprintf( timeText , sizeof(timeText) , L"WAIT MSEC[%llu]" , m_DispTime );

  m_pDrawUtilText->DrawText( 0, 130, timeText, color, print::SystemFont_GetFont() );
  m_pDrawUtilText->DrawText( 0, 170, L"A AutoMatch 2", color, print::SystemFont_GetFont() );
  m_pDrawUtilText->DrawText( 0, 200, L"B Disconnect/Exit", color, print::SystemFont_GetFont() );
}


//------------------------------------------------------------------
/**
* @brief   P2P初期化
*/
//------------------------------------------------------------------
void LocalQuickMatchTest::P2pInitialize( u32 connectNum )
{
  gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

 gflnet2::EN_NET_TYPE netType;
 netType = gflnet2::TYPE_P2P_WIRELESS;

  //-------------------------------------
  /// 通信初期化パラメータ（アプリ固有）
  //=====================================
  gflnet2::InitParameter netInitParam = {
    nn::cfg::GetTransferableId(GameSys::GameManager::APLICATION_UNIQUE_ID), // 自分のTransferdID
    pMemHeap, // システムメモリ
    pDevHeap, // デバイスメモリ
    netType, // ネットワーク種別
    0,        // 通信別のID
    connectNum,        // 最大接続人数
    true,//マイグレーション有効無効
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
  m_isConnectSuccess = false;
  m_isDisconnectSuccess = false;

  // 送受信管理クラス（ないとNetGameBaseがないので、P2PManagerが動かない）
  m_pNetGame = GFL_NEW( pMemHeap ) gflnet2::p2p::NetGame( m_pP2pManager->GetInitParam() );
}



GFL_NAMESPACE_END(LocalQuickMatchTest)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_CTR
#endif //PM_DEBUG
