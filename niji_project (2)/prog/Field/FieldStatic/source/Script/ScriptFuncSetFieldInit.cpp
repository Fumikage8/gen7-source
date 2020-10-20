//=============================================================================
/**
 *
 *  @file   ScriptFuncSetInitField.cpp
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

#include "FieldScript/include/ScriptObject.h"

//  ZoneDataLoader
//  pokediary
//  Savedata::BoxPokemon
//  field::FieldSkillUse::PokePartyHaveWaza
#include <Fade/include/gfl2_FadeManager.h>


#include "Savedata/include/MyStatus.h"
#include "Savedata/include/ZukanSave.h"
#include "Savedata/include/FieldMenuSave.h"

#include "System/include/HeapDefine.h"
#include "System/include/PokemonVersion.h"
#include "GameSys/include/GameManager.h"

#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include "Field/FieldStatic/include/Time/EvTime.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetFieldInit.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProcManager.h"

#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldBgEventActor.h"

// PokeTool
#include "PokeTool/include/PokeToolForm.h"
#include "PokeTool/include/NakayoshiManager.h"
#include "Field/FieldStatic/include/FieldWarpData.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

#include "niji_reference_files/script/ScriptPokeParamDefine.h"
#include "niji_conv_header/field/script/inc/scene_work_const.inc"

// Env
#include "Field/FieldRo/include/EnvSound/EnvSoundManager.h"
#include "Field/FieldRo/include/FieldEnvManager.h"

#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"

// resource
#include "arc_def_index/arc_def.h"

// MoveModel
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeNpc.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTraffic.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"

// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectNewTrial5.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectNewTrial5ForModel.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFureaiIconBase.h"

// skybox
#include "System/include/Skybox/Skybox.h"
#include "Field/FieldRo/include/Skybox/FieldSkyboxDataManager.h"

#include "Field/FieldRos/FieldGimmickShirenDenki/include/FieldGimmickShirenDenki.h"
#include "Field/FieldRos/FieldGimmickTriggerPos/include/FieldGimmickTriggerPos.h"

// fureai
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiDataAccessor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActor.h"
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiTool.h"

// PlayerShade
#include "Field/FieldRo/include/Shade/FieldPlayerShade.h"

#include "Field/FieldRo/include/Traffic/FieldTrafficManager.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )  ///< ScriptObject の定義に合わせて修正

//-----------------------------------------------------------------------------
/**
 *  @brief  命令テーブルセットを取得
 */
//-----------------------------------------------------------------------------
const GFL_PAWN_FUNC_TABLE* FuncSetFieldInit::GetTable( void )
{
  static const GFL_PAWN_FUNC_TABLE s_FuncTable[] =
  {
    GFL_PAWN_FUNC( ChrAdd )
    GFL_PAWN_FUNC( ChrDel )
    GFL_PAWN_FUNC( ChrIsExist )
    
    GFL_PAWN_FUNC( ChrSetPos_ )
    GFL_PAWN_FUNC( ChrSetPosAng_ )
    GFL_PAWN_FUNC( ChrSetPosVec3_ )
    GFL_PAWN_FUNC( ChrSetAng )
    GFL_PAWN_FUNC( ChrSetAngVec3 )
    GFL_PAWN_FUNC( ChrSetPosAngVec3_ )

    GFL_PAWN_FUNC( ChrGetPosX_ )
    GFL_PAWN_FUNC( ChrGetPosY_ )
    GFL_PAWN_FUNC( ChrGetPosZ_ )
    GFL_PAWN_FUNC( ChrGetAng_ )
    GFL_PAWN_FUNC( ChrGetTalkReturnAng_ )

    GFL_PAWN_FUNC( ChrCollision )
    GFL_PAWN_FUNC( ChrFloat )
    GFL_PAWN_FUNC( ChrGetFloat )
    GFL_PAWN_FUNC( ChrDisp )
    GFL_PAWN_FUNC( ChrShadowDisp )
    GFL_PAWN_FUNC( ChrPartsDisp )
    GFL_PAWN_FUNC( ChrSetDefaultIdle )
    GFL_PAWN_FUNC( ChrGetDefaultIdle )
    GFL_PAWN_FUNC( ChrCheckSittingTalk )
    GFL_PAWN_FUNC( ChrDisableDispCntCulling )
    GFL_PAWN_FUNC( ChrDisableAppCulling )
    GFL_PAWN_FUNC( ChrSetMotionMoveDisableFlag )
    GFL_PAWN_FUNC( ChrSetStateBitShadowOff )
    GFL_PAWN_FUNC( ChrSetStateBitInvalidResistCheck )
    GFL_PAWN_FUNC( ChrSetStateBitCullingAabbMinimun )
    GFL_PAWN_FUNC( ChrSetStateBitCullingLengthOptDisable )

    GFL_PAWN_FUNC( ChrMotionEndWatchStart )
    GFL_PAWN_FUNC( ChrMotionEndWatchClear )
    GFL_PAWN_FUNC( ChrMotionGetEndFrame )
    
    GFL_PAWN_FUNC( ChrIsNpcMoveCode )
    GFL_PAWN_FUNC( NpcOnOption )
    GFL_PAWN_FUNC( NpcOffOption )
    GFL_PAWN_FUNC( NpcResetOption )
    GFL_PAWN_FUNC( NpcClearTalkEnd )
    GFL_PAWN_FUNC( NpcClearDirtyFlag )
    GFL_PAWN_FUNC( NpcOverWriteReturnAngle_ )

    GFL_PAWN_FUNC( PlayerFreeMoveStateReset )
    GFL_PAWN_FUNC( ChrFreeMoveStateReset )
    GFL_PAWN_FUNC( ModelSearchByCharacterID )

    GFL_PAWN_FUNC( ObjAdd )
    GFL_PAWN_FUNC( ObjDel )
    GFL_PAWN_FUNC( ObjAddAll )
    GFL_PAWN_FUNC( ObjDelAll )
    GFL_PAWN_FUNC( ObjGetPosX )
    GFL_PAWN_FUNC( ObjGetPosY )
    GFL_PAWN_FUNC( ObjGetPosZ )
    GFL_PAWN_FUNC( ZigarudeCellStatusSet )
    GFL_PAWN_FUNC( PokeFinderAddDel )

    GFL_PAWN_FUNC( EnvSoundStatusSet )
    GFL_PAWN_FUNC( EnvSoundVolumeChange )
    GFL_PAWN_FUNC( FootSoundVolumeChange )
    GFL_PAWN_FUNC( WeatherSoundVolumeChange )

    GFL_PAWN_FUNC( ChangeWeather )
    GFL_PAWN_FUNC( ResetWeather )
    GFL_PAWN_FUNC( GetNowWeather )

    GFL_PAWN_FUNC( RotomPosEventSet )

    GFL_PAWN_FUNC( StaticModelMotionPlay )
    GFL_PAWN_FUNC( StaticModelMotionIsEnd )
    GFL_PAWN_FUNC( StaticModelMotionPlaySmooth )
    GFL_PAWN_FUNC( StaticModelMotionIsEndSmooth )
    GFL_PAWN_FUNC( StaticModelMotionPlayInv )
    GFL_PAWN_FUNC( StaticModelMotionSetEnd )
    GFL_PAWN_FUNC( StaticModelMotionReset )
    GFL_PAWN_FUNC( StaticModelMotionStop )
    GFL_PAWN_FUNC( StaticModelMotionPause )
    GFL_PAWN_FUNC( StaticModelMotionRestart )
    GFL_PAWN_FUNC( StaticModelMotionAllStop )
    GFL_PAWN_FUNC( StaticModelMotionAllRestart )
    GFL_PAWN_FUNC( StaticModelLoopMotionPlay )
    GFL_PAWN_FUNC( StaticModelLoopMotionStop )

    GFL_PAWN_FUNC( StaticModelMotionByStaticIdPlay )
    GFL_PAWN_FUNC( StaticModelMotionByStaticIdIsEnd )
    GFL_PAWN_FUNC( StaticModelMotionByStaticIdStop )
    GFL_PAWN_FUNC( StaticModelMotionByStaticIdSetFrame )
    GFL_PAWN_FUNC( StaticModelSetPosAng )
    GFL_PAWN_FUNC( S5_GimmickModelMotionPlay )
    GFL_PAWN_FUNC( S5_GimmickModelMotionSetEnd )
    GFL_PAWN_FUNC( S5_GimmickModelMotionIsEnd )

    GFL_PAWN_FUNC( RegistSkyboxPosFix )
    GFL_PAWN_FUNC( UnRegistSkyboxPosFix )

    GFL_PAWN_FUNC( ChrSetPosForInit )
    GFL_PAWN_FUNC( ChrSetPosAngForInit )
    GFL_PAWN_FUNC( ChrSetPosVec3ForInit )
    GFL_PAWN_FUNC( ChrSetPosAngVec3ForInit )
    GFL_PAWN_FUNC( GetLunaSolDiveMonsno )

    GFL_PAWN_FUNC( SystemEffectSetPos )
    GFL_PAWN_FUNC( SystemEffectSetChr )
    GFL_PAWN_FUNC( SystemEffectDel )
    GFL_PAWN_FUNC( S5_SystemEffectChange )
    GFL_PAWN_FUNC( S5_SystemEffectFade )
    GFL_PAWN_FUNC( GimmickTriggerPosSetActiveFlag )
    GFL_PAWN_FUNC( FureaiIconCreate )
    GFL_PAWN_FUNC( FureaiIconDelete )
    GFL_PAWN_FUNC( FureaiIconModeChange )
    GFL_PAWN_FUNC( FureaiIconIsExist )
    GFL_PAWN_FUNC( FureaiIconGetState )
		GFL_PAWN_FUNC( FureaiIconEnableSound )
    GFL_PAWN_FUNC( FureaiGetPokemonSettingData )
    GFL_PAWN_FUNC( FureaiChrGetPokemonData )
    GFL_PAWN_FUNC( FureaiChrPointAdd )
    GFL_PAWN_FUNC( FureaiSaveSetNickNameKnow )
    GFL_PAWN_FUNC( FureaiSaveSetNadeNadeReqEventEnd )
    GFL_PAWN_FUNC( FureaiSaveSetPoint )
    GFL_PAWN_FUNC( FureaiChrPlayVoice )
    GFL_PAWN_FUNC( FureaiChrCanNadeNadeDistance )
		GFL_PAWN_FUNC( FureaiChrApplyToDebugParam )
		GFL_PAWN_FUNC( ChrGetChrID )

    GFL_PAWN_FUNC( ShadeAreaApply )

    GFL_PAWN_FUNC_END,
  };

  return s_FuncTable;
}



/*
 *  Y軸回転の成分のみを抽出
 */
f32 FuncSetFieldInit::ConvRotationVector3ToYRadian( gfl2::math::Vector3& src )
{
  gfl2::math::Vector3 UnitX    (1.0f,0.0f,0.0f);
  gfl2::math::Vector3 UnitZ    (0.0f,0.0f,1.0f);
  gfl2::math::Vector3 vec=src;

  vec.y = 0.0f;
  vec   = vec.Normalize();
  f32 rad  = gfl2::math::AcosRad( UnitZ.Dot( vec ) );
  if( UnitX.Dot( vec ) < 0.0f ){ rad *= -1.0f; }

  return rad;
}

/*
 *  Y軸回転の成分のみを抽出
 */
f32 FuncSetFieldInit::ConvRotationQuaternionToYRadian( gfl2::math::Quaternion& src )
{
  gfl2::math::Matrix34  Mtx;
  gfl2::math::Vector3 UnitZ    (0.0f,0.0f,1.0f);

  src.QuaternionToMatrix( Mtx );

  UnitZ = Mtx * UnitZ;

  return ConvRotationVector3ToYRadian(UnitZ);
}



//------------------------------------------------------------------------------
/**
 * @func        ChrAdd
 * @brief       指定キャラクターをAdd
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrAdd( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  ZoneID  zoneID = ptr[1];
  u32     EventID = ptr[2];

  Field::FieldResident* pFieldResident = pSys->GetGameManager()->GetFieldResident();
  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  if( pPlacementDataManager->SearchZoneEventIDModel( EventID ) != NULL ){
    //すでにいるよ？　警告する
    SCRIPT_WARNING_MSG(0,"ChrAdd 既に存在するNPC EventID=%d\n", EventID);
    return false;
  }

  // @note momiji対応、ChrAdd時に対応するバニッシュフラグ(バニッシュ領域のみ)を無効化する
  if( pPlacementDataManager->RegistZoneEventIDModel( EventID, pSys->GetGameManager()->GetGameData()->GetEventWork(), false ) < 0 ){
    GFL_ASSERT_MSG(0,"ChrAdd 存在しないEventID %d\n", EventID);
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrDel
 * @brief       指定キャラクターをDel
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrDel( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  ZoneID  zoneID = ptr[1];
  u32     EventID = ptr[2];

  Field::FieldResident* pFieldResident = pSys->GetGameManager()->GetFieldResident();
  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  if( !pPlacementDataManager->IsEntryZoneEventIDModel( EventID ))
  {
    GFL_ASSERT_MSG(0,"ChrDel 存在しないEventID %d\n", EventID);
    return false;
  }
  
  // @note momiji対応、ChrDel時に対応するバニッシュフラグ(バニッシュ領域のみ)を有効化する
  if( pPlacementDataManager->UnRegistZoneEventIDModel( EventID, pSys->GetGameManager()->GetGameData()->GetEventWork(), false ) < 0 ){
    //すでにいないよ？　警告する
    SCRIPT_WARNING_MSG(0,"ChrDel 既にUnregistされたNPC EventID=%d\n", EventID);
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrIsExist
 * @brief       指定キャラクターアクターの現存をチェック
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrIsExist( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  u32     EventID = ptr[1];

  Field::FieldResident* pFieldResident = pSys->GetGameManager()->GetFieldResident();
  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  if( pPlacementDataManager->SearchZoneEventIDModel( EventID ) != NULL ){
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
* @func        _ChrSetPos
* @brief       キャラクターの座標を指定します
* @author      N.Takeda
* @date        2015/05/22(金) 21:42:09
*
* @com 相対座標対応コマンド
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPos_(AMX* amx, const cell* ptr)
{
#if PM_DEBUG
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GFL_ASSERT_MSG(!(pSys->IsInitScriptTiming()), "can not use [ChrSetPos]");
#endif //PM_DEBUG
  return ChrSetPos_core(amx, ptr);
}

//------------------------------------------------------------------------------
/**
 * @func        _ChrSetPos
 * @brief       キャラクターの座標を指定します
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:09
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPos_core(AMX* amx, const cell* ptr, b32 isInit )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  f32   posX   = amx_ctof( ptr[2] );
  f32   posZ   = amx_ctof( ptr[3] );
  b32   isEnd   = ptr[4];

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel == NULL ){
    GFL_ASSERT_MSG(0,"%d\n",EventID);
    return false;
  }

  /// 移動
  gfl2::math::Vector3 vec = pSys->CalcEventPositionWorld(posX,0.0,posZ); //Y吸着

  if (isEnd)
  {
    vec.x = posX;
    vec.z = posZ;
  }
  
  // バミリチェック用フラグ操作
  if (isInit)
  {
    pCharaModel->ClearPositionCheckForEventScript();
  }
  else if (isEnd)
  {
    pCharaModel->EnablePointMoveForEventScript();
  }
  else
  {
    pCharaModel->EnableFreeMoveForEventScript();
  }

  // 主人公終了時
  if (isEnd && pCharaModel->GetEventId() == Field::FIELD_EVENTID_PLAYER)
  {
    // @note 念のため現在のY座標を引き継ぐ
    gfl2::math::Vector3 now = pCharaModel->GetCharaDrawInstance()->GetPosition();
    vec.y = now.y;
    
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
    gfl2::math::Vector3* pos = GFL_NEW(p_heap) gfl2::math::Vector3();
    *pos = vec;

    // プレイヤーのジャンプ前に往来との衝突回避リクエストを出す
    Field::TrafficManager* pTrafficManager = pSys->GetGameManager()->GetFieldmap()->GetTrafficManager();
    pTrafficManager->RequestPlayerWarp(vec);

    pSys->SetWaitFunc(amx, WaitFunc_PlayerSetPosForEnd, 0, pos);
    pSys->Suspend();
  }
  else
  {
    gfl2::math::Vector3 now = pCharaModel->GetCharaDrawInstance()->GetPosition();

    //高さが無い状態でコマンド発行された場合のセーフティ。高さがなかった場合は接地処理で更新されないので、元の高さのままになるよう明示指定する
    {
      pCharaModel->GetCharaDrawInstance()->SetPosition(vec);

      // 地面情報は更新するが接地処理は行わないようにする
      b32 isEnable = pCharaModel->IsForceLanding();
      pCharaModel->SetForceLanding(false);

      // 地面情報を更新
      pCharaModel->AdjustGround();

      if (pCharaModel->IsSetCollisionSceneContainerForGround()){
        if (pCharaModel->IsOnGround() == false){
          //      GFL_ASSERT_MSG(0,"キャラNo.%dの位置には高さが無い\n");
          vec.y = now.y;
        }
      }
      //現在地点でアジャストしなおす
      pCharaModel->GetCharaDrawInstance()->SetPosition(now);
      pCharaModel->AdjustGround();

      // 接地処理を基に戻す
      pCharaModel->SetForceLanding(isEnable);
    }

    // 座標
    pCharaModel->GetCharaDrawInstance()->SetPosition(vec);
  }
  return true;
}

//------------------------------------------------------------------------------
/**
* @func        _ChrSetPosAng
* @brief       キャラクターの座標を指定します
* @author      N.Takeda
* @date        2015/05/22(金) 21:42:10
*
* @com 相対座標対応コマンド
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPosAng_(AMX* amx, const cell* ptr)
{
#if PM_DEBUG
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GFL_ASSERT_MSG(!(pSys->IsInitScriptTiming()), "can not use [ChrSetPosAng]");
#endif //PM_DEBUG
  return ChrSetPosAng_core(amx, ptr);
}

//------------------------------------------------------------------------------
/**
 * @func        _ChrSetPosAng
 * @brief       キャラクターの座標を指定します
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:10
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPosAng_core(AMX* amx, const cell* ptr, b32 isInit )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  f32   fX    = amx_ctof( ptr[2] );
  f32   fZ    = amx_ctof( ptr[3] );
  f32   fAngY = amx_ctof( ptr[4] );
  b32   isEnd = ( ptr[5] );
  b32   isFoceWorld = ( ptr[6] );
  
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel == NULL ){
    GFL_ASSERT_MSG(0,"%d\n",EventID);
    return false;
  }

  gfl2::math::Vector3 vec = pSys->CalcEventPositionWorld( fX, 0.0, fZ );  //Yは勝手に接地
  gfl2::math::Vector3 now = pCharaModel->GetCharaDrawInstance()->GetPosition();

  if (isEnd || isFoceWorld)
  {
    vec.x = fX;
    vec.z = fZ;
  }

  // バミリチェック用フラグ操作
  if (isInit)
  {
    pCharaModel->ClearPositionCheckForEventScript();
  }
  else if (isEnd)
  {
    pCharaModel->EnablePointMoveForEventScript();
  }
  else
  {
    pCharaModel->EnableFreeMoveForEventScript();
  }

  // 主人公終了時
  if (isEnd && pCharaModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    // @note 念のため現在のY座標を引き継ぐ
    gfl2::math::Vector3 now = pCharaModel->GetCharaDrawInstance()->GetPosition();
    vec.y = now.y;

    /// 回転.
    gfl2::math::Quaternion quat(pCharaModel->GetCharaDrawInstance()->GetRotationQuat());
    quat.RadianYXZToQuaternion(0.0f, gfl2::math::ConvDegToRad(fAngY), 0.0f);
    pCharaModel->GetCharaDrawInstance()->SetRotationQuat(quat);

    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
    gfl2::math::Vector3* pos = GFL_NEW(p_heap) gfl2::math::Vector3();
    *pos = vec;

    // プレイヤーのジャンプ前に往来との衝突回避リクエストを出す
    Field::TrafficManager* pTrafficManager = pSys->GetGameManager()->GetFieldmap()->GetTrafficManager();
    pTrafficManager->RequestPlayerWarp(vec);

    pSys->SetWaitFunc(amx, WaitFunc_PlayerSetPosForEnd, 0, pos);
    pSys->Suspend();
  }
  else
  {
    //高さが無い状態でコマンド発行された場合のセーフティ。高さがなかった場合は接地処理で更新されないので、元の高さのままになるよう明示指定する
    {
      pCharaModel->GetCharaDrawInstance()->SetPosition(vec);

      // 地面情報は更新するが接地処理は行わないようにする
      b32 isEnable = pCharaModel->IsForceLanding();
      pCharaModel->SetForceLanding(false);

      // 地面情報を更新
      pCharaModel->AdjustGround();

      if (pCharaModel->IsSetCollisionSceneContainerForGround()){
        if (pCharaModel->IsOnGround() == false){
          //      GFL_ASSERT_MSG(0,"キャラNo.%dの位置には高さが無い\n");
          vec.y = now.y;
        }
      }
      //現在地点でアジャストしなおす
      pCharaModel->GetCharaDrawInstance()->SetPosition(now);
      pCharaModel->AdjustGround();

      // 接地処理を基に戻す
      pCharaModel->SetForceLanding(isEnable);
    }

    /// 座標. 
    pCharaModel->GetCharaDrawInstance()->SetPosition(vec);

    /// 回転.
    gfl2::math::Quaternion quat(pCharaModel->GetCharaDrawInstance()->GetRotationQuat());

    quat.QuaternionToRadian(&vec.x, &vec.y, &vec.z);
    quat.RadianYXZToQuaternion(0.0f, gfl2::math::ConvDegToRad(fAngY), 0.0f);
    pCharaModel->GetCharaDrawInstance()->SetRotationQuat(quat);
  }
  return true;
}

//------------------------------------------------------------------------------
/**
* @func        _ChrSetPosVec3
* @brief       キャラクターの位置を３軸指定します
* @author      N.Takeda
* @date        2015/05/22(金) 21:42:11
*
* @com 相対座標対応コマンド
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPosVec3_(AMX* amx, const cell* ptr)
{
#if PM_DEBUG
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GFL_ASSERT_MSG(!(pSys->IsInitScriptTiming()), "can not use [ChrSetPosVec3]");
#endif //PM_DEBUG
  return ChrSetPosVec3_core(amx, ptr);
}

//------------------------------------------------------------------------------
/**
 * @func        _ChrSetPosVec3
 * @brief       キャラクターの位置を３軸指定します
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:11
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPosVec3_core(AMX* amx, const cell* ptr, b32 isInit)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  f32   fX   = amx_ctof( ptr[2] );
  f32   fY   = amx_ctof( ptr[3] );
  f32   fZ   = amx_ctof( ptr[4] );
  b32   isEnd = ( ptr[5] );
  b32   isFoceWorld = ( ptr[6] );
 
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel == NULL ){
    GFL_ASSERT_MSG(0,"%d\n",EventID);
    return false;
  }

  gfl2::math::Vector3 vec = pSys->CalcEventPositionWorld(fX, fY, fZ);

  if (isEnd || isFoceWorld)
  {
    vec.x = fX;
    vec.z = fZ;
  }

  // バミリチェック用フラグ操作
  if (isInit)
  {
    pCharaModel->ClearPositionCheckForEventScript();
  }
  else if (isEnd)
  {
    pCharaModel->EnablePointMoveForEventScript();
  }
  else
  {
    pCharaModel->EnableFreeMoveForEventScript();
  }

  // 主人公終了時
  if (isEnd && pCharaModel->GetEventId() == Field::FIELD_EVENTID_PLAYER)
  {
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
    gfl2::math::Vector3* pos = GFL_NEW(p_heap) gfl2::math::Vector3();
    *pos = vec;

    // プレイヤーのジャンプ前に往来との衝突回避リクエストを出す
    Field::TrafficManager* pTrafficManager = pSys->GetGameManager()->GetFieldmap()->GetTrafficManager();
    pTrafficManager->RequestPlayerWarp(vec);

    pSys->SetWaitFunc(amx, WaitFunc_PlayerSetPosForEnd, 0, pos);
    pSys->Suspend();
  }
  else
  {
    // 座標
    pCharaModel->GetCharaDrawInstance()->SetPosition(vec);
  }
  return true;
}


//------------------------------------------------------------------------------
/**
 * @func        _ChrSetAng
 * @brief       キャラクターの向き（Ｙ角度）だけを指定します
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:12
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetAng( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  f32   fAngY = amx_ctof( ptr[2] );
  
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );
  
  if( pCharaModel == NULL ){
    GFL_ASSERT_MSG(0,"%d\n",EventID);
    return false;
  }

  /// 回転.
  gfl2::math::Quaternion quat ( pCharaModel->GetCharaDrawInstance()->GetRotationQuat() );
  gfl2::math::Vector3 vec;

  quat.QuaternionToRadian(&vec.x,&vec.y,&vec.z);
  quat.RadianYXZToQuaternion(0.0, gfl2::math::ConvDegToRad(fAngY), 0.0);
  pCharaModel->GetCharaDrawInstance()->SetRotationQuat( quat );
  
  return true;
}


//------------------------------------------------------------------------------
/**
 * @func        _ChrSetAngVec3
 * @brief       キャラクターの向きを３軸指定します
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:13
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetAngVec3( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  //f32   fAngX = amx_ctof( ptr[2] );
  f32   fAngY = amx_ctof( ptr[3] );
  f32   fAngZ = amx_ctof( ptr[4] );

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );
  
  if( pCharaModel == NULL ){
    GFL_ASSERT_MSG(0,"%d\n",EventID);
    return false;
  }

  /// 回転.
  gfl2::math::Quaternion quat;        ///< 回転系コマンドの実装を全てクォータニオンに移行 Iwasawa. N.Takeda 2015/06/11
  quat.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(fAngZ), gfl2::math::ConvDegToRad(fAngY), gfl2::math::ConvDegToRad(fAngZ));
  pCharaModel->GetCharaDrawInstance()->SetRotationQuat( quat );
  
  return true;
}

//------------------------------------------------------------------------------
/**
* @func        _ChrSetPosAngVec3
* @brief       キャラクターの座標と角度を３軸指定します
* @author      N.Takeda
* @date        2015/05/22(金) 21:42:14
*
* @com 相対座標対応コマンド
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPosAngVec3_(AMX* amx, const cell* ptr)
{
#if PM_DEBUG
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GFL_ASSERT_MSG(!(pSys->IsInitScriptTiming()), "can not use [ChrSetPosAngVec3]");
#endif //PM_DEBUG
  return ChrSetPosAngVec3_core(amx, ptr);
}
//------------------------------------------------------------------------------
/**
 * @func        _ChrSetPosAngVec3
 * @brief       キャラクターの座標と角度を３軸指定します
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:14
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPosAngVec3_core(AMX* amx, const cell* ptr, b32 isInit)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  f32   fX    = amx_ctof( ptr[2] );
  f32   fY    = amx_ctof( ptr[3] );
  f32   fZ    = amx_ctof( ptr[4] );
  //f32   fAngX = amx_ctof( ptr[5] );
  f32   fAngY = amx_ctof( ptr[6] );
  f32   fAngZ = amx_ctof( ptr[7] );
  b32   isEnd = ( ptr[8] );


  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );
  

  if( pCharaModel == NULL ){
    GFL_ASSERT_MSG(0,"%d\n",EventID);
    return false;
  }

  gfl2::math::Vector3 vec = pSys->CalcEventPositionWorld(fX, fY, fZ);
  if (isEnd)
  {
    vec.x = fX;
    vec.z = fZ;
  }
  
  // バミリチェック用フラグ操作
  if (isInit)
  {
    pCharaModel->ClearPositionCheckForEventScript();
  }
  else if (isEnd)
  {
    pCharaModel->EnablePointMoveForEventScript();
  }
  else
  {
    pCharaModel->EnableFreeMoveForEventScript();
  }

  // 主人公終了時
  if (isEnd && pCharaModel->GetEventId() == Field::FIELD_EVENTID_PLAYER)
  {
    /// 回転.
    gfl2::math::Quaternion quat;        ///< 回転系コマンドの実装を全てクォータニオンに移行 Iwasawa. N.Takeda 2015/06/11
    quat.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(fAngZ), gfl2::math::ConvDegToRad(fAngY), gfl2::math::ConvDegToRad(fAngZ));
    pCharaModel->GetCharaDrawInstance()->SetRotationQuat(quat);
    
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
    gfl2::math::Vector3* pos = GFL_NEW(p_heap) gfl2::math::Vector3();
    *pos = vec;

    // プレイヤーのジャンプ前に往来との衝突回避リクエストを出す
    Field::TrafficManager* pTrafficManager = pSys->GetGameManager()->GetFieldmap()->GetTrafficManager();
    pTrafficManager->RequestPlayerWarp(vec);

    pSys->SetWaitFunc(amx, WaitFunc_PlayerSetPosForEnd, 0, pos);
    pSys->Suspend();
  }
  else
  {
    // 座標
    pCharaModel->GetCharaDrawInstance()->SetPosition(vec);

    /// 回転.
    gfl2::math::Quaternion quat;        ///< 回転系コマンドの実装を全てクォータニオンに移行 Iwasawa. N.Takeda 2015/06/11
    quat.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(fAngZ), gfl2::math::ConvDegToRad(fAngY), gfl2::math::ConvDegToRad(fAngZ));
    pCharaModel->GetCharaDrawInstance()->SetRotationQuat(quat);
  }
  
  return true;
}


//------------------------------------------------------------------------------
/**
 * @func        ChrGetPosX_
 * @brief       ChrGetPosX関数.
      C:/work/sangoScript/sango_project/prog/src/field/script/ScriptFuncSetInitMoveModel.cpp(315): cell AMX_NATIVE_CALL FuncSetInit::MdlGetPosX( AMX * amx, const cell * ptr )
 * @author      N.Takeda
 * @date        2015/10/15(木) 20:14:12
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrGetPosX_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  u32         EventID = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetCharaDrawInstance();
  gfl2::math::Vector3 pos = p_sys->CalcEventPositionOffset( pCharaDrawInstance->GetPosition() );
  
  return amx_ftoc(pos.x);
}

//------------------------------------------------------------------------------
/**
 * @func        ChrGetPosY_
 * @brief       ChrGetPosY関数.
 * @author      N.Takeda
 * @date        2015/10/15(木) 20:14:11
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrGetPosY_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  u32         EventID = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetCharaDrawInstance();
  
  gfl2::math::Vector3 pos = p_sys->CalcEventPositionOffset( pCharaDrawInstance->GetPosition() );

  return amx_ftoc(pos.y);
}

//------------------------------------------------------------------------------
/**
 * @func        ChrGetPosZ_
 * @brief       ChrGetPosZ関数.
 * @author      N.Takeda
 * @date        2015/10/15(木) 20:14:10
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrGetPosZ_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  u32         EventID = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetCharaDrawInstance();
  
  gfl2::math::Vector3 pos = p_sys->CalcEventPositionOffset( pCharaDrawInstance->GetPosition() );

  return amx_ftoc(pos.z);
}

//------------------------------------------------------------------------------
/**
 * @func        ChrGetAng_
 * @brief       ChrGetAng関数.
 * @author      N.Takeda
 * @date        2015/10/15(木) 20:14:08
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrGetAng_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32         EventID = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetCharaDrawInstance();
  gfl2::math::Quaternion  cQuat = pCharaDrawInstance->GetRotationQuat();
  
  /// クォータニオンから角を求める.
  f32 fAngle = gfl2::math::ConvRadToDeg(FuncSetFieldInit::ConvRotationQuaternionToYRadian(cQuat));
  
  return amx_ftoc(fAngle);
}

/**
 *  @brief  往来用復帰角取得
 *  @fix NMcat[2141]:往来NPCの反転モーション中に話しかけると会話後にモーションがスキップされる対処。戻す方向を取得するコマンドを新規追加、往来の場合は現在の向きではなく反転後の向きに戻すようにする
 */
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrGetTalkReturnAng_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32         EventID = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  f32 fAngle = 0.0f;
  Field::MoveModel::FieldMoveModel *pMoveModel = pFieldMoveModelManager->GetFieldMoveModel( ChrID );
  if( pMoveModel->GetMoveCodeId() == Field::MoveModel::FIELD_MOVE_CODE_TRAFFIC )
  {
    // 往来処理
    Field::MoveModel::FieldMoveCodeWork *pMoveCodeWork = pMoveModel->GetMoveCodeWork();
    Field::MoveModel::FieldMoveCodeTraffic::Work *pWork = NULL;
    u32 size = sizeof(Field::MoveModel::FieldMoveCodeTraffic::Work);
    pWork = static_cast<Field::MoveModel::FieldMoveCodeTraffic::Work*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, size ) );
    fAngle = gfl2::math::ConvRadToDeg( pWork->path->GetCurrentRotation().y );
  }
  else
  {
    // 他のキャラクタはChrGetAngに接続する
    poke_3d::model::BaseModel*  pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();
    gfl2::math::Quaternion  cQuat = pCharaDrawInstance->GetRotationQuat();
  
    /// クォータニオンから角を求める.
    fAngle = gfl2::math::ConvRadToDeg(FuncSetFieldInit::ConvRotationQuaternionToYRadian(cQuat));
  }

  return amx_ftoc(fAngle);
}

cell AMX_NATIVE_CALL FuncSetFieldInit::ChrCollision( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  bool  setCollision  = GFL_BOOL_CAST(ptr[2]);

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  // コリジョンセット
  if( setCollision ){
    pCharaModel->OffStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE );
  }else{
    pCharaModel->OnStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE );
  }
  return false;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::ChrFloat( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  bool  setFloat  = GFL_BOOL_CAST(ptr[2]);

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  // 接地のOn/Off
  pCharaModel->SetForceLanding(!setFloat);
  return false;
}
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrGetFloat(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);

  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return 0;
  }

  // 接地のOn/Off
  return !pCharaModel->IsForceLanding();
}


cell AMX_NATIVE_CALL FuncSetFieldInit::ChrDisp( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  bool  setDisp = GFL_BOOL_CAST(ptr[2]);

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  // 表示のOn/Off
  pCharaModel->GetCharaDrawInstance()->SetVisible( setDisp );

  // プレイヤーが乗っているライドポケモンにも表示のOnf/Offを流し込む NMCat[2403]
  if( pCharaModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    MoveModel::FieldMoveModelPlayer* pPlayer = static_cast<MoveModel::FieldMoveModelPlayer*>( pCharaModel );
    if( pPlayer->GetOnMoveModel() )
    {
      pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->SetVisible( setDisp );
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrShadowDisp
 * @brief       ChrShadowDisp関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:27
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrShadowDisp( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  s32     EventID     = ptr[1];                   ///< chr_id
  bool    setDisp     = GFL_BOOL_CAST(ptr[2]);
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID  = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  u32   characterID = pCharaModel->GetCharacterId();
  
  Field::MoveModel::ApplicationDataForCommon* pAppData = const_cast<Field::MoveModel::ApplicationDataForCommon*>(p_gman->GetGameData()->GetFieldCharaModelManager()->GetApplicationData( characterID ));
  
  if( pAppData->type == Field::MoveModel::TYPE_PLAYER )
  {
    Field::MoveModel::ApplicationDataForPlayer* pData = reinterpret_cast<Field::MoveModel::ApplicationDataForPlayer*>(pAppData);
    pData->shadowScale = static_cast<f32>(setDisp) * 100.f;
  }
  else if( pAppData->type == Field::MoveModel::TYPE_NPC )
  {
    Field::MoveModel::ApplicationDataForNPC* pData = reinterpret_cast<Field::MoveModel::ApplicationDataForNPC*>(pAppData);
    pData->shadowScale = static_cast<f32>(setDisp) * 100.f;
  }
  else if( pAppData->type == Field::MoveModel::TYPE_POKEMON )
  {
    Field::MoveModel::ApplicationDataForPokemon* pData = reinterpret_cast<Field::MoveModel::ApplicationDataForPokemon*>(pAppData);
    pData->shadowScale = static_cast<f32>(setDisp) * 100.f;
  }
  
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrPartsDisp
 * @brief       ChrPartsDisp関数.
 * @author      N.Takeda
 * @date        2015/10/08(木) 21:31:44
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrPartsDisp( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  u32   PartsID = ptr[2];
  bool  bDisp = GFL_BOOL_CAST(ptr[3]);  //!< (true:=表示ON, false:=表示OFF)
  
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );
  if( pMoveModel && pMoveModel->IsSetup() )
  {
    if( PartsID <= PLAYER_PARTS_TOPS )
    {
      //! 着せ替えモデルだけは処理が別.
      poke_3d::model::DressUpModel  *pDressUpModel = static_cast<poke_3d::model::DressUpModel*>( pMoveModel->GetCharaDrawInstance() );
      
      //  r:/home/niji/git_program/poke_3d_lib/model/include/gfl2_DressUpModel.h(35):   enum PartsModel
      //  R:/home/momiji/exec/reference_files/niji_reference_files/script/FieldPawnTypes.h(315):   PARTS_FACE,// 顔
      switch( PartsID ){
      case PLAYER_PARTS_FACE:      pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_FACE)->SetVisible( bDisp );      break;  // 顔
      case PLAYER_PARTS_HAIR:      pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_HAIR)->SetVisible( bDisp );      break;  // 髪
      case PLAYER_PARTS_ACCBADGE:  pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_ACCBADGE)->SetVisible( bDisp );  break;  // バッジアクセサリ
      case PLAYER_PARTS_ACCEYE:    pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_ACCEYE)->SetVisible( bDisp );    break;  // アイアクセサリ
      case PLAYER_PARTS_ACCHAIR:   pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_ACCHAIR)->SetVisible( bDisp );   break;  // ヘアアクセサリ
      case PLAYER_PARTS_BAG:       pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_BAG)->SetVisible( bDisp );       break;  // バッグ
      case PLAYER_PARTS_BNGL:      pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_BNGL)->SetVisible( bDisp );      break;  // バングル
      case PLAYER_PARTS_BODY:      pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_BODY)->SetVisible( bDisp );      break;  // ボディ
      case PLAYER_PARTS_BOTTOMS:   pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_BOTTOMS)->SetVisible( bDisp );   break;  // ボトムス
      case PLAYER_PARTS_HAT:       pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_HAT)->SetVisible( bDisp );       break;  // 帽子
      case PLAYER_PARTS_LEGS:      pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_LEGS)->SetVisible( bDisp );      break;  // レッグ
      case PLAYER_PARTS_SHOES:     pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_SHOES)->SetVisible( bDisp );     break;  // シューズ
      case PLAYER_PARTS_TOPS:      pDressUpModel->GetPartsModelInstanceNode(poke_3d::model::DressUpModel::PARTS_MODEL_TOPS)->SetVisible( bDisp );      break;  // トップス
      }
    }
    else
    {
      GFL_ASSERT( NPC_PARTS_EYE <= PartsID && PartsID <= NPC_PARTS_MOUTH );
      //! 一体型の場合.
      poke_3d::model::CharaModel* pCharaModel = pMoveModel->GetCharaDrawInstance();
      pCharaModel->GetModelInstanceNode()->GetMeshLinkData( PartsID - NPC_PARTS_EYE )->SetVisible( bDisp );
#if 0
      for( u32 i=0 ; i<pCharaModel->GetModelInstanceNode()->GetMeshNum() ; i++ )
      {
        GFL_PRINT( "MeshList: %s\n", pCharaModel->GetModelInstanceNode()->GetMeshLinkData(i)->GetName() );
      }
      // MeshLinkDataの引数は↓でも取得可能です
      // s32 index = pCharaModel->GetMeshIndex("XXX");
#endif
    }
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrSetDefaultIdle
 * @brief       キャラクターのデフォルト待機アニメIDを指定します
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetDefaultIdle( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  u32   animeID = ptr[2];
  bool  isMotionChange = GFL_BOOL_CAST(ptr[3]);
  s32   blendFrame = ptr[4];
  
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );
 
  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }

  // 座標
  pCharaModel->SetDefaultIdleAnimationId(animeID);
  
  if( isMotionChange ){
    if( blendFrame >= 0 ){
      //モーションを補間ありで再生
      pCharaModel->GetCharaDrawInstance()->ChangeAnimationSmooth(animeID,blendFrame);
    }else{
      //デザイナー指定のデフォルト補間フレームで再生
      pCharaModel->GetCharaDrawInstance()->ChangeAnimation(animeID);
    }
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrGetDefaultIdle
 * @brief       キャラクターのデフォルト待機アニメIDを取得します
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrGetDefaultIdle( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );
  
  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  return pCharaModel->GetDefaultIdleAnimationId();
}

//------------------------------------------------------------------------------
/**
 * @func        ChrCheckSittingTalk
 * @brief       キャラがしゃがみ対象か否かを得る.
 * @author      N.Takeda
 * @date        2015/08/27(木) 21:41:08
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrCheckSittingTalk( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  s32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  u32   characterID = pCharaModel->GetCharacterId();
  
  Field::MoveModel::ApplicationDataForCommon* pAppData = (Field::MoveModel::ApplicationDataForCommon*)pFieldMoveModelManager->GetApplicationData( characterID );
  if( !pAppData ) return 0;
  if( pAppData->type == Field::MoveModel::TYPE_NPC )
  {
    Field::MoveModel::ApplicationDataForNPC* pNpcData = (Field::MoveModel::ApplicationDataForNPC*)pAppData;
    return pNpcData->isSittingTalk;
  }
  else if( pAppData->type == Field::MoveModel::TYPE_POKEMON )
  {
    Field::MoveModel::ApplicationDataForPokemon* pPokemonData = (Field::MoveModel::ApplicationDataForPokemon*)pAppData;
    return pPokemonData->isSittingTalk;
  }
  
  return 0;
}


cell AMX_NATIVE_CALL FuncSetFieldInit::ChrDisableDispCntCulling(AMX *amx, const cell *ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  s32     EventID = ptr[1];                   ///< chr_id
  s32     flag = ptr[2];

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return 0;
  }

  pCharaModel->OnStateBit( Field::MoveModel::FIELD_MOVE_MODEL_DISP_CNT_CULLING_DISABLE );
  return 0;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::ChrDisableAppCulling(AMX *amx, const cell *ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  s32     EventID = ptr[1];                   ///< chr_id
  s32     flag = ptr[2];

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return 0;
  }
  if (flag)
  {
    pCharaModel->OnStateBit( Field::MoveModel::FIELD_MOVE_MODEL_APP_CULLING_DISABLE );
  }
  else
  {
    pCharaModel->OffStateBit( Field::MoveModel::FIELD_MOVE_MODEL_APP_CULLING_DISABLE );
  }
  return 0;
}

// @fix MMcat[409]:ホクラニ天文台にて、自機が入口付近に居るとマーレインの影が正しく表示されない、対処。表示数オーバーが発生していたので、明示的に切る機能を追加
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetStateBitShadowOff(AMX *amx, const cell *ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  s32     EventID = ptr[1];                   ///< chr_id
  s32     flag = ptr[2];

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return 0;
  }
  if (flag)
  {
    pCharaModel->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_SHADOW_OFF);
  }
  else
  {
    pCharaModel->OffStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_SHADOW_OFF);
  }
  return 0;
}

// @fix MMcat[607]:特異なケースにおいて、レジスト状態チェックを明示的に切る機能を追加
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetStateBitInvalidResistCheck(AMX *amx, const cell *ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  s32     EventID = ptr[1];                   ///< chr_id
  s32     flag = ptr[2];

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return 0;
  }
  if (flag)
  {
    pCharaModel->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_RESIST_CHECK_INVALID);
  }
  else
  {
    pCharaModel->OffStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_RESIST_CHECK_INVALID);
  }
  return 0;
}


cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetStateBitCullingAabbMinimun(AMX *amx, const cell *ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  s32     EventID = ptr[1];                   ///< chr_id
  s32     flag = ptr[2];

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return 0;
  }
  if (flag)
  {
    pCharaModel->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_CULLING_AABB_MINIMUM);
  }
  else
  {
    pCharaModel->OffStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_CULLING_AABB_MINIMUM);
  }
  return 0;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetStateBitCullingLengthOptDisable(AMX *amx, const cell *ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  s32     EventID = ptr[1];                   ///< chr_id
  s32     flag = ptr[2];

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return 0;
  }
  if (flag)
  {
    pCharaModel->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_CULLING_LENGTH_OPT_DISABLE);
  }
  else
  {
    pCharaModel->OffStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_CULLING_LENGTH_OPT_DISABLE);
  }
  return 0;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetMotionMoveDisableFlag(AMX *amx, const cell *ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  s32     EventID = ptr[1];                   ///< chr_id
  s32     flag = ptr[2];

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return 0;
  }

  if( flag )
  {
    pCharaModel->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE);
  }
  else
  {
    pCharaModel->OffStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE);
  }
  return 0;
}


cell AMX_NATIVE_CALL FuncSetFieldInit::ChrMotionGetEndFrame(AMX *amx, const cell *ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  s32     EventID = ptr[1];                   ///< chr_id
  s32     flag = ptr[2];

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);


  f32 return_val(0.0f);
  if (pCharaModel == NULL || pCharaModel->GetCharaDrawInstance() == NULL ){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return amx_ftoc( return_val );
  }

  poke_3d::model::CharaModel* pPokeCharaModel = pCharaModel->GetCharaDrawInstance();
  return_val = pPokeCharaModel->GetAnimationEndFrame();
  return amx_ftoc(return_val);
}
//------------------------------------------------------------------------------
/**
 * @brief       キャラのモーションが終了するのを監視する
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrMotionEndWatchStart( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32     EventID     = ptr[1];                   ///< chr_id
  u32     next_anime_id = ptr[2];

  if( p_sys->ChrMotionEndWatchStart( EventID, next_anime_id ) )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief       キャラのモーションが終了するのを監視する、終了
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrMotionEndWatchClear( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32     EventID     = ptr[1];                   ///< chr_id

  p_sys->ChrMotionEndWatchClear( EventID );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       指定キャラに当てられている動作コードがNPC挙動関連か取得する
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrIsNpcMoveCode( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  s32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32 moveModelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( moveModelIndex == MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  
  Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel( moveModelIndex );
  return Field::MoveModel::FieldMoveCodeManager::IsNpcMoveCode( pMoveModel->GetMoveCodeId() );
}

//------------------------------------------------------------------------------
/**
 * @brief       NPCのオプションをONする
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::NpcOnOption( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  s32     EventID     = ptr[1];                   ///< chr_id
  u32     optionBit   = ptr[2];
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   moveModelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( moveModelIndex == MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel( moveModelIndex );
  if( !Field::MoveModel::FieldMoveCodeManager::IsNpcMoveCode( pMoveModel->GetMoveCodeId() ) )
  {
    GFL_ASSERT_MSG(0,"EventID(%d) is not NPC MoveCode \n", EventID );
    return 0;
  }

  MoveModel::FieldMoveCodeNpc::OnOption( pMoveModel->GetMoveCodeWork(), optionBit );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       NPCのオプションをOFFする
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::NpcOffOption( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  s32     EventID     = ptr[1];                   ///< chr_id
  u32     optionBit   = ptr[2];
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   moveModelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( moveModelIndex == MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel( moveModelIndex );
  if( !Field::MoveModel::FieldMoveCodeManager::IsNpcMoveCode( pMoveModel->GetMoveCodeId() ) )
  {
    GFL_ASSERT_MSG(0,"EventID(%d) is not NPC MoveCode \n", EventID );
    return 0;
  }

  MoveModel::FieldMoveCodeNpc::OffOption( pMoveModel->GetMoveCodeWork(), optionBit );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       NPCのオプションをリセットする
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::NpcResetOption( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  s32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   moveModelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( moveModelIndex == MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel( moveModelIndex );
  if( !Field::MoveModel::FieldMoveCodeManager::IsNpcMoveCode( pMoveModel->GetMoveCodeId() ) )
  {
    GFL_ASSERT_MSG(0,"EventID(%d) is not NPC MoveCode \n", EventID );
    return 0;
  }

  MoveModel::FieldMoveCodeNpc::ResetOption( pMoveModel->GetMoveCodeWork() );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       NPCの会話済みフラグをリセットする
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::NpcClearTalkEnd( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  s32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   moveModelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( moveModelIndex == MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel( moveModelIndex );
  if( !Field::MoveModel::FieldMoveCodeManager::IsNpcMoveCode( pMoveModel->GetMoveCodeId() ) )
  {
    GFL_ASSERT_MSG(0,"EventID(%d) is not NPC MoveCode \n", EventID );
    return 0;
  }

  MoveModel::FieldMoveCodeNpc::ClearTalkEnd( pMoveModel->GetMoveCodeWork() );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       NPCの向き汚れフラグをリセットする
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::NpcClearDirtyFlag( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  s32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   moveModelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( moveModelIndex == MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel( moveModelIndex );
  if( !Field::MoveModel::FieldMoveCodeManager::IsNpcMoveCode( pMoveModel->GetMoveCodeId() ) )
  {
    GFL_ASSERT_MSG(0,"EventID(%d) is not NPC MoveCode \n", EventID );
    return 0;
  }

  MoveModel::FieldMoveCodeNpc::ClearDirtyFlag( pMoveModel->GetMoveCodeWork() );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       NPCの戻す向きを設定する
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::NpcOverWriteReturnAngle_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  s32     EventID     = ptr[1];                   ///< chr_id
  f32     angleDeg = amx_ctof( ptr[2] );
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   moveModelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( moveModelIndex == MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }

  Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel( moveModelIndex );
  if( !Field::MoveModel::FieldMoveCodeManager::IsNpcMoveCode( pMoveModel->GetMoveCodeId() ) )
  {
    GFL_ASSERT_MSG(0,"EventID(%d) is not NPC MoveCode \n", EventID );
    return 0;
  }

  MoveModel::FieldMoveCodeNpc::OverWriteReturnAngle( pMoveModel->GetMoveCodeWork(), angleDeg );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       プレイヤーのフリームーブチェックステートをリセットする
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 * 
 * GFNMCat[5410] ベッドイベントでバミリアサート
 * 
 * ロケーター吸着が最終地点のイベントなど、システム上バミリを最終移動地点に出来ない
 * 一部スクリプトのバミリアサート回避のために
 * フリームーブステータスを強制クリアするコマンドを提供する
 *
 * 利用箇所は運用で制御する。利用許可制コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::PlayerFreeMoveStateReset( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer* pPlayer = 
    static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  //プレイヤーが居ない事はないはずだが、一応チェック
  if( pPlayer != NULL ){
    // @todo
    pPlayer->ClearPositionCheckForEventScript();
  }

  return 0;
}
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrFreeMoveStateReset(AMX *amx, const cell *ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   EventID = ptr[1];
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);
  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "%d\n", EventID);
    return false;
  }

  pCharaModel->ClearPositionCheckForEventScript();
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        ModelSearchByCharacterID
 * @brief       ModelSearchByCharacterID関数.
 * @author      N.Takeda
 * @date        2015/11/13(金) 12:46:24
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ModelSearchByCharacterID(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  
  u32   CharId  = ptr[1];   ///<  ../../../exec/conv_header/niji_conv_header/field/chara/chara_model_id.h
  
  
  for( s32 ii=0 ; ii<Field::MoveModel::FIELD_MOVE_MODEL_MAX ; ii++ )
  {
    if( pFieldMoveModelManager->GetFieldMoveModel( ii )==NULL) continue;

    if( pFieldMoveModelManager->GetFieldMoveModel( ii )->GetCharacterId() == CharId )
    {
      s32 id = pFieldMoveModelManager->GetFieldMoveModel( ii )->GetEventId();
      return id;
    }
  }
  
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        ObjAdd
 * @brief       表示切替オブジェクトの追加.
 * @author      N.Takeda
 * @date        2015/10/16(金) 19:03:45
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ObjAdd( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 eventId = ptr[1];
  
  Field::FieldResident* pFieldResident = p_sys->GetGameManager()->GetFieldResident();

  if( pFieldResident == NULL ) return false;

  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  pPlacementDataManager->RegistZoneEventIDItem( eventId );
  pPlacementDataManager->RegistZoneEventIDBgEvent( eventId );
  pPlacementDataManager->RegistZoneEventIDStaticModel( eventId );
  pPlacementDataManager->RegisterZoneFishingSpot( eventId );

  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ObjDel
 * @brief       表示切替オブジェクトの削除.
 * @author      N.Takeda
 * @date        2015/10/16(金) 19:03:47
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ObjDel( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 eventId = ptr[1];
  
  Field::FieldResident* pFieldResident = p_sys->GetGameManager()->GetFieldResident();

  if( pFieldResident == NULL ) return false;

  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  pPlacementDataManager->UnRegistZoneEventIDItem( eventId );
  pPlacementDataManager->UnRegistZoneEventIDBgEvent( eventId );
  pPlacementDataManager->UnRegistZoneEventIDStaticModel( eventId );
  pPlacementDataManager->UnregisterZoneFishingSpot(eventId);

  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ObjAddAll
 * @brief       種類を問わないオブジェクトの追加.
 * @author      Ohhira Syo
 * @date        2016/01/15(金) 15:47:00
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ObjAddAll( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 eventId = ptr[1];
  
  Field::FieldResident* pFieldResident = p_sys->GetGameManager()->GetFieldResident();

  if( pFieldResident == NULL ) return false;

  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  pPlacementDataManager->RegistZoneEventIDItem( eventId );
  pPlacementDataManager->RegistZoneEventIDBgEvent( eventId );
  pPlacementDataManager->RegistZoneEventIDStaticModel( eventId );
  pPlacementDataManager->RegisterZoneFishingSpot( eventId );

  pPlacementDataManager->RegistZoneEventIDMapJump( eventId );
  pPlacementDataManager->RegistZoneEventIDActionEvent( eventId );
  pPlacementDataManager->RegistZoneEventIDModel( eventId, p_sys->GetGameManager()->GetGameData()->GetEventWork(), false );
  pPlacementDataManager->RegistZoneEventIDTrainer( eventId );
  pPlacementDataManager->RegistZoneEventIDGimmickEncount( eventId );

  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ObjDellAll
 * @brief       種類を問わないオブジェクトの追加.
 * @author      Ohhira Syo
 * @date        2016/01/15(金) 15:47:00
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ObjDelAll( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 eventId = ptr[1];
  
  Field::FieldResident* pFieldResident = p_sys->GetGameManager()->GetFieldResident();

  if( pFieldResident == NULL ) return false;

  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  pPlacementDataManager->UnRegistZoneEventIDItem( eventId );
  pPlacementDataManager->UnRegistZoneEventIDBgEvent( eventId );
  pPlacementDataManager->UnRegistZoneEventIDStaticModel( eventId );
  pPlacementDataManager->UnregisterZoneFishingSpot(eventId);

  pPlacementDataManager->UnRegistZoneEventIDMapJump( eventId );
  pPlacementDataManager->UnRegistZoneEventIDActionEvent( eventId );
  pPlacementDataManager->UnRegistZoneEventIDModel( eventId, p_sys->GetGameManager()->GetGameData()->GetEventWork(), false );
  pPlacementDataManager->UnRegistZoneEventIDTrainer( eventId );
  pPlacementDataManager->UnRegistZoneEventIDGimmickEncount( eventId );

  return true;
}

/*
 *  イベント配置物の座標を取得
 **/
b32 FuncSetFieldInit::GetActorPos(GameSys::GameManager* pGameManager, s32 eventId, b32 isCalcEventBasePos, gfl2::math::Vector3* pPos )
{
  Field::FieldResident* pFieldResident = pGameManager->GetFieldResident();
  FieldScriptSystem*    pFieldScriptSystem = pGameManager->GetFieldScriptSystem();
  
  if( pFieldResident == NULL ) return false;
  Field::PlacementDataManager   *pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  IFieldActorBase *pActor = NULL;
  pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBgEvent(eventId));
  if (!pActor) pActor = pPlacementDataManager->SearchZoneMapJump(eventId);
  if (!pActor) pActor = pPlacementDataManager->SearchZoneItem(eventId);
  if (!pActor) pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBerryPointFromEventID(eventId));
  if (!pActor) pActor = pPlacementDataManager->SearchZoneEventIDStaticModel(eventId);
  if (!pActor) return false;

  gfl2::math::Vector3 pos;
  pos = pActor->GetPosision();

  if (isCalcEventBasePos)
  {
    pos = pFieldScriptSystem->CalcEventPositionOffset( pos );
  }
 
  *pPos = pos;
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief       イベントオブジェクトの座標を取得
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ObjGetPosX( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 eventId = ptr[1];
  s32 is_calc_event_base_pos = ptr[2];
  
  gfl2::math::Vector3 pos;
  if (GetActorPos(p_gman, eventId, is_calc_event_base_pos, &pos))
  {
    return amx_ftoc(pos.x);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "ObjGetPosX:event_id(%d)が存在しないか、対応していない種類のデータです\n", eventId );
    f32 value = 0.0f;
    return amx_ftoc(value);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief       イベントオブジェクトの座標を取得
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ObjGetPosY( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 eventId = ptr[1];
  s32 is_calc_event_base_pos = ptr[2];
  
  gfl2::math::Vector3 pos;
  if (GetActorPos(p_gman, eventId, is_calc_event_base_pos, &pos))
  {
    return amx_ftoc(pos.y);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "ObjGetPosX:event_id(%d)が存在しないか、対応していない種類のデータです\n", eventId );
    f32 value = 0.0f;
    return amx_ftoc(value);
  }
}
//------------------------------------------------------------------------------
/**
 * @brief       イベントオブジェクトの座標を取得
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ObjGetPosZ( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 eventId = ptr[1];
  s32 is_calc_event_base_pos = ptr[2];
  
  gfl2::math::Vector3 pos;
  if (GetActorPos(p_gman, eventId, is_calc_event_base_pos, &pos))
  {
    return amx_ftoc(pos.z);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "ObjGetPosX:event_id(%d)が存在しないか、対応していない種類のデータです\n", eventId );
    f32 value = 0.0f;
    return amx_ftoc(value);
  }
}


//------------------------------------------------------------------------------
/**
 * @func        ZigarudeCellStatusSet
 * @brief       ジガルデセルの表示切替
 * @author      Miyuki Iwasawa
 * @date        2016/02/14
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ZigarudeCellStatusSet( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  u32 eventId = ptr[1];
  s32 status = ptr[2];
  
  Field::FieldResident* pFieldResident = p_sys->GetGameManager()->GetFieldResident();

  if( pFieldResident == NULL ) return false;

  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  {
    FieldBgEventActor* pBgActor = pPlacementDataManager->SearchZoneBgEvent(eventId);
    
    if( pBgActor != NULL ){
      pBgActor->SetEventFlagWork(status);
    }
  }

  if( status == SEQ_ZIGARUDE_CELL_ON ){
    pPlacementDataManager->RegistZoneEventIDBgEvent( eventId );
  }else{
    pPlacementDataManager->UnRegistZoneEventIDBgEvent( eventId );
  } 
  return true;
}

/**
 * @brief       ポケファインダーチュートリアル用入れ替え
 */
cell AMX_NATIVE_CALL FuncSetFieldInit::PokeFinderAddDel( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32 flagwork = ptr[1];
  
  Field::FieldResident* pFieldResident = p_gman->GetFieldResident();
  if( pFieldResident == NULL ){ return 0; }

  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();
  // flagworkにかかわるファインダーを破棄
  pPlacementDataManager->UnRegistZonePokeFinder( flagwork );
  // flagworkにかかわるファインダーを生成
  pPlacementDataManager->RegistZonePokeFinder( flagwork );

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        SetEnvSoundStatus
 * @brief       EnvSoundStatusのセット.
 * @author      N.Takeda
 * @date        2015/11/12(木) 13:33:46
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::EnvSoundStatusSet(AMX * amx, const cell * ptr )
{
//  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
//  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem     *pSys           = FieldScriptSystem::GetInstance();
  GameSys::GameManager  *p_gman         = pSys->GetGameManager();
  
  s32   se_id  = ptr[1];
  bool  enable_flag = GFL_BOOL_CAST(ptr[2]);
  
  GFL_ASSERT( p_gman );
  Field::FieldResident      *pFieldResident = p_gman->GetFieldResident();
  GFL_ASSERT( pFieldResident );
  Field::EnvSoundManager    *pEnvSound = (pFieldResident) ? pFieldResident->GetEnvSoundManager() : NULL;
  
  GFL_ASSERT( pEnvSound );
  if( pEnvSound ) pEnvSound->SetEnableElem( enable_flag, se_id );
  
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func    EnvSEVolumeChange
 * @brief   環境音ボリューム変更
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::EnvSoundVolumeChange( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  
  f32 volume = amx_ctof( ptr[1] );
  Sound::FadeFrame fadeFrame = static_cast<Sound::FadeFrame>(ptr[2]);

  p_gman->GetGameData()->GetFieldSound()->ChangeEnvSoundVolume(volume,fadeFrame);

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func    FootSoundVolumeChange
 * @brief   足音ボリューム変更
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::FootSoundVolumeChange( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  
  f32 volume = amx_ctof( ptr[1] );
  Sound::FadeFrame fadeFrame = static_cast<Sound::FadeFrame>(ptr[2]);

  p_gman->GetGameData()->GetFieldSound()->ChangeFootSoundVolume(volume,fadeFrame);

  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func    WeatherSoundVolumeChange
 * @brief   天候音ボリューム変更
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::WeatherSoundVolumeChange( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  
  f32 volume = amx_ctof( ptr[1] );
  Sound::FadeFrame fadeFrame = static_cast<Sound::FadeFrame>(ptr[2]);

  p_gman->GetGameData()->GetFieldSound()->ChangeWeatherSoundVolume(volume,fadeFrame);

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @func ChangeWeather_
 * @brief 天候の変更
 * @author saito
 * @date 2015/12/21
 * @note フィールドマップがセットアップされている状態で使用する必要がある
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetFieldInit::ChangeWeather( AMX * amx, const cell * ptr )
{
  ScriptObject*           p_obj             = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*   p_game_manager    = p_obj->GetGameManager();
  GameSys::GameData*      p_game_data       = p_game_manager->GetGameData();
  Fieldmap*               pFieldmap         = p_game_manager->GetFieldmap();

  ReplaceGroup  group = (ReplaceGroup)ptr[1];
  weather::WeatherKind weather = (weather::WeatherKind)ptr[2];

  u8 idx = (u8)group;
  //セーブデータにアクセス
  Savedata::WeatherSave *p_weather_save = p_game_data->GetWeatherSave();
  //セーブデータに天候セット
  p_weather_save->SetWeather(idx, weather);
  //現在のゾーンが所属している天候リプレイスグループと指定天候リプレイスグループが異なるかで分岐
  ZoneID zone_id = pFieldmap->GetZoneID();
  if ( weather::WeatherControl::GetReplaceGroup( zone_id ) == group )
  {
    //天候変更リクエスト
    Field::weather::WeatherControl *p_weather_ctrl = pFieldmap->GetWeatherControl();
    p_weather_ctrl->SetForceWeather(weather);
  }

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @func ResetWeather_
 * @brief 天候のリセット
 * @author saito
 * @date 2015/12/21
 * @note フィールドマップがセットアップされている状態で使用する必要がある
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetFieldInit::ResetWeather( AMX * amx, const cell * ptr )
{
  ScriptObject*           p_obj             = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*   p_game_manager    = p_obj->GetGameManager();
  GameSys::GameData*      p_game_data       = p_game_manager->GetGameData();
  Fieldmap*               pFieldmap         = p_game_manager->GetFieldmap();

  ReplaceGroup  group = (ReplaceGroup)ptr[1];
  
  u8 idx = (u8)group;
  //セーブデータにアクセス
  Savedata::WeatherSave *p_weather_save = p_game_data->GetWeatherSave();
  //セーブデータの天候リセット
  p_weather_save->ResetWeather(idx);
  //現在のゾーンが所属している天候リプレイスグループと指定天候リプレイスグループが異なるかで分岐
  ZoneID zone_id = pFieldmap->GetZoneID();
  if ( weather::WeatherControl::GetReplaceGroup( zone_id ) == group )
  {
    //天候変更リクエスト
    Field::weather::WeatherControl *p_weather_ctrl = pFieldmap->GetWeatherControl();
    p_weather_ctrl->ResetForceWeather();
  }

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief 現在の天候を取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::GetNowWeather( AMX * amx, const cell * ptr )
{
  ScriptObject*           p_obj             = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*   p_game_manager    = p_obj->GetGameManager();
  GameSys::GameData*      p_game_data       = p_game_manager->GetGameData();
  Fieldmap*               pFieldmap         = p_game_manager->GetFieldmap();

  return pFieldmap->GetWeatherControl()->GetNowWeatherKind();
}


//-----------------------------------------------------------------------------
/**
 * @brief ロトムイベントPos起動リクエスト
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::RotomPosEventSet( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman  = p_sys->GetGameManager();
  u32 msg_id = ptr[1];

  Fieldmap* p_fieldmap = p_gman->GetFieldmap();
  Field::SubScreen::FieldSubScreenProcManager* p_subsreen = p_fieldmap->GetFieldSubScreenManager();
  p_subsreen->SetRotomPosEvent(msg_id);    

  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        StaticModelMotionPlay
 * @brief       StaticModelをアニメさせる.
 * @author      N.Takeda
 * @date        2015/11/12(木) 20:42:57
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionPlay(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventID = ptr[1];
  s32   AnimId  = ptr[2];
  f32   fSpeed  = amx_ctof(ptr[3]);
  bool  isLoop  = GFL_BOOL_CAST(ptr[4]);
  
  Field::StaticModel::StaticModel *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  
  GFL_ASSERT( pStaticModel );
  StaticModel::StaticModel::Handle_StartEventAnimation handle( AnimId, fSpeed, isLoop );
  if( pStaticModel )  pStaticModel->Set( &handle);
  
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionPlaySmooth(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();

  u32   EventID = ptr[1];
  u32   SlotID  = ptr[2];
  s32   AnimId  = ptr[3];
  f32   fSpeed  = amx_ctof(ptr[4]);
  bool  isLoop  = GFL_BOOL_CAST(ptr[5]);
  u32   lerpFrame  = ptr[6];
  u32   startFrame = ptr[7];
  
  Field::StaticModel::StaticModel *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );

  GFL_ASSERT( pStaticModel );
  if( pStaticModel->CanUseChangeAnimeSmooth() == false ){
    GFL_ASSERT_MSG( 0,"登録済みモデル以外では使用不可\n" );
    return false;
  }

  StaticModel::StaticModel::Handle_StartEventAnimationEx handle( SlotID, AnimId, fSpeed, isLoop, lerpFrame, startFrame );
  if( pStaticModel )  pStaticModel->Set( &handle);

  return true;
}



//------------------------------------------------------------------------------
/**
 * @func        StaticModelMotionIsEnd
 * @brief       StaticModelのアニメは終了したか？
 * @author      N.Takeda
 * @date        2015/11/12(木) 20:42:55
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionIsEnd(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventID = ptr[1];
  s32   AnimId  = ptr[2];
  
  StaticModel::StaticModel  *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  StaticModel::StaticModel::Handle_IsEndEventAnimation handle( AnimId );
  
  GFL_ASSERT( pStaticModel );
  return ( pStaticModel ) ? pStaticModel->IsEnd( handle) : true;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionIsEndSmooth(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();

  u32   EventID = ptr[1];
  u32   SlotID  = ptr[2];
  s32   AnimId  = ptr[3];

  StaticModel::StaticModel  *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  StaticModel::StaticModel::Handle_IsEndEventAnimationEx handle( SlotID, AnimId );

  GFL_ASSERT( pStaticModel );
  if( pStaticModel->CanUseChangeAnimeSmooth() == false ){
    GFL_ASSERT_MSG( 0,"登録済みモデル以外では使用不可\n" );
    return true;
  }
  return ( pStaticModel ) ? pStaticModel->IsEnd( handle) : true;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  配置モデルのイベント逆再生アニメ開始
 *
 *  @param  event_id 配置モデルのイベントID
 *  @param  index イベントアニメID
 *  @param  is_start_frame true=アニメ終端から逆再生, false=アニメの途中から逆再生
 *  @param  Float:speed 再生スピード（1.0が等速)
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionPlayInv(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventID = ptr[1];
  s32   AnimId        = ptr[2];
  bool  isStartFrame  = GFL_BOOL_CAST(ptr[3]);
  f32   fSpeed        = amx_ctof(ptr[4]);
  bool  isLoop        = GFL_BOOL_CAST(ptr[5]);
  
  Field::StaticModel::StaticModel *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  
  GFL_ASSERT( pStaticModel );
  if( isStartFrame == false )
  {
    StaticModel::StaticModel::Handle_RestartEventAnimation handle( AnimId, fSpeed * -1.f ); ///< -で逆再生.
    if( pStaticModel )  pStaticModel->Set( &handle);
  }else{
    {
      StaticModel::StaticModel::Handle_StartEventAnimation handle( AnimId, fSpeed * -1.f, isLoop ); ///< -で逆再生.
      if( pStaticModel )  pStaticModel->Set( &handle);
    }
    {
      StaticModel::StaticModel::Handle_SetLastFrameEventAnimation handle( AnimId );
      if( pStaticModel )  pStaticModel->Set( &handle);
    }
  }
  return NULL;
}
//-----------------------------------------------------------------------------
/**
 *  @brief  配置モデルを終了フレーム状態にする。
 *
 *  @param  event_id 配置モデルのイベントID
 *  @param  index イベントアニメID
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionSetEnd(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventID = ptr[1];
  s32   AnimId  = ptr[2];
  
  StaticModel::StaticModel  *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  StaticModel::StaticModel::Handle_EndEventAnimation handle( AnimId );
  
  GFL_ASSERT( pStaticModel );
  if( pStaticModel )  pStaticModel->Set( &handle);
  
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  配置モデルのイベントアニメをリセット（頭だし）
 *
 *  @param  event_id 配置モデルのイベントID
 *  @param  index イベントアニメID
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionReset(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventID = ptr[1];
  s32   AnimId  = ptr[2];
  
  StaticModel::StaticModel  *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  StaticModel::StaticModel::Handle_SetFrameEventAnimation handle( AnimId, 0 );
  
  GFL_ASSERT( pStaticModel );
  if( pStaticModel )  pStaticModel->Set( &handle);
  
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  配置モデルのイベントアニメをとめる（アンバインドする）
 *
 *  @param  event_id 配置モデルのイベントID
 *  @param  index イベントアニメID
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionStop(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventID = ptr[1];
  s32   AnimId  = ptr[2];
  
  StaticModel::StaticModel  *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  
  GFL_ASSERT( pStaticModel );
  StaticModel::StaticModel::Handle_StopEventAnimation handle( AnimId );
  if( pStaticModel )  pStaticModel->Set( &handle);
  
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  配置モデルのイベントアニメをとめる（アンバインドしない）
 *
 *  @param  event_id 配置モデルのイベントID
 *  @param  index イベントアニメID
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionPause(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventID = ptr[1];
  s32   AnimId  = ptr[2];
  
  StaticModel::StaticModel  *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  StaticModel::StaticModel::Handle_PauseEventAnimation handle( AnimId );
  
  GFL_ASSERT( pStaticModel );
  if( pStaticModel )  pStaticModel->Set( &handle);
  
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  配置モデルのイベントアニメを再開する
 *
 *  @param  event_id 配置モデルのイベントID
 *  @param  index イベントアニメID
 *  @param  Float:speed 再生スピード
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionRestart(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventID   = ptr[1];
  s32   AnimId    = ptr[2];
  f32   fSpeed    = amx_ctof(ptr[3]);
  
  StaticModel::StaticModel  *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  StaticModel::StaticModel::Handle_RestartEventAnimation handle( AnimId, fSpeed );
  
  GFL_ASSERT( pStaticModel );
  if( pStaticModel )  pStaticModel->Set( &handle);
  
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  配置モデルの全ての常駐アニメを止める
 *
 *  @param  event_id 配置モデルのイベントID
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionAllStop(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventID   = ptr[1];
  
  StaticModel::StaticModel  *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  
  GFL_ASSERT( pStaticModel );
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  配置モデルの全ての常駐アニメを再開
 *
 *  @param  event_id 配置モデルのイベントID
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionAllRestart(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventID   = ptr[1];
  
  StaticModel::StaticModel  *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  
  GFL_ASSERT( pStaticModel );
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        StaticModelLoopMotionPlay
 * @brief       ループアニメーションさせる.
 * @author      N.Takeda
 * @date        2015/11/18(水) 17:22:52
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelLoopMotionPlay(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventID   = ptr[1];
  f32   fStartFrame  = amx_ctof(ptr[2]);
  
  Field::StaticModel::StaticModel *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  
  GFL_ASSERT( pStaticModel );
  StaticModel::StaticModel::Handle_StartLoopAnimation handle( fStartFrame );
  if( pStaticModel )  pStaticModel->Set( &handle);
  
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ループアニメーションをとめる.
 *
 *  @param  event_id 配置モデルのイベントID
 *  @param  index イベントアニメID
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelLoopMotionStop(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32   EventId  = ptr[1];
  
  StaticModel::StaticModel  *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventId );
  
  GFL_ASSERT( pStaticModel );
  StaticModel::StaticModel::Handle_StopLoopAnimation handle;
  if( pStaticModel )  pStaticModel->Set( &handle);
  
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        StaticModelMotionByStaticIdPlay
 * @brief       StaticModelをアニメさせる.
 * @author      Ohhira Syo
 * @date        2015/11/13(金) 17:54:00
 * @date        2015/12/03(木) 20:23:18 N.Takeda  逆再生に対応.
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionByStaticIdPlay(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  s32   staticId  = ptr[1];
  s32   AnimId  = ptr[2];
  f32   speed = amx_ctof(ptr[3]);
  
  StaticModel::StaticModel *pStaticModel = pFieldmap->GetStaticModelManager()->GetStaticModelIDStaticModel( staticId );
  
  GFL_ASSERT( pStaticModel );
  StaticModel::StaticModel::Handle_StartEventAnimation handle( AnimId, speed );
  if( pStaticModel )  pStaticModel->Set( &handle);
  
  if( 0.f > speed )
  {
    /// 逆再生.
    StaticModel::StaticModel::Handle_SetLastFrameEventAnimation handle( AnimId );
    if( pStaticModel )  pStaticModel->Set( &handle);
  }
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        StaticModelMotionByStaticIdIsEnd
 * @brief       StaticModelのアニメは終了したか？
 * @author      Ohhira Syo
 * @date        2015/11/13(金) 17:54:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionByStaticIdIsEnd(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  s32   staticId  = ptr[1];
  s32   AnimId  = ptr[2];
  
  StaticModel::StaticModel *pStaticModel = pFieldmap->GetStaticModelManager()->GetStaticModelIDStaticModel( staticId );
  StaticModel::StaticModel::Handle_IsEndEventAnimation handle( AnimId );
  
  GFL_ASSERT( pStaticModel );
  return ( pStaticModel ) ? pStaticModel->IsEnd( handle) : true;
}

//------------------------------------------------------------------------------
/**
 * @func        StaticModelMotionByStaticIdStop
 * @brief       StaticModelのアニメを終了させる.
 * @author      Ohhira Syo
 * @date        2015/11/13(金) 17:54:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionByStaticIdStop(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  s32   staticId  = ptr[1];
  s32   AnimId  = ptr[2];
  
  StaticModel::StaticModel *pStaticModel = pFieldmap->GetStaticModelManager()->GetStaticModelIDStaticModel( staticId );
  
  GFL_ASSERT( pStaticModel );
  StaticModel::StaticModel::Handle_StopEventAnimation handle( AnimId );
  if( pStaticModel )  pStaticModel->Set( &handle);
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        StaticModelMotionByStaticIdSetFrame
 * @brief       StaticModelのアニメのフレーム指定
 * @author      Ohhira Syo
 * @date        2015/11/16(月) 16:49:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelMotionByStaticIdSetFrame(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  s32   staticId  = ptr[1];
  s32   AnimId  = ptr[2];
  s32   frame  = ptr[3];
  
  StaticModel::StaticModel *pStaticModel = pFieldmap->GetStaticModelManager()->GetStaticModelIDStaticModel( staticId );
  
  GFL_ASSERT( pStaticModel );
  StaticModel::StaticModel::Handle_SetFrameEventAnimation handle( AnimId, frame );
  if( pStaticModel )  pStaticModel->Set( &handle);
  
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::StaticModelSetPosAng(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();

  u32   EventID = ptr[1];
  f32   fX = amx_ctof(ptr[2]);
  f32   fY = amx_ctof(ptr[3]);
  f32   fZ = amx_ctof(ptr[4]);
  f32   fAngY = amx_ctof(ptr[5]);

  StaticModel::StaticModel *pStaticModel = pFieldmap->GetStaticModelManager()->GetEventIDStaticModel( EventID );
  if (pStaticModel == NULL)
  {
    GFL_ASSERT( 0 );
    return NULL;
  }
  gfl2::math::SRT srt = pStaticModel->GetLocalSRT();
  srt.translate.x = fX;
  srt.translate.y = fY;
  srt.translate.z = fZ;

  // @fix MMcat[255]:パネルの床の向きが正解の向きと相違、対処。回転の値設定が不正だった
  srt.rotation.RadianYToQuaternion(gfl2::math::ConvDegToRad(fAngY));
    
  pStaticModel->SetLocalSRT( srt );
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        S5_StaticModelMotionPlay
 * @brief       StaticModelをアニメさせる(電気試練用)
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::S5_GimmickModelMotionPlay(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  GameSys::GameManager*   pGameManager = pFieldmap->GetGameManager();

  u32   model_id = ptr[1];
  s32   slot_index = ptr[2];
  s32   anime_id = ptr[3];
  f32   stepFrame = amx_ctof(ptr[4]);
  bool  isLoop = GFL_BOOL_CAST(ptr[5]);


  FieldGimmick* gimmick = pGameManager->GetFieldmap()->GetFieldGimmick(Fieldmap::GIMMICK_IDX_ZONE);
  FieldGimmickShirenDenki* gimmick_shiren_denki = static_cast<FieldGimmickShirenDenki*>(gimmick);
  poke_3d::model::BaseModel* pModel = gimmick_shiren_denki->GetModel( model_id );
  GFL_ASSERT( pModel );

  pModel->ChangeAnimation( anime_id, slot_index );
  pModel->SetAnimationLoop( isLoop, slot_index );
  pModel->SetAnimationStepFrame( stepFrame, slot_index );
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::S5_GimmickModelMotionSetEnd(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  GameSys::GameManager*   pGameManager = pFieldmap->GetGameManager();

  u32   model_id = ptr[1];
  s32   slot_index = ptr[2];

  FieldGimmick* gimmick = pGameManager->GetFieldmap()->GetFieldGimmick(Fieldmap::GIMMICK_IDX_ZONE);
  FieldGimmickShirenDenki* gimmick_shiren_denki = static_cast<FieldGimmickShirenDenki*>(gimmick);
  poke_3d::model::BaseModel* pModel = gimmick_shiren_denki->GetModel(model_id);
  GFL_ASSERT(pModel);


  pModel->SetAnimationFrame( pModel->GetAnimationEndFrame( slot_index ), slot_index );
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::S5_GimmickModelMotionIsEnd(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  GameSys::GameManager*   pGameManager = pFieldmap->GetGameManager();

  u32   model_id = ptr[1];
  s32   slot_index = ptr[2];

  FieldGimmick* gimmick = pGameManager->GetFieldmap()->GetFieldGimmick(Fieldmap::GIMMICK_IDX_ZONE);
  FieldGimmickShirenDenki* gimmick_shiren_denki = static_cast<FieldGimmickShirenDenki*>(gimmick);
  poke_3d::model::BaseModel* pModel = gimmick_shiren_denki->GetModel(model_id);
  GFL_ASSERT(pModel);

  return ( pModel->IsAnimationLastFrame() );
}

cell AMX_NATIVE_CALL FuncSetFieldInit::SystemEffectSetPos( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32   effect_type = ptr[1];
  f32   fX          = amx_ctof(ptr[2]);
  f32   fY          = amx_ctof(ptr[3]);
  f32   fZ          = amx_ctof(ptr[4]);
  bool  se_play     = GFL_BOOL_CAST(ptr[5]);
  // s32   iScale      = ptr[6];

  Field::Effect::Type effectType = Field::FieldScript::ScriptEffectManager::TranslateId( effect_type );
  gfl2::math::Vector3 pos = p_sys->CalcEventPositionWorld( fX, fY, fZ );
 
  s32 dataIndex;
  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  pEffectManager->CreateEffect(effectType, pos, se_play, Effect::EffectManager::WORK_TYPE_SYS, &dataIndex);
  
  return dataIndex;
}
cell AMX_NATIVE_CALL FuncSetFieldInit::SystemEffectSetChr( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();

  s32     effect_type = ptr[1];
  u32     EventID     = ptr[2];                   ///< chr_id
  bool    se_play = GFL_BOOL_CAST(ptr[3]);
  //f32 step_frame = amx_ctof( ptr[5] );
  //f32 scale = amx_ctof( ptr[6] );
  
  Field::Effect::Type effectType = Field::FieldScript::ScriptEffectManager::TranslateId( effect_type );

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  if( chrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return -1; 
  }

  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  s32 dataIndex;
  pEffectManager->CreateEffect(effectType, pFieldMoveModelManager->GetFieldMoveModel( chrID ), se_play, Effect::EffectManager::WORK_TYPE_SYS, &dataIndex);
  return dataIndex;
}
cell AMX_NATIVE_CALL FuncSetFieldInit::SystemEffectDel( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();

  s32     handle_id = ptr[1];

  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  s32 dataIndex;

  Effect::IEffectBase* pEffect = pEffectManager->GetEffect( handle_id );
  if (pEffect)
  {
    pEffectManager->DeleteEffect( pEffect );
  }
  return 0;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::S5_SystemEffectChange(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   handle_id = ptr[1];
  s32   kind = ptr[2];

  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  Field::Effect::IEffectBase *pEffect = pEffectManager->GetEffect(handle_id);
  if (pEffect)
  {
    if (kind >= EFF_S5_MDL_KUWA_BEAM1)
    {
      // モデルタイプ
      Field::Effect::EffectNewTrial5ForModel *pTrial = reinterpret_cast<Field::Effect::EffectNewTrial5ForModel*>(pEffect);
      pTrial->CreateEffect(kind);
    }
    else
    {
      // パーティクルタイプ
      Field::Effect::EffectNewTrial5 *pTrial = reinterpret_cast<Field::Effect::EffectNewTrial5*>(pEffect);
      pTrial->CreateEffect(kind);
    }
   
  }

  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::S5_SystemEffectFade(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   handle_id = ptr[1];

  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  Field::Effect::IEffectBase *pEffect = pEffectManager->GetEffect(handle_id);
  if (pEffect)
  {
    if (pEffect->GetType() == Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_02)
    {
      // モデルタイプ
      Field::Effect::EffectNewTrial5ForModel *pTrial = reinterpret_cast<Field::Effect::EffectNewTrial5ForModel*>(pEffect);
    }
    else if (pEffect->GetType() == Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_01)
    {
      // パーティクルタイプ
      Field::Effect::EffectNewTrial5 *pTrial = reinterpret_cast<Field::Effect::EffectNewTrial5*>(pEffect);
      pTrial->Fade();
    }
  }

  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::GimmickTriggerPosSetActiveFlag(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  GameSys::GameManager*   pGameManager = pFieldmap->GetGameManager();
  
  s32   event_id = ptr[1];
  b32   flag = ptr[2];

  FieldGimmick* gimmick = pGameManager->GetFieldmap()->GetFieldGimmick(Fieldmap::GIMMICK_IDX_ZONE);
  FieldGimmickTriggerPos* gimmick_trigger_pos = static_cast<FieldGimmickTriggerPos*>(gimmick);

  gimmick_trigger_pos->SetActiveFlag( event_id, flag );
  return NULL;
}

/**
*  @brief  ふれあいアイコンの生成
*/
cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiIconCreate(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();

  s32     effect_type = ptr[1];
  u32     EventID     = ptr[2];                   ///< chr_id
  bool    se_play = GFL_BOOL_CAST(ptr[3]);
  bool    voice_play = GFL_BOOL_CAST(ptr[4]);
  bool    is_keep = GFL_BOOL_CAST(ptr[5]);
  //f32 step_frame = amx_ctof( ptr[5] );
  //f32 scale = amx_ctof( ptr[6] );
  
  Field::Effect::Type effectType = Field::FieldScript::ScriptEffectManager::TranslateId( effect_type );

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  if( chrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return -1; 
  }

  s32 dataIndex;
  PokemonFureai::CreateEffect( effectType, pFieldMoveModelManager->GetFieldMoveModel( chrID ), se_play, voice_play, is_keep, &dataIndex );
  return dataIndex;

}

cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiIconDelete(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();

  u32     EventID = ptr[1];                   ///< chr_id

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);

  if (chrID == MoveModel::FIELD_MOVE_MODEL_MAX){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return -1;
  }

  return PokemonFureai::CloseEffect(pFieldMoveModelManager->GetFieldMoveModel(chrID));
}



/**
*  @brief  ふれあいアイコンの表示モード切替
*/
cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiIconModeChange(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   EventID = ptr[1];
  s32   mode = ptr[2];

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);

  if (chrID == MoveModel::FIELD_MOVE_MODEL_MAX){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return -1;
  }

  Field::MoveModel::FieldMoveModel* pFieldMoveModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  // 今でているアイコンをclose
  for (u32 i = 0; i < Field::Effect::EffectManager::MAX_EFFECT_NUM; ++i)
  {
    Field::Effect::IEffectBase *pEffect = pEffectManager->GetEffect(i);
    if (pEffect && pEffect->IsExec() && pEffect->IsSameParent(pFieldMoveModel))
    {
      switch (pEffect->GetType())
      {
      case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2:
      case Effect::EFFECT_TYPE_FUREAI_ICON_HEART1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_TEN1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1:
      {
        Field::Effect::EffectFureaiIconBase *pIconEffct = reinterpret_cast<Field::Effect::EffectFureaiIconBase*>(pEffect);
        pIconEffct->SetMode( mode );
        break;
      }
      }
    }
  }

  return NULL;
}

/**
*  @brief  ふれあいアイコンが存在するか
*/
cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiIconIsExist(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   EventID = ptr[1];
  s32   mode = ptr[2];

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);

  if (chrID == MoveModel::FIELD_MOVE_MODEL_MAX){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return -1;
  }

  Field::MoveModel::FieldMoveModel* pFieldMoveModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  // 今でているアイコンをclose
  for (u32 i = 0; i < Field::Effect::EffectManager::MAX_EFFECT_NUM; ++i)
  {
    Field::Effect::IEffectBase *pEffect = pEffectManager->GetEffect(i);
    if (pEffect && pEffect->IsExec() && pEffect->IsSameParent(pFieldMoveModel))
    {
      switch (pEffect->GetType())
      {
      case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2:
      case Effect::EFFECT_TYPE_FUREAI_ICON_HEART1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_TEN1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1:
      {
        return true;
      }
      }
    }
  }

  return false;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiIconEnableSound(AMX * amx, const cell * ptr)
{
	ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
	GameSys::GameManager    *p_gman = p_obj->GetGameManager();
	FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
	s32   EventID = ptr[1];
	s32   flag = ptr[2];

	Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
	Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
	u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);

	if (chrID == MoveModel::FIELD_MOVE_MODEL_MAX){
		GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
		return -1;
	}

	Field::MoveModel::FieldMoveModel* pFieldMoveModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

	// 今でているアイコンをclose
	for (u32 i = 0; i < Field::Effect::EffectManager::MAX_EFFECT_NUM; ++i)
	{
		Field::Effect::IEffectBase *pEffect = pEffectManager->GetEffect(i);
		if (pEffect && pEffect->IsExec() && pEffect->IsSameParent(pFieldMoveModel))
		{
			switch (pEffect->GetType())
			{
			case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2:
			case Effect::EFFECT_TYPE_FUREAI_ICON_HEART1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_TEN1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1:
			{
        Field::Effect::EffectFureaiIconBase *pIconEffct = reinterpret_cast<Field::Effect::EffectFureaiIconBase*>(pEffect);
				pIconEffct->SetPlaySe( flag );
				return true;
			}
			}
		}
	}

	return false;
}


/**
*  @brief  ふれあいアイコンの現在の状態を取得
*/
cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiIconGetState(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   handle_id = ptr[1];

  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  Field::Effect::IEffectBase *pEffect = pEffectManager->GetEffect(handle_id);
  if (pEffect)
  {
    Field::Effect::EffectFureaiIconBase *pIconEffct = reinterpret_cast<Field::Effect::EffectFureaiIconBase*>(pEffect);
    return pIconEffct->GetStateForScript();
  }

  return NULL;
}
/**
*  @brief  ふれあいのポケモン設定を取得
*/
cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiGetPokemonSettingData(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   event_id = ptr[1];
  s32   data_type = ptr[2];
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( event_id );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  s32 model_id = pCharaModel->GetHeaderResource().characterId;
  Field::FieldResident* pFieldResident = pSys->GetGameManager()->GetFieldResident();
  Field::PokemonFureai::DataAccessor dataAccessor( pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__POKEMON_FUREAI_BIN ) );

  const Field::PokemonFureai::POKEMON_SETTING_DATA* pData = dataAccessor.GetSettingData( model_id );


  // 調整用機能
#if PM_DEBUG
	Field::MoveModel::FieldMoveCodeFureaiTypeA::DEBUG_SETTING* pDebugSetting = Field::MoveModel::FieldMoveCodeFureaiTypeA::GetDebugSettingData();
	if (pDebugSetting->isNadeNadeTest)
	{
		switch (data_type)
		{
		case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_NADE_DISTANCE:
		{
			return amx_ftoc(pDebugSetting->nadenade_distance);
		}
		case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_NADE_X:
		{
			return amx_ftoc(pDebugSetting->nadenade_distance_x);
		}
		case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_NADE_ANGLE:
		{
			return amx_ftoc(pDebugSetting->nadenade_angle);
		}
		} // switch
	}

	if (pDebugSetting->isBaseStanceTest)
	{
		switch (data_type)
		{
		case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_BASE_STANCE:
		{
		  return amx_ftoc(pDebugSetting->base_stance);
		}
		}
	}
#endif


  switch (data_type)
  {
  case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_BASE_STANCE:
  {
    return pData->base_stance;
  }
  case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_NADE_DISTANCE:
  {
    return amx_ftoc( pData->nadenade_distance );
  }
  case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_NADE_X:
  {
    return amx_ftoc( pData->nadenade_x );
  }
  case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_NADE_ANGLE:
  {
    return amx_ftoc( pData->nadenade_angle );
  }
  case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_SPEED:
  {
    return amx_ftoc( pData->speed );
  }
  case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_YOROKOBI1_TYPE:
  {
    return pData->yorokobi1_type;
  }
  case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_YOROKOBI2_TYPE:
  {
    return pData->yorokobi2_type;
  }
  case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_NEMURI_TYPE:
  {
    return pData->nemuri_type;
  }
  case Field::PokemonFureai::FPF_POKEMON_SETTING_DATA_KAITEN_ASOBI_TYPE:
  {
    return pData->kaiten_asobi_type;
  }
  }

  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiChrApplyToDebugParam(AMX * amx, const cell * ptr)
{
#if PM_DEBUG
	ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
	GameSys::GameManager    *p_gman = p_obj->GetGameManager();
	FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
	s32   event_id = ptr[1];
	s32   data_type = ptr[2];

	Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
	u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(event_id);
	Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

	if (pCharaModel == NULL){
		GFL_ASSERT_MSG(0, "%d\n", event_id);
		return false;
	}

	s32 model_id = pCharaModel->GetHeaderResource().characterId;
	Field::FieldResident* pFieldResident = pSys->GetGameManager()->GetFieldResident();
	Field::PokemonFureai::DataAccessor dataAccessor(pFieldResident->GetResource(BL_IDX_FIELD_RESIDENT__POKEMON_FUREAI_BIN));

	const Field::PokemonFureai::POKEMON_SETTING_DATA* pData = dataAccessor.GetSettingData(model_id);

	Field::MoveModel::FieldMoveCodeFureaiTypeA::DEBUG_SETTING* pDebugSetting = Field::MoveModel::FieldMoveCodeFureaiTypeA::GetDebugSettingData();
	pDebugSetting->nadenade_distance = pData->nadenade_distance;
	pDebugSetting->nadenade_distance_x = pData->nadenade_x;
	pDebugSetting->nadenade_angle = pData->nadenade_angle;
#endif
	return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiChrGetPokemonData(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   event_id = ptr[1];
  s32   data_type = ptr[2];

  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32 chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( event_id );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "%d\n", event_id );
    return false;
  }

  // 個別データ
  Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pCharaModel->GetActorUserWork());
  const FieldContactPokemonAccessor::ContactPokemonData* pContactPokemonData = pFieldMoveModelActor->GetContactPokemonData();
  
  // 種族データ
  s32 model_id = pCharaModel->GetHeaderResource().characterId;
  Field::FieldResident* pFieldResident = pSys->GetGameManager()->GetFieldResident();
  Field::PokemonFureai::DataAccessor dataAccessor( pFieldResident->GetResource( BL_IDX_FIELD_RESIDENT__POKEMON_FUREAI_BIN ) );
  const Field::PokemonFureai::POKEMON_SETTING_DATA* pData = dataAccessor.GetSettingData( model_id );
  
  // 共有ワークを取得
  MoveModel::FieldMoveCodeFureaiTypeA::Work* pWork = (MoveModel::FieldMoveCodeFureaiTypeA::Work*)Field::MoveModel::FieldMoveCodeManager::GetMoveCodeWork(pCharaModel->GetMoveCodeWork(), sizeof(MoveModel::FieldMoveCodeFureaiTypeA::Work));

  switch (data_type)
  {
  case PokemonFureai::FPF_POKEMON_DATA_POINT:
  {
    return PokemonFureai::GetPoint( pContactPokemonData );
  }

  case PokemonFureai::FPF_POKEMON_DATA_RANK:
  {
    return PokemonFureai::GetRank( pCharaModel );
  }
  case PokemonFureai::FPF_POKEMON_DATA_IS_ADD_POINT:
  {
    return PokemonFureai::IsAddPoint( pContactPokemonData );
  }

  case PokemonFureai::FPF_POKEMON_DATA_MOVE_MODE:
  {
    return pData->move_mode;
  }
  case PokemonFureai::FPF_POKEMON_DATA_ROT_TYPE:
  {
    return pData->rot_type;
  }
  case PokemonFureai::FPF_POKEMON_DATA_IS_FIRST:          // 初回
  {
    return (!pWork->isComEventEndFlag);
  }
  case PokemonFureai::FPF_POKEMON_DATA_IS_LOSTED_FIRST:   // みうしなってから初回
  {
    return (!pWork->isComEventEndFlagDiscoverAfter);
  }

  case PokemonFureai::FPF_POKEMON_DATA_PARENT_EVENT_ID:
  {
    return pContactPokemonData->npcEventID;
  }
  
  case PokemonFureai::FPF_POKEMON_DATA_IS_NICKNAME_KNOW:
  {
    return PokemonFureai::IsNickNameKnow( pContactPokemonData );
  }

  case PokemonFureai::FPF_POKEMON_DATA_FUREAI_NO:
  {
    return pContactPokemonData->fureaiNo;
  }
  case PokemonFureai::FPF_POKEMON_DATA_IS_SLEEP:
  {
    return MoveModel::FieldMoveCodeFureaiTypeA::IsSleep( pCharaModel->GetMoveCodeWork() );
  }
  case PokemonFureai::FPF_POKEMON_DATA_FIELD_ATTR:
  {
    return pCharaModel->GetGroundAttribute();
  }
  case PokemonFureai::FPF_POKEMON_DATA_IS_NADENADE_REQ_EVENT_END:
  {
    return PokemonFureai::IsNadeNadeRequestEventEnd(pContactPokemonData->fureaiNo);
  }

	case PokemonFureai::FPF_POKEMON_DATA_DEFAULT_POS_X:         // 初期位置
	{
		gfl2::math::Vector3 pos;
		pFieldMoveModelActor->GetFureaiBasePosition(&pos );
		return amx_ftoc( pos.x );
	}
	case PokemonFureai::FPF_POKEMON_DATA_DEFAULT_POS_Z:         // 初期位置
	{
		gfl2::math::Vector3 pos;
		pFieldMoveModelActor->GetFureaiBasePosition(&pos );
		return amx_ftoc( pos.z );
	}
	case PokemonFureai::FPF_POKEMON_DATA_DEFAULT_ANGLE:         // 初期向き
	{
		gfl2::math::Quaternion  qua;
		pFieldMoveModelActor->GetFureaiBaseQuaternion(&qua);
		f32 defDeg = gfl2::math::ConvRadToDeg(FuncSetFieldInit::ConvRotationQuaternionToYRadian(qua));
		return amx_ftoc(defDeg);
	}

	case PokemonFureai::FPF_POKEMON_DATA_DEFAULT_DISTANCE:
	{
		f32 work = MoveModel::FieldMoveCodeFureaiTypeA::GetBaseDistanceForModorou(pCharaModel->GetMoveCodeWork());
		return amx_ftoc( work );
	}
  }

  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiChrPointAdd(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   event_id = ptr[1];
  s32   value = ptr[2];

  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32 chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(event_id);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "%d\n", event_id);
    return false;
  }

  PokemonFureai::AddPoint( pCharaModel, value );
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiSaveSetNickNameKnow(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   fureai_no = ptr[1];
  s32   flag = ptr[2];

  PokemonFureai::SetNickNameKnow( fureai_no, flag );
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiSaveSetNadeNadeReqEventEnd(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   fureai_no = ptr[1];
  s32   flag = ptr[2];

  PokemonFureai::SetNadeNadeRequestEventEnd( fureai_no, flag );
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiSaveSetPoint(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   fureai_no = ptr[1];
  s32   point = ptr[2];

  PokemonFureai::SetPoint( fureai_no, point );
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiChrPlayVoice(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   event_id = ptr[1];
  s32   voice_type = ptr[2];

  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32 chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(event_id);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "%d\n", event_id);
    return false;
  }

  PokemonFureai::PlayVoice( pCharaModel, voice_type );
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetFieldInit::FureaiChrCanNadeNadeDistance(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   event_id = ptr[1];
  s32   voice_type = ptr[2];

  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32 chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(event_id);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "%d\n", event_id);
    return false;
  }

  return MoveModel::FieldMoveCodeFureaiTypeA::CanNadeNadeDistance( pCharaModel->GetMoveCodeWork() );
}

cell AMX_NATIVE_CALL FuncSetFieldInit::ChrGetChrID(AMX * amx, const cell * ptr)
{
	ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
	GameSys::GameManager    *p_gman = p_obj->GetGameManager();
	FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
	s32   event_id = ptr[1];
	s32   voice_type = ptr[2];

	Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
	u32 chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(event_id);
	Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

	if (pCharaModel == NULL){
		GFL_ASSERT_MSG(0, "%d\n", event_id);
		return 0;
	}

	return pCharaModel->GetCharacterId();
}


//-----------------------------------------------------------------------------
/**
*  @brief  影領域を指定動作モデルへ適用させる
*
*  @param  event_id 動作モデルのイベントID
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ShadeAreaApply(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();

  s32   event_id  = ptr[1];

  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32 chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(event_id);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "%d\n", event_id);
    return NULL;
  }

  Field::Shade::FieldPlayerShade* const pPlayerShade = p_fieldmap->GetFieldPlayerShade();
  if (pPlayerShade == NULL){
    GFL_ASSERT_MSG(0, "FieldPlayerShade is NULL [%d]\n", event_id);
    return NULL;
  }

  pPlayerShade->ApplyShadeAreaForMoveModel(pCharaModel);

  return NULL;
}



//------------------------------------------------------------------------------
/**
 * @func        RegistSkyboxPosFix
 * @brief       天球の位置を固定
 * @author      saita_kazuki
 * @date        2016/06/01
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::RegistSkyboxPosFix(AMX * amx, const cell * ptr )
{
  ScriptObject*           p_obj                 = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*   p_game_manager        = p_obj->GetGameManager();
  Fieldmap*               p_fieldmap            = p_game_manager->GetFieldmap();
  System::Skybox::Skybox* p_skybox              = p_game_manager->GetSkybox();
  Skybox::DataManager*    p_skybox_data_manager = p_fieldmap->GetSkyboxDataManager();

  gfl2::math::Vector3 fixPos = p_skybox->GetPartsModel( System::Skybox::Skybox::PARTS_TYPE_SKY )->GetPosition();
  p_skybox_data_manager->RegistFixPosition( fixPos );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        UnRegistSkyboxPosFix
 * @brief       天球の位置固定を解除
 * @author      saita_kazuki
 * @date        2016/06/01
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::UnRegistSkyboxPosFix(AMX * amx, const cell * ptr )
{
  ScriptObject*           p_obj                 = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*   p_game_manager        = p_obj->GetGameManager();
  Fieldmap*               p_fieldmap            = p_game_manager->GetFieldmap();
  Skybox::DataManager*    p_skybox_data_manager = p_fieldmap->GetSkyboxDataManager();

  p_skybox_data_manager->UnRegistFixPosition();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrSetPosForInit
 * @brief       キャラクターの座標を指定します
 * @author      N.Saito
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPosForInit( AMX* amx, const cell* ptr )
{
#if PM_DEBUG
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GFL_ASSERT_MSG(pSys->IsInitScriptTiming(), "can not use [ChrSetPosForInit]");
#endif //PM_DEBUG
  return ChrSetPos_core( amx, ptr, true );
}

//------------------------------------------------------------------------------
/**
 * @func        ChrSetPosAngForInit
 * @brief       キャラクターの座標を指定します
 * @author      N.Saito
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPosAngForInit( AMX* amx, const cell* ptr )
{
#if PM_DEBUG
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GFL_ASSERT_MSG(pSys->IsInitScriptTiming(), "can not use [ChrSetPosAngForInit]");
#endif //PM_DEBUG
  return ChrSetPosAng_core( amx, ptr, true );
}

//------------------------------------------------------------------------------
/**
 * @func        ChrSetPosVec3ForInit
 * @brief       キャラクターの位置を３軸指定します
 * @author      N.Saito
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPosVec3ForInit( AMX* amx, const cell* ptr )
{
#if PM_DEBUG
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GFL_ASSERT_MSG(pSys->IsInitScriptTiming(), "can not use [ChrSetPosVec3ForInit]");
#endif //PM_DEBUG
  return ChrSetPosVec3_core( amx, ptr, true );
}

//------------------------------------------------------------------------------
/**
 * @func        ChrSetPosAngVec3ForInit
 * @brief       キャラクターの座標と角度を３軸指定します
 * @author      N.Saito
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetFieldInit::ChrSetPosAngVec3ForInit( AMX* amx, const cell* ptr )
{
#if PM_DEBUG
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GFL_ASSERT_MSG(pSys->IsInitScriptTiming(),"can not use [ChrSetPosAngVec3ForInit]");
#endif //PM_DEBUG
  return ChrSetPosAngVec3_core( amx, ptr, true );
}

/**
*  @brief  ルナソルダイブモンスターナンバー取得
*/
cell AMX_NATIVE_CALL FuncSetFieldInit::GetLunaSolDiveMonsno(AMX * amx, const cell * ptr)
{
  FieldScriptSystem *pScriptSystem = FieldScriptSystem::GetInstance();
  u32 monsno = pScriptSystem->GetGameManager()->GetGameData()->GetLunaSolDiveMonsno();
  return monsno;
}


///WaitFunctions
bool FuncSetFieldInit::WaitFunc_PlayerSetPosForEnd(FieldScriptSystem* p_sys, u32 label, void* p_work)
{
  GameSys::GameManager  *p_gman = p_sys->GetGameManager();
  Field::TrafficManager* pTrafficManager = p_gman->GetFieldmap()->GetTrafficManager();

  if (pTrafficManager->CheckEscape() == false){
    return false; //まだ退避が済んでいない
  }
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModel* pPlayer = pFieldMoveModelManager->GetFieldMoveModel(Field::MoveModel::FIELD_MOVE_MODEL_PLAYER);
  gfl2::math::Vector3* pos = reinterpret_cast<gfl2::math::Vector3*>(p_work);

  //退避がすんだのでプレイヤーをワープさせる
  if (pPlayer != NULL){
    pPlayer->GetCharaDrawInstance()->SetPosition(*pos);

    //プレイヤーのフリー移動解除記録
    pPlayer->EnablePointMoveForEventScript();
  }

  GFL_SAFE_DELETE(pos);

  return true;
}

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

