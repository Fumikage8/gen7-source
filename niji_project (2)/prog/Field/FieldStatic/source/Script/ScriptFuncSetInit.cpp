//=============================================================================
/**
 *
 *  @file   ScriptFuncSetInit.cpp
 *  @brief  スクリプト命令セット：初期化スクリプト用命令セット
 *  @author hosaka genya
 *  @data   2011.09.27
 *
 */
//=============================================================================

#include <pml/include/pml_PokeParty.h>
#include <debug/include/gfl2_Assert.h>

#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptWork.h"

#include "niji_reference_files/script/FieldPawnTypes.h"

#include "FieldScript/include/ScriptObject.h"

#include "Savedata/include/SaveData.h"
#include "Savedata/include/MyStatus.h"
#include "Savedata/include/ZukanSave.h"
#include "Savedata/include/FieldMenuSave.h"
#include "Savedata/include/JoinFestaDataSave.h"
#include "Savedata/include/Record.h"
#include "Savedata/include/PokeDiarySave.h"

#include "System/include/HeapDefine.h"
#include "System/include/PokemonVersion.h"
#include "GameSys/include/GameManager.h"
#include "Trainer/Trainer/include/tr_tool.h"

#include "Field/FieldStatic/include/Script/ScriptFuncSetInit.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include "Field/FieldStatic/include/Event/EventAddPokemon.h"
#include "Field/FieldStatic/include/Time/EvTime.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProcManager.h"
#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"
#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

// PokeTool
#include "PokeTool/include/PokeToolForm.h"
#include "PokeTool/include/NakayoshiManager.h"
#include "PokeTool/include/PokeToolRemaindSkill.h"
#include <PokeTool/include/PokeToolExtendData.h>
#include "Field/FieldStatic/include/FieldWarpData.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Battle/include/battle_SetupParam.h"

#include "niji_reference_files/script/ScriptPokeParamDefine.h"

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "NetStatic/NetEvent/include/JoinFestaRecreateFieldPersonalListEvent.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

// Regulation
#include "Battle/Regulation/include/RegulationScriptChecker.h"

// resource
#include "arc_def_index/arc_def.h"

#include <AppLib/include/Tool/AppToolRotomFriendship.h>

#include <math/include/gfl2_Easing.h>

// 不要な依存
#include "Field/FieldStatic/include/Script/ScriptFuncSetFieldInit.h"
#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )  ///< ScriptObject の定義に合わせて修正

//-----------------------------------------------------------------------------
/**
 *  @brief  命令テーブルセットを取得
 */
//-----------------------------------------------------------------------------
const GFL_PAWN_FUNC_TABLE* FuncSetInit::GetTable( void )
{
  static const GFL_PAWN_FUNC_TABLE s_FuncTable[] =
  {
    GFL_PAWN_FUNC(FlagGet)
    GFL_PAWN_FUNC(FlagSet)
    GFL_PAWN_FUNC(FlagReset)
    GFL_PAWN_FUNC(WorkGet)
    GFL_PAWN_FUNC(WorkSet)
    GFL_PAWN_FUNC(ZoneIsPokecen)
    GFL_PAWN_FUNC(IsContinueRecover)
    GFL_PAWN_FUNC(CommandNOP)
    GFL_PAWN_FUNC(Assert_)
    GFL_PAWN_FUNC(ReserveScript)
    GFL_PAWN_FUNC(IsReserveScriptSetting)
    GFL_PAWN_FUNC(WeakReserveScript)
    GFL_PAWN_FUNC(CallBackRegist_)
    GFL_PAWN_FUNC(CallBackGetScriptID_)
    GFL_PAWN_FUNC(FadeIn_)
    GFL_PAWN_FUNC(FadeOut_)
    GFL_PAWN_FUNC(FadeOutColor)
    GFL_PAWN_FUNC( FadeWait_ )
    GFL_PAWN_FUNC( FadeCheck_ )

    GFL_PAWN_FUNC( FieldOpen_ )
    GFL_PAWN_FUNC( FieldClose_ )

    GFL_PAWN_FUNC( LoadFieldDLL_ )
    GFL_PAWN_FUNC( UnloadFieldDLL_ )
    GFL_PAWN_FUNC( MapChangeNoFieldCore_ )

    GFL_PAWN_FUNC( SetEventBasePos )
    GFL_PAWN_FUNC( ResetEventBasePos )
    GFL_PAWN_FUNC( CheckEventBasePosEnable )

    GFL_PAWN_FUNC( ItemAdd )
    GFL_PAWN_FUNC( ItemSub )
    GFL_PAWN_FUNC( ItemGetNum )
    GFL_PAWN_FUNC( ItemAddCheck )
    GFL_PAWN_FUNC( ItemIsWazaMachine )
    GFL_PAWN_FUNC( ItemIsMegaStone )
    GFL_PAWN_FUNC( ItemGetCategory )
    GFL_PAWN_FUNC( ItemGetNutsID )
    GFL_PAWN_FUNC( ItemBallID2ItemID )
    GFL_PAWN_FUNC( PocketGetID )
    GFL_PAWN_FUNC( PocketGetTotalCount )
    GFL_PAWN_FUNC( DressupItemAddDel_ )
    GFL_PAWN_FUNC( PokeBeansAddSub )
    GFL_PAWN_FUNC( PokeBeansNumGet )
    
    GFL_PAWN_FUNC( GetTimeZone )
    GFL_PAWN_FUNC( GetHour )
    GFL_PAWN_FUNC( GetMinute )
    GFL_PAWN_FUNC( GetSecond )
    GFL_PAWN_FUNC( GetDay )
    GFL_PAWN_FUNC( GetMonth )
    GFL_PAWN_FUNC( GetWeek )
    GFL_PAWN_FUNC( GetDateTimeToDay )
    GFL_PAWN_FUNC( GetRealTimeToDay )
    GFL_PAWN_FUNC( GetDateTimeToSecond )
    GFL_PAWN_FUNC( GetRealTimeToSecond )
    GFL_PAWN_FUNC( GetGameStartTimeToSecond )
    GFL_PAWN_FUNC( GetBirthdayMonth )
    GFL_PAWN_FUNC( GetBirthdayDay )

    GFL_PAWN_FUNC( RomGetVersion )
    GFL_PAWN_FUNC( RomGetLanguageID )
    GFL_PAWN_FUNC( RegionGet )

    GFL_PAWN_FUNC( PokemonAdd )
    GFL_PAWN_FUNC( PokePartyRemoveMember )
    GFL_PAWN_FUNC( PokePartyGetParam )
    GFL_PAWN_FUNC( PokePartySetParam )
    GFL_PAWN_FUNC( PokePartyAddParam )
    GFL_PAWN_FUNC( PokePartyLevelUp )
    GFL_PAWN_FUNC( PokePartyGetCount )
    GFL_PAWN_FUNC( PokePartyGetCountEx )
    GFL_PAWN_FUNC( PokePartyRecoverAll )
    GFL_PAWN_FUNC( PokePartyIsOwner )
    GFL_PAWN_FUNC( PokePartyHaveWaza )
    GFL_PAWN_FUNC( PokePartyGetWazaCount )
    GFL_PAWN_FUNC( PokePartyGetWazaID )
    GFL_PAWN_FUNC( PokePartySetWaza )
    GFL_PAWN_FUNC( PokePartyCheckWazaRemind )
    GFL_PAWN_FUNC( PokePartySetDefaultName )
    GFL_PAWN_FUNC( PokePartyRecover )
    GFL_PAWN_FUNC( PokePartyGetTrainingDone )
    GFL_PAWN_FUNC( PokePartySetTrainingDone )
    GFL_PAWN_FUNC( PokePartyGetMezameruPowerType )
    GFL_PAWN_FUNC( Shoki3GetPersonalRnd )

    GFL_PAWN_FUNC( BoxIsFull )
    GFL_PAWN_FUNC( BoxIsBattleTeamIn )
    GFL_PAWN_FUNC( BoxMonsNoExists )

    GFL_PAWN_FUNC( BoxGetPokeCount )
    GFL_PAWN_FUNC( WazaOshieCheck )
    GFL_PAWN_FUNC( WazaOshieGetWazaID )

    GFL_PAWN_FUNC( PokePartyToBox )
    GFL_PAWN_FUNC( PokePartyChangeFormNo )
    GFL_PAWN_FUNC( PokePartyChangeRotomFormNo )
    GFL_PAWN_FUNC( PokePartyChangeTorimianFormNo )
    GFL_PAWN_FUNC( ZukanGetCount )
    GFL_PAWN_FUNC( ZukanGetOpenFlag )
    GFL_PAWN_FUNC( ZukanCheckGetFlag )
    GFL_PAWN_FUNC( ZukanCheckSeeFlag )
    GFL_PAWN_FUNC( ZukanIsComplete )

    GFL_PAWN_FUNC( PlayerSetFieldMenu )
    GFL_PAWN_FUNC( PlayerGetFieldMenu )
    GFL_PAWN_FUNC( PlayerGetSex )
    GFL_PAWN_FUNC( PlayerGetMoney )
    GFL_PAWN_FUNC( PlayerSetMoney )
    GFL_PAWN_FUNC( PlayerAddMoney )
    GFL_PAWN_FUNC( PlayerSubMoney )
    
    GFL_PAWN_FUNC( PlayerGetBP )
    GFL_PAWN_FUNC( PlayerSetBP )
    GFL_PAWN_FUNC( PlayerGetZoneID )
    GFL_PAWN_FUNC( PlayerGetID )

    GFL_PAWN_FUNC( PlayerSetTrainerPassStamp )
    GFL_PAWN_FUNC( PlayerResetTrainerPassStamp )
    GFL_PAWN_FUNC( PlayerCheckTrainerPassStamp )

    GFL_PAWN_FUNC( PlayerBallThrowTypeSet )
    GFL_PAWN_FUNC( PlayerBallThrowTypeGet )

    GFL_PAWN_FUNC( GetPlaceNameIDFormZoneID )
    
    GFL_PAWN_FUNC( RecordGet )
    GFL_PAWN_FUNC( RecordAdd )
    GFL_PAWN_FUNC( RecordSet )
    
    GFL_PAWN_FUNC( FlyFlagSet )
    
    GFL_PAWN_FUNC( ZenryokuRingFlagSet )
    GFL_PAWN_FUNC( ZenryokuRingFlagGet )
    GFL_PAWN_FUNC( MegaRingFlagSet )
    GFL_PAWN_FUNC( MegaRingFlagGet )
    GFL_PAWN_FUNC( ShimameguriNoShirushiFlagSet )
    GFL_PAWN_FUNC( ShimameguriNoShirushiFlagGet )
    
    GFL_PAWN_FUNC( PokeDiarySet )

    GFL_PAWN_FUNC( PokePartyCheckDistribution )
    GFL_PAWN_FUNC( MagianaQREventEnableCheck )
    GFL_PAWN_FUNC( M20PikatyuQREventEnableCheck )
    GFL_PAWN_FUNC( SodateyaIsEggExist )

    GFL_PAWN_FUNC( ZigarudeCellGet )
    GFL_PAWN_FUNC( ZigarudeCellAddCheck )
    GFL_PAWN_FUNC( ZigarudeCellSubCheck )
    GFL_PAWN_FUNC( ZigarudeCellAdd )
    GFL_PAWN_FUNC( ZigarudeCellSub )

    GFL_PAWN_FUNC( CheckWLANSwitch )

    GFL_PAWN_FUNC( IsPenaltyTime )
    GFL_PAWN_FUNC( WarpIDSet )
    GFL_PAWN_FUNC( SetEscapeLocation )

    GFL_PAWN_FUNC( IsJoinFestaReward )
    GFL_PAWN_FUNC( IsJoinFestaAttractionOpen )
    GFL_PAWN_FUNC( IsJoinFestaAttractionPlay )
    GFL_PAWN_FUNC( RecreateFieldPersonalList )
    GFL_PAWN_FUNC( JoinFestaAttractionRideOff )
    GFL_PAWN_FUNC( JoinFestaResetRecruiter )

    GFL_PAWN_FUNC( PostmanGetGiftCount )
    GFL_PAWN_FUNC( PostmanGetGiftParam )
    GFL_PAWN_FUNC( PostmanSetGiftReceive )
    GFL_PAWN_FUNC( PostmanSetGiftUsedFlag )

    GFL_PAWN_FUNC( DebugChrEditListAdd_ )
    GFL_PAWN_FUNC( DebugSetSceneEditMode )
    GFL_PAWN_FUNC( DebugGetSceneEditMode )

    GFL_PAWN_FUNC( SetNormalAlolaOffset )
    GFL_PAWN_FUNC( MathGetDiffAngleAbs )
    GFL_PAWN_FUNC( MathGetTargetAngle )
    GFL_PAWN_FUNC( MathGetVectorLength )
    GFL_PAWN_FUNC( MathGetEasingValue )
    GFL_PAWN_FUNC( MathGetVectorX )
    GFL_PAWN_FUNC( MathGetVectorZ )

    GFL_PAWN_FUNC_END,
  };

  return s_FuncTable;
}

const GFL_PAWN_FUNC_TABLE* FuncSetInit::GetTableForRegulation( void )
{
  static const GFL_PAWN_FUNC_TABLE s_FuncTableForRegulation[] =
  {
    GFL_PAWN_FUNC( PokePartyGetParamForRegulation )
    GFL_PAWN_FUNC_END,
  };

  return s_FuncTableForRegulation;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  イベントフラグを取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::FlagGet( AMX * amx, const cell * ptr )
{
  u16 flag_no = ptr[1];

  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_script_work = p_obj->GetWork();

  return p_script_work->GetEventFlag( flag_no );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  イベントフラグを設定
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::FlagSet( AMX * amx, const cell * ptr )
{
  u16 flag_no = ptr[1];

  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_script_work = p_obj->GetWork();

  p_script_work->SetEventFlag( flag_no, true );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  イベントフラグをリセット
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::FlagReset( AMX * amx, const cell * ptr )
{
  u16 flag_no = ptr[1];

  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_script_work = p_obj->GetWork();

  p_script_work->SetEventFlag( flag_no, false );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  イベントワークを取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::WorkGet( AMX * amx, const cell * ptr )
{
  u16 work_no = ptr[1];

  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_script_work = p_obj->GetWork();
  u32 value = p_script_work->GetWorkValue( work_no );

  return value;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  イベントワークを設定
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::WorkSet( AMX * amx, const cell * ptr )
{
  u16 work_no = ptr[1];
  u32 value = ptr[2];

  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_script_work = p_obj->GetWork();

  p_script_work->SetWorkValue( work_no, value );

  return NULL;
}


//=============================================================================
//
//
//          システム系命令
//
//
//=============================================================================
//-----------------------------------------------------------------------------
/**
 * @brief   何もしない！
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::CommandNOP( AMX * amx, const cell * ptr )
{
  return 0;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief アサート
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::Assert_( AMX* amx, const cell * ptr )
{
  if ( ptr[1] == false )
  {
    //char str[256] = {0};
    //amx_GetString( str, &ptr[3], false, 256 );
    GFL_ASSERT_MSG(0, "ScriptCodeAssert ErrorCode = %d, Line = %d \n", ptr[2], ptr[3] );
  }

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  予約スクリプトの登録
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::ReserveScript( AMX* amx, const cell * ptr )
{
  u32 scrid = ptr[1];
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  p_sys->SetReserveScript( scrid );

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief リザーブスクリプトを既にセットしているか判定
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::IsReserveScriptSetting( AMX* amx, const cell * ptr )
{
  //u32 scrid = ptr[1];
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  return p_sys->IsReserveScriptSetting();
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 弱い予約スクリプト呼び出し（他のコールがあった場合に消える）
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::WeakReserveScript( AMX* amx, const cell * ptr )
{
  u32 scrid = ptr[1];
  u32 p0 = ptr[2];
  u32 p1 = ptr[3];
  u32 p2 = ptr[4];
  u32 p3 = ptr[5];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();

  p_sys->SetWeakReservedScript( scrid, p0, p1, p2, p3 );

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 現在のゾーンがポケセンのゾーンか判定
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZoneIsPokecen( AMX* amx, const cell * ptr )
{
  FieldScriptSystem     *p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager  *p_gman = p_sys->GetGameManager();
  GameSys::GameData     *p_gdata = p_gman->GetGameData();

  ZoneDataLoader* pZoneDataLoader = p_gdata->GetFieldZoneDataLoader();
  ZoneDataAccessor zoneDataAccessor( pZoneDataLoader->GetZoneData( p_gdata->GetNowZoneID() ));

  // ポケセン判定
  return zoneDataAccessor.IsPokemonCenter();
}

/* ------------------------------------------------------------------------- */
/**
 * @brief セーブ復帰時によばれたかチェック
 *
 * @return true : セーブ復帰, false : セーブ復帰でない
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::IsContinueRecover( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  return p_sys->IsContinueRecover();
}

//-----------------------------------------------------------------------------
/**
 * @brief トレーナー戦後に呼び出されるコールバックスクリプトの登録
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::CallBackRegist_( AMX* amx, const cell * ptr )
{
  SCR_CALLBACK_TYPE callback_type = static_cast<SCR_CALLBACK_TYPE>(ptr[1]);
  SCRIPTID scrid = ptr[2];
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  p_sys->RegisterCallBackScript( callback_type, scrid );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   コールバックスクリプトIDの取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::CallBackGetScriptID_( AMX* amx, const cell * ptr )
{
  SCR_CALLBACK_TYPE callback_type = static_cast<SCR_CALLBACK_TYPE>(ptr[1]);
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  return p_sys->GetCallBackScriptID( callback_type );
}


/*
 * FadeType変換
 * pawn定義からランタイム上の定義に変換
*/
gfl2::Fade::FADE_TYPE FuncSetInit::ConvertFadeType( s32 type, gfl2::math::Vector4* col_s, gfl2::math::Vector4* col_e )
{
  if ( col_s != NULL )
  {
    if ( type == OUTTYPE_WHITE )
    {
      col_s->Set( 255.0f,255.0f,255.0f,0.0f );
    }else if (type == OUTTYPE_YELLOW){
      col_s->Set( 255.0f,203.0f,51.0f,0.0f ); // あまいみつのいろ
    }else if (type == OUTTYPE_SHIREN3){
      col_s->Set( 220.0f,189.0f,186.0f,0.0f ); // 試練３専用（ポイ：の色）
    }else{
      col_s->Set( 0.0f,0.0f,0.0f,0.0f );
    }
  }
  if ( col_e != NULL )
  {
    if ( type == OUTTYPE_WHITE )
    {
      col_e->Set( 255.0f,255.0f,255.0f,255.0f );
    }else if (type == OUTTYPE_YELLOW){
      col_e->Set( 255.0f,203.0f,51.0f,200.0f ); // あまいみつのいろ
    }else if (type == OUTTYPE_SHIREN3){
      col_e->Set( 220.0f,189.0f,186.0f,200.0f ); // 試練３専用（ポイ：の色）
    }else{
      col_e->Set( 0.0f,0.0f,0.0f,255.0f );
    }
  }
  switch (type) {
  case OUTTYPE_NONE:
    return gfl2::Fade::FADE_TYPE_NONE;
  case OUTTYPE_BLACK:
    return gfl2::Fade::FADE_TYPE_ALPHA;
  case OUTTYPE_WHITE:
    return gfl2::Fade::FADE_TYPE_ALPHA;
  case OUTTYPE_BALL:
    return gfl2::Fade::FADE_TYPE_BALL;
  case OUTTYPE_MASK:
    return gfl2::Fade::FADE_TYPE_MASK;
  case OUTTYPE_DIAMOND:
    return gfl2::Fade::FADE_TYPE_DIAMOND;
  case OUTTYPE_YELLOW:
    return gfl2::Fade::FADE_TYPE_ALPHA;
  case OUTTYPE_CROSS:
    return gfl2::Fade::FADE_TYPE_CROSS;
  case OUTTYPE_SHIREN3:
    return gfl2::Fade::FADE_TYPE_ALPHA;
  case OUTTYPE_CIRCLE:
    return gfl2::Fade::FADE_TYPE_CIRCLE;
  }
  return gfl2::Fade::FADE_TYPE_ALPHA;
}

cell AMX_NATIVE_CALL FuncSetInit::FadeIn_( AMX* amx, const cell * ptr )
{
  s32 sync = ptr[1];
  u32 display = ptr[2];
  s32 type = ptr[3];
  gfl2::Fade::FADE_TYPE fadeType = FuncSetInit::ConvertFadeType( type, NULL, NULL );

  if( fadeType == gfl2::Fade::FADE_TYPE_NONE ){
    return false;
  } 
  if( display == DISP_UPPER || display == DISP_DOUBLE ){
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn( gfl2::Fade::DISP_UPPER, fadeType, sync );
  }
  if( display == DISP_LOWER || display == DISP_DOUBLE ){
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn( gfl2::Fade::DISP_LOWER, fadeType, sync );
  }
  
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetInit::FadeOut_( AMX* amx, const cell * ptr )
{
  s32 type = ptr[1];
  s32 sync = ptr[2];
  s32 display = ptr[3];
  bool bCapture = GFL_BOOL_CAST(ptr[4]);
  
  gfl2::math::Vector4 col_s(0,0,0,0);
  gfl2::math::Vector4 col_e(0,0,0,0);

  gfl2::Fade::FADE_TYPE fadeType = FuncSetInit::ConvertFadeType( type, &col_s, &col_e );

  if( fadeType == gfl2::Fade::FADE_TYPE_NONE ){
    return false;
  }

  if( display == DISP_UPPER || display == DISP_DOUBLE ){
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, fadeType, &col_s, &col_e, sync, bCapture );
  }
  if( display == DISP_LOWER || display == DISP_DOUBLE ){
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, fadeType, &col_s, &col_e, sync, bCapture );
  }
  
  return true;
}

cell AMX_NATIVE_CALL FuncSetInit::FadeOutColor( AMX* amx, const cell * ptr )
{
  s32 sync = ptr[1];
  u32 display = ptr[2];
  f32 r = amx_ctof( ptr[3] );
  f32 g = amx_ctof( ptr[4] );
  f32 b = amx_ctof( ptr[5] );
  f32 a = amx_ctof( ptr[6] );
  bool bCapture = GFL_BOOL_CAST(ptr[7]);
    
  gfl2::math::Vector4 col_s(r,g,b,0);
  gfl2::math::Vector4 col_e(r,g,b,a);

  gfl2::Fade::FADE_TYPE fadeType = gfl2::Fade::FADE_TYPE_ALPHA;

  if (display == DISP_UPPER || display == DISP_DOUBLE){
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestOut(gfl2::Fade::DISP_UPPER, fadeType, &col_s, &col_e, sync, bCapture);
  }
  if (display == DISP_LOWER || display == DISP_DOUBLE){
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestOut(gfl2::Fade::DISP_LOWER, fadeType, &col_s, &col_e, sync, bCapture);
  }
  
  return NULL;
}


cell AMX_NATIVE_CALL FuncSetInit::FadeWait_( AMX* amx, const cell * ptr )
{
  if(
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_UPPER ) &&
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_LOWER )
  ) return true;
  
  return false;
}

cell AMX_NATIVE_CALL FuncSetInit::FadeCheck_( AMX* amx, const cell * ptr )
{
  u16 display = ptr[1];

  if( display == DISP_UPPER ){
    return !GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEndStatus(gfl2::Fade::DISP_UPPER);
  }
  if( display == DISP_LOWER ){
    return !GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEndStatus(gfl2::Fade::DISP_LOWER);
  }
  if( display == DISP_DOUBLE ){
    return !GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEndStatus();
  }  
  return 0;
}

cell AMX_NATIVE_CALL FuncSetInit::FieldOpen_( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  EventFieldmapOpen* p_event;
  
  ScrFieldmapControlMode open_mode = static_cast<ScrFieldmapControlMode>(ptr[1]);
  ScrFadeMode fade = static_cast<ScrFadeMode>(ptr[2]);

  FieldmapDesc desc;
  desc.playerLocation = *p_gdata->GetStartLocation();
  switch( open_mode ){
  case SCR_FIELDMAP_CONTROL_MAPJUMP:
    desc.openMode = FM_OPEN_MODE_INIT;
    break;
  case SCR_FIELDMAP_CONTROL_BATTLE:
  case SCR_FIELDMAP_CONTROL_APP:
  default:
    desc.openMode = FM_OPEN_MODE_APP_RECOVER;
    break;
  }

  // オープンイベント
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen > (
                p_gman->GetGameEventManager( ) );
  p_event->SetDesc( desc );

  if( fade == SCR_FADE_ON ){
    p_event->SetFadeCallBackDefault();
  }else{
    p_event->SetFadeCallBack( NULL, NULL, NULL );
  }
  p_sys->Suspend();
  
  return NULL;
}
cell AMX_NATIVE_CALL FuncSetInit::FieldClose_( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
//  GameSys::GameData* p_gdata = p_gman->GetGameData();
  EventFieldmapClose* p_event;
  
  ScrFieldmapControlMode close_mode = static_cast<ScrFieldmapControlMode>(ptr[1]);
  ScrFadeMode fade = static_cast<ScrFadeMode>(ptr[2]);

  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose > (
              p_gman->GetGameEventManager( ) );
  
  EventFieldmapClose::Desc desc;
 
  switch( close_mode ){
  case SCR_FIELDMAP_CONTROL_MAPJUMP:
    desc.closeType = FM_CLOSE_MODE_MAPJUMP;
    break;
  case SCR_FIELDMAP_CONTROL_BATTLE:
  case SCR_FIELDMAP_CONTROL_APP:
  default:
    desc.closeType = FM_CLOSE_MODE_APP; // メモリを全破棄
    break;
  }
  p_event->SetDesc( desc );

  if( fade == SCR_FADE_ON ){
    p_event->SetFadeCallBackDefault();
  }else{
    p_event->SetFadeCallBack( NULL, NULL, NULL );
  }
  p_sys->Suspend();
  return NULL;
}


cell AMX_NATIVE_CALL FuncSetInit::LoadFieldDLL_( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();

  //既に読み込まれていない場合何もしない
  p_gman->GetFieldResident()->InitializeFieldDLL( p_gman->GetAsyncFileManager() );

  return NULL;
}
cell AMX_NATIVE_CALL FuncSetInit::UnloadFieldDLL_( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();

  //読み込まれていない場合何もしない
  p_gman->GetFieldResident()->TerminateFieldDLL();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func  MapChangeWithoutFieldCore_
 * @brief フィールド破棄状態からのマップチェンジコア
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::MapChangeNoFieldCore_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = pSys->GetGameManager();

  ZoneID  zoneId = ptr[1];
  f32     posX   = amx_ctof( ptr[2] );
  f32     posY   = amx_ctof( ptr[3] );
  f32     posZ   = amx_ctof( ptr[4] );
  u32     isForceEnd = ptr[5];
  u32     isFadeEnable = ptr[6];
  bool    isBGMKeep = GFL_BOOL_CAST(ptr[7]);
  bool    isPlaceNameEnable = GFL_BOOL_CAST(ptr[8]);

  gfl2::math::Vector3 pos(posX,posY,posZ);

  EventMapJumpWithoutField::EventMapJumpType type = EventMapJumpWithoutField::EV_MAPJUMP_BLACK_FADE;

  if( isFadeEnable == false ){
    type = EventMapJumpWithoutField::EV_MAPJUMP_NO_EFFECT;
  }

  if( isForceEnd ){
    EventMapJumpWithoutField* p_event = EventMapJump::CreateMapJumpPosWithoutField( p_gman, zoneId, pos, type, isPlaceNameEnable, isBGMKeep );
    pSys->EntryNextEvent(p_event);
    pSys->ForceEnd();
  }else{
    EventMapJump::CallMapJumpPosWithoutField( p_gman, zoneId, pos, type, isPlaceNameEnable, isBGMKeep );
  }

  pSys->Suspend();
  return NULL;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  イベント相対座標システム
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::SetEventBasePos( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();

  f32  posX = amx_ctof( ptr[1] );
  f32  posY = amx_ctof( ptr[2] );
  f32  posZ = amx_ctof( ptr[3] );

  p_sys->SetEventPosition(posX,posY,posZ);
  p_sys->Suspend();
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetInit::ResetEventBasePos( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  p_sys->ResetEventPosition();
  p_sys->Suspend();
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetInit::CheckEventBasePosEnable( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  return p_sys->CheckEventPositionEnable();
}

//=============================================================================
//
//
//    本体情報
//
//
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *  @brief  現在のタイムゾーンを取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetTimeZone( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  return EvTime::GetAlolaTimeZone( p_gdata );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  現在の時間を取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetHour( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  return EvTime::GetAlolaHour( p_gdata );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  現在の分を取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetMinute( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  return EvTime::GetAlolaMinute( p_gdata );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  現在の秒を取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetSecond( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  return EvTime::GetAlolaSecond( p_gdata );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  現在日を取得（本体時間）
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetDay( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  const Savedata::GameTime* p_gtime = p_gdata->GetGameTime();
  gfl2::system::Date date;
  p_gtime->GetDate( &date );

  return date.GetDay();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  現在の月を取得（本体時間）
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetMonth( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  const Savedata::GameTime* p_gtime = p_gdata->GetGameTime();
  gfl2::system::Date date;
  p_gtime->GetDate( &date );

  return date.GetMonth();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  現在の曜日を取得（本体時間）
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetWeek( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  return EvTime::GetDeviceWeek( p_gdata );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ペナルティタイム発動中かどうかをチェック
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::IsPenaltyTime( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  return p_gdata->GetGameTime()->IsPenaltyMode();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  2000年 1月 1日 00:00:00 からの経過日数を取得する（本体時間：EvTime）
 *  - セーブデータの初期値を0として設計するため1オリジンとする
 *  - ホテルを実装する際などに利用する
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetDateTimeToDay( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  gfl2::system::Date date;
  EvTime::GetDeviceDateTime( p_gdata, &date );
  return static_cast< cell >( date.GetDateTimeToDay() + 1 ); // 100年を保障すればいいのでcellがu16以上の幅を持っていればOK
}

//-----------------------------------------------------------------------------
/**
 *  @brief  2000年 1月 1日 00:00:00 からの経過日数を取得する（本体時間：リアルタイム）
 *  - セーブデータの初期値を0として設計するため1オリジンとする
 *  - ホテルを実装する際などに利用する
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetRealTimeToDay( AMX * amx, const cell * ptr )
{
  gfl2::system::Date date;
  EvTime::GetDeviceRealDateTime( &date );
  return static_cast< cell >( date.GetDateTimeToDay() + 1 ); // 100年を保障すればいいのでcellがu16以上の幅を持っていればOK
}

/* ------------------------------------------------------------------------- */
/**
 *  @brief  2000年 1月 1日 00:00:00 からの経過秒数を取得する（本体時間：EvTime）
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::GetDateTimeToSecond( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  gfl2::system::Date date;
  EvTime::GetDeviceDateTime( p_gdata, &date );
  return static_cast< cell >( date.GetDateTimeToSecond() );
}

/* ------------------------------------------------------------------------- */
/**
 *  @brief  2000年 1月 1日 00:00:00 からの経過秒数を取得する(本体時間：リアルタイム)
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::GetRealTimeToSecond( AMX* amx, const cell * ptr )
{
  gfl2::system::Date date;
  EvTime::GetDeviceRealDateTime( &date );

  return static_cast< cell >( date.GetDateTimeToSecond() );
}

/* ------------------------------------------------------------------------- */
/**
 * ゲーム開始時の 2000年 1月 1日 00:00:00 からの経過秒を取得（本体時間：EvTime）
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::GetGameStartTimeToSecond( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager * p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  return static_cast< cell >( p_gdata->GetGameTime()->GetStartSec() );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  本体情報から、誕生日の月を取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetBirthdayMonth( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager * p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  s32 birth_month = 1;
  s32 birth_day   = 1;

  EvTime::GetBirthday(p_gdata, &birth_month, &birth_day);

  return birth_month;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  本体情報から、誕生日の日を取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetBirthdayDay( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager * p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  s32 birth_month = 1;
  s32 birth_day   = 1;

  EvTime::GetBirthday(p_gdata, &birth_month, &birth_day);

  return birth_day;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  カセットバージョンの取得（VERSION_CTR_RUBY or VERSION_CTR_SAPPHIRE
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::RomGetVersion( AMX * amx, const cell * ptr )
{
  return GET_VERSION();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  言語IDの取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::RomGetLanguageID( AMX * amx, const cell * ptr )
{
  return System::GetLang();
}

//-----------------------------------------------------------------------------
/**
 * @brief CTR本体リージョンの取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::RegionGet( AMX * amx, const cell * ptr )
{
  return System::GflUse::GetHardwareRegion();
}

//=============================================================================
//
//
//      ポケモン
//
//
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *  @brief  ポケパーティ/ボックスにポケモンを追加
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokemonAdd(AMX * amx, const cell * ptr)
{
  u16 add_id = ptr[1];
  AddPokemonMode add_mode = static_cast<AddPokemonMode>(ptr[2]);
  s32 type = ptr[3];
  s32 sync = ptr[4];
  s32 display = ptr[5];
  
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();

  gfl2::math::Vector4 col_s(0.0f,0.0f,0.0f,0.0f);
  gfl2::math::Vector4 col_e(0.0f,0.0f,0.0f,255.0f);

  gfl2::Fade::FADE_TYPE fadeType = FuncSetInit::ConvertFadeType( type, &col_s, &col_e );
  gfl2::Fade::DISP dispType = gfl2::Fade::DISP_DOUBLE;

  if ( display == DISP_UPPER )
  {
    dispType = gfl2::Fade::DISP_UPPER;
  }
  else if ( display == DISP_LOWER )
  {
    dispType = gfl2::Fade::DISP_LOWER;
  }
  EventAddPokemon* p_event = EventAddPokemon::CallAddPokemon( p_gman, add_id, add_mode );
  p_event->SetFadeParam( dispType, fadeType, col_s, col_e, sync );

  // ウェイト
  p_sys->Suspend();

  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ポケパーティからポケモンを削除(利用許可制)
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokePartyRemoveMember(AMX * amx, const cell * ptr)
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman   = p_sys->GetGameManager();
  GameSys::GameData*     p_gdata  = p_gman->GetGameData();
  pml::PokeParty*        party    = p_gdata->GetPlayerParty();
  pml::pokepara::PokemonParam*     p_pp     = NULL;
  
  u8 pos = ptr[1];

  if( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return false; //index不正
  }
  //戦えるポケモンの数チェック
  if( party->GetMemberCountEx( pml::PokeParty::POKE_COUNT_TYPE_BATTLE_ENABLE, (1<<pos) ) < 1  )
  {
    GFL_ASSERT(0);
    return false;
  }
  p_pp = party->GetMemberPointer( pos );

  //所持道具チェック(何か所持しているときは削除できないことにしておく)
  if( p_pp->GetItem() != ITEM_DUMMY_DATA ){
    GFL_ASSERT(0);
    return false;
  }
  party->RemoveMember(pos);
  return true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ポケモンのパラメータを取得する
 *
 *  @param[in]  pos 取得するポケモンの位置
 *  @param[in]  パラメータID
 *
 *  @retval 取得したパラメータ
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokePartyGetParam(AMX* amx, const cell * ptr)
{
  //ScripPokeParamID idx = ptr[1];
  u16 pos = ptr[1];
  u16 idx = ptr[2];
  u16 param = ptr[3];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party      = p_gameData->GetPlayerParty();

  return GetPokePartyPokemonParam( pos, idx, param, party, p_gameData );
}

cell AMX_NATIVE_CALL FuncSetInit::PokePartyGetParamForRegulation(AMX* amx, const cell * ptr)
{
  u16 pos = ptr[1];
  u16 idx = ptr[2];
  u16 param = ptr[3];

  s32 result = 0;

  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

  pml::PokeParty* pCheckPokeParty = RegulationScriptChecker::GetCheckPokeParty();

  if( pCheckPokeParty )
  {
    result = GetPokePartyPokemonParam( pos, idx, param, pCheckPokeParty, pGameData );
  }

  return result;
}

s32 FuncSetInit::GetPokePartyPokemonParam( u16 pos, u16 idx, u16 param, pml::PokeParty* party, GameSys::GameData* p_gameData )
{
  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);
  s32 ret = 0;

  // パーソナルをロード
  pml::personal::LoadPersonalData( p_pp->GetMonsNo(), p_pp->GetFormNo() );

  switch (idx)
  {
    case SCRID_monsno:
      // タマゴの場合はタマゴを返す
      if( p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
      {
        return MONSNO_TAMAGO;
      }
      ret = p_pp->GetMonsNo();
      return ret;
    case SCRID_formno:
      ret = p_pp->GetFormNo();
      return ret;
    case SCRID_hp:
      return p_pp->GetHp();
    case SCRID_hpmax:
      return p_pp->GetMaxHp();
    case SCRID_pp:
      return p_pp->GetWazaPP( param );
    case SCRID_ppmax:
      return p_pp->GetWazaMaxPP( param );
    case SCRID_sex:
      /*
       *  - タマゴの場合、念のため不定を返す
       *  副作用がでないよう、予めタマゴチェックを通すと良い
       *  - また、PM_*とpawnのSC_SEX_*は一致する設計のため
       *  そのままの値を返す
       */
      ret = PM_NEUTRAL;
      if( p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false )
      {
        ret = p_pp->GetSex();
      }
      return ret;
    case SCRID_type1:
      return p_pp->GetType1();
    case SCRID_type2:
      return p_pp->GetType2();
    case SCRID_isEgg:
      return p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
    case SCRID_isEggLegal:
      return p_pp->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG );
    case SCRID_natsuki:
      return p_pp->GetFamiliarity();
    case SCRID_total_effort:
      return p_pp->GetTotalEffortPower();
    case SCRID_item :
      return p_pp->GetItem( );    // read onlyで用意
    case SCRID_ribbon :
      return p_pp->HaveRibbon( param );
    case SCRID_now_level:
      return p_pp->GetLevel();
    case SCRID_get_level:
      return p_pp->GetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL );
    case SCRID_talent_hp:
      return p_pp->GetTalentPower( pml::pokepara::POWER_HP );
    case SCRID_talent_atk:
      return p_pp->GetTalentPower( pml::pokepara::POWER_ATK );
    case SCRID_talent_def:
      return p_pp->GetTalentPower( pml::pokepara::POWER_DEF );
    case SCRID_talent_spatk:
      return p_pp->GetTalentPower( pml::pokepara::POWER_SPATK );
    case SCRID_talent_spdef:
      return p_pp->GetTalentPower( pml::pokepara::POWER_SPDEF );
    case SCRID_talent_agi:
      return p_pp->GetTalentPower( pml::pokepara::POWER_AGI );
    case SCRID_seikaku:
      return p_pp->GetSeikaku();
    case SCRID_tokusei:
      return p_pp->GetTokuseiNo();
    case SCRID_effort_hp:      // 努力値：HP
      return p_pp->GetEffortPower( pml::pokepara::POWER_HP );
    case SCRID_effort_atk:     // 努力値：こうげき
      return p_pp->GetEffortPower( pml::pokepara::POWER_ATK );
    case SCRID_effort_def:     // 努力値：ぼうぎょ
      return p_pp->GetEffortPower( pml::pokepara::POWER_DEF );
    case SCRID_effort_spatk:   // 努力値：とくこう
      return p_pp->GetEffortPower( pml::pokepara::POWER_SPATK );
    case SCRID_effort_spdef:   // 努力値：とくぼう
      return p_pp->GetEffortPower( pml::pokepara::POWER_SPDEF );
    case SCRID_effort_agi:     // 努力値：すばやさ
      return p_pp->GetEffortPower( pml::pokepara::POWER_AGI );
    case SCRID_power_hp:             ///< ステータス：HP
      return p_pp->GetPower( pml::pokepara::POWER_HP );
    case SCRID_power_atk:      ///< ステータス：ATK
      return p_pp->GetPower( pml::pokepara::POWER_ATK );
    case SCRID_power_def:      ///< ステータス：DEF
      return p_pp->GetPower( pml::pokepara::POWER_DEF );
    case SCRID_power_spatk:    ///< ステータス：SPATK
      return p_pp->GetPower( pml::pokepara::POWER_SPATK );
    case SCRID_power_spdef:    ///< ステータス：SPDEF
      return p_pp->GetPower( pml::pokepara::POWER_SPDEF );
    case SCRID_power_agi:      ///< ステータス：AGI
      return p_pp->GetPower( pml::pokepara::POWER_AGI );
    case SCRID_condition_style:      ///< コンディション：かっこよさ
      return p_pp->GetCondition( pml::pokepara::CONDITION_STYLE );
    case SCRID_condition_beautiful:  ///< コンディション：うつくしさ
      return p_pp->GetCondition( pml::pokepara::CONDITION_BEAUTIFUL );
    case SCRID_condition_cute:       ///< コンディション：かわいさ
      return p_pp->GetCondition( pml::pokepara::CONDITION_CUTE );
    case SCRID_condition_clever:     ///< コンディション：かしこさ
      return p_pp->GetCondition( pml::pokepara::CONDITION_CLEVER );
    case SCRID_condition_strong:     ///< コンディション：たくましさ
      return p_pp->GetCondition( pml::pokepara::CONDITION_STRONG );
    case SCRID_condition_fur:        ///< コンディション：毛艶
      return p_pp->GetCondition( pml::pokepara::CONDITION_FUR );
    case SCRID_height:
      return pml::personal::GetPersonalParam( pml::personal::PARAM_ID_height );
    case SCRID_weight:
      return pml::personal::GetPersonalParam( pml::personal::PARAM_ID_weight );
    case SCRID_eventflag:
      return p_pp->GetEventPokeFlag();
    case SCRID_sick:
      return p_pp->HaveSick();
    case SCRID_nakayoshi:
      {
        Savedata::MyStatus* p_status = p_gameData->GetPlayerStatus();
        return PokeTool::nakayoshi::GetFriendship( p_pp, p_status );
      }
    case SCRID_feed:
      return p_pp->GetFeed();
    case SCRID_isRare:  //BTS5422 トリミアンのトリミングイベントでレアかどうかを取得したい
      return p_pp->IsRare();
    case SCRID_haveNickName:  ///<ニックネームを持っているか？
      return p_pp->HaveNickName();
    case SCRID_isDefaultNickName:  ///<プレイ言語デフォルト名か？
      return p_pp->IsDefaultNickName();
    case SCRID_egg_birth:
      return pml::personal::GetPersonalParam( pml::personal::PARAM_ID_egg_birth );
    case SCRID_rom_version:
      return p_pp->GetCassetteVersion();
    case SCRID_get_ball:
      return p_pp->GetGetBall();
    case SCRID_draw_id:
      u32 poke_id;
      PokeTool::GetDrawID(p_pp, &poke_id);
      return poke_id;
    case SCRID_is_egg_birth:
      // タマゴチェック
      if (p_pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)) return false; // まだタマゴの場合はfalse
      // タマゴから孵化したかのチェック
      return (p_pp->GetMemories(pml::pokepara::MEMORIES_EGG_TAKEN_PLACE) != 0); //タマゴ捕獲場所１が０でなければタマゴ孵化
    case SCRID_personal_rnd:
      return p_pp->GetPersonalRnd();
  }

  GFL_ASSERT(0);
  return 0;
}


//-----------------------------------------------------------------------------
/**
 *  @brief ポケモンのパラメータをセットする
 *
 *  @param[in]  pos 取得するポケモンの位置
 *  @param[in]  id  パラメータID
 *  @param[in]  value セットする値
 *
 *  @return   取得したパラメータ
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokePartySetParam(AMX* amx, const cell * ptr)
{
  u16 pos = ptr[1];
  u16 id  = ptr[2];
  u16 value = ptr[3];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party      = p_gameData->GetPlayerParty();


  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);
  switch (id) {
  case SCRID_SET_talent_hp:
    p_pp->ChangeTalentPower( pml::pokepara::POWER_HP, value );
    break;
  case SCRID_SET_talent_atk:
    p_pp->ChangeTalentPower( pml::pokepara::POWER_ATK, value );
    break;
  case SCRID_SET_talent_def:
    p_pp->ChangeTalentPower( pml::pokepara::POWER_DEF, value );
    break;
  case SCRID_SET_talent_spatk:
    p_pp->ChangeTalentPower( pml::pokepara::POWER_SPATK, value );
    break;
  case SCRID_SET_talent_spdef:
    p_pp->ChangeTalentPower( pml::pokepara::POWER_SPDEF, value );
    break;
  case SCRID_SET_talent_agi:
    p_pp->ChangeTalentPower( pml::pokepara::POWER_AGI, value );
    break;
  case SCRID_SET_ribbon:
    p_pp->SetRibbon( value );
    break;
  case SCRID_SET_natsuki:
    p_pp->SetFamiliarity( value );
    break;
  case SCRID_SET_effort_hp:
    p_pp->ChangeEffortPower( pml::pokepara::POWER_HP, value );
    break;
  case SCRID_SET_effort_atk:
    p_pp->ChangeEffortPower( pml::pokepara::POWER_ATK, value );
    break;
  case SCRID_SET_effort_def:
    p_pp->ChangeEffortPower( pml::pokepara::POWER_DEF, value );
    break;
  case SCRID_SET_effort_spatk:
    p_pp->ChangeEffortPower( pml::pokepara::POWER_SPATK, value );
    break;
  case SCRID_SET_effort_spdef:
    p_pp->ChangeEffortPower( pml::pokepara::POWER_SPDEF, value );
    break;
  case SCRID_SET_effort_agi:
    p_pp->ChangeEffortPower( pml::pokepara::POWER_AGI, value );
    break;
  case SCRID_SET_condition_style:      ///< コンディション：かっこよさ
    p_pp->SetCondition( pml::pokepara::CONDITION_STYLE, value );
    break;
  case SCRID_SET_condition_beautiful:  ///< コンディション：うつくしさ
    p_pp->SetCondition( pml::pokepara::CONDITION_BEAUTIFUL, value );
    break;
  case SCRID_SET_condition_cute:       ///< コンディション：かわいさ
    p_pp->SetCondition( pml::pokepara::CONDITION_CUTE, value );
    break;
  case SCRID_SET_condition_clever:     ///< コンディション：かしこさ
    p_pp->SetCondition( pml::pokepara::CONDITION_CLEVER, value );
    break;
  case SCRID_SET_condition_strong:     ///< コンディション：たくましさ
    p_pp->SetCondition( pml::pokepara::CONDITION_STRONG, value );
    break;
  case SCRID_SET_condition_fur:        ///< コンディション：毛艶
    p_pp->SetCondition( pml::pokepara::CONDITION_FUR, value );
    break;
  default:
    GFL_ASSERT( 0 );
  }

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief ポケモンのパラメータを増やす
 *
 *  @param[in]  pos 取得するポケモンの位置
 *  @param[in]  id  パラメータID
 *  @param[in]  value セットする値
 *
 *  @return   取得したパラメータ
 */
//-----------------------------------------------------------------------------
static bool _PokePartyAddParam( s32 pos, u16 id, u16 value, pml::PokeParty* p_party, Savedata::MyStatus* p_status )
{
  if ( static_cast<u32>(pos) >= p_party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return false;
  }

  pml::pokepara::PokemonParam* p_pp = p_party->GetMemberPointer(pos);

  switch (id)
  {
  case SCRID_ADD_nakayoshi:
    PokeTool::nakayoshi::UpdateFriendship( p_pp, p_status, value );
    break;

  case SCRID_ADD_feed:
    p_pp->AddFeed( value );
    break;

  case SCRID_ADD_effort_hp:        ///< 努力値を増やす：HP
    p_pp->AddEffortPower( pml::pokepara::POWER_HP, value );
    break;
  case SCRID_ADD_effort_atk:       ///< 努力値を増やす：こうげき
    p_pp->AddEffortPower( pml::pokepara::POWER_ATK, value );
    break;
  case SCRID_ADD_effort_def:       ///< 努力値を増やす：ぼうぎょ
    p_pp->AddEffortPower( pml::pokepara::POWER_DEF, value );
    break;
  case SCRID_ADD_effort_spatk:     ///< 努力値を増やす：とくこう
    p_pp->AddEffortPower( pml::pokepara::POWER_SPATK, value );
    break;
  case SCRID_ADD_effort_spdef:     ///< 努力値を増やす：とくぼう
    p_pp->AddEffortPower( pml::pokepara::POWER_SPDEF, value );
    break;
  case SCRID_ADD_effort_agi:       ///< 努力値を増やす：すばやさ
    p_pp->AddEffortPower( pml::pokepara::POWER_AGI, value );
    break;
  case SCRID_ADD_natsuki:          ///< なつき度を増やす
    p_pp->AddFamiliarity( value );
    break;
  //----------------------------------------------------------  
  case SCRID_SUB_effort_hp:        ///< 努力値を減らす：HP
    p_pp->SubEffortPower( pml::pokepara::POWER_HP, value );
    break;
  case SCRID_SUB_effort_atk:       ///< 努力値を減らす：こうげき
    p_pp->SubEffortPower( pml::pokepara::POWER_ATK, value );
    break;
  case SCRID_SUB_effort_def:       ///< 努力値を減らす：ぼうぎょ
    p_pp->SubEffortPower( pml::pokepara::POWER_DEF, value );
    break;
  case SCRID_SUB_effort_spatk:     ///< 努力値を減らす：とくこう
    p_pp->SubEffortPower( pml::pokepara::POWER_SPATK, value );
    break;
  case SCRID_SUB_effort_spdef:     ///< 努力値を減らす：とくぼう
    p_pp->SubEffortPower( pml::pokepara::POWER_SPDEF, value );
    break;
  case SCRID_SUB_effort_agi:       ///< 努力値を減らす：すばやさ
    p_pp->SubEffortPower( pml::pokepara::POWER_AGI, value );
    break;
  case SCRID_SUB_natsuki:          ///< なつき度を減らす
    p_pp->SubFamiliarity( value );
    break;
  

  default:
    GFL_ASSERT(0);
    return false;
  }

  return true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ポケモンのパラメータを増やす
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartyAddParam( AMX* amx, const cell * ptr )
{
  s32 pos = ptr[1];
  u16 id  = ptr[2];
  u16 value = ptr[3];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        p_party     = p_gameData->GetPlayerParty();
  Savedata::MyStatus*    p_status    = p_gameData->GetPlayerStatus();

  if ( pos == SCR_POKEPARTY_ADD_PARAM_POS_ALL )
  {
    u32 cnt = p_party->GetMemberCount();
    for ( u32 i = 0; i < cnt; ++i )
    {
      pml::pokepara::PokemonParam* p_pp = p_party->GetMemberPointer(i);
      // タマゴでないやつにセット
      if ( p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false )
      {
        _PokePartyAddParam( i, id, value, p_party, p_status );
      }
    }
  }
  else
  {
    // 単体
    _PokePartyAddParam( pos, id, value, p_party, p_status );
  }

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   手持ちポケモンのレベルを上げる
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokePartyLevelUp( AMX *amx, const cell * ptr )
{
  u16 pos       = ptr[1];
  u16 add_level = ptr[2];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();

  pml::PokeParty* party = p_gameData->GetPlayerParty();

  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return false;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);
  u32 before_level = p_pp->GetLevel();
  p_pp->LevelUp( add_level ); //レベル上げ処理
  //上がったかどうかをbool値で返している
  return ( before_level < p_pp->GetLevel() );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  自分のてもち数を取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokePartyGetCount( AMX * amx, const cell * ptr )
{
  pml::PokeParty::CountType type = static_cast<pml::PokeParty::CountType>(ptr[1]);

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();

  pml::PokeParty         *party = p_gameData->GetPlayerParty();

  return party->GetMemberCountEx( type );
}

cell AMX_NATIVE_CALL FuncSetInit::PokePartyGetCountEx( AMX * amx, const cell * ptr )
{
  pml::PokeParty::CountType type = static_cast<pml::PokeParty::CountType>(ptr[1]);
  u8                        egnore_bit = ptr[2];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();

  pml::PokeParty         *party = p_gameData->GetPlayerParty();

  return party->GetMemberCountEx( type, egnore_bit );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ポケパーティを全開する
 */
//-----------------------------------------------------------------------------

cell AMX_NATIVE_CALL FuncSetInit::PokePartyRecoverAll(AMX * amx, const cell *)
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty* party = p_gameData->GetPlayerParty();

  party->RecoverAll();

  return NULL;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief 手持ちのポケモンの親が自分かチェック
 *
 * @return true:自分が親, false:他人が親
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartyIsOwner( AMX* amx, const cell * ptr )
{
  u32 pos = ptr[1];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();
  Savedata::MyStatus*    p_status = p_gameData->GetPlayerStatus();

  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return false;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

  return p_status->IsMyPokemon( *p_pp );
}


/* ------------------------------------------------------------------------- */
/**
 * @brief 指定ワザを既に持っているかチェック
 *
 * @return IDX指定 > true : 持っている, false : 持っていない
 * @return 全体 > 手持ちIDX、ない場合は SCR_POKEPARTY_HAVE_WAZA_ALL_RET_NONE
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartyHaveWaza( AMX * amx, const cell * ptr )
{
  s32 pos = ptr[1]; // -1 だと全体
  WazaNo waza = static_cast<WazaNo>(ptr[2]); //

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();
  u32 count = party->GetMemberCount();

  if ( pos == SCR_POKEPARTY_ADD_PARAM_POS_ALL )
  {
    for ( u32 i = 0; i < count; ++i )
    {
      pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(i);

      // タマゴでないやつにセット
      if ( p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false )
      {
        if( p_pp->HaveWaza(waza) ) return i;
      }
    }
  }
  else
  {
    if( static_cast<u32>(pos) >= count ){
      GFL_ASSERT_MSG(0,"party index over\n");
      return false; //index over
    }
    pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

    if ( p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false ){
      return party->GetMemberPointer(pos)->HaveWaza(waza);
    }else{
      return false; //タマゴの中身はユーザーに見えないので、覚えていないと扱う
    }
  }
  return false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 持っている技の数を返す
 *
 * @return 持っている技の数
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartyGetWazaCount( AMX * amx, const cell * ptr )
{
  u32 pos = ptr[1];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();

  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return 0;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

  return p_pp->GetWazaCount();
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ワザIDを取得
 *
 * @return ワザID
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartyGetWazaID( AMX * amx, const cell * ptr )
{
  u32 pokepos = ptr[1];
  u32 wazapos = ptr[2];

  GFL_ASSERT( wazapos < pml::MAX_WAZA_NUM );

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();

  if ( pokepos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return 0;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pokepos);

  return p_pp->GetWazaNo( wazapos );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 手持ちポケモンの技セット(追加・置き換え・忘れ対応
 *
 * @param pos ポケモンの位置
 * @param waza_pos 技の位置(5で技を押し出し式で追加
 * @param waza_id  技の番号(0で技を忘れる
 *
 * @return 持っている技の数
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartySetWaza( AMX * amx, const cell * ptr )
{
  u32 pos = ptr[1];
  u32 waza_pos = ptr[2]; // 5で押し出し方式で追加
  WazaNo waza_no = static_cast<WazaNo>(ptr[3]);

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();

  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return 0;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

  GFL_ASSERT( waza_pos <= 5 );

  if( waza_no == WAZANO_NULL )
  {
    // 忘れる
    p_pp->RemoveWaza( waza_pos );
    p_pp->CloseUpWazaPos( );
  }
  else
  if( waza_pos == 5 )
  {
    // 押し出し方式で覚える
    p_pp->PushWaza( waza_no );
  }
  else
  {
    //SBTS[1361]SBTS[1522] スロットが歯抜けにならないように詰めるケアをいれておく
    //(CloseUpWazaPosでは2スロ以上の歯抜けを埋められないので、指定位置がオーバーしていたら最後につむ)
    u8 waza_count = p_pp->GetWazaCount();
    if( waza_pos >= waza_count ){
      waza_pos = waza_count;
    }
    p_pp->SetWaza( waza_pos, waza_no );
  }

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 思い出し技の有無をチェック
 *
 * @return true : あり, false : なし
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartyCheckWazaRemind( AMX* amx, const cell * ptr )
{
  u32 pos = ptr[1];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();

  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return false;
  }

  u32 ret = 0;

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  PokeTool::RemaindSkill* p_remind = GFL_NEW( p_heap ) PokeTool::RemaindSkill( p_heap, p_pp );

  ret = p_remind->GetListMax();

  GFL_SAFE_DELETE( p_remind );

  return ret > 0;
}

/* ------------------------------------------------------------------------- */
/*
 * @brief デフォルト名をセットする
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartySetDefaultName( AMX* amx, const cell * ptr )
{
  u32 pos = ptr[1];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();

  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return false;
  }
  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

  if ( p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ) return false;

  p_pp->SetDefaultNickName();

  return true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ポケパーティから1匹選んで全回復する
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartyRecover(AMX * amx, const cell * ptr)
{
  u32 pos = ptr[1];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();


  if( pos >= party->GetMemberCount())
  {
    GFL_ASSERT( 0);
    return false;
  }
    
  pml::pokepara::PokemonParam* pp = party->GetMemberPointer( pos);
  
  if ( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG))
  {
    return false;
  }

  pp->RecoverAll();

  return true;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief すごい特訓を行ったかのGetter
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartyGetTrainingDone(AMX* amx, const cell * ptr)
{
  //Param
  u32 poke_slot = ptr[1];
  pml::pokepara::PowerID para_id   = static_cast<pml::pokepara::PowerID>(ptr[2]);

  //Alias
  FieldScriptSystem*     p_sys       = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();

  //Check
  if( party->GetMemberCount() <= poke_slot )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  //Get
  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(poke_slot);
  return p_pp->IsTrainingDone(para_id);
}

/* ------------------------------------------------------------------------- */
/**
 * @brief すごい特訓を行ったかのSetter
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartySetTrainingDone(AMX* amx, const cell * ptr)
{
  //Param
  u32 poke_slot = ptr[1];
  pml::pokepara::PowerID para_id   = static_cast<pml::pokepara::PowerID>(ptr[2]);

  //Alias
  FieldScriptSystem*     p_sys       = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();

  //Check
  if( party->GetMemberCount() <= poke_slot )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  //Set
  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(poke_slot);
  p_pp->SetTrainingDone(para_id);

  //Apply
  //@fix GFNMCat[1390] ボックスなどに入れないとパラメータが反映されなかったのを即座に反映するように修正
  p_pp->RecalculateCalcData();

  return 0;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief めざめるパワーのタイプを取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartyGetMezameruPowerType( AMX* amx, const cell * ptr )
{
  u16 pos = ptr[1];
  u8 machine_no;

  //わざマシン１０がめざめるパワーじゃなくなったらAssert
  GFL_ASSERT( item::ITEM_GetWazaNo( ITEM_WAZAMASIN10 ) == WAZANO_MEZAMERUPAWAA );

  machine_no = item::ITEM_GetWazaMashineNo( ITEM_WAZAMASIN10 );

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  pml::PokeParty* party = p_gman->GetGameData()->GetPlayerParty();

  //Check
  if( party->GetMemberCount() <= pos )
  {
    GFL_ASSERT(0);
    return 0;
  }
  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

  if ( p_pp == NULL ){
  	//ないとは思うが指定したポケモンが居なければ何も覚えない事にする
    return -1;
  }

  if ( ! p_pp->CheckWazaMachine( machine_no ) ){
  	//めざめるパワーを覚えられなければ負の数を返す
    return -1;
  }

  return p_pp->GetMezapaType();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ボックスが満タンか調べる
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::BoxIsFull( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BoxPokemon* p_boxpoke = p_gdata->GetBoxPokemon();

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  u32 space_cnt = p_boxpoke->GetSpaceCountAll( p_heap );

  return space_cnt == 0;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 手持ちからボックスへ追加
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PokePartyToBox( AMX* amx, const cell * ptr )
{
  u32 pos = ptr[1];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        p_party     = p_gameData->GetPlayerParty();

  Savedata::BoxPokemon* p_boxpoke = p_gameData->GetBoxPokemon();

  pml::pokepara::PokemonParam* p_pp = p_party->GetMemberPointer(pos);

  if ( p_pp == NULL )
  {
    // エラー対処
    GFL_ASSERT(0);
    return false;
  }

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  u32 space_cnt = p_boxpoke->GetSpaceCountAll( p_heap );

  // ボックスがいっぱいなら追加しない
  if ( space_cnt == 0 )
  {
    GFL_ASSERT_MSG( 0, "BoxAddPoke : FAEILD : Box if full \n" );
    return false;
  }

  // 空いてる場所に追加
  pml::pokepara::CoreParam* p_cp = GFL_NEW( p_heap ) pml::pokepara::CoreParam( p_heap );
  u32 ret = p_boxpoke->PutPokemonEmptyTrayAll( p_pp, 0, p_cp );

  GFL_SAFE_DELETE( p_cp );

  // 成功判定
  if ( ret != Savedata::BoxPokemon::PUT_POKEMON_ERROR )
  {
    // 手持ちから消す
    p_party->RemoveMember( pos );
    return true;
  }

  // 失敗
  GFL_ASSERT(0);

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @biref フォルムチェンジ
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokePartyChangeFormNo( AMX* amx, const cell * ptr )
{
  u16 pos     = ptr[1];
  u16 formno  = ptr[2];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();


  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

  p_pp->ChangeFormNo( formno );

  // 図鑑登録「見た」
  p_gameData->GetZukanData()->SetPokeSee( *p_pp );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @biref フォルムチェンジ（ロトム専用）
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokePartyChangeRotomFormNo( AMX* amx, const cell * ptr )
{
  u16 pos       = ptr[1];
  u16 waza_pos  = ptr[2];
  u16 formno    = ptr[3];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();


  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

  //フォルムを変更
  p_pp->ChangeFormNo( formno );
  //すべてのロトム専用技を忘れさせる
  p_pp->RemoveAllRotomWaza();
  //フォルムに対応したロトム専用技を覚えさせる（ノーマルフォルム&わざがない場合は「でんきショック」）
  p_pp->SetRotomWaza( waza_pos );

  // 図鑑登録「見た」
  p_gameData->GetZukanData()->SetPokeSee( *p_pp );
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief フォルムチェンジ（トリミアン専用）
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokePartyChangeTorimianFormNo( AMX* amx, const cell * ptr )
{
  u16 pos     = ptr[1];
  u16 formno  = ptr[2];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();


  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

  PokeTool::form::SetPoodleForm( p_pp, formno );

  // 図鑑登録「見た」
  p_gameData->GetZukanData()->SetPokeSee( *p_pp );

  return NULL;
}

cell AMX_NATIVE_CALL FuncSetInit::Shoki3GetPersonalRnd( AMX* amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();

  return p_gameData->GetMisc()->GetShoki3PersonalRnd();
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ボックス内に指定ポケモンがいるか調べる
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::BoxMonsNoExists( AMX* amx, const cell * ptr )
{
  MonsNo monsno = static_cast<MonsNo>(ptr[1]);
  bool is_with_egg = GFL_BOOL_CAST( ptr[2] );

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BoxPokemon* p_boxpoke = p_gdata->GetBoxPokemon();

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  pml::pokepara::CoreParam* p_cp = GFL_NEW( p_heap ) pml::pokepara::CoreParam( p_heap );

  bool ret = false;

  for ( u32 tray_no = 0; tray_no < Savedata::BOX::TRAY_MAX; ++tray_no )
  {
    for ( u32 pos = 0; pos < Savedata::BoxPokemon::TRAY_POKE_MAX; ++pos )
    {
      // 存在チェック
      if ( p_boxpoke->CheckPokemon( tray_no, pos, p_cp ) == false )
      {
        continue;
      }

      p_boxpoke->GetPokemon( p_cp, tray_no, pos );

      // タマゴチェック
      if ( !is_with_egg && p_cp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
      {
        continue;
      }

      // 一致チェック
      if ( monsno == p_cp->GetMonsNo() )
      {
        ret = true;
        break;
      }

    }
  }

  GFL_SAFE_DELETE( p_cp );

  return ret;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ボックスのバトルチームにポケモンがいるか調べる
 * @note  6つあるチームの中に1匹でもいたらtrueを返します
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::BoxIsBattleTeamIn(AMX* amx, const cell * ptr)
{

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BOX*  pBoxSaveData = p_gdata->GetBOX();
  GFL_ASSERT(pBoxSaveData);

  //  Boxセーブにバトルチームが存在するかチェック
  {
    for (u32 boxTeamIdx = 0; boxTeamIdx < Savedata::BOX::TEAM_MAX; ++boxTeamIdx)
    {
      // バトルチームにポケモンが入っているかどうか
      if (pBoxSaveData->IsTeamPokeIn(boxTeamIdx) == true)
      {
        // バトルチームにポケモンが入っている！
        return true;
      }
    }
  }

  return false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ボックスのポケモン数を取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::BoxGetPokeCount( AMX* amx, const cell * ptr )
{
  int mode = ptr[1];

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::BoxPokemon* p_boxpoke = p_gdata->GetBoxPokemon();

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE )->GetLowerHandle();

  switch( mode )
  {
  case SCR_BOXPOKE_COUNT_NOT_EGG:        // タマゴを含まない
    return p_boxpoke->GetPokemonCountAll( p_heap, false );
  case SCR_BOXPOKE_COUNT_ALL:            // タマゴを含む全てのポケモン
    return p_boxpoke->GetPokemonCountAll( p_heap, true );
  }
  GFL_ASSERT( 0 ); //@check iwasawa 130929 ケア済み
  return 0;
}

//=============================================================================
//
// 技教え
//
//=============================================================================
//--------------------------------------------------------------
/// 技教えID
//==============================================================
enum {
  SKILLTEACH_ID_COALESCENCE_KUSA = 0, ///< 合体技 草
  SKILLTEACH_ID_COALESCENCE_HONOO,    ///< 合体技 炎
  SKILLTEACH_ID_COALESCENCE_MIZU,     ///< 合体技 水

  SKILLTEACH_ID_STRONGEST_KUSA,       ///< 最強技 草
  SKILLTEACH_ID_STRONGEST_HONOO,      ///< 最強技 炎
  SKILLTEACH_ID_STRONGEST_MIZU,       ///< 最強技 水

  SKILLTEACH_ID_DRAGON,               ///< ドラゴン技

  SKILLTEACH_ID_MAX,
};

/* ------------------------------------------------------------------------- */
/**
 * @brief 技教え 教える技NOを取得
 *
 * @param p_pp ポケパラへのポインタ
 * @param mode 教えモード
 *
 * @return 教える技NO
 */
/* ------------------------------------------------------------------------- */
static WazaNo get_teach_waza( pml::pokepara::PokemonParam* p_pp, u32 mode )
{
  switch( mode )
  {
  case SCR_WAZA_OSHIE_MODE_DRAGON:
    return WAZANO_RYUUSEIGUN;
  case SCR_WAZA_OSHIE_MODE_STRONGEST:
    if( p_pp->CheckWazaOshie( SKILLTEACH_ID_STRONGEST_KUSA ) )
    {
      return WAZANO_HAADOPURANTO;
    }
    if( p_pp->CheckWazaOshie( SKILLTEACH_ID_STRONGEST_HONOO ) )
    {
      return WAZANO_BURASUTOBAAN;
    }
    if( p_pp->CheckWazaOshie( SKILLTEACH_ID_STRONGEST_MIZU ) )
    {
      return WAZANO_HAIDOROKANON;
    }
    GFL_ASSERT(0);
    break;
  case SCR_WAZA_OSHIE_MODE_COALESCENCE:
    if( p_pp->CheckWazaOshie( SKILLTEACH_ID_COALESCENCE_KUSA ) )
    {
      return WAZANO_KUSANOTIKAI;
    }
    if( p_pp->CheckWazaOshie( SKILLTEACH_ID_COALESCENCE_HONOO ) )
    {
      return WAZANO_HONOONOTIKAI;
    }
    if( p_pp->CheckWazaOshie( SKILLTEACH_ID_COALESCENCE_MIZU ) )
    {
      return WAZANO_MIZUNOTIKAI;
    }
    GFL_ASSERT(0);
    break;
  case SCR_WAZA_OSHIE_MODE_GARYOUTENSEI:
    return WAZANO_GARYOUTENSEI;
  case SCR_WAZA_OSHIE_MODE_MABOROSHI:
    if ( p_pp->GetMonsNo() == MONSNO_MEROETTA )
    {
      return WAZANO_INISIENOUTA;
    }
    if ( p_pp->GetMonsNo() == MONSNO_KERUDHIO )
    {
      return WAZANO_SINPINOTURUGI;
    }
    GFL_ASSERT(0);
    break;
  case SCR_WAZA_OSHIE_MODE_BORUTEKKAA:
    return WAZANO_BORUTEKKAA;
  default : GFL_ASSERT(0);
  }

  return WAZANO_BURASUTOBAAN;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 技教えチェック
 *
 * @param p_pp ポケパラへのポインタ
 * @param mode 教えモード
 *
 * @return 0:覚えられない 1:覚えられる 2:覚えている
 * @note ScriptFuncSetCommon.hでも使いたいので外部公開
 */
/* ------------------------------------------------------------------------- */
u8 FuncSetInit::CheckWazaOshieMons( pml::pokepara::PokemonParam* p_pp, u32 mode, WazaNo wazano /* = WAZANO_NULL  */ )
{
  switch ( mode )
  {

  case SCR_BP_WAZA_OSHIE_MODE:
    {
      if (p_pp->HaveWaza(wazano))
      {
        return 2; // 覚えている
      }
      else if ( p_pp->CheckBpWazaOshie(wazano) )
      {
        return 1; // 覚えられる
      }
    }
    return 0; // 覚えられない
  case SCR_WAZA_OSHIE_MODE_STRONGEST:
    if ( p_pp->CheckWazaOshie( SKILLTEACH_ID_STRONGEST_KUSA ) )
    {
      break;
    }
    if ( p_pp->CheckWazaOshie( SKILLTEACH_ID_STRONGEST_HONOO ) )
    {
      break;
    }
    if ( p_pp->CheckWazaOshie( SKILLTEACH_ID_STRONGEST_MIZU ) )
    {
      break;
    }
    return 0;

  case SCR_WAZA_OSHIE_MODE_DRAGON:

    if ( p_pp->CheckWazaOshie( SKILLTEACH_ID_DRAGON ) )
    {
      break;
    }
    return 0;

  case SCR_WAZA_OSHIE_MODE_COALESCENCE:
    if ( p_pp->CheckWazaOshie( SKILLTEACH_ID_COALESCENCE_KUSA ) )
    {
      break;
    }
    if ( p_pp->CheckWazaOshie( SKILLTEACH_ID_COALESCENCE_HONOO ) )
    {
      break;
    }
    if ( p_pp->CheckWazaOshie( SKILLTEACH_ID_COALESCENCE_MIZU ) )
    {
      break;
    }
    return 0;

  case SCR_WAZA_OSHIE_MODE_GARYOUTENSEI:
    //画竜点睛を覚えるのはレックウザのみなので、モンスターナンバーでチェックしてしまう
    if ( p_pp->GetMonsNo() == MONSNO_REKKUUZA )
    {
      break;
    }
    return 0;

  case SCR_WAZA_OSHIE_MODE_MABOROSHI:
    //対象はメロエッタとケルディオのみなので、モンスターナンバーでチェックしてしまう
    if ( p_pp->GetMonsNo() == MONSNO_MEROETTA )
    {
      break;
    }
    if ( p_pp->GetMonsNo() == MONSNO_KERUDHIO )
    {
      break;
    }
    return 0;

  case SCR_WAZA_OSHIE_MODE_BORUTEKKAA:
    //ボルテッカーを覚えるのはピカチュウのみなので、モンスターナンバーでチェックしてしまう
    if ( p_pp->GetMonsNo() == MONSNO_PIKATYUU )
    {
      break;
    }
    return 0;
  }

  if ( p_pp->HaveWaza( get_teach_waza( p_pp, mode ) ) == false )
  {
    return 1; // 覚えられる
  }

  return 2; // 覚えている
}

/* ------------------------------------------------------------------------- */
/**
 * @brief なつき度の最大値を取得
 *
 * @param mode 教えモード
 *
 * @return 最大値
 */
/* ------------------------------------------------------------------------- */
static u32 get_teach_limit_familiarity( u32 mode )
{
  GFL_UNUSED( mode );
  switch( mode ){
  case SCR_WAZA_OSHIE_MODE_GARYOUTENSEI:
  case SCR_WAZA_OSHIE_MODE_MABOROSHI:
  case SCR_WAZA_OSHIE_MODE_BORUTEKKAA:
    return 0;
  default:
    break;
  }
  //現状「画竜点睛」と「まぼろしポケモン」以外はなつき度MAXを一律要求する
  return pml::MAX_FAMILIARITY;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 技教え可能かチェック
 *
 * @return 技教え可能か？
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::WazaOshieCheck( AMX * amx, const cell * ptr )
{
  u32 pos = ptr[1];
  u32 mode = ptr[2]; // 技教えモード指定 SCR_WAZA_OSHIE_MODE_DRAGON など
  WazaNo wazano = static_cast<WazaNo>(ptr[3]); // 技NO

  FieldScriptSystem*     p_sys       = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();

  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return false;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

  if ( p_pp == NULL )
  {
    // エラー対処
    return SCR_WAZA_OSHIE_CHECK_RET_POKEMON_NG;
  }
  if ( p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
  {
    // タマゴを選んだ
    return SCR_WAZA_OSHIE_CHECK_RET_EGG_NG;
  }
  else
  {
    u8 ret = CheckWazaOshieMons( p_pp, mode, wazano );
    if (ret == 0)
    {
      // 対象となるポケモンでない
      return SCR_WAZA_OSHIE_CHECK_RET_POKEMON_NG;
    }
    else if (ret == 2)
    {
      // 既に覚えている
      return SCR_WAZA_OSHIE_CHECK_RET_ALREADY_NG;
    }
    else if( p_pp->GetFamiliarity() < get_teach_limit_familiarity( mode ) )
    {
      // なつきが足りない
      return SCR_WAZA_OSHIE_CHECK_RET_NATSUKI_NG;
    }
  }

  // 教えることが出来る！
  return SCR_WAZA_OSHIE_CHECK_RET_OK;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief 教えるワザIDを取得する
 *
 * @return ワザID
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::WazaOshieGetWazaID( AMX * amx, const cell * ptr )
{
  u16 pos = ptr[1];
  u32 mode = ptr[2];

  FieldScriptSystem*     p_sys       = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  pml::PokeParty*        party       = p_gameData->GetPlayerParty();

  if ( pos >= party->GetMemberCount() )
  {
    GFL_ASSERT(0);
    return 0;
  }

  pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(pos);

  return get_teach_waza( p_pp, mode );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  図鑑オープンフラグゲット
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::ZukanGetOpenFlag( AMX* amx, const cell* ptr )
{
  ZukanCtrlType             type = static_cast<ZukanCtrlType>(ptr[1]);

  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  pman = pSys->GetGameManager();

  Savedata::ZukanData::Mode mode = Savedata::ZukanData::MODE_ZENKOKU;
  switch(type){
  case ZUKANCTRL_TYPE_ZENKOKU:  //全国図鑑
    mode = Savedata::ZukanData::MODE_ZENKOKU;
    break;
  case ZUKANCTRL_TYPE_ALOLA:      //アローラ図鑑
    mode = Savedata::ZukanData::MODE_LOCAL_A;
    break;
  case ZUKANCTRL_TYPE_ALOLA_I01: //アローラ島１ (メレメレ)
    mode = Savedata::ZukanData::MODE_LOCAL_B;
    break;
  case ZUKANCTRL_TYPE_ALOLA_I02: //アローラ島2 (アーカラ)
    mode = Savedata::ZukanData::MODE_LOCAL_C;
    break;
  case ZUKANCTRL_TYPE_ALOLA_I03: //アローラ島3 (ウラウラ)
    mode = Savedata::ZukanData::MODE_LOCAL_D;
    break;
  case ZUKANCTRL_TYPE_ALOLA_I04: //アローラ島4 (ポニ)
    mode = Savedata::ZukanData::MODE_LOCAL_E;
    break;
  }

  return pman->GetGameData()->GetZukanData()->GetZukanOpenFlag( mode );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 図鑑の指定モードの数を取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZukanGetCount( AMX* amx, const cell * ptr )
{
  //Param
  u32           mode = ptr[1];
  ZukanCtrlType type = static_cast<ZukanCtrlType>(ptr[2]);

  //Alias
  FieldScriptSystem*     p_sys       = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  Savedata::ZukanData*   p_zukan     = p_gameData->GetZukanData();
  gfl2::heap::HeapBase*  p_heap      = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  //ExtendData
  PokeTool::ExtendData* p_pokeExtend = GFL_NEW( p_heap ) PokeTool::ExtendData( p_heap );
  p_pokeExtend->Load(p_heap);

  //LocalArea
  PokeTool::ExtendData::LocalArea LocalAreaArr[] = {
    PokeTool::ExtendData::LOCAL_AREA_NONE,  // <= ZUKANCTRL_TYPE_ZENKOKU
    PokeTool::ExtendData::LOCAL_AREA_A,  // <= ZUKANCTRL_TYPE_ALOLA
    PokeTool::ExtendData::LOCAL_AREA_B,  // <= ZUKANCTRL_TYPE_ALOLA_I01
    PokeTool::ExtendData::LOCAL_AREA_C,  // <= ZUKANCTRL_TYPE_ALOLA_I02
    PokeTool::ExtendData::LOCAL_AREA_D,  // <= ZUKANCTRL_TYPE_ALOLA_I03
    PokeTool::ExtendData::LOCAL_AREA_E,  // <= ZUKANCTRL_TYPE_ALOLA_I04
  };

  u16 count = 0;
  switch(mode){
  case ZUKANCTRL_MODE_SEE://見た数
    {
      if(type == ZUKANCTRL_TYPE_ZENKOKU){//全国
        count = p_zukan->GetPokeSeeCount();
      }else{//地方
        PokeTool::ExtendData::LocalArea local_area = LocalAreaArr[type];
        count = p_zukan->GetLocalPokeSeeCount(p_pokeExtend, local_area);
      }
    }
    break;
  case ZUKANCTRL_MODE_GET://捕まえた数
    {
      if(type == ZUKANCTRL_TYPE_ZENKOKU){//全国
        count = p_zukan->GetPokeGetCount();
      }else{//地方
        PokeTool::ExtendData::LocalArea local_area = LocalAreaArr[type];
        count = p_zukan->GetLocalPokeGetCount(local_area, p_pokeExtend);
      }
    }
    break;
  default:
    GFL_ASSERT_MSG(0, "ZukanGetCount : Invalid Mode = %d \n", mode );
    break;
  }

  GFL_SAFE_DELETE( p_pokeExtend );

  return count;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 捕獲チェックコマンド
 *
 * @param monsno モンスター番号
 * @param formno フォルム番号（-1ならmonsnoのみで確認）
 *
 * @return 捕獲しているならTrue
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZukanCheckGetFlag( AMX* amx, const cell * ptr )
{
  //Param
  MonsNo monsno = static_cast<MonsNo>(ptr[1]);

  //Alias
  FieldScriptSystem*     p_sys       = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  Savedata::ZukanData*   p_zukan     = p_gameData->GetZukanData();

  return p_zukan->GetPokeGetFlag(monsno);
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 見たチェックコマンド
 *
 * @param monsno モンスター番号
 * @param formno フォルム番号（-1ならmonsnoのみで確認）
 *
 * @return 見ているならTrue
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZukanCheckSeeFlag( AMX* amx, const cell * ptr )
{
  //Param
  MonsNo monsno = static_cast<MonsNo>(ptr[1]);
  FormNo formno = static_cast<FormNo>(ptr[2]);
  bool use_form = (0 <= static_cast<s32>(ptr[2]));

  //Alias
  FieldScriptSystem*     p_sys       = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  Savedata::ZukanData*   p_zukan     = p_gameData->GetZukanData();

  if(! use_form){
    //フォルムは考慮しない
    return p_zukan->GetPokeSeeFlag(monsno);
  }else{
    //指定されたフォルムを見ているか
    return p_zukan->GetPokeFormSeeFlag(monsno, formno);
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 図鑑のコンプリート状況を取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZukanIsComplete( AMX* amx, const cell * ptr )
{
  //Param
  u32           mode = ptr[1];
  ZukanCtrlType type = static_cast<ZukanCtrlType>(ptr[2]);

  //Alias
  FieldScriptSystem*     p_sys       = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  Savedata::ZukanData*   p_zukan     = p_gameData->GetZukanData();
  gfl2::heap::HeapBase*  p_heap      = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  //ExtendData
  PokeTool::ExtendData* p_pokeExtend = GFL_NEW( p_heap ) PokeTool::ExtendData( p_heap );
  p_pokeExtend->Load(p_heap);

  //LocalArea
  PokeTool::ExtendData::LocalArea LocalAreaArr[] = {
    PokeTool::ExtendData::LOCAL_AREA_NONE,  // <= ZUKANCTRL_TYPE_ZENKOKU
    PokeTool::ExtendData::LOCAL_AREA_A,  // <= ZUKANCTRL_TYPE_ALOLA
    PokeTool::ExtendData::LOCAL_AREA_B,  // <= ZUKANCTRL_TYPE_ALOLA_I01
    PokeTool::ExtendData::LOCAL_AREA_C,  // <= ZUKANCTRL_TYPE_ALOLA_I02
    PokeTool::ExtendData::LOCAL_AREA_D,  // <= ZUKANCTRL_TYPE_ALOLA_I03
    PokeTool::ExtendData::LOCAL_AREA_E,  // <= ZUKANCTRL_TYPE_ALOLA_I04
  };

  bool is_complete = false;
  switch(mode){
  case ZUKANCTRL_MODE_SEE://見た数
    {
      if(type == ZUKANCTRL_TYPE_ZENKOKU){//全国
        is_complete = p_zukan->CheckZenkokuComp();
      }else{//地方
        PokeTool::ExtendData::LocalArea local_area = LocalAreaArr[type];
        is_complete = p_zukan->CheckLocalSeeComp(local_area, p_pokeExtend);
      }
    }
    break;
  case ZUKANCTRL_MODE_GET://捕まえた数
    {
      if(type == ZUKANCTRL_TYPE_ZENKOKU){//全国
        is_complete = p_zukan->CheckZenkokuComp();
      }else{//地方
        PokeTool::ExtendData::LocalArea local_area = LocalAreaArr[type];
        is_complete = p_zukan->CheckLocalGetComp(local_area, p_pokeExtend);
      }
    }
    break;
  default:
    GFL_ASSERT_MSG(0, "ZukanIsComplete : Invalid Mode = %d \n", mode );
    break;
  }

  GFL_SAFE_DELETE( p_pokeExtend );

  return is_complete;
}

//=============================================================================
//
//        トレーナー
//
//=============================================================================

/* ------------------------------------------------------------------------- */
/**
 * @brief トレーナーIDからバトルルールを取得(同期読み込みするため利用箇所注意)
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::TrainerBattleGetRule( AMX* amx, const cell * ptr )
{
  trainer::TrainerID trid = static_cast<trainer::TrainerID>(ptr[1]);
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  trainer::TRAINER_DATA td;
  trainer::LoadTrainerData( p_heap, trid, &td );

  return td.fight_type;
}

//=============================================================================
//
//    自分のデータ取得・設定
//
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief フィールドメニューをセット
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PlayerSetFieldMenu( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman  = p_sys->GetGameManager();
  GameSys::GameData*     p_gdata = p_gman->GetGameData();
  Savedata::FieldMenu*   p_fmenu_sv = p_gdata->GetFieldMenu();
  FieldMenuIconID icon_req = static_cast<FieldMenuIconID>(ptr[1]);
  Savedata::FieldMenu::IconID icon_id;

  switch(icon_req)
  {
  case FMENU_ICON_ID_POKE_LIST:  //!< ポケモンリスト
    icon_id = Savedata::FieldMenu::ICON_ID_POKE_LIST;
    break;
  case FMENU_ICON_ID_ZUKAN:          //!< 図鑑
    icon_id = Savedata::FieldMenu::ICON_ID_ZUKAN;
    break;
  case FMENU_ICON_ID_BAG:            //!< バッグ
    icon_id = Savedata::FieldMenu::ICON_ID_BAG;
    break;
  case FMENU_ICON_ID_TR_CARD:        //!< トレーナーカード
    icon_id = Savedata::FieldMenu::ICON_ID_TR_CARD;
    break;
  case FMENU_ICON_ID_REPORT:         //!< レポート
    icon_id = Savedata::FieldMenu::ICON_ID_ALBUM;
    break;
  case FMENU_ICON_ID_CONFIG:         //!< コンフィグ
    icon_id = Savedata::FieldMenu::ICON_ID_CONFIG;
    break;
  case FMENU_ICON_ID_POKE_PLANET:    //!< ポケプラネット
    icon_id = Savedata::FieldMenu::ICON_ID_POKE_PLANET;
    break;
  case FMENU_ICON_ID_POKE_PARURE:    //!< ポケパルレ
    icon_id = Savedata::FieldMenu::ICON_ID_POKE_PARURE;
    break;
  case FMENU_ICON_ID_NET_PLAY:       //!< 通信で遊ぶ
    icon_id = Savedata::FieldMenu::ICON_ID_NET_PLAY;
    break;
  case FMENU_ICON_ID_JOIN_FESTA:     //!< ジョインフェスタ
    icon_id = Savedata::FieldMenu::ICON_ID_JOIN_FESTA;
    break;
  case FMENU_ICON_ID_BATTLE_VIDEO:   //!< バトルビデオ
    icon_id = Savedata::FieldMenu::ICON_ID_BATTLE_VIDEO;
    break;
  case FMENU_ICON_ID_QR_READ:        //!< QR読み込み
    icon_id = Savedata::FieldMenu::ICON_ID_QR_READ;
    break;
  case FMENU_ICON_ID_ALBUM:        //!< アルバム
    icon_id = Savedata::FieldMenu::ICON_ID_ALBUM;
    break;

  default: 
    GFL_ASSERT( 0 );
    return false;
  }

  if( !p_fmenu_sv->IsMenuIconEnable(icon_id) ){
    p_fmenu_sv->SetMenuIconID(icon_id);
  }
  //フィールドマップがある場合、下画面再構築
  {
    Fieldmap* p_fieldmap = p_gman->GetFieldmap();
    if( p_fieldmap != NULL ){
      Field::SubScreen::FieldSubScreenProcManager* p_subsreen = p_fieldmap->GetFieldSubScreenManager();
      p_subsreen->RemakeFrame();    
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief フィールドメニューの状態を取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PlayerGetFieldMenu( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman  = p_sys->GetGameManager();
  GameSys::GameData*     p_gdata = p_gman->GetGameData();
  Savedata::FieldMenu*   p_fmenu_sv = p_gdata->GetFieldMenu();
  FieldMenuIconID icon_req = static_cast<FieldMenuIconID>(ptr[1]);
  Savedata::FieldMenu::IconID icon_id;

  switch(icon_req)
  {
  case FMENU_ICON_ID_POKE_LIST:  //!< ポケモンリスト
    icon_id = Savedata::FieldMenu::ICON_ID_POKE_LIST;
    break;
  case FMENU_ICON_ID_ZUKAN:          //!< 図鑑
    icon_id = Savedata::FieldMenu::ICON_ID_ZUKAN;
    break;
  case FMENU_ICON_ID_BAG:            //!< バッグ
    icon_id = Savedata::FieldMenu::ICON_ID_BAG;
    break;
  case FMENU_ICON_ID_TR_CARD:        //!< トレーナーカード
    icon_id = Savedata::FieldMenu::ICON_ID_TR_CARD;
    break;
  case FMENU_ICON_ID_REPORT:         //!< レポート
    icon_id = Savedata::FieldMenu::ICON_ID_ALBUM;
    break;
  case FMENU_ICON_ID_CONFIG:         //!< コンフィグ
    icon_id = Savedata::FieldMenu::ICON_ID_CONFIG;
    break;
  case FMENU_ICON_ID_POKE_PLANET:    //!< ポケプラネット
    icon_id = Savedata::FieldMenu::ICON_ID_POKE_PLANET;
    break;
  case FMENU_ICON_ID_POKE_PARURE:    //!< ポケパルレ
    icon_id = Savedata::FieldMenu::ICON_ID_POKE_PARURE;
    break;
  case FMENU_ICON_ID_NET_PLAY:       //!< 通信で遊ぶ
    icon_id = Savedata::FieldMenu::ICON_ID_NET_PLAY;
    break;
  case FMENU_ICON_ID_JOIN_FESTA:     //!< ジョインフェスタ
    icon_id = Savedata::FieldMenu::ICON_ID_JOIN_FESTA;
    break;
  case FMENU_ICON_ID_BATTLE_VIDEO:   //!< バトルビデオ
    icon_id = Savedata::FieldMenu::ICON_ID_BATTLE_VIDEO;
    break;
  case FMENU_ICON_ID_QR_READ:        //!< QR読み込み
    icon_id = Savedata::FieldMenu::ICON_ID_QR_READ;
    break;
  case FMENU_ICON_ID_ALBUM:        //!< マイアルバム
    icon_id = Savedata::FieldMenu::ICON_ID_ALBUM;
    break;
  default: 
    GFL_ASSERT( 0 );
    return false;
  }
  return p_fmenu_sv->IsMenuIconEnable(icon_id);
}


//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤーの性別を取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PlayerGetSex( AMX * amx, const cell * ptr )
{
  FieldScriptSystem*    p_sys       = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman      = p_sys->GetGameManager();
  GameSys::GameData*    p_gameData  = p_gman->GetGameData();
  Savedata::MyStatus*   p_status    = p_gameData->GetPlayerStatus();

  // 性別を返す
  return p_status->GetSex();
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤーのおこづかいを取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PlayerGetMoney( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();

  return p_gameData->GetMisc()->GetGold();
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤーのおこづかいをセット
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PlayerSetMoney( AMX * amx, const cell * ptr )
{
  u32 money = ptr[1];
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();

  p_gameData->GetMisc()->SetGold(money);
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤーのおこづかいをセット
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PlayerAddMoney( AMX * amx, const cell * ptr )
{
  u32 money = ptr[1];
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();

  p_gameData->GetMisc()->AddGold(money);
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤーのおこづかいをセット
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PlayerSubMoney( AMX * amx, const cell * ptr )
{
  u32 money = ptr[1];
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();

  p_gameData->GetMisc()->SubGold(money);
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤーの所持BPを取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PlayerGetBP( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman   = p_sys->GetGameManager();
  GameSys::GameData*     p_gdata  = p_gman->GetGameData();

  return p_gdata->GetMisc()->GetBP();
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤーの所持BPをセット
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PlayerSetBP( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gdata  = p_gman->GetGameData();

  return p_gdata->GetMisc()->SetBP( ptr[1] );
}

//-----------------------------------------------------------------------------
/**
 * @brief ゾーンID取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PlayerGetZoneID( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  ZoneID zoneID =  p_gman->GetGameData()->GetStartLocation()->zoneId;

  return zoneID;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief プレイヤーのIDを収得
 *
 * @param amx
 * @param ptr
 *
 * @return
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PlayerGetID( AMX * amx, const cell * ptr )
{
  u16 is_low = ptr[1];
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager * p_gman   = p_sys->GetGameManager();
  GameSys::GameData *    p_gdata  = p_gman->GetGameData();
  Savedata::MyStatus *      p_status = p_gdata->GetPlayerStatus();

  if (is_low)
  {
    return p_status->GetIDLow();
  }

  return p_status->GetID();
}

/* ------------------------------------------------------------------------- */
/**
 * @brief トレーナーパス　スタンプをセット
 * @param amx
 * @param ptr
 * @return
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PlayerSetTrainerPassStamp( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager * p_gman  = p_sys->GetGameManager();
  Savedata::Misc *       p_misc  = p_gman->GetGameData()->GetMisc();

  Savedata::TrainerPassStampID stamp_id = static_cast<Savedata::TrainerPassStampID>(ptr[1]);

  p_misc->SetTrainerPassStamp(stamp_id);
  return true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief トレーナーパス　スタンプをリセット
 * @param amx
 * @param ptr
 * @return
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PlayerResetTrainerPassStamp( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager * p_gman  = p_sys->GetGameManager();
  Savedata::Misc *       p_misc  = p_gman->GetGameData()->GetMisc();

  Savedata::TrainerPassStampID stamp_id = static_cast<Savedata::TrainerPassStampID>(ptr[1]);

  p_misc->ResetTrainerPassStamp(stamp_id);
  return true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief トレーナーパス　スタンプをチェック
 * @param amx
 * @param ptr
 * @return
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PlayerCheckTrainerPassStamp( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager * p_gman  = p_sys->GetGameManager();
  Savedata::Misc *       p_misc  = p_gman->GetGameData()->GetMisc();

  Savedata::TrainerPassStampID stamp_id = static_cast<Savedata::TrainerPassStampID>(ptr[1]);

  return p_misc->IsTrainerPassStamp(stamp_id);
}


/* ------------------------------------------------------------------------- */
/**
 *　@brief 戦闘開始時にプレイヤーがボールを投げるモーションスタイルを設定
 *  @param type BallThrowTypeを設定
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PlayerBallThrowTypeSet( AMX * amx, const cell * ptr )
{
  //Param
  Savedata::BallThrowType type = static_cast<Savedata::BallThrowType>(ptr[1]);

  //Alias
  FieldScriptSystem*    p_sys    = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman   = p_sys->GetGameManager();
  GameSys::GameData*    p_gdata  = p_gman->GetGameData();
  Savedata::MyStatus*   p_status = p_gdata->GetPlayerStatus();

  //Set
  p_status->SetBallThrowType(type);

  return 0;
}


/* ------------------------------------------------------------------------- */
/**
 *　@brief 戦闘開始時にプレイヤーがボールを投げるモーションスタイルを取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PlayerBallThrowTypeGet( AMX * amx, const cell * ptr )
{
  //Alias
  FieldScriptSystem*    p_sys    = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman   = p_sys->GetGameManager();
  GameSys::GameData*    p_gdata  = p_gman->GetGameData();
  Savedata::MyStatus*   p_status = p_gdata->GetPlayerStatus();

  //Get
  return p_status->GetBallThrowType();
}



//-----------------------------------------------------------------------------
/**
 * @brief ゾーンIDから地名ID取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::GetPlaceNameIDFormZoneID( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();

  u16 zone_id = ptr[3];
  ZoneDataAccessor zoneDataAccessor( p_gman->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( zone_id ) );

  return zoneDataAccessor.GetPlaceLabelID();
}


//-----------------------------------------------------------------------------
/**
 * @brief レコード取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::RecordGet( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  Savedata::Record*         p_record    = p_gameData->GetRecordSaveData();
  Savedata::Record::RECORD_ID id =  static_cast<Savedata::Record::RECORD_ID>(ptr[1]);
  return Savedata::GetRecord( id );
}

//-----------------------------------------------------------------------------
/**
 * @brief レコード加算
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::RecordAdd( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  Savedata::Record*         p_record    = p_gameData->GetRecordSaveData();
  Savedata::Record::RECORD_ID id =  static_cast<Savedata::Record::RECORD_ID>(ptr[1]);
  u32 value =     ptr[2];
  Savedata::AddRecord( id, value );
  if (id == Savedata::Record::RECID_RECOVERY_PC )
  {
    App::Tool::RotomFriendship::AddCharacterCounter(Savedata::FieldMenu::ROTOM_CHAR_OPTIMISTIC, value);
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief レコード設定
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::RecordSet( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman      = p_sys->GetGameManager();
  GameSys::GameData*     p_gameData  = p_gman->GetGameData();
  Savedata::Record*         p_record    = p_gameData->GetRecordSaveData();
  Savedata::Record::RECORD_ID id =  static_cast<Savedata::Record::RECORD_ID>(ptr[1]);
  u32 value =     ptr[2];
  Savedata::SetRecord( id, value );
  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 全力リングフラグを設定
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZenryokuRingFlagSet( AMX * amx, const cell * ptr )
{
  bool flag = GFL_BOOL_CAST(ptr[1]);
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  Savedata::MyStatus* p_status = p_gman->GetGameData()->GetPlayerStatus();
  p_status->SetZenryokuRing(flag);
  return flag;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief メガリングフラグ取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZenryokuRingFlagGet( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  Savedata::MyStatus* p_status = p_gman->GetGameData()->GetPlayerStatus();
  return  p_status->HasZenryokuRing();
}

/* ------------------------------------------------------------------------- */
/**
 * @brief メガリングフラグ操作
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::MegaRingFlagSet( AMX * amx, const cell * ptr )
{
  bool flag = GFL_BOOL_CAST(ptr[1]);

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  Savedata::MyStatus* p_status = p_gman->GetGameData()->GetPlayerStatus();

  p_status->SetMegaRing( flag );

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief メガリングフラグ取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::MegaRingFlagGet( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  Savedata::MyStatus* p_status = p_gman->GetGameData()->GetPlayerStatus();

  return p_status->HasMegaRing();
}


/* ------------------------------------------------------------------------- */
/**
 * @brief しまめぐりのしるし設定
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ShimameguriNoShirushiFlagSet( AMX * amx, const cell * ptr )
{
  bool flag = GFL_BOOL_CAST(ptr[1]);

  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  Savedata::MyStatus* p_status = p_gman->GetGameData()->GetPlayerStatus();

  p_status->SetShimameguriNoShirushi(flag);

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief しまめぐりのしるし取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ShimameguriNoShirushiFlagGet( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  Savedata::MyStatus* p_status = p_gman->GetGameData()->GetPlayerStatus();

  return p_status->HasShimameguriNoShirushi();
}


//-----------------------------------------------------------------------------
/**
 * @brief ポケモンダイアリー　セット
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokeDiarySet( AMX * amx, const cell * ptr )
{
  PokeDiary::Set( static_cast<PDID>(ptr[1]), ptr[2], ptr[3], ptr[4] );
  return NULL;
}



//-----------------------------------------------------------------------------
/**
 *  @brief  手持ち配布ポケモンチェック
 *
 *  @param[in]  チェックするポケモンナンバー
 *
 *  @retval true で配布ポケモン
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokePartyCheckDistribution(AMX* amx, const cell * ptr)
{
  u16 target_monsno = ptr[1];

  FieldScriptSystem*     p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman  = p_sys->GetGameManager();
  GameSys::GameData*     p_gdata = p_gman->GetGameData();
  pml::PokeParty*        party   = p_gdata->GetPlayerParty();

  int num = party->GetMemberCount();

  for ( int i=0; i<num;i++ )
  {
    pml::pokepara::PokemonParam* p_pp = party->GetMemberPointer(i);

    //モンスターナンバーチェック
    if ( target_monsno != p_pp->GetMonsNo() ) continue; //モンスターナンバーが違う場合はスキップ

    //タマゴチェック
    if( p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ) continue; //タマゴの場合処理をスキップ

    //タマゴから孵化したかのチェック
    if ( p_pp->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_PLACE ) != 0 ) continue; //タマゴ捕獲場所１が０でなければスキップ
    //配布フラグのチェック
    if ( !p_pp->GetEventPokeFlag() ) continue; //配布フラグが立っていなければ、スキップ

    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  QRマギアナイベントが有効かチェック
 *  @retval true 有効
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::MagianaQREventEnableCheck(AMX* amx, const cell * ptr)
{
  FieldScriptSystem*     p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman  = p_sys->GetGameManager();
  GameSys::GameData*     p_gdata = p_gman->GetGameData();

  //trueならマギアナQR読み込み済み
  if( p_gdata->GetQRReaderSave()->IsReadMagianaQR() == false )
  {
    return false;
  }
  //ゲームクリアしたか？
  if( p_gdata->GetEventWork()->CheckEventFlag( SYS_FLAG_GAME_CLEAR ) == false )
  {
    return false;
  }
  //マギアナイベントを消化済みか？
  if( p_gdata->GetEventWork()->CheckEventFlag( SYS_FLAG_MAGIANA_CLEAR ) != false )
  {
    return false;
  }
  return true;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  M20ピカチュウイベントが有効かチェック
 *  @retval true 有効
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::M20PikatyuQREventEnableCheck(AMX* amx, const cell * ptr)
{
  FieldScriptSystem*     p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman  = p_sys->GetGameManager();
  GameSys::GameData*     p_gdata = p_gman->GetGameData();

  //trueならM20PikatyuQR読み込み済み
  if( !p_gdata->GetQRReaderSave()->IsReadM20PikaQR() )
  {
    return false;
  }
  //M20Pikatyuイベント許可条件を満たしていない（島2未到達）
  if( !p_gdata->GetEventWork()->CheckEventFlag( SYS_FLAG_SATOPIKA_OPEN ) )
  {
    return false;
  }
  //M20Pikatyuイベント消化済みか
  if( p_gdata->GetEventWork()->CheckEventFlag( SYS_FLAG_SATOPIKA_CLEAR ) )
  {
    return false;
  }
  return true;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief 無線通信チェック
 */
/* -----------------------  -------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::CheckWLANSwitch( AMX* amx, const cell * ptr )
{
#ifdef  GF_PLATFORM_CTR
  return NetLib::NijiNetworkSystem::IsWLANSwitchEnable();
#else  //GF_PLATFORM_WIN
  return false;
#endif
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 指定ゾーンに関連したフライフラグをセットする
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::FlyFlagSet( AMX * amx, const cell * ptr )
{
  u16 zone_id = ptr[1];

  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = pSys->GetGameManager();
  EventWork * p_eventwork = p_gman->GetGameData()->GetEventWork();

  WarpDataTools         *pWarpData = p_gman->GetGameData()->GetWarpDataTools();
  pWarpData->SetFlyFlags( p_eventwork, zone_id );

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ワープIDのセット
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::WarpIDSet( AMX * amx, const cell * ptr )
{
  u16 warp_id = ptr[1];

  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = pSys->GetGameManager();
  WarpDataTools         *pWarpData = p_gman->GetGameData()->GetWarpDataTools();

  if ( pWarpData->IsWarpIDOK( warp_id ) )
  {
    p_gman->GetGameData()->SetWarpID( warp_id );
  }
  return NULL;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief エスケープロケーション(穴抜けの脱出先)の明示的記録 
 *
 * @param zone_id 脱出先ゾーンID
 * @param gx      脱出先GX
 * @param y       脱出先Y(floatなので注意)
 * @param gz      脱出先GZ
 * @param dir     脱出時の向き
 * @param form    脱出時のフォルム(デフォルトでは-1で現在のプレイヤーフォルムを指定する設定となる)
 *
 * @note  スクリプトからのMapChangeコマンドではエスケープロケーション記録は行われない
 *
 *        MapChange系コマンドのnative処理側で記録を行わない理由は
 *        ・スクリプトイベントによるマップチェンジではエスケープ先記憶をすべきでない場合のほうが多い
 *        ・MapChangeが呼び出されてる場所・タイミングは不定なので、現在位置やEntranceLocationを復帰座標に使えない
 *
 * @note  地形条件が特殊等でドア接続の代わりにPOSイベントからのMapChangeを行う場合で
 *        エスケープポイント記録が必要な場合に明示的に呼び出す
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::SetEscapeLocation( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = pSys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Field::Location location;

  u16 zone_id = ptr[1];

  gfl2::math::Vector3 pos(amx_ctof(ptr[2]), amx_ctof(ptr[3]), amx_ctof(ptr[4]));
  float angY = amx_ctof(ptr[5]);
  s32 form = ptr[6];

  RIDE_POKEMON_ID ride_id = RIDE_NO_POKEMON; //ライド無し

  switch( form ){
  case PLAYER_FORM_RIDE_ROBA2:
    ride_id = RIDE_ROBA2;
    break;
  case PLAYER_FORM_RIDE_SAMEHADAA:
    ride_id = RIDE_SAMEHADA;
    break;
  case PLAYER_FORM_RIDE_RAPURASU:
    ride_id = RIDE_LAPLACE;
    break;
  default:
    break;
  }
  
  /// 回転.
  gfl2::math::Quaternion quat;
  quat.RadianYXZToQuaternion(0.0, gfl2::math::ConvDegToRad(angY), 0.0);

  location.Initialize();
  location.Set( p_gdata->GetFieldZoneDataLoader()->GetZoneWorldId(zone_id),  zone_id, pos, quat );

  p_gdata->SetEscapeLocation( &location, ride_id );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        IsJoinFestaReward
 * @brief       ジョインフェスタご褒美イベントチェック
 * @author      ichiraku_katsuhiko
 * @date        2015/11/16 16:41
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::IsJoinFestaReward(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Savedata::JoinFestaDataSave* pSave = p_gman->GetGameData()->GetJoinFestaData();

  return pSave->IsRewardEvent();
}

//------------------------------------------------------------------------------
/**
 * @func        IsJoinFestaAttractionOpen
 * @brief       ジョインフェスタのアトラクションが開放されているかチェック
 * @author      ichiraku_katsuhiko
 * @date        2015/11/17 16:41
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::IsJoinFestaAttractionOpen(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Savedata::JoinFestaDataSave* pSave = p_gman->GetGameData()->GetJoinFestaData();

  if( pSave->GetRewardState( JoinFestaScript::REWARD_ATTRACTION ) == JoinFestaScript::REWARD_STATE_FINISH )
  {
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        IsJoinFestaAttractionPlay
 * @brief       ジョインフェスタがアトラクション中かチェック
 * @author      ichiraku_katsuhiko
 * @date        2015/11/17 16:41
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::IsJoinFestaAttractionPlay(AMX * amx, const cell * ptr )
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);
  return pAttractionManager->IsPlaying();
}

/* ------------------------------------------------------------------------- */
/**
 * @brief JFのフィールド出現用パーソナルリストを再作成する
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::RecreateFieldPersonalList( AMX* amx, const cell * ptr )
{
  ScriptObject              *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager      *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem         *p_sys    = p_gman->GetFieldScriptSystem();

  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::JoinFestaRecreateFieldPersonalListEvent>( p_gman->GetGameEventManager() );
  
  p_sys->Suspend();
  
  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief JFのアトラクション後のライド降り
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::JoinFestaAttractionRideOff( AMX* amx, const cell * ptr )
{
  ScriptObject              *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager      *p_gman   = p_obj->GetGameManager();

  Field::MoveModel::FieldMoveModelManager* pMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pMoveModelPlayer  = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  // ライド乗っているか
  if( pMoveModelPlayer->IsRideMoveModelPrevClearResource() )
  {
    // ライド状態を解消
    pMoveModelPlayer->InitRideMoveModelHeader();
    pMoveModelManager->TerminateMoveModelWorkResource( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON );
  }
  
  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief JFのリクルーターリセット
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::JoinFestaResetRecruiter( AMX* amx, const cell * ptr )
{
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ResetRecruiterPersonal();
  return NULL;
}


//-----------------------------------------------------------------------------
/**
 *  @brief 育て屋 タマゴが発生しているか？
 *
 *  その他の育て屋専用コマンドは FuncSetProgField::ScriptFuncSetSodateya.cpp にある
 *  このコマンドのみinitスクリプトで使いたかったため引越しした 130329
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::SodateyaIsEggExist(AMX * amx, const cell * ptr)
{
  Savedata::SodateyaID id = static_cast<Savedata::SodateyaID>(ptr[1]);
  Savedata::Sodateya* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSodateyaSave();

  return pSave->IsEggExist( id );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ジガルデセル取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZigarudeCellGet( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = pSys->GetGameManager();
  EventWork * p_eventwork = p_gman->GetGameData()->GetEventWork();

  return p_eventwork->GetZigarudeCell();
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ジガルデセル追加チェック momijiでは未使用
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZigarudeCellAddCheck( AMX * amx, const cell * ptr )
{
  GFL_ASSERT(0);
  return 0;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ジガルデセル利用チェック momijiでは未使用
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZigarudeCellSubCheck( AMX * amx, const cell * ptr )
{
  GFL_ASSERT(0);
  return 0;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ジガルデセル追加
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZigarudeCellAdd( AMX * amx, const cell * ptr )
{
  u16 add = ptr[1];

  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = pSys->GetGameManager();
  EventWork * p_eventwork = p_gman->GetGameData()->GetEventWork();

  return p_eventwork->AddZigarudeCell( add );  
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ジガルデセル利用
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ZigarudeCellSub( AMX * amx, const cell * ptr )
{
  u16 sub = ptr[1];

  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = pSys->GetGameManager();
  EventWork * p_eventwork = p_gman->GetGameData()->GetEventWork();

  return p_eventwork->SubZigarudeCell( sub );
}


//=========================================================================================
/*
    デバッグ時のみ機能が有効なコマンド群
    コマンドの存在はFinalでも消えないが、機能的には何もしないNopとなる
*/
//=========================================================================================

//------------------------------------------------------------------------------
/**
 * @func        DebugChrEditListAdd_
 * @brief       デバッグ機能 キャラエディットリスト登録
 * @author      Miyuki Iwasawa
 * @date        2015/10/24(土)
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::DebugChrEditListAdd_(AMX * amx, const cell * ptr)
{
#if PM_DEBUG
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  Work                    *p_script_work = p_obj->GetWork();
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  cell*                    text;          // 文字列アドレス取得用
  char                     name[ DEBUG_CHR_EDIT_NAME_LEN ] = {0}; // 登録コード

  u32  event_id          = ptr[2];  //< 登録ターゲットのイベントID
  int  dynamic_mot_id    = ptr[3];  //<非常駐モーションID

  amx_GetAddr( amx, ptr[ 1 ], &text );                  // リスト登録名のアドレスを取得
  amx_GetString( &name[ 0 ], text, 0, sizeof( name ) ); // リスト登録名を保存

  p_script_work->DebugChrEditListEntry(name,event_id,dynamic_mot_id);
  
#endif
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        DebugSetSceneEditMode
 * @brief       デバッグ機能 シーンエディットモードOn/Off
 * @author      Miyuki Iwasawa
 * @date        2015/10/24(土)
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::DebugSetSceneEditMode(AMX * amx, const cell * ptr)
{
#if PM_DEBUG
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();

  bool  enable_f          = GFL_BOOL_CAST(ptr[1]);

  FieldScript::FieldScriptSystem::DebugSceneEditEnableFlag = enable_f;
  
#endif
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        DebugSetSceneEditMode
 * @brief       デバッグ機能 シーンエディットモードOn/Offを取得
 * @author      Miyuki Iwasawa
 * @date        2015/10/24(土)
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::DebugGetSceneEditMode(AMX * amx, const cell * ptr)
{
#if PM_DEBUG
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();

  return FieldScript::FieldScriptSystem::DebugSceneEditEnableFlag;
#else  
  return false;
#endif
}

//------------------------------------------------------------------------------
/**
 * @func        SetNormalAlolaOffset
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::SetNormalAlolaOffset(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  // アローラ時間のセットアップ
  s64 alola_offset = GameSys::AlolaTime::GetAlolaOffset( GET_VERSION(), GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_NORMAL );
  p_gdata->GetGameTime()->SetAlolaOffset( alola_offset );
  
  return 0;
}

cell AMX_NATIVE_CALL FuncSetInit::MathGetDiffAngleAbs(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  f32     angle_a  = gfl2::math::ConvDegToRad( amx_ctof( ptr[1] ) );
  f32     angle_b  = gfl2::math::ConvDegToRad( amx_ctof( ptr[2] ) );
  
  f32 result = gfl2::math::FAbs(gfl2::math::ConvRadToDeg(FuncSetCommon::DiffAngle( angle_a, angle_b )));
  return amx_ftoc(result);
}

cell AMX_NATIVE_CALL FuncSetInit::MathGetTargetAngle(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  gfl2::math::Vector3 pos(0.0f, 0.0f, 0.0f);
  gfl2::math::Vector3 target(0.0f, 0.0f, 0.0f);

  pos.x = amx_ctof( ptr[1] );
  pos.z = amx_ctof( ptr[2] );
  target.x = amx_ctof( ptr[3] );
  target.z = amx_ctof( ptr[4] );
  
  gfl2::math::Vector3 vec( target - pos );

  f32 result(0.0f);
  if (vec.Length() > 0.001)
  {
    result = FuncSetFieldInit::ConvRotationVector3ToYRadian( vec );
    result = gfl2::math::ConvRadToDeg( result );
  }

  return amx_ftoc(result);
}

cell AMX_NATIVE_CALL FuncSetInit::MathGetVectorLength(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  gfl2::math::Vector3 vector(0.0f, 0.0f, 0.0f);

  vector.x = amx_ctof( ptr[1] );
  vector.y = amx_ctof( ptr[2] );
  vector.z = amx_ctof( ptr[3] );
  f32 length = vector.Length();
  return amx_ftoc( length );
}

cell AMX_NATIVE_CALL FuncSetInit::MathGetEasingValue(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  s32 func = ptr[1];
  f32 value = amx_ctof( ptr[2] );
  value = gfl2::math::Easing::GetValue( static_cast<  gfl2::math::Easing::EaseFunc >( func ), 0.0f, 1.0f, value );
  return amx_ftoc( value );
}

cell AMX_NATIVE_CALL FuncSetInit::MathGetVectorX(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  f32 deg = amx_ctof( ptr[1] );

  gfl2::math::Vector3 vec(0.0f, 0.0f, 1.0f);
  vec = gfl2::math::Matrix34::GetRotationY(gfl2::math::ConvDegToRad(deg)).TransformCoord( vec );
  f32 value = vec.x;
  return amx_ftoc( value );
}

cell AMX_NATIVE_CALL FuncSetInit::MathGetVectorZ(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  f32 deg = amx_ctof( ptr[1] );

  gfl2::math::Vector3 vec(0.0f, 0.0f, 1.0f);
  vec = gfl2::math::Matrix34::GetRotationY(gfl2::math::ConvDegToRad(deg)).TransformCoord( vec );
  f32 value = vec.z;
  return amx_ftoc( value );
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )
