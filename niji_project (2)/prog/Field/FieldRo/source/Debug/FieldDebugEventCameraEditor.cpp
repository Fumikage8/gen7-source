//======================================================================
/**
 * @file FieldDebugEventCameraEditor.cpp
 * @date 2015/09/08 14:55:22
 * @author saita_kazuki
 * @brief デバッグ：EventCameraEditor
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "Field/FieldRo/include/Debug/FieldDebugEventCameraEditor.h"

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

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

//------------------------------------------------------------------------------
// STATIC
//------------------------------------------------------------------------------

/**
 *  @brief  Group Creacte
 */
static void CreateDebugGroupFunc_Edit( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugEventCameraEditor *pDebug = reinterpret_cast<DebugEventCameraEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::debug::DebugWinGroup* pDebugGroup = pDebug->GetDebugWinGroup( DebugEventCameraEditor::DEBUG_GROUP_INDEX_EDIT);

    DebugEventCameraEditorModeManager* pModeManager = pDebug->GetModeManager();
    pModeManager->Setup( DebugEventCameraEditorModeManager::MODE_EDIT, pDebugGroup);

    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    Fieldmap* pFieldmap = pDebug->GetFieldmap();
    FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
    GameSys::GameManager* pGameManager = pDebug->GetGameManager();

    EventDebugEventCameraEditor* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugEventCameraEditor>( pGameManager->GetGameEventManager());
    EventDebugEventCameraEditor::InitializeDescription desc;
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
static void DestroyDebugGroupFunc_Edit( void *userWork, gfl2::debug::DebugWinItem *item )
{
}

/**
 *  @brief  Group Creacte
 */
static void CreateDebugGroupFunc_Framing( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugEventCameraEditor *pDebug = reinterpret_cast<DebugEventCameraEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::debug::DebugWinGroup* pDebugGroup = pDebug->GetDebugWinGroup( DebugEventCameraEditor::DEBUG_GROUP_INDEX_FRAMING);

    DebugEventCameraEditorModeManager* pModeManager = pDebug->GetModeManager();
    pModeManager->Setup( DebugEventCameraEditorModeManager::MODE_FRAMING, pDebugGroup);

    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    Fieldmap* pFieldmap = pDebug->GetFieldmap();
    FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
    GameSys::GameManager* pGameManager = pDebug->GetGameManager();

    EventDebugEventCameraEditor* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugEventCameraEditor>( pGameManager->GetGameEventManager());
    EventDebugEventCameraEditor::InitializeDescription desc;
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
static void DestroyDebugGroupFunc_Framing( void *userWork, gfl2::debug::DebugWinItem *item )
{
}

/**
 *  @brief  Group Creacte
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugEventCameraEditor *pDebug = reinterpret_cast<DebugEventCameraEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );


  // スクリプト実行中以外は何もしない
  Field::FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
  if( pScriptSystem->IsRunning() == false)
  {
    return;
  }

  Camera::CameraManager* pCameraManager = pDebug->GetFieldmap()->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Field::Camera::UnitNo::EVENT);

  if( pCameraUnit->GetControllerBase() == NULL)
  {
    GFL_PRINT( "EvCameraStart / EvCameraEnd コマンドを入れてください \n");
    return;
  }

  pCameraUnit->SetUpdateFlag( false);

  pDebug->Setup();


  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();

    gfl2::debug::DebugWinGroup* pDebugGroupRoot = pDebug->GetDebugWinGroup( DebugEventCameraEditor::DEBUG_GROUP_INDEX_ROOT);

    {
      gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_event_camera_editor_edit, pDebugGroupRoot );
      pGroup->SetCreateDestroyGroupFunc( pDebug, CreateDebugGroupFunc_Edit, DestroyDebugGroupFunc_Edit );
      pDebug->SetDebugWinGroup( pGroup, DebugEventCameraEditor::DEBUG_GROUP_INDEX_EDIT);
    }

    {
      gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_event_camera_editor_framing, pDebugGroupRoot );
      pGroup->SetCreateDestroyGroupFunc( pDebug, CreateDebugGroupFunc_Framing, DestroyDebugGroupFunc_Framing );
      pDebug->SetDebugWinGroup( pGroup, DebugEventCameraEditor::DEBUG_GROUP_INDEX_FRAMING);
    }

    {
      gfl2::debug::DebugWin_AddItemValueInt( pDebugGroupRoot, pHeap, L"上画面ヘルプ表示", &DebugEventCameraEditorModeManager::m_IsDrawTextInfo, 0, 1, 1);
    }

  }
}

/**
 *  @brief  Group Destroy
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugEventCameraEditor *pDebug = reinterpret_cast<DebugEventCameraEditor*>( userWork );
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
// DebugEventCameraEditor
//------------------------------------------------------------------------------

/**
 * @class DebugEventCameraEditor
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
DebugEventCameraEditor::DebugEventCameraEditor( void )
  : m_pHeap( NULL)
  , m_pFieldmap( NULL)
  , m_pMsgData( NULL)
  , m_pScriptSystem( NULL)
  , m_pModeManager( NULL)
{
}

/**
 *  @brief  デストラクタ
 */
DebugEventCameraEditor::~DebugEventCameraEditor( void )
{
  Terminate();
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

/**
 *  @brief  初期化
 */
void DebugEventCameraEditor::Initialize(
  gfl2::heap::HeapBase*             pHeap,
  gfl2::str::MsgData*               pMsgData,
  Fieldmap*                         pFieldmap,
  FieldScript::FieldScriptSystem*   pScriptSystem,
  GameSys::GameManager*             pGameManager
  )
{
  m_pHeap = pHeap;
  m_pMsgData = pMsgData;
  m_pFieldmap = pFieldmap;
  m_pScriptSystem = pScriptSystem;
  m_pGameManager = pGameManager;
}

/**
 *  @brief  破棄
 */
void DebugEventCameraEditor::Terminate( void )
{
  gfl2::debug::DebugWin_RemoveGroup( m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT]);
}

/**
 *  @brief  セットアップ
 */
void DebugEventCameraEditor::Setup()
{
  // モード管理インスタンス作成
  {
    m_pModeManager = GFL_NEW( m_pHeap) DebugEventCameraEditorModeManager();

    DebugEventCameraEditorModeManager::InitializeDescription desc;
    desc.pHeap            = m_pHeap;
    desc.pMsgData         = m_pMsgData;
    desc.pFieldmap        = m_pFieldmap;
    desc.pScriptSystem    = m_pScriptSystem;
    m_pModeManager->Initialize( desc);
  }
}

/**
 *  @brief  クリーン
 */
void DebugEventCameraEditor::Clean( void )
{
  // モード管理インスタンス破棄
  if( m_pModeManager)
  {
    m_pModeManager->Terminate();
    GFL_DELETE( m_pModeManager);
    m_pModeManager = NULL;
  
    // ターゲット座標表示OFF
    FIELD_DEBUG_SWITCH_SET( Debug::DebugTypes::Switch::VISIBLE_CAMERA_TARGET_POSITION, 0 );
  }
}

/**
 *  @brief  メニューの作成
 */
void DebugEventCameraEditor::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_event_camera_editor_title, root );
  m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT]->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

/**
*  @brief  メニューの破棄
*/
void DebugEventCameraEditor::DeleteDebugMenu( void )
{
  if( m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] )
  {
    gfl2::debug::DebugWin_RemoveGroup( m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] );
    m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] = NULL;
  }
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // GF_ENABLE_DEBUG_EDITOR
#endif // PM_DEBUG
