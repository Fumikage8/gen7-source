//==============================================================================
/**
 * @file    FinderProc.cpp
 * @author  fukushima_yuya
 * @date    2015.06.08
 * @brief   ポケファインダーUIプロック
 */
//==============================================================================

// niji
#include "App/Finder/include/FinderProc.h"

#include <AppLib/include/Util/AppRenderingManager.h>
#include <Field/FieldStatic/include/FieldResident/FieldResident.h>
#include <GameSys/include/GameManager.h>
#include <System/include/HeapDefine.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

namespace {
  static const u32 MANAGER_HEAP_SIZE      = 4 * 1024;
  //static const u32 LOCAL_SYSTEM_HEAP_SIZE = 4 * 1024 * 1024;
  //static const u32 LOCAL_DEVICE_HEAP_SIZE = 4 * 1024 * 1024;
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderProc::FinderProc( void )
  : m_pManagerHeap( NULL )
  , m_heap()
  , m_pRenderingManager( NULL )
  , m_pFrameManager( NULL )
  , m_pFinderFrame( NULL )
  , m_pAppParam( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
FinderProc::~FinderProc( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
gfl2::proc::Result FinderProc::InitFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );

  if( m_pRenderingManager == NULL )
  {
    Field::FieldResident * field_resident = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldResident();

    gfl2::heap::HeapBase * heap     = field_resident->GetFieldSubScreenHeap();
    gfl2::heap::HeapBase * dev_heap = field_resident->GetFieldSubScreenDeviceHeap();

    // マネージャヒープの生成
    m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( dev_heap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

    // app::util::Heap作成
    gfl2::heap::NwAllocator * allocator     = GFL_NEW( heap ) gfl2::heap::NwAllocator( heap );
    gfl2::heap::NwAllocator * dev_allocator = GFL_NEW( heap ) gfl2::heap::NwAllocator( dev_heap );

    gfl2::util::GLHeapAllocator * gl_allocator     = GFL_NEW( heap ) gfl2::util::GLHeapAllocator( heap );
    gfl2::util::GLHeapAllocator * dev_gl_allocator = GFL_NEW( heap ) gfl2::util::GLHeapAllocator( dev_heap );

    m_heap.Create(
      heap,
      dev_heap,
      allocator,
      dev_allocator,
      gl_allocator,
      dev_gl_allocator );

    // レンダリングマネージャの生成
    m_pRenderingManager = GFL_NEW(m_heap.GetDeviceHeap()) app::util::AppRenderingManager();
    {
      app::util::AppRenderingManager::Description desc;
      {
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        desc.isUpperDisplayStereo = true;   //!< 立体視ON
      }

      gfl2::fs::AsyncFileManager * afm = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
      m_pRenderingManager->StartAsyncCreate( desc, &m_heap, NULL, afm );
    }

    // フレームマネージャの生成
    m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
  }
  
  if( m_pRenderingManager->IsCreated() )
  {
    // フレームの生成
    m_pFinderFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, FinderFrame>( m_pFrameManager );
    m_pFinderFrame->Setup( m_pAppParam, &m_heap, m_pRenderingManager );

    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン更新を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
gfl2::proc::Result FinderProc::UpdateFunc(gfl2::proc::Manager* pManager)
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
 * @brief   PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void FinderProc::DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo)
{
  GFL_UNUSED( pManager );

  m_pRenderingManager->Update();

  m_pFrameManager->Draw( displayNo );
  m_pRenderingManager->Draw( displayNo );
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
gfl2::proc::Result FinderProc::EndFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );

  // 終了処理が完了するまで待つ
  if( m_pFrameManager->End() )
  {
    return gfl2::proc::RES_CONTINUE;
  }
  if( !m_pRenderingManager->CanDestroy() )
  {
    return gfl2::proc::RES_CONTINUE;
  }

  GFL_SAFE_DELETE( m_pFrameManager );

  GFL_SAFE_DELETE( m_pRenderingManager );

  {
    gfl2::heap::NwAllocator * allocator;

    allocator = m_heap.GetSystemAllocator();
    GFL_DELETE allocator;

    allocator = m_heap.GetDeviceAllocator();
    GFL_DELETE allocator;
  }
  {
    gfl2::util::GLHeapAllocator * allocator;

    allocator = m_heap.GetGLHeapAllocator();
    GFL_DELETE allocator;

    allocator = m_heap.GetDevGLHeapAllocator();
    GFL_DELETE allocator;
  }
  m_heap.Delete();
  
  GFL_DELETE_HEAP( m_pManagerHeap );

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief  フィールド復帰　再稼働
 */
//------------------------------------------------------------------------------
void FinderProc::Resume( void )
{

}

//------------------------------------------------------------------------------
/**
 * @brief  フィールド削除　一時停止
 */
//------------------------------------------------------------------------------
void FinderProc::Suspend( void )
{

}

//------------------------------------------------------------------------------
/**
  * @brief   ポケファインダーフレームの取得
  * @return  FinderFrameクラスポインタ
  */
//------------------------------------------------------------------------------
FinderFrame* FinderProc::GetFinderFrame( void )
{
  return m_pFinderFrame;
}

//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータのセット
 * @param   pAppParam   外部設定パラメータ
 */
//------------------------------------------------------------------------------
void FinderProc::SetAppParam( APP_PARAM* pAppParam )
{
  m_pAppParam = pAppParam;
}


GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )
