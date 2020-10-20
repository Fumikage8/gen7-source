//===================================================================
/**
 * @file    GameManager.cpp
 * @brief   ゲームで全体を通して管理が必要なクラスのインスタンスを持つクラス
 * @author  k.ohno
 * @date    10.12.22
*/
//===================================================================

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugWinSystem.h>
#include <gfx/include/gfl2_GFGL.h>  //GPU計測用
#include <Layout/include/gfl2_Layout.h>
//#include <ro/gfl_RoManager.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include <system/include/InputDevice/gfl2_InputTypes.h>
#include <system/include/InputDevice/gfl2_Pad.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "System/include/GflUse.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/PlayTimeManager.h"
#include "Print/include/GrammaticalAttributeProvider.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"
#include "Debug/DebugWin/include/DebugPause.h"
#include  <debug/include/gfl2_DebugLogWindow.h>
#include <debug/include/gfl2_DebugWinLocal.h>
#include "FieldScript/include/FieldScriptSystem.h"
#include "Field/FieldStatic/include/SubScreen/FieldSubScreen.h"
#include "Sound/include/Sound.h"
#include "AppLib/include/ui/UIViewManager.h"
#include "Fade/include/gfl2_FadeManager.h"
#include "System/include/Skybox/Skybox.h"
#include "AppLib/include/Fade/AppFade.h"
#include <ExtSavedata/include/NijiExtSaveSystem.h>
#include "Field/FieldStatic/include/BattleInst/BattleInstManager.h"
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"

// RTTIデバッグシステム
#include "Field/FieldRo/include/RttiDebug/RttiEditor.h"
#include "Field/FieldRo/include/RttiDebug/RttiManager.h"

#if defined(GF_PLATFORM_WIN32)
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLGL.h>
#include <system/include/InputDevice/gfl2_KeyBoard.h>
#include <system/include/InputDevice/gfl2_Pad.h>
#endif


#include <gfx/include/gfl2_DrawUtil.h>


// debug
#include "Field/FieldStatic/include/Debug/DebugStartSettingAccessor.h"

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(GameSys::GameManager)
template class gfl2::base::SingletonAccessor<GameSys::GameManager>;

namespace GameSys 
{
  // マイクロ秒を取得
  u64 GetMicroSecond( void )
  {
#if defined(GF_PLATFORM_WIN32)
#if 0
#include <windows.h>
    LARGE_INTEGER tick;
    LARGE_INTEGER frequency;
    if( ::QueryPerformanceCounter(&tick) == FALSE || ::QueryPerformanceFrequency(&frequency) == FALSE )
    {
      return 0ULL;
    }

    return 1000000ULL * (u64)tick.QuadPart  / (u64)frequency.QuadPart ;
#else
    return gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
#endif 
#elif defined(GF_PLATFORM_CTR)
    nn::os::Tick now_tick = nn::os::Tick::GetSystemCurrent();
    return static_cast<s32>(now_tick.ToTimeSpan().GetMicroSeconds());
#else // GF_PLATFORM_CTR
#endif
  }

  // 垂直同期回数を取得
  // win32版は1秒/60を1回とした擬似同期回数を返す
  s32 GetVSyncCount( void )
  {
#if defined(GF_PLATFORM_WIN32)
    s64 begin_time = gfl2::gfx::winOpenGL::WinOpenGLGL::GetVSyncBeginTime();  //VSync計算で使用されている。msの基準値
    return (GetMicroSecond() - begin_time) * 60 / 1000000;
#endif //GF_PLATFORM_WIN32
#if defined(GF_PLATFORM_CTR)
    return nngxCheckVSync(NN_GX_DISPLAY_BOTH);
#endif // GF_PLATFORM_CTR

  }



  /**
   *  @brief  Sleepを使用したVSyncWait
   *  @param  waitStartVCount   Wait開始VCount
   *  @param  vsyncCount        WaitするVCount
   *  @param  isOverflow        VCountオーバーフローしているか？(回り込んでいるか？)
   *  @param  milliseconds      1回のスリープ時間
   */
  void VSyncWaitBySleep( s32 waitStartVCount, s32 vsyncCount, b32 isOverflow, s64 milliseconds )
  {
    if( !isOverflow )
    {
      //vsyncCountになるまでスリープ
      while( vsyncCount > GetVSyncCount() )
      {
  #if defined(GF_PLATFORM_WIN32)
        Sleep( milliseconds );
  #endif //GF_PLATFORM_WIN32
  #if defined(GF_PLATFORM_CTR)
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( milliseconds ) );
  #endif // GF_PLATFORM_CTR
      }
    }
    else
    {
      // wait開始vcountから　vsyncCountになるまでまつ
      while( (waitStartVCount <= GetVSyncCount()) || (vsyncCount > GetVSyncCount()) )
      {
  #if defined(GF_PLATFORM_WIN32)
        Sleep( milliseconds );
  #endif //GF_PLATFORM_WIN32
  #if defined(GF_PLATFORM_CTR)
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( milliseconds ) );
  #endif // GF_PLATFORM_CTR
      }
    }
  }

  // バッファクリア設定、初期値
  static const GameManager::BUFFER_CLEAR_SETTING sc_BufferClearDefaultSettingTable[ gfl2::gfx::CtrDisplayNo::NumberOf ] = 
  {
    // LEFT,           //!< 3DSの左画面 400x240
    {
      gfl2::gfx::Color( 0.3f, 0.3f, 0.3f, 1.0f ),
      1.0f, 
      255.0f,
      GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
    },
    // RIGHT,          //!< 3DSの右画面 400x240
    {
      gfl2::gfx::Color( 0.3f, 0.3f, 0.3f, 1.0f ),
      1.0f, 
      255.0f,
      GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
    },
    // DOWN,           //!< 3DSの下画面 320x240
    {
      gfl2::gfx::Color( 0.3f, 0.3f, 0.3f, 1.0f ),
      1.0f, 
      255.0f,
      GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
    },
  };

#if PM_DEBUG
// DebugTick
#if GAME_MANAGER_DEBUG_TICK_ENABLE
  b32 GameManager::s_TickResetRequest( false );
  gfl2::debug::DebugTickDump GameManager::s_TickWork( GameManager::TickDefine::MAX );
#endif // GAME_MANAGER_DEBUG_TICK_ENABLE

  // DebugTickログ表示名の文字列
  static const c8* const sc_DebugTickGameManagerNameTable[ GameManager::TickDefine::MAX ] = {
#define DEFINE(x,y) y,
#include "GameSys/include/GameManagerDebugTickDefine.h"
#undef DEFINE
  };
  // 各デバイスのログのバッファサイズ
  const u32 GameManager::DEBUG_UI_LOG_BUFFER_MAX[ GameManager::DEBUG_UI_LOG_MAX ] = {
    0x229C, // BUTTON
    0x229C, // BUTTON_EMU
    0x4538, // CROSS
    0x4538, // CROSS_EMU
    0x4538, // STICK
    0x33EA, // TOUCH
  };
#endif // PM_DEBUG


const f32 GameManager::PARALLAX_DEPTH_RANGE_ONE = 1.0f; //視差1(立体視標準)

//----------------------------------------------------------------------------
/**
 *  @brief    コンストラクタ＆デストラクタ
 *  @param    sHeap システムヒープ
 *  @param    dHeap デバイスヒープ
 */
//-----------------------------------------------------------------------------
GameManager::GameManager(gfl2::heap::HeapBase *dHeap) :
  m_ReqFrameMode(FRAMEMODE_MAX),
  m_FrameMode(FRAMEMODE_DEFAULT),
  m_VSyncCount( 0 ),
  m_parallaxDepthRange(PARALLAX_DEPTH_RANGE_ONE),
  m_fatalEvent(false),
  mEnableSkipDraw(SKIP_DRAWMODE_NONE),
  mIsSwapBuffer(false),
  mIsSkipDraw(false),
  mGpuRunSkipCount(0) // @fix GFNMCat[663]
  , m_pDrawingLytWkManager(NULL),  // @fix GFNMCat[3490] 【図鑑】検索画面から戻れなくなる  // デバッグ機能のつもりでPM_DEBUGで括っていたが、デバッグ用途以外で使用している箇所があったので本機能に昇格。PM_DEBUGを外した。
  m_DrawingLytWkManagerAdditionIsEnabled(true)  // 必ず使用するのでtrue
{
  m_bVSyncSkip = false;
  
  for( u32 i = 0; i < gfl2::gfx::CtrDisplayNo::NumberOf; i++ )
  {
    m_BufferClearSettingTable[ i ] = sc_BufferClearDefaultSettingTable[ i ];
  }

  m_deviceHeap = dHeap;

  m_procCellHeap = GFL_CREATE_HEAP(m_deviceHeap, HEAPID_PROCCELL, System::HeapSize::PROCCELL, gfl2::heap::HEAP_TYPE_EXP, false);

  //UIデバイスマネージャのフレームレート初期設定
  gfl2::ui::DeviceManager* p_devman = this->GetUiDeviceManager();
  if( m_FrameMode == FRAMEMODE_60 )
  {
    p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_60 );

#if defined(GF_PLATFORM_WIN32)
    gfl2::system::InputDevice::KeyBoard::SetFrameRate( gfl2::system::InputDevice::FrameRate::RATE_60 );
    gfl2::system::InputDevice::Pad::SetFrameRate( gfl2::system::InputDevice::FrameRate::RATE_60 );
#endif

  }
  else
  {
    p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_30 );  

#if defined(GF_PLATFORM_WIN32)
    // FPS
    gfl2::gfx::GFGL::SetTargetFPS( 30.0f );
    gfl2::system::InputDevice::KeyBoard::SetFrameRate( gfl2::system::InputDevice::FrameRate::RATE_30 );
    gfl2::system::InputDevice::Pad::SetFrameRate( gfl2::system::InputDevice::FrameRate::RATE_30 );
#endif

  }

  p_devman->SynchronizeInputUpdateForAllDevice();

#if PM_DEBUG
  mDebugKeyDisable = false;

  gfl2::heap::HeapBase * p_debug = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);

  for( u32 i=0; i<DEBUG_UI_LOG_MAX; ++i )
  {
    if( p_debug ){
      mDebugUiDevice[i] = GflHeapAllocMemory( p_debug, DEBUG_UI_LOG_BUFFER_MAX[i] );
    }
    else{
      mDebugUiDevice[i] = NULL;
    }
  }
#endif
}


//----------------------------------------------------------------------------
/**
 *  @brief    デストラクタ
 */
//-----------------------------------------------------------------------------
GameManager::~GameManager()
{
#if PM_DEBUG
  for( u32 i=0; i<DEBUG_UI_LOG_MAX; ++i )
  {
    GflHeapSafeFreeMemory( mDebugUiDevice[i] );
  }
#endif // PM_DEBUG


  if( m_procCellHeap != NULL )
  {
    gfl2::heap::Manager::DeleteHeap(m_procCellHeap);
    m_procCellHeap = NULL;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief    初期化
 */
//-----------------------------------------------------------------------------
void GameManager::Initialize(void)
{
#if PM_DEBUG
  // 開発用設定ファイルを読み込む
  gfl2::heap::HeapBase* pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
  Field::Debug::StartSettingAccessor* pDebugStartSettingAccessor = GFL_NEW(pDebugHeap) Field::Debug::StartSettingAccessor(pDebugHeap);
  // ロードはHIO有効時のみ
#if ( GFL_HIO_ENABLE || defined(GF_PLATFORM_WIN32) )
  pDebugStartSettingAccessor->Load(pDebugHeap);
#endif
#if defined(GF_PLATFORM_CTR)
  // HIOオプションの切り替え
  if( pDebugStartSettingAccessor->HasHioEnable() )
  {
    gfl2::fs::AsyncFileManager* pRomAsyncFileManager = GetAsyncFileManager();
    b32 nowFlag = pRomAsyncFileManager->GetHioUseFlag();
    b32 newFlag = pDebugStartSettingAccessor->GetHioEnable();

    if( nowFlag != newFlag )
    { 
      pRomAsyncFileManager->SetHioUseFlag( newFlag );
      System::GflUse::ReloadGarc();
    }
  }
#endif
#endif

  this->CreateProcManager();

#if USE_RTTI_DEBUG
  {
    gfl2::heap::HeapBase * pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEBUGGER );
    gfl2::rttidebug::RttiEditor::getInstance()->Initialize      ( pDebugHeap , 1024*96 );
    gfl2::rttidebug::RttiParamManager::getInstance()->Initialize( pDebugHeap , 1024*96 );
  }
#endif

  // 文法対応モジュール
  m_pGrammaticalAttributeProvider = GFL_NEW(m_deviceHeap) print::GrammaticalAttributeProvider();
  m_pGrammaticalAttributeProvider->Initialize(m_deviceHeap);

  m_GameData = GFL_NEW(m_deviceHeap) GameData( m_deviceHeap, this->GetUiDeviceManager());
#if PM_DEBUG
  m_GameData->Set( pDebugStartSettingAccessor );  // @note HIOを先に有効化するため、外部で生成することにした
#endif


  m_FieldScriptSystem = GFL_NEW(m_deviceHeap) Field::FieldScript::FieldScriptSystem( this, m_deviceHeap );

  // プレイ時間マネージャ初期化
  m_PlayTimeManager = GFL_NEW(m_deviceHeap) PlayTimeManager();
  m_PlayTimeManager->Initialize();

  // @fix GFNMCat[3490] 【図鑑】検索画面から戻れなくなる  // デバッグ機能のつもりでPM_DEBUGで括っていたが、デバッグ用途以外で使用している箇所があったので本機能に昇格。PM_DEBUGを外した。
  // 描画中レイアウトワークのマネージャ  // 全てのレイアウトの先んじて生成する
  {
    gfl2::heap::HeapBase* pHeapForDrawingLytWkManager = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );  // 本機能に昇格したのでHEAPID_DEBUGからHEAPID_RESIDENT_DEVICEに変更。
    m_pDrawingLytWkManager = GFL_NEW(pHeapForDrawingLytWkManager) gfl2::lyt::DrawingLytWkManager(pHeapForDrawingLytWkManager, 256);
  }

  m_pUIViewManager = GFL_NEW(m_deviceHeap) app::ui::UIViewManager();
  m_pSkybox = GFL_NEW(m_deviceHeap) System::Skybox::Skybox( m_GameData->GetEventWork() );

  m_pAppFade = GFL_NEW(m_deviceHeap) AppLib::Fade::AppFade();
  m_pAppFade->Init(m_deviceHeap,m_deviceHeap);

  m_pBattleInstManager = NULL;

  m_pFieldSubScreen = GFL_NEW(m_deviceHeap) Field::SubScreen::FieldSubScreen();

  m_pRotomPowerManager = GFL_NEW(m_deviceHeap) Field::RotomPower::RotomPowerManager(m_deviceHeap);

  this->ClearFrameControl();
  this->SetDefaultRenderingParameter();

#if PM_DEBUG
  ApplyStartSettingAccessorToGameSetting();
#endif

 
}


//----------------------------------------------------------------------------
/**
 *  @brief プロセスマネージャを生成する
 */
//-----------------------------------------------------------------------------
void GameManager::CreateProcManager( void )
{
  GFL_ASSERT( m_procManager == NULL ); //@check
  m_procManager = GFL_NEW(m_deviceHeap) GameProcManager(m_procCellHeap, this);
  m_GameEventManager = GFL_NEW(m_deviceHeap) GameEventManager(this);
}

//----------------------------------------------------------------------------
/**
 *  @brief    メイン処理
 *  @return   処理が終了する場合false 続行中true
 */
//-----------------------------------------------------------------------------
bool GameManager::Process(void)
{
  this->ChangeFrameControl();                // 動作フレームモードの変更

  // プレイ時間更新
  m_PlayTimeManager->Update( m_GameData->GetPlayTime() );
  // ロトムパワー効果時間更新
  m_pRotomPowerManager->Update();

  bool result_proc = this->DoFrameAction();  // 1フレームで行われる処理

  // イベントも、Procも無くなったらゲーム終了
  //return ( result_proc || m_GameEventManager->IsExists() );
	return true; // @fix GFMMCat[2] エージング中にスタッフロール後のセーブ後にアサート 作内にfalseを返すべき状態は無いため、trueを返す。
}




//----------------------------------------------------------------------------
/**
 *  @brief    VSyncWait
 */
//-----------------------------------------------------------------------------
void GameManager::VSyncWait(void)
{
#if PM_DEBUG
  static b32 DebugTickStart = false;
#endif // PM_DEBUG

  bool is_boost = false;

#if PM_DEBUG
#if defined(GF_PLATFORM_WIN32)
  // L2 押しっぱなしで高速動作。
  {
    if( gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::L2 ) )
    {
      is_boost = true;

      // 下位のスレッドに1000usのみ処理を渡す。
      gfl2::thread::ThreadStatic::CurrentSleep(1);
    }
  }
#endif  // defined(GF_PLATFORM_WIN32)
#endif // PM_DEBUG

  if( m_bVSyncSkip )
  {
    is_boost = true;
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }
  

  // 最後のvcountまでスリープする。
  {
    s32 vsyncWait = this->GetFrameControlVSyncCount( m_FrameMode ) - 1;

    if( vsyncWait )
    {
      b32 overflow = false;
      s32 checkVCount = this->CalcVSyncCount( m_VSyncCount, vsyncWait, &overflow );
      //TOMOYA_PRINT( "VSyncWait pre start %d check %d now %d\n", m_VSyncCount, checkVCount, GetVSyncCount() );
      if( !is_boost )
      {
        VSyncWaitBySleep( m_VSyncCount, checkVCount, overflow, 1 );
      }
    }
  }

  bool isCommandDone = false;

  // @fix GFNMCat[3478] フィールド移動中に上画面がカクつく事があります。 
  // WaitGpuCommandDone内でSwapBufferリクエストを発行している。 CTRはリクエスト後の最初のVSyncで画面が更新されるため、
  // 必ず1/60たってからSwapBufferリス絵ストを発行しないと、たまに1/60のタイミングで画面が更新してしまうことがある。
  // Gpu待ち
  if( (this->EnableSkipDrawMode() == SKIP_DRAWMODE_FPS_HALF) && (GetIsSkipDraw() == true) ) //@fix SKIP_DRAWMODE_FPS_HALFのときのGPUコマンド実行待ちが2/30のタイミングになっていない。
  {
    //GFL_RELEASE_PRINT( "WaitGpuCommandDone Skip\n" );
  }
  else
  {
    this->WaitGpuCommandDone();
    isCommandDone = true;
  }

#if PM_DEBUG
  if( DebugTickStart ) Debug::DebugWin::TickEnd(Debug::DebugWin::DEBUGWIN_TICK_CPU);
#endif

  {
    // このフレーム描画コマンドをつんだか保存
    bool isDraw = !GetIsSkipDraw();

    // VSyncまちを実行　すでに超えていたら、vsyncをスキップする。
    s32 vcount = this->GetFrameControlVSyncCount( m_FrameMode );
    b32 is_overflow = false;
    s32 checkVCount = CalcVSyncCount( m_VSyncCount, vcount, &is_overflow );
    b32 is_vsyncSkip = false;
    s32 nowVCount = GetVSyncCount(); // @fix GFNMCat[663] 最もチェックの近くで、VSyncカウントを取得する。

    //TOMOYA_PRINT( "VSyncWait start %d check %d now %d\n", m_VSyncCount, checkVCount, GetVSyncCount() );
    // 範囲内!
    if( (!is_overflow && (checkVCount > nowVCount)) || (is_overflow && ((checkVCount > nowVCount) || (m_VSyncCount <= nowVCount))) )
    {
      if(isCommandDone)
      {
        gfl2::gfx::GFGL::SwapDisplay_SwapBuffers();// @fix GFNMCat[663] CTRでマップを移動していると画面右側（画面端）が乱れる
      }

      this->SetIsSwapBuffer( isDraw );  // 実際に描画コマンドをつんだ時にSwapBufferフラグをたてる。
      if( !is_boost )
      {
        gfl2::gfx::GFGL::SwapDisplay_WaitVSync();
      }
    }
    // 処理落ち状態 
    else
    {
#if defined(GF_PLATFORM_WIN32)
      if(isCommandDone)
      {
        gfl2::gfx::GFGL::SwapDisplay_SwapBuffers(); // @fix GFNMCat[663] CTRでマップを移動していると画面右側（画面端）が乱れる
      }
      this->SetIsSwapBuffer( isDraw );  // 実際に描画コマンドをつんだ時にSwapBufferフラグをたてる。
      gfl2::gfx::GFGL::SwapDisplay_WaitVSync(); // 関数内でVsync待ちのスキップ処理が入っている。

      // 処理落ちしたためVSYNCはスキップするが、
      // 下位のスレッドに処理を渡す。
      gfl2::thread::ThreadStatic::CurrentSleepMicro(100);

#elif defined(GF_PLATFORM_CTR)
      {
        // @fix GFNMCat[663] CTRでマップを移動していると画面右側（画面端）が乱れる
        switch(this->EnableSkipDrawMode())
        {
        case SKIP_DRAWMODE_NONE:  //【スキップなしモード】 常にレンダリングを実行
          is_vsyncSkip = false;
          break;

        case SKIP_DRAWMODE_CPU_OVERFLOW_LIGHT:  //【1フレーム描画スキップモード】
          // 2フレーム連続で描画をスキップしない
          if( mGpuRunSkipCount >= 1 )
          {
            is_vsyncSkip = false;
          }
          else
          {
            is_vsyncSkip = true;
          }
          break;

        case SKIP_DRAWMODE_CPU_OVERFLOW:  //【1フレーム描画スキップモード】
          // 3フレーム連続で処理落ちでの描画スキップをしない
          if( mGpuRunSkipCount >= 2 )
          {
            is_vsyncSkip = false;
          }
          else
          {
            is_vsyncSkip = true;
          }
          break;

        case SKIP_DRAWMODE_FPS_HALF:  //【常にFPSを半分にする。 】
          is_vsyncSkip = false;
          break;
        }

        // 処理落ちしていてもVSYNC待ちを実行する必要がある。
        if( !is_vsyncSkip )
        {
          if(isCommandDone)
          {
            gfl2::gfx::GFGL::SwapDisplay_SwapBuffers(); // @fix GFNMCat[663] CTRでマップを移動していると画面右側（画面端）が乱れる
          }
          this->SetIsSwapBuffer( isDraw );  // 実際に描画コマンドをつんだ時にSwapBufferフラグをたてる。

          if( !is_boost )
          {
            gfl2::gfx::GFGL::SwapDisplay_WaitVSync(); // 関数内でVsync待ちのスキップ処理が入っている。
          }
        }
        else
        {
          // 処理落ちしたためVSYNCはスキップするが、
          // 下位のスレッドに処理を渡す。
          gfl2::thread::ThreadStatic::CurrentSleepMicro(100);
        }
      }
#endif
    }

    //TOMOYA_PRINT( "vCount now %d vsyncSkip %d\n", GetVSyncCount(), is_vsyncSkip );
    // 描画スキップを行なうかチェックする。
    this->checkDrawSkip( is_vsyncSkip );

#if PM_DEBUG
    Debug::DebugWin::TickStart(Debug::DebugWin::DEBUGWIN_TICK_CPU);
    Debug::DebugWin::UpdateSystem();
    DebugTickStart = true;
#endif
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief    コマンドリストの実行
 */
//-----------------------------------------------------------------------------
void GameManager::RunCommandList(void)
{
#if PM_DEBUG
  // コマンド系の数値を保存
  {
#if USE_DEBUG_WIN
#if defined(GF_PLATFORM_CTR)
#if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)
    GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->mCommandBufferUseSize = gfl2::gfx::GFGL::GetCommandListUsedBufferSize();
    GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->mCommandBufferMaxSize = gfl2::gfx::GFGL::GetCommandListMaxBufferSize();
    GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->mCommandRequestCount = gfl2::gfx::GFGL::GetCommandListUsedRequestCount();
    GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->mCommandRequestMax = gfl2::gfx::GFGL::GetCommandListMaxRequestCount();
#endif //defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)
#endif  //プラットフォーム分岐    
#endif //USE_DEBUG_WIN
  }
#endif
  if( (this->EnableSkipDrawMode() == SKIP_DRAWMODE_FPS_HALF) && (this->GetIsSwapBuffer() == false) ) //@fix SKIP_DRAWMODE_FPS_HALFのときのGPUコマンド実行待ちが2/30のタイミングになっていない。
  {
    //GFL_RELEASE_PRINT( "CommandRun Skip\n" );
    mGpuRunSkipCount ++;  // スキップした回数をカウント
    m_VSyncCount = GetVSyncCount();
  }
  else
  {
    if(this->GetIsSwapBuffer()) // 描画コマンドをつんでSwapしたときだけ実行
    {
      //GFL_RELEASE_PRINT( "CommandRun\n\n" );
      // @fix GFNMCat[663] CTRでマップを移動していると画面右側（画面端）が乱れる
      mGpuRunSkipCount = 0; // 通常描画したのでスキップカウントをクリア
    }
    else
    {
      // @fix GFNMCat[663] CTRでマップを移動していると画面右側（画面端）が乱れる
      // コマンドのクリア処理
#if defined(GF_PLATFORM_CTR)
      gfl2::gfx::GFGL::GetCurrentCommandList()->Clear();
#endif

      //GFL_RELEASE_PRINT( "CommandNotRun\n" );
      mGpuRunSkipCount ++;  // スキップした回数をカウント
    }

    gfl2::gfx::GFGL::SwapDisplay_RunGPUCommand(); 

    m_VSyncCount = GetVSyncCount();
    SetIsSwapBuffer(false);
  }

#if PM_DEBUG
  // GPU実行時間を保存
  {
    Debug::DebugWin::TickCalc(Debug::DebugWin::DEBUGWIN_TICK_GPU,gfl2::gfx::GFGL::GetTotalGPUTimeMicroSecond());
  }
#endif
}




//----------------------------------------------------------------------------
/**
 *  @brief 1フレームで行う処理を実行する
 *
 *  @retval true    途中
 *  @retval false   終了（プロセスがなくなった）
 */
//-----------------------------------------------------------------------------
bool GameManager::DoFrameAction( void )
{
  bool result_proc   = true;

#if PM_DEBUG
  bool is_before_ui_playlog = this->DebugUiDeviceIsPlayLog();
#endif

  // 
  {
#if PM_DEBUG
    // R＋ L + X　で、キー無効状態で、デバッグウィンドウをオープン
    this->GetUiDeviceManager()->SetDebugKeyDisable( false );  // キー有効

    //if( !IsDebugMode() )
    {
      bool is_before_open = gfl2::debug::DebugWin_IsOpen();

      // デバッグキー　ログプレイ中でも、オリジナルのインプットON
      DebugUiDeviceSoftwareControlEnabel(false);
      {

        gfl2::debug::DebugWin_Update();
      }
      DebugUiDeviceSoftwareControlEnabel(true);
      // デバッグキー　ログプレイ中でも、オリジナルのインプットOFF

      // ここでOPENになった。
      if( gfl2::debug::DebugWin_IsOpen() && !is_before_open )
      {
        // デバッグキーレコード・プレイ停止
        this->DebugUiDeviceEndLogRec();
        this->DebugUiDeviceEndPlayLog();
        // this->Debug_StopInputRandom();


        // UPも押されていたら、キー無効状態に
        if( this->GetUiDeviceManager()->GetButton(0)->IsHold( gfl2::ui::BUTTON_UP ) )
        {
          mDebugKeyDisable = true;
        }
      }
    }

    gfl2::debug::DebugLogWindow*  pLogWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugLogWindow); 
    if( pLogWin )
    {
      pLogWin->Update();
    }

#endif
  }

  // デバイスの更新をカット
#if GFL_DEBUG
  // @fix 社内要望：ログ再生の初回プロック更新は入力取得を無効にする、Uiの更新が実行されるまで生入力が返ってしまう為。
  if(this->IsDebugKeyDisable() || ( !is_before_ui_playlog && this->DebugUiDeviceIsPlayLog()) )
  {
    this->GetUiDeviceManager()->SetDebugKeyDisable( true );
  }
#endif // PM_DEBUG

  {
#if GAME_MANAGER_DEBUG_TICK_ENABLE
    if( s_TickResetRequest )
    {
      GAME_MANAGER_DEBUG_TICK_RESET();
      s_TickResetRequest = false;
    }
#endif  

    GAME_MANAGER_DEBUG_TICK_START( TickDefine::UPDATE );
    {
      Debug::DebugWin::PauseSystem_Update();

      if( !isPause() )
      {
        //GFL_RELEASE_PRINT( "GameManager::DoProcessStart\n" );
        Debug::DebugWin::TickStart(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);

        // イベント更新
        GAME_MANAGER_DEBUG_TICK_START( TickDefine::UPDATE_EVENT );
        this->UpdateEventProcess();
        GAME_MANAGER_DEBUG_TICK_END( TickDefine::UPDATE_EVENT );

        // 1/60プロセス処理
        GAME_MANAGER_DEBUG_TICK_START( TickDefine::UPDATE_PROC );
        result_proc = this->UpdateProcManager();
        GAME_MANAGER_DEBUG_TICK_END( TickDefine::UPDATE_PROC );

        m_pAppFade->Update();

        Debug::DebugWin::TickEnd(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);

        //GFL_RELEASE_PRINT( "GameManager::DoProcessEnd\n" );
      }
    }
    
    // サウンドシステムメイン
    GAME_MANAGER_DEBUG_TICK_START( TickDefine::UPDATE_SOUND );
#if SOUND_UPDATE_THREAD_ENABLE
    Sound::UpdateThreadSync();
#else
    Sound::Update();
#endif
    GAME_MANAGER_DEBUG_TICK_END( TickDefine::UPDATE_SOUND );

    GAME_MANAGER_DEBUG_TICK_END( TickDefine::UPDATE );
  }

  // 描画スキップチェック
  if( this->GetIsSkipDraw() == false )
  {
    //GFL_RELEASE_PRINT( "GameManager::Draw\n" );
    this->DrawProcManager();
  }
  else
  {
    //GFL_RELEASE_PRINT( "GameManager::Draw skip \n" );
  }


#if PM_DEBUG
  // ここで、デバッグキー無効を解消する。
  // こうしないと、デバッグウィンドウを消したフレームで、
  // アプリケーションがキー入力してしまう。
  if( !gfl2::debug::DebugWin_IsOpen() ){ mDebugKeyDisable = false; }
#endif // PM_DEBUG

  return result_proc;
}

//----------------------------------------------------------------------------
/**
 *  @brief    GPU処理の実行待ち
 */
//-----------------------------------------------------------------------------
void GameManager::WaitGpuCommandDone()
{
  // @fix GFNMCat[663] CTRでマップを移動していると画面右側（画面端）が乱れる
  // SwapBuffer
  if( gfl2::gfx::GFGL::SwapDisplay_WaitGPUCommandDone() )
  {
   // GFL_RELEASE_PRINT( "WaitDoneCommandList\n" );
  }
  else
  {
    //GFL_RELEASE_PRINT( "NotWaitDoneCommandList\n" );
  }

  // @fix GFNMCat[3490] 【図鑑】検索画面から戻れなくなる  // デバッグ機能のつもりでPM_DEBUGで括っていたが、デバッグ用途以外で使用している箇所があったので本機能に昇格。PM_DEBUGを外した。
  // レイアウトが描画中か否かの管理
  {
    if(m_pDrawingLytWkManager) m_pDrawingLytWkManager->SwapList();
  }

  // @fix GFNMCat[663] CTRでマップを移動していると画面右側（画面端）が乱れる
  // 下記タイミングでSwapすると、VSyncオーバー後にSwapしてしまう可能性がある。
  // VSync直前に移動する
  //gfl2::gfx::GFGL::SwapDisplay_SwapBuffers();

  // バッファを切り替え
  gfl2::gfx::DrawUtil::SwapBuffers();
}

/**
 *  @brief  フレームモードごとの待ちVCount数の取得
 */
s32 GameManager::GetFrameControlVSyncCount( FrameMode mode ) const  // 
{
  switch( mode )
  {
  case FRAMEMODE_60:   ///< 1/60
    return 1;

  case FRAMEMODE_30:   ///< 1/30
    return 2;

    /*
  case FRAMEMODE_20:   ///< 1/20
    return 3;

  case FRAMEMODE_15:   ///< 1/15
    return 4;
    */
  }

  return 1;
}

/**
 *  @brief  Overflowを考慮した、VCountの加算
 */
s32 GameManager::CalcVSyncCount( s32 vcount, s32 add, b32 * p_overflow ) const // ?B
{
  s32 diff = 0x7ffffffe - vcount;
  s32 checkVCount = 0;

  diff -= add;
  if(diff < 0)
  {
    checkVCount =  gfl2::math::Abs(diff) - 1;
    *p_overflow = true;
  }
  else
  {
    checkVCount = vcount + add;
    *p_overflow = false;
  }

  return checkVCount;
}


/**
 *  @brief  DrawSkipするかチェック
 */
void GameManager::checkDrawSkip(b32 isVsyncOverFlow)
{
  switch( this->EnableSkipDrawMode() )
  {
  case SKIP_DRAWMODE_CPU_OVERFLOW:
    if( isVsyncOverFlow )         // CPUがオーバーフローしたときのみ
    {
      this->SetIsSkipDraw(true);
    }
    else
    {
      this->SetIsSkipDraw(false);
    }
    break;

  case SKIP_DRAWMODE_FPS_HALF:
    if( this->GetIsSkipDraw()==false )
    {
      this->SetIsSkipDraw(true);  // 常に2回に1回描画をスキップ
    }
    else
    {
      this->SetIsSkipDraw(false);  // 常に2回に1回描画をスキップ
    }
    break;

  default:
    //TOMOYA_PRINT( "GameManager::DrawSkip\n" );
    this->SetIsSkipDraw(false);
    break;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief プロセスマネージャを更新する
 *
 *  @retval true    途中
 *  @retval false   終了（プロセスがなくなった）
 */
//-----------------------------------------------------------------------------
bool GameManager::UpdateProcManager( void )
{
  if( m_procManager ){
    const gfl2::proc::MainStatus ret = m_procManager->Main();
    if(gfl2::proc::MAINSTATUS_NULL == ret ){
      //ゲーム終了
      return false;
    }
  }
  return true;
}


//----------------------------------------------------------------------------
/**
 *  @brief プロセスマネージャを 1/30フレーム動作時の2/60 更新する
 *
 *  @retval true    途中
 *  @retval false   終了（プロセスがなくなった）
 */
//-----------------------------------------------------------------------------
bool GameManager::NextFrameUpdateProcManager( void )
{
  if( m_procManager ){
    const gfl2::proc::MainStatus ret = m_procManager->NextFrameMain();
    if(gfl2::proc::MAINSTATUS_NULL == ret){
      //ゲーム終了
      return false;
    }
  }
  return true;
}


//----------------------------------------------------------------------------
/**
 *  @brief  描画処理
 */
//-----------------------------------------------------------------------------
void GameManager::DrawProcManager( void )
{
  if( m_procManager )
  {
#if 0
#if PM_DEBUG
    if( !IsDebugMode() )
#endif
#endif
    {
      {
        GAME_MANAGER_DEBUG_TICK_START( TickDefine::DRAW );
        Debug::DebugWin::TickStart(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
        // 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行
        if( m_procManager->GetDrawFlag() )
        {
          GAME_MANAGER_DEBUG_TICK_START( TickDefine::DRAW_PRE );
          m_procManager->PreDraw();
          GAME_MANAGER_DEBUG_TICK_END( TickDefine::DRAW_PRE );
          m_pAppFade->PreDraw();
        }


        for( u32 i = 0; i < gfl2::gfx::CtrDisplayNo::NumberOf; i++ )
        {
#if GAME_MANAGER_DEBUG_TICK_ENABLE
          TickDefine::Index drawTalbe[ gfl2::gfx::CtrDisplayNo::NumberOf ] = { TickDefine::DRAW_LEFT, TickDefine::DRAW_RIGHT, TickDefine::DRAW_DOWN };
          TickDefine::Index drawProcTalbe[ gfl2::gfx::CtrDisplayNo::NumberOf ] = { TickDefine::DRAW_LEFT_PROC, TickDefine::DRAW_RIGHT_PROC, TickDefine::DRAW_DOWN_PROC };
          TickDefine::Index drawDebugTalbe[ gfl2::gfx::CtrDisplayNo::NumberOf ] = { TickDefine::DRAW_LEFT_DEBUG, TickDefine::DRAW_RIGHT_DEBUG, TickDefine::DRAW_DOWN_DEBUG };
#endif

#if defined(GF_PLATFORM_WIN32)

          // PCでは右画面の描画をスキップ
          if( i == gfl2::gfx::CtrDisplayNo::RIGHT )
          {
            continue;
          }

#endif //GF_PLATFORM_WIN32
#if defined(GF_PLATFORM_CTR)

          // 立体視OFF時は右画面の描画をスキップ
          if( i == gfl2::gfx::CtrDisplayNo::RIGHT && nn::gx::CTR::IsStereoVisionAllowed() == false )
          {
            continue;
          }
          if( i == gfl2::gfx::CtrDisplayNo::RIGHT && gfl2::gfx::ctr::CTRGL::IsStereoModeEnable() == false )
          {
            continue;
          }

#endif // GF_PLATFORM_CTR
          
          GAME_MANAGER_DEBUG_TICK_START( drawTalbe[i] );

          // ディスプレイ設定
          gfl2::gfx::GFGL::SetActiveDisplay( System::GflUse::m_HandleList[i] );
          
          // レンダーターゲット設定
          gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ i ] );
          gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ i ] );
          gfl2::gfx::GFGL::SetRenderTarget( pBackBuffer );
          gfl2::gfx::GFGL::SetDepthStencil( pDepthBuffer );

          // レンダーターゲットクリア処理
          // @note BeginScene()内部でRenderTarget及び、DepthStencilを参照する。Setされていない場合はアクセス例外が発生します 150224 ikuta_junya
          if( gfl2::gfx::GFGL::BeginScene() )
          {
            const BUFFER_CLEAR_SETTING& rSetting = m_BufferClearSettingTable[ i ];

            // ビューポート設定
            u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ i ] );
            u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ i ] );

            if ( displayWidth && displayHeight )
            {
              gfl2::gfx::GFGL::SetViewport( 0, 0, displayWidth, displayHeight );
              gfl2::gfx::GFGL::SetScissor( 0, 0, displayWidth, displayHeight );
            }
            else
            {
              const gfl2::gfx::Surface::SurfaceDesc     &rDesc = gfl2::gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
              gfl2::gfx::GFGL::SetViewport( 0, 0, rDesc.width, rDesc.height);
              gfl2::gfx::GFGL::SetScissor( 0, 0, rDesc.width, rDesc.height);
            }

            // カラー＆深度ステンシル
            if( rSetting.clearTarget == ( BUFFER_CLEAR_TARGET_BIT_COLOR | BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL ) )
            {
              gfl2::gfx::GFGL::ClearRenderTargetDepthStencil( rSetting.color, rSetting.depth, rSetting.stencil );
            }
            else 
            {
              // カラー
              if( rSetting.clearTarget & BUFFER_CLEAR_TARGET_BIT_COLOR )
              {
                gfl2::gfx::GFGL::ClearRenderTarget( rSetting.color );
              }

              // 深度ステンシル
              if( rSetting.clearTarget & BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL )
              {
                gfl2::gfx::GFGL::ClearDepthStencil( rSetting.depth, rSetting.stencil );
              }
            }
            gfl2::gfx::GFGL::EndScene();
          }

          // 登録したPROCによる、フレームバッファへの描画を実行します
          
          {
            if( m_procManager->GetDrawFlag() )
            {
              GAME_MANAGER_DEBUG_TICK_START( drawProcTalbe[ i ] );
              m_procManager->Draw( static_cast< gfl2::gfx::CtrDisplayNo >( i ) );
              GAME_MANAGER_DEBUG_TICK_END( drawProcTalbe[ i ] );
            }

            m_pAppFade->Draw(static_cast< gfl2::gfx::CtrDisplayNo >( i ));
#if PM_DEBUG
            GAME_MANAGER_DEBUG_TICK_START( drawDebugTalbe[ i ] );
            gfl2::debug::DebugWin_Draw(static_cast< gfl2::gfx::CtrDisplayNo >( i ) , System::GflUse::m_HandleList[i]);
            GAME_MANAGER_DEBUG_TICK_END( drawDebugTalbe[ i ] );

            {
              gfl2::debug::DebugLogWindow*  pLogWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugLogWindow); 
              if( pLogWin && GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->IsOpen() == false )
              {
                pLogWin->Draw(static_cast< gfl2::gfx::CtrDisplayNo >( i ), System::GflUse::m_HandleList[ i ]);
              }
            }
#endif
            gfl2::gfx::GFGL::TransferDisplay(); 
          }
          
          // 
          GAME_MANAGER_DEBUG_TICK_END( drawTalbe[i] );
        }

        Debug::DebugWin::TickEnd(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
        GAME_MANAGER_DEBUG_TICK_END( TickDefine::DRAW );
        
      }
    }
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  イベント処理
 */
//-----------------------------------------------------------------------------
void GameManager::UpdateEventProcess( void )
{
  // NULLかどうかだけチェック
  if( m_GameEventManager->isExists() == false )
  {
    // イベント実行前　イベント検知処理
    m_procManager->EventCheck();
  }

  // イベント動作
  m_GameEventManager->Main();
}


//----------------------------------------------------------------------------
/**
 *  @brief    開放処理
 */
//-----------------------------------------------------------------------------
void GameManager::Finalize(void)
{
  // @fix GFNMCat[3490] 【図鑑】検索画面から戻れなくなる  // デバッグ機能のつもりでPM_DEBUGで括っていたが、デバッグ用途以外で使用している箇所があったので本機能に昇格。PM_DEBUGを外した。
  // 描画中レイアウトワークのマネージャ
  {
    GFL_SAFE_DELETE(m_pDrawingLytWkManager);
  }

  GFL_SAFE_DELETE(m_pRotomPowerManager);
  GFL_SAFE_DELETE( m_pFieldSubScreen);
  GFL_SAFE_DELETE( m_FieldScriptSystem );
  m_pGrammaticalAttributeProvider->Finalize();
  GFL_SAFE_DELETE( m_pGrammaticalAttributeProvider );
  GFL_SAFE_DELETE( m_pSkybox );
  GFL_SAFE_DELETE( m_pUIViewManager );
  GFL_SAFE_DELETE( m_GameData );
  GFL_SAFE_DELETE( m_PlayTimeManager );

  GFL_SAFE_DELETE(m_pAppFade);

#if USE_RTTI_DEBUG
  gfl2::rttidebug::RttiEditor::getInstance()->Terminate();
  gfl2::rttidebug::RttiParamManager::getInstance()->Terminate();
#endif

  this->DeleteProcManager();
}


//----------------------------------------------------------------------------
/**
 *  @brief プロセスマネージャを破棄する
 */
//-----------------------------------------------------------------------------
void GameManager::DeleteProcManager( void )
{
  GFL_SAFE_DELETE( m_GameEventManager );
  GFL_SAFE_DELETE( m_procManager );
}


//----------------------------------------------------------------------------
/**
 *  @brief    動作フレーム変更リクエスト
 */
//-----------------------------------------------------------------------------
void GameManager::RequestFrameMode( FrameMode mode )
{
  this->RequestFrameControl(mode);
}


//----------------------------------------------------------------------------
/**
 *  @brief    レンダリングのパラメータにデフォルト値を設定する。
 */
//-----------------------------------------------------------------------------
void GameManager::SetDefaultRenderingParameter( void )
{
  m_parallaxDepthRange = PARALLAX_DEPTH_RANGE_ONE;
}


//----------------------------------------------------------------------------
/**
 *  @brief    レンダリングのパラメータを設定する。
 *            設定し直すまでこの値が使用され続けます。
 *
 *  @param    parallaxDepthRange  立体視の視差の補正値
 *                                 (gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
 *                                  gfl2::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
 */
//-----------------------------------------------------------------------------
void GameManager::SetRenderingParameter( const f32 parallaxDepthRange )
{
  m_parallaxDepthRange = parallaxDepthRange;
}


//----------------------------------------------------------------------------
/**
 *  @brief    レンダリングのパラメータを取得する。
 *
 *  @param[out]    parallaxDepthRange  立体視の視差の補正値
 *                                      (gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
 *                                       gfl2::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
 */
//-----------------------------------------------------------------------------
void GameManager::GetRenderingParameter( f32* parallaxDepthRange ) const
{
  if( parallaxDepthRange )
  {
    *parallaxDepthRange = m_parallaxDepthRange;
  }
}


// @fix SBTS[2501] : 【表示：特定のデモ中に立体視をONにすると右目用映像に不具合が発生する】
//----------------------------------------------------------------------------
/**
 *  @brief  右目がレンダリングされるか？
 *
 *  @retval true  レンダリングされる
 *  @retval false レンダリングされない
 */
//-----------------------------------------------------------------------------
bool GameManager::IsUpperRightRendering( void ) const
{
  return false;
#if 0
  // 視差がつかない場合は、右目をレンダリングしていない。
  if( (m_parallaxDepthRange * (nn::gx::IsStereoVisionAllowed() ? 1.0f:0.0f)) == gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ZERO ){
    return false;
  }
  return true;
#endif
}


//-----------------------------------------------------------------------------
/**
 * @brief ファイル読み込みマネージャを取得する
 */
//-----------------------------------------------------------------------------
gfl2::fs::AsyncFileManager* GameManager::GetAsyncFileManager( void ) const
{
  return System::GflUse::m_Singlton.m_romAsyncFileManager;
}


//-----------------------------------------------------------------------------
/**
 * @brief UI デバイスマネージャを取得する
 */
//-----------------------------------------------------------------------------
gfl2::ui::DeviceManager* GameManager::GetUiDeviceManager( void ) const
{
  return System::GflUse::m_Singlton.m_pUiDeviceManager;
}


//----------------------------------------------------------------------------
/**
 * @brief 拡張セーブデータ管理システムの取得
 */
//----------------------------------------------------------------------------
ExtSavedata::NijiExtSaveSystem * GameManager::GetNijiExtSaveSystem( void ) const
{
  return System::GflUse::m_Singlton.m_pNijiExtSaveSystem;
}

//-----------------------------------------------------------------------------
/**
 * @brief 現在動作しているプロセスがあるかチェック
 */
//-----------------------------------------------------------------------------
bool GameManager::IsProcessExists( void ) const
{
  if( m_procManager->GetBaseProcess() == NULL ){
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 現在動作しているGameEventがあるかチェック
 */
//-----------------------------------------------------------------------------
bool GameManager::IsEventExists( void ) const
{
  return m_GameEventManager->IsExists();
}

// バッファクリア設定

/**
* @brief  指定画面のクリア設定を設定する
*
*  @param  display   対象画面
*  @param  rSetting  クリア設定
*
*  @return 無し
*/
void GameManager::SetBufferClearSetting( gfl2::gfx::CtrDisplayNo display, const BUFFER_CLEAR_SETTING& rSetting )
{
  if( display >= gfl2::gfx::CtrDisplayNo::NumberOf )
  {
    return;
  }

  m_BufferClearSettingTable[ display ] = rSetting;
}

/**
* @brief  指定画面のクリア設定を取得する
*
*  @param  display   対象画面
*  @param  pOut      格納先
*
*  @return 成否
*/
b32 GameManager::GetBufferClearSetting( gfl2::gfx::CtrDisplayNo display, BUFFER_CLEAR_SETTING* pOut ) const
{
  if( display >= gfl2::gfx::CtrDisplayNo::NumberOf )
  {
    return false;
  }

  *pOut = m_BufferClearSettingTable[ display ];
  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  フレーム管理の初期化
 */
//-----------------------------------------------------------------------------
void GameManager::ClearFrameControl( void )
{
  m_ReqFrameMode= FRAMEMODE_MAX;
  m_FrameMode   = FRAMEMODE_DEFAULT;
}


//----------------------------------------------------------------------------
/**
 *  @brief  今のモードチェック
 *
 *  @param  mode  調べたいモード
 *
 *  @retval true  そのモード
 *  @retval false 違うモード
 */
//-----------------------------------------------------------------------------
bool GameManager::IsFrameControl( FrameMode mode ) const   //
{
  return ( m_FrameMode == mode );
}


//----------------------------------------------------------------------------
/**
 *  @brief  フレームの更新処理
 */
//-----------------------------------------------------------------------------
void GameManager::ChangeFrameControl( void )
{
  // リクエストが無ければ、何もしない。
  if( m_ReqFrameMode == FRAMEMODE_MAX ){
    return ;
  }

  // 変更
  if( m_FrameMode != m_ReqFrameMode ){
    
    gfl2::ui::DeviceManager* p_devman = this->GetUiDeviceManager();

    if( m_ReqFrameMode == FRAMEMODE_60 ){

      // キー入力
      p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_60 );
      p_devman->SynchronizeInputUpdateForAllDevice();

#if defined(GF_PLATFORM_WIN32)
      // FPS
      gfl2::gfx::GFGL::SetTargetFPS( 60.0f );
      gfl2::system::InputDevice::KeyBoard::SetFrameRate( gfl2::system::InputDevice::FrameRate::RATE_60 );
      gfl2::system::InputDevice::Pad::SetFrameRate( gfl2::system::InputDevice::FrameRate::RATE_60 );

#endif

    }
    // 1/30フレーム動作
    else{

      // 
      p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_30 );
      p_devman->SynchronizeInputUpdateForAllDevice();

#if defined(GF_PLATFORM_WIN32)
      // FPS
      gfl2::gfx::GFGL::SetTargetFPS( 30.0f );
      gfl2::system::InputDevice::KeyBoard::SetFrameRate( gfl2::system::InputDevice::FrameRate::RATE_30 );
      gfl2::system::InputDevice::Pad::SetFrameRate( gfl2::system::InputDevice::FrameRate::RATE_30 );
#endif
    }

    // Topフレームから開始
    
    m_FrameMode  = m_ReqFrameMode;
  }

  m_ReqFrameMode = FRAMEMODE_MAX;
}


//----------------------------------------------------------------------------
/**
 *  @brief  フレームコントロールのモード変更リクエスト設定
 *
 *  @param  mode  変更したモード
 */
//-----------------------------------------------------------------------------
void GameManager::RequestFrameControl( FrameMode mode )
{
  if( mode < FRAMEMODE_MAX ){
    m_ReqFrameMode = mode;
  }else{
    GFL_ASSERT( mode < FRAMEMODE_MAX ); //@check
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief   止めるかどうか
 */
//-----------------------------------------------------------------------------
bool GameManager::isPause( void )
{
  if( Debug::DebugWin::PauseSystem_IsPause() ){
    return true;
  }
  return false;
}


#if PM_DEBUG
//-----------------------------------------------------------------------------
/**
 * @brief UIログ再生の一時停止設定
 */
//-----------------------------------------------------------------------------
void GameManager::DebugUiDeviceSoftwareControlEnabel( bool isEnable )
{
  /**
   * @brief 十字キーを取得する
   * @param keyID  取得する十字キーのID
   */
  gfl2::ui::VectorDevice* pCross = GetUiDeviceManager()->GetCrossKey( 0 );
  pCross->SetSoftwareControlEnable(isEnable);

  /**
   * @brief アナログスティックを取得する
   * @param stickID  取得するアナログスティックのID
   */
  gfl2::ui::VectorDevice* pStick = GetUiDeviceManager()->GetStick( 0 );
  pStick->SetSoftwareControlEnable(isEnable);

  /**
   * @brief ボタンを取得する
   * @param buttonID  取得するボタンのID
   */
  gfl2::ui::Button* pButton = GetUiDeviceManager()->GetButton( 0 );
  pButton->SetSoftwareControlEnable(isEnable);

  /**
   * @brief タッチパネルを取得する
   * @param tpID  取得するタッチパネルのID
   */
  gfl2::ui::TouchPanel* pTouch = GetUiDeviceManager()->GetTouchPanel( 0 );
  pTouch->SetSoftwareControlEnable(isEnable);
}


//-----------------------------------------------------------------------------
/**
 * @brief UIログセーブ開始
 */
//-----------------------------------------------------------------------------
void GameManager::DebugUiDeviceStartLogRec( void )
{
  /**
   * @brief 十字キーを取得する
   * @param keyID  取得する十字キーのID
   */
  gfl2::ui::VectorDevice* pCross1 = GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  gfl2::ui::VectorDevice* pCross2 = GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
  pCross1->SetLogBuffer( mDebugUiDevice[ DEBUG_UI_LOG_CROSS ], DEBUG_UI_LOG_BUFFER_MAX[ DEBUG_UI_LOG_CROSS ] );
  pCross2->SetLogBuffer( mDebugUiDevice[ DEBUG_UI_LOG_CROSS_EMU ], DEBUG_UI_LOG_BUFFER_MAX[ DEBUG_UI_LOG_CROSS_EMU ] );

  pCross1->StartLogRec(); // 0番目のデバイス
  pCross2->StartLogRec(); // 1番目のデバイス
  ICHI_PRINT("pCross1:ログ最大バッファサイズ %d 最大記録位置 %d \n", pCross1->GetLogBufferMaxSize(), pCross1->GetLogRecMaxPos() );
  ICHI_PRINT("pCross2:ログ最大バッファサイズ %d 最大記録位置 %d \n", pCross2->GetLogBufferMaxSize(), pCross2->GetLogRecMaxPos() );

  /**
   * @brief アナログスティックを取得する
   * @param stickID  取得するアナログスティックのID
   */
  gfl2::ui::VectorDevice* pStick = GetUiDeviceManager()->GetStick( 0 );
  pStick->SetLogBuffer( mDebugUiDevice[ DEBUG_UI_LOG_STICK ], DEBUG_UI_LOG_BUFFER_MAX[ DEBUG_UI_LOG_STICK ] );
  pStick->StartLogRec();
  ICHI_PRINT("pStick:ログ最大バッファサイズ %d 最大記録位置 %d \n", pStick->GetLogBufferMaxSize(), pStick->GetLogRecMaxPos() );

  /**
   * @brief ボタンを取得する
   * @param buttonID  取得するボタンのID
   */
  gfl2::ui::Button* pButton1 = GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  gfl2::ui::Button* pButton2 = GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STICK_EMU );
  pButton1->SetLogBuffer( mDebugUiDevice[ DEBUG_UI_LOG_BUTTON ], DEBUG_UI_LOG_BUFFER_MAX[ DEBUG_UI_LOG_BUTTON ] );
  pButton2->SetLogBuffer( mDebugUiDevice[ DEBUG_UI_LOG_BUTTON_EMU ], DEBUG_UI_LOG_BUFFER_MAX[ DEBUG_UI_LOG_BUTTON_EMU ] );
  pButton1->StartLogRec(); // 0番目のデバイス
  pButton2->StartLogRec(); // 1番目のデバイス
  ICHI_PRINT("pButton1:ログ最大バッファサイズ %d 最大記録位置 %d \n", pButton1->GetLogBufferMaxSize(), pButton1->GetLogRecMaxPos() );
  ICHI_PRINT("pButton2:ログ最大バッファサイズ %d 最大記録位置 %d \n", pButton2->GetLogBufferMaxSize(), pButton2->GetLogRecMaxPos() );

  /**
   * @brief タッチパネルを取得する
   * @param tpID  取得するタッチパネルのID
   */
  gfl2::ui::TouchPanel* pTouch = GetUiDeviceManager()->GetTouchPanel( 0 );
  pTouch->SetLogBuffer( mDebugUiDevice[ DEBUG_UI_LOG_TOUCH ], DEBUG_UI_LOG_BUFFER_MAX[ DEBUG_UI_LOG_TOUCH ] );
  pTouch->StartLogRec();
  ICHI_PRINT("Touch:ログ最大バッファサイズ %d 最大記録位置 %d \n", pTouch->GetLogBufferMaxSize(), pTouch->GetLogRecMaxPos() );

#if 0 // メモリが足りなかった。。。
  /**
   * @brief 加速度センサを取得する
   * @param accelID  取得する加速度センサのID
   */
  gfl2::ui::Accelerometer* pAccele = GetUiDeviceManager()->GetAccelerometer( 0 );
  if(pAccele){
    pAccele->SetLogBuffer( mDebugUiDevice[ DEBUG_UI_LOG_ACCELEROMETER ], DEBUG_UI_LOG_BUFFER_MAX );
    pAccele->StartLogRec();
  }

  /**
   * @brief ジャイロセンサを取得する
   * @param gyroID  取得するジャイロセンサのID
   */
  gfl2::ui::Gyroscope* pGyro = GetUiDeviceManager()->GetGyroscope( 0 );
  if(pGyro){
    pGyro->SetLogBuffer( mDebugUiDevice[ DEBUG_UI_LOG_GYROSCOPE ], DEBUG_UI_LOG_BUFFER_MAX );
    pGyro->StartLogRec();
  }
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief UIログセーブ終了
 */
//-----------------------------------------------------------------------------
void GameManager::DebugUiDeviceEndLogRec( void )
{
  /**
   * @brief 十字キーを取得する
   * @param keyID  取得する十字キーのID
   */
  gfl2::ui::VectorDevice* pCross1 = GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  gfl2::ui::VectorDevice* pCross2 = GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
  if( pCross1->IsLogRecording() ){
    pCross1->StopLogRec();
    if(pCross1->GetRecordedLogFrameCount()>0){
      pCross1->SetPlayableLogFrameCount( pCross1->GetRecordedLogFrameCount()-DEBUG_UI_LOG_END_DIFF );
    }
  }
  if( pCross2->IsLogRecording() ){
    pCross2->StopLogRec();
    if(pCross2->GetRecordedLogFrameCount()>0){
      pCross2->SetPlayableLogFrameCount( pCross2->GetRecordedLogFrameCount()-DEBUG_UI_LOG_END_DIFF );
    }
  }

  /**
   * @brief アナログスティックを取得する
   * @param stickID  取得するアナログスティックのID
   */
  gfl2::ui::VectorDevice* pStick = GetUiDeviceManager()->GetStick( 0 );
  if( pStick->IsLogRecording() ){
    pStick->StopLogRec();
    if(pStick->GetRecordedLogFrameCount()>0){
      pStick->SetPlayableLogFrameCount( pStick->GetRecordedLogFrameCount()-DEBUG_UI_LOG_END_DIFF );
    }
  }

  /**
   * @brief ボタンを取得する
   * @param buttonID  取得するボタンのID
   */
  gfl2::ui::Button* pButton1 = GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  gfl2::ui::Button* pButton2 = GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STICK_EMU );
  if( pButton1->IsLogRecording() ){
    pButton1->StopLogRec();
    if(pButton1->GetRecordedLogFrameCount()>0){
      pButton1->SetPlayableLogFrameCount( pButton1->GetRecordedLogFrameCount()-DEBUG_UI_LOG_END_DIFF );
    }
  }
  if( pButton2->IsLogRecording() ){
    pButton2->StopLogRec();
    if(pButton2->GetRecordedLogFrameCount()>0){
      pButton2->SetPlayableLogFrameCount( pButton2->GetRecordedLogFrameCount()-DEBUG_UI_LOG_END_DIFF );
    }
  }

  /**
   * @brief タッチパネルを取得する
   * @param tpID  取得するタッチパネルのID
   */
  gfl2::ui::TouchPanel* pTouch = GetUiDeviceManager()->GetTouchPanel( 0 );
  if( pTouch->IsLogRecording() ){
    pTouch->StopLogRec();
    if(pTouch->GetRecordedLogFrameCount()>0){
      pTouch->SetPlayableLogFrameCount( pTouch->GetRecordedLogFrameCount()-DEBUG_UI_LOG_END_DIFF );
    }
  }

#if 0 // メモリが足りなかった。。。
  /**
   * @brief 加速度センサを取得する
   * @param accelID  取得する加速度センサのID
   */
  gfl2::ui::Accelerometer* pAccele = GetUiDeviceManager()->GetAccelerometer( 0 );
  if(pAccele){
    if( pAccele->IsLogRecording() ){
      pAccele->StopLogRec();
    }
    if(pAccele->GetLogRecorderIndex()>0){
      pAccele->SetLogRecorderIndex( pCross->GetLogRecorderIndex()-1 );
    }
  }


  /**
   * @brief ジャイロセンサを取得する
   * @param gyroID  取得するジャイロセンサのID
   */
  gfl2::ui::Gyroscope* pGyro = GetUiDeviceManager()->GetGyroscope( 0 );
  if(pGyro){
    if( pGyro->IsLogRecording() ){
      pGyro->StopLogRec();
    }
    if(pGyro->GetLogRecorderIndex()>0){
      pGyro->SetLogRecorderIndex( pCross->GetLogRecorderIndex()-1 );
    }
  }
#endif
}

bool GameManager::DebugUiDeviceIsLogRec( void ) const
{
  gfl2::ui::VectorDevice* pCross = GetUiDeviceManager()->GetCrossKey( 0 );
  return pCross->IsLogRecording();
}

//-----------------------------------------------------------------------------
/**
 * @brief UIログ再生開始
 */
//-----------------------------------------------------------------------------
void GameManager::DebugUiDeviceStartPlayLog( bool is_loop )
{
  /**
   * @brief 十字キーを取得する
   * @param keyID  取得する十字キーのID
   */
  gfl2::ui::VectorDevice* pCross1 = GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  gfl2::ui::VectorDevice* pCross2 = GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
  pCross1->StartLogPlay();
  pCross1->SetLogLoopPlayEnable(is_loop);
  pCross2->StartLogPlay();
  pCross2->SetLogLoopPlayEnable(is_loop);
  /**
   * @brief アナログスティックを取得する
   * @param stickID  取得するアナログスティックのID
   */
  gfl2::ui::VectorDevice* pStick = GetUiDeviceManager()->GetStick( 0 );
  pStick->StartLogPlay();
  pStick->SetLogLoopPlayEnable(is_loop);

  /**
   * @brief ボタンを取得する
   * @param buttonID  取得するボタンのID
   */
  gfl2::ui::Button* pButton1 = GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  gfl2::ui::Button* pButton2 = GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STICK_EMU );
  pButton1->StartLogPlay();
  pButton1->SetLogLoopPlayEnable(is_loop);
  pButton2->StartLogPlay();
  pButton2->SetLogLoopPlayEnable(is_loop);

  /**
   * @brief タッチパネルを取得する
   * @param tpID  取得するタッチパネルのID
   */
  gfl2::ui::TouchPanel* pTouch = GetUiDeviceManager()->GetTouchPanel( 0 );
  pTouch->StartLogPlay();
  pTouch->SetLogLoopPlayEnable(is_loop);

#if 0 // メモリが足りなかった。。。
  /**
   * @brief 加速度センサを取得する
   * @param accelID  取得する加速度センサのID
   */
  gfl2::ui::Accelerometer* pAccele = GetUiDeviceManager()->GetAccelerometer( 0 );
  if(pAccele){
    pAccele->StartLogPlay(true);
  }


  /**
   * @brief ジャイロセンサを取得する
   * @param gyroID  取得するジャイロセンサのID
   */
  gfl2::ui::Gyroscope* pGyro = GetUiDeviceManager()->GetGyroscope( 0 );
  if(pGyro){
    pGyro->StartLogPlay(true);
  }
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief UIログ再生終了
 */
//-----------------------------------------------------------------------------
void GameManager::DebugUiDeviceEndPlayLog( void )
{
  /**
   * @brief 十字キーを取得する
   * @param keyID  取得する十字キーのID
   */
  gfl2::ui::VectorDevice* pCross1 = GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  gfl2::ui::VectorDevice* pCross2 = GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
  if( pCross1->IsLogPlaying() ){
    pCross1->StopLogPlay();
  }
  if( pCross2->IsLogPlaying() ){
    pCross2->StopLogPlay();
  }

  /**
   * @brief アナログスティックを取得する
   * @param stickID  取得するアナログスティックのID
   */
  gfl2::ui::VectorDevice* pStick = GetUiDeviceManager()->GetStick( 0 );
  if( pStick->IsLogPlaying() ){
    pStick->StopLogPlay();
  }

  /**
   * @brief ボタンを取得する
   * @param buttonID  取得するボタンのID
   */
  gfl2::ui::Button* pButton1 = GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  gfl2::ui::Button* pButton2 = GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STICK_EMU );
  if( pButton1->IsLogPlaying() ){
    pButton1->StopLogPlay();
  }
  if( pButton2->IsLogPlaying() ){
    pButton2->StopLogPlay();
  }

  /**
   * @brief タッチパネルを取得する
   * @param tpID  取得するタッチパネルのID
   */
  gfl2::ui::TouchPanel* pTouch = GetUiDeviceManager()->GetTouchPanel( 0 );
  if( pTouch->IsLogPlaying() ){
    pTouch->StopLogPlay();
  }

#if 0 // メモリが足りなかった。。。
  /**
   * @brief 加速度センサを取得する
   * @param accelID  取得する加速度センサのID
   */
  gfl2::ui::Accelerometer* pAccele = GetUiDeviceManager()->GetAccelerometer( 0 );
  if(pAccele){
    if( pAccele->IsLogPlaying() ){
      pAccele->StopLogPlay();
    }
  }


  /**
   * @brief ジャイロセンサを取得する
   * @param gyroID  取得するジャイロセンサのID
   */
  gfl2::ui::Gyroscope* pGyro = GetUiDeviceManager()->GetGyroscope( 0 );
  if(pGyro){
    if( pGyro->IsLogPlaying() ){
      pGyro->StopLogPlay();
    }
  }
#endif
}

bool GameManager::DebugUiDeviceIsPlayLog( void ) const
{
  gfl2::ui::VectorDevice* pCross = GetUiDeviceManager()->GetCrossKey( 0 );
  return pCross->IsLogPlaying();
}

#endif //PM_DEBUG


#if PM_DEBUG
/**
* @brief DebugTick定義から定義名を取得する
*
* @param index DebugTickの定義値
*
* @return 定義名(不正な場合はNULL)
*/ 
const c8* GameManager::GetTickLabel( TickDefine::Index index )
{
  if( index >= TickDefine::MAX )
  {
    return 0;
  }

  return sc_DebugTickGameManagerNameTable[ index ];
}
#endif // PM_DEBUG



//! @brief  描画中レイアウトワークのマネージャを取得する
gfl2::lyt::DrawingLytWkManager* GameManager::GetDrawingLytWkManager(void) const
{
  // @fix GFNMCat[3490] 【図鑑】検索画面から戻れなくなる  // デバッグ機能のつもりでPM_DEBUGで括っていたが、デバッグ用途以外で使用している箇所があったので本機能に昇格。PM_DEBUGを外した。
  if(m_DrawingLytWkManagerAdditionIsEnabled)
  {
    return m_pDrawingLytWkManager;
  }
  else
  {
    return NULL;
  }
}

#if PM_DEBUG
void GameManager::SetDrawingLytWkManagerAdditionEnabled(bool /*enable*/)
{
  // @fix GFNMCat[3490] 【図鑑】検索画面から戻れなくなる  // デバッグ機能のつもりでPM_DEBUGで括っていたが、デバッグ用途以外で使用している箇所があったので本機能に昇格。必ず使用するのでtrueから変更させないようコメントアウトした。
  //m_DrawingLytWkManagerAdditionIsEnabled = enable;
}
bool GameManager::IsDrawingLytWkManagerAdditionEnabled(void) const
{
  return m_DrawingLytWkManagerAdditionIsEnabled;
}
#endif  // PM_DEBUG

//
// private
//

#if PM_DEBUG
//! @brief  起動時設定ファイルの設定をゲーム設定に適用する
void GameManager::ApplyStartSettingAccessorToGameSetting(void)
{
#if defined(GF_PLATFORM_WIN32)
  // win版のみマスター音量を変更する機能を提供
  if( m_GameData->GetDebugStartSettingAccessor()->HasSoundMasterVolume() )
  {
    f32 masterVolume( m_GameData->GetDebugStartSettingAccessor()->GetSoundMasterVolume() );
    if( GFL_SINGLETON_INSTANCE( gfl2::sound::SoundSystem )->GetMasterVolume() != masterVolume )
    {
      GFL_SINGLETON_INSTANCE( gfl2::sound::SoundSystem )->SetMasterVolume( masterVolume, 0 );
    }
  }
#endif
#if defined(GF_PLATFORM_CTR)
  // HIOオプションの切り替え
  if( m_GameData->GetDebugStartSettingAccessor()->HasHioEnable() )
  {
    gfl2::fs::AsyncFileManager* pRomAsyncFileManager = GetAsyncFileManager();
    b32 nowFlag = pRomAsyncFileManager->GetHioUseFlag();
    b32 newFlag = m_GameData->GetDebugStartSettingAccessor()->GetHioEnable();

    if( nowFlag != newFlag )
    { 
      pRomAsyncFileManager->SetHioUseFlag( newFlag );
      System::GflUse::ReloadGarc();
    }
  }
#endif
}
#endif  // PM_DEBUG


} // namespace GameSys 

