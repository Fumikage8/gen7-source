//======================================================================
/**
 * @file FieldDebugMenuAttrDispEditor.cpp
 * @date 2015/09/08 14:55:22
 * @author ikuta_junya
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#include "Field/FieldDebug/include/Menu/FieldDebugMenuAttrDispEditor.h"

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <Debug/include/gfl2_DebugPrint.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryCollisionMdlData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>

// niji
#include "System/include/nijiAllocator.h"

// gameSys
#include "GameSys/include/GameManager.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainLayoutData.h"

// conv_header
#include <niji_conv_header/message/debug/msg_debug_menu.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/debug_message.gaix>

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)
GFL_NAMESPACE_BEGIN(Menu)

static const f32 ATTR_ALPHA( 0.5f );

static const gfl2::math::Vector4 sc_AttrColorTable[] =
{
  gfl2::math::Vector4( 0.0f, 0.0f, 0.0f, 0.0f ),
  gfl2::math::Vector4( 1.0f, 0.0f, 0.0f, ATTR_ALPHA ),
  gfl2::math::Vector4( 0.0f, 1.0f, 0.0f, ATTR_ALPHA ),
  gfl2::math::Vector4( 0.0f, 0.0f, 1.0f, ATTR_ALPHA ),
  gfl2::math::Vector4( 1.0f, 1.0f, 0.0f, ATTR_ALPHA ),
  gfl2::math::Vector4( 1.0f, 0.0f, 1.0f, ATTR_ALPHA ),
  gfl2::math::Vector4( 0.0f, 1.0f, 1.0f, ATTR_ALPHA )
};

static const u32 ATTR_COLOR_INDEX_MAX = GFL_NELEMS( sc_AttrColorTable );

//------------------------------------------------------------------------------
// STATIC
//------------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN(Item)

void AttrAllChangeValue( void *work )
{
  for( u32 i = 0; i < GFL_NELEMS( Field::Debug::DebugTypes::s_AttrColorIndexTable ); i++ )
  {
    Field::Debug::DebugTypes::s_AttrColorIndexTable[i] = Field::Debug::DebugTypes::s_AttrColorIndexAll;
  }
}

static wchar_t* AttrDisp( void *userWork, void* pValue, gfl2::debug::DebugWinItem *item )
{
  gfl2::str::MsgData* pMsgData = (gfl2::str::MsgData*)userWork;
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  s32 value = *(s32*)pValue;
  
  gfl2::str::StrBuf *workStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  pMsgData->GetString( msg_dmenu_attr_color_00 + value, *workStrBuf );

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}

static void CreateGroup( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugMenuAttrDispEditor *pDebug = reinterpret_cast<DebugMenuAttrDispEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  gfl2::str::MsgData* pMsgData = pDebug->GetDebugMenuMagData();
  gfl2::str::MsgData* pMsgDataAttrName = pDebug->GetAttrNameMagData();

  // アトリビュートをチェック、ものすごく重い
  if( !pDebug->IsAttrCheck() )
  {
    pDebug->CheckAttr();
  }

  // 地形表示
  gfl2::debug::DebugWin_AddItemValueInt( pGroup, pHeap, pMsgData, msg_dmenu_attr_disp_editor_00, Field::Debug::DebugTypes::GetSwitchValuePtr( Field::Debug::DebugTypes::Switch::TERRAIN ), 0, 1, 1 );

  // アトリビュート表示
  gfl2::debug::DebugWin_AddItemValueInt( pGroup, pHeap, pMsgData, msg_dmenu_attr_disp_editor_02, Field::Debug::DebugTypes::GetSwitchValuePtr( Field::Debug::DebugTypes::Switch::VISIBLE_TERRAIN_COLLISION ), 0, 1, 1 );

  // 深度テスト
  gfl2::debug::DebugWin_AddItemValueInt( pGroup, pHeap, pMsgData, msg_dmenu_attr_disp_editor_03, Field::Debug::DebugTypes::GetSwitchValuePtr( Field::Debug::DebugTypes::Switch::COLLISION_OPACITY ), 0, 1, 1 );
  
  // Yオフセット
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_attr_disp_editor_05, &Field::Debug::DebugTypes::s_AttrDrawOffset, -1000.0f, 1000.0f, 0.1f );

  // アトリビュート関連項目を追加
  gfl2::debug::DebugWinFunc* pFunc = gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_attr_disp_editor_01, NULL, NULL );
  if( pFunc )
  {
    pFunc->SetIsSelectSkip( true );
  }

  for( u32 i = 0; i < ATTRIBUTE_MAX; i++ )
  {
    if( !pDebug->IsAttrExist( i ) )
    {
      continue;
    }

    // アトリビュート項目追加
    gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
    pMsgDataAttrName->GetString( i, *workStrBuf );
    gfl2::debug::DebugWinItemValueDispOverRide<s32>::AddItem( pGroup, pHeap, workStrBuf->GetPtr(), pMsgData, Item::AttrDisp, &Field::Debug::DebugTypes::s_AttrColorIndexTable[i], 0, ATTR_COLOR_INDEX_MAX - 1, 1 );
  }

  if( pDebug->GetAttrErrorCount() > 0 )
  {
    gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
    pMsgDataAttrName->GetString( ATTRIBUTE_MAX, *workStrBuf );
    gfl2::debug::DebugWin_AddItemValueInt( pGroup, pHeap, workStrBuf->GetPtr(), &Field::Debug::DebugTypes::s_AttrColorIndexTable[ATTRIBUTE_MAX], 0, ATTR_COLOR_INDEX_MAX - 1, 1 );
  }
  
  // All
  gfl2::debug::DebugWinItemValueDispOverRide<s32>* pItem = gfl2::debug::DebugWinItemValueDispOverRide<s32>::AddItem( pGroup, pHeap, pMsgData, msg_dmenu_attr_disp_editor_04, pMsgData, Item::AttrDisp, &Field::Debug::DebugTypes::s_AttrColorIndexAll, 0, ATTR_COLOR_INDEX_MAX - 1, 1 );
  if( pItem )
  {
    pItem->SetChangedValueFunc( Item::AttrAllChangeValue, NULL );
  }
}

static void DeleteGroup( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugMenuAttrDispEditor *pDebug = reinterpret_cast<DebugMenuAttrDispEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}
GFL_NAMESPACE_END(Item)

# if 0
/**
 *  @brief  Group Creacte
 */
static void CreateDebugGroupFunc_DispModelList( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugMenuAttrDispEditor *pDebug = reinterpret_cast<DebugMenuAttrDispEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::debug::DebugWinGroup* pDebugGroup = pDebug->GetDebugWinGroup( DebugMenuAttrDispEditor::DEBUG_GROUP_INDEX_DISP_MODEL_LIST);

    DebugMenuAttrDispEditorModeManager* pModeManager = pDebug->GetModeManager();
    pModeManager->Setup( DebugMenuAttrDispEditorModeManager::MODE_DISP_MODEL_LIST, pDebugGroup);

    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    Fieldmap* pFieldmap = pDebug->GetFieldmap();
    FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
    GameSys::GameManager* pGameManager = pDebug->GetGameManager();

    EventDebugMenuAttrDispEditor* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugMenuAttrDispEditor>( pGameManager->GetGameEventManager());
    EventDebugMenuAttrDispEditor::InitializeDescription desc;
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
  DebugMenuAttrDispEditor *pDebug = reinterpret_cast<DebugMenuAttrDispEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::debug::DebugWinGroup* pDebugGroup = pDebug->GetDebugWinGroup( DebugMenuAttrDispEditor::DEBUG_GROUP_INDEX_EDIT_PLACEMENT);

    DebugMenuAttrDispEditorModeManager* pModeManager = pDebug->GetModeManager();
    pModeManager->Setup( DebugMenuAttrDispEditorModeManager::MODE_PLACEMENT_EDIT, pDebugGroup);

    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    Fieldmap* pFieldmap = pDebug->GetFieldmap();
    FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
    GameSys::GameManager* pGameManager = pDebug->GetGameManager();

    EventDebugMenuAttrDispEditor* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugMenuAttrDispEditor>( pGameManager->GetGameEventManager());
    EventDebugMenuAttrDispEditor::InitializeDescription desc;
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
  DebugMenuAttrDispEditor *pDebug = reinterpret_cast<DebugMenuAttrDispEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::debug::DebugWinGroup* pDebugGroup = pDebug->GetDebugWinGroup( DebugMenuAttrDispEditor::DEBUG_GROUP_INDEX_PLAY_MOTION);

    DebugMenuAttrDispEditorModeManager* pModeManager = pDebug->GetModeManager();
    pModeManager->Setup( DebugMenuAttrDispEditorModeManager::MODE_PLAY_MOTION, pDebugGroup);

    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    Fieldmap* pFieldmap = pDebug->GetFieldmap();
    FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
    GameSys::GameManager* pGameManager = pDebug->GetGameManager();

    EventDebugMenuAttrDispEditor* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugMenuAttrDispEditor>( pGameManager->GetGameEventManager());
    EventDebugMenuAttrDispEditor::InitializeDescription desc;
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
  DebugMenuAttrDispEditor *pDebug = reinterpret_cast<DebugMenuAttrDispEditor*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::debug::DebugWinGroup* pDebugGroup = pDebug->GetDebugWinGroup( DebugMenuAttrDispEditor::DEBUG_GROUP_INDEX_PLAY_DYNAMIC_MOTION);

    DebugMenuAttrDispEditorModeManager* pModeManager = pDebug->GetModeManager();
    pModeManager->Setup( DebugMenuAttrDispEditorModeManager::MODE_PLAY_DYNAMIC_MOTION, pDebugGroup);

    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    Fieldmap* pFieldmap = pDebug->GetFieldmap();
    FieldScript::FieldScriptSystem* pScriptSystem = pDebug->GetFieldScriptSystem();
    GameSys::GameManager* pGameManager = pDebug->GetGameManager();

    EventDebugMenuAttrDispEditor* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugMenuAttrDispEditor>( pGameManager->GetGameEventManager());
    EventDebugMenuAttrDispEditor::InitializeDescription desc;
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
  DebugMenuAttrDispEditor *pDebug = reinterpret_cast<DebugMenuAttrDispEditor*>( userWork );
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

    gfl2::debug::DebugWinGroup* pDebugGroupRoot = pDebug->GetDebugWinGroup( DebugMenuAttrDispEditor::DEBUG_GROUP_INDEX_ROOT);

    {
      gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_chara_layout_editor_disp_model_list, pDebugGroupRoot );
      pGroup->SetCreateDestroyGroupFunc( pDebug, CreateDebugGroupFunc_DispModelList, DestroyDebugGroupFunc_DispModelList );
      pDebug->SetDebugWinGroup( pGroup, DebugMenuAttrDispEditor::DEBUG_GROUP_INDEX_DISP_MODEL_LIST);
    }

    {
      gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_chara_layout_editor_edit_placement, pDebugGroupRoot );
      pGroup->SetCreateDestroyGroupFunc( pDebug, CreateDebugGroupFunc_EditPlacement, DestroyDebugGroupFunc_EditPlacement );
      pDebug->SetDebugWinGroup( pGroup, DebugMenuAttrDispEditor::DEBUG_GROUP_INDEX_EDIT_PLACEMENT);
    }

    {
      gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_chara_layout_editor_play_motion_static, pDebugGroupRoot );
      pGroup->SetCreateDestroyGroupFunc( pDebug, CreateDebugGroupFunc_PlayMotion, DestroyDebugGroupFunc_PlayMotion );
      pDebug->SetDebugWinGroup( pGroup, DebugMenuAttrDispEditor::DEBUG_GROUP_INDEX_PLAY_MOTION);
    }

    {
      gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_chara_layout_editor_play_motion_dynamic, pDebugGroupRoot );
      pGroup->SetCreateDestroyGroupFunc( pDebug, CreateDebugGroupFunc_PlayDynamicMotion, DestroyDebugGroupFunc_PlayDynamicMotion );
      pDebug->SetDebugWinGroup( pGroup, DebugMenuAttrDispEditor::DEBUG_GROUP_INDEX_PLAY_DYNAMIC_MOTION);
    }
  }
}

/**
 *  @brief  Group Destroy
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugMenuAttrDispEditor *pDebug = reinterpret_cast<DebugMenuAttrDispEditor*>( userWork );
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
#endif

//------------------------------------------------------------------------------
// DebugMenuAttrDispEditor
//------------------------------------------------------------------------------

/**
 * @class DebugMenuAttrDispEditor
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
DebugMenuAttrDispEditor::DebugMenuAttrDispEditor( void )
  : m_pHeap( NULL)
  , m_pFieldmap( NULL)
  , m_pMsgData( NULL)
  , m_pMsgDataAttrName( NULL)
  , m_pGameManager( NULL)
  , m_pAsyncFileReadManager( NULL)
{
  m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] = NULL;

  m_IsAttrCheck = false;
  for( u32 i = 0; i < ATTRIBUTE_MAX; i++ )
  {
    m_AttrCountTable[ i ] = 0;
  }
  m_AttrErrorCount = 0;

}

/**
 *  @brief  デストラクタ
 */
DebugMenuAttrDispEditor::~DebugMenuAttrDispEditor( void )
{
  Terminate();
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

/**
 *  @brief  初期化
 */
void DebugMenuAttrDispEditor::Initialize(
  gfl2::heap::HeapBase*             pHeap,
  gfl2::str::MsgData*               pMsgData,
  gfl2::str::MsgData*               pMsgDataAttrName,
  Fieldmap*                         pFieldmap,
  GameSys::GameManager*             pGameManager,
  gfl2::fs::AsyncFileManager*       pAsyncFileReadManager
  )
{
  m_pHeap = pHeap;
  m_pMsgData = pMsgData;
  m_pMsgDataAttrName = pMsgDataAttrName;
  m_pFieldmap = pFieldmap;
  m_pGameManager = pGameManager;
  m_pAsyncFileReadManager = pAsyncFileReadManager;


#if 0
  this->SyncArcOpen( m_pHeap, m_pAsyncFileReadManager, ARCID_CHARA_LAYOUT_EDITOR);
  this->SyncArcLoad( m_pHeap, m_pAsyncFileReadManager, ARCID_CHARA_LAYOUT_EDITOR, GARC_debug_chara_layout_editor_msg_id_offset_BIN, &m_pDynamicMotionMsgIDData, NULL);
  this->SyncArcClose( m_pHeap, m_pAsyncFileReadManager, ARCID_CHARA_LAYOUT_EDITOR);
#endif
}

/**
 *  @brief  破棄
 */
void DebugMenuAttrDispEditor::Terminate( void )
{
  DeleteDebugMenu();
}

/**
 *  @brief  セットアップ
 */
void DebugMenuAttrDispEditor::Setup()
{
}

/**
 *  @brief  クリーン
 */
void DebugMenuAttrDispEditor::Clean( void )
{
}

/**
 *  @brief  メニューの作成
 */
void DebugMenuAttrDispEditor::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_attr_disp_editor, root, ATTRIBUTE_MAX + 7 );
  m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT]->SetCreateDestroyGroupFunc( this, Item::CreateGroup, Item::DeleteGroup );
}

/**
*  @brief  メニューの破棄
*/
void DebugMenuAttrDispEditor::DeleteDebugMenu( void )
{
  if( m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] )
  {
    gfl2::debug::DebugWin_RemoveGroup( m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] );
    m_pDebugGroup[DEBUG_GROUP_INDEX_ROOT] = NULL;
  }
}

//--------------------------------------------------------------
/**
* @brief   アトリビュートカラーを更新
*/
//--------------------------------------------------------------
void DebugMenuAttrDispEditor::UpdateColor( void )
{
  for( u32 i = 0; i < GFL_NELEMS( Field::Debug::DebugTypes::s_AttrColorIndexTable ); i++ )
  {
    u32 colorIndex = Field::Debug::DebugTypes::s_AttrColorIndexTable[ i ];
    Field::Debug::DebugTypes::s_AttrColorTable[ i ] = sc_AttrColorTable[ colorIndex ];
  }
}

//
// メニュー生成関数向け
// 

/**
* @brief 今のエリアのアトリビュート情報を取得
* @note  全ブロックをロードし探索するため、かなり重い
*/
void DebugMenuAttrDispEditor::CheckAttr( void )
{
  Field::Terrain::TerrainManager* pTerrainManager = m_pFieldmap->GetTerrainManager();
  const Field::Terrain::TerrainLayoutData* pLayout = pTerrainManager->GetLayoutData();
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  System::nijiAllocator* pAllocator = GFL_NEW(pHeap) System::nijiAllocator( pHeap, System::nijiAllocator::APPLICATION );
  const gfl2::fs::ArcFile* pFile = m_pAsyncFileReadManager->GetArcFile( ARCID_FIELD_TERRAIN_HIGH_MODEL );

  // 初期化
  for( u32 i = 0; i < ATTRIBUTE_MAX; i++ )
  {
    m_AttrCountTable[ i ] = 0;
  }
  m_AttrErrorCount = 0;
  
  // 全ブロックをロードし、アトリビュートをカウントする 
  for( u32 x = 0; x < pLayout->GetBlockNumOfXAxis(); x++ )
  {
    for( u32 z = 0; z < pLayout->GetBlockNumOfZAxis(); z++ )
    {
      u32 datId = pLayout->GetHighModelDataIndex( x, z, pTerrainManager->GetReplaceNo( x, z ) );
      if( datId == Field::Terrain::DATA_ID_INVALID_VALUE )
      {
        continue;
      }

      // データが0バイト(ダミー)になっていないか？ (製品版ではありえない)
      size_t dataSize;
      gfl2::fs::Result fsResult;
      fsResult = pFile->GetRealDataSize( &dataSize, datId, pHeap );

      // データサイズ0はダミーデータが登録されている
      if( fsResult.IsSuccess() == false || dataSize == 0 )
      {
        continue;
      }

      // ロード
      void* pData;
      u32 size;
      SyncArcLoad( pHeap, m_pAsyncFileReadManager, ARCID_FIELD_TERRAIN_HIGH_MODEL, datId, &pData, &size );
       
      gfl2::fs::BinLinkerAccessor dataAccessor( pData );
      gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
      resourceData.SetModelData( static_cast<c8*>( dataAccessor.GetData( 6 )), dataAccessor.GetDataSize( 6 ) ); 
      gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &resourceData );

      // 床コリをインスタンス化
      gfl2::collision::CollisionModel* pClollisionModel = GFL_NEW( pHeap ) gfl2::collision::CollisionModel();
      pClollisionModel->Initialize( pResourceNode );


      // アトリビュート探索
      gfl2::renderingengine::scenegraph::resource::CollisionMeshResourceNode* pNode = pClollisionModel->GetCollisionMeshNode();
      u32 triangleCount = pNode->GetCollisionTriangleCount();
      for (u32 i = 0; i < triangleCount; ++i)
      {
        const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle = pNode->GetCollisionTriangle(i);
        if( pTriangle->m_GroundAttribute >= 0 && pTriangle->m_GroundAttribute < ATTRIBUTE_MAX )
        {
          m_AttrCountTable[ pTriangle->m_GroundAttribute ] += 1;
        }
        else
        {
          m_AttrErrorCount++;
        }
      }

      // 破棄
      GFL_SAFE_DELETE( pClollisionModel );
      GFL_SAFE_DELETE( pResourceNode );
      GflHeapSafeFreeMemory( pData );
    }
  }

  GFL_SAFE_DELETE( pAllocator );

  m_IsAttrCheck = true;
}

/**
* @brief 指定アトリビュートが存在するか取得
*/
b32 DebugMenuAttrDispEditor::IsAttrExist( u32 attrId )
{
  return ( m_AttrCountTable[ attrId ] > 0 );
}

/**
 *  @brief  同期arcオープン
 */
void DebugMenuAttrDispEditor::SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID)
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
void DebugMenuAttrDispEditor::SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
  closeReq.arcId = arcID;
  closeReq.heapForReq = pHeap;
  pAsyncFileReadManager->SyncArcFileClose( closeReq );
}

/**
 *  @brief  同期データ読み込み
 */
void DebugMenuAttrDispEditor::SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
  loadReq.arcId = arcID;
  loadReq.datId = datID;
  loadReq.ppBuf = ppBuff;
  loadReq.pBufSize = pSize;
  loadReq.heapForBuf = pHeap;
  loadReq.heapForCompressed = pHeap->GetLowerHandle();
  loadReq.align = 4;
  pAsyncFileReadManager->SyncArcFileLoadData( loadReq );
}


GFL_NAMESPACE_END(Menu)
GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG