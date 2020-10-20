//======================================================================
/**
 * @file FieldDebugTrialModel.cpp
 * @date 2015/09/08 14:55:22
 * @author miyachi_soichi
 * @brief デバッグ：TrialModel
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
// GameSys
#include "GameSys/include/GameManager.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
// TrialModel
#include "Field/FieldRo/include/TrialModel/FieldTrialModelFactory.h"
#include "Field/FieldRo/include/TrialModel/FieldTrialModel.h"
// self
#include "Field/FieldRo/include/Debug/FieldDebugTrialModel.h"
// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/trial_event.gaix>

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

#if PM_DEBUG

#define DEBUG_MONGOOSE1 0
#define DEBUG_MONGOOSE2 1

#if DEBUG_MONGOOSE1
static MonsNo debug_monsno = MONSNO_MANGUUSU1;
static u32 debug_dat_id = GARC_trial_event_trial_mongoose1_COMP;
static u32 debug_anim_maxi = 5;
#elif DEBUG_MONGOOSE2
static MonsNo debug_monsno = MONSNO_MANGUUSU2;
static u32 debug_dat_id = GARC_trial_event_trial_mongoose2_COMP;
static u32 debug_anim_maxi = 4;
#else
static MonsNo debug_monsno = MONSNO_PIKATYUU;
static u32 debug_dat_id = GARC_trial_event_test_trial_COMP;
static u32 debug_anim_maxi = 2;
#endif

static wchar_t* DrawStateFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( pDebug )
  {
    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
    switch( pDebug->m_eState )
    {
    case DebugTrialModel::STATE_NONE:
      pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_system_state_none, *workStrBuf );
      break;
    case DebugTrialModel::STATE_INIT:
      pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_system_state_init, *workStrBuf );
      if( pDebug->GetFactory()->IsInitialize() )
      {
        pDebug->m_eState = DebugTrialModel::STATE_READY;
        gfl2::heap::HeapBase *pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
        pDebug->GetFactory()->CreateSystem( pHeapBase );
      }
      break;
    case DebugTrialModel::STATE_READY:
      pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_system_state_ready, *workStrBuf );
      break;
    case DebugTrialModel::STATE_TERM:
      pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_system_state_term, *workStrBuf );
      if( pDebug->GetFactory()->IsDelete() )
      {
        pDebug->GetFactory()->FinalizeSystem();
        pDebug->m_eState = DebugTrialModel::STATE_NONE;
      }
      break;
    }
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %ls ]", item->GetName(), workStrBuf->GetPtr() );
  }
  return workStr;
}
static void UpdateInitFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug->m_eState == DebugTrialModel::STATE_NONE )
  {
    gfl2::heap::HeapBase *pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
    pDebug->GetFactory()->InitializeSystem( pHeapBase, pHeapBase, 1 );
    pDebug->m_eState = DebugTrialModel::STATE_INIT;
  }
}
static void UpdateTermFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug->m_eState == DebugTrialModel::STATE_READY )
  {
    pDebug->GetFactory()->DeleteSystem();
    pDebug->m_eState = DebugTrialModel::STATE_TERM;
  }
}
static wchar_t* DrawMotionStateFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::fs::AsyncFileManager *pFileMgr    = pGameManager->GetAsyncFileManager();

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( pDebug )
  {
    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();

    switch( pDebug->m_eMotionState )
    {
    case DebugTrialModel::MS_NONE:
       pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_motion_state_none, *workStrBuf );
     break;
    case DebugTrialModel::MS_OPNE_WAIT:
      pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_motion_state_opening, *workStrBuf );
      if( pFileMgr->IsArcFileOpenFinished( ARCID_TRIAL_EVENT ) )
      {
        pDebug->m_eMotionState = DebugTrialModel::MS_OPEN;
      }
      break;
    case DebugTrialModel::MS_OPEN:
       pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_motion_state_is_open, *workStrBuf );
     break;
    case DebugTrialModel::MS_LOAD_WAIT:
      pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_motion_state_loading, *workStrBuf );
      if( pFileMgr->IsArcFileLoadDataFinished( &(pDebug->m_pResource) ) )
      {
        pDebug->m_eMotionState = DebugTrialModel::MS_LOAD;
      }
      break;
    case DebugTrialModel::MS_LOAD:
      pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_motion_state_is_loaded, *workStrBuf );
      break;
    case DebugTrialModel::MS_CLOSE_WAIT:
      pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_motion_state_closing, *workStrBuf );
      if( pFileMgr->IsArcFileCloseFinished( ARCID_TRIAL_EVENT ) )
      {
        pDebug->m_eMotionState = DebugTrialModel::MS_NONE;
      }
      break;
    }
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %ls ]", item->GetName(), workStrBuf->GetPtr() );
  }

  return workStr;
}
static void UpdateMotionOpenFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  gfl2::fs::AsyncFileManager *pFileMgr    = pGameManager->GetAsyncFileManager();
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug->m_eMotionState == DebugTrialModel::MS_NONE )
  {
    gfl2::heap::HeapBase *pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openr;
    openr.arcId       = ARCID_TRIAL_EVENT;
    openr.heapForFile = pHeapBase;
    openr.heapForReq  = pHeapBase->GetLowerHandle();
    pFileMgr->AddArcFileOpenReq( openr );
    pDebug->m_eMotionState = DebugTrialModel::MS_OPNE_WAIT;
  }
}
static void UpdateMotionLoadFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  gfl2::fs::AsyncFileManager *pFileMgr    = pGameManager->GetAsyncFileManager();
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug->m_eMotionState == DebugTrialModel::MS_OPEN )
  {
    gfl2::heap::HeapBase *pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadr;
    loadr.arcId = ARCID_TRIAL_EVENT;
    loadr.datId = debug_dat_id;
    loadr.ppBuf = &(pDebug->m_pResource);
    loadr.heapForBuf = pHeapBase;
    loadr.heapForReq = pHeapBase->GetLowerHandle();
    loadr.heapForCompressed = pHeapBase->GetLowerHandle();
    loadr.align = 128;
    pFileMgr->AddArcFileLoadDataReq( loadr );
    pDebug->m_eMotionState = DebugTrialModel::MS_LOAD_WAIT;
  }
}
static void UpdateMotionCloseFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  gfl2::fs::AsyncFileManager *pFileMgr    = pGameManager->GetAsyncFileManager();
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug->m_eMotionState == DebugTrialModel::MS_LOAD )
  {
    gfl2::heap::HeapBase *pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closr;
    closr.arcId = ARCID_TRIAL_EVENT;
    closr.heapForReq = pHeapBase;
    pFileMgr->AddArcFileCloseReq( closr );
    pDebug->m_eMotionState = DebugTrialModel::MS_CLOSE_WAIT;
  }
}
static void UpdateInstanceCreateFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    gfl2::heap::HeapBase *pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );

    PokeTool::SimpleParam param;
    param.monsNo = debug_monsno;
    PokeTool::PokeModel::SetupOption opt;
    opt.dataHeap = pHeapBase;
    opt.workHeap = pHeapBase->GetLowerHandle();
    opt.animeType = PokeTool::MODEL_ANIMETYPE_BATTLE;
    opt.useShadow = false;
    pDebug->m_pModel = pDebug->GetFactory()->CreateTrialModel( pHeapBase, &param, opt );
  }
}
static wchar_t* DrawInstanceCreateFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( pDebug )
  {
    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
    if( !pDebug->m_pModel )
    {
      pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_instance_create, *workStrBuf );
    }
    else if( pDebug->m_pModel->IsEnable() )
    {
      pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_instance_still, *workStrBuf );
    }
    else if( pDebug->m_pModel->IsReady() )
    {
      gfl2::heap::HeapBase *pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );

      pDebug->m_pAllocator = GFL_NEW( pHeapBase ) System::nijiAllocator( pHeapBase );
      pDebug->m_pModel->CreateModel();
      pDebug->m_pModel->LoadMotion( pDebug->m_pAllocator, pHeapBase, pDebug->m_pResource );
    }
    else
    {
      pDebug->GetMessageData()->GetString( msg_dmenu_field_trial_model_instance_loading, *workStrBuf );
    }
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", workStrBuf->GetPtr() );
  }

  return workStr;
}
static void UpdateInstanceDispFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug->m_pModel )
  {
    if( pDebug->m_pModel->IsReady() )
    {
      bool flag = !pDebug->m_pModel->IsDisp();
      pDebug->m_pModel->SetDisp( flag );
    }
  }
}
static void UpdateInstanceDeleteFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->GetFactory()->DeleteTrialModel( pDebug->m_pModel );
    pDebug->m_pModel = NULL;
    GFL_SAFE_DELETE( pDebug->m_pAllocator );
  }
}
static void UpdateInstanceAnimFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->m_pModel->ChangeAnimation( pDebug->m_nAnim );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    -- pDebug->m_nAnim;
    if( pDebug->m_nAnim < 0 )
    {
      pDebug->m_nAnim = 0;
    }
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    ++ pDebug->m_nAnim;
    if( pDebug->m_nAnim >= debug_anim_maxi )
    {
      pDebug->m_nAnim = debug_anim_maxi-1;
    }
  }
}
static wchar_t* DrawInstanceAnimeFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->m_nAnim );
  return workStr;
}

/**
 *  @brief  Group ctor
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugTrialModel *pDebug = reinterpret_cast<DebugTrialModel*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData(); 
    {
      gfl2::debug::DebugWinGroup *pg_sys = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_field_trial_model_system_top, pGroup );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pg_sys, pHeap, pMsgData, msg_dmenu_field_trial_model_system_state, pDebug, NULL, DrawStateFunc );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pg_sys, pHeap, pMsgData, msg_dmenu_field_trial_model_system_init, pDebug, UpdateInitFunc, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pg_sys, pHeap, pMsgData, msg_dmenu_field_trial_model_system_term, pDebug, UpdateTermFunc, NULL );
    }
    {
      gfl2::debug::DebugWinGroup *pg_mot = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_field_trial_model_motion_top, pGroup );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pg_mot, pHeap, pMsgData, msg_dmenu_field_trial_model_motion_state, pDebug, NULL, DrawMotionStateFunc );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pg_mot, pHeap, pMsgData, msg_dmenu_field_trial_model_motion_open, pDebug, UpdateMotionOpenFunc, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pg_mot, pHeap, pMsgData, msg_dmenu_field_trial_model_motion_load, pDebug, UpdateMotionLoadFunc, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pg_mot, pHeap, pMsgData, msg_dmenu_field_trial_model_motion_close, pDebug, UpdateMotionCloseFunc, NULL );
    }
    {
      gfl2::debug::DebugWinGroup *pg_ins = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_field_trial_model_instance_top, pGroup );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pg_ins, pHeap, pMsgData, msg_dmenu_field_trial_model_instance_create, pDebug, UpdateInstanceCreateFunc, DrawInstanceCreateFunc );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pg_ins, pHeap, pMsgData, msg_dmenu_field_trial_model_instance_delete, pDebug, UpdateInstanceDeleteFunc, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pg_ins, pHeap, pMsgData, msg_dmenu_field_trial_model_instance_disp, pDebug, UpdateInstanceDispFunc, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pg_ins, pHeap, pMsgData, msg_dmenu_field_trial_model_instance_anim, pDebug, UpdateInstanceAnimFunc, DrawInstanceAnimeFunc );
    }
  }
}
/**
 *  @brief  Group dtor
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

/**
 * @class DebugTrialModel
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
DebugTrialModel::DebugTrialModel( void )
{
}

/**
 *  @brief  デストラクタ
 */
DebugTrialModel::~DebugTrialModel( void )
{
  Terminate();
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void DebugTrialModel::Initialize( gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData* pMsgData )
{
  m_pHeap = pHeap;
  m_pFactory = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetTrialModelFactory();

  m_eState = STATE_NONE;
  m_eMotionState = MS_NONE;
  m_pModel = NULL;
  m_pAllocator = NULL;
  m_nAnim = 0;

  m_pMsgData = pMsgData;
}

/**
 *  @brief  破棄
 */
void DebugTrialModel::Terminate( void )
{
}

/**
 *  @brief  メニューの作成
 */
void DebugTrialModel::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_trial_model_top, root );
  pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

#endif // PM_DEBUG

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)
