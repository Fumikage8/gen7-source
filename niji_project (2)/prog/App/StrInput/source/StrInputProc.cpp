//======================================================================
/**
 * @file    StrInputProc.cpp
 * @date    2015/12/21 15:59:35
 * @author  fukushima_yuya
 * @brief   文字入力：PROC
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/StrInput/include/StrInputProc.h>
#include <App/StrInput/include/StrInputFrame.h>

#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <GameSys/include/GameData.h>
#include <System/include/HeapDefine.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StrInput)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const u32 SIZE_PROC_MANAGER = 4 * 1024;            //!< 4Kバイト マネージャーのヒープは荒れるので別途管理
static const u32 SIZE_MY_APP        = 1 * 1024;           //!< 1Kバイト
static const u32 SIZE_MY_APPDEV     = 1 * 1024 * 1024;    //!< 1Mバイト

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
Proc::Proc( void )
  : m_pAppHeap(NULL)
  , m_pAppRenderingManager(NULL)
  , m_pFrameManager(NULL)
  , m_pManagerHeap(NULL)
  , m_FrameCount(0)
  , m_pFrame(NULL)
  , m_pAppParam(NULL)
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::InitFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED( pManager );

  if( m_pAppRenderingManager == NULL )
  {
    m_pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

    //HEAPID_APP  本体に関するものはAPPめもりーから確保します
    gfl2::heap::HeapBase* pAppHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
    gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( pDevHeap, SIZE_PROC_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false );

    // アプリ用ヒープの生成
    m_pAppHeap = GFL_NEW(m_pManagerHeap) app::util::Heap();
    m_pAppHeap->LocalHeapCreate( pAppHeap, pDevHeap, SIZE_MY_APP,SIZE_MY_APPDEV );

    // フレームマネージャの生成
    m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
    m_pFrameManager->SetAppFrameListener(this);

    // レンダリングマネージャの生成
    m_pAppRenderingManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();
    {
      app::util::AppRenderingManager::Description desc;
      {
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      }

      gfl2::fs::AsyncFileManager * afm = m_pGameManager->GetAsyncFileManager();
      m_pAppRenderingManager->StartAsyncCreate( desc, m_pAppHeap, NULL, afm );
    }
  }

  if( m_pAppRenderingManager->IsCreated() )
  {
    // フレームの生成
    CreateFrame();

    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン更新を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED( pManager );

  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCの描画処理(下準備)
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void Proc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  m_pAppRenderingManager->PreDraw();
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void Proc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo no )
{
  GFL_UNUSED( pManager );

  m_pAppRenderingManager->Update();

  m_pFrameManager->Draw( no );
  m_pAppRenderingManager->Draw( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 *  1/30フレームの②フレーム目の処理です。
 *  1/60フレーム動作時には、使われません。
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::NextFrameMainFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED( pManager );

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::EndFunc( gfl2::proc::Manager* pManager )
{
  // 終了処理が完了するまで待つ
  if( m_pFrameManager->End() == true )
  {
    return gfl2::proc::RES_CONTINUE;
  }
  if( !m_pAppRenderingManager->CanDestroy() )
  {
    return gfl2::proc::RES_CONTINUE;
  }

  if( m_FrameCount == 0 )
  {
    m_FrameCount++;
    return gfl2::proc::RES_CONTINUE;
  }

  GFL_UNUSED( pManager );

  GFL_SAFE_DELETE( m_pAppRenderingManager );
  GFL_SAFE_DELETE( m_pFrameManager );
  GFL_SAFE_DELETE( m_pAppHeap );
  gfl2::heap::Manager::DeleteHeap( m_pManagerHeap );

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   EndFuncが完了した直後に呼び出される関数
 */
//------------------------------------------------------------------------------
void Proc::EndFuncFinish( applib::frame::CellSuper* pEndCell )
{
  FUKUSHIMA_PRINT("EndFuncFinish呼び出し\n");

  if( pEndCell == m_pFrame )
  {
    m_pFrame = NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   フレームの生成
 */
//------------------------------------------------------------------------------
void Proc::CreateFrame( void )
{
  GFL_ASSERT( m_pAppParam );

  {
    GameSys::GameManager* gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    GameSys::GameData* gd    = gm->GetGameData();

    m_pAppParam->mpGameProcManager     = gm->GetProcManager();
    m_pAppParam->mpAppRenderingManager = m_pAppRenderingManager;

    m_pAppParam->pSaveMisc = gd->GetMisc();
    m_pAppParam->language  = System::GetLang();
  }

  // フレームの生成
  m_pFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, StrInputFrame>( this->GetFrameManager() );
  {
    GFL_ASSERT( m_pFrame );

    m_pFrame->Setup( m_pAppParam );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータのセット
 */
//------------------------------------------------------------------------------
void Proc::SetAppParam( APP_PARAM* pParam )
{
  GFL_ASSERT( pParam );
  m_pAppParam = pParam;
}

GFL_NAMESPACE_END(StrInput)
GFL_NAMESPACE_END(App)
