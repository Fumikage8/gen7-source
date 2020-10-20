/**
* @brief  フィールドデバッグメニュー、地形関連
* @file   FieldDebugMenuTerrain.cpp
* @author ikuta_junya
* @data   2015.06.07
*/

#if PM_DEBUG

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>

// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "Sound/include/Sound.h"

// Field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainBgPartsManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainBlock.h"


// Field Debug
#include "Field/FieldRo/include/Debug/Menu/FieldDebugMenuTerrain.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h" // @note フィールド上のデバック定義は可能な限りここに集める
#include "Field/FieldRo/include/Event/FieldEventDebugTerrainCheck.h"
#include "Field/FieldRo/include/Event/FieldEventDebugTerrainBlockControl.h"

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );
GFL_NAMESPACE_BEGIN( Menu );


// デバッグメニューで項目を選択中に実行される関数
GFL_NAMESPACE_BEGIN( Item );

static void BgPartsLoad( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  if( pFieldmap == NULL )
  {
    return;
  }
  
  gfl2::heap::HeapBase *eventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  Field::Terrain::TerrainManager* pTerrainManager = pFieldmap->GetTerrainManager();
  if( pTerrainManager->GetBgPartsManager()->LoadBgPartsAsync( 
    Field::Debug::DebugTypes::s_BgPartsNo, eventDeviceHeap, System::GflUse::GetEventDeviceAllocator() ) )
  {
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
  else
  {
    Sound::PlaySE( SEQ_SE_BEEP );
  }
}

static wchar_t* DrawBgPartsLoad( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
  Field::Terrain::TerrainManager* pTerrainManager = pFieldmap->GetTerrainManager();

  s32 value( 0 );
  if( pTerrainManager->GetBgPartsManager()->CheckBgPartsExist( Field::Debug::DebugTypes::s_BgPartsNo ) )
  {
    value = 1;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), value );
  return workStr;
}

static void BgPartsUnLoad( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  if( pFieldmap == NULL )
  {
    return;
  }
  
  gfl2::heap::HeapBase *eventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  Field::Terrain::TerrainManager* pTerrainManager = pFieldmap->GetTerrainManager();
  if( pTerrainManager->GetBgPartsManager()->UnLoadBgParts( 
    Field::Debug::DebugTypes::s_BgPartsNo ))
  {
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
  else
  {
    Sound::PlaySE( SEQ_SE_BEEP );
  }
}

static void BgPartsUnLoadAll( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  if( pFieldmap == NULL )
  {
    return;
  }

  Field::Terrain::TerrainManager* pTerrainManager = pFieldmap->GetTerrainManager();
  pTerrainManager->GetBgPartsManager()->UnLoadBgPartsAll();
  Sound::PlaySE( SEQ_SE_DECIDE1 );
}

// 地形チェックイベント起動
static void CallTerrainCheck( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  Event::EventDebugTerrainCheck* pEvent = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, Event::EventDebugTerrainCheck >( pGameManager->GetGameEventManager() );
  
  Event::EventDebugTerrainCheck::InitializeDescription initDesc;
  initDesc.m_pHeap = NULL;
  initDesc.dummy = 0;
  pEvent->Initialize( initDesc );

  // メニューを閉じる
  gfl2::debug::DebugWin_CloseWindow();
}

// 地形チェックイベント起動
#if ENABLE_DEBUG_TERRAIN_BLOCK_CONTROL
static void CallTerrainBlockControl( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  Event::EventDebugTerrainBlockControl* pEvent = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, Event::EventDebugTerrainBlockControl >( pGameManager->GetGameEventManager() );
  
  Event::EventDebugTerrainBlockControl::InitializeDescription initDesc;
  initDesc.m_pHeap = NULL;
  initDesc.dummy = 0;
  pEvent->Initialize( initDesc );

  // メニューを閉じる
  gfl2::debug::DebugWin_CloseWindow();
}
#endif

/**
 *  @brief  Group ctor
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  Terrain *pDebug = reinterpret_cast<Terrain*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
    Field::Terrain::TerrainManager* pTerrainManager = pFieldmap->GetTerrainManager();
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    
    gfl2::debug::DebugWin_AddItemValueInt( pGroup, pHeap, L"MiddleModelDrawLine", pTerrainManager->GetMiddleModelDrawLinePtr(), -1, 50, 1);
    gfl2::debug::DebugWin_AddItemValueU32( pGroup, pHeap, L"EncountGrassModelPriority", Field::Terrain::TerrainBlock::GetEncountGrassModelPriorityDefaultSettingPtr(), 0, 255, 1);
    {
      gfl2::debug::DebugWin_AddItemValueU32(
        pGroup, 
        pHeap, 
        pDebug->GetMessageData(),
        msg_dmenu_field_terrain_bg_parts_no,
        &Field::Debug::DebugTypes::s_BgPartsNo,
        0,
        255,
        1
        );
    }

    {
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_terrain_load, NULL, Item::BgPartsLoad, Item::DrawBgPartsLoad );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_terrain_unload, NULL, Item::BgPartsUnLoad, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_terrain_unload_all, NULL, Item::BgPartsUnLoadAll, NULL );
    }


    {
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_terrain_call_check, NULL, Item::CallTerrainCheck, NULL );
#if ENABLE_DEBUG_TERRAIN_BLOCK_CONTROL
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_terrain_call_block_ctrl, NULL, Item::CallTerrainBlockControl, NULL );
#endif
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

GFL_NAMESPACE_END( Item );



/**
* @brief コンストラクタ
* フィールドデバッグメニューの構築を実行します
*
* @param  pHeap         メニュー構築に使用するヒープ
* @param  pParentGroup  Terrainを追加するグループ
*
* @return 無し
*/ 
Terrain::Terrain( gfl2::heap::HeapBase* pHeap, gfl2::debug::DebugWinGroup* pParentGroup, gfl2::str::MsgData* pMsgData ) :
  m_pHeap( NULL )
  ,m_pParentGroup( NULL )
  ,m_pTerrainGrop( NULL )
{ 
  m_pHeap = pHeap;
  m_pParentGroup = pParentGroup;
  m_pMsgData = pMsgData;

  // Terrainに関するグループを追加
  {
    m_pTerrainGrop = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_terrain_top, m_pParentGroup );
    m_pTerrainGrop->SetCreateDestroyGroupFunc( this, Item::CreateDebugGroupFunc, Item::DestroyDebugGroupFunc );
  }
}

/**
* @brief デストラクタ
*
* @return 無し
*/
Terrain::~Terrain( void )
{
  // 登録したグループは親が開放してくれるが、念のため
}

GFL_NAMESPACE_END( Menu );
GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );


#endif // #if PM_DEBUG