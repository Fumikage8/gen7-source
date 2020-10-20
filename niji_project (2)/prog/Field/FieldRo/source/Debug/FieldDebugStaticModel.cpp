//======================================================================
/**
 * @file FieldDebugStaticModel.cpp
 * @date 2015/09/08 14:55:22
 * @author saita_kazuki
 * @brief デバッグ：StaticModel 配置モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "Field/FieldRo/include/Debug/FieldDebugStaticModel.h"

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
// System
#include "System/include/nijiAllocator.h"
// GameSys
#include "GameSys/include/GameManager.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h" 
// @debug
#include "Field/FieldStatic/include/FieldArea.h"

// conv_header
#include <niji_conv_header/message/debug/msg_debug_menu.h>

// arc
#include <arc_def_index/arc_def.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

//------------------------------------------------------------------------------
// STATIC
//------------------------------------------------------------------------------

void UpdataFunc_CreateStaticModel( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugStaticModel* pThis = (DebugStaticModel*)userWork;

  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pThis->CreateStaticModel();
  }

  s32 add = 1;
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    add *= 10;
  }

  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    pThis->m_staticModelID += add;
    if( pThis->m_staticModelID >= STATIC_MODEL_ID_MAX)
    {
      pThis->m_staticModelID -= STATIC_MODEL_ID_MAX;
    }
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    pThis->m_staticModelID -= add;
    if( pThis->m_staticModelID < 0)
    {
      pThis->m_staticModelID += STATIC_MODEL_ID_MAX;
    }
  }
}

void UpdataFunc_CreateStaticModelDynamic( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugStaticModel* pThis = (DebugStaticModel*)userWork;

  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pThis->CreateStaticModelDynamic();
  }

  s32 add = 1;
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    add *= 10;
  }

  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    pThis->m_staticModelID += add;
    if( pThis->m_staticModelID >= STATIC_MODEL_ID_MAX)
    {
      pThis->m_staticModelID -= STATIC_MODEL_ID_MAX;
    }
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    pThis->m_staticModelID -= add;
    if( pThis->m_staticModelID < 0)
    {
      pThis->m_staticModelID += STATIC_MODEL_ID_MAX;
    }
  }
}

wchar_t* DrawFunc_CreateStaticModel( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugStaticModel* pThis = (DebugStaticModel*)userWork;

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), pThis->m_staticModelID );

  return workStr;
}

void UpdataFunc_WorkDump( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugStaticModel* pThis = (DebugStaticModel*)userWork;

  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pThis->DumpStaticModelWorks();
  }
}

void UpdataFunc_SetVisible( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugStaticModel* pThis = (DebugStaticModel*)userWork;

  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pThis->SetVisible( pThis->m_controlStaticModelID, !pThis->IsVisible( pThis->m_controlStaticModelID));
  }

  s32 add = 1;
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    add *= 5;
  }

  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    pThis->m_controlStaticModelID += add;
    if( pThis->m_controlStaticModelID >= StaticModel::StaticModelManager::STATIC_MODEL_MAX)
    {
      pThis->m_controlStaticModelID -= StaticModel::StaticModelManager::STATIC_MODEL_MAX;
    }
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    pThis->m_controlStaticModelID -= add;
    if( pThis->m_controlStaticModelID < 0)
    {
      pThis->m_controlStaticModelID += StaticModel::StaticModelManager::STATIC_MODEL_MAX;
    }
  }
}

wchar_t* DrawFunc_SetVisible( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugStaticModel* pThis = (DebugStaticModel*)userWork;

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ] [ Now %ls ]", item->GetName(), pThis->m_controlStaticModelID, pThis->IsVisible( pThis->m_controlStaticModelID) ? L"true" : L"false" );

  return workStr;
}

void UpdataFunc_SetVisibleAll( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugStaticModel* pThis = (DebugStaticModel*)userWork;

  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pThis->SetVisibleAll( pThis->m_isVisibleAll);
  }

  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) || pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    pThis->m_isVisibleAll = !pThis->m_isVisibleAll;
  }
}

wchar_t* DrawFunc_SetVisibleAll( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugStaticModel* pThis = (DebugStaticModel*)userWork;

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ Run %ls ]", item->GetName(), pThis->m_isVisibleAll ? L"true" : L"false" );

  return workStr;
}

void UpdataFunc_VisibleReset( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugStaticModel* pThis = (DebugStaticModel*)userWork;

  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pThis->ResetVisible();
  }
}

/**
 *  @brief  Group create
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugStaticModel *pDebug = reinterpret_cast<DebugStaticModel*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_static_model_create_static, pDebug, UpdataFunc_CreateStaticModel, DrawFunc_CreateStaticModel );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_static_model_create_dynamic, pDebug, UpdataFunc_CreateStaticModelDynamic, DrawFunc_CreateStaticModel );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_static_model_work_dump, pDebug, UpdataFunc_WorkDump, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_static_model_visible_set, pDebug, UpdataFunc_SetVisible, DrawFunc_SetVisible );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_static_model_visible_set_all, pDebug, UpdataFunc_SetVisibleAll, DrawFunc_SetVisibleAll );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_static_model_visible_reset, pDebug, UpdataFunc_VisibleReset, NULL );
  }

  pDebug->InitializeCreateDebugFunc();
}

/**
 *  @brief  Group destroy
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugStaticModel* pThis = (DebugStaticModel*)userWork;

  pThis->TerminateDestroyDebugFunc();

  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

//------------------------------------------------------------------------------
// DebugStaticModel
//------------------------------------------------------------------------------

/**
 * @class DebugStaticModel
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
DebugStaticModel::DebugStaticModel( void )
  : m_pHeap( NULL)
  , m_pFieldmap( NULL)
  , m_pRenderingPipeLine( NULL)
  , m_pMsgData( NULL)
  , m_pAsyncFileReadManager( NULL)
  , m_pStaticModelManager( NULL)
  , m_staticModelID( 0)
  , m_pos( gfl2::math::Vector3::GetZero())
  , m_controlStaticModelID( 0)
  , m_isVisibleAll( false)
{
  for( u32 i = 0; i < StaticModel::StaticModelManager::STATIC_MODEL_MAX; ++i)
  {
    m_pStaticModel[i] = NULL;
    m_pStaticModelPackData[i] = NULL;
    m_isVisibleDefault[i] = false;
  }
  m_rot = gfl2::math::Quaternion( 0.0f, 0.0f, 0.0f, 1.0f);
}

/**
 *  @brief  デストラクタ
 */
DebugStaticModel::~DebugStaticModel( void )
{
  Terminate();
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

/**
 *  @brief  初期化
 */
void DebugStaticModel::Initialize(
  gfl2::heap::HeapBase *pHeap,
  gfl2::fs::AsyncFileManager* pAsyncFileReadManager,
  Fieldmap* pFieldmap,
  MyRenderingPipeLine* pRenderingPipeLine,
  gfl2::str::MsgData* pMsgData
  )
{
  m_pHeap = pHeap;
  m_pFieldmap = pFieldmap;
  m_pRenderingPipeLine = pRenderingPipeLine;
  m_pMsgData = pMsgData;
  m_pAsyncFileReadManager = pAsyncFileReadManager;

  m_pStaticModelManager = m_pFieldmap->GetStaticModelManager();

  for( u32 i = 0; i < StaticModel::StaticModelManager::STATIC_MODEL_MAX; ++i)
  {
    m_pStaticModelPackData[i] = NULL;
  }

  this->SyncArcOpen( m_pHeap, m_pAsyncFileReadManager, ARCID_STATIC_MODEL_D);

  m_pAllocator = GFL_NEW( m_pHeap) System::nijiAllocator( m_pHeap);
}

/**
 *  @brief  破棄
 */
void DebugStaticModel::Terminate( void )
{
  for( u32 i = 0; i < StaticModel::StaticModelManager::STATIC_MODEL_MAX; ++i)
  {
    if( m_pStaticModel[i])
    {
      m_pRenderingPipeLine->RemoveEdgeRenderingTarget( m_pStaticModel[i]->GetBaseModel( StaticModel::MODEL_TYPE_HIGH)->GetModelInstanceNode());
    }
  }

  this->SyncArcClose( m_pHeap, m_pAsyncFileReadManager, ARCID_STATIC_MODEL_D);

  for( u32 i = 0; i < StaticModel::StaticModelManager::STATIC_MODEL_MAX; ++i)
  {
    GflHeapSafeFreeMemory( m_pStaticModelPackData[i]);
  }
  GFL_SAFE_DELETE( m_pAllocator );
}

/**
 *  @brief  メニューの作成
 */
void DebugStaticModel::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_static_model_title, root );
  pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

/**
 *  @brief  配置モデル作成
 */
void DebugStaticModel::CreateStaticModel()
{
  u32 index = 0;
  void** ppBuffer = NULL;
  ppBuffer = this->GetModelPackDataBuffer( &index);

  if( ppBuffer == NULL)
  {
    return;
  }

  this->SyncArcLoad( m_pHeap, m_pAsyncFileReadManager, ARCID_STATIC_MODEL_D, m_staticModelID, ppBuffer, NULL);

  StaticModel::StaticModel* pStaticModel = m_pStaticModelManager->CreateModelDebug(
      *ppBuffer,
      static_cast<u32>( m_staticModelID),
      m_pFieldmap->GetPlayerCharacter()->GetCharaDrawInstance()->GetPosition(),
      gfl2::math::Quaternion( 0.0f, 0.0f, 0.0f, 1.0f)
      );


  if( pStaticModel)
  {
    m_pStaticModel[index] = pStaticModel;
    m_pStaticModel[index]->SetVisibleAll( true);
    m_pStaticModel[index]->SetUpdateEnable( true);
    m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pStaticModel[index]->GetBaseModel( StaticModel::MODEL_TYPE_HIGH)->GetModelInstanceNode());
  }
}

/**
 *  @brief  配置モデル動的作成
 */
void DebugStaticModel::CreateStaticModelDynamic()
{
  u32 index = 0;
  void** ppBuffer = NULL;
  ppBuffer = this->GetModelPackDataBuffer( &index);

  if( ppBuffer == NULL)
  {
    return;
  }

  this->SyncArcLoad( m_pHeap, m_pAsyncFileReadManager, ARCID_STATIC_MODEL_D, m_staticModelID, ppBuffer, NULL);

  StaticModel::StaticModel* pStaticModel = m_pStaticModelManager->CreateModelDynamicDebug(
      *ppBuffer,
      static_cast<u32>( m_staticModelID),
      m_pFieldmap->GetPlayerCharacter()->GetCharaDrawInstance()->GetPosition(),
      gfl2::math::Quaternion( 0.0f, 0.0f, 0.0f, 1.0f)
      );

  if( pStaticModel)
  {
    m_pStaticModel[index] = pStaticModel;
    m_pStaticModel[index]->SetVisibleAll( true);
    m_pStaticModel[index]->SetUpdateEnable( true);
    m_pRenderingPipeLine->AddEdgeRenderingTarget( m_pStaticModel[index]->GetBaseModel( StaticModel::MODEL_TYPE_HIGH)->GetModelInstanceNode());
  }
}

/**
 *  @brief  配置モデルワーク出力
 */
void DebugStaticModel::DumpStaticModelWorks()
{
  GFL_PRINT( "StaticModel Work Dump\n");
  GFL_PRINT( "--------------------------------------------------\n");
  for( u32 i = 0; i < StaticModel::StaticModelManager::STATIC_MODEL_MAX; ++i)
  {
    StaticModel::StaticModel* pStaticModel = m_pStaticModelManager->GetStaticModel( i);
    if( pStaticModel && pStaticModel->IsEmpty() == false)
    {
      GFL_PRINT( "StaticModel [%2d] : EventID = %d, ModelID = %d\n", i, pStaticModel->GetEventID(), pStaticModel->GetStaticModelID());
    }
    else
    {
      GFL_PRINT( "StaticModel [%2d] : Empty\n", i);
    }
  }
  GFL_PRINT( "--------------------------------------------------\n");
}

/**
 *  @brief  配置モデルワーク表示ON/OFF
 */
void DebugStaticModel::SetVisible( u32 index, bool isVisible)
{
  StaticModel::StaticModel* pStaticModel = m_pStaticModelManager->GetStaticModel( index);
  if( pStaticModel && pStaticModel->IsEmpty() == false)
  {
    pStaticModel->SetVisible( isVisible);
  }
}

/**
 *  @brief  配置モデルワーク表示ON/OFF取得
 */
bool DebugStaticModel::IsVisible( u32 index)
{
  StaticModel::StaticModel* pStaticModel = m_pStaticModelManager->GetStaticModel( index);
  if( pStaticModel && pStaticModel->IsEmpty() == false)
  {
    return pStaticModel->GetVisible( );
  }
  return false;
}

/**
 *  @brief  全配置モデルワーク表示ON/OFF
 */
void DebugStaticModel::SetVisibleAll( bool isVisible)
{
  for( u32 i = 0; i < StaticModel::StaticModelManager::STATIC_MODEL_MAX; ++i)
  {
    this->SetVisible( i, isVisible);
  }
}

/**
 *  @brief  DebugGroupが生成されたときに呼ぶ初期化
 */
void DebugStaticModel::InitializeCreateDebugFunc()
{
  for( u32 i = 0; i < StaticModel::StaticModelManager::STATIC_MODEL_MAX; ++i)
  {
    StaticModel::StaticModel* pStaticModel = m_pStaticModelManager->GetStaticModel( i);
    if( pStaticModel && pStaticModel->IsEmpty() == false)
    {
      m_isVisibleDefault[i] = pStaticModel->GetVisible();
    }
  }
}

/**
 *  @brief  DebugGroupが破棄されたときに呼ぶ破棄
 */
void DebugStaticModel::TerminateDestroyDebugFunc()
{
}

/**
 *  @brief  リセット
 */
void DebugStaticModel::ResetVisible()
{
  for( u32 i = 0; i < StaticModel::StaticModelManager::STATIC_MODEL_MAX; ++i)
  {
    StaticModel::StaticModel* pStaticModel = m_pStaticModelManager->GetStaticModel( i);
    if( pStaticModel && pStaticModel->IsEmpty() == false)
    {
      pStaticModel->SetVisible( m_isVisibleDefault[i]);
    }
  }
}

//------------------------------------------------------------------------------
// PRIVATE
//------------------------------------------------------------------------------

/**
 *  @brief  同期arcオープン
 */
void DebugStaticModel::SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID)
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
  openReq.arcId = arcID;
  openReq.heapForFile = pHeap;
  openReq.heapForArcSrc = pHeap;
  openReq.heapForReq = pHeap->GetLowerHandle();
  pAsyncFileReadManager->SyncArcFileOpen( openReq );
}

/**
 *  @brief  同期arcクローズ
 */
void DebugStaticModel::SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
  closeReq.arcId = arcID;
  closeReq.heapForReq = pHeap;
  pAsyncFileReadManager->SyncArcFileClose( closeReq );
}

/**
 *  @brief  同期データ読み込み
 */
void DebugStaticModel::SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
  loadReq.arcId = arcID;
  loadReq.datId = datID;
  loadReq.ppBuf = ppBuff;
  loadReq.pBufSize = pSize;
  loadReq.heapForBuf = pHeap;
  loadReq.align = 128;
  pAsyncFileReadManager->SyncArcFileLoadData( loadReq );
}

/**
 *  @brief  データバッファ取得
 *  @param pOutWorkIndex バッファ番号
 *  @return バッファのポインタ
 */
void** DebugStaticModel::GetModelPackDataBuffer( u32* pOutWorkIndex)
{
  for( u32 i = 0; i < StaticModel::StaticModelManager::STATIC_MODEL_MAX; ++i)
  {
    if( m_pStaticModelPackData[i] == NULL)
    {
      *pOutWorkIndex = i;
      return &m_pStaticModelPackData[i];
    }
  }
  return NULL;
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
