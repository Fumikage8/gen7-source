//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   GflUse.cpp
 *  @brief  GameManagerを動作させるために必要なGFLIB2の管理
 *  @date   2015.02.01
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>
#include <math/include/gfl2_math.h>
#include <system/include/Date/gfl2_Date.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>
#include <crypto/include/gfl2_ISHA256Calculator.h>
#include <system/include/HomeSleepAndPowerSystem.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "System/include/nijiAllocator.h"
#include "System/include/ThreadPriority.h"
#include "System/include/GflUse.h"
#include "System/include/SystemEventManager.h"
#include "System/include/HomeSleepAndPowerSystem.h"
#include "System/include/DressUp.h"
#include "Print/include/SystemFont.h"
#include "Sound/include/Sound.h"

#include <arc_def_index/arc_def.h>

#include <ExtSavedata/include/NijiExtSaveSystem.h>

#include "gfx/include/gfl2_DrawUtil.h"

#include "Main/include/Main.h"  //ResidentHeapの初期化/開放関数定義

// gfl2_Typedef.hの検証用Assertコード
#include <types/include/gfl2_Typedef_assert.inl>

#if defined(GF_PLATFORM_CTR)
#include <NetStatic/NetLib/include/Error/NijiNetworkErreulaApplet.h>
#endif



#if defined(GF_PLATFORM_WIN32)
#include <random>
#define NIJI_WIN32_ENABLE_BUTTON_LAYOUT_CHANGE_TO_3DS 1     // ゲームパッドのキー配置変更制御、nijiプロジェクトでは3DSの配置に合わせる事になった　ikuta_junya 150424
#endif

#if defined(GF_PLATFORM_CTR)
// @note CTRでsjisのcsvを使うため追加したが、プログラムサイズが25kbほど増えるため、やめた
#if 0
// for setlocale
#pragma import __use_sjis_ctype 
#endif
#endif


//---------------------------------------------------------------------------------------
// 動的ヘッダシステムの設定
// ファイルスコープの静的構造体実体はヘッダ側に記述できないのでここで定義
// 
// (初期化時以外に明示的に参照する必要のない値なのでスコープをこのファイルに限定)
//---------------------------------------------------------------------------------------
#if GFL_ENABLE_DYNAMIC_HEADER
namespace {
  gfl2::debug::DynamicHeader::RootPathConfig s_dynamicHeaderRootPathConfig = {
    {
      "R:/home/momiji/exec/conv_header/",
      "R:/home/momiji/exec/arc/",
    },
  };
}
#endif //GFL_ENABLE_DYNAMIC_HEADER

namespace System{

//-----------------------------------------------------------------------------
// 静的変数の定義
//-----------------------------------------------------------------------------
gfl2::gfx::DisplayDeviceHandle	System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::NumberOf];
gfl2::gfx::DisplayList*			System::GflUse::m_pDisplayLists[2];
nijiAllocator*			System::GflUse::m_pAllocator = NULL;
nijiAllocator*			System::GflUse::m_pEventDeviceAllocator = NULL;

#if PM_DEBUG
nijiAllocator*			System::GflUse::m_pAllocatorForDebug = NULL;
nijiAllocator*			System::GflUse::m_pAllocatorForDebugDrawUtil = NULL;  // @note デバッガ接続時はHEAPID_DEBUG_DEBUGGER、ROM起動時はHEAPID_DEBUGから生成
void*               System::GflUse::m_pBufferForDebugDrawUtil = NULL;
System::GflUse::RandDebug::Value System::GflUse::m_RandDebugMode = System::GflUse::RandDebug::NONE;
#endif


#if defined(GF_PLATFORM_WIN32)
//!Textureテンポラリー用アロケーター
nijiAllocator*					 System::GflUse::m_pTextureFactoryTemporraryAllocator = NULL;
#endif

//-----------------------------------------------------------------------------
//  hudsonのROMが他環境で動作できない不具合の切り分け対応用PRINT
//-----------------------------------------------------------------------------
#if defined(DEBUG_ONLY_FOR_hudson) || defined(DEBUG_ONLY_FOR_jenkins) || defined(DEBUG_ONLY_FOR_tamada)
#define HUDSON_TROUBLE_PRINT( ... ) GFL_PRINT(__VA_ARGS__)
#else
#define HUDSON_TROUBLE_PRINT( ... ) /* do nothing */
#endif

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------
static uptr                 s_memBlockPtr;

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *  singleton管理
 */
//-----------------------------------------------------------------------------
GflUse::SingltonLibrary GflUse::m_Singlton = { NULL, };

#if defined(VALID_ASSEART_APPLET)
const char*                   GflUse::m_pAssertMsgBuffer = NULL;
gfl2::thread::CriticalSection GflUse::m_pCriticalSectionForAssertMsgBuffer;
b32                           GflUse::m_IsAssertCallApplet = false;  // trueならアサートをアプレットで表示する、デバッガー起動ならtrue、CommonHeapCreate以降で有効
#endif

//----------------------------------------------------------------------------
/**
 *  @brief  乱数取得
 *
 *  @param  max_range 取得する最大値（0 <= n < max_range）
 *
 *  @return 乱数
 */
//-----------------------------------------------------------------------------
u32 GflUse::GetPublicRand( u32 max_range )
{
  GFL_ASSERT_STOP( m_Singlton.m_pRandom != NULL );

#if PM_DEBUG
  switch( m_RandDebugMode )
  {
  case RandDebug::FIX_MIN: return 0;
  case RandDebug::FIX_MAX: return max_range > 0 ? ( max_range - 1 ) : 0;
  }
#endif

  // 乱数取得
  return m_Singlton.m_pRandom->Next64( max_range );
}

//----------------------------------------------------------------------------
/**
 *  @brief  乱数取得（0?U32_MAX)
 *
 *  @return 乱数
 */
//-----------------------------------------------------------------------------
u32 GflUse::GetPublicRand( void )
{
  GFL_ASSERT_STOP( m_Singlton.m_pRandom != NULL );

#if PM_DEBUG
  switch( m_RandDebugMode )
  {
  case RandDebug::FIX_MIN: return 0;
  case RandDebug::FIX_MAX: return 0xFFFFFFFF;
  }
#endif

  // 乱数取得
  return m_Singlton.m_pRandom->Next64();
}

//----------------------------------------------------------------------------
/**
 *  @brief  乱数取得
 *
 *  @param  max_range 取得する最大値（0 <= n < max_range）
 *
 *  @return 乱数
 */
//-----------------------------------------------------------------------------
u64 GflUse::GetPublicRand64( u64 max_range )
{
  GFL_ASSERT_STOP( m_Singlton.m_pRandom != NULL );

#if PM_DEBUG
  switch( m_RandDebugMode )
  {
  case RandDebug::FIX_MIN: return 0;
  case RandDebug::FIX_MAX: return max_range > 0 ? ( max_range - 1 ) : 0;
  }
#endif

  // 乱数取得
  return m_Singlton.m_pRandom->Next64( max_range );
}

//----------------------------------------------------------------------------
/**
 *  @brief  乱数取得（0?U32_MAX)
 *
 *  @return 乱数
 */
//-----------------------------------------------------------------------------
u64 GflUse::GetPublicRand64( void )
{
  GFL_ASSERT_STOP( m_Singlton.m_pRandom != NULL );

#if PM_DEBUG
  switch( m_RandDebugMode )
  {
  case RandDebug::FIX_MIN: return 0;
  case RandDebug::FIX_MAX: return 0xFFFFFFFFFFFFFFFF;
  }
#endif

  // 乱数取得
  return m_Singlton.m_pRandom->Next64();
}

//----------------------------------------------------------------------------
/**
 *  @brief  ポケモン生成乱数取得
 *  @param  max_range 取得する最大値（0 <= n < max_range）
 *  @return 乱数
 */
//-----------------------------------------------------------------------------
u32 GflUse::GetPokemonCreateRand( u32 max_range )
{
  return GetPublicRand(max_range);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ポケモン生成乱数取得（0?U32_MAX)
 *
 *  @return 乱数
 */
//-----------------------------------------------------------------------------
u32 GflUse::GetPokemonCreateRand( void )
{
  return GetPublicRand();
}

//----------------------------------------------------------------------------
/**
 *  @brief  ポケモン生成乱数取得64bit版（0?U64_MAX)
 *
 *  @return 乱数
 *
 *  @note タマゴ生成用seedなど、乱数オブジェクトの初期化に用いるseed値などに利用
 */
//-----------------------------------------------------------------------------
u64 GflUse::GetPokemonCreateRand64( void )
{
  return GetPublicRand64();
}

//----------------------------------------------------------------------------
/**
 *  @brief  ポケモン生成乱数取得
 *  @param  max_range 取得する最大値（0 <= n < max_range）
 *  @return 乱数
 */
//-----------------------------------------------------------------------------
u64 GflUse::GetPokemonCreateRand64( u64 max_range )
{
  return GetPublicRand64(max_range);
}

//----------------------------------------------------------------------------
/**
 *  @brief  暗号用途の乱数を生成します
 *  @detail 暗号用途の場合にのみ使用してください
 */
//-----------------------------------------------------------------------------
void GflUse::GenerateRandomBytes(void* pDst, size_t size)
{
#if defined(GF_PLATFORM_CTR)
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEVICE);
  gfl2::crypto::ISHA256Calculator* pSHA256Calculator = gfl2::crypto::ISHA256Calculator::CreateSHA256Calculator( pHeap, 256, System::ThreadPriority::ASYNC_SHA256_CRYPTO_THREAD_PRIORITY );
  
  gfl2::system::Date date;
  gfl2::system::Date::GetNow(&date);

  struct SeedKey{
    u64 tick;
    u64 now;
  }; // key storage type

  SeedKey key;
  key.tick = gfl2::system::Timer::PerformanceCounter::GetTick();
  key.now = date.GetDateTimeToMilliSecond();

  gfl2::crypto::SHA256Hash* pSHA256Hash = pSHA256Calculator->CalcHash( &key, sizeof( key ) );
  memcpy(pDst, pSHA256Hash->GetData(), std::min( size, pSHA256Hash->GetDataSize() ) );

  GFL_SAFE_DELETE( pSHA256Calculator );
#else // WIN32
  std::random_device rd; // C++11
  std::uniform_int_distribution<int> dist(0, UINT8_MAX);
  for (size_t i = 0; i < size; ++i)
  {
    u8 rnd = dist(rd);
    memcpy((void*)((uintptr_t)pDst + i), &rnd, sizeof(rnd));
  }
#endif // GF_PLATFORM_CTR
}

//----------------------------------------------------------------------------
/**
 *  @brief  ループ内コールバック処理
 */
//-----------------------------------------------------------------------------
bool GflUse::LoopFunc( const char* pbuff )
{
//  m_Singlton.m_pUiDeviceManager->Main();
  return false;
}




//----------------------------------------------------------------------------
/**
 *  @brief  ASSERTループ内コールバック処理
 */
//-----------------------------------------------------------------------------
void GflUse::AssertLoopFunc( const char* pbuff,b32 bStop )
{
  // @note kujira/sangoではこの関数の呼び出し元で無限ループを行っていたが、nijiではこの内部で無限ループを行う

#if GFL_ENABLE_ASSERT
  GFL_RELEASE_PRINT("%s", pbuff );

  // CTR版ROM挙動
  // ファイナル以外でROMに焼かれていた時のみ、アプレット起動
#if defined(VALID_ASSEART_APPLET)

  if( m_IsAssertCallApplet )
  {
    do
    {
      
      bit32 thread_id = gfl2::thread::ThreadStatic::GetCurrentId();
      if( gfl2::thread::ThreadStatic::IsMainThread( thread_id ) )
      {
        // メインスレッドの場合は、すぐにアプレットを呼び出す
        NetLib::Error::NijiNetworkErreulaApplet::ErrorDispChar(pbuff);
      }
      else
      {
        // サブスレッドの場合、メインスレッド側でアプレットを呼び出す
        
        // 排他制御開始
        m_pCriticalSectionForAssertMsgBuffer.Enter();

        // 複数の同時書き込みは行わない
        if( m_pAssertMsgBuffer == NULL )
        {
          int error_disp_wait = 0;
          m_pAssertMsgBuffer = pbuff;
          while( m_pAssertMsgBuffer )
          {
            gfl2::thread::ThreadStatic::CurrentSleep( 1 );
            // ONOUE_PRINT("アプレット待ち=%d\n", error_disp_wait);
            error_disp_wait++;
          }
        }

        // 排他制御終了
        m_pCriticalSectionForAssertMsgBuffer.Leave();
      }
    }
    while( bStop ); // STOP系の場合、無限起動
    
    return;
  }

#endif // #if defined(VALID_ASSEART_APPLET)

  for ever
  {

    // STOP系の場合はXY抜け無し
    if( m_Singlton.m_pUiDeviceManager && bStop == false ) // 入力初期化前にアサートが発生する可能性があるので
    {
      m_Singlton.m_pUiDeviceManager->Main();
      gfl2::ui::Button* p_button = m_Singlton.m_pUiDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
      u32 key = p_button->GetHold();
      u32 trg_key = p_button->GetTrigger();
      if( key ==  (gfl2::ui::BUTTON_X|gfl2::ui::BUTTON_Y) ){
        break;
      }

      // 再度ログ出力
      if( trg_key ==  (gfl2::ui::BUTTON_A) ){
        GFL_RELEASE_PRINT("%s", pbuff );
      }
    }
  }

#else

  // @note GF_PLATFORM_CTR無効 == 製品版、製品版ではSTOP系でしか呼ばれる事が無い
  for ever
  {

  }

#endif // #if defined(GF_PLATFORM_CTR)

}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void GflUse::checkHeap( void )
{
#if PM_DEBUG
  gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  gfl2::heap::HeapBase *deviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
  gfl2::heap::HeapBase *residentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT );
  gfl2::heap::HeapBase *residentDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );
  GFL_ASSERT(systemHeap->Check());
  GFL_ASSERT(deviceHeap->Check());
  GFL_ASSERT(residentHeap->Check());
  GFL_ASSERT(residentDeviceHeap->Check());
#endif  //PM_DEBUG
}

//==============================================================================
/**
 *  @brief  常駐GFLIBシステムの初期化
 */
//==============================================================================
void GflUse::Initialize( void )
{
#if defined(VALID_ASSEART_APPLET)
  {
    // クスレッド内アサートメッセージバッファ用クリティカルセクション初期化
    m_pCriticalSectionForAssertMsgBuffer.Initialize();
  }
#endif

  m_Singlton.SoftResetFlg = false;
  m_Singlton.m_bTransitToEshopTitlePageRequest = false;   /**<  eshop titlePageへの遷移リクエスト  */
  m_Singlton.m_bTransitToEshopPatchPageRequest = false;   /**<  eshop patchPageへの遷移リクエスト  */

  //-----------------------------------------------------------------------------
  /*
   *    Resident以下のヒープ初期化
   */
  //-----------------------------------------------------------------------------
  CommonHeapCreate();
  gfl2::heap::HeapBase *residentDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );


  //-----------------------------------------------------------------------------
  /**
   *   スレッド管理
   */
  //-----------------------------------------------------------------------------
//  gfl2::thread::Thread::InitializeThreadManagement( residentHeap );
//  checkHeap();

  //-----------------------------------------------------------------------------
  /**
   *    Fileシステムの初期化
   */
  //-----------------------------------------------------------------------------
  FileSystemInitialize( residentDeviceHeap );


#if defined(GF_PLATFORM_CTR)
  // デバイス劣化時のファイルアクセス遅延のエミュレーションを初期化
  // 製品版では無効になる
  // 遅延設定はカードへのROMイメージ書き込み時のオプションで指定する
  // 2016.05.19 遅延が開発作業に影響するため無効にする。どう扱うかはNCLと協議
  //nn::fs::InitializeLatencyEmulation();
  nn::fs::SetSdmcEjectionFatal();  // nn::fs::RegisterAutoFatalAsSdmcEjectedEventが非サポートなので置き換えた。  CTR_SDK/include/nn/fs/fs_Api.h参考。
#endif

  /*
   * 拡張セーブデータシステムを生成
   *
   */
  m_Singlton.m_pNijiExtSaveSystem = GFL_NEW( residentDeviceHeap ) ExtSavedata::NijiExtSaveSystem( residentDeviceHeap );

  //-----------------------------------------------------------------------------
  /*
   * 言語管理クラスを生成
   */
  //-----------------------------------------------------------------------------
  m_Singlton.m_pLanguage = GFL_NEW( residentDeviceHeap ) Language();

  //-----------------------------------------------------------------------------
  /**
   *    時間管理
   */
  //-----------------------------------------------------------------------------
  gfl2::system::Date::Initialize();



  //-----------------------------------------------------------------------------
  /**
   *    サウンドシステムの初期化
   */
  //-----------------------------------------------------------------------------
  {
    gfl2::heap::HeapBase* system_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_SOUND);
    gfl2::heap::HeapBase* device_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_SOUND_DEVICE);
    Sound::Initialize(system_heap, device_heap);
    //SoundConfigInit();
#if PM_DEBUG
#if PM_DEBUG_PRINT
    {
      GFL_PRINT("======================================\n");
      GFL_PRINT("INIT END SoundSysHeap  Free Size 0x%08x\n", system_heap->GetTotalFreeSize());
      GFL_PRINT("INIT END SoundDevHeap  Free Size 0x%08x\n", device_heap->GetTotalFreeSize());
      GFL_PRINT("======================================\n");
    }
#endif //PM_DEBUG_PRINT
#endif //PM_DEBUG
  
  }
  //-----------------------------------------------------------------------------
  /**
   *    描画システムの初期化
   *    タイトル用としてグラフィックスシステムを初期化　メモリは下からとる。
   */
  //-----------------------------------------------------------------------------
  //createGraphicsSystemTitle();
  //m_Singlton.CPUframeOverFlag = false;    // 処理落ちフラグ

  //-----------------------------------------------------------------------------
  /**
   *    乱数の初期化
   *    乱数種には起動からの経過時間(nanosec) * 現在時刻(millisec) をSHA256でハッシュ化したものを使用
   */
  //-----------------------------------------------------------------------------
  m_Singlton.m_pRandom = GFL_NEW(residentDeviceHeap) gfl2::math::SFMTRandom();

  u64 seed = 0;
  GenerateRandomBytes(&seed, sizeof(seed));

  GFL_PRINT("GflUse: seed = %llu\n", seed);

  m_Singlton.m_pRandom->Initialize(seed);

  //-----------------------------------------------------------------------------
  /**
   *    VCount監視開始
   */
  //-----------------------------------------------------------------------------
  m_Singlton.m_LastVCount = -1;

  //-----------------------------------------------------------------------------
  /**
   *    システムイベントマネージャの初期化
   */
  //-----------------------------------------------------------------------------
  m_Singlton.m_pSystemEventManager = GFL_NEW(residentDeviceHeap) System::SystemEventManager();
  System::HomeSleepAndPowerSystem::SetSystemEventManager( m_Singlton.m_pSystemEventManager );

  //-----------------------------------------------------------------------------
  /**
   *    ASSERTループ内コールバック関数設定
   */
  //-----------------------------------------------------------------------------
  AssertSetup();
  

  //-----------------------------------------------------------------------------
  /**
   *    動的モジュールマネージャの初期化
   */
  //-----------------------------------------------------------------------------
  {
    gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );

    m_Singlton.m_roManager = GFL_NEW(residentDeviceHeap) gfl2::ro::RoManager( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD ) );
    m_Singlton.m_roManager->Initialize( systemHeap, m_Singlton.m_romAsyncFileManager , "static.crs", ".crr/static.crr" );
  }

  //-----------------------------------------------------------------------------
	/**
	*   アロケーターの初期化
	*/
	//-----------------------------------------------------------------------------
  nijiAllocator::Initialize( residentDeviceHeap );
	m_pAllocator = GFL_NEW(residentDeviceHeap) nijiResidentAllocator( residentDeviceHeap, nijiAllocator::RESIDENT );
  
  gfl2::heap::HeapBase *eventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  m_pEventDeviceAllocator = GFL_NEW(eventDeviceHeap) nijiAllocator( eventDeviceHeap, nijiAllocator::APPLICATION );

  //-----------------------------------------------------------------------------
	/**
	*   ディスプレイの初期化
	*/
	//-----------------------------------------------------------------------------
  gfl2::gfx::GFGL::Initialize( m_pAllocator );
#if defined(GF_PLATFORM_CTR)
  {
    gfl2::gfx::GFGL::InitDesc desc;
    desc.commandBufferSize = 0x120000;
    desc.requestCount = 256;
    desc.colorBufferAllocation = gfl2::gfx::Pool::LocalA;
    desc.depthBufferAllocation = gfl2::gfx::Pool::LocalB;

    gfl2::gfx::GFGL::InitializeCTR(m_pAllocator, desc);
  }
#endif
	m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]		= gfl2::gfx::GFGL::CreateDisplay(m_pAllocator, gfl2::gfx::DisplayMode::CTR_LEFT , gfl2::gfx::Format::A8R8G8B8, gfl2::gfx::Format::D24S8, gfl2::gfx::MultiSample::None);
	m_HandleList[gfl2::gfx::CtrDisplayNo::RIGHT]	= gfl2::gfx::GFGL::CreateDisplay(m_pAllocator, gfl2::gfx::DisplayMode::CTR_RIGHT, gfl2::gfx::Format::A8R8G8B8, gfl2::gfx::Format::D24S8, gfl2::gfx::MultiSample::None);
	m_HandleList[gfl2::gfx::CtrDisplayNo::DOWN]		= gfl2::gfx::GFGL::CreateDisplay(m_pAllocator, gfl2::gfx::DisplayMode::CTR_DOWN , gfl2::gfx::Format::A8R8G8B8, gfl2::gfx::Format::D24S8, gfl2::gfx::MultiSample::None);

  gfl2::gfx::DrawUtil::Initialize(m_pAllocator, 400, 240, 256, 8);

#if PM_DEBUG
  {
    // デバッグ表示用DrawUtilの初期化
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_DEBUGGER ) )
    {
      // デバッガ起動なら、HEAPID_DEBUG_DEBUGGERから膨大に確保
      gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEBUGGER );

      // DrawUtilの拡張バッファをデバッグ用として初期化
      m_pAllocatorForDebugDrawUtil = GFL_NEW( pHeap ) nijiAllocator( pHeap, nijiAllocator::RESIDENT );
      gfl2::gfx::DrawUtil::InitializeExtension( m_pAllocatorForDebugDrawUtil, 40000, 2000 );

      // @note ROM起動時にデバッグメモリが足りなくなるのを防ぐため、デバッグヒープからもメモリ確保しておく
      m_pBufferForDebugDrawUtil = GflHeapAllocMemory( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ), 0xCC11C );  // primitiveNum=4000、drawCommandNum=200での実測地が0xCC11C(アロケータ生成含む)
    }
    else
    {
      // ROM起動なら、HEAPID_DEBUGからフィールドコリジョン表示をカリング込みでギリギリ運用できるサイズを確保( HEAPID_DEBUGの残量もギリギリ )
      gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

      // DrawUtilの拡張バッファをデバッグ用として初期化
      m_pAllocatorForDebugDrawUtil = GFL_NEW( pHeap ) nijiAllocator( pHeap, nijiAllocator::RESIDENT );
      gfl2::gfx::DrawUtil::InitializeExtension( m_pAllocatorForDebugDrawUtil, 4000, 200 );
    }
		{
			gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG );
			m_pAllocatorForDebug = GFL_NEW(pHeap) nijiAllocator(pHeap, nijiAllocator::APPLICATION);
		}
  }
#endif

  //-----------------------------------------------------------------------------
  /**
   *    入力システムの初期化
   */
  //-----------------------------------------------------------------------------
  gfl2::ui::Initialize();
#if defined(GF_PLATFORM_CTR)
  m_Singlton.m_pUiDeviceManager = GFL_NEW(residentDeviceHeap) gfl2::ui::CTR_DeviceManager( residentDeviceHeap );
#elif defined(GF_PLATFORM_WIN32)
  m_Singlton.m_pUiDeviceManager = GFL_NEW(residentDeviceHeap) gfl2::ui::WIN32_DeviceManager( residentDeviceHeap, gfl2::gfx::GFGL::GetHwnd() );

#if defined NIJI_WIN32_ENABLE_BUTTON_LAYOUT_CHANGE_TO_3DS
  // @note ゲームパッドのキー配置変更、nijiプロジェクトでは3DSの配置に合わせる事になった　ikuta_junya 150424
  {
    // XとY、AとBを入れ替える
    // 差し替え対象をAssignし、自身をRemoveする事で差し替える
    for( u32 i = 0; i < gfl2::ui::DeviceManager::BUTTON_NUM; i++ )
    {
      gfl2::ui::Button* pButton = m_Singlton.m_pUiDeviceManager->GetButton( static_cast< gfl2::ui::DeviceId >( i ) );
      if( pButton == NULL )
      {
        continue;
      }

      pButton->AddButtonAssignment( gfl2::ui::BUTTONID_X, gfl2::ui::BUTTONID_Y );
      pButton->RemoveButtonAssignment( gfl2::ui::BUTTONID_X, gfl2::ui::BUTTONID_X ) ;

      pButton->AddButtonAssignment( gfl2::ui::BUTTONID_Y, gfl2::ui::BUTTONID_X );
      pButton->RemoveButtonAssignment( gfl2::ui::BUTTONID_Y, gfl2::ui::BUTTONID_Y ) ;

      pButton->AddButtonAssignment( gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_B );
      pButton->RemoveButtonAssignment( gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_A ) ;

      pButton->AddButtonAssignment( gfl2::ui::BUTTONID_B, gfl2::ui::BUTTONID_A );
      pButton->RemoveButtonAssignment( gfl2::ui::BUTTONID_B, gfl2::ui::BUTTONID_B ) ;
    }
  }
#endif // 
#endif

  //-----------------------------------------------------------------------------
  /**
   *   SystemFontの作成 レイアウトの初期化
   */
  //-----------------------------------------------------------------------------
  m_Singlton.m_pSystemFontHeap = GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_SYSTEM_FONT, HeapSize::SYSTEM_FONT_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_Singlton.m_pSystemFontDevHeap = GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_SYSTEM_FONT_DEVICE, HeapSize::SYSTEM_FONT_DEVICE_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_Singlton.m_pSystemFont = GFL_NEW(residentDeviceHeap) print::SystemFont(m_Singlton.m_pSystemFontHeap,m_Singlton.m_pSystemFontDevHeap, IsJpnFontLoadRegion());

	m_Singlton.m_pMemAllocator = GFL_NEW(residentDeviceHeap) gfl2::heap::NwAllocator( residentDeviceHeap );
	m_Singlton.m_pDevAllocator = GFL_NEW(residentDeviceHeap) gfl2::heap::NwAllocator( residentDeviceHeap );
  // LytSys初期化
  gfl2::lyt::LytSys::InitializeSystem( m_Singlton.m_pMemAllocator, m_Singlton.m_pDevAllocator, GFL_SINGLETON_INSTANCE(print::SystemFont)->GetRectDrawerShaderRes() );
  //フォント設定
  print::SystemFont_SetLytSystemDefaultFont();

	//-----------------------------------------------------------------------------
	/**
	*   レンダリングエンジンの初期化
	*/
	//-----------------------------------------------------------------------------
	//全ノードを管理するマネージャ初期化
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::InitDescription desc( 1024 * 8 );
    desc.m_AABBTreeSize = gfl2::math::AABB( gfl2::math::Vector4(0, 0, 0), gfl2::math::Vector4(10240,10240,10240) );
    desc.m_AABBDepth = 3; // 2のm_AABBDepth+1乗の分割数
    gfl2::renderingengine::scenegraph::SceneGraphManager::Initialize(m_pAllocator, desc);
  }

	//リソースノードを生成するマネージャ初期化
	gfl2::renderingengine::scenegraph::resource::ResourceManager::Initialize(m_pAllocator);
	m_pDisplayLists[0] = gfl2::gfx::GFGL::CreateDisplayList(m_pAllocator);
    m_pDisplayLists[1] = gfl2::gfx::GFGL::CreateDisplayList(m_pAllocator);


  //-----------------------------------------------------------------------------
	/**
	*   テクスチャーファクトリーに対して、テクスチャ生成用テンポラリバッファ　確保先を指定
	*/
	//-----------------------------------------------------------------------------
#if defined(GF_PLATFORM_WIN32)
  {
    gfl2::heap::HeapBase* pWin32DebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_WIN32_WORK);
    //!Textureテンポラリー用アロケーター
    m_pTextureFactoryTemporraryAllocator = GFL_NEW(pWin32DebugHeap) nijiAllocator( pWin32DebugHeap, nijiAllocator::RESIDENT );

    const c8* pFactoryName;
    pFactoryName = gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory::GetFactoryTypeName();
    gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory;
    pTextureResourceNodeFactory = reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>( gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory( gfl2::renderingengine::scenegraph::resource::IResourceData::Type( pFactoryName ) ) );
    pTextureResourceNodeFactory->SetTemporaryAllocator( m_pTextureFactoryTemporraryAllocator );
  }
#endif


	//-----------------------------------------------------------------------------
	/**
	*   通信システムの初期化
	*/
	//-----------------------------------------------------------------------------
  {
    m_Singlton.m_pNijiNetworkSystem = GFL_NEW(residentDeviceHeap) NetLib::NijiNetworkSystem( residentDeviceHeap );
  }


  //-----------------------------------------------------------------------------
  /**
  *   動的ヘッダシステムの初期化
  */
  //-----------------------------------------------------------------------------
#if GFL_ENABLE_DYNAMIC_HEADER
  {
	  // ヒープの取得
    gfl2::heap::HeapBase* pParentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEBUGGER );
    gfl2::heap::HeapBase* pHeap = GFL_CREATE_HEAP( pParentHeap, HEAPID_DEBUG_DYNAMIC_HEADER_DEVICE, System::HeapSize::DEBUG_DYNAMIC_HEADER_DEVICE, gfl2::heap::HEAP_TYPE_EXP, false );

    // 動的ヘッダシステムの初期化
    GFL_DYNAMIC_HEADER_MANAGER_INITIALIZE(pHeap, NIJI_DYNAMIC_HEADER_FILEAMOUNTMAX, NIJI_DYNAMIC_HEADER_ALERTLIMIT, &s_dynamicHeaderRootPathConfig);
#if defined(GF_PLATFORM_WIN32)
    //Winのときは常時有効
    gfl2::debug::DynamicHeader::HeaderHashManager::SetIsEnable(true);
#endif //GF_PLATFORM_WIN32
  }
#endif

  //-----------------------------------------------------------------------------
  /**
  *   バトルとフィールドの遷移高速化のためのヒープ確保
  */
  //-----------------------------------------------------------------------------
  {
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_BATTLE_TRANSITION, System::HeapSize::HEAP_SIZE_BATTLE_TRANSITION, gfl2::heap::HEAP_TYPE_EXP, false );
  }
  // バトル中のバトルフォントの確保ヒープ nijiでは常駐
  {
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_BATTLE_FONT_TEMP, 0x220000, gfl2::heap::HEAP_TYPE_EXP, false );
  }
    


  //-----------------------------------------------------------------------------
  /**
   *   AutoStackManager初期化
   */
  //-----------------------------------------------------------------------------
#if defined(GF_PLATFORM_CTR)
  {
    // AutoStackManager用のヒープ k.ohno systemから確保許可済み
    gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
    gfl2::heap::HeapBase* stackHeap = GFL_CREATE_HEAP( systemHeap, HEAPID_AUTO_STACK, System::HeapSize::HEAPSIZE_AUTO_STACK, gfl2::heap::HEAP_TYPE_EXP, false );
//    nn::os::Thread::SetAutoStackManager( GFL_NEW(residentDeviceHeap) gfl2::base::AutoStackManager( stackHeap ) );
  }
#endif //defined(GF_PLATFORM_CTR)

  //-----------------------------------------------------------------------------
  /**
   *   本体リージョンの初期化
   */
  //-----------------------------------------------------------------------------
  InitHardwareRegion();

  //-----------------------------------------------------------------------------
  /**
   *   CharaModelFactory初期化
   */
  //-----------------------------------------------------------------------------

  gfl2::heap::HeapBase* pBattleTransitionHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_BATTLE_TRANSITION);
  m_Singlton.m_pCharaModelHeap = GFL_CREATE_LOCAL_HEAP_NAME( pBattleTransitionHeap, 0x00038000, gfl2::heap::HEAP_TYPE_EXP, false,"CharaResident");

  InitializeCharaModelSystem();

  //-----------------------------------------------------------------------------
  /**
   *  BattleFont常駐
   *  @caution 過去作では必要時にロードし解放していたので、アッパーでメモリが不足したらその対応を検討すること
   */
  //-----------------------------------------------------------------------------
  {
    gfl2::heap::HeapBase* pBattleFontHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_BATTLE_FONT_TEMP);
    gfl2::heap::HeapBase* pTempHeap         = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE); //作業用テンポラリ、即時解放
    print::SystemFont_CreateBattleFont(pTempHeap, pBattleFontHeap);
    print::SystemFont_SetLytSystemBattleFont();
  }

  // NMCat[ 957 ] QRコードスキャナを起動中にMiiverseへ投稿すると、カメラ画像が投稿可能になっています
  //sango追加
  //-----------------------------------------------------------------------------
  /**
   *    Miiverce写真投稿アプリの初期設定
   */
  //-----------------------------------------------------------------------------
  //デフォルトOFF。ASSERT回避のため、trueで初期化して設定関数を通す。
  m_Singlton.mScreenCapturePostPermission = true;
  SetScreenCapturePostPermission(false);


  //-----------------------------------------------------------------------------
  /**
   *    ヒープの余り
   */
  //-----------------------------------------------------------------------------
#if PM_DEBUG
  {
#if PM_DEBUG_PRINT
    gfl2::heap::HeapBase *systemH = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
    gfl2::heap::HeapBase *deviceH = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
    gfl2::heap::HeapBase *residentH = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT );
    gfl2::heap::HeapBase *residentDeviceH = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );
    gfl2::heap::HeapBase *appH = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP );
    gfl2::heap::HeapBase *appDeviceH = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
    GFL_PRINT( "======================================\n" );
    GFL_PRINT( "INIT END SysHeap  Free Size 0x%08x\n", systemH->GetTotalFreeSize() );
    GFL_PRINT( "INIT END SysDev   Free Size 0x%08x\n", deviceH->GetTotalFreeSize() );
    GFL_PRINT( "INIT END ResHeap  Free Size 0x%08x\n", residentH->GetTotalFreeSize() );
    GFL_PRINT( "INIT END ResDev   Free Size 0x%08x\n", residentDeviceH->GetTotalFreeSize() );
    GFL_PRINT( "INIT END AppHeap  Free Size 0x%08x\n", appH->GetTotalFreeSize() );
    GFL_PRINT( "INIT END AppDev   Free Size 0x%08x\n", appDeviceH->GetTotalFreeSize() );
    GFL_PRINT( "======================================\n" );
#endif //PM_DEBUG_PRINT

    GFL_PRINT( "======================================\n" );
    nijiAllocator::DebugVramMemoryInfoDump();
    nijiAllocator::DebugSetResidentVramCheck();
    GFL_PRINT( "======================================\n" );
  }
#endif //PM_DEBUG
}


//==============================================================================
/**
 *  @brief  常駐GFLIBシステムの破棄
 */
//==============================================================================
void GflUse::Finalize( void )
{
  //-----------------------------------------------------------------------------
  // 常駐バトルフォント解放
  //-----------------------------------------------------------------------------
   GFL_SINGLETON_INSTANCE(print::SystemFont)->DeleteBattleFont();

  //-----------------------------------------------------------------------------
  /**
   *   CharaModelFactory破棄
   */
  //-----------------------------------------------------------------------------
  TerminateCharaModelSystem();
  GFL_DELETE_HEAP(m_Singlton.m_pCharaModelHeap);

  //-----------------------------------------------------------------------------
  /**
  *   動的ヘッダシステムの破棄
  */
  //-----------------------------------------------------------------------------
#if GFL_ENABLE_DYNAMIC_HEADER
  {
    // 動的ヘッダシステムの終了処理
    GFL_DYNAMIC_HEADER_MANAGER_FINALIZE();
    // ヒープの開放
    GFL_DELETE_HEAP (gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DYNAMIC_HEADER_DEVICE ));
  }
#endif
	//-----------------------------------------------------------------------------
	/**
	*   通信システムの破棄
	*/
	//-----------------------------------------------------------------------------
  GFL_SAFE_DELETE( m_Singlton.m_pNijiNetworkSystem );

	//-----------------------------------------------------------------------------
	/**
	*   レンダリングエンジンの破棄
	*/
	//-----------------------------------------------------------------------------
#if PM_DEBUG
  gfl2::gfx::DrawUtil::TerminateExtension();
#endif
  gfl2::gfx::DrawUtil::Terminate();


#if defined(GF_PLATFORM_WIN32)
  //!Textureテンポラリー用アロケーター
  GFL_SAFE_DELETE( m_pTextureFactoryTemporraryAllocator );
#endif 

  //-----------------------------------------------------------------------------
  /**
   *   SystemFont & layout の廃棄
   */
  //-----------------------------------------------------------------------------

  gfl2::lyt::LytSys::FinalizeSystem();
  GFL_SAFE_DELETE( m_Singlton.m_pDevAllocator );
  GFL_SAFE_DELETE( m_Singlton.m_pMemAllocator );

  GFL_SAFE_DELETE( m_Singlton.m_pSystemFont );

  //-----------------------------------------------------------------------------
  /**
   *    乱数の破棄
   */
  //-----------------------------------------------------------------------------
  GFL_SAFE_DELETE(m_Singlton.m_pRandom);

  //-----------------------------------------------------------------------------
  /**
   *    システムイベントマネージャの破棄
   */
  //-----------------------------------------------------------------------------
  System::HomeSleepAndPowerSystem::SetSystemEventManager( NULL );
  GFL_SAFE_DELETE( m_Singlton.m_pSystemEventManager );

  //-----------------------------------------------------------------------------
  /**
   *    サウンドシステムの破棄
   */
  //-----------------------------------------------------------------------------
  Sound::Finalize();

  //-----------------------------------------------------------------------------
  /**
   *    描画システムの破棄
   */
  //-----------------------------------------------------------------------------
  //deleteGraphicsSystem();

  //-----------------------------------------------------------------------------
  /**
   *    入力システムの破棄
   */
  //-----------------------------------------------------------------------------
  GFL_SAFE_DELETE(m_Singlton.m_pUiDeviceManager);
  gfl2::ui::Finalize();

  //-----------------------------------------------------------------------------
  /*
   * 言語管理クラスを生成
   */
  //-----------------------------------------------------------------------------
  GFL_SAFE_DELETE( m_Singlton.m_pLanguage );

  //-----------------------------------------------------------------------------
  /*
   * 拡張セーブデータの終了
   */
  //-----------------------------------------------------------------------------
  GFL_SAFE_DELETE( m_Singlton.m_pNijiExtSaveSystem );

  //-----------------------------------------------------------------------------
  /**
   *    Fileシステムの破棄
   */
  //-----------------------------------------------------------------------------
  FileSystemTerminate();

  //-----------------------------------------------------------------------------
  /**
   *   スレッド管理
   */
  //-----------------------------------------------------------------------------
  //  gfl::base::Thread::FinalizeThreadManagement();


  //-----------------------------------------------------------------------------
	/**
	*   アロケーターの解放
	*/
	//-----------------------------------------------------------------------------
  nijiAllocator::Terminate();
	GFL_DELETE m_pAllocator;
  
  //-----------------------------------------------------------------------------
  /*
   *    ヒープの開放
   */
  //-----------------------------------------------------------------------------
  CommonHeapFree();

#if defined(VALID_ASSEART_APPLET)
  {
    // スレッド内アサートメッセージバッファ用クリティカルセクション終了
    m_pCriticalSectionForAssertMsgBuffer.Finalize();
  }
#endif

	//-----------------------------------------------------------------------------
	/*
	*    ディスプレイの開放
	*/
	//-----------------------------------------------------------------------------
	for (u32 i = 0; i < gfl2::gfx::CtrDisplayNo::NumberOf; ++i)
  {
		gfl2::gfx::GFGL::DestroyDisplay(m_HandleList[i]);
  }
	gfl2::gfx::GFGL::Terminate();
}


//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックスシステムの実行が完了してるかチェックする。
 */
//-----------------------------------------------------------------------------
bool GflUse::IsGpuIdle( void )
{
#if 0
  if(m_Singlton.m_pGraphicsSystem){
    gfl::grp::CommandListManager * pCommandList = m_Singlton.m_pGraphicsSystem->GetCommandListManager();
    return !(pCommandList->IsRuning());
  }
#endif
  return true;
}

//------------------------------------------------------------------
/**
  * @brief   ソフトリセットリクエスト確認
  * @return リクエストがあると１になる
  */
//------------------------------------------------------------------
bool GflUse::IsSoftResetRequest(void)
{
#if defined(GF_PLATFORM_CTR)
  gfl2::ui::Button* pButton = m_Singlton.m_pUiDeviceManager->GetButton(gfl2::ui::CTR_DeviceManager::BUTTON_STANDARD);
  u32 key = pButton->GetHold(gfl2::ui::Button::INPUT_STATE_ORIGINAL);
  if( key ==  (gfl2::ui::BUTTON_L|gfl2::ui::BUTTON_R|gfl2::ui::BUTTON_START) ){
    return true;
  }
#endif

  if( m_Singlton.SoftResetFlg )
  {
    return true;
  }

  return false;
}

//------------------------------------------------------------------
/**
  * @brief   ソフトリセットリクエスト
  * @caution  必ず成功するので気を付けて 特にセーブが終わってから呼ぶこと
  */
//------------------------------------------------------------------
void GflUse::SetSoftResetRequest(void)
{
  m_Singlton.SoftResetFlg = true;
}


//==============================================================================
/**
 *  @brief  常駐GFLIBシステムの更新(ProcUpdate前)
 */
//==============================================================================
void GflUse::UpdateBeforeProc( void )
{
  // UI(入力デバイス)の更新 
  m_Singlton.m_pUiDeviceManager->Main();
  // 時間機能の更新
  gfl2::system::Date::Update();

  m_Singlton.m_pNijiNetworkSystem->UpdateBefore();

  // ファイル読み込み管理の更新
  //m_Singlton.m_pFileReadManager->UpdateFileRead();
  //m_Singlton.m_pUiDeviceManager->Main();
}

//==============================================================================
/**
 *  @brief  常駐GFLIBシステムの更新(ProcUpdate後)
 */
//==============================================================================
void GflUse::UpdateAfterProc( void )
{
  // UI(入力デバイス)の更新 
  m_Singlton.m_pUiDeviceManager->SynchronizeInputUpdateForAllDevice();
  m_Singlton.m_pUiDeviceManager->Main();
  m_Singlton.m_pNijiNetworkSystem->UpdateAfter();

#if defined(VALID_ASSEART_APPLET)
  // サブスレッドで発生したアサートを表示
  if( m_pAssertMsgBuffer )
  {
    NetLib::Error::NijiNetworkErreulaApplet::ErrorDispChar( m_pAssertMsgBuffer );
    m_pAssertMsgBuffer = NULL;
  }
#endif
}

//==============================================================================
/**
 *  @brief  アプレット更新
 */
//==============================================================================
void GflUse::UpdateAppletSystem( void )
{
  m_Singlton.m_pNijiNetworkSystem->UpdateErrorApplet();
}

//==============================================================================
//==============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  常駐GFLIBシステム　Vsync待ち処理
 */
//-----------------------------------------------------------------------------
void GflUse::VSyncWait( void )
{
#if 0
  if(m_Singlton.m_pGraphicsSystem){
    m_Singlton.CPUframeOverFlag = false;    // 処理落ちフラグ

    {
      // 処理落ち時にVSyncWait
      // をするべきか？
      s32 vcount = nngxCheckVSync(NN_GX_DISPLAY_BOTH);
      if( m_Singlton.m_LastVCount >= vcount )
      {
        //// 処理落ちしていない場合。 ////
        // VSync待ち
        NN_PROF_RecordWaitForVBlank();
        gfl::grp::GraphicsSystem::WaitVSync();
      }
      else{
        //// 処理落ちした場合。 ////
        m_Singlton.CPUframeOverFlag = true;    // 処理落ちフラグ
        // SwapBufferされたときだけ、確実に描画する。
        if( m_Singlton.m_pGraphicsSystem->IsSwapBuffer() ){
          NN_PROF_RecordWaitForVBlank();
          gfl::grp::GraphicsSystem::WaitVSync();
        }
      }
    }
  } else {
    // グラフィックシステムを持っていない場合は、そちらにVSyncWaitを内包させるようにする
    //nngxWaitVSync(NN_GX_DISPLAY_BOTH);
  }
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  常駐GFLIBシステム　Vsync処理
 */
//-----------------------------------------------------------------------------
void GflUse::VSync( void )
{
#if 0
  if(m_Singlton.m_pGraphicsSystem)
  {
    {
      if(m_Singlton.CPUframeOverFlag == false){    // 処理落ちフラグ
        //// 処理落ちしていない場合。 ////
        m_Singlton.m_pGraphicsSystem->VSyncAfter();
      } else {
        //// 処理落ちした場合。 ////
        // SwapBufferされたときだけ、確実に描画する。
        if( m_Singlton.m_pGraphicsSystem->IsSwapBuffer() ){
          m_Singlton.m_pGraphicsSystem->VSyncAfter();
        }
      }
    }
    m_Singlton.m_LastVCount = nngxCheckVSync(NN_GX_DISPLAY_BOTH);
  }
#endif
}




//-----------------------------------------------------------------------------
/**
 *    singletonライブラリを管理するメンバ関数郡
 */
//-----------------------------------------------------------------------------
#if 0
//----------------------------------------------------------------------------
/**
 *  @brief  タイトル用としてグラフィックスシステムを初期化　メモリは下からとる。
 *
 *  @param  p_heap      ヒープ
 *  @param  p_device    デバイス
 */
//-----------------------------------------------------------------------------
void GflUse::createGraphicsSystemTitle( void )
{
  gfl::heap::HeapBase* p_heap = gfl::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT );
  gfl::heap::HeapBase* p_heapDevice = gfl::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );
  gfl::math::VEC4 fadefill_color( 0.0f, 0.0f, 0.0f, 1.0f );

  createGraphicsSystem( m_DefaultGraphicsSetUp, DEFAULT_GRAPHICS_SYSTEM_CONTROL_VRAMA_SIZE, DEFAULT_GRAPHICS_SYSTEM_CONTROL_VRAMB_SIZE, p_heap, p_heapDevice, false, fadefill_color );
}

//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックスシステムの初期化
 *
 *  @param  setup       セットアップ
 *  @param  vramA       VRAM-A
 *  @param  vramB       VRAM-B
 *  @param  p_heap      ヒープ
 *  @param  p_device    デバイス
 *  @param  heap_low    ヒープの下から確保するか？
 *  @param  color       フェードバッファのクリアカラー
 */
//-----------------------------------------------------------------------------
void GflUse::createGraphicsSystem( const gfl::grp::GraphicsSystemSetUp& setup, u32 vramA, u32 vramB, gfl::heap::HeapBase* p_heap, gfl::heap::HeapBase* p_device, bool heap_low, const gfl::math::VEC4& color )
{
  // 下からとるか？
  if( heap_low ){
    TOMOYA_PRINT( "createGraphicsSystem heap Low\n" );
    p_heap    = p_heap->GetLowerHandle();
    p_device  = p_device->GetLowerHandle();
  }
#if PM_DEBUG
  else{
    TOMOYA_PRINT( "createGraphicsSystem heap normal\n" );
  }
#endif // PM_DEBUG

  m_Singlton.m_pGraphicsHeapMemAllocator    = GFL_NEW( p_heap ) gfl::heap::NwAllocator( p_heap );
  m_Singlton.m_pGraphicsDeviceMemAllocator  = GFL_NEW( p_heap ) gfl::heap::NwAllocator( p_device );

  m_Singlton.m_pGraphicsSystem = GFL_NEW( p_heap ) gfl::grp::GraphicsSystem(
      m_Singlton.m_pGraphicsHeapMemAllocator,
      m_Singlton.m_pGraphicsDeviceMemAllocator,
      &setup,
      true,     // アプリケーション側で立体視のバッファを確保
      vramA,
      vramB );

  SystemFont_Initialize();

  gfl::grp::util::DrawUtil::Initialize(
          m_Singlton.m_pGraphicsSystem ,
          m_Singlton.m_pGraphicsHeapMemAllocator,
          m_Singlton.m_pGraphicsDeviceMemAllocator );
  gfl::grp::util::DrawUtil::SetFont( SystemFont_GetFont() );

  // LytSys初期化
  gfl::grp::g2d::LytSys::InitializeSystem( m_Singlton.m_pGraphicsHeapMemAllocator, m_Singlton.m_pGraphicsDeviceMemAllocator );
  //フォント設定
  SystemFont_SetLytSystemDefaultFont();
  //gfl::grp::g2d::LytSys::SetDefaultFont( SystemFont_GetFont( ), print::LAYOUT_DEFAULT_FONT, DEFAULT_FONT_IDX_KUJIRA_DEFAULT );

  // フェード
  gfl::fs::ArcFile* af = GFL_NEW_LOW(m_Singlton.m_pGraphicsHeapMemAllocator->GetHeapBase()) gfl::fs::ArcFile(
      m_Singlton.m_pGraphicsHeapMemAllocator->GetHeapBase()->GetLowerHandle(),
      ARCID_FADE,
      gfl::fs::ArcFile::OPEN);
  u32 size = af->GetDataSize(0);
  GFL_PRINT("FadeResSize[%d]\n", size);
  m_Singlton.m_pFadeRes = GflHeapAllocMemoryAlign(
      m_Singlton.m_pGraphicsDeviceMemAllocator->GetHeapBase(),
      size,
      128);
  af->LoadData(
      0,
      m_Singlton.m_pFadeRes);
  m_Singlton.m_pFade = GFL_NEW(p_device) gfl::grp::Fade(
      m_Singlton.m_pGraphicsHeapMemAllocator,
      m_Singlton.m_pGraphicsDeviceMemAllocator,
      m_Singlton.m_pGraphicsSystem,
      m_Singlton.m_pFadeRes);
  GFL_DELETE af;
  m_Singlton.m_pFade->ForceOut( gfl::grp::Fade::DISP_MAX, &color );

  // HOMEボタン禁止マーク
  m_Singlton.m_pHomeNix = GFL_NEW(p_heap) System::HomeNixDisp();
  m_Singlton.m_pHomeNix->Initialize();

  // Kujiraでは、常にコマンドリストの実行時間をチェックする必要がある。
  // そのため、製品版でもこのデバッグ機能は使用する。
  // コマンドリストのデバッグ機能ON
  gfl::grp::CommandListManager::DebugSetFlag( true );

#ifdef DEBUG_ONLY_FOR_takahashi_tomoya
  GFL_PRINT( "System VRAMA 0x%x  VRAMB 0x%x\n", m_Singlton.m_pGraphicsSystem->GetVramAAllocatableSize(), m_Singlton.m_pGraphicsSystem->GetVramBAllocatableSize() );

  GFL_PRINT( "ResidentHeap Dump\n" );
  p_heap->Dump();

  GFL_PRINT( "ResidentDeviceHeap Dump\n" );
  p_device->Dump();
#endif

}

//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックスシステムの破棄
 */
//-----------------------------------------------------------------------------
void GflUse::deleteGraphicsSystem( void )
{
  if(m_Singlton.m_pGraphicsSystem){
#if 0
    m_Singlton.m_pHomeNix->Finalize();
    GFL_DELETE m_Singlton.m_pHomeNix;
    GFL_DELETE m_Singlton.m_pFade;
    GFL_DELETE m_Singlton.m_pFadeRes;

    // LytSys破棄
    gfl::grp::g2d::LytSys::FinalizeSystem();

    gfl::grp::util::DrawUtil::Terminate();
    SystemFont_Terminate();
#endif
    GFL_DELETE m_Singlton.m_pGraphicsSystem;
    m_Singlton.m_pGraphicsSystem = NULL;

    GFL_DELETE m_Singlton.m_pGraphicsDeviceMemAllocator;
    m_Singlton.m_pGraphicsDeviceMemAllocator = NULL;
    GFL_DELETE m_Singlton.m_pGraphicsHeapMemAllocator;
    m_Singlton.m_pGraphicsDeviceMemAllocator = NULL;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @return
 */
//-----------------------------------------------------------------------------
bool GflUse::isCreateGraphicsSystem( void )
{
  // グラフィックスシステムが常駐しているかチェック
  if(m_Singlton.m_pGraphicsSystem){
    return true;
  }
  return false;
}
#endif


//----------------------------------------------------------------------------
/**
 *  @brief  コモンヒープ作成
 */
//-----------------------------------------------------------------------------
void GflUse::CommonHeapCreate(void)
{
  //Resident初期化
  Main::ResidentHeapCreate();

  gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  gfl2::heap::HeapBase *deviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
  gfl2::heap::HeapBase *residentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT );
  gfl2::heap::HeapBase *residentDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );

  // デバッグ用メモリの割り当て
#if PM_DEBUG
  {
  #if defined(GF_PLATFORM_WIN32)
    size_t debugDeviceHeapSize = System::HeapSize::DEBUG_DEVICE;

  #elif defined(GF_PLATFORM_CTR)
    const size_t assingment   = nn::os::GetAppMemorySize();
    size_t debugDeviceHeapSize = 0x0;

    switch(assingment){
    case 0xB200000: //Snake dev1=178M
      debugDeviceHeapSize = System::HeapSize::DEBUG_DEBUGGER_178M; //
      break;

    case 0x7C00000: //Snake dev2=124M
      debugDeviceHeapSize = System::HeapSize::DEBUG_DEBUGGER_124M; //
      break;
    case 0x6000000: //CTR dev1=96M
      debugDeviceHeapSize = System::HeapSize::DEBUG_DEBUGGER_96M;
      break;

    // @note 中国語対応により、CTR dev2はRelease or Final でしか動作しなくなる。
    case 0x5000000: //CTR dev2
      debugDeviceHeapSize = System::HeapSize::DEBUG_DEBUGGER_80M;
      break;
    default:
      GFL_PRINT( "SystemMode CTR prod or ????\n" );
      GFL_ASSERT_STOP_MSG(0, "SystemMode CTR prod or ????\n" );
      break;
    }
  #endif  //defined(GF_PLATFORM_CTR)
    gfl2::heap::HeapBase* pDebugHeap = GFL_CREATE_HEAP( deviceHeap , HEAPID_DEBUG ,System::HeapSize::DEBUG_HEAP, gfl2::heap::HEAP_TYPE_EXP , true );
    GFL_CREATE_HEAP( systemHeap , HEAPID_DEBUG_DLL_LOAD, System::HeapSize::DEBUG_DLL_LOAD + 0x1C000, gfl2::heap::HEAP_TYPE_EXP , true );

    // Debugger及び、ROMでのみ実行
    if( debugDeviceHeapSize > 0 )
    {
      GFL_CREATE_HEAP( deviceHeap, HEAPID_DEBUG_DEBUGGER, debugDeviceHeapSize, gfl2::heap::HEAP_TYPE_EXP , true );
    }
  }
#endif //PM_DEBUG

#if defined(GF_PLATFORM_CTR)   ///  MobiClip用にMemoryBlockを確保 システムから確保許可済み k.ohno
  if( !s_memBlockPtr )
  {
    size_t autoStackSize = 4096*8;
    s_memBlockPtr = (uptr)GflHeapAllocMemory(systemHeap, autoStackSize);
    nn::os::InitializeMemoryBlock( s_memBlockPtr, autoStackSize );
  }
#endif

  // 常駐メモリの作成
  {
    // ファイル読み込み
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_FILEREAD, System::HeapSize::FILE_READ, gfl2::heap::HEAP_TYPE_EXP, false );
    // EventDeviceメモリ      parent::residentHeap
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_EVENT_DEVICE,System::HeapSize::EVENT_DEVICE, gfl2::heap::HEAP_TYPE_EXP, false );
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_EVENT,System::HeapSize::EVENT_HEAP, gfl2::heap::HEAP_TYPE_EXP, false );
    // 動的モジュール読み込み
    TOMOYA_PRINT( "DLLLoad heapSize 0x%x\n", System::HeapSize::DLL_LOAD );
    GFL_CREATE_HEAP( systemHeap, HEAPID_DLL_LOAD, System::HeapSize::DLL_LOAD, gfl2::heap::HEAP_TYPE_EXP, false );
  }

  {
    // Appメモリ      parent::systemHeap
    GFL_CREATE_HEAP( systemHeap, HEAPID_APP,System::HeapSize::APP, gfl2::heap::HEAP_TYPE_EXP, false );
    // AppDeviceメモリ  parent::deviceHeap
    GFL_CREATE_HEAP( deviceHeap, HEAPID_APP_DEVICE,System::HeapSize::APP_DEVICE, gfl2::heap::HEAP_TYPE_EXP, false );
  }
  {
    // Soundメモリ      parent::residentHeap
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_SOUND,System::HeapSize::HEAP_SOUND, gfl2::heap::HEAP_TYPE_EXP, false );
    // SondDeviceメモリ  parent::residentDeviceHeap
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_SOUND_DEVICE,System::HeapSize::HEAP_SOUND_DEVICE, gfl2::heap::HEAP_TYPE_EXP, false );    
  }
  {
    // 通信メモリ
    GFL_CREATE_HEAP( residentHeap, HEAPID_NETWORK_RESIDENT,System::HeapSize::NETWORK_RESIDENT, gfl2::heap::HEAP_TYPE_EXP, false );
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_ERROR_APPLET_SYSTEM,System::HeapSize::ERROR_APPLET_SYSTEM, gfl2::heap::HEAP_TYPE_EXP, false );
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_ERROR_APPLET_MESSAGE,System::HeapSize::ERROR_APPLET_MESSAGE, gfl2::heap::HEAP_TYPE_EXP, false );
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_JOIN_FESTA_PERSONAL,System::HeapSize::JOIN_FESTA_PERSONAL, gfl2::heap::HEAP_TYPE_EXP, false );
  }
  {
    //セーブデータクラス用メモリ
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_SAVE_DATA , System::HeapSize::SAVE_DATA_HEAP_SIZE , gfl2::heap::HEAP_TYPE_EXP, false );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  コモンヒープ開放
 */
//-----------------------------------------------------------------------------
void GflUse::CommonHeapFree(void)
{
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_SAVE_DATA ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_ERROR_APPLET_SYSTEM ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_ERROR_APPLET_MESSAGE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_JOIN_FESTA_PERSONAL ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_SOUND_DEVICE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_SOUND ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT ));
#if PM_DEBUG
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEBUGGER ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DLL_LOAD ));
  GflHeapSafeFreeMemory( m_pBufferForDebugDrawUtil );
  GFL_SAFE_DELETE( m_pAllocatorForDebugDrawUtil );
  GFL_SAFE_DELETE( m_pAllocatorForDebug );
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ));
#endif

  //Resident開放
  Main::ResidentHeapFree();
}

//----------------------------------------------------------------------------
/**
 *  @brief  アサート設定 @note 入力処理
 */
//-----------------------------------------------------------------------------
void GflUse::AssertSetup( void )
{
  //
  // ASSERTループ内コールバック関数設定
  //
  gfl2::debug::GFL_ASSERT_SetLoopFunc(AssertLoopFunc);

#if defined(VALID_ASSEART_APPLET)

  //
  // アプレット起動判定
  //

  // @note この関数をマスター提出する ROM に使用することはできません。
  switch( nn::os::CTR::GetRunningTargetHardware() )
  {
  case nn::os::CTR::TARGET_HARDWARE_KMC_DEBUGGER:       //!< PARTNER-CTR Debugger を表します。
  case nn::os::CTR::TARGET_HARDWARE_KMC_CAPTURE:        //!< PARTNER-CTR Capture を表します。
  case nn::os::CTR::TARGET_HARDWARE_IS_DEBUGGER:        //!< IS-CTR-DEBUGGER を表します。
  case nn::os::CTR::TARGET_HARDWARE_SNAKE_IS_DEBUGGER:  //!< IS-SNAKE-BOX を表します。
  case nn::os::CTR::TARGET_HARDWARE_SNAKE_IS_CAPTURE:   //!< IS-SNAKE-BOX デバッグ機能なし版を表します。
  case nn::os::CTR::TARGET_HARDWARE_SNAKE_KMC_DEBUGGER: //!< PARTNER-SNAKE Debugger を表します。
  case nn::os::CTR::TARGET_HARDWARE_TS_BOARD:           //!< TS ボードを表します。
    {
      // デバッガ接続時はプリント表示
      m_IsAssertCallApplet = false;
      break;
    }
  case nn::os::CTR::TARGET_HARDWARE_PRODUCT:            //!< 製品同等のハードウェアを表します。
  case nn::os::CTR::TARGET_HARDWARE_SNAKE_PRODUCT:      //!< SNAKE 製品版と同等のハードウェアを表します。
    {
      // 開発実機ではアプレット表示
      m_IsAssertCallApplet = true;
      break;
    }
  
  case nn::os::CTR::TARGET_HARDWARE_UNKNOWN:            //!< 不明なハードウェアを表します。
  default:
    {
      // 不明、デバッガ非接続で無表示だと困るので、アプレット表示を試みる
      m_IsAssertCallApplet = true;

      // @attention 通信モジュールの初期化が終わっていないため、ここで実行するとハングする
      // GFL_ASSERT_MSG( 0, "TargetHardware Unknown(%d)\n", nn::os::CTR::GetRunningTargetHardware() );
      break;
    }
  }
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  ファイルシステム初期化
 */
//-----------------------------------------------------------------------------
void GflUse::FileSystemInitialize(gfl2::heap::HeapBase* heap)
{
  const u32 ENV_BUF_SIZE  = 512;
  const u32 PATH_BUF_SIZE = 512;

  gfl2::fs::Result            res;
  char                        env[ENV_BUF_SIZE]   = { 0 };
  char                        path[PATH_BUF_SIZE] = { 0 };
  gfl2::fs::AsyncFileManager* romAsyncFileManager = NULL;

  gfl2::heap::HeapBase* heapDebugFsGasc                = NULL;
  gfl2::heap::HeapBase* heapDebugFsLog                 = NULL;
  gfl2::heap::HeapBase* heapDebugFsHioInitializeDevice = NULL;  // これだけは必ずデバイスメモリで。
  gfl2::heap::HeapBase* heapDebugFsHio                 = NULL;
  gfl2::heap::HeapBase* heapDebugFsCaller              = NULL;
  gfl2::heap::HeapBase* heapDebugFsDirectSdmc          = NULL;
  gfl2::heap::HeapBase* heapDebugFsMatchCheck          = NULL;
  gfl2::heap::HeapBase* heapDebugFsIntervalCheck       = NULL;

  m_Singlton.m_romAsyncFileManager = NULL;
  m_Singlton.m_romArcFileTable     = NULL;
#if PM_DEBUG
  m_Singlton.m_hioArcFileTable      = NULL;
  m_Singlton.m_arcSrcFileTable      = NULL;
  m_Singlton.m_directSdmcArcFileTable  = NULL;
#endif

  // CTRのHIO
  bool hioEnableFlag = false;


#if PM_DEBUG
  ////////////////////////////////////////
  // (0) メモリ確保
  ////////////////////////////////////////
  {
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG ) )
    {
      gfl2::heap::HeapBase* parentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
      heapDebugFsLog                   = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_LOG                , System::HeapSize::DEBUG_FS_LOG                , gfl2::heap::HEAP_TYPE_EXP, false );
      heapDebugFsCaller                = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_CALLER             , System::HeapSize::DEBUG_FS_CALLER             , gfl2::heap::HEAP_TYPE_EXP, false );
      heapDebugFsDirectSdmc            = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_DIRECT_SDMC        , System::HeapSize::DEBUG_FS_DIRECT_SDMC        , gfl2::heap::HEAP_TYPE_EXP, false );
      heapDebugFsMatchCheck            = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_MATCH_CHECK        , System::HeapSize::DEBUG_FS_MATCH_CHECK        , gfl2::heap::HEAP_TYPE_EXP, false );

    }
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_DEBUGGER ) )
    {
      gfl2::heap::HeapBase* parentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEBUGGER );
      heapDebugFsGasc                  = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_GASC                 , System::HeapSize::DEBUG_FS_GASC                 , gfl2::heap::HEAP_TYPE_EXP, false );
      heapDebugFsHio                   = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_HIO                  , System::HeapSize::DEBUG_FS_HIO                  , gfl2::heap::HEAP_TYPE_EXP, false );
      heapDebugFsHioInitializeDevice   = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_HIO_INITIALIZE_DEVICE, System::HeapSize::DEBUG_FS_HIO_INITIALIZE_DEVICE, gfl2::heap::HEAP_TYPE_EXP, false );
      heapDebugFsIntervalCheck         = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_INTERVAL_CHECK       , System::HeapSize::DEBUG_FS_INTERVAL_CHECK       , gfl2::heap::HEAP_TYPE_EXP, false );
    }
  }
#endif


  ////////////////////////////////////////
  // (1) ファイルシステムの初期化処理
  ////////////////////////////////////////
  res = gfl2::fs::System::Initialize(heapDebugFsHioInitializeDevice, heapDebugFsLog);

  // CTRのHIO
  res = gfl2::fs::System::GetHioEnableFlag(&hioEnableFlag);


#if defined(GF_PLATFORM_CTR)
  ////////////////////////////////////////
  // (2) デバイスのマウント
  ////////////////////////////////////////
  const size_t    maxFile       = 4096;
  const size_t    maxDirectory  = 128;
  const bool      useCache      = true;
  res = gfl2::fs::System::MountCtrRomDevice(heap, "rom:", "/", maxFile, maxDirectory, useCache);  // rsfファイルで設定しているところを起点に指定する
  // romfilesが
  // R:\home\niji\git_program\gflib2\SystemTest\roapp_template\project\romfiles
  // のとき、
  // "rom:/data"は
  // R:\home\niji\git_program\gflib2\SystemTest\roapp_template\project\romfiles\data
  // を指している。

#if PM_DEBUG
  // CTRのHIO
  {
    if(hioEnableFlag)
    {
      if(heapDebugFsHio)
      {
        res = gfl2::fs::PcUtil::GetProcessEnvironmentVariable(NULL, env, ENV_BUF_SIZE, "NIJI_ENVIRONMENT_ROOT");  // CTRからはPCに設定されている環境変数しか取れない。batで設定したものは取れない。
        sprintf(path, "%s/../data_ctr", env);
        res = gfl2::fs::System::MountCtrHioRomDevice(heapDebugFsHio, path);
      }
    }
  }

  // ArcSrcファイル
  {
    if(hioEnableFlag)  // CTRのHIO
    {
      if(heapDebugFsGasc)
      {
        // ArcSrcデバイスのマウント
        sprintf(path, "%s/../arc/arc_ctr", env);  // envはhioEnableFlagが真なら、gfl2::fs::System::MountCtrHioRomDevice関数を呼ぶ前に設定されている。
        gfl2::fs::System::MountCtrArcSrcDevice(heapDebugFsGasc, path);
      }
    }
  }
#endif

  ////////////////////////////////////////
  // (3) ファイルマネージャ生成
  ////////////////////////////////////////
  res = gfl2::fs::System::CreateAsyncFileManagerForCtrRomDevice(
    heap, 
    System::ThreadPriority::ROM_ASYNC_FILE_MANAGER_FILE_THREAD_PRIORITY,
    System::ThreadPriority::ROM_ASYNC_FILE_MANAGER_OTHER_THREAD_PRIORITY,
    480
  );
  romAsyncFileManager = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  // arcファイルリストを設定する
  {
#include <arc_out/arc_file_utf8.h>
    romAsyncFileManager->SetArcFileNameTable( gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, heap, ArchiveFileTable, sizeof(ArchiveFileTable)/sizeof(ArchiveFileTable[0]), 0, NULL );
  }

#if PM_DEBUG
  // CTRのHIO
  {
    if(hioEnableFlag)
    {
      if(heapDebugFsHio)
      {
        // この時点でHIOでPC上の通常ファイル(garcでないファイル)は読める。
        m_Singlton.m_hioArcFileTable = GFL_NEW(heapDebugFsHio) gfl2::fs::NameTable();
        m_Singlton.m_hioArcFileTable->SyncCreateTable(
          romAsyncFileManager,
          "../arc/arc_out/hio_arc_file.bin",  // CTRのHIO版は短縮名ファイルを使用しない
          heapDebugFsHio,      
          heapDebugFsHio->GetLowerHandle(),      
          NULL,
          gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_HIO
        );
        romAsyncFileManager->SetArcFileNameTable( gfl2::fs::AsyncFileManager::ArcFileNameTableType::HIO, heapDebugFsHio, const_cast<const char**>(m_Singlton.m_hioArcFileTable->GetNameTable()), m_Singlton.m_hioArcFileTable->GetNameNum(), 0, "../arc/arc_ctr/" );
      }
    }
  }
#endif

#elif defined(GF_PLATFORM_WIN32)
  ////////////////////////////////////////
  // (2) デバイスのマウント
  ////////////////////////////////////////
  res = gfl2::fs::PcUtil::GetProcessEnvironmentVariable(NULL, env, ENV_BUF_SIZE, "NIJI_ENVIRONMENT_ROOT");  // CTRと合わせるためにNIJI_ROOT_DIRを使うのはやめた。
  sprintf_s(path, PATH_BUF_SIZE, "%s/../data_win32", env);  // 絶対パスで指定する
  res = gfl2::fs::System::MountWin32RomDevice(heap, path);

#if PM_DEBUG
  // ArcSrcファイル
  {
    if(heapDebugFsGasc)
    {
      // ArcSrcデバイスのマウント
      sprintf_s(path, PATH_BUF_SIZE, "%s/../arc/arc_win32", env);
      gfl2::fs::System::MountWin32ArcSrcDevice(heapDebugFsGasc, path);
    }
  }
#endif

  ////////////////////////////////////////
  // (3) ファイルマネージャ生成
  ////////////////////////////////////////
  res = gfl2::fs::System::CreateAsyncFileManagerForWin32RomDevice(
    heap, 
    System::ThreadPriority::ROM_ASYNC_FILE_MANAGER_FILE_THREAD_PRIORITY,
    System::ThreadPriority::ROM_ASYNC_FILE_MANAGER_OTHER_THREAD_PRIORITY,
    480
  );
  romAsyncFileManager = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  // arcファイルリストを設定する
  {
    m_Singlton.m_romArcFileTable = GFL_NEW(heap) gfl2::fs::NameTable();
    m_Singlton.m_romArcFileTable->SyncCreateTable(
      romAsyncFileManager,
      "../arc/arc_out/hio_arc_file.bin",  // Win32版は短縮名ファイルを使用しない
      heap,      
      heap->GetLowerHandle(),      
      NULL,
      gfl2::fs::AsyncFileManager::HioUseSetting::SAME_AS_MANAGER
    );
    romAsyncFileManager->SetArcFileNameTable( gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, heap, const_cast<const char**>(m_Singlton.m_romArcFileTable->GetNameTable()), m_Singlton.m_romArcFileTable->GetNameNum(), 0, "../arc/arc_win32/" );
  }

#endif  // GF_PLATFORM_

  //ファイルマネージャを設定する
  romAsyncFileManager->SetCallerHeap(heapDebugFsCaller);
  romAsyncFileManager->SetIntervalCheckHeap(heapDebugFsIntervalCheck);
  {
    if(heapDebugFsMatchCheck)
    {
      romAsyncFileManager->CreateMatchCheckSystem(heapDebugFsMatchCheck);
      romAsyncFileManager->SetMatchCheckFlag(true);
      romAsyncFileManager->SetMatchCheckAnnounceAssert(true);
    }
  }
  m_Singlton.m_romAsyncFileManager = romAsyncFileManager;


  ////////////////////////////////////////
  // その他
  ////////////////////////////////////////
#if PM_DEBUG
  // ArcSrcファイル
  {
    bool arcSrcCanCreateFlag = false;
#if defined(GF_PLATFORM_CTR)
    if(hioEnableFlag)  // CTRのHIO
    {
      arcSrcCanCreateFlag = true;
    }
#elif defined(GF_PLATFORM_WIN32)
    arcSrcCanCreateFlag = true;
#endif  // GF_PLATFORM_

    if(arcSrcCanCreateFlag)
    {
      // 一旦HIOを使用する状態にする
      romAsyncFileManager->SetHioUseFlag(true);

      if(heapDebugFsGasc)
      {
        // gascファイルリストを設定する
        m_Singlton.m_arcSrcFileTable = GFL_NEW(heapDebugFsGasc) gfl2::fs::NameTable();
        m_Singlton.m_arcSrcFileTable->SyncCreateTable(
          romAsyncFileManager,
          "../arc/arc_out/gasc_file.bin",
          heapDebugFsGasc,      
          heapDebugFsGasc->GetLowerHandle(),      
          NULL,
          gfl2::fs::AsyncFileManager::HioUseSetting::SAME_AS_MANAGER
        );
        romAsyncFileManager->SetArcFileNameTable( gfl2::fs::AsyncFileManager::ArcFileNameTableType::ARCSRC, heapDebugFsGasc, const_cast<const char**>(m_Singlton.m_arcSrcFileTable->GetNameTable()), m_Singlton.m_arcSrcFileTable->GetNameNum(), 0, NULL );

        // ArcSrcファイルの読み込みに使うデフォルトメモリを設定する
        romAsyncFileManager->SetArcSrcDefaultHeap(heapDebugFsGasc);

        // ArcSrcファイルを生成するフラグ初期値
        romAsyncFileManager->SetArcSrcCreateFlag(false);
        // ArcSrcファイルを使用するフラグ初期値
        romAsyncFileManager->SetArcSrcUseFlag(false);
      }

      // HIOを使用しない状態に戻す
      romAsyncFileManager->SetHioUseFlag(false);
    }
  }
#endif

#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
  // CTRのSDカード
  {
    gfl2::heap::HeapBase* debugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    gfl2::fs::System::InitializeCtrSdmcDevice(debugHeap, "sdmc:");
  }
#endif  // GF_PLATFORM_CTR
#endif

#if PM_DEBUG
  // DirectSdmc
  {
    if(heapDebugFsDirectSdmc)
    {
#if defined(GF_PLATFORM_CTR)
      gfl2::fs::System::MountCtrDirectSdmcDevice(heapDebugFsDirectSdmc, "/niji/direct_sdmc");
#elif defined(GF_PLATFORM_WIN32)
      sprintf_s(path, PATH_BUF_SIZE, "%s/../direct_sdmc", env);  // envはgfl2::fs::System::MountWin32DirectSdmcDevice関数を呼ぶ前に設定されている。
      gfl2::fs::System::MountWin32DirectSdmcDevice(heapDebugFsDirectSdmc, path);
#endif  // GF_PLATFORM_
    }
  }  // DirectSdmc

  // DirectSdmcにあるgarcファイル関連(mstxtのgarcをSDカードから読み込むのに使用する)
  {
    if(heapDebugFsDirectSdmc)
    {
      gfl2::fs::NameTable* directSdmcArcFileTable = NULL;

      /*
      エラーがたくさん出るのでこの方法はやめる。
#if defined(GF_PLATFORM_CTR)
      // CTRのときはHIOのファイルテーブルを使用する
      if(m_Singlton.m_hioArcFileTable)
      {
        directSdmcArcFileTable = m_Singlton.m_hioArcFileTable;
      }
      else
      {
        // HIOのファイルテーブルがないときは、DirectSdmcからファイルテーブルを読み込む
        gfl2::fs::DirectSdmcFile* file = gfl2::fs::System::CreateDirectSdmcFile(heapDebugFsDirectSdmc->GetLowerHandle(), "arc/hio_arc_file.bin", gfl2::fs::Define::OpenMode::READ);
        if(file)
        {
          void*  buf  = NULL;
          size_t size = 0;
          file->GetFileSize(&size);
          file->Read(&buf, NULL, NULL, size, heapDebugFsDirectSdmc, 4);
          gfl2::fs::System::DestroyDirectSdmcFile(file);
     
          m_Singlton.m_directSdmcArcFileTable = GFL_NEW(heapDebugFsDirectSdmc) gfl2::fs::NameTable();
          m_Singlton.m_directSdmcArcFileTable->CreateTable(heapDebugFsDirectSdmc, buf);  // bufの破棄はm_directSdmcArcFileTableに任せる
          
          directSdmcArcFileTable = m_Singlton.m_directSdmcArcFileTable;
        }
      }
#elif defined(GF_PLATFORM_WIN32)
      // Win32のときはMAINのファイルテーブルを使用する
      if(m_Singlton.m_romArcFileTable)
      {
        directSdmcArcFileTable = m_Singlton.m_romArcFileTable;
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。Win32のときにMAINのファイルテーブルがない。
      }
#endif  // GF_PLATFORM_
      */
      {
        // DirectSdmcからファイルテーブルを読み込む
        gfl2::fs::DirectSdmcFile* file = gfl2::fs::System::CreateDirectSdmcFile(heapDebugFsDirectSdmc->GetLowerHandle(), "arc/hio_arc_file.bin", gfl2::fs::Define::OpenMode::READ);
        if(file)
        {
          void*  buf  = NULL;
          size_t size = 0;
          file->GetFileSize(&size);
          file->Read(&buf, NULL, NULL, size, heapDebugFsDirectSdmc, 4);
          gfl2::fs::System::DestroyDirectSdmcFile(file);
     
          m_Singlton.m_directSdmcArcFileTable = GFL_NEW(heapDebugFsDirectSdmc) gfl2::fs::NameTable();
          m_Singlton.m_directSdmcArcFileTable->CreateTable(heapDebugFsDirectSdmc, buf);  // bufの破棄はm_directSdmcArcFileTableに任せる
          
          directSdmcArcFileTable = m_Singlton.m_directSdmcArcFileTable;
        }
        else
        {
          GFL_PRINT("上記の \"ERROR: nn::Result = 0xC8804478\" はDirectSdmcのgarcファイルが使えないことを示しているだけなので気にしないで下さい。\n");
        }
      }

      if(directSdmcArcFileTable)
      {
        romAsyncFileManager->SetArcFileNameTable( gfl2::fs::AsyncFileManager::ArcFileNameTableType::DIRECT_SDMC, heapDebugFsDirectSdmc, const_cast<const char**>(directSdmcArcFileTable->GetNameTable()), directSdmcArcFileTable->GetNameNum(), 0, "arc/" );
        romAsyncFileManager->CreateDirectSdmcArcFileExistTable(heapDebugFsDirectSdmc);
     
        // DirectSdmcにあるgarcファイルを使用する状態にする
        romAsyncFileManager->SetDirectSdmcArcFileUseFlag(true);
      }
    }
  }  // DirectSdmcにあるgarcファイル関連
#endif  // PM_DEBUG

#if defined(GF_PLATFORM_CTR)
#if PM_DEBUG
  // CTRのHIO
  {
    // HIOを使用するフラグ初期値
  #if GF_DEVELOP_HIO_RESOURCE
    romAsyncFileManager->SetHioUseFlag(hioEnableFlag);
  #else
    romAsyncFileManager->SetHioUseFlag(false);
  #endif
  }
#endif
#endif  // GF_PLATFORM_CTR






  //SDカードマウント
#if PM_DEBUG && GFL_ENABLE_SDMC
  /*
  知らない記述なので取り敢えずコメントアウト。kawada_koji

  gfl2::fs::FileSystem::MountSdmcArchive();
  gfl2::fs::ArcFile::SetSDMountFileNameTable( SDCardArchiveFileTable );
  //常時有効ならここで実行して良い
  //gfl::fs::ArcFile::SetSDMountIsEnable(true);

  */
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  ファイルシステム開放
 */
//-----------------------------------------------------------------------------
void GflUse::FileSystemTerminate(void)
{
  gfl2::fs::Result    res;

  ////////////////////////////////////////
  // その他
  ////////////////////////////////////////
#if PM_DEBUG
  // DirectSdmcにあるgarcファイル関連
  {
    GFL_SAFE_DELETE(m_Singlton.m_directSdmcArcFileTable);
  }

  // DirectSdmc
  {
#if defined(GF_PLATFORM_CTR)
    gfl2::fs::System::UnmountCtrDirectSdmcDevice();
#elif defined(GF_PLATFORM_WIN32)
    gfl2::fs::System::UnmountWin32DirectSdmcDevice();
#endif  // GF_PLATFORM_
  }
#endif

#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
  // CTRのSDカード
  {
    gfl2::fs::System::TerminateCtrSdmcDevice();
  }
#endif  // GF_PLATFORM_CTR
#endif

#if PM_DEBUG
  // ArcSrcファイル
  {
    // gascファイルリストの解放
    GFL_SAFE_DELETE(m_Singlton.m_arcSrcFileTable);
  }
#endif


#if defined(GF_PLATFORM_CTR)
  ////////////////////////////////////////
  // (5) ファイルマネージャ破棄
  ////////////////////////////////////////
  res = gfl2::fs::System::DestroyAsyncFileManagerForCtrRomDevice();

#if PM_DEBUG
  // CTRのHIO
  {
    GFL_SAFE_DELETE(m_Singlton.m_hioArcFileTable);
  }
#endif

  ////////////////////////////////////////
  // (6) デバイスのアンマウント
  ////////////////////////////////////////
  res = gfl2::fs::System::UnmountCtrRomDevice();

#if PM_DEBUG
  // CTRのHIO
  {
    res = gfl2::fs::System::UnmountCtrHioRomDevice();
  }
  // ArcSrcファイル
  {
    res = gfl2::fs::System::UnmountCtrArcSrcDevice();
  }
#endif

#elif defined(GF_PLATFORM_WIN32)
  ////////////////////////////////////////
  // (5) ファイルマネージャ破棄
  ////////////////////////////////////////
  res = gfl2::fs::System::DestroyAsyncFileManagerForWin32RomDevice();
  GFL_SAFE_DELETE(m_Singlton.m_romArcFileTable);

  ////////////////////////////////////////
  // (6) デバイスのアンマウント
  ////////////////////////////////////////
  res = gfl2::fs::System::UnmountWin32RomDevice();

#if PM_DEBUG
  // ArcSrcファイル
  {
    res = gfl2::fs::System::UnmountWin32ArcSrcDevice();
  }
#endif

#endif  // GF_PLATFORM_


  ////////////////////////////////////////
  // (7) ファイルシステムの終了処理
  ////////////////////////////////////////
  res = gfl2::fs::System::Terminate();


#if PM_DEBUG
  ////////////////////////////////////////
  // (0) メモリ解放
  ////////////////////////////////////////
  {
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_FS_INTERVAL_CHECK ) )
    {
      gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_FS_INTERVAL_CHECK ));
    }

    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_FS_MATCH_CHECK ) )
    {
      gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_FS_MATCH_CHECK ));
    }
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_FS_HIO_INITIALIZE_DEVICE ) )
    {
      gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_FS_HIO_INITIALIZE_DEVICE ));
    }

    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_FS_DIRECT_SDMC ) )
    {
      gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_FS_DIRECT_SDMC ));
    }
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_FS_CALLER ) )
    {
      gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_FS_CALLER ));
    }
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_FS_HIO ) )
    {
      gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_FS_HIO ));
    }
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_FS_LOG ) )
    {
      gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_FS_LOG ));
    }
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_FS_GASC ) )
    {
      gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_FS_GASC ));
    }
  }
#endif


  m_Singlton.m_romAsyncFileManager = NULL;
  m_Singlton.m_romArcFileTable     = NULL;
#if PM_DEBUG
  m_Singlton.m_hioArcFileTable      = NULL;
  m_Singlton.m_arcSrcFileTable      = NULL;
  m_Singlton.m_directSdmcArcFileTable  = NULL;
#endif
}


//------------------------------------------------------------------
/**
 * @brief	  本体リージョン確認
 * @return  TRUEの場合 日、米、欧、豪  FALSEの場合 韓、中、台
 */
//------------------------------------------------------------------
bool GflUse::IsJpnFontLoadRegion(void)
{
#if defined(GF_PLATFORM_CTR)
  nn::cfg::CfgRegionCode cr = nn::cfg::GetRegion();
  bool bJpnFontLoadRegion = true;
  if(((cr == nn::cfg::CFG_REGION_KOREA) || (cr == nn::cfg::CFG_REGION_CHINA) || (cr == nn::cfg::CFG_REGION_TAIWAN))){
    bJpnFontLoadRegion = false;
  }
  return bJpnFontLoadRegion;
#elif defined(GF_PLATFORM_WIN32)
  return true;
#endif
}

//------------------------------------------------------------------
/**
 * @brief	  キャラモデルシステムの初期化
 */
//------------------------------------------------------------------
void GflUse::InitializeCharaModelSystem()
{
  poke_3d::model::CharaModelFactory::ResidentInitialize(m_Singlton.m_romAsyncFileManager, m_Singlton.m_pCharaModelHeap, ARCID_CHARA_MODEL_FIELD_NORMAL);
  poke_3d::model::DressUpModelResourceManagerCore::ResidentInitialize(m_Singlton.m_romAsyncFileManager, m_Singlton.m_pCharaModelHeap, System::DressUp::GetDressUpArcIdListBattle());
  poke_3d::model::DressUpModelResourceManagerCore::ResidentInitialize(m_Singlton.m_romAsyncFileManager, m_Singlton.m_pCharaModelHeap, System::DressUp::GetDressUpArcIdListBattle() + (poke_3d::model::DressUpModel::PARTS_MODEL_COUNT + 1));
  poke_3d::model::DressUpModelResourceManagerCore::ResidentInitialize(m_Singlton.m_romAsyncFileManager, m_Singlton.m_pCharaModelHeap, System::DressUp::GetDressUpArcIdListField());
  poke_3d::model::DressUpModelResourceManagerCore::ResidentInitialize(m_Singlton.m_romAsyncFileManager, m_Singlton.m_pCharaModelHeap, System::DressUp::GetDressUpArcIdListField() + (poke_3d::model::DressUpModel::PARTS_MODEL_COUNT + 1));

  // HIO切り替え時にReloadGarc()で先に読み込み直すので断片化対策に一番最後にもっていく
  poke_3d::model::CharaModelFactory::ResidentInitialize(m_Singlton.m_romAsyncFileManager, m_Singlton.m_pCharaModelHeap, ARCID_CHARA_MODEL_BATTLE_NORMAL);
}

//------------------------------------------------------------------
/**
 * @brief	  キャラモデルシステムの終了
 */
//------------------------------------------------------------------
void GflUse::TerminateCharaModelSystem()
{
  poke_3d::model::CharaModelFactory::ResidentFinalize(ARCID_CHARA_MODEL_BATTLE_NORMAL);
  poke_3d::model::CharaModelFactory::ResidentFinalize(ARCID_CHARA_MODEL_FIELD_NORMAL);
  poke_3d::model::DressUpModelResourceManagerCore::ResidentFinalize(System::DressUp::GetDressUpArcIdListBattle());
  poke_3d::model::DressUpModelResourceManagerCore::ResidentFinalize(System::DressUp::GetDressUpArcIdListBattle() + (poke_3d::model::DressUpModel::PARTS_MODEL_COUNT + 1));
  poke_3d::model::DressUpModelResourceManagerCore::ResidentFinalize(System::DressUp::GetDressUpArcIdListField());
  poke_3d::model::DressUpModelResourceManagerCore::ResidentFinalize(System::DressUp::GetDressUpArcIdListField() + (poke_3d::model::DressUpModel::PARTS_MODEL_COUNT + 1));
}

//------------------------------------------------------------------
/**
 * @brief	  キャラモデルシステムのリロード
 */
//------------------------------------------------------------------
void GflUse::ReloadCharaModelSystem()
{
  TerminateCharaModelSystem();
  InitializeCharaModelSystem();
}

//------------------------------------------------------------------
/**
 * @brief   常駐GARCをPCから読み込みなおす用にリロード
 */
//------------------------------------------------------------------
void GflUse::ReloadGarc()
{
  // バトルのGARCだけは独自の読み込みを行っているので直ぐに開きなおす
  poke_3d::model::CharaModelFactory::ResidentFinalize(ARCID_CHARA_MODEL_BATTLE_NORMAL);
  poke_3d::model::CharaModelFactory::ResidentInitialize(m_Singlton.m_romAsyncFileManager, m_Singlton.m_pCharaModelHeap, ARCID_CHARA_MODEL_BATTLE_NORMAL);

  // キャラモデルシステムを読み込み直し予約
  poke_3d::model::CharaModelFactory::ResidentReloadRequest(&ReloadCharaModelSystem);
}

//------------------------------------------------------------------
/**
 * @brief   CTR本体リージョンコードの取得
 * @caution  nn::cfg::CTR::CfgRegionCodeと一致させている
 */
//------------------------------------------------------------------
HardwareRegion GflUse::GetHardwareRegion(void)
{
  return m_Singlton.m_hardowareRegion;
}

//------------------------------------------------------------------
/**
 * @brief   CTR本体リージョンコードの初期化
 * @caution  nn::cfg::CTR::CfgRegionCodeと一致させている
 */
//------------------------------------------------------------------
void GflUse::InitHardwareRegion(void)
{
#if defined(GF_PLATFORM_CTR)
  NN_COMPILER_ASSERT( nn::cfg::CTR::CFG_REGION_JAPAN == HW_REGION_JAPAN );
  NN_COMPILER_ASSERT( nn::cfg::CTR::CFG_REGION_AMERICA == HW_REGION_AMERICA );
  NN_COMPILER_ASSERT( nn::cfg::CTR::CFG_REGION_EUROPE == HW_REGION_EUROPE );
  NN_COMPILER_ASSERT( nn::cfg::CTR::CFG_REGION_CHINA == HW_REGION_CHINA );
  NN_COMPILER_ASSERT( nn::cfg::CTR::CFG_REGION_KOREA == HW_REGION_KOREA );
  NN_COMPILER_ASSERT( nn::cfg::CTR::CFG_REGION_TAIWAN == HW_REGION_TAIWAN );
  NN_COMPILER_ASSERT( nn::cfg::CTR::CFG_REGION_MAX == HW_REGION_MAX );

  m_Singlton.m_hardowareRegion = static_cast<HardwareRegion>(nn::cfg::GetRegion());
#elif defined(GF_PLATFORM_WIN32)
  m_Singlton.m_hardowareRegion = HW_REGION_JAPAN;
#endif

}

#if PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief   CTR本体リージョンコードの変数アドレス取得
 */
//------------------------------------------------------------------
s32* GflUse::DebugGetHardwareRegionPointer(void)
{
  return reinterpret_cast<s32*>(&m_Singlton.m_hardowareRegion);
}
#endif  //PM_DEBUG


//------------------------------------------------------------------
/**
  *  @brief    eshopアプリが有効であるか
  */
//------------------------------------------------------------------
bool GflUse::IsEshopAvailable(void)
{
  bool  isAvailable = false;

#if defined(GF_PLATFORM_CTR)
  isAvailable  = nn::applet::IsEShopAvailable();
#endif

  return isAvailable;
}



//------------------------------------------------------------------
/**
  * @brief   eshopパッチページへの遷移リクエスト
  * @param  [in]  bToPatchPage  false: タイトルページへの遷移リクエスト true:パッチページへの遷移リクエスト
  */
//------------------------------------------------------------------
bool GflUse::TransitToEshopRequest(const bool bToPatchPage)
{
  bool  isAccepted  = false;

#if defined(GF_PLATFORM_CTR)

  if(IsEshopAvailable())
  {
    if(bToPatchPage)
    {
      m_Singlton.m_bTransitToEshopPatchPageRequest = true;
    }
    else
    {
      m_Singlton.m_bTransitToEshopTitlePageRequest = true;
    }
    isAccepted  = true;
  }

#endif

  return isAccepted;
}


//------------------------------------------------------------------
/**
  *  @brief eshopタイトルページへの遷移リクエストがされているか
  */
//------------------------------------------------------------------
bool GflUse::IsTransitToEshopTitlePageRequest(void)
{
  return m_Singlton.m_bTransitToEshopTitlePageRequest;
}


//------------------------------------------------------------------
/**
  *  @brief eshopパッチページへの遷移リクエストがされているか
  */
//------------------------------------------------------------------
bool GflUse::IsTransitToEshopPatchPageRequest(void)
{
  return m_Singlton.m_bTransitToEshopPatchPageRequest;
}


//-----------------------------------------------------------------------------
/**
 *   Miiverseへ写真投稿の可否を設定する
 *   trueで投稿可能 falseで投稿不可能になります。   
 */
//-----------------------------------------------------------------------------
void GflUse::SetScreenCapturePostPermission( bool isEnable )
{
  GFL_ASSERT_MSG( !(m_Singlton.mScreenCapturePostPermission == false && isEnable == false) , "すでに投稿禁止なのに、再度投稿禁止にしようとしています！\n");
  GFL_ASSERT_MSG( !(m_Singlton.mScreenCapturePostPermission &&  isEnable) , "すでに投稿許可なのに、再度投稿許可にしようとしています！\n");
  
  m_Singlton.mScreenCapturePostPermission = isEnable;

#if defined(GF_PLATFORM_CTR)
  if( m_Singlton.mScreenCapturePostPermission )
  {
    // sangoでは写真投稿を必ずしない！
    //ARIIZUMI_PRINT("□□□□□□□□□□□□□□□\nMiiverse投稿を許可しました。\n□□□□□□□□□□□□□□□\n");
    nn::applet::CTR::SetScreenCapturePostPermission( nn::applet::CTR::SCREEN_CAPTURE_POST_DISABLE );
  }
  else
  {
    ARIIZUMI_PRINT("□□□□□□□□□□□□□□□\nMiiverse投稿を禁止しました。\n□□□□□□□□□□□□□□□\n");
    nn::applet::CTR::SetScreenCapturePostPermission( nn::applet::CTR::SCREEN_CAPTURE_POST_DISABLE );
  }
#endif // defined(GF_PLATFORM_CTR)
}

//-----------------------------------------------------------------------------
/**
 *   Miiverseへ写真投稿の可否を設定するの取得
 *   trueで投稿可能 falseで投稿不可能です。
 */
//-----------------------------------------------------------------------------
bool GflUse::IsScreenCapturePostPermission( void )
{
  return m_Singlton.mScreenCapturePostPermission;
}




} // System

