//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   GflvUse.cpp
 *  @brief  GameManagerを動作させるために必要なGFLIB2の管理
 *  @date   2015.02.01
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <thread/include/gfl2_Thread.h>
#include <math/include/gfl2_math.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>

#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "System/include/nijiAllocator.h"
#include "System/include/ThreadPriority.h"
#include "System/include/GflvUse.h"
//#include "Sound/include/Sound.h"//del

#include "Main/Main.h"  //ResidentHeapの初期化/開放関数定義@saito

namespace System{

//-----------------------------------------------------------------------------
// 静的変数の定義
//-----------------------------------------------------------------------------
gfl2::gfx::DisplayDeviceHandle	System::GflvUse::m_HandleList[gfl2::gfx::CtrDisplayNo::NumberOf];
gfl2::gfx::DisplayList*			System::GflvUse::m_pDisplayLists[2];
nijiAllocator*			System::GflvUse::m_pAllocator = NULL;

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
GflvUse::SingltonLibrary GflvUse::m_Singlton = { NULL, };

#if defined(GF_PLATFORM_CTR)
 #if GFL_ENABLE_ASSERT
  #if !PM_DEBUG_PRINT
  const char* GflvUse::m_pAssertMsgBuffer = NULL;
  nn::os::CriticalSection GflvUse::m_pCriticalSectionForAssertMsgBuffer;
  #endif // PM_DEBUG_PRINT
 #endif // GFL_ENABLE_ASSERT
#endif //defined(GF_PLATFORM_CTR)

#if 0
//-----------------------------------------------------------------------------
/**
 *  デフォルトのグラフィックスセットアップ情報
 */
//-----------------------------------------------------------------------------
const gfl::grp::GraphicsSystemSetUp GflvUse::m_DefaultGraphicsSetUp = {
  gfl::grp::MODE_DOUBLE_BUFFER,                     // バッファリングモード
  false,                                             // 右目を使用するか。
  false,                                             // 立体視On状態で初期化するか。

  // ディスプレイバッファ情報
  {
    // DisplayBuffer UPPER
    {
      gfl::grp::DISPLAY_UPPER,            // ディスプレイの種類です。
      400,                                // ディスプレイの横幅です。
      240,                                // ディスプレイの縦幅です。
      gfl::grp::TRANSFER_MODE_ANTIALIASE_NOT_USED,  // ティスプレイバッファの転送モードです。
      gfl::grp::MEMORY_AREA_VRAMB,        // メモリ確保エリアです。
      gfl::grp::RENDER_COLOR_FORMAT_RGB8,// カラーフォーマットです
      false,                              // 転送時にＸ方向にフリップするかどうかを表します。
      0,                                  // 表示を行う始点Ｘ座標です。
      0                                   // 表示を行う始点Ｙ座標です。
    },
    // DisplayBuffer LOWER
    {
      gfl::grp::DISPLAY_LOWER,            // ディスプレイの種類です。
      320,                                // ディスプレイの横幅です。
      240,                                // ディスプレイの縦幅です。
      gfl::grp::TRANSFER_MODE_ANTIALIASE_NOT_USED,  // ティスプレイバッファの転送モードです。
      gfl::grp::MEMORY_AREA_VRAMA,        // メモリ確保エリアです。
      gfl::grp::RENDER_COLOR_FORMAT_RGB8,// カラーフォーマットです
      false,                              // 転送時にＸ方向にフリップするかどうかを表します。
      0,                                  // 表示を行う始点Ｘ座標です。
      0                                   // 表示を行う始点Ｙ座標です。
    },
    // DisplayBuffer UPPER_RIGHT
    {
      gfl::grp::DISPLAY_UPPER_RIGHT,      // ディスプレイの種類です。
      400,                                // ディスプレイの横幅です。
      240,                                // ディスプレイの縦幅です。
      gfl::grp::TRANSFER_MODE_ANTIALIASE_NOT_USED,  // ティスプレイバッファの転送モードです。
      gfl::grp::MEMORY_AREA_VRAMB,        // メモリ確保エリアです。　アプリケーションのデバイスを使う。
      gfl::grp::RENDER_COLOR_FORMAT_RGB8,// カラーフォーマットです
      false,                              // 転送時にＸ方向にフリップするかどうかを表します。
      0,                                  // 表示を行う始点Ｘ座標です。
      0                                   // 表示を行う始点Ｙ座標です。
    },
  },

  // フレームバッファ情報
  {
    // FrameBuffer UPPER
    {
      gfl::grp::DISPLAY_UPPER,                      // 上画面用
      512,                                // バッファの横幅です。
      256,                                // バッファの縦幅です。
      gfl::grp::RENDER_COLOR_FORMAT_RGBA8,          // カラーバッファフォーマット
      gfl::grp::RENDER_DEPTH_FORMAT_24_STENCIL8,    // 深度バッファフォーマット
      gfl::grp::MEMORY_AREA_VRAMA,                  // 描画対象のカラーバッファの配置場所です。
      gfl::grp::MEMORY_AREA_VRAMB,                  // 描画対象のデプスバッファの配置場所です。
      gfl::grp::DISPLAY_NONE,                       // このフレームバッファがオリジナル
      0,
      0,
      400,
      240,
    },

    // FrameBuffer LOWER
    {
      gfl::grp::DISPLAY_LOWER,                      // 下画面用
      512,                                // バッファの横幅です。
      256,                                // バッファの縦幅です。
      gfl::grp::RENDER_COLOR_FORMAT_RGBA8,          // カラーバッファフォーマット
      gfl::grp::RENDER_DEPTH_FORMAT_24_STENCIL8,    // 深度バッファフォーマット
      gfl::grp::MEMORY_AREA_NONE,                   // 描画対象のカラーバッファの配置場所です。
      gfl::grp::MEMORY_AREA_NONE,                   // 描画対象のデプスバッファの配置場所です。
      gfl::grp::DISPLAY_UPPER,                      // UPPERのフレームバッファを流用
      0,
      0,
      320,
      240,
    },

    // FrameBuffer UPPER_RIGHT
    {
      gfl::grp::DISPLAY_UPPER_RIGHT,                // 下画面用
      512,                                // バッファの横幅です。
      256,                                // バッファの縦幅です。
      gfl::grp::RENDER_COLOR_FORMAT_RGBA8,          // カラーバッファフォーマット
      gfl::grp::RENDER_DEPTH_FORMAT_24_STENCIL8,    // 深度バッファフォーマット
      gfl::grp::MEMORY_AREA_NONE,                   // 描画対象のカラーバッファの配置場所です。
      gfl::grp::MEMORY_AREA_NONE,                   // 描画対象のデプスバッファの配置場所です。
      gfl::grp::DISPLAY_UPPER,                      // UPPERのフレームバッファを流用
      0,
      0,
      400,
      240,
    },
  },

  // コマンドリスト設定
  {
    // MAIN
    {
      0x20000,                 // バッファサイズ（コマンドバッファの総サイズ）
      256,                       // キューイングコマンド数
    },

    // UPPER
    {
      0x40000,                 // バッファサイズ（コマンドバッファの総サイズ）
      128,                      // キューイングコマンド数
    },

    // LOWER
    {
      0x40000,                 // バッファサイズ（コマンドバッファの総サイズ）
      32,                      // キューイングコマンド数
    },
  },

  // セーブコマンドリスト設定
  {
    0x80000,                 // バッファサイズ（コマンドバッファの総サイズ）
    128,                      // キューイングコマンド数
  },

  // クリアカラー
  {
    // ClearColor UPPER
    {
      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0xFF
    },

    // ClearColor LOWER
    {
      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0xFF
    },
  },

};
#endif

#if 0
//----------------------------------------------------------------------------
/**
 *  @brief  乱数取得
 *
 *  @param  max_range 取得する最大値（0 <= n < max_range）
 *
 *  @return 乱数
 */
//-----------------------------------------------------------------------------
u32 GflvUse::GetPublicRand( u32 max_range )
{
  GFL_ASSERT_STOP( m_Singlton.m_pRandom != NULL );
  // 乱数取得
  return m_Singlton.m_pRandom->Next( max_range );
}

//----------------------------------------------------------------------------
/**
 *  @brief  乱数取得（0?U32_MAX)
 *
 *  @return 乱数
 */
//-----------------------------------------------------------------------------
u32 GflvUse::GetPublicRand( void )
{
  GFL_ASSERT_STOP( m_Singlton.m_pRandom != NULL );
  // 乱数取得
  return m_Singlton.m_pRandom->Next();
}



//----------------------------------------------------------------------------
/**
 *  @brief  ポケモン生成乱数取得
 *  @param  max_range 取得する最大値（0 <= n < max_range）
 *  @return 乱数
 */
//-----------------------------------------------------------------------------
u32 GflvUse::GetPokemonCreateRand( u32 max_range )
{
  GFL_ASSERT_STOP( m_Singlton.m_pPokemonCreateRandom != NULL );
  // 乱数取得
  return m_Singlton.m_pPokemonCreateRandom->Next( max_range );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ポケモン生成乱数取得（0?U32_MAX)
 *
 *  @return 乱数
 */
//-----------------------------------------------------------------------------
u32 GflvUse::GetPokemonCreateRand( void )
{
  GFL_ASSERT_STOP( m_Singlton.m_pPokemonCreateRandom != NULL );
  // 乱数取得
  return m_Singlton.m_pPokemonCreateRandom->Next();
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
u64 GflvUse::GetPokemonCreateRand64( void )
{
  GFL_ASSERT_STOP( m_Singlton.m_pPokemonCreateRandom != NULL );
  // 乱数取得
  return m_Singlton.m_pPokemonCreateRandom->Next(); //Next64(); @todo
}
#endif

//------------------------------------------------------------------
/**
 * @name     GetTransferableId
 * @brief    固有IDを取得する
 */
//------------------------------------------------------------------

void GflvUse::PokemonCreateRandomInit(u32 seed)
{
#if 0
  nn::fnd::TimeSpan now = nn::fnd::DateTime::GetNow()  - nn::fnd::DateTime() - nn::cfg::GetUserTimeOffset();
  u32 z = static_cast<u32>(now.GetMilliSeconds());

  z += seed;
  m_Singlton.m_pPokemonCreateRandom->Initialize( z );
#endif

}


//----------------------------------------------------------------------------
/**
 *  @brief  アーカイブファイルをPCから読み込むことが可能か？
 *
 *  @retval true    可能
 *  @retval false   不可能　→　可能にするには、デバッグビルドをして、デバッガ設定をdev2にしてください。
 */
//-----------------------------------------------------------------------------
bool GflvUse::IsArcHioMode( void )
{
#if GFL_HIO_ENABLE
  if( gfl::heap::Manager::IsExistHeapId( HEAPID_DEBUG ) &&
      gfl::heap::Manager::IsExistHeapId( HEAPID_DEBUG_DEVICE ) )
  {
    return true;
  }

  return false;
#else
  return false;
#endif

}

//----------------------------------------------------------------------------
/**
 *  @brief  ホームボタン＆電源ボタンの復帰処理
 */
//-----------------------------------------------------------------------------

void GflvUse::ReturnProsessing(void)
{
}


//----------------------------------------------------------------------------
/**
 *  @brief  ループ内コールバック処理
 */
//-----------------------------------------------------------------------------
bool GflvUse::LoopFunc( const char* pbuff )
{
//  m_Singlton.m_pUiDeviceManager->Main();
  return false;
}




//----------------------------------------------------------------------------
/**
 *  @brief  ASSERTループ内コールバック処理
 */
//-----------------------------------------------------------------------------
void GflvUse::AssertLoopFunc( const char* pbuff,b32 bStop )
{

  for(;;){
    m_Singlton.m_pUiDeviceManager->Main();
    gfl2::ui::Button* p_button = m_Singlton.m_pUiDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    u32 key = p_button->GetHold();
    if( key ==  (gfl2::ui::BUTTON_X|gfl2::ui::BUTTON_Y) ){
      break;
    }
  }


#if 0
  xy::SleepHandler::Func();

  //キーで抜ける処理
#if GFL_ENABLE_ASSERT

#if PM_DEBUG_PRINT
  m_Singlton.m_pUiDeviceManager->Main();
  gfl::ui::Button* p_button = m_Singlton.m_pUiDeviceManager->GetButton(gfl::ui::CTR_DeviceManager::BUTTON_STANDARD);
  u32 key = p_button->GetHold();
  if( key ==  (gfl::ui::BUTTON_X|gfl::ui::BUTTON_Y) ){
    return true;
  }
  return false;


#else //PM_DEBUG_PRINT
  // サブスレッドの場合は、メインスレッドの返してアプレットを表示する
  bit32 current_id = nn::os::Thread::GetCurrentId();
  if( !gfl::base::Thread::IsMainThread(current_id) )
  {
    // 排他制御開始
    m_pCriticalSectionForAssertMsgBuffer.Enter();

    // 複数の同時書き込みは行わない
    if( m_pAssertMsgBuffer == NULL )
    {
      // サブスレッドからの呼び出しの場合のみ、SDカードにエラー内容を書き込む
      int error_disp_wait = 0;
      m_pAssertMsgBuffer = pbuff;
      while( m_pAssertMsgBuffer )
      {
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
        ONOUE_PRINT("アプレット待ち=%d\n", error_disp_wait);
        error_disp_wait++;
      }
    }

    // 排他制御終了
    m_pCriticalSectionForAssertMsgBuffer.Leave();
  }
  else
  {
    // メインスレッドの場合は、すぐにアプレットを呼び出す
    gfl::err::ErrAdapter::ErrorDispChar(pbuff);
  }

  return true;

#endif //PM_DEBUG_PRINT

#else
  return false;
  
#endif //GFL_ENABLE_ASSERT
#endif
//  return false;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void GflvUse::checkHeap( void )
{
#if GFL_DEBUG
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
void GflvUse::Initialize( void )
{

#if defined(GF_PLATFORM_CTR)
 #if GFL_ENABLE_ASSERT
  #if !PM_DEBUG_PRINT
  {
    // クスレッド内アサートメッセージバッファ用クリティカルセクション初期化
    m_pCriticalSectionForAssertMsgBuffer.Initialize();
  }
  #endif // PM_DEBUG_PRINT
 #endif // GFL_ENABLE_ASSERT
#endif //defined(GF_PLATFORM_CTR)

  //-----------------------------------------------------------------------------
  /*
   *    Resident以下のヒープ初期化
   */
  //-----------------------------------------------------------------------------
  CommonHeapCreate();
  gfl2::heap::HeapBase *residentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT );
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
  ///FileSystemInitialize( residentHeap );


#if defined(GF_PLATFORM_CTR)
  // デバイス劣化時のファイルアクセス遅延のエミュレーションを初期化
  // 製品版では無効になる
  // 遅延設定はカードへのROMイメージ書き込み時のオプションで指定する
  nn::fs::InitializeLatencyEmulation();
  nn::fs::RegisterAutoFatalAsSdmcEjectedEvent();
#endif

  /*
   * 拡張セーブデータシステムを生成
   *
   */
  //m_Singlton.m_pXyExtSaveSystem = GFL_NEW( residentHeap ) extsavedata::XyExtSaveSystem( residentHeap );

  //-----------------------------------------------------------------------------
  /*
   * 言語管理クラスを生成
   */
  //-----------------------------------------------------------------------------
  ///m_Singlton.m_pLanguage = GFL_NEW( residentHeap ) Language();

  //-----------------------------------------------------------------------------
  /**
   *    時間管理
   */
  //-----------------------------------------------------------------------------
  //gfl::std::DateTime::Initialize();

  //-----------------------------------------------------------------------------
  /**
   *    入力システムの初期化
   */
  //-----------------------------------------------------------------------------
  gfl2::ui::Initialize();
#if defined(GF_PLATFORM_CTR)
  m_Singlton.m_pUiDeviceManager = GFL_NEW(residentHeap) gfl2::ui::CTR_DeviceManager( residentHeap );
#elif defined(GF_PLATFORM_WIN32)
  m_Singlton.m_pUiDeviceManager = GFL_NEW(residentHeap) gfl2::ui::WIN32_DeviceManager( residentHeap );
#endif

#if 0
  //-----------------------------------------------------------------------------
  /**
   *    サウンドシステムの初期化
   */
  //-----------------------------------------------------------------------------
  {
    gfl2::heap::HeapBase* system_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_SOUND );
    gfl2::heap::HeapBase* device_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_SOUND_DEVICE );
    Sound::Initialize( system_heap, device_heap );
    //SoundConfigInit();
  }
#endif //del
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
   */
  //-----------------------------------------------------------------------------
  m_Singlton.m_pRandom = GFL_NEW(residentHeap) gfl2::math::Random();
  m_Singlton.m_pRandom->Initialize();
  m_Singlton.m_pPokemonCreateRandom = GFL_NEW(residentHeap) gfl2::math::MTRandom();

  {
    bit64 id = 0; //netapp::cfg::GetTransferableId(); @todo
    bit64 x1 = (id>>32) & 0xffffffff;
    bit64 x2 = id & 0xffffffff;
    x1 = x1 ^ x2;
    u32 z = x1;
    PokemonCreateRandomInit(z);
  }

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
  //m_Singlton.m_pSystemEventManager = GFL_NEW(residentHeap) event::SystemEventManager();

  //-----------------------------------------------------------------------------
  /**
   *    ASSERTループ内コールバック関数設定
   */
  //-----------------------------------------------------------------------------
  gfl2::debug::GFL_ASSERT_SetLoopFunc(AssertLoopFunc);
  
#if PM_DEBUG
  //-----------------------------------------------------------------------------
  /**
   *    起動プロテクト
   */
  //-----------------------------------------------------------------------------
  //xy::debug::DebugRomProtect* pDRP = GFL_NEW(residentDeviceHeap) xy::debug::DebugRomProtect::DebugRomProtect();
  //pDRP->KeyCheckLoop(m_Singlton.m_pUiDeviceManager->GetButton(gfl::ui::CTR_DeviceManager::BUTTON_STANDARD),LoopFunc,residentDeviceHeap);
  //GFL_DELETE pDRP;
#endif //PM_DEBUG
#if 0
  //-----------------------------------------------------------------------------
  /**
   *    動的モジュールマネージャの初期化
   */
  //-----------------------------------------------------------------------------
  m_Singlton.m_roManager = GFL_NEW(residentHeap) gfl2::ro::RoManager( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD ) );
  m_Singlton.m_roManager->Initialize( residentHeap, m_Singlton.m_romAsyncFileManager , "static.crs", ".crr/static.crr" );
#endif //@saito

  //-----------------------------------------------------------------------------
	/**
	*   アロケーターの初期化
	*/
	//-----------------------------------------------------------------------------
  nijiAllocator::Initialize( residentHeap );
	m_pAllocator = GFL_NEW(residentHeap) nijiAllocator( residentDeviceHeap, nijiAllocator::RESIDENT );

  //-----------------------------------------------------------------------------
	/**
	*   ディスプレイの初期化
	*/
	//-----------------------------------------------------------------------------
  gfl2::gfx::GFGL::Initialize( m_pAllocator );
	m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]		= gfl2::gfx::GFGL::CreateDisplay(gfl2::gfx::DisplayMode::CTR_LEFT , gfl2::gfx::Format::A8R8G8B8, gfl2::gfx::Format::D24S8, gfl2::gfx::MultiSample::None);
	m_HandleList[gfl2::gfx::CtrDisplayNo::RIGHT]	= gfl2::gfx::GFGL::CreateDisplay(gfl2::gfx::DisplayMode::CTR_RIGHT, gfl2::gfx::Format::A8R8G8B8, gfl2::gfx::Format::D24S8, gfl2::gfx::MultiSample::None);
	m_HandleList[gfl2::gfx::CtrDisplayNo::DOWN]		= gfl2::gfx::GFGL::CreateDisplay(gfl2::gfx::DisplayMode::CTR_DOWN , gfl2::gfx::Format::A8R8G8B8, gfl2::gfx::Format::D24S8, gfl2::gfx::MultiSample::None);

  //-----------------------------------------------------------------------------
  /**
   *   SystemFontの作成
   */
  //-----------------------------------------------------------------------------
  m_Singlton.m_pSystemFontHeap = GFL_CREATE_HEAP( residentHeap, HEAPID_SYSTEM_FONT, HeapSize::SYSTEM_FONT_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_Singlton.m_pSystemFontDevHeap = GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_SYSTEM_FONT_DEVICE, HeapSize::SYSTEM_FONT_DEVICE_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  ///m_Singlton.m_pSystemFont = GFL_NEW(residentHeap) view_print::SystemFont(m_Singlton.m_pSystemFontHeap,m_Singlton.m_pSystemFontDevHeap, IsJpnFontLoadRegion());

	//-----------------------------------------------------------------------------
	/**
	*   レンダリングエンジンの初期化
	*/
	//-----------------------------------------------------------------------------
	//全ノードを管理するマネージャ初期化
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::InitDescription desc( 1024 * 4 );
    desc.m_AABBTreeSize = gfl2::math::AABB( gfl2::math::Vector4(0, 0, 0), gfl2::math::Vector4(10240,10240,10240) );
    desc.m_AABBDepth = 3; // 2のm_AABBDepth+1乗の分割数
    gfl2::renderingengine::scenegraph::SceneGraphManager::Initialize(desc);
  }

	//リソースノードを生成するマネージャ初期化
	gfl2::renderingengine::scenegraph::resource::ResourceManager::Initialize();
	m_pDisplayLists[0] = gfl2::gfx::GFGL::CreateDisplayList();
    m_pDisplayLists[1] = gfl2::gfx::GFGL::CreateDisplayList();

  //-----------------------------------------------------------------------------
  /**
   *    ヒープの余り
   */
  //-----------------------------------------------------------------------------
  {
    gfl2::heap::HeapBase *systemH = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
    gfl2::heap::HeapBase *deviceH = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
    gfl2::heap::HeapBase *residentH = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT );
    gfl2::heap::HeapBase *residentDeviceH = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );
    GFL_PRINT( "======================================\n" );
    GFL_PRINT( "INIT END SysHeap  Free Size 0x%08x\n", systemH->GetTotalFreeSize() );
    GFL_PRINT( "INIT END SysDev   Free Size 0x%08x\n", deviceH->GetTotalFreeSize() );
    GFL_PRINT( "INIT END ResHeap  Free Size 0x%08x\n", residentH->GetTotalFreeSize() );
    GFL_PRINT( "INIT END ResDev   Free Size 0x%08x\n", residentDeviceH->GetTotalFreeSize() );
    GFL_PRINT( "======================================\n" );
  }
}


//==============================================================================
/**
 *  @brief  常駐GFLIBシステムの破棄
 */
//==============================================================================
void GflvUse::Finalize( void )
{
  //-----------------------------------------------------------------------------
  /**
   *   SystemFontの廃棄
   */
  //-----------------------------------------------------------------------------
  ///GFL_SAFE_DELETE( m_Singlton.m_pSystemFont );

  GFL_DELETE_HEAP( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_SYSTEM_FONT_DEVICE) );
  GFL_DELETE_HEAP( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_SYSTEM_FONT) );
  //-----------------------------------------------------------------------------
  /**
   *    乱数の破棄
   */
  //-----------------------------------------------------------------------------
  GFL_SAFE_DELETE(m_Singlton.m_pPokemonCreateRandom);
  GFL_SAFE_DELETE(m_Singlton.m_pRandom);

  //-----------------------------------------------------------------------------
  /**
   *    システムイベントマネージャの破棄
   */
  //-----------------------------------------------------------------------------
  //GFL_SAFE_DELETE( m_Singlton.m_pSystemEventManager );
#if 0
  //-----------------------------------------------------------------------------
  /**
   *    サウンドシステムの破棄
   */
  //-----------------------------------------------------------------------------
  Sound::Finalize();
#endif //del
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
  ///GFL_SAFE_DELETE( m_Singlton.m_pLanguage );

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
  /*
   *    ヒープの開放
   */
  //-----------------------------------------------------------------------------
  CommonHeapFree();

#if defined(GF_PLATFORM_CTR)
 #if GFL_ENABLE_ASSERT
  #if !PM_DEBUG_PRINT
  {
    // スレッド内アサートメッセージバッファ用クリティカルセクション終了
    m_pCriticalSectionForAssertMsgBuffer.Finalize();
  }
  #endif // PM_DEBUG_PRINT
 #endif // GFL_ENABLE_ASSERT
#endif //defined(GF_PLATFORM_CTR)

	for (u32 i = 0; i < 2; ++i)
  {
		GFL_DELETE(m_pDisplayLists[i]);
	}
	//-----------------------------------------------------------------------------
	/*
	*    ディスプレイの開放
	*/
	//-----------------------------------------------------------------------------
	for (u32 i = 0; i < gfl2::gfx::CtrDisplayNo::NumberOf; ++i)
  {
		gfl2::gfx::GFGL::DestroyDisplay(m_HandleList[i]);
  }

	gfl2::renderingengine::scenegraph::resource::ResourceManager::Terminate();
	gfl2::renderingengine::scenegraph::SceneGraphManager::Terminate();
	gfl2::gfx::GFGL::Terminate();

	//-----------------------------------------------------------------------------
	/**
	*   アロケーターの解放
	*/
	//-----------------------------------------------------------------------------
  nijiAllocator::Terminate();
	GFL_DELETE m_pAllocator;

	//Resident開放
  Main::ResidentHeapFree();
}


//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックスシステムの実行が完了してるかチェックする。
 */
//-----------------------------------------------------------------------------
bool GflvUse::IsGpuIdle( void )
{
#if 0
  if(m_Singlton.m_pGraphicsSystem){
    gfl::grp::CommandListManager * pCommandList = m_Singlton.m_pGraphicsSystem->GetCommandListManager();
    return !(pCommandList->IsRuning());
  }
#endif
  return true;
}




//==============================================================================
/**
 *  @brief  常駐GFLIBシステムの更新
 */
//==============================================================================
void GflvUse::Update( void )
{
  // UI(入力デバイス)の更新 
  m_Singlton.m_pUiDeviceManager->Main();

#if 0
  // ファイル読み込み管理の更新
  m_Singlton.m_pFileReadManager->UpdateFileRead();

  m_Singlton.m_pUiDeviceManager->Main();
#endif
}

//==============================================================================
//==============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  常駐GFLIBシステム　Vsync待ち処理
 */
//-----------------------------------------------------------------------------
void GflvUse::VSyncWait( void )
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
void GflvUse::VSync( void )
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


void GflvUse::FatalErrorDisp( void )
{
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
void GflvUse::createGraphicsSystemTitle( void )
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
void GflvUse::createGraphicsSystem( const gfl::grp::GraphicsSystemSetUp& setup, u32 vramA, u32 vramB, gfl::heap::HeapBase* p_heap, gfl::heap::HeapBase* p_device, bool heap_low, const gfl::math::VEC4& color )
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
void GflvUse::deleteGraphicsSystem( void )
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
bool GflvUse::isCreateGraphicsSystem( void )
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
void GflvUse::CommonHeapCreate(void)
{
  //Resident初期化
  Main::ResidentHeapCreate();

  gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  gfl2::heap::HeapBase *deviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
  gfl2::heap::HeapBase *residentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT );
  gfl2::heap::HeapBase *residentDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );

  // デバッグ用メモリの割り当て
#if GFL_DEBUG
  {
  #if defined(GF_PLATFORM_WIN32)
    size_t debugDeviceHeapSize = System::HeapSize::DEBUG_DEVICE;

  #elif defined(GF_PLATFORM_CTR)
    const size_t assingment   = nn::os::GetAppMemorySize();
    size_t debugDeviceHeapSize = 0x100;

    switch(assingment){
    case 0x6000000: //CTR dev1=96M
      debugDeviceHeapSize = System::HeapSize::DEBUG_DEVICE_96M;
      break;
    case 0x7C00000: //Snake dev2=124M
    case 0xB200000: //Snake dev1=178M
      debugDeviceHeapSize = System::HeapSize::DEBUG_DEVICE_80M; //CTR dev2と同じサイズにしておく
      break;
    case 0x5000000: //CTR dev2
      debugDeviceHeapSize = System::HeapSize::DEBUG_DEVICE_80M;
      break;
    default:
      GFL_PRINT( "SystemMode CTR prod or ????\n" );
      GFL_ASSERT_STOP_MSG(0, "SystemMode CTR prod or ????\n" );
      break;
    }
  #endif  //defined(GF_PLATFORM_CTR)
    GFL_CREATE_HEAP( systemHeap , HEAPID_DEBUG ,System::HeapSize::DEBUG_HEAP, gfl2::heap::HEAP_TYPE_EXP , true );
    GFL_CREATE_HEAP( deviceHeap, HEAPID_DEBUG_DEVICE,debugDeviceHeapSize, gfl2::heap::HEAP_TYPE_EXP , true );
  }
#endif //GFL_DEBUG

#if 0
  // MobiClip用にMemoryBlockを確保
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
    GFL_CREATE_HEAP( residentHeap, HEAPID_FILEREAD, System::HeapSize::FILE_READ, gfl2::heap::HEAP_TYPE_EXP, false );
    // Eventメモリ      parent::residentHeap
    GFL_CREATE_HEAP( residentHeap, HEAPID_EVENT,System::HeapSize::EVENT, gfl2::heap::HEAP_TYPE_EXP, false );
    // EventDeviceメモリ      parent::residentHeap
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_EVENT_DEVICE,System::HeapSize::EVENT_DEVICE, gfl2::heap::HEAP_TYPE_EXP, false );
    // 動的モジュール読み込み
    GFL_CREATE_HEAP( residentHeap, HEAPID_DLL_LOAD, System::HeapSize::DLL_LOAD, gfl2::heap::HEAP_TYPE_EXP, false );
  }

  {
    // Appメモリ      parent::systemHeap
    GFL_CREATE_HEAP( systemHeap, HEAPID_APP,System::HeapSize::APP, gfl2::heap::HEAP_TYPE_EXP, false );
    // AppDeviceメモリ  parent::deviceHeap
    GFL_CREATE_HEAP( deviceHeap, HEAPID_APP_DEVICE,System::HeapSize::APP_DEVICE, gfl2::heap::HEAP_TYPE_EXP, false );
  }
  {
    // Soundメモリ      parent::residentHeap
    GFL_CREATE_HEAP( residentHeap, HEAPID_SOUND,System::HeapSize::HEAP_SOUND, gfl2::heap::HEAP_TYPE_EXP, false );
    // SondDeviceメモリ  parent::residentDeviceHeap
    GFL_CREATE_HEAP( residentDeviceHeap, HEAPID_SOUND_DEVICE,System::HeapSize::HEAP_SOUND_DEVICE, gfl2::heap::HEAP_TYPE_EXP, false );    
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  コモンヒープ開放
 */
//-----------------------------------------------------------------------------
void GflvUse::CommonHeapFree(void)
{
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_SOUND_DEVICE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_SOUND ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD ));
#if GFL_DEBUG
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEVICE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ));
#endif

  
}


//----------------------------------------------------------------------------
/**
 *  @brief  ファイルシステム初期化
 */
//-----------------------------------------------------------------------------
void GflvUse::FileSystemInitialize(gfl2::heap::HeapBase* heap)
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

  m_Singlton.m_romAsyncFileManager = NULL;
  m_Singlton.m_romArcFileTable     = NULL;
#if GFL_DEBUG
  m_Singlton.m_romHioArcFileTable  = NULL;
  m_Singlton.m_romArcSrcFileTable  = NULL;
#endif

  // CTRのHIO
  bool hioEnableFlag = false;


#if GFL_DEBUG
  ////////////////////////////////////////
  // (0) メモリ確保
  ////////////////////////////////////////
  {
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG ) )
    {
      gfl2::heap::HeapBase* parentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
      heapDebugFsGasc = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_GASC, System::HeapSize::DEBUG_FS_GASC, gfl2::heap::HEAP_TYPE_EXP, false );
      heapDebugFsLog  = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_LOG , System::HeapSize::DEBUG_FS_LOG , gfl2::heap::HEAP_TYPE_EXP, false );
      heapDebugFsHio  = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_HIO , System::HeapSize::DEBUG_FS_HIO , gfl2::heap::HEAP_TYPE_EXP, false );
    }
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_DEVICE ) )
    {
      gfl2::heap::HeapBase* parentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEVICE );
      heapDebugFsHioInitializeDevice = GFL_CREATE_HEAP( parentHeap, HEAPID_DEBUG_FS_HIO_INITIALIZE_DEVICE, System::HeapSize::DEBUG_FS_HIO_INITIALIZE_DEVICE, gfl2::heap::HEAP_TYPE_EXP, false );
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

#if GFL_DEBUG
  // CTRのHIO
  {
    if(hioEnableFlag)
    {
      res = gfl2::fs::PcUtil::GetProcessEnvironmentVariable(NULL, env, ENV_BUF_SIZE, "NIJI_ENVIRONMENT_ROOT");  // CTRからはPCに設定されている環境変数しか取れない。batで設定したものは取れない。
      sprintf(path, "%s/../data_ctr", env);
      res = gfl2::fs::System::MountCtrHioRomDevice(heapDebugFsHio, path);
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
    System::ThreadPriority::ROM_ASYNC_FILE_MANAGER_OTHER_THREAD_PRIORITY
  );
  romAsyncFileManager = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  // arcファイルリストを設定する
  {
#include <arc_out/arc_file_utf8.h>
    romAsyncFileManager->SetArcFileNameTable( gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, heap, ArchiveFileTable, sizeof(ArchiveFileTable)/sizeof(ArchiveFileTable[0]), 0, NULL );
  }

#if GFL_DEBUG
  // CTRのHIO
  {
    if(hioEnableFlag)
    {
      // この時点でHIOでPC上の通常ファイル(garcでないファイル)は読める。
      m_Singlton.m_romHioArcFileTable = GFL_NEW(heap) gfl2::fs::NameTable();
      m_Singlton.m_romHioArcFileTable->SyncCreateTable(
        romAsyncFileManager,
        "../arc/arc_out/hio_arc_file.bin",  // CTRのHIO版は短縮名ファイルを使用しない
        heapDebugFsHio,      
        heapDebugFsHio->GetLowerHandle(),      
        NULL,
        gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_HIO
      );
      romAsyncFileManager->SetArcFileNameTable( gfl2::fs::AsyncFileManager::ArcFileNameTableType::HIO, heapDebugFsHio, const_cast<const char**>(m_Singlton.m_romHioArcFileTable->GetNameTable()), m_Singlton.m_romHioArcFileTable->GetNameNum(), 0, "../arc/arc_ctr/" );
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

#if GFL_DEBUG
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
    System::ThreadPriority::ROM_ASYNC_FILE_MANAGER_OTHER_THREAD_PRIORITY
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

#endif

  //ファイルマネージャを設定する
  m_Singlton.m_romAsyncFileManager = romAsyncFileManager;


  ////////////////////////////////////////
  // その他
  ////////////////////////////////////////
#if GFL_DEBUG
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
#endif  // GF_PLATFORM_CTR

    if(arcSrcCanCreateFlag)
    {
      // 一旦HIOを使用する状態にする
      romAsyncFileManager->SetHioUseFlag(true);

      if(heapDebugFsGasc)
      {
        // gascファイルリストを設定する
        m_Singlton.m_romArcSrcFileTable = GFL_NEW(heap) gfl2::fs::NameTable();
        m_Singlton.m_romArcSrcFileTable->SyncCreateTable(
          romAsyncFileManager,
          "../arc/arc_out/gasc_file.bin",
          heapDebugFsGasc,      
          heapDebugFsGasc->GetLowerHandle(),      
          NULL,
          gfl2::fs::AsyncFileManager::HioUseSetting::SAME_AS_MANAGER
        );
        romAsyncFileManager->SetArcFileNameTable( gfl2::fs::AsyncFileManager::ArcFileNameTableType::ARCSRC, heapDebugFsGasc, const_cast<const char**>(m_Singlton.m_romArcSrcFileTable->GetNameTable()), m_Singlton.m_romArcSrcFileTable->GetNameNum(), 0, NULL );

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

#if defined(GF_PLATFORM_CTR)
#if GFL_DEBUG
  // CTRのHIO
  {
    // HIOを使用するフラグ初期値
    romAsyncFileManager->SetHioUseFlag(false);
  }
#endif
#endif  // GF_PLATFORM_CTR






  //SDカードマウント
#if PM_DEBUG && GFL_ENABLE_SDMC
  gfl2::fs::FileSystem::MountSdmcArchive();
  gfl2::fs::ArcFile::SetSDMountFileNameTable( SDCardArchiveFileTable );
  //常時有効ならここで実行して良い
  //gfl::fs::ArcFile::SetSDMountIsEnable(true);
#endif

  //HIO初期化
#if GFL_HIO_ENABLE
  /*
  知らない記述なので取り敢えずコメントアウト。kawada_koji

  if( gfl::heap::Manager::IsExistHeapId( HEAPID_DEBUG ) &&
      gfl::heap::Manager::IsExistHeapId( HEAPID_DEBUG_DEVICE ) )
  {
    gfl::heap::HeapBase* debug_heap = gfl::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    gfl::heap::HeapBase* debug_device_heap = gfl::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEVICE );

    // 特定のファイルのみをPCから読み出す
    gfl::fs::FileSystem::InitializeHio( deviceHeap );
    gfl::fs::FileSystem::MountHioArchive( deviceHeap, "pc:", (char*)TEST_DIRECTORY );
    gfl::fs::FileSystem::MountHioArchive( deviceHeap, "arc:", (char*)ARC_DIRECTORY );
    gfl::fs::ArcFile::SetHioFileNameTable( HioArchiveFileTable );

    checkHeap();

    // HIO ホストの生成
    gfl::hio::Host::CreateInstance( debug_heap,deviceHeap );

    GFL_NEW( debug_heap ) gfl::hio::HioSharedMemory( debug_heap, debug_device_heap, System::hio::XY_HIO_FRAGMENT_CHECK );

    checkHeap();

  }

  */
#endif  //GFL_HIO_ENABLE

}

//----------------------------------------------------------------------------
/**
 *  @brief  ファイルシステム開放
 */
//-----------------------------------------------------------------------------
void GflvUse::FileSystemTerminate(void)
{
  gfl2::fs::Result    res;






  //HIOマネージャ破棄
#if GFL_HIO_ENABLE
  /*
  知らない記述なので取り敢えずコメントアウト。kawada_koji
  gfl::fs::FileSystem::FinalizeHio();
  */
#endif






  ////////////////////////////////////////
  // その他
  ////////////////////////////////////////
#if GFL_DEBUG
  // ArcSrcファイル
  {
    // gascファイルリストの解放
    GFL_SAFE_DELETE(m_Singlton.m_romArcSrcFileTable);
  }
#endif


#if defined(GF_PLATFORM_CTR)
  ////////////////////////////////////////
  // (5) ファイルマネージャ破棄
  ////////////////////////////////////////
  res = gfl2::fs::System::DestroyAsyncFileManagerForCtrRomDevice();

#if GFL_DEBUG
  // CTRのHIO
  {
    GFL_SAFE_DELETE(m_Singlton.m_romHioArcFileTable);
  }
#endif

  ////////////////////////////////////////
  // (6) デバイスのアンマウント
  ////////////////////////////////////////
  res = gfl2::fs::System::UnmountCtrRomDevice();

#if GFL_DEBUG
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

#if GFL_DEBUG
  // ArcSrcファイル
  {
    res = gfl2::fs::System::UnmountWin32ArcSrcDevice();
  }
#endif

#endif


  ////////////////////////////////////////
  // (7) ファイルシステムの終了処理
  ////////////////////////////////////////
  res = gfl2::fs::System::Terminate();


#if GFL_DEBUG
  ////////////////////////////////////////
  // (0) メモリ解放
  ////////////////////////////////////////
  {
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_FS_HIO_INITIALIZE_DEVICE ) )
    {
      gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_FS_HIO_INITIALIZE_DEVICE ));
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
#if GFL_DEBUG
  m_Singlton.m_romHioArcFileTable  = NULL;
  m_Singlton.m_romArcSrcFileTable  = NULL;
#endif
}


//------------------------------------------------------------------
/**
 * @brief	  本体リージョン確認
 * @return  TRUEの場合 日、米、欧、豪  FALSEの場合 韓、中、台
 */
//------------------------------------------------------------------
bool GflvUse::IsJpnFontLoadRegion(void)
{
#if defined(GF_PLATFORM_CTR)
  nn::cfg::CfgRegionCode cr = nn::cfg::GetRegion();
  bool bJpnFontLoadRegion = true;
  if(((cr == nn::cfg::CFG_REGION_KOREA) || (cr == nn::cfg::CFG_REGION_TAIWAN) || (cr == nn::cfg::CFG_REGION_CHINA))){
    bJpnFontLoadRegion = false;
  }
  return bJpnFontLoadRegion;
#elif defined(GF_PLATFORM_WIN32)
  return true;
#endif
}


} // System

