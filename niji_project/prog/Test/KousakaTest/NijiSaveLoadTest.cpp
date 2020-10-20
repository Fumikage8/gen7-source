#if PM_DEBUG
//==============================================================================
/**
 * @file	NijiSaveLoadTest.cpp
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


// mine
#include "Test/KousakaTest/NijiSaveLoadTest.h"
#include <system/include/GflUse.h>
#include <System/include/nijiAllocator.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(NijiSaveLoadTest)



//コンストラクタ
NijiSaveLoadTest::NijiSaveLoadTest( void )
{
  m_pMemAllocator = NULL;
	m_pDevAllocator = NULL;
  m_pNijiAllocator = NULL;
	m_pRenderingPipeLine = NULL;
  m_pDrawUtilText = NULL;

  m_stopRequest = false;
  m_interval = 0;

  m_lastSaveTime = 0;
  m_lastLoadTime = 0;

  m_mode = 0;
  m_waitTime = 0;
  m_DispTime = 0;

  gfl2::debug::SetAssertEnable( false );
}


//デストラクタ
NijiSaveLoadTest::~NijiSaveLoadTest( void )
{
  gfl2::debug::SetAssertEnable( true );
  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  pGameData->GetSaveLoadControl()->SetDebugDummySizeEnable( false );
}

//------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result NijiSaveLoadTest::InitFunc(gfl2::proc::Manager* pManager)
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



  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result NijiSaveLoadTest::UpdateFunc(gfl2::proc::Manager* pManager)
{
  // 文字描画
  this->DrawText();

  // ボタン検知
  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  gfl2::heap::HeapBase *pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  switch( m_mode )
  {
  case MODE_SETTING:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {//セーブ開始
        m_stopRequest = false;
        m_mode = MODE_SAVE_INTERVAL;
        m_waitTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
        pGameData->GetSaveLoadControl()->SetDebugDummySizeEnable( false );
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
      {//セーブ開始
        m_stopRequest = false;
        m_mode = MODE_SAVE_INTERVAL;
        m_waitTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
        pGameData->GetSaveLoadControl()->SetDebugDummySizeEnable( true );
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        return gfl2::proc::RES_FINISH;
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
      {
        if( m_interval > 0 )
        {
          m_interval--;
        }
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
      {
        m_interval++;
      }
    }
    break;
  case MODE_WAIT_SAVE:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        m_stopRequest = true;
      }

      gfl2::fs::FsCtrBackupSystem::Result result = pGameData->SaveMain();
      if( result != gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE )
      {
        if(result == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
        {
          m_lastSaveTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds() - m_waitTime;
          m_waitTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
          m_mode = MODE_LOAD_INTERVAL;
        }
        else
        {//エラー
          m_mode = MODE_ERROR;
        }
      }
    }
  break;
  case MODE_WAIT_LOAD:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        m_stopRequest = true;
      }

      gfl2::fs::FsCtrBackupSystem::Result result = pGameData->LoadMain();
      if( result != gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE )
      {
        if(result == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
        {
          m_lastLoadTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds() - m_waitTime;
          if( m_stopRequest )
          {
            m_mode = MODE_SETTING;
          }
          else
          {
            m_waitTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
            m_mode = MODE_SAVE_INTERVAL;
          }
        }
        else
        {//エラー
          m_mode = MODE_ERROR;
        }
      }
    }
  break;
  };

  switch( m_mode )
  {
  case MODE_SAVE_INTERVAL:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        m_stopRequest = true;
      }

      u64 nowTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
      if( nowTime - m_waitTime >= m_interval * 1000 )
      {
        pGameData->SaveStart( pDeviceHeap );
        m_waitTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
        m_mode = MODE_WAIT_SAVE;
      }
    }
    break;
  case MODE_LOAD_INTERVAL:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        m_stopRequest = true;
      }

      u64 nowTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
      if( nowTime - m_waitTime >= m_interval * 1000 )
      {
        gfl2::fs::FsCtrBackupSystem::Result loadResult = pGameData->LoadStart( pDeviceHeap );
        if( loadResult == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
        {
          m_waitTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
          m_mode = MODE_WAIT_LOAD;
        }
        else
        {//エラー
          m_mode = MODE_ERROR;
        }
      }
    }
    break;
  }

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void NijiSaveLoadTest::DrawFunc(gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo displayNo)
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
gfl2::proc::Result NijiSaveLoadTest::EndFunc(gfl2::proc::Manager* pManager)
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
* @brief 文字描画
*/
//------------------------------------------------------------------
void NijiSaveLoadTest::DrawText( void )
{
  gfl2::math::Vector4 color( 1.0f, 1.0f, 1.0f, 1.0f );
  m_pDrawUtilText->ClearDrawText();
  m_pDrawUtilText->SetTextScale( 1.0f, 1.0f );
  //m_pDrawUtilText->DrawText( 0, 70, L"seq %d", color, m_subseq, print::SystemFont_GetFont() );

  wchar_t saveText[32] = { 0 };
  swprintf( saveText , sizeof(saveText) , L"LastSave[%u]msec" , m_lastSaveTime );
  m_pDrawUtilText->DrawText( 0, 70, saveText, color, print::SystemFont_GetFont() );
  
  wchar_t loadText[32] = { 0 };
  swprintf( loadText , sizeof(loadText) , L"LastLoad[%u]msec" , m_lastLoadTime );
  m_pDrawUtilText->DrawText( 0, 50, loadText, color, print::SystemFont_GetFont() );
  

  switch( m_mode )
  {
  case MODE_SETTING:
    m_pDrawUtilText->DrawText( 0, 100, L"MODE_SETTING", color, print::SystemFont_GetFont() );
    m_waitTime = 0;
  break;
  case MODE_WAIT_SAVE:
    color.Set( 1.0f , 0.0f , 0.0f , 1.0f );
    m_pDrawUtilText->DrawText( 0, 100, L"MODE_WAIT_SAVE...", color, print::SystemFont_GetFont() );
  break;
  case MODE_WAIT_LOAD:
    color.Set( 0.0f , 0.0f , 1.0f , 1.0f );
    m_pDrawUtilText->DrawText( 0, 100, L"MODE_WAIT_LOAD...", color, print::SystemFont_GetFont() );
  break;
  case MODE_SAVE_INTERVAL:
    m_pDrawUtilText->DrawText( 0, 100, L"MODE_INTERVAL...", color, print::SystemFont_GetFont() );
  break;
  case MODE_LOAD_INTERVAL:
    m_pDrawUtilText->DrawText( 0, 100, L"MODE_INTERVAL...", color, print::SystemFont_GetFont() );
  break;
  case MODE_ERROR:
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
      m_errorCode = pGameData->GetSaveLoadControl()->GetLastNNResult().GetPrintableBits();
      wchar_t errorText[32] = { 0 };
      swprintf( errorText , sizeof(errorText) , L"error[0x%x]" , m_errorCode );
      m_pDrawUtilText->DrawText( 0, 20, errorText, color, print::SystemFont_GetFont() );
    }
  break;
  };

  if( m_waitTime != 0 )
  {
    u64 nowTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
    m_DispTime = nowTime - m_waitTime;
  }
    
  color.Set( 1.0f , 1.0f , 1.0f , 1.0f );

  wchar_t timeText[32] = { 0 };
  swprintf( timeText , sizeof(timeText) , L"MODE_RUNNING [%llu]msec" , m_DispTime );

  wchar_t intervalText[32] = { 0 };
  swprintf( intervalText , sizeof(intervalText) , L"←→ Interval [%u]sec" , m_interval );

  switch( m_mode )
  {
  case MODE_SETTING:
    m_pDrawUtilText->DrawText( 0, 170, L"A NormalTest Exec", color, print::SystemFont_GetFont() );
    m_pDrawUtilText->DrawText( 0, 200, L"B Exit", color, print::SystemFont_GetFont() );
    m_pDrawUtilText->DrawText( 0, 150, L"X DummySizeTest Exec", color, print::SystemFont_GetFont() );
    m_pDrawUtilText->DrawText( 0, 130, intervalText, color, print::SystemFont_GetFont() );

  break;
  case MODE_WAIT_SAVE:
    if( m_stopRequest == false )
    {
      m_pDrawUtilText->DrawText( 0, 200, L"B Stop", color, print::SystemFont_GetFont() );
    }
    m_pDrawUtilText->DrawText( 0, 130, timeText, color, print::SystemFont_GetFont() );
  break;
  case MODE_WAIT_LOAD:
    if( m_stopRequest == false )
    {
      m_pDrawUtilText->DrawText( 0, 200, L"B Stop", color, print::SystemFont_GetFont() );
    }
    m_pDrawUtilText->DrawText( 0, 130, timeText, color, print::SystemFont_GetFont() );
  break;
  case MODE_SAVE_INTERVAL:
    if( m_stopRequest == false )
    {
      m_pDrawUtilText->DrawText( 0, 200, L"B Stop", color, print::SystemFont_GetFont() );
    }
    m_pDrawUtilText->DrawText( 0, 130, timeText, color, print::SystemFont_GetFont() );
  break;
  case MODE_LOAD_INTERVAL:
    if( m_stopRequest == false )
    {
      m_pDrawUtilText->DrawText( 0, 200, L"B Stop", color, print::SystemFont_GetFont() );
    }
    m_pDrawUtilText->DrawText( 0, 130, timeText, color, print::SystemFont_GetFont() );
  break;
  };

  
  
  
}





GFL_NAMESPACE_END(NijiSaveLoadTest)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_CTR
#endif //PM_DEBUG
