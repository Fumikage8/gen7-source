//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   FieldResident.h
 *  @brief  フィールドresident
 *  @author tomoya takahashi
 *  @date   2015.05.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"

// コリジョンシステム
#include "Field/FieldRo/include/Collision/BaseActor.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"
#include "Field/FieldRo/include/Collision/DynamicActor.h"
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Field/FieldRo/include/Collision/CollisionSceneDefault.h"
#include "Field/FieldRo/include/Collision/CollisionSceneNone.h"

// 配置モデル
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"

// 環境音
#include "Field/FieldRo/include/EnvSound/EnvSoundManager.h"

#include <heap/include/gfl2_heap_manager.h>

#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"

#include <arc_def_index/arc_def.h>

#include "System/include/nijiAllocator.h"
#include "Print/include/WordSetLoader.h"

#include "GameSys/include/GameManager.h"
#include "System/include/Skybox/Skybox.h"

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"

//下画面
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProcManager.h"
#include "Field/FieldStatic/include/SubScreen/FieldSubScreen.h"


GFL_NAMESPACE_BEGIN(Field);

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

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
/**
 *  @brief  コンストラクタ
 */
FieldResident::FieldResident() : 
  m_IsInitialized(false),
  m_InitializeSeq(0),
  m_pFieldHeap(NULL),
  m_pFieldDeviceHeap(NULL),
  m_pRegidentBuffHeap(NULL),
  m_pFieldDeviceHeapAllocator(NULL),
  m_pAsyncFileMan(NULL),
  m_BinAccessor(),
  m_pPlacementDataManager(NULL),
  m_pEnvSoundManager(NULL),
  m_pGameData(NULL)
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
void FieldResident::Initialize( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::fs::AsyncFileManager* p_fileManager, const GameSys::GameData& gameData )
{
  m_pAsyncFileMan = p_fileManager;
  m_pGameData     = &gameData;

  this->InitializeSkyboxHeap();
  this->InitializeFieldDLL( m_pAsyncFileMan );
  this->InitializeHeap( pHeap, pDeviceHeap );
  this->InitializeCollisionScene();
  this->InitializeGimmickEncountManager();
  this->InitializePlacementDataManager();
  this->InitializeResource();
  this->InitializeStaticModelManager();
  this->InitializeEnvSoundManager();
  this->InitializeWordSetLoader();
  this->InitializeFieldSubScreenManager();

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
      bool is_resource = this->WaitInitializeResource();
      // WordSet待ち
      bool is_wordset = this->WaitInitializeWordSetLoader();

      if( !is_resource )
      {
        break;
      }
      if( !is_wordset ) { break; }

      m_InitializeSeq ++;
    }
    break;

  case SEQ_END:
    
    m_IsInitialized = true;

    // 『m_pRegidentBuffHeap』のメモリ残量監視用
    KAWAZOE_PRINT("▼★★★ m_pRegidentBuffHeap メモリ残量チェック ★★★▼\n");
    KAWAZOE_PRINT("TotalSize      :0x%08x[%d]\n", m_pRegidentBuffHeap->GetTotalSize(), m_pRegidentBuffHeap->GetTotalSize());
    KAWAZOE_PRINT("FreeSize       :0x%08x[%d]\n", m_pRegidentBuffHeap->GetTotalFreeSize(), m_pRegidentBuffHeap->GetTotalFreeSize());
    KAWAZOE_PRINT("AllocatableSize:0x%08x[%d]\n", m_pRegidentBuffHeap->GetTotalAllocatableSize(), m_pRegidentBuffHeap->GetTotalAllocatableSize());
    KAWAZOE_PRINT("▲★★★ m_pRegidentBuffHeap メモリ残量チェック ★★★▲\n");
    return true;
  }

  return false;
}

/**
 *  @brief  破棄
 */
void FieldResident::Terminate( void )
{
  this->TerminateFieldSubScreenManager();
  this->TerminateWordSetLoader();
  this->TerminateEnvSoundManager();
  this->TerminateStaticModelManager();
  this->TerminateResouce();
  this->TerminatePlacementDataManager();
  this->TerminateGimmickEncountManager();
  this->TeminateCollisionScene();
  this->TerminateHeap();
  this->TerminateFieldDLL();
  this->TerminateSkyboxHeap();

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

  // @note 地形ブロックを削減した分からmomijiを乗り越えるのに充分な200kbを追加。(170207)
  m_pRegidentBuffHeap = GFL_CREATE_LOCAL_HEAP_NAME(m_pFieldDeviceHeap, 0x26F700 + 0x2000 + 0x32000, gfl2::heap::HEAP_TYPE_EXP, false, "FieldRegident");
  m_pFieldDeviceHeapAllocator = GFL_NEW(m_pFieldDeviceHeap) System::nijiAllocator( m_pFieldDeviceHeap, System::nijiAllocator::APPLICATION );
  

  // フィールド下画面メニュー用ヒープ
  // @note 下記ヒープ内部の構成はメニュー班の管轄になります
  m_pFieldSubScreenHeap = GFL_CREATE_HEAP( pHeap, HEAPID_FIELD_SUBSCREEN , System::HeapSize::HEAP_FIELD_SUBSCREEN_SIZE , gfl2::heap::HEAP_TYPE_EXP, false );
  m_pFieldSubScreenDeviceHeap = GFL_CREATE_HEAP( pDeviceHeap, HEAPID_FIELD_SUBSCREEN_DEVICE, System::HeapSize::HEAP_FIELD_SUBSCREEN_DEVICE_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  m_pFieldSubScreenDeviceHeapAllocator = GFL_NEW(m_pFieldSubScreenDeviceHeap) System::nijiAllocator( m_pFieldSubScreenDeviceHeap, System::nijiAllocator::APPLICATION );


  GFL_PRINT( "AppHeap[0x%x][0x%x]\n",             pHeap->GetTotalFreeSize(), pHeap->GetTotalAllocatableSize() );
  GFL_PRINT( "AppDeviceHeap[0x%x][0x%x]\n",       pDeviceHeap->GetTotalFreeSize(), pDeviceHeap->GetTotalAllocatableSize() );
  GFL_PRINT( "FieldHeap[0x%x][0x%x]\n",       m_pFieldHeap->GetTotalFreeSize(), m_pFieldHeap->GetTotalAllocatableSize() );
  GFL_PRINT( "FieldDeviceHeap[0x%x][0x%x]\n", m_pFieldDeviceHeap->GetTotalFreeSize(), m_pFieldDeviceHeap->GetTotalAllocatableSize() );
  GFL_PRINT( "FieldSubScreenHeap[0x%x][0x%x]\n",       m_pFieldSubScreenHeap->GetTotalFreeSize(), m_pFieldSubScreenHeap->GetTotalAllocatableSize() );
  GFL_PRINT( "FieldSubScreenDeviceHeap[0x%x][0x%x]\n", m_pFieldSubScreenDeviceHeap->GetTotalFreeSize(), m_pFieldSubScreenDeviceHeap->GetTotalAllocatableSize() );
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
    GFL_DELETE_HEAP(m_pRegidentBuffHeap);
    GFL_DELETE_HEAP( m_pFieldHeap );
    GFL_DELETE_HEAP( m_pFieldDeviceHeap );

    m_pFieldHeap = NULL;
    m_pFieldDeviceHeap = NULL;
    m_pRegidentBuffHeap = NULL;
    m_pFieldDeviceHeapAllocator = NULL;
  }
}


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
    openReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
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
    loadReq.heapForBuf = m_pRegidentBuffHeap;
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
bool FieldResident::InitializeFieldDLL(gfl2::fs::AsyncFileManager * pAsyncFileMan)
{
  if(m_pRoModule == NULL)
  {
    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    m_pRoModule = roMgr->LoadModule(pAsyncFileMan, "FieldRo.cro");
    roMgr->StartModule( m_pRoModule );
    return true;
  }
  return false;
}

bool FieldResident::TerminateFieldDLL()
{
  if(m_pRoModule)
  {
    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    roMgr->DisposeModule( m_pRoModule );
    m_pRoModule = NULL;
    return true;
  }
  return false;
}


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


//! 環境音生成
void FieldResident::InitializeEnvSoundManager()
{
  m_pEnvSoundManager = GFL_NEW(m_pFieldDeviceHeap) EnvSoundManager();
}

//! 環境音破棄
void FieldResident::TerminateEnvSoundManager()
{
  GFL_SAFE_DELETE( m_pEnvSoundManager);
}

//! WordSetLoader生成
void FieldResident::InitializeWordSetLoader()
{
  m_pWordSetLoader = GFL_NEW( m_pFieldDeviceHeap ) WordSetLoader( m_pFieldDeviceHeap, PRELOAD_PLACENAME );
  m_pWordSetLoader->StartLoad( m_pFieldDeviceHeap->GetLowerHandle() );
}

//! WordSetLoader生成待ち
bool FieldResident::WaitInitializeWordSetLoader()
{
  return m_pWordSetLoader->WaitLoad();
}

//! WordSetLoader破棄
void FieldResident::TerminateWordSetLoader()
{
  if( m_pWordSetLoader ) m_pWordSetLoader->Release();
  GFL_SAFE_DELETE( m_pWordSetLoader );
}

//! 天球ヒープ生成
void FieldResident::InitializeSkyboxHeap()
{
  System::Skybox::Skybox* pSkybox = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetSkybox();
  if( pSkybox->IsCreateResource() == false)
  {
    pSkybox->CreateHeap();
  }
}
//! 天球ヒープ破棄
void FieldResident::TerminateSkyboxHeap()
{
  System::Skybox::Skybox* pSkybox = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetSkybox();
  if( pSkybox->IsCreateResource() == false)
  {
    pSkybox->DeleteHeap();
  }
}

//! ギミックエンカウントマネージャー生成
void FieldResident::InitializeGimmickEncountManager()
{
  m_pGimmickEncountManager = GFL_NEW( m_pFieldDeviceHeap) GimmickEncount::Manager();
}
//! ギミックエンカウントマネージャー破棄
void FieldResident::TerminateGimmickEncountManager()
{
  GFL_SAFE_DELETE( m_pGimmickEncountManager);
}


   //   {//フィールドサブスクリーン
//        gfl2::heap::HeapBase *systemHeap = m_pFieldResident->GetFieldSubScreenHeap();
//        gfl2::heap::HeapBase *devHeap =  m_pFieldResident->GetFieldSubScreenDeviceHeap();
//        m_pSubScreen = GFL_NEW(devHeap) ::Field::SubScreen::FieldSubScreenProcManager(systemHeap,devHeap);

//! フィールドサブスクリーン生成
void FieldResident::InitializeFieldSubScreenManager(void)
{
  gfl2::heap::HeapBase *systemHeap = GetFieldSubScreenHeap();
  gfl2::heap::HeapBase *devHeap =  GetFieldSubScreenDeviceHeap();
  m_pFieldSubScreenManager = GFL_NEW( devHeap) ::Field::SubScreen::FieldSubScreenProcManager(systemHeap,devHeap);
}
//! フィールドサブスクリーン破棄
void FieldResident::TerminateFieldSubScreenManager(void)
{
  GFL_SAFE_DELETE( m_pFieldSubScreenManager);
}




GFL_NAMESPACE_END(Field);
