//===================================================================
/**
 * @file    GameManager.cpp
 * @brief   ゲームで全体を通して管理が必要なクラスのインスタンスを持つクラス
 * @author  k.ohno
 * @date    10.12.22
*/
//===================================================================

#include <heap/include/gfl2_Heap.h>
///#include <debug/include/gfl2_DebugWinSystem.h>
///#include <gfx/include/gfl2_GFGL.h>  //GPU計測用@saito不要
//#include <ro/gfl_RoManager.h>

#include "../../System/include/HeapDefine.h"
#include "../../System/include/HeapSize.h"
#include "System/include/GflvUse.h"
#include "GameSystem/include/GameManager.h"
#include "GameSystem/include/GameProcManager.h"
#include "GameSystem/include/GameEventManager.h"
#include "GameSystem/include/GameData.h"
///#include "Print/include/GrammaticalAttributeProvider.h"@saito不要
///#include "Debug/DebugWin/include/DebugWinSystem.h"@saito
///#include "FieldScript/include/FieldScriptSystem.h"@saito不要
///#include "Sound/include/Sound.h"@saito不要

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(GameSystem::GameManager);
template class gfl2::base::SingletonAccessor<GameSystem::GameManager>;

namespace GameSystem 
{
  // マイクロ秒を取得
  u64 GetMicroSecond( void )
  {
#if defined(GF_PLATFORM_WIN32)
#include <windows.h>
    LARGE_INTEGER tick;
    LARGE_INTEGER frequency;
    if( ::QueryPerformanceCounter(&tick) == FALSE || ::QueryPerformanceFrequency(&frequency) == FALSE )
    {
      return 0ULL;
    }

    return 1000000ULL * (u64)tick.QuadPart  / (u64)frequency.QuadPart ;
#endif
#if defined(GF_PLATFORM_CTR)
    nn::os::Tick now_tick = nn::os::Tick::GetSystemCurrent();
    return static_cast<s32>(now_tick.ToTimeSpan().GetMicroSeconds());
#endif // GF_PLATFORM_CTR
    return 0;
  }

  // 垂直同期回数を取得
  // win32版は1秒/60を1回とした擬似同期回数を返す
  s32 GetVSyncCount( void )
  {
#if defined(GF_PLATFORM_WIN32)
    return GetMicroSecond() * 60 / 1000000;
#endif //GF_PLATFORM_WIN32
#if defined(GF_PLATFORM_CTR)
    return nngxCheckVSync(NN_GX_DISPLAY_BOTH);
#endif // GF_PLATFORM_CTR

  }

  // 垂直同期待ち
  // win32版は1秒/60を1回とした擬似同期タイミングまで待つ
  void VSyncWait( void )
  {
#if defined(GF_PLATFORM_WIN32)

    s32 nowCount( GetVSyncCount() );
    while( nowCount == GetVSyncCount() )
    {
      Sleep(1);
    }

#endif //GF_PLATFORM_WIN32
#if defined(GF_PLATFORM_CTR)
    nngxWaitVSync(NN_GX_DISPLAY_BOTH);
#endif // GF_PLATFORM_CTR

  }

  // 垂直同期待ち
  // スリープで待つ版
  // @param nowCount      現在のカウンタ
  // @param milliseconds  1スリープあたりの待ち時間(ミリ秒)
  void VSyncWaitBySleep( s32 nowCount, s64 milliseconds )
  {
    while( nowCount == GetVSyncCount() )
    {
#if defined(GF_PLATFORM_WIN32)
      Sleep( milliseconds );
#endif //GF_PLATFORM_WIN32
#if defined(GF_PLATFORM_CTR)
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( milliseconds ) );
#endif // GF_PLATFORM_CTR
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

//----------------------------------------------------------------------------
/**
 *  @brief    コンストラクタ＆デストラクタ
 *  @param    sHeap システムヒープ
 *  @param    dHeap デバイスヒープ
 */
//-----------------------------------------------------------------------------
GameManager::GameManager(gfl2::heap::HeapBase *sHeap, gfl2::heap::HeapBase *dHeap) :
  m_ReqFrameMode(GameSystem::FRAMEMODE_MAX),
  m_FrameMode(GameSystem::FRAMEMODE_60),
  m_FrameCount(0),
  m_VSyncCount( 0 ),
  m_parallaxDepthRange(1.0f/*gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ONE*/),
  m_fatalEvent(false)
{
  for( u32 i = 0; i < gfl2::gfx::CtrDisplayNo::NumberOf; i++ )
  {
    m_BufferClearSettingTable[ i ] = sc_BufferClearDefaultSettingTable[ i ];
  }

  m_systemHeap = sHeap;
  m_deviceHeap = dHeap;

  m_procCellHeap = GFL_CREATE_HEAP(m_systemHeap, HEAPID_PROCCELL, System::HeapSize::PROCCELL, gfl2::heap::HEAP_TYPE_EXP, false);

  //UIデバイスマネージャのフレームレート初期設定
  gfl2::ui::DeviceManager* p_devman = this->GetUiDeviceManager();
  if( m_FrameMode == GameSystem::FRAMEMODE_60 )
  {
    p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_60 );
  }
  else
  {
    p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_30 );
  }

  p_devman->SynchronizeInputUpdateForAllDevice();

#if GFL_DEBUG
  mDebugKeyDisable = false;
#endif

	mEndRequest = false;
}


//----------------------------------------------------------------------------
/**
 *  @brief    デストラクタ
 */
//-----------------------------------------------------------------------------
GameManager::~GameManager()
{
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
  this->CreateProcManager();
/**
  // 文法対応モジュール
  m_pGrammaticalAttributeProvider = GFL_NEW(m_systemHeap) print::GrammaticalAttributeProvider();
  m_pGrammaticalAttributeProvider->Initialize(m_systemHeap);
  
  m_FieldScriptSystem = GFL_NEW(m_systemHeap) Field::FieldScript::FieldScriptSystem( this, m_systemHeap );
*///@saito不要
	m_GameData = GFL_NEW(m_systemHeap) GameData( m_systemHeap, m_deviceHeap, this->GetUiDeviceManager());
  this->ClearFrameControl();
  this->SetDefaultRenderingParameter();
  m_pFieldSystem = NULL;
}


//----------------------------------------------------------------------------
/**
 *  @brief プロセスマネージャを生成する
 */
//-----------------------------------------------------------------------------
void GameManager::CreateProcManager( void )
{
  GFL_ASSERT( m_procManager == NULL ); //@check
  m_procManager = GFL_NEW(m_systemHeap) GameSystem::GameProcManager(m_procCellHeap, this);
  m_GameEventManager = GFL_NEW(m_systemHeap) GameSystem::GameEventManager(this);
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

  bool result_proc = this->DoFrameAction();  // 1フレームで行われる処理

  // イベントも、Procも無くなったらゲーム終了
  return ( result_proc | m_GameEventManager->IsExists() );
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
  bool isUpdateFrame = this->UpdateFrameControl();
#if 0
  if( isUpdateFrame )
  {
#if GFL_DEBUG
    // R＋ L + X　で、キー無効状態で、デバッグウィンドウをオープン
    this->GetUiDeviceManager()->SetDebugKeyDisable( false );  // キー有効

    //@todo xy_system::GflUse::isCreateGraphicsSystem();をチェックしていたがなぜ？
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
        //@todo ログ再生の仕組み(オレンジ箱のDebug機能(有料)にあるが、nijiで個別実装するか？

        // Lも押されていたら、キー無効状態に
        if( this->GetUiDeviceManager()->GetButton(0)->IsHold( gfl2::ui::BUTTON_L ) )
        {
          mDebugKeyDisable = true;
        }
      }

    }
#endif
  }
#endif //@saito_del

    // @todo デバッグウィンドウ関連処理
    // デバイスの更新をカット
#if GFL_DEBUG
  if(this->IsDebugKeyDisable())
  {
    this->GetUiDeviceManager()->SetDebugKeyDisable( true );
  }
#endif // GFL_DEBUG
  if( isUpdateFrame )
  {
    {
      //xy_debug::PauseSystem_Update();

      if( !isPause() )
      {
        ///Debug::DebugWin::TickStart(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);@saito不要

        // イベント更新
        this->UpdateEventProcess();

        // 1/60プロセス処理
        result_proc = this->UpdateProcManager();

        ///Debug::DebugWin::TickEnd(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);@saito不要
      }
    }
    // サウンドシステムメイン
    ///Sound::Update();@saito不要
  }
  else
  {
    // 2/60プロセス処理。 「1/30動作時」のみこちらに処理が流れます。
    {
      if( !isPause() )
      {
        ///Debug::DebugWin::TickStart(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);@saito不要
        result_proc = this->NextFrameUpdateProcManager();
        ///Debug::DebugWin::TickEnd(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);@saito不要
      }
    }
  }

  // 描画
  if( m_FrameMode == GameSystem::FRAMEMODE_60 || !isUpdateFrame )
  {

    if( 1 /*mGameData->GetIsSkipDraw() == false*/ ) //@todo
    {
      //ここでDrawの計測を行うと、関数内でVSyncWaitが呼ばれているので困るから内部に移動。
      this->DrawProcManager();

      //@todo ホントはここでGPU/Drawのオーバーチェックをしたりする
    }
#if 0
    else
    {
      mGameData->SetIsSkipDraw(false);
    }
#endif
  }

#if GFL_DEBUG
  // ここで、デバッグキー無効を解消する。
  // こうしないと、デバッグウィンドウを消したフレームで、
  // アプリケーションがキー入力してしまう。
  ///if( !gfl2::debug::DebugWin_IsOpen() ){ mDebugKeyDisable = false; }
#endif // GFL_DEBUG

  return result_proc;
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
 *  @brief  描画処理(1/60動作時）
 */
//-----------------------------------------------------------------------------
void GameManager::DrawProcManager( void )
{
  if( m_procManager )
  {
#if 0
#if GFL_DEBUG
    if( !IsDebugMode() )
#endif
#endif
    {
      {
        ///Debug::DebugWin::TickStart(Debug::DebugWin::DEBUGWIN_TICK_DRAW);@saito不要
        // 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行
        if( m_procManager->GetDrawFlag() )
        {
          m_procManager->PreDraw();
        }

        for( u32 i = 0; i < gfl2::gfx::CtrDisplayNo::NumberOf; i++ )
        {
#if defined(GF_PLATFORM_WIN32)

          // PCでは右画面の描画をスキップ
          if( i == gfl2::gfx::CtrDisplayNo::RIGHT )
          {
            continue;
          }

#endif //GF_PLATFORM_WIN32
#if defined(GF_PLATFORM_CTR)

          // 立体視OFF時は右画面の描画をスキップ
          // @todo 立体時になった瞬間にチラつきが発生する、対処方法はまだ決まってない 150223 ikuta_junya
          if( i == gfl2::gfx::CtrDisplayNo::RIGHT && nn::gx::CTR::IsStereoVisionAllowed() == false )
          {
            continue;
          }

#endif // GF_PLATFORM_CTR

          // ディスプレイ設定
          gfl2::gfx::GFGL::SetActiveDisplay( System::GflvUse::m_HandleList[i] );
          
          // レンダーターゲット設定
          gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflvUse::m_HandleList[ i ] );
          gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflvUse::m_HandleList[ i ] );
          gfl2::gfx::GFGL::SetRenderTarget( pBackBuffer );
		      gfl2::gfx::GFGL::SetDepthStencil( pDepthBuffer );

          // レンダーターゲットクリア処理
          // @note BeginScene()内部でRenderTarget及び、DepthStencilを参照する。Setされていない場合はアクセス例外が発生します 150224 ikuta_junya
          if( gfl2::gfx::GFGL::BeginScene() )
          {
            const BUFFER_CLEAR_SETTING& rSetting = m_BufferClearSettingTable[ i ];

            // ビューポート設定
            u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflvUse::m_HandleList[ i ] );
            u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflvUse::m_HandleList[ i ] );

            if ( displayWidth && displayHeight )
            {
              gfl2::gfx::GFGL::SetViewport( 0, 0, displayWidth, displayHeight );
              gfl2::gfx::GFGL::SetScissor( 0, 0, displayWidth, displayHeight );
            }
            else
            {
              const gfl2::gfx::Surface::SurfaceDesc			&rDesc = gfl2::gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
              gfl2::gfx::GFGL::SetViewport( 0, 0, rDesc.width, rDesc.height);
              gfl2::gfx::GFGL::SetScissor( 0, 0, rDesc.width, rDesc.height);
            }

            // カラー＆深度ステンシル
            if( rSetting.clearTarget == ( BUFFER_CLEAR_TARGET_BIT_COLOR | BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL ) )
            {
              gfl2::gfx::GFGL::ClearRenderTargetDepthStencil( pBackBuffer, rSetting.color, pDepthBuffer, rSetting.depth, rSetting.stencil );
            }
            else 
            {
              // カラー
              if( rSetting.clearTarget & BUFFER_CLEAR_TARGET_BIT_COLOR )
              {
                gfl2::gfx::GFGL::ClearRenderTarget( pBackBuffer, rSetting.color );
              }

              // 深度ステンシル
              if( rSetting.clearTarget & BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL )
              {
                gfl2::gfx::GFGL::ClearDepthStencil( pDepthBuffer, rSetting.depth, rSetting.stencil );
              }
            }
            gfl2::gfx::GFGL::EndScene();
          }

          // 登録したPROCによる、フレームバッファへの描画を実行します
          if( m_procManager->GetDrawFlag() )
          {
            m_procManager->Draw( static_cast< gfl2::gfx::CtrDisplayNo >( i ) );
            ///gfl2::debug::DebugWin_Draw(static_cast< gfl2::gfx::CtrDisplayNo >( i ) , System::GflvUse::m_HandleList[i]);
            
            gfl2::gfx::GFGL::TransferDisplay();	// @todo 本当はif( m_procManager->GetDrawFlag() )の外だが、何も描画せずにに実行するとハングしてしまう？ 
          }
          // @todo Fade描画処理
          // @todo Debug描画処理
          
          // ディスプレイバッファに転送 @todo これはGameManager
        }

        // @todo GPU計測 ikuta_junya 150215

        //gfl2::grp::Fade_Update();
        //xy_system::GflUse::GetHomeNix()->Update();
        //xy_system::GflUse::GetHomeNix()->Draw();
        // grpSys->EndRendering(m_parallaxDepthRange * (nn::gx::IsStereoVisionAllowed() ? 1.0f:0.0f));

        ///Debug::DebugWin::TickEnd(Debug::DebugWin::DEBUGWIN_TICK_DRAW);@saito不要
        // @todo このif分は本当は不要だが、現状ではDraw関数が実行されないときに止まってしまう
        if( m_procManager->GetDrawFlag() )
        {
          // 30フレームモードで、前回から同期されてないなら同期を挟む
          if( m_FrameMode == GameSystem::FRAMEMODE_30 && m_VSyncCount == GetVSyncCount() )
          {
            // VSyncWait発行直前に変わってしまう可能性があるので、Sleepで待つ。
            VSyncWaitBySleep( m_VSyncCount, 1 );
          }

          // PresentPath
          gfl2::gfx::GFGL::Finish();
          gfl2::gfx::GFGL::SwapDisplay();

#if defined(GF_PLATFORM_WIN32)
          // win32では垂直同期を切っているので、自前で1秒/60を1回とした擬似同期タイミングまで待ちます @todo 仮 150219 ikuta_junya
          VSyncWait();
#endif

          m_VSyncCount = GetVSyncCount();
        }

     }

      // 1秒あたりの描画回数をプリント
#if GFL_DEBUG
      static u64 s_BeforeTime = 0;
      static u32 s_FpsCount = 0;

      // FPSカウント：1秒に1回、呼び出し回数をリセット
      u64 now = GetMicroSecond();
      if( now - s_BeforeTime >= 1000000 )
      {
        s_BeforeTime = now;
        //GFL_PRINT("FPS: %d \n", s_FpsCount );
        s_FpsCount = 0;
      }

      s_FpsCount++;
#endif
    }
  ///Debug::DebugWin::TickCalc(Debug::DebugWin::DEBUGWIN_TICK_GPU,gfl2::gfx::GFGL::GetTotalGPUTimeMicroSecond());@saito不要
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  イベント処理
 */
//-----------------------------------------------------------------------------
void GameManager::UpdateEventProcess( void )
{
  /**
  // NULLかどうかだけチェック
  if( m_GameEventManager->isExists() == false )
  {
    // イベント実行前　イベント検知処理
    m_procManager->EventCheck();
  }

  // イベント動作
  m_GameEventManager->Main();
  *///@saito hock
}


//----------------------------------------------------------------------------
/**
 *  @brief    開放処理
 */
//-----------------------------------------------------------------------------
void GameManager::Finalize(void)
{
/**  
  GFL_SAFE_DELETE( m_FieldScriptSystem );
  m_pGrammaticalAttributeProvider->Finalize();
  GFL_SAFE_DELETE( m_pGrammaticalAttributeProvider );
  
*///@saito不要
	GFL_SAFE_DELETE( m_GameData );
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
void GameManager::RequestFrameMode( GameSystem::FrameMode mode )
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
  m_parallaxDepthRange = 1.0f; //gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ONE; @todo
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
 *	@brief  右目がレンダリングされるか？
 *
 *	@retval true  レンダリングされる
 *	@retval false レンダリングされない
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
  return System::GflvUse::m_Singlton.m_romAsyncFileManager;
}


//-----------------------------------------------------------------------------
/**
 * @brief UI デバイスマネージャを取得する
 */
//-----------------------------------------------------------------------------
gfl2::ui::DeviceManager* GameManager::GetUiDeviceManager( void ) const
{
  return System::GflvUse::m_Singlton.m_pUiDeviceManager;
}

#if 0
//-----------------------------------------------------------------------------
/**
 * @brief グラフィックスシステムを取得する
 */
//-----------------------------------------------------------------------------
gfl2::grp::GraphicsSystem* GameManager::GetGraphicsSystem( void ) const
{
  return xy_system::GflvUse::m_Singlton.m_pGraphicsSystem;
}


//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックスシステムをデフォルトの状態に戻す。
 */
//-----------------------------------------------------------------------------
void GameManager::SetupDefaultGraphicsSystem( void )
{
  xy_system::GflvUse::m_Singlton.m_pGraphicsSystem->ChangeSetUp( &xy_system::GflvUse::m_DefaultGraphicsSetUp );
}


//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックスシステムのセットアップ情報にデフォルト値を与える。
 */
//-----------------------------------------------------------------------------
void GameManager::SetDefaultGraphicsSystemSetUp( gfl2::grp::GraphicsSystemSetUp* setup ) const
{
  *setup = xy_system::GflvUse::m_DefaultGraphicsSetUp;
}
#endif

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
*	@brief  指定画面のクリア設定を設定する
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
*	@brief  指定画面のクリア設定を取得する
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
  m_ReqFrameMode= GameSystem::FRAMEMODE_MAX;
  m_FrameMode   = GameSystem::FRAMEMODE_60;
  m_FrameCount  = 0;
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
bool GameManager::IsFrameControl( GameSystem::FrameMode mode ) const   //
{
  return ( m_FrameMode == mode );
}


//----------------------------------------------------------------------------
/**
 *  @brief  フレームコントロール更新
 *
 *  @retval true    1フレーム目の処理を行う（イベントメインとプロセスメイン）
 *  @retval false   2フレーム目の処理を行う（プロセスのネクストフレームメイン）
 */
//-----------------------------------------------------------------------------
bool GameManager::UpdateFrameControl( void )
{
  if( m_FrameMode == GameSystem::FRAMEMODE_60 ){
    return true;
  }
  m_FrameCount ^= 1;

  return m_FrameCount;
}


//----------------------------------------------------------------------------
/**
 *  @brief  フレームの更新処理
 */
//-----------------------------------------------------------------------------
void GameManager::ChangeFrameControl( void )
{
  // リクエストが無ければ、何もしない。
  if( m_ReqFrameMode == GameSystem::FRAMEMODE_MAX ){
    return ;
  }

  // 変更
  if( m_FrameMode != m_ReqFrameMode ){
    
    gfl2::ui::DeviceManager* p_devman = this->GetUiDeviceManager();

    if( m_ReqFrameMode == GameSystem::FRAMEMODE_60 ){

      // キー入力
      p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_60 );

    }
    // 1/30フレーム動作
    else{

      // キー入力
      p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_30 );
      p_devman->SynchronizeInputUpdateForAllDevice();
    }

    // Topフレームから開始
    m_FrameCount = 0;
    m_FrameMode  = m_ReqFrameMode;
  }

  m_ReqFrameMode = GameSystem::FRAMEMODE_MAX;
}


//----------------------------------------------------------------------------
/**
 *  @brief  フレームコントロールのモード変更リクエスト設定
 *
 *  @param  mode  変更したモード
 */
//-----------------------------------------------------------------------------
void GameManager::RequestFrameControl( GameSystem::FrameMode mode )
{
  if( mode < GameSystem::FRAMEMODE_MAX ){
    m_ReqFrameMode = mode;
  }else{
    GFL_ASSERT( mode < GameSystem::FRAMEMODE_MAX ); //@check
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief   止めるかどうか
 */
//-----------------------------------------------------------------------------
bool GameManager::isPause( void )
{
#if 0
  if( xy_debug::PauseSystem_IsPause() ){
    return true;
  }
#endif
  return false;
}


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


} // namespace GameSystem

