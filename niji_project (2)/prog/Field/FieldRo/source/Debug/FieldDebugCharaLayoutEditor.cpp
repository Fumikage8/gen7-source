//======================================================================
/**
 * @file FieldDebugCharaLayoutEditor.cpp
 * @date 2015/09/08 14:55:22
 * @author saita_kazuki
 * @brief デバッグ：CharaLayoutEditor
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "Field/FieldRo/include/Debug/FieldDebugCharaLayoutEditor.h"

#if defined(GF_ENABLE_DEBUG_EDITOR)

// gfl2

#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <Debug/include/gfl2_DebugPrint.h>


// gameSys
#include "GameSys/include/GameManager.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"

// script
#include "FieldScript/include/FieldScriptSystem.h"

// conv_header
#include <niji_conv_header/message/debug/msg_debug_menu.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/debug_message.gaix>
#include <arc_index/debug_chara_layout_editor.gaix>


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

//------------------------------------------------------------------------------
// STATIC
//------------------------------------------------------------------------------

/**
 *  @brief  Group Creacte
 */
static void CreateDebugGroupFunc_DispModelList( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugCharaLayoutEditor *pDebug = reinterpret_cast<DebugCharaLayoutEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::debug::DebugWinGroup* pDebugGroup = pDebug->GetDebugWinGroup( DebugCharaLayoutEditor::DEBUG_GROUP_INDEX_DISP_MODEL_LIST);

    DebugCharaLayoutEditorModeManager* pModeManager = pDebug->GetModeManager();
    pModeManager->Setup( DebugCharaLayoutEditorModeManager::MODE_DISP_MODEL_LIST, pDebugGroup);

    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    Fieldmap* pFieldmap = pDebug->GetFieldmap();
    FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
    GameSys::GameManager* pGameManager = pDebug->GetGameManager();

    EventDebugCharaLayoutEditor* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugCharaLayoutEditor>( pGameManager->GetGameEventManager());
    EventDebugCharaLayoutEditor::InitializeDescription desc;
    desc.pHeap          = pHeap;
    desc.pMsgData       = pMsgData;
    desc.pDebugGroup    = pDebugGroup;
    desc.pFieldmap      = pFieldmap;
    desc.pScriptSystem  = pScriptSystem;
    desc.pModeManager   = pModeManager;
    pEvent->Initialize( desc);
  }
}

/**
 *  @brief  Group Destroy
 */
static void DestroyDebugGroupFunc_DispModelList( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

/**
 *  @brief  Group Creacte
 */
static void CreateDebugGroupFunc_EditPlacement( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugCharaLayoutEditor *pDebug = reinterpret_cast<DebugCharaLayoutEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::debug::DebugWinGroup* pDebugGroup = pDebug->GetDebugWinGroup( DebugCharaLayoutEditor::DEBUG_GROUP_INDEX_EDIT_PLACEMENT);

    DebugCharaLayoutEditorModeManager* pModeManager = pDebug->GetModeManager();
    pModeManager->Setup( DebugCharaLayoutEditorModeManager::MODE_PLACEMENT_EDIT, pDebugGroup);

    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    Fieldmap* pFieldmap = pDebug->GetFieldmap();
    FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
    GameSys::GameManager* pGameManager = pDebug->GetGameManager();

    EventDebugCharaLayoutEditor* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugCharaLayoutEditor>( pGameManager->GetGameEventManager());
    EventDebugCharaLayoutEditor::InitializeDescription desc;
    desc.pHeap          = pHeap;
    desc.pMsgData       = pMsgData;
    desc.pDebugGroup    = pDebugGroup;
    desc.pFieldmap      = pFieldmap;
    desc.pScriptSystem  = pScriptSystem;
    desc.pModeManager   = pModeManager;
    pEvent->Initialize( desc);
  }
}

/**
 *  @brief  Group Destroy
 */
static void DestroyDebugGroupFunc_EditPlacement( void *userWork, gfl2::debug::DebugWinItem *item )
{
}

/**
 *  @brief  Group Creacte
 */
static void CreateDebugGroupFunc_PlayMotion( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugCharaLayoutEditor *pDebug = reinterpret_cast<DebugCharaLayoutEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::debug::DebugWinGroup* pDebugGroup = pDebug->GetDebugWinGroup( DebugCharaLayoutEditor::DEBUG_GROUP_INDEX_PLAY_MOTION);

    DebugCharaLayoutEditorModeManager* pModeManager = pDebug->GetModeManager();
    pModeManager->Setup( DebugCharaLayoutEditorModeManager::MODE_PLAY_MOTION, pDebugGroup);

    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    Fieldmap* pFieldmap = pDebug->GetFieldmap();
    FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
    GameSys::GameManager* pGameManager = pDebug->GetGameManager();

    EventDebugCharaLayoutEditor* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugCharaLayoutEditor>( pGameManager->GetGameEventManager());
    EventDebugCharaLayoutEditor::InitializeDescription desc;
    desc.pHeap          = pHeap;
    desc.pMsgData       = pMsgData;
    desc.pDebugGroup    = pDebugGroup;
    desc.pFieldmap      = pFieldmap;
    desc.pScriptSystem  = pScriptSystem;
    desc.pModeManager   = pModeManager;
    pEvent->Initialize( desc);
  }
}

/**
 *  @brief  Group Destroy
 */
static void DestroyDebugGroupFunc_PlayMotion( void *userWork, gfl2::debug::DebugWinItem *item )
{
}

/**
 *  @brief  Group Creacte
 */
static void CreateDebugGroupFunc_PlayDynamicMotion( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugCharaLayoutEditor *pDebug = reinterpret_cast<DebugCharaLayoutEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::debug::DebugWinGroup* pDebugGroup = pDebug->GetDebugWinGroup( DebugCharaLayoutEditor::DEBUG_GROUP_INDEX_PLAY_DYNAMIC_MOTION);

    DebugCharaLayoutEditorModeManager* pModeManager = pDebug->GetModeManager();
    pModeManager->Setup( DebugCharaLayoutEditorModeManager::MODE_PLAY_DYNAMIC_MOTION, pDebugGroup);

    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    Fieldmap* pFieldmap = pDebug->GetFieldmap();
    FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
    GameSys::GameManager* pGameManager = pDebug->GetGameManager();

    EventDebugCharaLayoutEditor* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugCharaLayoutEditor>( pGameManager->GetGameEventManager());
    EventDebugCharaLayoutEditor::InitializeDescription desc;
    desc.pHeap          = pHeap;
    desc.pMsgData       = pMsgData;
    desc.pDebugGroup    = pDebugGroup;
    desc.pFieldmap      = pFieldmap;
    desc.pScriptSystem  = pScriptSystem;
    desc.pModeManager   = pModeManager;
    pEvent->Initialize( desc);
  }
}

/**
 *  @brief  Group Destroy
 */
static void DestroyDebugGroupFunc_PlayDynamicMotion( void *userWork, gfl2::debug::DebugWinItem *item )
{
}

/**
 *  @brief  Group Creacte
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugCharaLayoutEditor *pDebug = reinterpret_cast<DebugCharaLayoutEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );


  // スクリプト実行中以外は何もしない
  Field::FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
  if( pScriptSystem->IsRunning() == false)
  {
    return;
  }

  // キャラが登録されていなかったら何もしない
  int editCharaNum = 0;
  pScriptSystem->DebugGetChrEditList( &editCharaNum);
  if( editCharaNum == 0)
  {
    return;
  }

  Camera::CameraManager* pCameraManager = pDebug->GetFieldmap()->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Field::Camera::UnitNo::EVENT);
  pCameraUnit->SetUpdateFlag( false);

  pDebug->Setup();


  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();

    gfl2::debug::DebugWinGroup* pDebugGroupRoot = pDebug->GetDebugWinGroup( DebugCharaLayoutEditor::DEBUG_GROUP_INDEX_ROOT);

    {
      gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_chara_layout_editor_disp_model_list, pDebugGroupRoot );
      pGroup->SetCreateDestroyGroupFunc( pDebug, CreateDebugGroupFunc_DispModelList, DestroyDebugGroupFunc_DispModelList );
      pDebug->SetDebugWinGroup( pGroup, DebugCharaLayoutEditor::DEBUG_GROUP_INDEX_DISP_MODEL_LIST);
    }

    {
      gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_chara_layout_editor_edit_placement, pDebugGroupRoot );
      pGroup->SetCreateDestroyGroupFunc( pDebug, CreateDebugGroupFunc_EditPlacement, DestroyDebugGroupFunc_EditPlacement );
      pDebug->SetDebugWinGroup( pGroup, DebugCharaLayoutEditor::DEBUG_GROUP_INDEX_EDIT_PLACEMENT);
    }

    {
      gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_chara_layout_editor_play_motion_static, pDebugGroupRoot );
      pGroup->SetCreateDestroyGroupFunc( pDebug, CreateDebugGroupFunc_PlayMotion, DestroyDebugGroupFunc_PlayMotion );
      pDebug->SetDebugWinGroup( pGroup, DebugCharaLayoutEditor::DEBUG_GROUP_INDEX_PLAY_MOTION);
    }

    {
      gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_chara_layout_editor_play_motion_dynamic, pDebugGroupRoot );
      pGroup->SetCreateDestroyGroupFunc( pDebug, CreateDebugGroupFunc_PlayDynamicMotion, DestroyDebugGroupFunc_PlayDynamicMotion );
      pDebug->SetDebugWinGroup( pGroup, DebugCharaLayoutEditor::DEBUG_GROUP_INDEX_PLAY_DYNAMIC_MOTION);
    }
  }
}

/**
 *  @brief  Group Destroy
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugCharaLayoutEditor *pDebug = reinterpret_cast<DebugCharaLayoutEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );

  Camera::CameraManager* pCameraManager = pDebug->GetFieldmap()->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Field::Camera::UnitNo::EVENT);
  pCameraUnit->SetUpdateFlag( true);

  pDebug->Clean();

  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}


//------------------------------------------------------------------------------
// DebugCharaLayoutEditor
//------------------------------------------------------------------------------

/**
 * @class DebugCharaLayoutEditor
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
DebugCharaLayoutEditor::DebugCharaLayoutEditor( void )
  : m_pHeap( NULL)
  , m_pFieldmap( NULL)
  , m_pMsgData( NULL)
  , m_pScriptSystem( NULL)
  , m_pGameManager( NULL)
  , m_pAsyncFileReadManager( NULL)
  , m_pModeManager( NULL)
  , m_pMsgDataMotion( NULL)
  , m_pMsgDataDynamicMotion( NULL)
  , m_pDynamicMotionMsgIDData( NULL)
{
}

/**
 *  @brief  デストラクタ
 */
DebugCharaLayoutEditor::~DebugCharaLayoutEditor( void )
{
  Terminate();
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

/**
 *  @brief  初期化
 */
void DebugCharaLayoutEditor::Initialize(
  gfl2::heap::HeapBase*             pHeap,
  gfl2::str::MsgData*               pMsgData,
  Fieldmap*                         pFieldmap,
  FieldScript::FieldScriptSystem*   pScriptSystem,
  GameSys::GameManager*             pGameManager,
  gfl2::fs::AsyncFileManager*       pAsyncFileReadManager
  )
{
  m_pHeap = pHeap;
  m_pMsgData = pMsgData;
  m_pFieldmap = pFieldmap;
  m_pScriptSystem = pScriptSystem;
  m_pGameManager = pGameManager;
  m_pAsyncFileReadManager = pAsyncFileReadManager;

  m_pMsgDataMotion = GFL_NEW( m_pHeap ) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_debug_field_chara_motion_DAT, m_pHeap, gfl2::str::MsgData::LOAD_PART/*LOAD_FULL*/ );
  m_pMsgDataDynamicMotion = GFL_NEW( m_pHeap ) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_debug_field_chara_dynamic_motion_DAT, m_pHeap, gfl2::str::MsgData::LOAD_PART/*LOAD_FULL*/ );

  this->SyncArcOpen( m_pHeap, m_pAsyncFileReadManager, ARCID_CHARA_LAYOUT_EDITOR);
  this->SyncArcLoad( m_pHeap, m_pAsyncFileReadManager, ARCID_CHARA_LAYOUT_EDITOR, GARC_debug_chara_layout_editor_msg_id_offset_BIN, &m_pDynamicMotionMsgIDData, NULL);
  this->SyncArcClose( m_pHeap, m_pAsyncFileReadManager, ARCID_CHARA_LAYOUT_EDITOR);
}

/**
 *  @brief  破棄
 */
void DebugCharaLayoutEditor::Terminate( void )
{
  GflHeapSafeFreeMemory( m_pDynamicMotionMsgIDData);
  GFL_SAFE_DELETE( m_pMsgDataMotion );
  GFL_SAFE_DELETE( m_pMsgDataDynamicMotion );
  gfl2::debug::DebugWin_RemoveGroup( m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT]);
}

/**
 *  @brief  セットアップ
 */
void DebugCharaLayoutEditor::Setup()
{
  // モード管理インスタンス作成
  {
    m_pModeManager = GFL_NEW( m_pHeap) DebugCharaLayoutEditorModeManager();

    DebugCharaLayoutEditorModeManager::InitializeDescription desc;
    desc.pHeap            = m_pHeap;
    desc.pMsgData         = m_pMsgData;
    desc.pFieldmap        = m_pFieldmap;
    desc.pScriptSystem    = m_pScriptSystem;
    desc.pMsgDataMotion           = m_pMsgDataMotion;
    desc.pMsgDataDynamicMotion    = m_pMsgDataDynamicMotion;
    desc.pDynamicMotionMsgIDData  = m_pDynamicMotionMsgIDData;
    m_pModeManager->Initialize( desc);
  }
}

/**
 *  @brief  クリーン
 */
void DebugCharaLayoutEditor::Clean( void )
{
  // モード管理インスタンス破棄
  if( m_pModeManager)
  {
    m_pModeManager->Terminate();
    GFL_DELETE( m_pModeManager);
    m_pModeManager = NULL;
  }
}

/**
 *  @brief  メニューの作成
 */
void DebugCharaLayoutEditor::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_chara_layout_editor_title, root );
  m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT]->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

/**
*  @brief  メニューの破棄
*/
void DebugCharaLayoutEditor::DeleteDebugMenu( void )
{
  if( m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] )
  {
    gfl2::debug::DebugWin_RemoveGroup( m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] );
    m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] = NULL;
  }
}

/**
 *  @brief  同期arcオープン
 */
void DebugCharaLayoutEditor::SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID)
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
  openReq.arcId = arcID;
  openReq.heapForFile = pHeap->GetLowerHandle();
  openReq.heapForArcSrc = pHeap->GetLowerHandle();
  openReq.heapForReq = pHeap->GetLowerHandle();
  pAsyncFileReadManager->SyncArcFileOpen( openReq );
}

/**
 *  @brief  同期arcクローズ
 */
void DebugCharaLayoutEditor::SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
  closeReq.arcId = arcID;
  closeReq.heapForReq = pHeap;
  pAsyncFileReadManager->SyncArcFileClose( closeReq );
}

/**
 *  @brief  同期データ読み込み
 */
void DebugCharaLayoutEditor::SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
  loadReq.arcId = arcID;
  loadReq.datId = datID;
  loadReq.ppBuf = ppBuff;
  loadReq.pBufSize = pSize;
  loadReq.heapForBuf = pHeap;
  loadReq.align = 4;
  pAsyncFileReadManager->SyncArcFileLoadData( loadReq );
}


GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // GF_ENABLE_DEBUG_EDITOR
#endif // PM_DEBUG
