// ============================================================================
/*
 * @file CecTestProc.cpp
 * @brief すれ違い通信のテストです。
 * @date 2015.03.03
 */
// ============================================================================
#if PM_DEBUG

#include "Test/CecTest/include/CecTestProc.h"
#include "Test/CecTest/include/CecTestStateAccessor.h"
#include "Test/CecTest/include/CecTestMessageBoxEventListener.h"
#include <system/include/GflUse.h>
#include <gflnet2/include/cec/gflnet2_ICecCoreMessageBox.h>
#include <gflnet2/include/cec/gflnet2_ICecSendMessageBox.h>
#include <gflnet2/include/cec/gflnet2_ICecRecvMessageBox.h>
#include <ui/include/gfl2_UI.h>
#include <System/include/ThreadPriority.h>
#include <System/include/nijiAllocator.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(CecTest)

//static const u32     TITLEID = nn::cec::MakeCecTitleId(XY_CEC_UNIQUE_ID);     // すれちがい通信ID(固定値)
//static const u32     PRIVATEID = XY_CEC_UNIQUE_ID;   // 固有ID (一本体内に閉じられた値)
//static const char    HMACKEY[] = "82794422010026988327904149461845";
static const u32     TITLEID = 0x12233;     // すれちがい通信ID(固定値)
static const u32     PRIVATEID = 0xaabbccdd;   // 固有ID (一本体内に閉じられた値)
static const char    HMACKEY[] = "12345678901234567890123456789012";


CecTestProc::CecTestProc() :
  m_pStateAccessor( NULL ),
  m_pMessageBoxEventListener( NULL ),
  m_pCecSystem( NULL ),
  m_pMemAllocator( NULL ),
  m_pDevAllocator( NULL ),
  m_pNijiAllocator( NULL ),
  m_pRenderingPipeLine( NULL ),
  m_pResFont( NULL )
{
}


CecTestProc::~CecTestProc()
{
}


gfl2::proc::Result CecTestProc::InitFunc( gfl2::proc::Manager* pManager )
{
  gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // cec
  {
    m_pStateAccessor = GFL_NEW( pMemHeap ) CecTestStateAccessor();
    m_pMessageBoxEventListener = GFL_NEW( pMemHeap ) CecTestMessageBoxEventListener( m_pStateAccessor );
    m_pCecSystem = gflnet2::cec::ICecSystem::CreateCecSystem( pMemHeap );

    // gflnet2のすれ違い通信ライブラリ内で使用する送信パケットと受信パケットのサイズを指定します
    gflnet2::cec::CecSystemInitParam initParam;
    initParam.titleID           = TITLEID;
    initParam.privateID         = PRIVATEID;
    initParam.sendDataSize      = 128;
    initParam.sendIconSize      = 128;
    initParam.sendInfoTextSize  = 128;
    initParam.recvPacketLength  = 30;
    initParam.recvDataSize      = 128;
    initParam.recvIconSize      = 128;
    initParam.recvInfoTextSize  = 128;
    initParam.threadPriority    = System::ThreadPriority::ASYNC_CEC_THREAD_PRIORITY;

    m_pCecSystem->Initialize( pMemHeap, &initParam );

#if GFL_DEBUG
    gflnet2::cec::ICecSystem::ResetCecInfo();
#endif // GFL_DEBUG

    m_pStateAccessor->SetState( CecTestStateCreateMessageBoxCheck );

#if GF_PLATFORM_CTR
    nn::fs::Initialize();
#endif // GF_PLATFORM_CTR

  }

  // font
  {
 	  m_pMemAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pMemHeap );
	  m_pDevAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pDevHeap );
    m_pNijiAllocator = GFL_NEW(pDevHeap) System::nijiAllocator( pDevHeap );

    m_pRenderingPipeLine = GFL_NEW( pManager->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine( m_pNijiAllocator, 512, 256 );
    m_pResFont = gfl2::str::CreateResFont( "data/app/test/fukushima_test/layout_sample_data/fonts/cbf_std_kujira.bffnt", pMemHeap, pDevHeap, pMemHeap );
		m_pDrawUtilText = GFL_NEW( pMemHeap ) gfl2::util::DrawUtilText(m_pMemAllocator, m_pDevAllocator);
		m_pDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
	  m_pDrawUtilText->SetFont( m_pResFont );
  }


  return gfl2::proc::RES_FINISH;
}


gfl2::proc::Result CecTestProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  m_pCecSystem->Update();

  bool bIsFailure = UpdateCec();

  DisplayPrint();

  if( bIsFailure )
  {
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}


bool CecTestProc::UpdateCec()
{
  CecTestState eState = m_pStateAccessor->GetState();
  switch( eState )
  {
    // --------------------------------------------------------------------------------------------
    // メッセージボックスが作成されているか調べる
    case Test::CecTest::CecTestStateCreateMessageBoxCheck:
    {
      gflnet2::cec::ICecCoreMessageBox* pCoreMessageBox = m_pCecSystem->GetCecCoreMessageBox();
      pCoreMessageBox->IsCreateMessageBoxAsync( m_pMessageBoxEventListener );
      m_pStateAccessor->SetState( Test::CecTest::CecTestStateCreateMessageBoxCheckWait );
    }
    break;

    // --------------------------------------------------------------------------------------------
    // メッセージボックスの作成をする
    case Test::CecTest::CecTestStateCreateMessageBox:
    {
      gflnet2::cec::CecMessageBoxCreateCoreParam createCoreParam;
      gfl2::std::MemCopy( HMACKEY, createCoreParam.hmackey, sizeof( createCoreParam.hmackey ) );
      // createCoreParam.icon = ; // アイコンデータを設定する
      createCoreParam.iconSize = sizeof( createCoreParam.icon );
      wchar_t name[64] = L"CEC_TEST_BOX1";
      gfl2::std::MemCopy( name, createCoreParam.name, sizeof( name ) );
      createCoreParam.nameSize = sizeof( name );
      createCoreParam.recvBoxSize = 512 * 1024;
      createCoreParam.sendBoxSize = 512 * 1024;
      createCoreParam.recvMessageCount = 30;
      createCoreParam.sendMessageCount = 1;

      gflnet2::cec::ICecCoreMessageBox* pCoreMessageBox = m_pCecSystem->GetCecCoreMessageBox();
      pCoreMessageBox->CreateMessageBoxAsync( &createCoreParam, m_pMessageBoxEventListener );
      m_pStateAccessor->SetState( Test::CecTest::CecTestStateCreateMessageBoxWait );
    }
    break;

    // --------------------------------------------------------------------------------------------
    // 送信ボックス内のデータを削除する
    case Test::CecTest::CecTestStateDeleteAllSendMessage:
    {
      gflnet2::cec::ICecSendMessageBox* pSendMessageBox = m_pCecSystem->GetCecSendMessageBox();
      pSendMessageBox->DeleteAllMessageAsync( m_pMessageBoxEventListener );
      m_pStateAccessor->SetState( Test::CecTest::CecTestStateDeleteAllSendMessageWait );
    }
    break;

    // --------------------------------------------------------------------------------------------
    // 送信ボックス内に送信データを書き込む
    case Test::CecTest::CecTestStateWriteMessage:
    {
      CecSendData cecSendData;
      cecSendData.testData1 = 1;
      cecSendData.testData2 = 2;
      cecSendData.testData3 = 3;

      // 送信データの設定(必須)
      gflnet2::cec::CecSendPacket cecSendPacket;
      cecSendPacket.pSendData = &cecSendData;
      cecSendPacket.sendSize = sizeof( CecSendData );

      // アイコンデータの設定(必須)
      u8 dummyIcon[128];
      cecSendPacket.pIconData = dummyIcon;
      cecSendPacket.iconSize = sizeof( dummyIcon );

      // 情報テキストの設定(無くても大丈夫)
      wchar_t infoText[16] = L"TEST_DATA";
      cecSendPacket.pInfoTextData = infoText;
      cecSendPacket.infoTextSize = sizeof( infoText );

      gflnet2::cec::ICecSendMessageBox* pSendMessageBox = m_pCecSystem->GetCecSendMessageBox();
      pSendMessageBox->WriteMessageAsync( &cecSendPacket, m_pMessageBoxEventListener );
      m_pStateAccessor->SetState( Test::CecTest::CecTestStateWriteMessageWait );
    }
    break;

    // --------------------------------------------------------------------------------------------
    // すれ違い通信のセットアップが完了され何もしていない状態
    case Test::CecTest::CecTestStateIdle:
    {
      gflnet2::cec::ICecRecvMessageBox* pCecRecvMessageBox = m_pCecSystem->GetCecRecvMessageBox();
      gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();

      gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        pCecRecvMessageBox->GetMessageCountAsync( m_pMessageBoxEventListener );
        m_pStateAccessor->SetState( Test::CecTest::CecTestStateGetMessageCountWait );
      }
      else
      if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
      {
        pCecRecvMessageBox->ReadMessageAsync( m_pMessageBoxEventListener );
        m_pStateAccessor->SetState( Test::CecTest::CecTestStateReadMessageWait );
      }
      else
      if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
      {
        pCecRecvMessageBox->DeleteAllMessageAsync( m_pMessageBoxEventListener );
        m_pStateAccessor->SetState( Test::CecTest::CecTestStateDeleteAllRecvMessageWait );
      }
      else
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        return true;
      }
    }
    break;

    case Test::CecTest::CecTestStateFailure:
    return true;
  }

  return false;
}


void CecTestProc::DisplayPrint()
{
  CecTestState eState = m_pStateAccessor->GetState();

  m_pDrawUtilText->ClearDrawText();


	m_pDrawUtilText->SetTextScale( 1.0f, 1.0f );
  m_pDrawUtilText->DrawText( 0,   0, L"State = %d", eState );
  m_pDrawUtilText->DrawText( 0,  50, L"A GetMessageCount" );
  m_pDrawUtilText->DrawText( 0,  80, L"X ReadMessage" );
  m_pDrawUtilText->DrawText( 0, 110, L"Y DeleteAllMessage" );
  m_pDrawUtilText->DrawText( 0, 140, L"B Exit" );

}


void CecTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
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


gfl2::proc::Result CecTestProc::EndFunc( gfl2::proc::Manager* pManager )
{
  // font
  {
	  m_pDrawUtilText->Terminate();

		GFL_SAFE_DELETE( m_pDrawUtilText );
    void* pFontBuffer = m_pResFont->RemoveResource();
	  GflHeapFreeMemory( pFontBuffer );
	  GFL_SAFE_DELETE( m_pResFont );

    GFL_SAFE_DELETE( m_pRenderingPipeLine );

    GFL_SAFE_DELETE( m_pNijiAllocator );
    GFL_SAFE_DELETE( m_pDevAllocator );
	  GFL_SAFE_DELETE( m_pMemAllocator );
  }

  // cec
  {
    m_pCecSystem->Terminate();

    GFL_SAFE_DELETE( m_pCecSystem );
    GFL_SAFE_DELETE( m_pMessageBoxEventListener );
    GFL_SAFE_DELETE( m_pStateAccessor );
  }

  return gfl2::proc::RES_FINISH;
}


GFL_NAMESPACE_END(CecTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
