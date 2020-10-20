//=============================================================================
/**
 *
 *  @file   ScriptFuncSetFieldInit.h
 *  @brief  スクリプト命令セット：初期化スクリプト用命令セット
 *  @author Miyuki Iwasawa
 *  @data   2015.04.29
 *
 */
//=============================================================================
#ifndef __SCRIPTFUNCSETFIELDINIT_H__
#define __SCRIPTFUNCSETFIELDINIT_H__

#include <pawn\include\gfl2_Pawn.h>

namespace GameSys{
  class GameData;
}
namespace pml{
  namespace pokepara{
    class PokemonParam;
  }
}

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( FieldScript );

//==============================================================
///   初期化時も通常時も使用できるスクリプトコマンド
//==============================================================
class FuncSetFieldInit
{
  GFL_FORBID_COPY_AND_ASSIGN( FuncSetFieldInit );
public:
  static const GFL_PAWN_FUNC_TABLE* GetTable( void );

private:
  //直接スクリプトコマンドとしては使用しない
  static cell AMX_NATIVE_CALL ChrSetPos_core(AMX*, const cell*, b32 isInit = false );
  static cell AMX_NATIVE_CALL ChrSetPosAng_core(AMX*, const cell*, b32 isInit = false);
  static cell AMX_NATIVE_CALL ChrSetPosVec3_core(AMX*, const cell*, b32 isInit = false);
  static cell AMX_NATIVE_CALL ChrSetPosAngVec3_core(AMX*, const cell*, b32 isInit = false);

private:

  static cell AMX_NATIVE_CALL ChrAdd( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrDel( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrIsExist( AMX*, const cell* );
  
  static cell AMX_NATIVE_CALL ChrSetPos_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrSetPosAng_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrSetPosVec3_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrSetAng( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrSetAngVec3( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrSetPosAngVec3_( AMX*, const cell* );
  
  static cell AMX_NATIVE_CALL ChrGetPosX_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrGetPosY_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrGetPosZ_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrGetAng_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrGetTalkReturnAng_( AMX *, const cell * );

  static cell AMX_NATIVE_CALL ChrCollision( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrFloat( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrGetFloat( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrDisp( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrShadowDisp( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrPartsDisp( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrSetDefaultIdle( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrGetDefaultIdle( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrCheckSittingTalk( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrDisableDispCntCulling( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrDisableAppCulling( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrSetMotionMoveDisableFlag( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrSetStateBitShadowOff( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrSetStateBitInvalidResistCheck( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrSetStateBitCullingAabbMinimun( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrSetStateBitCullingLengthOptDisable( AMX *, const cell * );

  static cell AMX_NATIVE_CALL ChrMotionEndWatchStart( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrMotionEndWatchClear( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrMotionGetEndFrame( AMX *, const cell * );

  static cell AMX_NATIVE_CALL ChrIsNpcMoveCode( AMX*, const cell* );
  static cell AMX_NATIVE_CALL NpcOnOption( AMX*, const cell* );
  static cell AMX_NATIVE_CALL NpcOffOption( AMX*, const cell* );
  static cell AMX_NATIVE_CALL NpcResetOption( AMX*, const cell* );
  static cell AMX_NATIVE_CALL NpcClearTalkEnd( AMX*, const cell* );
  static cell AMX_NATIVE_CALL NpcClearDirtyFlag( AMX*, const cell* );
  static cell AMX_NATIVE_CALL NpcOverWriteReturnAngle_( AMX*, const cell* );
  
  static cell AMX_NATIVE_CALL PlayerFreeMoveStateReset(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrFreeMoveStateReset(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ModelSearchByCharacterID(AMX *, const cell * );

  static cell AMX_NATIVE_CALL ObjAdd( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ObjDel( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ObjAddAll( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ObjDelAll( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ObjGetPosX( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ObjGetPosY( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ObjGetPosZ( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ZigarudeCellStatusSet( AMX *, const cell * );
  static cell AMX_NATIVE_CALL PokeFinderAddDel( AMX *, const cell * );

  static cell AMX_NATIVE_CALL EnvSoundStatusSet( AMX *, const cell * );
  static cell AMX_NATIVE_CALL EnvSoundVolumeChange( AMX *, const cell * );
  static cell AMX_NATIVE_CALL FootSoundVolumeChange( AMX *, const cell * );
  static cell AMX_NATIVE_CALL WeatherSoundVolumeChange( AMX *, const cell * );

  static cell AMX_NATIVE_CALL ChangeWeather(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ResetWeather(AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetNowWeather(AMX *, const cell * );
  static cell AMX_NATIVE_CALL RotomPosEventSet( AMX * amx, const cell * ptr );

  static cell AMX_NATIVE_CALL StaticModelMotionPlay(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionIsEnd(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionPlaySmooth(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionIsEndSmooth(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionPlayInv(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionSetEnd(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionReset(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionStop(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionPause(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionRestart(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionAllStop(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionAllRestart(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelLoopMotionStop(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelLoopMotionPlay(AMX *, const cell * );

  static cell AMX_NATIVE_CALL StaticModelMotionByStaticIdPlay(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionByStaticIdIsEnd(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionByStaticIdStop(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionByStaticIdSetFrame(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelSetPosAng(AMX *, const cell * );
  static cell AMX_NATIVE_CALL S5_GimmickModelMotionPlay(AMX *, const cell * );
  static cell AMX_NATIVE_CALL S5_GimmickModelMotionSetEnd(AMX *, const cell * );
  static cell AMX_NATIVE_CALL S5_GimmickModelMotionIsEnd(AMX *, const cell * );
  
  static cell AMX_NATIVE_CALL SystemEffectSetPos(AMX *, const cell *);
  static cell AMX_NATIVE_CALL SystemEffectSetChr(AMX *, const cell *);
  static cell AMX_NATIVE_CALL SystemEffectDel(AMX *, const cell *);
  static cell AMX_NATIVE_CALL S5_SystemEffectChange(AMX *, const cell *);
  static cell AMX_NATIVE_CALL S5_SystemEffectFade(AMX *, const cell *);
  static cell AMX_NATIVE_CALL GimmickTriggerPosSetActiveFlag(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiIconCreate(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiIconDelete(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiIconModeChange(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiIconIsExist(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiIconGetState(AMX *, const cell *);
	static cell AMX_NATIVE_CALL FureaiIconEnableSound(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiGetPokemonSettingData(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiChrGetPokemonData(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiChrPointAdd(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiSaveSetNickNameKnow(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiSaveSetNadeNadeReqEventEnd(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiSaveSetPoint(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiChrPlayVoice(AMX *, const cell *);
  static cell AMX_NATIVE_CALL FureaiChrCanNadeNadeDistance(AMX *, const cell *);
	static cell AMX_NATIVE_CALL FureaiChrApplyToDebugParam(AMX *, const cell *);

	static cell AMX_NATIVE_CALL ChrGetChrID(AMX *, const cell *);

  static cell AMX_NATIVE_CALL ShadeAreaApply(AMX *, const cell *);

  static cell AMX_NATIVE_CALL RegistSkyboxPosFix(AMX *, const cell * );
  static cell AMX_NATIVE_CALL UnRegistSkyboxPosFix(AMX *, const cell * );

  static cell AMX_NATIVE_CALL ChrSetPosForInit( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrSetPosAngForInit( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrSetPosVec3ForInit( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrSetPosAngVec3ForInit( AMX*, const cell* );

  static cell AMX_NATIVE_CALL GetLunaSolDiveMonsno(AMX *, const cell *); //!< ルナソルダイブモンスターナンバー取得


public:
  /*
   *  Y軸回転の成分のみを抽出する汎用関数
   */
  static f32 ConvRotationVector3ToYRadian( gfl2::math::Vector3& src );
  static f32 ConvRotationQuaternionToYRadian( gfl2::math::Quaternion& src );

  /*
   *  イベント配置物の座標を取得
   **/
  static b32 GetActorPos(GameSys::GameManager* pGameManager, s32 eventId, b32 isCalcEventBasePos, gfl2::math::Vector3* pPos);


  ///WaitFunctions
  static bool WaitFunc_PlayerSetPosForEnd(FieldScriptSystem* p_sys, u32 label, void* p_work);

};

GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );

#endif // __SCRIPTFUNCSETFIELDINIT_H__

