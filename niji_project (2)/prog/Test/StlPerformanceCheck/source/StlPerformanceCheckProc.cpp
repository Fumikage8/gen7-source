// ============================================================================
/*
 * @file StlPerformanceCheckProc.cpp
 * @brief stl と gfl の テンプレートライブラリのパフォーマンスを比較します
 * @date 2015.03.03
 */
// ============================================================================
#if PM_DEBUG

#include "Test/StlPerformanceCheck/include/StlPerformanceCheckProc.h"
#include <system/include/GflUse.h>
#include <util/include/gfl2_Map.h>
#include <util/include/gfl2_List.h>
#include <base/include/gfl2_Singleton.h>
#include <ui/include/gfl2_UI.h>
#include <util/include/gfl2_String.h>
#include "AppLib/include/Util/app_util_heap.h"
#include <System/include/nijiAllocator.h>

#include <map>
#include <list>

#if GF_PLATFORM_CTR
#include <nn/os.h>
#endif // GF_PLATFORM_CTR

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(StlPerformanceCheck)

static app::util::Heap* g_pAppHeap = NULL;

class Test
{
public:
  Test()
  {
    GFL_PRINT( "Test constructor\n" );
  }
  Test( const Test& t )
  {
    GFL_PRINT( "Test copy constructor\n" );
  }
  ~Test()
  {
    GFL_PRINT( "Test destructor\n" );
  }
};


StlPerformanceCheckProc::StlPerformanceCheckProc() :
  m_pMemAllocator( NULL ),
  m_pDevAllocator( NULL ),
  m_pNijiAllocator( NULL ),
  m_pRenderingPipeLine( NULL ),
  m_pResFont( NULL ),
  m_DrawFlag( 0 ),
  m_StlMapInsertTime( 0 ),
  m_GflMapInsertTime( 0 ),
  m_StlMapFindTime( 0 ),
  m_GflMapFindTime( 0 ),
  m_StlMapIteratorTime( 0 ),
  m_GflMapIteratorTime( 0 ),
  m_StlMapEraseTime( 0 ),
  m_GflMapEraseTime( 0 ),
  m_StlListPushBackTime( 0 ),
  m_GflListPushBackTime( 0 ),
  m_StlListPopBackTime( 0 ),
  m_GflListPopBackTime( 0 )
{
}


StlPerformanceCheckProc::~StlPerformanceCheckProc()
{
}


gfl2::proc::Result StlPerformanceCheckProc::InitFunc( gfl2::proc::Manager* pManager )
{
  gfl2::heap::HeapBase* pMemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  g_pAppHeap = GFL_NEW( pDevHeap ) app::util::Heap();
  g_pAppHeap->LocalHeapCreate( pDevHeap, pDevHeap, 1024 * 1024 * 2, 1024 * 1024 * 10 );

  // font
  {
 	  m_pMemAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pMemHeap );
	  m_pDevAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pDevHeap );
    m_pNijiAllocator = GFL_NEW(pDevHeap) System::nijiAllocator( pDevHeap );
    m_pRenderingPipeLine = GFL_NEW( pManager->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine( m_pNijiAllocator, 512, 256 );
    m_pResFont = gfl2::str::CreateResFont( "data/app/test/fukushima_test/layout_sample_data/fonts/cbf_std_kujira.bffnt", pMemHeap, pDevHeap, pMemHeap );
		m_pDrawUtilText = GFL_NEW(pManager->GetHeap()) gfl2::util::DrawUtilText(m_pMemAllocator, m_pDevAllocator); 
    m_pDrawUtilText->Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
	  m_pDrawUtilText->SetFont( m_pResFont );
  }

  {
    gfl2::util::String str( pMemHeap, 4 );
    gfl2::util::String str2( pMemHeap, 8 );
    char test[4];
    char test2[4];
    test[0] = 1;
    test[1] = 1;
    test[2] = 1;
    test[3] = 0;
    // NULL文字入れてない時は失敗する
    test2[0] = 1;
    test2[1] = 1;
    test2[2] = 1;
    test2[3] = 1;

    str = test;

    str2 = test;
    str2 += test2;

    GFL_PRINT( "str = %s\n", str.c_str() );
    GFL_PRINT( "str2 = %s\n", str2.c_str() );
  }

  {
    gfl2::util::String str( pMemHeap, 4 );
    gfl2::util::String str2( pMemHeap, 8 );
    char test[4];
    char test2[4];
    test[0] = 1;
    test[1] = 1;
    test[2] = 1;
    test[3] = 0;
    // NULL文字入れた時は成功する
    test2[0] = 1;
    test2[1] = 1;
    test2[2] = 1;
    test2[3] = 0;

    str = test;

    str2 = test;
    str2 += test2;

    GFL_PRINT( "str = %s\n", str.c_str() );
    GFL_PRINT( "str2 = %s\n", str2.c_str() );
  }

  // コンストラクタが呼ばれるタイミングのチェック
  {
    GFL_PRINT( "stl check start ----------------\n" );
    Test t;
    ::std::list<Test> stl_list;
    GFL_PRINT( "stl list push back\n" );
    stl_list.push_back(t);
  }
  GFL_PRINT( "stl check end ----------------\n" );

  {
    GFL_PRINT( "gfl check start ----------------\n" );
    Test t;
    gfl2::util::List<Test> gfl_list( pMemHeap, 3 );
    GFL_PRINT( "gfl list push back\n" );
    gfl_list.PushBack(t);
  }
  GFL_PRINT( "gfl check end ----------------\n" );


  {
    ::std::list<int>  stl_list;
    stl_list.push_back(1);
    stl_list.push_back(2);
    stl_list.push_back(2);
    stl_list.push_back(3);
    stl_list.push_back(3);
    stl_list.push_back(3);
    GFL_PRINT( "size = %d\n", stl_list.size() );
    for( ::std::list<int>::iterator it = stl_list.begin(); it != stl_list.end(); ++it )
    {
      GFL_PRINT( "%d,", *it );
    }
    GFL_PRINT( "\n" );

    stl_list.remove(2);

    GFL_PRINT( "size = %d\n", stl_list.size() );
    for( ::std::list<int>::iterator it = stl_list.begin(); it != stl_list.end(); ++it )
    {
      GFL_PRINT( "%d,", *it );
    }
    GFL_PRINT( "\n" );
  }

  {
    gfl2::util::List<int> gfl_list( pMemHeap, 64 );
    gfl_list.PushBack(1);
    gfl_list.PushBack(2);
    gfl_list.PushBack(2);
    gfl_list.PushBack(3);
    gfl_list.PushBack(3);
    gfl_list.PushBack(3);
    GFL_PRINT( "size = %d\n", gfl_list.Size() );
    for( gfl2::util::List<int>::Iterator it = gfl_list.Begin(); it != gfl_list.End(); ++it )
    {
      GFL_PRINT( "%d,", *it );
    }
    GFL_PRINT( "\n" );

    gfl_list.Remove(2);

    GFL_PRINT( "size = %d\n", gfl_list.Size() );
    for( gfl2::util::List<int>::Iterator it = gfl_list.Begin(); it != gfl_list.End(); ++it )
    {
      GFL_PRINT( "%d,", *it );
    }
    GFL_PRINT( "\n" );
  }

  {
    gfl2::util::List<int> gfl_list( pMemHeap, 64 );
    gfl_list.PushBack(1);
    gfl_list.PushBack(2);
    gfl_list.PushBack(3);
    gfl_list.PushBack(4);
    gfl_list.PushBack(5);

    GFL_PRINT( "size = %d\n", gfl_list.Size() );
    for( gfl2::util::List<int>::Iterator it = gfl_list.Begin(); it != gfl_list.End(); ++it )
    {
      GFL_PRINT( "%d,", *it );
    }
    GFL_PRINT( "\n" );

    gfl_list.Erase();
    GFL_PRINT( "gfl_list.Erase() \n" );

    GFL_PRINT( "size = %d\n", gfl_list.Size() );
    for( gfl2::util::List<int>::Iterator it = gfl_list.Begin(); it != gfl_list.End(); ++it )
    {
      GFL_PRINT( "%d,", *it );
    }
    GFL_PRINT( "\n" );

  }

  {
    GFL_PRINT( "=============================================================\n" );
    GFL_PRINT( "-------------------------------------------------------------\n" );
    GFL_PRINT( "gfl list\n" );
    gfl2::math::Random rand;
    rand.Initialize(12345);

    gfl2::util::List<u32> gfl_list( pMemHeap, 64 );

    for( int i = 0; i < 16; i++ )
    {
      gfl_list.PushBack( rand.Next() );
    }

    {
      for( gfl2::util::List<u32>::Iterator it = gfl_list.Begin(); it != gfl_list.End(); it++ )
      {
        GFL_PRINT( "%d,", *it );
      }
      GFL_PRINT( "\n" );
    }

    {
      gfl2::util::List<u32>::Iterator it = gfl_list.End();
      for( it--; it != gfl_list.End(); it-- )
      {
        GFL_PRINT( "%d,", *it );
      }
      GFL_PRINT( "\n" );
    }
  }

  {
    GFL_PRINT( "=============================================================\n" );
    GFL_PRINT( "-------------------------------------------------------------\n" );
    GFL_PRINT( "gfl map\n" );
    gfl2::math::Random rand;
    rand.Initialize(12345);

    gfl2::util::Map<u32, u32> gfl_map( pMemHeap, 64 );

    for( int i = 0; i < 16; i++ )
    {
      u32 randValue = rand.Next();
      gfl_map.Insert( gfl2::util::Pair<u32, u32>( randValue, randValue ) );
    }

    {
      for( gfl2::util::Map<u32, u32>::Iterator it = gfl_map.Begin(); it != gfl_map.End(); it++ )
      {
        GFL_PRINT( "%d,", it->first );
      }
      GFL_PRINT( "\n" );
    }

    {
      gfl2::util::Map<u32, u32>::Iterator it = gfl_map.End();
      for( it--; it != gfl_map.End(); it-- )
      {
        GFL_PRINT( "%d,", it->first );
      }
      GFL_PRINT( "\n" );
    }
  }

  {
    GFL_PRINT( "=============================================================\n" );
    GFL_PRINT( "-------------------------------------------------------------\n" );
    GFL_PRINT( "gfl map test\n" );

    gfl2::util::Map<u32, u32> gfl_map( pMemHeap, 64 );

    gfl_map.Insert( gfl2::util::Pair<u32, u32>( 5, 5 ) );
    gflMapDebugPrint( gfl_map );

    gfl_map.Insert( gfl2::util::Pair<u32, u32>( 3, 3 ) );
    gfl_map.Insert( gfl2::util::Pair<u32, u32>( 7, 7 ) );
    gflMapDebugPrint( gfl_map );

    gfl_map.Insert( gfl2::util::Pair<u32, u32>( 0, 0 ) );
    gfl_map.Insert( gfl2::util::Pair<u32, u32>( 10, 10 ) );
    gflMapDebugPrint( gfl_map );

    gfl_map.Erase( 0 );
    gflMapDebugPrint( gfl_map );

    gfl_map.Erase( 10 );
    gflMapDebugPrint( gfl_map );

    gfl_map.Insert( gfl2::util::Pair<u32, u32>( 0, 0 ) );
    gfl_map.Insert( gfl2::util::Pair<u32, u32>( 10, 10 ) );
    gfl_map.Erase( 3 );
    gflMapDebugPrint( gfl_map );

    gfl_map.Erase( 0 );
    gfl_map.Erase( 3 );
    gfl_map.Erase( 5 );
    gfl_map.Erase( 7 );
    gfl_map.Erase( 10 );

    GFL_PRINT( "size = %d\n", gfl_map.Size() );

    gfl_map.Insert( gfl2::util::Pair<u32, u32>( 0, 0 ) );
    gfl_map.Insert( gfl2::util::Pair<u32, u32>( 1, 1 ) );
    gfl_map.Insert( gfl2::util::Pair<u32, u32>( 2, 2 ) );
    gflMapDebugPrint( gfl_map );

    gfl_map.Erase( 0 );
    gfl_map.Erase( 1 );
    gfl_map.Erase( 2 );

    GFL_PRINT( "size = %d\n", gfl_map.Size() );

    GFL_PRINT( "\n" );
  }

  GFL_PRINT( "\n" );

  {
    GFL_PRINT( "=============================================================\n" );
    GFL_PRINT( "-------------------------------------------------------------\n" );

    gfl2::util::List<u32> gfl_list( pMemHeap, 64 );
    gfl2::util::Map<u32, u32> gfl_map( pMemHeap, 64 );

    GFL_PRINT( "gfl_list size = %d\n", gfl_list.Size() );
    GFL_PRINT( "gfl_map size = %d\n", gfl_map.Size() );
    
    GFL_PRINT( "gfl_list for loop start.\n" );
      for( gfl2::util::List<u32>::Iterator it = gfl_list.Begin(); it != gfl_list.End(); ++it )
      {
        GFL_PRINT( "gfl_list for loop.\n" );
      }
    GFL_PRINT( "gfl_list for loop end.\n" );


    GFL_PRINT( "gfl_map for loop start.\n" );
      for( gfl2::util::Map<u32, u32>::Iterator it = gfl_map.Begin(); it != gfl_map.End(); it++ )
      {
        GFL_PRINT( "gfl_map for loop.\n" );
      }
    GFL_PRINT( "gfl_map for loop end.\n" );

    GFL_PRINT( "gfl_list size = %d\n", gfl_list.Size() );
    GFL_PRINT( "gfl_map size = %d\n", gfl_map.Size() );

  }

  GFL_PRINT( "\n" );

  return gfl2::proc::RES_FINISH;
}

void StlPerformanceCheckProc::gflMapDebugPrint( gfl2::util::Map<u32, u32>& gfl_map )
{
  GFL_PRINT( "-------------------------------------------------------------\n" );
  
  {
    for( gfl2::util::Map<u32, u32>::Iterator it = gfl_map.Begin(); it != gfl_map.End(); ++it )
    {
      GFL_PRINT( "%d,", it->first );
    }
    GFL_PRINT( "\n" );
  }

  {
    gfl2::util::Map<u32, u32>::Iterator it = gfl_map.End();
    for( --it; it != gfl_map.End(); --it )
    {
      GFL_PRINT( "%d,", it->first );
    }
    GFL_PRINT( "\n" );
  }

}

gfl2::proc::Result StlPerformanceCheckProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();

  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
//    StlMapInsertPerformance();
    GflMapInsertPerformance();
//    StlMapFindPerformance();
    GflMapFindPerformance();
//    StlMapIteratorPerformance();
    GflMapIteratorPerformance();
//    StlMapErasePerformance();
    GflMapErasePerformance();
    m_DrawFlag = 0;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
//    StlListPushBackPerformance();
    GflListPushBackPerformance();
//    StlListPopBackPerformance();
    GflListPopBackPerformance();
    m_DrawFlag = 1;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    return gfl2::proc::RES_FINISH;
  }

  DisplayPrint();

  return gfl2::proc::RES_CONTINUE;
}

void StlPerformanceCheckProc::DisplayPrint()
{
  m_pDrawUtilText->ClearDrawText();

	m_pDrawUtilText->SetTextScale( 1.0f, 1.0f );
  switch( m_DrawFlag )
  {
    case 0:
      m_pDrawUtilText->DrawText( 0,   0, L"stl insert = %d     ", (u32)m_StlMapInsertTime );
      m_pDrawUtilText->DrawText( 0,  30, L"gfl insert = %d     ", (u32)m_GflMapInsertTime );
      m_pDrawUtilText->DrawText( 0,  60, L"stl find = %d       ", (u32)m_StlMapFindTime );
      m_pDrawUtilText->DrawText( 0,  90, L"gfl find = %d       ", (u32)m_GflMapFindTime );
      m_pDrawUtilText->DrawText( 0, 120, L"stl it = %d         ", (u32)m_StlMapIteratorTime );
      m_pDrawUtilText->DrawText( 0, 150, L"gfl it = %d         ", (u32)m_GflMapIteratorTime );
      m_pDrawUtilText->DrawText( 0, 180, L"stl erase = %d      ", (u32)m_StlMapEraseTime );
      m_pDrawUtilText->DrawText( 0, 210, L"gfl erase = %d      ", (u32)m_GflMapEraseTime );
    break;

    case 1:
      m_pDrawUtilText->DrawText( 0,   0, L"stl push b = %d     ", (u32)m_StlListPushBackTime );
      m_pDrawUtilText->DrawText( 0,  30, L"gfl push b = %d     ", (u32)m_GflListPushBackTime );
      m_pDrawUtilText->DrawText( 0,  60, L"stl pop b = %d      ", (u32)m_StlListPopBackTime );
      m_pDrawUtilText->DrawText( 0,  90, L"gfl pop b = %d      ", (u32)m_GflListPopBackTime );
    break;
  }
}

void StlPerformanceCheckProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
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


gfl2::proc::Result StlPerformanceCheckProc::EndFunc( gfl2::proc::Manager* pManager )
{
  // font
  {
	  m_pDrawUtilText->Terminate();
		GFL_SAFE_DELETE(m_pDrawUtilText);

    void* pFontBuffer = m_pResFont->RemoveResource();
	  GflHeapFreeMemory( pFontBuffer );
	  GFL_SAFE_DELETE( m_pResFont );

    GFL_SAFE_DELETE( m_pRenderingPipeLine );

    GFL_SAFE_DELETE( m_pNijiAllocator );
    GFL_SAFE_DELETE( m_pDevAllocator );
	  GFL_SAFE_DELETE( m_pMemAllocator );
  }

  GFL_SAFE_DELETE( g_pAppHeap );

  return gfl2::proc::RES_FINISH;
}


// --------------------------------------------------------------------------
/**
  * @brief  stl の map insert 処理のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::StlMapInsertPerformance()
{
#if GF_PLATFORM_CTR

  gfl2::math::Random rand;

  rand.Initialize(12345);

  ::std::map<u32, u32> map;

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  for( u32 i = 0; i < 10000; ++i )
  {
    map.insert( ::std::pair<u32, u32>( rand.Next(), i ) );
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "stl map insert performance = %d micro second.\n", time.ToMicroSeconds() );
  m_StlMapInsertTime = time.ToMicroSeconds();

#endif // GF_PLATFORM_CTR
}

// --------------------------------------------------------------------------
/**
  * @brief  gfl2 の map insert 処理のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::GflMapInsertPerformance()
{
#if GF_PLATFORM_CTR

  gfl2::math::Random rand;

  rand.Initialize(12345);

  gfl2::util::Map<u32, u32> map( g_pAppHeap->GetDeviceHeap(), 10000 );

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  for( u32 i = 0; i < 10000; ++i )
  {
    map.Insert( gfl2::util::Pair<u32, u32>( rand.Next(), i ) );
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "gfl2 map insert performance = %d micro second.\n", time.ToMicroSeconds() );
  m_GflMapInsertTime = time.ToMicroSeconds();

#endif // GF_PLATFORM_CTR
}

// --------------------------------------------------------------------------
/**
  * @brief  stl の map find 処理のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::StlMapFindPerformance()
{
#if GF_PLATFORM_CTR

  gfl2::math::Random rand;

  rand.Initialize(12345);

  u32 keys[10000];

  ::std::map<u32, u32> map;
  for( u32 i = 0; i < 10000; ++i )
  {
    keys[i] = rand.Next();
    map.insert( ::std::pair<u32, u32>( keys[i], i ) );
  }

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  u32 index = 5000;
  for( u32 i = 0; i < 10000; ++i )
  {
    ::std::map<u32, u32>::iterator it = map.find( keys[ index ] );
    index = ( index + 1 ) % 10000;
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "stl map find performance = %d micro second.\n", time.ToMicroSeconds() );
  m_StlMapFindTime = time.ToMicroSeconds();

#endif // GF_PLATFORM_CTR
}

// --------------------------------------------------------------------------
/**
  * @brief  gfl2 の map find 処理のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::GflMapFindPerformance()
{
#if GF_PLATFORM_CTR

  gfl2::math::Random rand;

  rand.Initialize(12345);

  u32 keys[10000];

  gfl2::util::Map<u32, u32> map( g_pAppHeap->GetDeviceHeap(), 10000 );
  for( u32 i = 0; i < 10000; ++i )
  {
    keys[i] = rand.Next();
    map.Insert( gfl2::util::Pair<u32, u32>( keys[i], i ) );
  }

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  u32 index = 5000;
  for( u32 i = 0; i < 10000; ++i )
  {
    gfl2::util::Map<u32, u32>::Iterator it = map.Find( keys[ index ] );
    index = ( index + 1 ) % 10000;
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "gfl2 map find performance = %d micro second.\n", time.ToMicroSeconds() );
  m_GflMapFindTime = time.ToMicroSeconds();

#endif // GF_PLATFORM_CTR
}

// --------------------------------------------------------------------------
/**
  * @brief  stl の iterator のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::StlMapIteratorPerformance()
{
#if GF_PLATFORM_CTR

  gfl2::math::Random rand;

  rand.Initialize(12345);

  ::std::map<u32, u32> map;
  for( u32 i = 0; i < 10000; ++i )
  {
    map.insert( ::std::pair<u32, u32>( rand.Next(), i ) );
  }

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  for( ::std::map<u32, u32>::iterator it = map.begin(); it != map.end(); ++it )
  {
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "stl iterator performance = %d micro second.\n", time.ToMicroSeconds() );
  m_StlMapIteratorTime = time.ToMicroSeconds();

#endif // GF_PLATFORM_CTR
}

// --------------------------------------------------------------------------
/**
  * @brief  gfl2 の iterator のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::GflMapIteratorPerformance()
{
#if GF_PLATFORM_CTR

  gfl2::math::Random rand;

  rand.Initialize(12345);

  gfl2::util::Map<u32, u32> map( g_pAppHeap->GetDeviceHeap(), 10000 );
  for( u32 i = 0; i < 10000; ++i )
  {
    map.Insert( gfl2::util::Pair<u32, u32>( rand.Next(), i ) );
  }

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  for( gfl2::util::Map<u32, u32>::Iterator it = map.Begin(); it != map.End(); ++it )
  {
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "gfl2 iterator performance = %d micro second.\n", time.ToMicroSeconds() );
  m_GflMapIteratorTime = time.ToMicroSeconds();

#endif // GF_PLATFORM_CTR
}


// --------------------------------------------------------------------------
/**
  * @brief  stl の erase のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::StlMapErasePerformance()
{
#if GF_PLATFORM_CTR

  gfl2::math::Random rand;

  rand.Initialize(12345);

  u32 keys[10000];

  ::std::map<u32, u32> map;
  for( u32 i = 0; i < 10000; ++i )
  {
    keys[i] = rand.Next();
    map.insert( ::std::pair<u32, u32>( keys[i], i ) );
  }
  GFL_PRINT( "size = %d\n", map.size() );

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  u32 deleteIndex = 5000;
  for( u32 i = 0; i < 10000; ++i )
  {
    map.erase( keys[ deleteIndex ] );
    deleteIndex = ( deleteIndex + 1 ) % 10000;
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "stl erase performance = %d micro second.\n", time.ToMicroSeconds() );
  m_StlMapEraseTime = time.ToMicroSeconds();

  GFL_PRINT( "size = %d\n", map.size() );

#endif // GF_PLATFORM_CTR
}


// --------------------------------------------------------------------------
/**
  * @brief  gfl の erase のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::GflMapErasePerformance()
{
#if GF_PLATFORM_CTR

  gfl2::math::Random rand;

  rand.Initialize(12345);

  u32 keys[10000];

  gfl2::util::Map<u32, u32> map( g_pAppHeap->GetDeviceHeap(), 10000 );
  for( u32 i = 0; i < 10000; ++i )
  {
    keys[i] = rand.Next();
    map.Insert( gfl2::util::Pair<u32, u32>( keys[i], i ) );
  }
  GFL_PRINT( "size = %d\n", map.Size() );

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  u32 deleteIndex = 5000;
  for( u32 i = 0; i < 10000; ++i )
  {
    map.Erase( keys[ deleteIndex ] );
    deleteIndex = ( deleteIndex + 1 ) % 10000;
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "gfl2 erase performance = %d micro second.\n", time.ToMicroSeconds() );
  m_GflMapEraseTime = time.ToMicroSeconds();

  GFL_PRINT( "size = %d\n", map.Size() );

#endif // GF_PLATFORM_CTR
}


// --------------------------------------------------------------------------
/**
  * @brief  stl の push_back のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::StlListPushBackPerformance()
{
#if GF_PLATFORM_CTR

  ::std::list<u32>  list;

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  for( u32 i = 0; i < 20000; ++i )
  {
    list.push_back(i);
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "stl push_back performance = %d micro second.\n", time.ToMicroSeconds() );
  m_StlListPushBackTime = time.ToMicroSeconds();

#endif // GF_PLATFORM_CTR
}


// --------------------------------------------------------------------------
/**
  * @brief  gfl の push_back のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::GflListPushBackPerformance()
{
#if GF_PLATFORM_CTR

  gfl2::util::List<u32>  list( g_pAppHeap->GetDeviceHeap(), 20000 );

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  for( u32 i = 0; i < 20000; ++i )
  {
    list.PushBack(i);
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "gfl2 push_back performance = %d micro second.\n", time.ToMicroSeconds() );
  m_GflListPushBackTime = time.ToMicroSeconds();

#endif // GF_PLATFORM_CTR
}


// --------------------------------------------------------------------------
/**
  * @brief  stl の pop_back のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::StlListPopBackPerformance()
{
#if GF_PLATFORM_CTR

  ::std::list<u32>  list;

  for( u32 i = 0; i < 20000; ++i )
  {
    list.push_back(i);
  }

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  for( u32 i = 0; i < 20000; ++i )
  {
    list.pop_back();
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "stl pop_back performance = %d micro second.\n", time.ToMicroSeconds() );
  m_StlListPopBackTime = time.ToMicroSeconds();

#endif // GF_PLATFORM_CTR
}


// --------------------------------------------------------------------------
/**
  * @brief  gfl の pop_back のパフォーマンス計測
  */
// --------------------------------------------------------------------------
void StlPerformanceCheckProc::GflListPopBackPerformance()
{
#if GF_PLATFORM_CTR

  gfl2::util::List<u32>  list( g_pAppHeap->GetDeviceHeap(), 20000 );

  for( u32 i = 0; i < 20000; ++i )
  {
    list.PushBack(i);
  }

  nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  for( u32 i = 0; i < 20000; ++i )
  {
    list.PopBack();
  }

  nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

  nn::os::Tick time = end - start;

  GFL_PRINT( "gfl2 pop_back performance = %d micro second.\n", time.ToMicroSeconds() );
  m_GflListPopBackTime = time.ToMicroSeconds();

#endif // GF_PLATFORM_CTR
}


GFL_NAMESPACE_END(StlPerformanceCheck)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
