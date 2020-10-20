// ============================================================================
/*
 * @file ApplicationWorkBase.cpp
 * @brief アプリを動かす為に必要なワークの基底クラスです。
 *        このクラスにはどのアプリでも必ず使うようなモジュールやデータを持つ事になります。
 * @date 2015.03.26
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


ApplicationWorkBase::ApplicationWorkBase( u32 heapSize, u32 deviceHeapSize ) :
  m_pAppFrameManagerHeap( NULL ),
  m_pAppHeap( NULL ),
  m_pDevHeap( NULL ),
  m_pDevHeapAllocator( NULL ),
  m_pDevGLHeapAllocator( NULL ),
  m_ExecuteFrameID( NetAppLib::System::FRAME_ID_INVALID ),
  m_LastExecuteFrameID( NetAppLib::System::FRAME_ID_INVALID ),
  m_FrameResult( NetAppLib::System::FRAME_RESULT_INVALID ),
  m_bSuspend( false ),
  m_pResourceManager( NULL ),
  m_pAppRenderingManager( NULL ),
  m_pMessageUtility( NULL ),
  m_pAppCountryRegionNameUtility( NULL ),
  m_pAppCommonGrpIconData( NULL )
{
  CreateHeap( HEAPID_APP_DEVICE, heapSize, deviceHeapSize );
}


ApplicationWorkBase::ApplicationWorkBase( gfl2::heap::HEAPID heapId, u32 heapSize, u32 deviceHeapSize ) :
  m_pAppFrameManagerHeap( NULL ),
  m_pAppHeap( NULL ),
  m_pDevHeap( NULL ),
  m_pDevHeapAllocator( NULL ),
  m_pDevGLHeapAllocator( NULL ),
  m_ExecuteFrameID( NetAppLib::System::FRAME_ID_INVALID ),
  m_LastExecuteFrameID( NetAppLib::System::FRAME_ID_INVALID ),
  m_FrameResult( NetAppLib::System::FRAME_RESULT_INVALID ),
  m_bSuspend( false ),
  m_pResourceManager( NULL ),
  m_pAppRenderingManager( NULL ),
  m_pMessageUtility( NULL ),
  m_pAppCountryRegionNameUtility( NULL ),
  m_pAppCommonGrpIconData( NULL )
{
  CreateHeap( heapId, heapSize, deviceHeapSize );
}


ApplicationWorkBase::~ApplicationWorkBase()
{
  DeleteHeap();
}


void ApplicationWorkBase::DeleteHeap()
{
  GFL_SAFE_DELETE( m_pDevGLHeapAllocator );

  GFL_SAFE_DELETE( m_pDevHeapAllocator );
  
  if( m_pDevHeap )
  {
    gfl2::heap::Manager::DeleteHeap( m_pDevHeap );
    m_pDevHeap = NULL;
  }

  GFL_SAFE_DELETE( m_pAppHeap );

  if( m_pAppFrameManagerHeap )
  {
    gfl2::heap::Manager::DeleteHeap( m_pAppFrameManagerHeap );
    m_pAppFrameManagerHeap = NULL;
  }
}


void ApplicationWorkBase::CreateHeap( gfl2::heap::HEAPID heapId, u32 heapSize, u32 deviceHeapSize )
{
	gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( heapId );

  if( !m_pAppFrameManagerHeap )
  {
    m_pAppFrameManagerHeap = GFL_CREATE_LOCAL_HEAP_NAME( pDevHeap, FRAME_MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "AppFrameManager" );
  }
  
  if( !m_pAppHeap )
  {
    m_pAppHeap = GFL_NEW( pDevHeap ) app::util::Heap();

    m_pDevHeap = GFL_CREATE_LOCAL_HEAP( pDevHeap, heapSize + deviceHeapSize, gfl2::heap::HEAP_TYPE_EXP, false );

    m_pDevHeapAllocator = GFL_NEW( m_pDevHeap ) gfl2::heap::NwAllocator( m_pDevHeap );

    m_pDevGLHeapAllocator = GFL_NEW( m_pDevHeap ) gfl2::util::GLHeapAllocator( m_pDevHeap );

    m_pAppHeap->Create( m_pDevHeap, m_pDevHeap, m_pDevHeapAllocator, m_pDevHeapAllocator, m_pDevGLHeapAllocator, m_pDevGLHeapAllocator );
  }
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)
