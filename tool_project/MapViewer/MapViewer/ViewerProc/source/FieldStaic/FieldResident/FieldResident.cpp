//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldResident.h
 *	@brief  フィールドresident
 *	@author	tomoya takahashi
 *	@date		2015.05.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "ViewerProc/include/FieldStatic/FieldResident/FieldResident.h"

///#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"

// コリジョンシステム
#include "ViewerProc/include/FieldStatic/Collision/BaseActor.h"
#include "ViewerProc/include/FieldStatic/Collision/StaticActor.h"
#include "ViewerProc/include/FieldStatic/Collision/DynamicActor.h"
#include "ViewerProc/include/FieldStatic/Collision/BaseCollisionScene.h"
#include "ViewerProc/include/FieldStatic/Collision/CollisionSceneDefault.h"
#include "ViewerProc/include/FieldStatic/Collision/CollisionSceneNone.h"

// 配置モデル
///#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"

#include <heap/include/gfl2_heap_manager.h>

#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"

#include <arc_def_index/arc_def.h>

#include "System/include/nijiAllocator.h"

GFL_NAMESPACE_BEGIN(Field);

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 */
FieldResident::FieldResident() : 
  m_IsInitialized(false),
  m_InitializeSeq(0),
  m_pFieldHeap(NULL),
  m_pFieldDeviceHeap(NULL),
  m_pFieldDeviceHeapAllocator(NULL),
  m_pAsyncFileMan(NULL),
  m_BinAccessor()
  ///m_pPlacementDataManager(NULL)
{
  for( u32 i=0; i<COLLISION_SCENE_MAX; ++i ){
    m_pCollisionScene[i] = NULL;
  }
}

/**
 *  @brief  デストラクタ
 */
FieldResident::~FieldResident()
{
  this->Terminate();
}

/**
 *  @brief  初期化
 */
void FieldResident::Initialize( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::fs::AsyncFileManager* p_fileManager )
{
  m_pAsyncFileMan = p_fileManager;

  ///this->InitializeFieldDLL();
  this->InitializeHeap( pHeap, pDeviceHeap );
  this->InitializeCollisionScene();
  ///this->InitializePlacementDataManager();
  ///this->InitializeResource();
  ///this->InitializeStaticModelManager();

  m_InitializeSeq = 0;
}

/**
 *  @brief  初期化完了待ち
 */
bool FieldResident::InitializeWait()
{
  enum
  {
    SEQ_LOAD_WAIT,
    SEQ_END,
  };

  switch( m_InitializeSeq )
  {
  case SEQ_LOAD_WAIT:
    {
#if 0
      bool is_resource = this->WaitInitializeResource();
      if( !is_resource ){
        break;
      }
#endif //@saito_del
      m_InitializeSeq ++;
    }
    break;

  case SEQ_END:
    m_IsInitialized = true;
    return true;
  }

  return false;
}

/**
 *  @brief  破棄
 */
void FieldResident::Terminate( void )
{
  ///this->TerminateStaticModelManager();
  ///this->TerminateResouce();
  ///this->TerminatePlacementDataManager();
  this->TeminateCollisionScene();
  this->TerminateHeap();
  ///this->TerminateFieldDLL();

  m_IsInitialized = false;
}


//=============================================================================
// private
//=============================================================================

/**
 *  @ヒープ生成
 */
void FieldResident::InitializeHeap( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap )
{
  GFL_ASSERT( m_pFieldHeap == NULL );
  GFL_ASSERT( m_pFieldDeviceHeap == NULL );

  // フィールドプロック内でのヒープメモリリークを検知しやすいように専用の物を作成
  m_pFieldHeap = GFL_CREATE_HEAP( pHeap, HEAPID_FIELD , System::HeapSize::HEAP_FIELD_SIZE , gfl2::heap::HEAP_TYPE_EXP, false );
  m_pFieldDeviceHeap = GFL_CREATE_HEAP( pDeviceHeap, HEAPID_FIELD_DEVICE, System::HeapSize::HEAP_FIELD_DEVICE_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  m_pFieldDeviceHeapAllocator = GFL_NEW(m_pFieldDeviceHeap) System::nijiAllocator( m_pFieldDeviceHeap, System::nijiAllocator::APPLICATION );


  // フィールド下画面メニュー用ヒープ
  // @note 下記ヒープ内部の構成はメニュー班の管轄になります
  m_pFieldSubScreenHeap = GFL_CREATE_HEAP( pHeap, HEAPID_FIELD_SUBSCREEN , System::HeapSize::HEAP_FIELD_SUBSCREEN_SIZE , gfl2::heap::HEAP_TYPE_EXP, false );
  m_pFieldSubScreenDeviceHeap = GFL_CREATE_HEAP( pDeviceHeap, HEAPID_FIELD_SUBSCREEN_DEVICE, System::HeapSize::HEAP_FIELD_SUBSCREEN_DEVICE_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  m_pFieldSubScreenDeviceHeapAllocator = GFL_NEW(m_pFieldSubScreenDeviceHeap) System::nijiAllocator( m_pFieldSubScreenDeviceHeap, System::nijiAllocator::APPLICATION );
}

void FieldResident::TerminateHeap( void )
{
  if( m_pFieldSubScreenHeap ){
    GFL_DELETE(m_pFieldSubScreenDeviceHeapAllocator);
    GFL_DELETE_HEAP( m_pFieldSubScreenHeap );
    GFL_DELETE_HEAP( m_pFieldSubScreenDeviceHeap );

    m_pFieldSubScreenHeap = NULL;
    m_pFieldSubScreenDeviceHeap = NULL;
    m_pFieldSubScreenDeviceHeapAllocator = NULL;
  }

  if( m_pFieldHeap ){
    GFL_DELETE(m_pFieldDeviceHeapAllocator);
    GFL_DELETE_HEAP( m_pFieldHeap );
    GFL_DELETE_HEAP( m_pFieldDeviceHeap );

    m_pFieldHeap = NULL;
    m_pFieldDeviceHeap = NULL;
    m_pFieldDeviceHeapAllocator = NULL;
  }
}

#if 0
/**
 *  @brief  リソース初期化(読み込み開始）
 */
void FieldResident::InitializeResource()
{
  //---------------------------------------------------
  // ARCファイルオープン
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_FIELD_RESIDENT;
    openReq.heapForFile = m_pFieldDeviceHeap;
    openReq.heapForArcSrc = m_pFieldDeviceHeap->GetLowerHandle();
    openReq.heapForReq = m_pFieldDeviceHeap;
    m_pAsyncFileMan->AddArcFileOpenReq( openReq );
  }
  //---------------------------------------------------
  // ファイル読み込みリクエスト
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

    loadReq.arcId = ARCID_FIELD_RESIDENT;
    loadReq.datId = 0;
    loadReq.ppBuf = &m_pBuffer;
    loadReq.heapForBuf = m_pFieldDeviceHeap;
    loadReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
    loadReq.heapForCompressed = m_pFieldDeviceHeap->GetLowerHandle();
    loadReq.pBufSize = NULL;
    loadReq.align = 128;
    m_pAsyncFileMan->AddArcFileLoadDataReq( loadReq );
  }
}

/**
 *  @brief  リソース初期化待ち（読み込み待ち）
 */
bool FieldResident::WaitInitializeResource()
{
  // 読み込み完了待ち
  if( !m_pAsyncFileMan->IsArcFileOpenFinished( ARCID_FIELD_RESIDENT ) ){
    return false;
  }
  if( !m_pAsyncFileMan->IsArcFileLoadDataFinished( &m_pBuffer ) ){
    return false;
  }

  // リソースをBinLinkerに渡す。
  m_BinAccessor.Initialize(m_pBuffer);

  return true;
}

/**
 *  @brief  リソース破棄
 */
void FieldResident::TerminateResouce()
{
  if( m_pBuffer ){
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_FIELD_RESIDENT;
      closeReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
      m_pAsyncFileMan->SyncArcFileClose( closeReq );
    }


    GflHeapSafeFreeMemory( m_pBuffer );
    m_BinAccessor.Initialize(NULL);
  }
}


//! DLL
void FieldResident::InitializeFieldDLL()
{
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  m_pRoModule = roMgr->LoadModule(m_pAsyncFileMan, "FieldRo.cro");
  roMgr->StartModule( m_pRoModule );
}

void FieldResident::TerminateFieldDLL()
{
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  roMgr->DisposeModule( m_pRoModule );
}
#endif //@saito_del

//! コリジョンシーン生成
void FieldResident::InitializeCollisionScene()
{
  // コリジョンシステムの作成
  for( u32 i=0; i<COLLISION_SCENE_MAX; ++i )
  {
    m_pCollisionScene[i] = GFL_NEW(m_pFieldHeap) CollisionSceneDefault;
    m_pCollisionScene[i]->Setup( m_pFieldDeviceHeapAllocator,m_pFieldDeviceHeap );
  }
}

void FieldResident::TeminateCollisionScene()
{
  // コリジョンシステムの作成
  for( u32 i=0; i<COLLISION_SCENE_MAX; ++i )
  {
    GFL_SAFE_DELETE(m_pCollisionScene[i]);
  }
}
/**
//! 配置データマネージャー
void FieldResident::InitializePlacementDataManager()
{
  m_pPlacementDataManager = GFL_NEW(m_pFieldHeap) PlacementDataManager();
}

void FieldResident::TerminatePlacementDataManager()
{
  GFL_SAFE_DELETE( m_pPlacementDataManager );
}

//! 配置モデル生成
void FieldResident::InitializeStaticModelManager()
{
  m_pStaticModelManager = StaticModel::StaticModelManager::CreateStaticModelManager( m_pFieldDeviceHeapAllocator);
}

//! 配置モデル破棄
void FieldResident::TerminateStaticModelManager()
{
  GFL_SAFE_DELETE( m_pStaticModelManager);
}
*/

GFL_NAMESPACE_END(Field);
