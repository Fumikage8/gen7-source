//==============================================================================
/**
 * @file        ScriptFuncSetWindow.cpp
 * @brief       ScriptFuncSetCommon系 メッセージウィンドウ他UI制御系統
    
 * @author      N.Takeda
 * @date        2015/04/13(月) 18:52:48
 */
//==============================================================================

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/SaveData.h"
#include <arc_def_index/arc_def.h>

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/source/EventCheck/FieldEventCheck.h"
#include "Field/FieldRo/include/Encount/FieldEncount.h"
#include "Field/FieldStatic/include/Encount/FieldEncountUtil.h"
#include "Field/FieldStatic/include/PokemonSearch/FieldPokemonSearchDataAccessor.h"

#include "Field/FieldStatic/include/event/EventBattleCall.h"
#include "Field/FieldStatic/include/Event/EventGameOver.h"
#include "Field/FieldStatic/include/Event/EventBattleCallDefine.h"

// trainer
#include "Trainer/Trainer/include/tr_tool.h"
#include "Trainer/Trainer/include/TrainerMessage.h"


// encount_effect
#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"
#include "Field/FieldStatic/include/Encount/FieldEncountEffect.h"
#include "Battle/include/battle_SetupParam.h"


#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "niji_reference_files/script/FieldPawnTypes.h"


#include <niji_conv_header/field/script/ScrOffsetId.h>
#include <arc_def_index/arc_def.h>
#include <arc_index/script_event_data.gaix>
#include <arc_index/message.gaix>
#include <arc_index/script_message.gaix>

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )


//------------------------------------------------------------------------------
/**
 * @func        TrainerIDGet
 * @brief       自身のSCRIDからトレーナーIDを取得(TrainerTalkBattle専用)
 * @author      N.Takeda
 * @date        2015/06/30(火) 19:59:59
 *
 * @param[in,out] AMX*          amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::TrainerIDGet(AMX* amx, const cell * ptr)
{
  Field::FieldScript::FieldScriptSystem *pSys = Field::FieldScript::FieldScriptSystem::GetInstance();
  Field::FieldScript::SCRIPT_ARC_TABLE  *pScriptArcTable = pSys->GetScriptArcTable();
  s32 scrid = ptr[1];
  
  if( scrid < pScriptArcTable[ID_TRAINER_2VS2_INDEX].scrid_start )
  {
    return ( scrid - pScriptArcTable[ID_TRAINER_INDEX].scrid_start ); //0オリジン
  }
  return scrid - pScriptArcTable[ID_TRAINER_2VS2_INDEX].scrid_start; //0オリジン
}


/* ------------------------------------------------------------------------- */
/**
 * @brief 野生戦バトル コール
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::CallWildBattleCore( AMX* amx, const cell * ptr )
{
  u16 tbl_no = ptr[1];
  u32 flag    = ptr[2];
  u32 ret_bgm_id = ptr[3];

  FieldScriptSystem*    p_sys  = FieldScriptSystem::GetInstance();
  ScriptObject*         p_obj  = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager* p_gman = p_obj->GetGameManager();

  GameSys::GameData*      p_game_data       = p_gman->GetGameData();
  Encount::EncountWork*   p_encount_work    = p_game_data->GetEncountWork();
  Encount::EncountEffect* p_encount_effect  = p_encount_work->GetEncountEffect();

  u32 option = 0;
  
  if( flag & WILD_BTL_FLAG_SKIP_EFFECT ){
    option |= EVBTL_CALLOPT_SKIP_ENCEFF;
  }
  if( flag & WILD_BTL_FLAG_SKIP_FIELD_CLOSE )
  {
    option |= EVBTL_CALLOPT_SKIP_ENCEFF;
    option |= EVBTL_CALLOPT_SKIP_FIELD_CLOSE;
  }
  if( flag & WILD_BTL_FLAG_SKIP_FIELD_OPEN )
  {
    option |= EVBTL_CALLOPT_SKIP_FIELD_OPEN;
  }
  if( flag & WILD_BTL_FLAG_SKIP_REFLE )
  {
    option |= EVBTL_CALLOPT_SKIP_REFLE;
  }
  if( flag & WILD_BTL_FLAG_NO_LOSE )
  {
    option |= EVBTL_CALLOPT_NO_LOSE;
  }
  if( flag & WILD_BTL_FLAG_SKIP_BTLBGM )
  {
    option |= EVBTL_CALLOPT_SKIP_BTL_BGM;
  }
  if ( flag & WILD_BTL_FLAG_SKIP_BGM_RECOVER ){
    option |= EVBTL_CALLOPT_SKIP_BGM_RECOVER;
  }
  if( flag & WILD_BTL_FLAG_RUN_ENEMY_TURN )
  {
    option |= EVBTL_CALLOPT_RUN_ENEMY_TURN;
  }
  if( flag & WILD_BTL_FLAG_CAN_NOT_CAPTURE )
  {
    option |= EVBTL_CALLOPT_CAN_NOT_CAPTURE;
  }
  if (flag & WILD_BTL_FLAG_AURA)
  {
    option |= EVBTL_CALLOPT_AURA;
  }
  if (flag & WILD_BTL_FLAG_NO_WILD_MESSAGE)
  {
    option |= EVBTL_CALLOPT_NO_WILD_MESSAGE;
  }

  //特定オプションが有効な際の操作
  if( option & EVBTL_CALLOPT_SKIP_ENCEFF )
  {
    if( flag & WILD_BTL_FLAG_PLAY_FADEOUT )
    {
      option |= EVBTL_CALLOPT_WITH_FADEOUT;
    }
    if( flag & WILD_BTL_FLAG_PLAY_BTLBGM )
    {
      option |= EVBTL_CALLOPT_PLAY_BTL_BGM;
    }
  }
  if( p_encount_effect->IsInitialized() )
  {
    option |= EVBTL_CALLOPT_ENCEFF_EXTERNAL;
  }

  Encount::Encount::CallSymbolEncount( p_gman, tbl_no , option, ret_bgm_id );

  // ウェイト
  p_sys->Suspend();

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  野生戦　敗戦処理
 *          この後すぐSuspendする。
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChangeEventWildLose_(AMX *, const cell *)
{
  GameSys::GameEvent* call_event;
  FieldScriptSystem*    p_sys  = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameEventManager* p_evman = p_gman->GetGameEventManager();

  call_event = GFL_NEW(p_evman->GetHeap()) EventNormalLose(p_evman->GetHeap());
  p_sys->EntryNextEvent(call_event);

  {
#if 0
    SystemWork* pSys = p_sys->GetSystemWork();
    
    //戦闘負け時はフラグOff処理だけでいい by nozomu_saito
    if( pSys->CheckBit( SCR_END_CHECK_BIT_SP_MOTION )){
      pSys->OffCheckBit( SCR_END_CHECK_BIT_SP_MOTION );
    }
    /*
     * BGMStartEvent()による環境音ポーズの後処理はEventBattleCall内の
     * FieldSound::EndBattleBGMForGameOver()でフォローしているので、ここではチェックBitのみOffする
     */
    if( pSys->CheckBit( SCR_END_CHECK_BIT_EVENT_BGM )){
      pSys->OffCheckBit( SCR_END_CHECK_BIT_EVENT_BGM );
    }
#endif
  }

  p_sys->ForceEnd();
  p_sys->Suspend();

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  野生戦　結果チェック
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetWildBattleResult(AMX *, const cell *)
{
  FieldScriptSystem*    p_sys  = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  const GameSys::GameData* cp_gdata = p_gman->GetGameData();

  BtlResult result = cp_gdata->GetLastBattleResult();

  return EventBattleCall::GetWildBattleResult( result );
}

//----------------------------------------------------------------------------
/**
 *  @brief  あまいミツ　野生戦が発生するかチェック
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::CheckAmaimitsuEncountDataExist( AMX*, const cell* )
{
  FieldScriptSystem*    p_sys  = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  const GameSys::GameData* cp_gdata = p_gman->GetGameData();

  FieldEventCheck::CalcCollisionEncount( *cp_gdata->GetFieldCharaModelManager(), *p_gman->GetFieldResident()->GetCollisionScene( FieldResident::COLLISION_SCENE_ENCOUNT ) );
  u32 encount = FieldEventCheck::CheckEncount( cp_gdata->GetFieldCharaModelManager()->GetFieldMoveModel( MoveModel::FIELD_MOVE_MODEL_PLAYER ) );

  // @fix GFNMCat[1261] 試練中はエンカウント禁止
  const ZoneDataLoader* pZoneDataLoader = cp_gdata->GetFieldZoneDataLoader();
  ZoneID zoneID = p_gman->GetFieldmap()->GetZoneID();
  const EventWork* pEventWork = cp_gdata->GetEventWork();
  if( pZoneDataLoader->CheckEncountNGZone( zoneID, pEventWork) )
  {
    return false;
  }

  return (encount != Encount::Encount::ENCOUNT_DATA_ID_ERROR);
}

//----------------------------------------------------------------------------
/**
 *  @brief  あまいミツ　野生戦を実行
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::CallAmaimitsuBattle( AMX*, const cell* )
{
  FieldScriptSystem*    p_sys  = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  const GameSys::GameData* cp_gdata = p_gman->GetGameData();
  Fieldmap* pFieldmap = p_gman->GetFieldmap();
  Encount::Encount * pEncount = pFieldmap->GetEncount();

  FieldEventCheck::CalcCollisionEncount( *cp_gdata->GetFieldCharaModelManager(), *p_gman->GetFieldResident()->GetCollisionScene( FieldResident::COLLISION_SCENE_ENCOUNT ) );
  u32 encount = FieldEventCheck::CheckEncount( cp_gdata->GetFieldCharaModelManager()->GetFieldMoveModel( MoveModel::FIELD_MOVE_MODEL_PLAYER ) );
  if(encount != Encount::Encount::ENCOUNT_DATA_ID_ERROR)
  {
    // @fix NMCat[777]「あまいミツ」では島スキャンのポケモンが出現しない
    // ポケモンサーチかどうかを判定し、通常のエンカウントと処理を分けます
    // ポケモンサーチだった場合は、
    // 次のスクリプトの予約し、（PokemonSearch::DataAccessor::ReserceSeearchBattle内でScriptSystem::SetReserveScriptを呼ぶ）
    // サスペンドせずに終了させます。(戻り値も適当なものを返します)

    // ポケモンサーチデータアクセサー作成
    FieldResident*      pResident         = p_gman->GetFieldResident();
    void*               pPokemonSearchBin = pResident->GetResource( BL_IDX_FIELD_RESIDENT__POKEMON_SEARCH_PACK);
    PokemonSearch::DataAccessor pokemonSearchDataAccessor( pPokemonSearchBin);

    // ポケモンサーチエンカウントかどうかを判定
    u32                 areaID            = pFieldmap->GetAreaID();
    const EventWork*    pEventWork        = cp_gdata->GetEventWork();
    bool isPokemonSearchEncountCall = pokemonSearchDataAccessor.IsSearchBattleCall( areaID, encount, pEventWork);

    // ポケモンサーチ
    if( isPokemonSearchEncountCall )
    {
      // ポケサーチバトルスクリプトを予約
      pokemonSearchDataAccessor.ReserveSearchBattle( p_sys );

      return Field::FieldScript::AMAIMITSU_BATTLE_CALL_RETURN_POKEMON_SEARCH;
    }
    // 通常のエンカウント
    else
    {
      pEncount->CallForceEncount( p_gman, encount, false );

      // ウェイト
      p_sys->Suspend();
    }
  }
  else
  {
    // 野生戦のエンカウントデータとマッチしない・・・　ちゃんとCheckAmaimitsuEncountDataExistでチェックした後にコールしているか？
    GFL_ASSERT_STOP(encount != Encount::Encount::ENCOUNT_DATA_ID_ERROR);
  }

  return Field::FieldScript::AMAIMITSU_BATTLE_CALL_RETURN_NORMAL;
}

/* ------------------------------------------------------------------------- */
/**
 * @func GetWildBattleEffectID
 * @brief シンボルエンカウントテーブルからバトルエフェクトIDを取得
 * @author saita_kazuki
 * @date 2015/12/15
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::GetWildBattleEffectID( AMX * amx, const cell * ptr )
{
  u16 tbl_no = ptr[1];

  ScriptObject*               p_obj                 = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*       p_game_manager        = p_obj->GetGameManager();
  gfl2::heap::HeapBase*       p_heap                = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  gfl2::fs::AsyncFileManager* p_async_file_manager  = p_game_manager->GetAsyncFileManager();

  Encount::SymbolEncountParam symbol_encount_param;
  Encount::EncountUtil::SyncLoadSymbolEncountParam( p_heap, p_async_file_manager, tbl_no, &symbol_encount_param);

  return symbol_encount_param.btl_eff;
}

//----------------------------------------------------------------------------
/**
 *  @brief  トレーナーバトルコール
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::CallTrainerBattleCore(AMX * amx, const cell * ptr)
{
  TrainerBattleParam param;

  gfl2::std::MemClear( &param, sizeof(Field::TrainerBattleParam));

  param.partnerID = static_cast<trainer::TrainerID>(ptr[1]);
  param.trID0 = static_cast<trainer::TrainerID>(ptr[2]);
  param.trID1 = static_cast<trainer::TrainerID>(ptr[3]);
  param.exFlag = ptr[4];
  param.bgFarType = static_cast<btl::bg::bgsys_far_typeID>(ptr[5]);
  param.bgNearType = static_cast<btl::bg::bgsys_near_typeID>(ptr[6]);
  param.btlEffId = static_cast<BattleEffectId>(ptr[7]);
  param.retBGMID = ptr[8];

  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

  // 強制マルチ
  if ( param.partnerID != trainer::TRID_NULL )
  {
    if( param.exFlag &  TRAINER_BTL_FLAG_ROYAL_BATTLE ){
      param.rule = BTL_RULE_ROYAL;  //ロイヤル
    }else{
      param.rule = BTL_RULE_DOUBLE; //マルチ
    }
  }
  else if( param.trID1 != trainer::TRID_NULL )
  {
    param.rule = BTL_RULE_DOUBLE; //タッグ
  }
  else
  {
    // トレーナーデータからルールを取得
    trainer::TRAINER_DATA tr_data;
    trainer::LoadTrainerData(p_heap, param.trID0, &tr_data);

    param.rule = static_cast<BtlRule>(tr_data.fight_type);
  }

  ScriptObject*           p_obj             = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*   p_gman            = p_obj->GetGameManager();
  GameSys::GameData*      p_game_data       = p_gman->GetGameData();
  Encount::EncountWork*   p_encount_work    = p_game_data->GetEncountWork();
  Encount::EncountEffect* p_encount_effect  = p_encount_work->GetEncountEffect();

  u32 option = 0;
  if( p_encount_effect->IsInitialized())
  {
    option |= EVBTL_CALLOPT_ENCEFF_EXTERNAL;
  }

  // バトルコール.
  EventBattleCall::CallTrainer( p_gman, param, option );

  // ウェイト
  pSys->Suspend();

  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func   TrainerGetInfo
 * @brief  トレーナー情報取得
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::TrainerGetInfo( AMX* amx, const cell* ptr )
{
  gfl2::heap::HeapBase*    p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  trainer::TrainerID       tr_id  = static_cast< trainer::TrainerID >( ptr[ 1 ] );

  trainer::TRAINER_DATA    tr_data;
  trainer::TrainerTypeData trtype_data;

  cell*                    buf;
  cell                     ret = 0;

  trainer::LoadTrainerData( p_heap_dev, tr_id, &tr_data );
  trtype_data.LoadData( p_heap_dev, static_cast< trainer::TrType >( tr_data.tr_type ) );

  amx_GetAddr( amx, ptr[ 2 ], &buf ); // 保存バッファを取得
  
  if ( buf == NULL ) return 0;

  switch ( ptr[ 3 ] )
  {
  case SCR_TR_INFO_TYPE:
    ret = static_cast<cell>(tr_data.tr_type);
    buf[ SCR_TR_INFO_TYPE ] = ret;
    break;
  case SCR_TR_INFO_SEX:
    ret = static_cast<cell>(trtype_data.GetSex());
    buf[ SCR_TR_INFO_SEX ] = ret;
    break;
  case SCR_TR_INFO_FIGHT_TYPE:
    ret = static_cast<cell>(tr_data.fight_type);
    buf[ SCR_TR_INFO_FIGHT_TYPE ] = ret;
    break;
  case SCR_TR_INFO_EYE_BGM:
    ret = static_cast<cell>(trtype_data.GetEyeBGM());
    buf[ SCR_TR_INFO_EYE_BGM ] = ret;
    break;
  case SCR_TR_INFO_EYE_SE:
    ret = static_cast<cell>(trtype_data.GetEyeSE());
    buf[ SCR_TR_INFO_EYE_SE ] = ret;
    break;
  case SCR_TR_INFO_MAX:
    buf[ SCR_TR_INFO_TYPE ]       = static_cast<cell>(tr_data.tr_type);
    buf[ SCR_TR_INFO_SEX ]        = static_cast<cell>(trtype_data.GetSex());
    buf[ SCR_TR_INFO_FIGHT_TYPE ] = static_cast<cell>(tr_data.fight_type);
    buf[ SCR_TR_INFO_EYE_BGM ]    = static_cast<cell>(trtype_data.GetEyeBGM());
    buf[ SCR_TR_INFO_EYE_SE ]     = static_cast<cell>(trtype_data.GetEyeSE());
    break;
  }

  return ret;
}

//------------------------------------------------------------------------------
/**
 * @func   TrainerGetMsgID
 * @brief  トレーナーメッセージID取得
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::TrainerGetMsgID( AMX* amx, const cell* ptr )
{
  trainer::TrainerID tr_id = static_cast<trainer::TrainerID> (ptr[1]);
  trainer::MessageKind kind_id = static_cast<trainer::MessageKind> (ptr[2]);

  gfl2::heap::HeapBase* p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  return trainer::TrainerMessage::GetStringID( tr_id, kind_id, p_heap_dev );
}

//----------------------------------------------------------------------------
/**
 *  @brief  視線トレーナー　トレーナーID取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::TrainerGetScrID(AMX * amx, const cell * ptr)
{
  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  Work* p_script_work = p_obj->GetWork();

  u16 index = ptr[1];

  const Work::TRAINER_HITDATA* cp_hitdata = p_script_work->GetTrainerHitDataConst(static_cast<Work::TRAINER_EYE_HIT> (index));
  return cp_hitdata->scr_id;
}

//----------------------------------------------------------------------------
/**
 *  @brief  視線トレーナー　EventID取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::TrainerGetEventID(AMX * amx, const cell * ptr)
{
  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  Work* p_script_work = p_obj->GetWork();

  u16 index = ptr[1];

  const Work::TRAINER_HITDATA* cp_hitdata = p_script_work->GetTrainerHitDataConst(static_cast<Work::TRAINER_EYE_HIT> (index));
  return cp_hitdata->event_id;
}

//----------------------------------------------------------------------------
/**
 *  @brief  トレーナー敗戦処理
 *          この後すぐSuspendする。
 */
//-----------------------------------------------------------------------------

cell AMX_NATIVE_CALL FuncSetCommon::ChangeEventTrainerLose_( AMX* amx, const cell * ptr )
{
  GameSys::GameEvent      *call_event;
  ScriptObject            *p_obj     = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman    = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys     = p_gman->GetFieldScriptSystem();
  GameSys::GameEventManager* p_evman = p_gman->GetGameEventManager();

  //スクリプトチェックフラグのオフ
  {
    SystemWork* pSystemWork = p_sys->GetSystemWork();

    //戦闘負け時はフラグOff処理だけでいい by iwasawa
#if 0
    if( pSystemWork->CheckBit( SCR_END_CHECK_BIT_SP_MOTION )){
      pSystemWork->OffCheckBit( SCR_END_CHECK_BIT_SP_MOTION );
    }
#endif
    //BGパーツの記憶削除
    p_sys->ClearBgPartsParam();
  }
  call_event = GFL_NEW(p_evman->GetHeap()) EventNormalLose(p_evman->GetHeap());
  p_sys->EntryNextEvent(call_event);
  
  p_sys->ForceEnd();
  p_sys->Suspend();

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトル結果取得
 */
//-----------------------------------------------------------------------------

cell AMX_NATIVE_CALL FuncSetCommon::BattleGetResult_(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  const GameSys::GameData* cp_gdata = p_gman->GetGameData();

  const BATTLE_RESULT_DATA* data = cp_gdata->GetLastBattleResultData();

  cell* buf;
  amx_GetAddr( amx, ptr[1], &buf );

  // BTS2122 【とまり：トレーナー戦で相討ちになると強制終了が発生する】対処
  switch( data->result )
  {
  case BTL_RESULT_CAPTURE:
  case BTL_RESULT_RUN_ENEMY:
  case BTL_RESULT_LOSE_NOT_GAMEOVER_FOR_SKY: //スカイバトルでしかありえず、スカイバトルではこの関数を使わない
    GFL_ASSERT( 0 );  //@check
    buf[0] = BTL_RESULT_WIN;
    break;

  case BTL_RESULT_LOSE:
  case BTL_RESULT_DRAW:
  case BTL_RESULT_COMM_ERROR:
    buf[0] = BTL_RESULT_LOSE; // 引き分けと通信エラーも負け扱い
    break;

  case BTL_RESULT_WIN:
  case BTL_RESULT_RUN:
    buf[0] = data->result;
    break;

  default:
    GFL_ASSERT(0);  //@check
    buf[0] = data->result;
  }

  buf[1] = data->turnNum;
  buf[2] = data->sakasaPoint;
  buf[3] = data->sakasaSpFlag;
  buf[4] = data->tvNaviData.megaPoke;
  buf[5] = data->tvNaviData.lastWaza;
  buf[6] = data->tvNaviData.frontPoke[0];
  buf[7] = data->tvNaviData.frontPoke[1];
  buf[8] = data->b_zenryoku_waza_used;

  GFL_PRINT("sakasaPoint %d\n", data->sakasaPoint);
  if( data->sakasaSpFlag )
    GFL_PRINT("sakasaSpFlag !!");

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @func PlayEncountEffect_
 * @brief 戦闘導入演出の再生開始
 * @param battle_effect_id バトルエフェクトID
 * @author saita_kazuki
 * @date 2015/12/15
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::PlayEncountEffect_( AMX * amx, const cell * ptr )
{
  BattleEffectId battle_effect_id = static_cast<BattleEffectId>( ptr[1] );

  ScriptObject*           p_obj             = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*   p_game_manager    = p_obj->GetGameManager();
  GameSys::GameData*      p_game_data       = p_game_manager->GetGameData();
  Encount::EncountWork*   p_encount_work    = p_game_data->GetEncountWork();
  Encount::EncountEffect* p_encount_effect  = p_encount_work->GetEncountEffect();

  gfl2::heap::HeapBase* p_heap      = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  gfl2::heap::HeapBase* p_heap_dll  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );

  // バトルエフェクトID取得
  BATTLE_SETUP_PARAM battle_setup_param;
  BATTLE_PARAM_SetBattleEffect( &battle_setup_param, battle_effect_id );

  // DLL読み込み
  p_encount_work->LoadEncountEffectDll( p_heap_dll );

  // 初期化
  Encount::EncountEffect::Description desc;
  desc.effectKind = static_cast<Encount::EncountEffect::EffectKind>( battle_setup_param.btlEffData.enceff_fld.eff_kind );
  p_encount_effect->Initialize( p_heap, desc );

  // 再生待機モードに設定
  p_encount_effect->SetSequenceStartupWaitMode( true );

  // 再生開始
  p_encount_effect->Start();

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @func WaitPlayEncountEffect_
 * @brief 戦闘導入演出シーケンスの再生準備ができたか
 * @author saita_kazuki
 * @date 2015/12/15
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::WaitPlayEncountEffect_( AMX * amx, const cell * ptr )
{
  ScriptObject*           p_obj             = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*   p_game_manager    = p_obj->GetGameManager();
  GameSys::GameData*      p_game_data       = p_game_manager->GetGameData();
  Encount::EncountWork*   p_encount_work    = p_game_data->GetEncountWork();
  Encount::EncountEffect* p_encount_effect  = p_encount_work->GetEncountEffect();

  // 再生準備ができたかチェック
  if( p_encount_effect->IsStart() )
  {
    // フィールドキャプチャーが必要ならリクエストを出す
    if( p_encount_effect->IsNeedFieldCapture() )
    {
      p_encount_effect->RequestFieldCapture();
    }

    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------- */
/**
 * @func SetupEncountEffect_
 * @brief 戦闘導入演出シーケンス再生前セットアップ
 * @author saita_kazuki
 * @date 2015/12/15
 * @note WaitPlayEncountEffect_の1フレーム後に呼び出す必要がある
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::SetupEncountEffect_( AMX * amx, const cell * ptr )
{
  ScriptObject*           p_obj             = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*   p_game_manager    = p_obj->GetGameManager();
  GameSys::GameData*      p_game_data       = p_game_manager->GetGameData();
  Encount::EncountWork*   p_encount_work    = p_game_data->GetEncountWork();
  Encount::EncountEffect* p_encount_effect  = p_encount_work->GetEncountEffect();

  // シーケンス再生前の準備
  p_encount_effect->Setup();

  // 再生待機モードの解除
  p_encount_effect->SetSequenceStartupWaitMode( false );

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @func GetWildBattleCaptureMonsNo
 * @brief 野生戦で捕獲したポケモンのNo取得
 * @author saita_kazuki
 * @date 2016/2/19
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::GetWildBattleCaptureMonsNo(AMX * amx, const cell *)
{
  ScriptObject*           p_obj             = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*   p_game_manager    = p_obj->GetGameManager();
  GameSys::GameData*      p_game_data       = p_game_manager->GetGameData();

  return p_game_data->GetLastBattleResultData()->captureMonsNo;
}

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )
