//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugMapJump.cpp
 *	@brief  マップジャンプ
 *	@author	tomoya takahashi
 *	@date		2015.04.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#if PM_DEBUG

#include <thread/include/gfl2_ThreadStatic.h>


#include "Field/FieldRo/include/Debug/FieldDebugMapJump.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"


#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include <niji_conv_header/field/zone/zone_id.h>
#include <arc_def_index/arc_def.h>

// niji
#include "Sound/include/Sound.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/GameEventManager.h"
#include "System/include/HeapDefine.h"

#include <niji_conv_header/field/world/world_id.h>

#include "Field/FieldRo/include/Fieldmap.h"
#include "FieldDebugEventAutoAllMapJump.h"
#include "Field/FieldRo/include/debug/FieldDebugMenu.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "FieldScript/include/FieldScriptSystem.h"

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"
#include <arc_index/message.gaix>

GFL_NAMESPACE_BEGIN(Field);
GFL_NAMESPACE_BEGIN(Debug);
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

struct ZONE_JUMP_GROUP
{
  const c8* preFix; 
};


static const ZONE_JUMP_GROUP sc_ZONE_JUMP_GROUP[] = {
  {"Z_"},
  {"Z_F0102"},
  {"Z_F0201"},
  {"Z_F0301"},
  {"Z_F0401"},
  {"Z_F0501"},
  {"Z_F0601"},
  {"Z_F0701"},
  {"Z_F0801"},
  {"Z_TESTPL"},
  {"Z_TESTPG"},
  {"Z_TESTSP"},
  {"Z_TESTBG"},
  {"Z_TESTWTH"},
};


static const u32 ZONE_FILETER_MAX = ( GFL_NELEMS( sc_ZONE_JUMP_GROUP ) );

static s32 s_ZoneFilterId( 0 );
static s32 s_ZoneIndexForFilter( 0 );


s16 GetPreFixLength( const c8* preFix )
{
  const c8* check = preFix;
  s16 len = 0;

  b32 numFlag = 0;
  b32 textFlag = 0;

  while( check != '\0' )
  {
    if(
      static_cast<u8>( *check ) >= static_cast<u8>('A') &&
      static_cast<u8>( *check ) <= static_cast<u8>('Z')
      )
    {
      // A-Z
      if( numFlag )
      {
        break;
      }

      textFlag = 1;
    }
    else if(
      static_cast<u8>( *check ) >= static_cast<u8>('0') &&
      static_cast<u8>( *check ) <= static_cast<u8>('9')
      )
    {
      // 0-9
      if( textFlag )
      {
        break;
      }

      numFlag = 1;
    }

    len++;
    check++;
  }

  return len;
}


/**
 *    @brief マップジャンプデバッグ機能　Update
 */
static void UpdateDebugMapJump(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);

  // Aボタンが押されたらマップジャンプ
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  s32 add_num = 1;
  s32 index = static_cast<int>(*pThis->GetSelectZoneId());


  // L押しで5倍速
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) ){
    add_num = 5;
  }

  // R押しで10倍速
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) ){
    add_num = 10;
  }


  if( cross->IsRepeat( gfl2::ui::DIR_LEFT ) ){
    index -= add_num;
  }else if( cross->IsRepeat( gfl2::ui::DIR_RIGHT ) ){
    index += add_num;
  }
  if( index < 0 ){ index += pThis->GetZoneMax(); }
  if( index >= static_cast<s32>(pThis->GetZoneMax()) ){ index -= pThis->GetZoneMax(); }
  
  *pThis->GetSelectZoneId() = static_cast<u32>(index);



  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    // ワールドに登録されているゾーンかチェック
    u16 world_id = pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneWorldId( static_cast<ZoneID>(*pThis->GetSelectZoneId()) );
    const char * cpErrorStr = "このゾーンはWorldに登録されていない為ジャンプできません\n";

    bool jump = false;
    if( (world_id != WORLD_ERROR) )
    {
      gfl2::heap::HeapBase* pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
      World* pWorld = GFL_NEW(pDebugHeap) World( HEAPID_DEBUG );
      pWorld->Initialize( world_id, pGameManager->GetAsyncFileManager() );
      while( pWorld->InitializeWait() == false )
      {
        gfl2::thread::ThreadStatic::CurrentSleep(1000);
      }
      
      jump = pWorld->CanMapJump();

      if( !jump )
      {
        cpErrorStr = "このゾーンは設計マップ段階なためジャンプできません\n";
      }

      GFL_DELETE pWorld;
    }

    if( jump )
    {
      FieldScript::FieldScriptSystem* script = pGameManager->GetFieldScriptSystem();
      if( script->IsRunning()){
        GFL_PRINT( "スクリプトイベント実行中はデバッグマップジャンプできません\n" );
        return;
      }
      EventMapJump::CallMapJumpDefaultPos( pGameManager, *pThis->GetSelectZoneId() );
      // イベントステータスを立てる。
      pGameManager->GetGameData()->GetFieldCharaModelManager()->AddEventRequestAll();

      // ウィンドウを閉じる
      gfl2::debug::DebugWin_CloseWindow();
    }
    else
    {
      GFL_PRINT( cpErrorStr );
    }
  }
}

/**
 *    @brief マップジャンプ、決定ボタンで実行
 */
static void DebugMapJumpImpl(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);

  // Aボタンが押されたらマップジャンプ
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    // ワールドに登録されているゾーンかチェック
    u16 world_id = pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneWorldId( static_cast<ZoneID>(s_ZoneIndexForFilter) );
    const char * cpErrorStr = "このゾーンはWorldに登録されていない為ジャンプできません\n";

    bool jump = false;
    if( (world_id != WORLD_ERROR) )
    {
      gfl2::heap::HeapBase* pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
      World* pWorld = GFL_NEW(pDebugHeap) World( HEAPID_DEBUG );
      pWorld->Initialize( world_id, pGameManager->GetAsyncFileManager() );
      while( pWorld->InitializeWait() == false )
      {
        gfl2::thread::ThreadStatic::CurrentSleep(1000);
      }

      jump = pWorld->CanMapJump();

      if( !jump )
      {
        cpErrorStr = "このゾーンは設計マップ段階なためジャンプできません\n";
      }

      GFL_DELETE pWorld;
    }

    if( jump )
    {
      FieldScript::FieldScriptSystem* script = pGameManager->GetFieldScriptSystem();
      if( script->IsRunning()){
        Sound::PlaySE( SEQ_SE_BEEP );
        GFL_PRINT( "スクリプトイベント実行中はデバッグマップジャンプできません\n" );
        return;
      }

      Sound::PlaySE( SEQ_SE_DECIDE1 );
      EventMapJump::CallMapJumpDefaultPos( pGameManager, s_ZoneIndexForFilter );
      // イベントステータスを立てる。
      pGameManager->GetGameData()->GetFieldCharaModelManager()->AddEventRequestAll();
      // ウィンドウを閉じる
      gfl2::debug::DebugWin_CloseWindow();
    }
    else
    {
      Sound::PlaySE( SEQ_SE_BEEP );
      GFL_PRINT( cpErrorStr );
    }
  }

}

/**
*    @brief マップジャンプデバッグ機能　Update
*/
static void UpdateDebugMapJumpZoneFilter(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);

  // Aボタンが押されたらマップジャンプ
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  s32 add_num = 1;
  s32 nowZoneIndex = static_cast<int>(s_ZoneIndexForFilter);
  s32 index = static_cast<int>(s_ZoneIndexForFilter);
  s32 add_count = 0;

  //
  // ゾーン変更
  //
  if( cross->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    b32 isAdd( true );

    // R押しで接頭辞
    if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      s_ZoneIndexForFilter = pThis->GetZoneSkipNext( index, sc_ZONE_JUMP_GROUP[ s_ZoneFilterId ].preFix, 1 );
     
      // スキップなしなら通常加算
      if( s_ZoneIndexForFilter != nowZoneIndex )
      {
        isAdd = false;
      }
    }

    if( isAdd )
    {
      add_count = add_num;
      pThis->SetZoneIndexForFilterTop( index, add_count, true );
    }
  }
  else if( cross->IsRepeat( gfl2::ui::DIR_LEFT ) )
  {
    b32 isAdd( true );

    // R押しで接頭辞
    if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      s_ZoneIndexForFilter = pThis->GetZoneSkipNext( index, sc_ZONE_JUMP_GROUP[ s_ZoneFilterId ].preFix, -1 );

      // スキップなしなら通常加算
      if( s_ZoneIndexForFilter != nowZoneIndex )
      {
        isAdd = false;
      }
    }

    if( isAdd )
    {
      add_count = -add_num;
      pThis->SetZoneIndexForFilterTop( index, add_count, true );
    }
  }

  // 変更音
  if( s_ZoneIndexForFilter != nowZoneIndex )
  {
    Sound::PlaySE( SEQ_SE_SLIDE );
  }

  //
  // 決定ボタンでジャンプ実行
  //
  DebugMapJumpImpl( userWork, item );

}

/**
 *    @brief マップジャンプデバッグ機能　Draw
 */
static wchar_t* DrawDebugMapJump( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), pThis->GetSelectZoneName() );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), pThis->GetSelectZoneName() ); //printfの%S書式がarmには無いので警告#226対策
#endif
  return workStr;

}

/**
 *    @brief マップジャンプデバッグ機能　Draw
 */
static wchar_t* DrawDebugMapJumpZoneFilter( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), 
    pThis->GetZoneName( s_ZoneIndexForFilter )
    );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), 
    pThis->GetZoneName( s_ZoneIndexForFilter )
    );
#endif
  return workStr;

}

/**
*    @brief マップジャンプデバッグ機能　Draw 地名表示
*/
static wchar_t* DrawPlacementName( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls(%ls%ls)", item->GetName(), 
    pThis->GetZonePlaceName( s_ZoneIndexForFilter ),
    pThis->GetZonePlaceNameFollow( s_ZoneIndexForFilter )
    );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls(%ls%ls)", item->GetName(), 
    pThis->GetZonePlaceName( s_ZoneIndexForFilter ),
    pThis->GetZonePlaceNameFollow( s_ZoneIndexForFilter )
    );
#endif
  return workStr;
}


/**
 *    @brief マップジャンプデバッグ機能　Update
 */
static void UpdateZoneFilter(void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);

  // Aボタンが押されたらマップジャンプ
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  s32 add_num = 1;
  s32 oldIndex = s_ZoneFilterId;
  s32 index = oldIndex;


  if( cross->IsRepeat( gfl2::ui::DIR_LEFT ) ){
    index -= add_num;
  }else if( cross->IsRepeat( gfl2::ui::DIR_RIGHT ) ){
    index += add_num;
  }
  if( index < 0 ){ index += ZONE_FILETER_MAX; }
  if( index >= ZONE_FILETER_MAX ){ index -= ZONE_FILETER_MAX; }
  
  s_ZoneFilterId = static_cast<u32>(index);

  if( index != oldIndex )
  {
    // 変更音
    Sound::PlaySE( SEQ_SE_SLIDE );
    pThis->SetZoneIndexForFilterTop();
  }

  //
  // 決定ボタンでジャンプ実行
  //
  DebugMapJumpImpl( userWork, item );

}




/**
 *    @brief マップジャンプデバッグ機能　Draw
 */
static wchar_t* DrawZoneFilter( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), sc_ZONE_JUMP_GROUP[ s_ZoneFilterId ].preFix );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), sc_ZONE_JUMP_GROUP[ s_ZoneFilterId ].preFix ); //printfの%S書式がarmには無いので警告#226対策
#endif
  return workStr;
}




/**
 *    @brief マップジャンプデバッグ機能　Draw
 */
static wchar_t* DrawDebugMapJumpForFilter( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), pThis->GetZoneName( s_ZoneIndexForFilter ) );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), pThis->GetZoneName( s_ZoneIndexForFilter ) ); //printfの%S書式がarmには無いので警告#226対策
#endif
  return workStr;

}

/**
 *    @brief 今のゾーン表示
 */
static wchar_t* DrawDebugNowZone( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), pThis->GetZoneName( pThis->GetNowZoneID() ) );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), pThis->GetZoneName( pThis->GetNowZoneID() ) ); //printfの%S書式がarmには無いので警告#226対策
#endif
  return workStr;

}


/**
 *    @brief Auto全マップジャンプ
 */
static void UpdateDebugAutoAllMapJump(void* userWork, gfl2::debug::DebugWinItem* item )
{
  //DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);

  // Aボタンが押されたらマップジャンプ
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  //const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    {
      FieldScript::FieldScriptSystem* script = pGameManager->GetFieldScriptSystem();
      if( script->IsRunning()){
        GFL_PRINT( "スクリプトイベント実行中はデバッグマップジャンプできません\n" );
        return;
      }
    }
    DebugEventAutoAllMapJump * p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, DebugEventAutoAllMapJump>( pGameManager->GetGameEventManager() );
    DebugEventAutoAllMapJump::InitializeDescription initDesc;
    p_event->SetDesc( initDesc );

    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }

}

static void UpdateDebugAutoAllMapJumpHeapInfoSave(void* userWork, gfl2::debug::DebugWinItem* item )
{
  //DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);

  // Aボタンが押されたらマップジャンプ
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  //const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    {
      FieldScript::FieldScriptSystem* script = pGameManager->GetFieldScriptSystem();
      if( script->IsRunning()){
        GFL_PRINT( "スクリプトイベント実行中はデバッグマップジャンプできません\n" );
        return;
      }
    }
    DebugEventAutoAllMapJump * p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, DebugEventAutoAllMapJump>( pGameManager->GetGameEventManager() );
    DebugEventAutoAllMapJump::InitializeDescription initDesc;
    initDesc.m_bBCancel = true;
    initDesc.m_bHeapInfoSave = true;
    p_event->SetDesc( initDesc );

    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}


/**
 *    @brief Auto全マップジャンプ(現在ワールド)
 */
static void UpdateDebugAutoAllMapJumpOnlyNowWorld(void* userWork, gfl2::debug::DebugWinItem* item )
{
  //DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);

  // Aボタンが押されたらマップジャンプ
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  //const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    {
      FieldScript::FieldScriptSystem* script = pGameManager->GetFieldScriptSystem();
      if( script->IsRunning()){
        GFL_PRINT( "スクリプトイベント実行中はデバッグマップジャンプできません\n" );
        return;
      }
    }
    Fieldmap* pFieldmap = pGameManager->GetFieldmap();
    
    DebugEventAutoAllMapJump * p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, DebugEventAutoAllMapJump>( pGameManager->GetGameEventManager() );
    DebugEventAutoAllMapJump::InitializeDescription initDesc;

    initDesc.m_Mode = DebugEventAutoAllMapJump::MODE_ONLY_WORLD;
    initDesc.m_WorldID = pFieldmap->GetWorldID();

    p_event->SetDesc( initDesc );

    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }

}

/**
 *    @brief AutoRANDOMマップジャンプ
 */
static void UpdateDebugAutoRandomMapJump(void* userWork, gfl2::debug::DebugWinItem* item )
{
  //DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);

  // Aボタンが押されたらマップジャンプ
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  //const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    {
      FieldScript::FieldScriptSystem* script = pGameManager->GetFieldScriptSystem();
      if( script->IsRunning()){
        GFL_PRINT( "スクリプトイベント実行中はデバッグマップジャンプできません\n" );
        return;
      }
    }
    DebugEventAutoAllMapJump * p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, DebugEventAutoAllMapJump>( pGameManager->GetGameEventManager() );
    DebugEventAutoAllMapJump::InitializeDescription initDesc;
    initDesc.m_Mode = DebugEventAutoAllMapJump::MODE_RANDOM;
    initDesc.m_bBCancel = true;
    p_event->SetDesc( initDesc );

    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }

}

/**
 *    @brief AutoRANDOM アプリ復帰繰り返し　マップジャンプ
 */
static void UpdateDebugAutoRandomAppReturnMapJump(void* userWork, gfl2::debug::DebugWinItem* item )
{
  //DebugMapJump * pThis = reinterpret_cast<DebugMapJump*>(userWork);

  // Aボタンが押されたらマップジャンプ
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  //const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    {
      FieldScript::FieldScriptSystem* script = pGameManager->GetFieldScriptSystem();
      if( script->IsRunning()){
        GFL_PRINT( "スクリプトイベント実行中はデバッグマップジャンプできません\n" );
        return;
      }
    }
    DebugEventAutoAllMapJump * p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, DebugEventAutoAllMapJump>( pGameManager->GetGameEventManager() );
    DebugEventAutoAllMapJump::InitializeDescription initDesc;
    initDesc.m_Mode = DebugEventAutoAllMapJump::MODE_APP_RETURN;
    initDesc.m_bBCancel = true;
    p_event->SetDesc( initDesc );

    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }

}

static void StartSetUp( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugMapJump *pDebug = reinterpret_cast<DebugMapJump*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );

  // 選択ゾーンを現在のゾーンに変更
  s_ZoneIndexForFilter = pDebug->GetNowZoneID();

  // 最新ゾーンが以前のフィルターに含まれるものなら、フィルターを維持
  if( pDebug->IsFilterHit( s_ZoneFilterId, pDebug->GetNowZoneID() ) )
  {
  }
  // 含まれないならALLフィルターに変更
  else
  {
    s_ZoneFilterId = 0;
  }
}


/**
 *  @brief  Group ctor
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugMapJump *pDebug = reinterpret_cast<DebugMapJump*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();

    // マップジャンプ
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, L"Filter", pDebug, UpdateZoneFilter, DrawZoneFilter );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, L"Jump", pDebug, UpdateDebugMapJumpZoneFilter, DrawDebugMapJumpZoneFilter);
    gfl2::debug::DebugWinFunc* pFunc = gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, L"-", pDebug, DebugMapJumpImpl, DrawPlacementName );
    if( pFunc )
    {
      pFunc->SetIsSelectSkip( true );
    }

    // 今のゾーン
    pFunc = gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_map_jump_now_zone, pDebug, NULL, DrawDebugNowZone );
    if( pFunc )
    {
      pFunc->SetIsSelectSkip( true );
    }

    // 自動全マップジャンプ
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_map_jump_all_jump, pDebug, UpdateDebugAutoAllMapJump, NULL );
    // 自動全マップジャンプ(現在ワールド)
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_map_jump_all_jump_now, pDebug, UpdateDebugAutoAllMapJumpOnlyNowWorld, NULL );
    // 自動RANDOMマップジャンプ
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_map_jump_random_jump, pDebug, UpdateDebugAutoRandomMapJump, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_map_jump_random_appreturn, pDebug, UpdateDebugAutoRandomAppReturnMapJump, NULL );
    // 自動全マップジャンプ(HeapInfoSave)
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, L"AutoAllMapJump(HeapInfoSave)", pDebug, UpdateDebugAutoAllMapJumpHeapInfoSave, NULL );

  }


  StartSetUp( userWork, item );

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
 *  @brief  WindowOpen
 */
static void WindowOpenGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  StartSetUp( userWork, item );
}





//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
DebugMapJump::DebugMapJump() : 
  m_pGroup( NULL ),
  m_pZoneName(NULL),
  m_ZoneFilterId( 0 ),
  m_ZoneIndexForFilter( 0 ),
  m_pStrBuffPlaceName( NULL ),
  m_pStrBuffPlaceNameFollow( NULL )
{
}
DebugMapJump::~DebugMapJump()
{
  Terminate();
}

/**
 *  @brief 初期化
 *  @param  pHeap ヒープ
 */
void DebugMapJump::Initialize( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 zoneID, Fieldmap* pFieldmap, gfl2::str::MsgData* pMsgData )

{
  m_pHeap = pHeap;
  m_ZoneIndex = zoneID;
  m_pFieldmap = pFieldmap;
  // メッセージデータ取得
  m_pMsgData = pMsgData;

  m_pStrBuffPlaceName = GFL_NEW( pHeap ) gfl2::str::StrBuf( 64, pHeap );
  m_pStrBuffPlaceNameFollow = GFL_NEW( pHeap ) gfl2::str::StrBuf( 64, pHeap );

  //---------------------------------------------------
  // ARCでモデルデータ読み込み
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_ZONE_DATA_D;
    openReq.heapForFile = pHeap->GetLowerHandle();
    openReq.heapForArcSrc = pHeap->GetLowerHandle();
    openReq.heapForReq = pHeap->GetLowerHandle();
    pAsyncFileReadManager->SyncArcFileOpen( openReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARCID_ZONE_DATA_D;
    loadReq.datId = 0;
    loadReq.ppBuf = reinterpret_cast<void**>(&m_pZoneName);
    loadReq.heapForBuf = pHeap;
    loadReq.pBufSize = &m_ZoneNameDataSize;
    loadReq.align = 4;
    pAsyncFileReadManager->SyncArcFileLoadData( loadReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_ZONE_DATA_D;
    closeReq.heapForReq = pHeap;
    pAsyncFileReadManager->SyncArcFileClose( closeReq );
  }

  // ZoneNameの最大値を求める。
  m_ZoneMax = m_ZoneNameDataSize / ONE_ZONE_CHAR_NUM;

}

/**
 *  @brief  破棄
 */
void DebugMapJump::Terminate()
{
  GFL_SAFE_DELETE( m_pStrBuffPlaceNameFollow );
  GFL_SAFE_DELETE( m_pStrBuffPlaceName );
  GflHeapSafeFreeMemory(m_pZoneName);

  if( m_pGroup )
  {
    gfl2::debug::DebugWin_RemoveGroup( m_pGroup );
    m_pGroup = NULL;
  }
}

/**
 *  @brief  ゾーン名取得
 */
const char* DebugMapJump::GetZoneName( u32 index ) const
{
  if( index < m_ZoneMax )
  {
    return &m_pZoneName[index * ONE_ZONE_CHAR_NUM];
  }
  GFL_ASSERT(index < m_ZoneMax);
  return &m_pZoneName[0];
}

const gfl2::str::STRCODE* DebugMapJump::GetZonePlaceName( u32 index )
{
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::ZoneDataLoader *pLoader = pGameManager->GetGameData()->GetFieldZoneDataLoader();
  void *zoneData = pLoader->GetZoneData( index );
  if( !zoneData ){ return NULL; }
  Field::ZoneDataAccessor accessor( zoneData );
  u16 placeStrID = accessor.GetPlaceLabelID();
  gfl2::str::MsgData *pMsgData = pGameManager->GetFieldResident()->GetWordSetLoader()->GetMsgData( GARC_message_place_name_DAT );
  pMsgData->GetString( placeStrID, *m_pStrBuffPlaceName );
  return m_pStrBuffPlaceName->GetPtr();
}

const gfl2::str::STRCODE* DebugMapJump::GetZonePlaceNameFollow( u32 index )
{
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::ZoneDataLoader *pLoader = pGameManager->GetGameData()->GetFieldZoneDataLoader();
  void *zoneData = pLoader->GetZoneData( index );
  if( !zoneData ){ return NULL; }
  Field::ZoneDataAccessor accessor( zoneData );
  u16 placeStrID = accessor.GetPlaceLabelID();
  gfl2::str::MsgData *pMsgData = pGameManager->GetFieldResident()->GetWordSetLoader()->GetMsgData( GARC_message_place_name_DAT );
  pMsgData->GetString( placeStrID + 1, *m_pStrBuffPlaceNameFollow );
  return m_pStrBuffPlaceNameFollow->GetPtr();
}

/**
 *  @brief デバッグメニューの構築
 */
void DebugMapJump::CreateDebugMenuGroup( gfl2::debug::DebugWinGroup* root )
{
  // マップジャンプに関するグループを追加
  {
    m_pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_map_jump_top, root );  
    m_pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
    m_pGroup->SetWindowOpenCloseGroupFunc( this, WindowOpenGroupFunc, NULL );
  }
}

/**
 *  @brief  今のゾーンID取得
 */
u32 DebugMapJump::GetNowZoneID()
{
  return m_pFieldmap->GetZoneID();
}

void DebugMapJump::SetZoneIndexForFilterTop( u32 startZone, s32 add, b32 firstSkip )
{
  s32 checkZone = startZone;
  const c8* checkPreFix = sc_ZONE_JUMP_GROUP[ s_ZoneFilterId ].preFix;
  u32 checkLen = strlen( sc_ZONE_JUMP_GROUP[ s_ZoneFilterId ].preFix );
  b32 hitFlag = false;

  s32 count = gfl2::math::Abs( add );
  if( add > 0 )
  {
    add = 1;
  }
  else
  {
    add = -1;
  }


  while( 1 )
  {
    if( firstSkip )
    {
      firstSkip = false;
    }
    else
    {
      if( strncmp( checkPreFix, GetZoneName( checkZone ), checkLen ) == 0 )
      {
        hitFlag = true;

        count--;
        if( count <= 0 )
        {
          s_ZoneIndexForFilter = checkZone;
          break;
        }
      }
    }

    checkZone += add;

    if( checkZone >= static_cast<s32>( GetZoneMax() ))
    {
      checkZone -= GetZoneMax();
    }
    if( checkZone < 0 )
    {
      checkZone += GetZoneMax();
    }

    if( checkZone == startZone && hitFlag == false )
    {
      break;
    }
  }
}

s32 DebugMapJump::GetZoneSkipNext( s32 startZone, const char* pFilter, s32 dir )
{
  u32 filterLen = strlen( pFilter );
  s32 checkZone = startZone;
  const c8* pNowZoneName = GetZoneName( startZone );
  u32 checkLen = filterLen + GetPreFixLength( &pNowZoneName[ filterLen ] );

  while( 1 )
  {
    checkZone += dir;

    if( checkZone >= (s32)GetZoneMax() )
    {
      checkZone -= (s32)GetZoneMax();
    }
    if( checkZone < 0 )
    {
      checkZone += (s32)GetZoneMax();
    }

    // 一蹴してしまった
    if( checkZone == startZone )
    {
      return startZone;
    }

    // チェックじゃ
    const c8* pCheckZoneName = GetZoneName( checkZone );
    
    if( strncmp( pFilter, pCheckZoneName, filterLen ) == 0 && strncmp( pNowZoneName, pCheckZoneName, checkLen ) != 0 )
    {
      return checkZone;
    }
  }

}


// 指定ゾーンが指定フィルターでヒットするか取得
b32 DebugMapJump::IsFilterHit( u32 filterIndex, s32 zoneId ) const
{
  const c8* checkPreFix = sc_ZONE_JUMP_GROUP[ filterIndex ].preFix;
  u32 checkLen = strlen( sc_ZONE_JUMP_GROUP[ filterIndex ].preFix );

  if( strncmp( checkPreFix, GetZoneName( zoneId ), checkLen ) == 0 )
  {
    return true;
  }
  else
  {
    return false;
  }
}


GFL_NAMESPACE_END(Debug);
GFL_NAMESPACE_END(Field);

#endif // PM_DEBUG
