//======================================================================
/**
 * @file    StatusProc.cpp
 * @date    2015/11/27 16:08:09
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：Proc
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/Status/include/StatusProc.h>

#include "StatusFrame.h"

#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <System/include/HeapDefine.h>
#include <System/include/nijiAllocator.h>

#include "../include/StatusAppParam.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const u32 SIZE_PROC_MANAGER  =  4 * 1024;            //!< 4Kバイト マネージャーのヒープは荒れるので別途管理
static const u32 SIZE_MY_APP        =  1 * 1024 * 1024;     //!< 1Mバイト
static const u32 SIZE_MY_APPDEV     = 16 * 1024 * 1024;     //!< 16Mバイト


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
  , m_pNijiAllocator(NULL)
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

    // NIJIアロケータの生成
    m_pNijiAllocator = GFL_NEW( pDevHeap ) System::nijiAllocator( pDevHeap );

    // フレームマネージャの生成
    m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
    m_pFrameManager->SetAppFrameListener(this);

    // レンダリングマネージャの生成
    m_pAppRenderingManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();
    {
      app::util::AppRenderingManager::Description desc;
      {
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_RADERCHART;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].pokeModelNumMax = 6;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].modelNumMax = 0;

        /*  forBFC  */
        if(m_pAppParam->mode == MODE_BFC)
        {
          desc.renderDesc[ app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ].renderOrder    = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
        }
      }

      gfl2::fs::AsyncFileManager * afm = m_pGameManager->GetAsyncFileManager();
      m_pAppRenderingManager->StartAsyncCreate( desc, m_pAppHeap, m_pNijiAllocator, afm );
    }
  }

  m_pAppRenderingManager->Update();  // Updateを呼ばないと非同期生成が進まない
  if( m_pAppRenderingManager->IsCreated() )
  {
    // デフォルト環境の生成
    //m_pAppRenderingManager->CreateDefaultDrawEnv( m_pAppHeap );

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
  GFL_SAFE_DELETE( m_pNijiAllocator );
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

  StatusFrame::SetupParam param;
  {
    param.appParam  = m_pAppParam;
    param.heap      = m_pAppHeap;
    param.renderMan = m_pAppRenderingManager;
    param.fadeMan   = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
    param.fileMan   = m_pGameManager->GetAsyncFileManager();
    param.devMan    = m_pGameManager->GetUiDeviceManager();
  }

  m_pFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, StatusFrame>( this->GetFrameManager() );
  m_pFrame->Setup( param );
}

//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータのセット
 */
//------------------------------------------------------------------------------
void Proc::SetAppParam( APP_PARAM* pParam )
{
  m_pAppParam = pParam;
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
