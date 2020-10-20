//======================================================================
/**
 * @file FieldDebugDispInfo.cpp
 * @date 2015/09/08 14:55:22
 * @author saita_kazuki
 * @brief デバッグ：DispInfo 情報表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "Field/FieldRo/include/Debug/FieldDebugDispInfo.h"

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
// GameSys
#include "GameSys/include/GameManager.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/FieldAttribute.h"
#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficManager.h"

// conv_header
#include <niji_conv_header/message/debug/msg_debug_menu.h>
#include <niji_conv_header/field/zone/zone_id.h>
#include <niji_conv_header/field/attribute/attribute_id.h>
#include <niji_conv_header/battle/background/bgsys_near_type_def.h>
#include <niji_conv_header/battle/background/bgsys_far_type_def.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/debug_attribute.gaix>

#include "Field/FieldStatic/include/Script/ScriptFuncSetFieldInit.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

//------------------------------------------------------------------------------
// STATIC
//------------------------------------------------------------------------------

/**
 *    @brief プレイヤー座標の表示
 */
static wchar_t* DrawDebugWritePlayerPos( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  MoveModel::FieldMoveModel* pChara = pGameManager->GetFieldmap()->GetPlayerCharacter();
  gfl2::math::Vector3 pos = pChara->GetLocalSRT().translate;
  
  /// クォータニオンから角を求める.
  gfl2::math::Quaternion  cQuat = pChara->GetLocalSRT().rotation;
  f32 fAngle = gfl2::math::ConvRadToDeg(FieldScript::FuncSetFieldInit::ConvRotationQuaternionToYRadian(cQuat));


  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%.3f, %.3f, %.3f][%.3f]", item->GetName(), pos.GetX(), pos.GetY(), pos.GetZ(), fAngle );
  return workStr;
}

/**
 *    @brief プレイヤー速度の表示
 */
static wchar_t* DrawDebugWritePlayerSpeed( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  MoveModel::FieldMoveModel* pChara = pGameManager->GetFieldmap()->GetPlayerCharacter();
  gfl2::math::Vector3 posStart = pChara->GetPositionPrevUpdateStart();
  gfl2::math::Vector3 posEnd   = pChara->GetPositionPrevUpdateEnd();
  gfl2::math::Vector3 def      = posEnd - posStart;
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%.3f]", item->GetName(), def.Length() );
  return workStr;
}

/**
 *    @brief プレイヤー速度の表示
 */
static wchar_t* DrawDebugWritePlayerAnimationId( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  MoveModel::FieldMoveModel* pChara = pGameManager->GetFieldmap()->GetPlayerCharacter();
  if( pChara && pChara->GetCharaDrawInstance() )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pChara->GetCharaDrawInstance()->GetAnimationId() );
  }
  else
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );
  }
  return workStr;
}

/**
 *    @brief プレイヤー速度の表示
 */
static wchar_t* DrawDebugWritePlayerAnimationFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  MoveModel::FieldMoveModel* pChara = pGameManager->GetFieldmap()->GetPlayerCharacter();
  if( pChara && pChara->GetCharaDrawInstance() )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%.2f]", item->GetName(), pChara->GetCharaDrawInstance()->GetAnimationFrame() );
  }
  else
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );
  }
  return workStr;
}

/**
 *    @brief 現在のブロック位置
 */
static wchar_t* DrawNowBlockIndex( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
  if( pFieldmap == NULL )
  {
    return NULL;
  }

  // 自機座標を取得
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pFieldmap->GetMoveModelManager();
  Field::MoveModel::FieldMoveModel* pPlayerCharaModel = pFieldMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
  gfl2::math::Vector3 playerPos;
  if( pPlayerCharaModel == NULL )
  {
    return NULL;
  }

  playerPos = pPlayerCharaModel->GetCharaDrawInstance()->GetPosition();


  // 自機座標から地形ブロック番号を取得
  s32 x, z;
  Field::Terrain::TerrainManager* pTerrainManager = pFieldmap->GetTerrainManager();
  if( pTerrainManager->GetBlockIndex( playerPos, &x, &z ) == false )
  {
    return NULL;
  }

  wchar_t* pWorkStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( pWorkStr, gfl2::debug::DEBUGWIN_NAME_LEN ,L"%ls[%d, %d]", item->GetName(), x, z );
  return pWorkStr;
}



/**
 *  @brief  現在のゾーンID表示
 */
wchar_t* DrawFunc_NowZoneID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), pThis->GetZoneName( pThis->GetNowZoneID() ) );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), pThis->GetZoneName( pThis->GetNowZoneID() ) ); //printfの%S書式がarmには無いので警告#226対策
#endif

  return workStr;
}

/**
 *  @brief  現在のアトリビュートID表示
 */
wchar_t* DrawFunc_NowAttributeID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;

  u32 attr = pThis->GetFieldmap()->GetPlayerCharacter()->GetGroundAttribute();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), pThis->GetAttributeName( attr ) );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), pThis->GetAttributeName( attr ) ); //printfの%S書式がarmには無いので警告#226対策
#endif

  return workStr;
}

/**
 *  @brief  一番最後に有効だったアトリビュートID表示
 */
wchar_t* DrawFunc_LastSuccessfulAttributeID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;

  u32 attr = pThis->GetFieldmap()->GetPlayerCharacter()->GetGroundAttributeLastSuccesful();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), pThis->GetAttributeName( attr ) );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), pThis->GetAttributeName( attr ) ); //printfの%S書式がarmには無いので警告#226対策
#endif

  return workStr;
}

/**
 *  @brief  現在のアトリビュートパターンID表示
 */
wchar_t* DrawFunc_NowAttributePatternID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;

  Field::ZoneDataAccessor accessor( pThis->GetZoneDataLoader()->GetZoneData( pThis->GetFieldmap()->GetZoneID()));
  u16 attrPattern = accessor.GetAttributeBgNearPattern();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), pThis->GetAttributePatternName( attrPattern ) );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), pThis->GetAttributePatternName( attrPattern ) ); //printfの%S書式がarmには無いので警告#226対策
#endif

  return workStr;
}

/**
 *  @brief  現在のバトル背景近景ID表示
 */
wchar_t* DrawFunc_NowBattleNearBackgroundID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;

  u32 attr = pThis->GetFieldmap()->GetPlayerCharacter()->GetGroundAttribute();

  Field::ZoneDataAccessor accessor( pThis->GetZoneDataLoader()->GetZoneData( pThis->GetFieldmap()->GetZoneID()));
  u16 attrPattern = accessor.GetAttributeBgNearPattern();

  u8 nearID = Attribute::GetBattleNearBGID( attr, attrPattern);

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), pThis->GetNearBgName( nearID ) );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), pThis->GetNearBgName( nearID ) ); //printfの%S書式がarmには無いので警告#226対策
#endif

  return workStr;
}

/**
 *  @brief  現在のバトル背景遠景ID表示
 */
wchar_t* DrawFunc_NowBattleFarBackgroundID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;

  Field::ZoneDataAccessor accessor( pThis->GetZoneDataLoader()->GetZoneData( pThis->GetFieldmap()->GetZoneID()));
  u16 farID = accessor.GetBattleFarBGID();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), pThis->GetFarBgName( farID ) );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), pThis->GetFarBgName( farID ) ); //printfの%S書式がarmには無いので警告#226対策
#endif

  return workStr;
}

/**
 *  @brief  現在の脱出先ロケーションの脱出先ゾーンID表示
 */
wchar_t* DrawFunc_EscapeZoneID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;
  const Field::Location* pLocation = pThis->GetFieldmap()->GetGameManager()->GetGameData()->GetEscapeLocation();

  // @note デバッグ開始すると最初は正しい値が入っていない
  if( pLocation->zoneId == ZONE_ID_INVALID )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[error]", item->GetName() );
  }
  else
  {
#if defined(GF_PLATFORM_WIN32)
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), pThis->GetZoneName( pLocation->zoneId ) );
#elif defined(GF_PLATFORM_CTR)
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]", item->GetName(), pThis->GetZoneName( pLocation->zoneId ) ); //printfの%S書式がarmには無いので警告#226対策
#endif
  }

  return workStr;
}

/**
 *  @brief  現在の脱出先ロケーションの脱出先座標表示
 */
wchar_t* DrawFunc_EscapePosition( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;
  const Field::Location* pLocation = pThis->GetFieldmap()->GetGameManager()->GetGameData()->GetEscapeLocation();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %f, %f, %f ]", item->GetName(), pLocation->position.x, pLocation->position.y, pLocation->position.z );

  return workStr;
}

/**
 *  @brief  現在の脱出先ロケーションのライドID表示
 */
wchar_t* DrawFunc_EscapeRideID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* pStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;
  const Field::Location* pLocation = pThis->GetFieldmap()->GetGameManager()->GetGameData()->GetEscapeLocation();
  gfl2::str::MsgData* pMsgData = pThis->GetMessageData();

  if( pLocation->formNo == RIDE_NO_POKEMON)
  {
    pMsgData->GetString( msg_dmenu_field_disp_info_escape_ride_none, *pStrBuf);
  }
  else
  {
    pMsgData->GetString( msg_dmenu_field_disp_info_escape_ride_id_0 + pLocation->formNo, *pStrBuf);
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), pStrBuf->GetPtr() );

  return workStr;
}


/**
*  @brief  現在ロードされている動作モデルの種類数を表示します
*/
wchar_t* DrawNowMoveModelTypeCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* pStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;
  s32 count = pThis->GetFieldmap()->GetMoveModelManager()->GetCountOfModelTypes();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ld]", item->GetName(), count );
  return workStr;
}

/**
*  @brief  現在存在する動作モデルの数を表示します
*/
wchar_t* DrawNowMoveModelDataCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* pStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;
  s32 count = pThis->GetFieldmap()->GetMoveModelManager()->GetCountOfSetupCompleteModel();
  s32 max = Field::MoveModel::FIELD_MOVE_MODEL_MAX;

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ld/%ld]", item->GetName(), count, max );
  return workStr;
}

/**
*  @brief  現在描画している動作モデルの数を表示します
*/
wchar_t* DrawNowMoveModelDrawingCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* pStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  DebugDispInfo* pThis = (DebugDispInfo*)userWork;
  s32 count = pThis->GetFieldmap()->GetMoveModelManager()->GetCountOfDrawingModel();
  s32 max = pThis->GetFieldmap()->GetMoveModelManager()->GetCountOfDispMax();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ld/%ld]", item->GetName(), count, max );
  return workStr;
}

/**
*  @brief  数字を表示します
*/
wchar_t* DrawInt( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* pStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  s32 value = *(s32*)userWork;
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ld]", item->GetName(), value );
  return workStr;
}


struct DRAW_INT_MAX
{
  s32 value;
  s32 max;
};

struct DRAW_INT_MAX_ALIAS
{
  s32 value;
  s32 max;
  s32 alias;
};


wchar_t* DrawIntMax( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* pStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  DRAW_INT_MAX* pData = (DRAW_INT_MAX*)userWork;
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ld/%ld]", item->GetName(), pData->value, pData->max );
  return workStr;
}

wchar_t* DrawIntMaxAlias( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* pStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  DRAW_INT_MAX_ALIAS* pData = (DRAW_INT_MAX_ALIAS*)userWork;
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ld/%ld](%ld)", item->GetName(), pData->value, pData->max, pData->alias );
  return workStr;
}


static void CreatePlacementDataGroup( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugDispInfo *pDebug = reinterpret_cast<DebugDispInfo*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();

    Field::TrafficManager* pTrafficManager = pDebug->GetFieldmap()->GetTrafficManager();
    Field::PlacementDataManager* pPlacementDataManager =  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident()->GetPlacementDataManager();
    //pDebug->GetFieldmap()->GetResident()->GetPlacementDataManager();

    static DRAW_INT_MAX_ALIAS moveModelInfo;
    moveModelInfo.value = pPlacementDataManager->GetMoveModelActorCount();
    moveModelInfo.max = pPlacementDataManager->GetMoveModelActorMax();
    moveModelInfo.alias = pPlacementDataManager->GetMoveModelActorAlias();

    static DRAW_INT_MAX_ALIAS trainerInfo;
    trainerInfo.value = pPlacementDataManager->GetTrainerActorCount();
    trainerInfo.max = pPlacementDataManager->GetTrainerActorMax();
    trainerInfo.alias = pPlacementDataManager->GetTrainerActorAlias();

    static DRAW_INT_MAX trafficInfo;
    trafficInfo.value = pTrafficManager->GetPathCount();
    trafficInfo.max = pTrafficManager->GetPathMax();

    static DRAW_INT_MAX gimmickEncoutInfo;
    gimmickEncoutInfo.value = pPlacementDataManager->GetGimmickEncountActorCount();
    gimmickEncoutInfo.max = pPlacementDataManager->GetGimmickEncountActorMax();

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"データ名[配置数/制限数](Alias数)" , NULL, NULL, NULL )->SetIsSelectSkip( true );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"配置NPC" , &moveModelInfo, NULL, DrawIntMaxAlias );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"視線トレーナー" , &trainerInfo, NULL, DrawIntMaxAlias );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"往来パス" , &trafficInfo, NULL, DrawIntMax );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"ギミックエンカウント" , &gimmickEncoutInfo, NULL, DrawIntMax );
  }
}

static void DestroyPlacementDataGroup( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

static void CreateMoveModelGroup( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugDispInfo *pDebug = reinterpret_cast<DebugDispInfo*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();

    Field::TrafficManager* pTrafficManager = pDebug->GetFieldmap()->GetTrafficManager();
    Field::PlacementDataManager* pPlacementDataManager =  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident()->GetPlacementDataManager();
    //pDebug->GetFieldmap()->GetResident()->GetPlacementDataManager();
    Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pDebug->GetFieldmap()->GetMoveModelManager();

    static s32 areaNum = pFieldMoveModelManager->GetCountOfModelTypes();
    static DRAW_INT_MAX instanceInfo;
    instanceInfo.value = pFieldMoveModelManager->GetCountOfSetupCompleteModel();
    instanceInfo.max = Field::MoveModel::FIELD_MOVE_MODEL_MAX;
    
    static DRAW_INT_MAX instancePlacementInfo;
    instancePlacementInfo.value = pFieldMoveModelManager->GetCountOfSetupCompletePlacementDataModel();
    instancePlacementInfo.max = pFieldMoveModelManager->GetCountOfPlacementDataModelMax();

    static DRAW_INT_MAX drawInfo;
    drawInfo.value = pFieldMoveModelManager->GetCountOfDrawableCullingTragetModel();
    drawInfo.max = pFieldMoveModelManager->GetCountOfDispMax();

    // drawable

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"データ名[数/制限数]" , NULL, NULL, NULL )->SetIsSelectSkip( true );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"エリア登録タイプ数" , &areaNum, NULL, DrawInt);
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"実体数" , &instanceInfo, NULL, DrawIntMax );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"実体数(配置データ枠)", &instancePlacementInfo, NULL, DrawIntMax );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"カリング対象実体数" , &drawInfo, NULL, DrawIntMax );
  }
}

static void DestroyMoveModelGroup( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}



/**
*  @brief  Group ctor
*/
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugDispInfo *pDebug = reinterpret_cast<DebugDispInfo*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( pHeap, L"配置データ情報", pGroup );
    pGropWork->SetCreateDestroyGroupFunc( pDebug, CreatePlacementDataGroup, DestroyPlacementDataGroup );
    pGropWork = gfl2::debug::DebugWin_AddGroup( pHeap, L"動作モデル情報", pGroup );
    pGropWork->SetCreateDestroyGroupFunc( pDebug, CreateMoveModelGroup, DestroyMoveModelGroup );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_player_pos, pDebug, NULL, DrawDebugWritePlayerPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"PlayerSpeed"          , pDebug, NULL, DrawDebugWritePlayerSpeed );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"PlayerAnimatioId"     , pDebug, NULL, DrawDebugWritePlayerAnimationId );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"PlayerAnimationFrame" , pDebug, NULL, DrawDebugWritePlayerAnimationFrame );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, L"BlockIndex(x, z)"     , pDebug, NULL, DrawNowBlockIndex );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_disp_info_zone_id, pDebug, NULL, DrawFunc_NowZoneID );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_disp_info_attr_id, pDebug, NULL, DrawFunc_NowAttributeID );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_disp_info_attr_id_last, pDebug, NULL, DrawFunc_LastSuccessfulAttributeID );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_disp_info_attr_pattern_id, pDebug, NULL, DrawFunc_NowAttributePatternID );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_disp_info_near_bg_id, pDebug, NULL, DrawFunc_NowBattleNearBackgroundID );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_disp_info_far_bg_id, pDebug, NULL, DrawFunc_NowBattleFarBackgroundID );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_disp_info_escape_zone_id, pDebug, NULL, DrawFunc_EscapeZoneID );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_disp_info_escape_pos, pDebug, NULL, DrawFunc_EscapePosition );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_disp_info_escape_ride, pDebug, NULL, DrawFunc_EscapeRideID );

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

//------------------------------------------------------------------------------
// DebugDispInfo
//------------------------------------------------------------------------------

/**
 * @class DebugDispInfo
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
DebugDispInfo::DebugDispInfo( void )
  : m_pHeap( NULL)
  , m_pFieldmap( NULL)
  , m_pZoneDataLoader( NULL)
  , m_pMsgData( NULL)
  , m_pZoneName( NULL)
  , m_pAttrName( NULL)
  , m_pAttrPatternName( NULL)
  , m_pNearBgName( NULL)
  , m_pFarBgName( NULL)
{
}

/**
 *  @brief  デストラクタ
 */
DebugDispInfo::~DebugDispInfo( void )
{
  Terminate();
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

/**
 *  @brief  初期化
 */
void DebugDispInfo::Initialize(
  gfl2::heap::HeapBase *pHeap,
  gfl2::fs::AsyncFileManager* pAsyncFileReadManager,
  Fieldmap* pFieldmap,
  ZoneDataLoader* pZoneDataLoader,
  gfl2::str::MsgData* pMsgData
  )
{
  m_pHeap = pHeap;
  m_pFieldmap = pFieldmap;
  m_pMsgData = pMsgData;
  m_pZoneDataLoader = pZoneDataLoader;

  this->SyncArcOpen( m_pHeap->GetLowerHandle(), pAsyncFileReadManager, ARCID_ZONE_DATA_D);
  this->SyncArcLoad( m_pHeap, pAsyncFileReadManager, ARCID_ZONE_DATA_D, 0, reinterpret_cast<void**>( &m_pZoneName), NULL);
  this->SyncArcClose( m_pHeap, pAsyncFileReadManager, ARCID_ZONE_DATA_D);

  this->SyncArcOpen( m_pHeap->GetLowerHandle(), pAsyncFileReadManager, ARCID_ATTRIBUTE_D);
  this->SyncArcLoad( m_pHeap, pAsyncFileReadManager, ARCID_ATTRIBUTE_D, GARC_debug_attribute_debug_attribute_name_BIN, reinterpret_cast<void**>( &m_pAttrName), NULL);
  u32 attrPatternSize = 0;
  this->SyncArcLoad( m_pHeap, pAsyncFileReadManager, ARCID_ATTRIBUTE_D, GARC_debug_attribute_debug_attribute_pattern_name_BIN, reinterpret_cast<void**>( &m_pAttrPatternName), &attrPatternSize);
  this->SyncArcLoad( m_pHeap, pAsyncFileReadManager, ARCID_ATTRIBUTE_D, GARC_debug_attribute_debug_near_bg_name_BIN, reinterpret_cast<void**>( &m_pNearBgName), NULL);
  this->SyncArcLoad( m_pHeap, pAsyncFileReadManager, ARCID_ATTRIBUTE_D, GARC_debug_attribute_debug_far_bg_name_BIN, reinterpret_cast<void**>( &m_pFarBgName), NULL);
  this->SyncArcClose( m_pHeap, pAsyncFileReadManager, ARCID_ATTRIBUTE_D);

  m_attrPatternMax = (attrPatternSize / ONE_ATTR_PATTERN_CHAR_NUM) - 1; // 末尾にERRORを入れているのでマイナス1
}

/**
 *  @brief  破棄
 */
void DebugDispInfo::Terminate( void )
{
  GflHeapSafeFreeMemory(m_pZoneName);
  GflHeapSafeFreeMemory(m_pAttrName);
  GflHeapSafeFreeMemory(m_pAttrPatternName);
  GflHeapSafeFreeMemory(m_pNearBgName);
  GflHeapSafeFreeMemory(m_pFarBgName);
}

/**
 *  @brief  メニューの作成
 */
void DebugDispInfo::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_disp_info_title, root );
  pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

/**
 *  @brief  ゾーン名取得
 */
char* DebugDispInfo::GetZoneName( u32 index )
{
  if( index < ZONE_ID_MAX )
  {
    return &m_pZoneName[index * ONE_ZONE_CHAR_NUM];
  }
  GFL_ASSERT(index < ZONE_ID_MAX);
  return &m_pZoneName[0];
}

/**
 *  @brief  アトリビュート名取得
 */
char* DebugDispInfo::GetAttributeName( u32 index )
{
  if( index < ATTRIBUTE_MAX )
  {
    return &m_pAttrName[index * ONE_ATTR_CHAR_NUM];
  }
  return &m_pAttrName[ATTRIBUTE_MAX * ONE_ATTR_CHAR_NUM]; // ERRORが格納されている
}

/**
 *  @brief  アトリビュートパターン名取得
 */
char* DebugDispInfo::GetAttributePatternName( u32 index )
{
  if( index < m_attrPatternMax )
  {
    return &m_pAttrPatternName[index * ONE_ATTR_PATTERN_CHAR_NUM];
  }
  return &m_pAttrPatternName[m_attrPatternMax * ONE_ATTR_PATTERN_CHAR_NUM]; // ERRORが格納されている
}

/**
 *  @brief  近景ID名取得
 */
char* DebugDispInfo::GetNearBgName( u32 index )
{
  if( index < btl::bg::BGSYS_NEAR_TYPE_MAX )
  {
    return &m_pNearBgName[index * ONE_NEAR_BG_CHAR_NUM];
  }
  return &m_pNearBgName[btl::bg::BGSYS_NEAR_TYPE_MAX * ONE_NEAR_BG_CHAR_NUM]; // ERRORが格納されている
}

/**
 *  @brief  遠景ID名取得
 */
char* DebugDispInfo::GetFarBgName( u32 index )
{
  if( index < btl::bg::BGSYS_FAR_TYPE_MAX )
  {
    return &m_pFarBgName[index * ONE_FAR_BG_CHAR_NUM];
  }
  return &m_pFarBgName[btl::bg::BGSYS_FAR_TYPE_MAX * ONE_FAR_BG_CHAR_NUM]; // ERRORが格納されている
}

/**
 *  @brief  今のゾーンID取得
 */
u32 DebugDispInfo::GetNowZoneID()
{
  return m_pFieldmap->GetZoneID();
}

//------------------------------------------------------------------------------
// PRIVATE
//------------------------------------------------------------------------------

/**
 *  @brief  同期arcオープン
 */
void DebugDispInfo::SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID)
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
void DebugDispInfo::SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
  closeReq.arcId = arcID;
  closeReq.heapForReq = pHeap;
  pAsyncFileReadManager->SyncArcFileClose( closeReq );
}

/**
 *  @brief  同期データ読み込み
 */
void DebugDispInfo::SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize)
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

#endif // PM_DEBUG
