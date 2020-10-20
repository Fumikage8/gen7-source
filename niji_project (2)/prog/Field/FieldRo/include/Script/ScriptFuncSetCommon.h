//==============================================================================
/**
 * @file        ScriptFuncSetCommon.h
 * @brief       要約説明
    
        このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/04/13(月) 18:52:41
 */
//==============================================================================

#if !defined(__SCRIPTFUNCSETCOMMON_H__) // 重複定義防止
#define __SCRIPTFUNCSETCOMMON_H__   // 重複定義防止マクロ
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "pawn/include/gfl2_Pawn.h"

/// デバッグではリミッター解除が出来るように.
#define DEBUG_NECKLIMIT PM_DEBUG

//前方宣言
namespace GameSys{
  class GameManager;
}
namespace Field{
  namespace Camera{
    class CameraManager;
  }
  namespace FieldScript{
    class FieldScriptSystem;
  }
}

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )

//------------------------------------------------------------------------------
/**
 * @class       FuncSetCommon
 * @brief       
 * @author      N.Takeda
 * @date        2015/04/13(月) 20:58:43
 */
//------------------------------------------------------------------------------
class FuncSetCommon {
  GFL_FORBID_COPY_AND_ASSIGN(FuncSetCommon);
public:
  /// 命令テーブルセットを取得.
  static const GFL_PAWN_FUNC_TABLE* GetTable(void);
  
  static void SetupScriptFuncEndCheck( GameSys::GameManager* p_gman );
  static bool CheckScriptFuncEnd( GameSys::GameManager* p_gman );
  static void ScriptFuncTerminateFieldTop( GameSys::GameManager* p_gman );
  static void ScriptFuncTerminateFieldBottom( GameSys::GameManager* p_gman );
  
  enum{ WORK_SIZE = 16  };
  static void  ClearWork(){  for( s32 ii=0 ; ii<WORK_SIZE ; ii++ ) m_iWork[ ii ] = 0;  }
  
private:
  static s32  m_iWork[ WORK_SIZE ];
  static f32  m_fAngle;
#if DEBUG_NECKLIMIT
  static f32  m_fLimitAngH;
  static f32  m_fLimitAngL;
  static f32  m_fLimitAngH2;
  static f32  m_fLimitAngL2;
#endif

  typedef bool (*SCR_END_CHECK_FUNC)( GameSys::GameManager* p_gman, s32* seq );
  static bool SCREND_CheckEndWin( GameSys::GameManager* p_gman, s32* seq );
  static bool SCREND_CheckEndEventCamera( GameSys::GameManager* p_gman, s32* seq );
  static bool SCREND_CheckEndBgParts( GameSys::GameManager* p_gman, s32* seq );
  static bool SCREND_CheckEndMapBlockCalcPos( GameSys::GameManager* p_gman, s32* seq );
  static bool SCREND_CheckEndMotionBlur( GameSys::GameManager* p_gman, s32* seq );
  static bool SCREND_CheckEndMapBlockHideMeshSystemOn( GameSys::GameManager* p_gman, s32* seq );

  
  // スクリプトに渡す関数
  static cell AMX_NATIVE_CALL GlobalCall(AMX *, const cell *);
  static cell AMX_NATIVE_CALL GlobalCallDebugOnetime(AMX *, const cell *);
  static cell AMX_NATIVE_CALL GlobalGetChildParam(AMX *, const cell *);
  static cell AMX_NATIVE_CALL GlobalSetChildParam(AMX *, const cell *);
  static cell AMX_NATIVE_CALL GlobalGetParentParam(AMX *, const cell *);
  static cell AMX_NATIVE_CALL GlobalSetParentParam(AMX *, const cell *);

  static cell AMX_NATIVE_CALL ParallelCall(AMX *, const cell *);
  static cell AMX_NATIVE_CALL ParallelCallIsFinished(AMX *, const cell *);
  static cell AMX_NATIVE_CALL ParallelCallIsFinishedAll(AMX *, const cell *);

  static cell AMX_NATIVE_CALL TrainerIDGet(AMX *, const cell *);

  static cell AMX_NATIVE_CALL MapChangeCore_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL MapChangeEscape_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL JoinFestaOpen(AMX *, const cell *);
  static cell AMX_NATIVE_CALL JoinFestaClose(AMX *, const cell *);

  static cell AMX_NATIVE_CALL PlaceNameRequest(AMX *, const cell *);

  static cell AMX_NATIVE_CALL ABKeyWait_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL LastKeyWait_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL SlidePadWait_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL InputKeyIsHold(AMX *, const cell *);
  static cell AMX_NATIVE_CALL InputKeyIsTrigger(AMX *, const cell *);
  static cell AMX_NATIVE_CALL InputKeyIsRelease(AMX *, const cell *);

  static cell AMX_NATIVE_CALL ExtraMsgInstant_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL ExtraMsgLoad_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL ExtraMsgIsLoaded_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL ExtraMsgUnload(AMX *, const cell *);
  static cell AMX_NATIVE_CALL ExtraMsgSwap_(AMX *, const cell *);

  static cell AMX_NATIVE_CALL YesNoWin_Seq(AMX *, const cell *);
  static cell AMX_NATIVE_CALL TalkMdlStartInit_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL TalkMdlEndInit_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL TalkMdlGetRepairAngle_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL TalkMsg_Seq(AMX *, const cell *);
  static cell AMX_NATIVE_CALL TalkMsg_SeqWait(AMX *, const cell *);
  static cell AMX_NATIVE_CALL TalkMsg_VisubleCursor(AMX *, const cell *);
  static cell AMX_NATIVE_CALL MsgWinCloseNo(AMX *, const cell *);
  static cell AMX_NATIVE_CALL ListMenuInit_Seq(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ListMenuAdd(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ListMenuStart_Seq(AMX *, const cell * );
  static cell AMX_NATIVE_CALL DebugListMenuAdd_(AMX *, const cell * );

  static cell AMX_NATIVE_CALL PG_WordSetRegister( AMX *, const cell * );
  static cell AMX_NATIVE_CALL WordSetNumber(AMX *, const cell * );
  static cell AMX_NATIVE_CALL WordSetMoney(AMX *, const cell * );
  
  static cell AMX_NATIVE_CALL SpMotionPacLoad( AMX*, const cell* );
  static cell AMX_NATIVE_CALL SpMotionPacDel( AMX*, const cell* );
  static cell AMX_NATIVE_CALL SpMotionPacLoad_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL SpMotionPacDel_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL SpMotionPacLoadForKisekae( AMX*, const cell* );
  static cell AMX_NATIVE_CALL SpMotionPacDelForKisekae( AMX*, const cell* );

  static cell AMX_NATIVE_CALL SpChrLoad( AMX*, const cell* );
  static cell AMX_NATIVE_CALL SpChrCreate( AMX*, const cell* );
  static cell AMX_NATIVE_CALL SpChrDel( AMX*, const cell* );
  static cell AMX_NATIVE_CALL SpChrUnLoad( AMX*, const cell* );


  static cell AMX_NATIVE_CALL ChrMove_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrMagnetActor_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrActionCommandIsPlaying_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrMotionGetPlayNo( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrMotionPlayFrame_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrMotionPlayFrameDirCorrect_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrMotionIsPlaying_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrMotionPause( AMX*, const cell* );
  
  static cell AMX_NATIVE_CALL ChrGetDataIndexMax_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrGetEventIdByDataIndex_( AMX*, const cell* );

  static cell AMX_NATIVE_CALL ChrRot_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrRotFrame( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrRotVec3_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrRotTarget_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrStepJump_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrRotReq( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrRotTargetReq( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrRotReqClear( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrRotPos( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrRotFieldActor( AMX*, const cell* );
  
  static cell AMX_NATIVE_CALL ChrFastRot_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrFastRotVec3_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrFastRotTarget_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrFastRotReq( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrFastRotTargetReq( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrFastRotPos( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrFastRotFieldActor( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChrFastRotFrame( AMX*, const cell* );

  static cell AMX_NATIVE_CALL EvCameraStart( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraEnd( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraMoveWait_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraRecordStart_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraRecordEnd_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraMovePos( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraMoveDist( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraMoveOffset( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraMoveLookAt( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraMoveLookAtChr( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraMoveLookAtChrEasy( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraMoveLookAtChrEasyPair( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraMoveLookAtChrEasyPairDistance( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraShake( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraShakeLoop( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraShakeEnd( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraAnimationLoad( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraAnimationUnLoad( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraAnimationIsLoading( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraAnimationPlay( AMX*, const cell* );
  static cell AMX_NATIVE_CALL EvCameraAnimationIsPlaying( AMX*, const cell* );
  static cell AMX_NATIVE_CALL FieldCameraShake( AMX*, const cell* );
  static cell AMX_NATIVE_CALL FieldCameraReset( AMX*, const cell* );
  static cell AMX_NATIVE_CALL GlobalCameraShake(AMX*, const cell*);
  
  static cell AMX_NATIVE_CALL SEPlay( AMX*, const cell* );
  static cell AMX_NATIVE_CALL SEChangeVolume( AMX*, const cell* );
  static cell AMX_NATIVE_CALL SEIsPlaying( AMX*, const cell* );
  static cell AMX_NATIVE_CALL SEStop( AMX*, const cell* );
  static cell AMX_NATIVE_CALL MEPlay( AMX*, const cell* );
  static cell AMX_NATIVE_CALL MEIsPlaying( AMX*, const cell* );
  static cell AMX_NATIVE_CALL MEReturnBGM( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BGMIsPlaying( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BGMChangeNowMap( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BGMStartEvent( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BGMChangeEvent( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BGMEndEvent( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BGMStartBattleEvent( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BGMChangeBattleEvent( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BGMVolumeChange( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BGMVolumeRecover( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BGMPause( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BGMPauseRelease( AMX*, const cell* );
  static cell AMX_NATIVE_CALL VoicePlay_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL VoicePlay( AMX*, const cell* );
  static cell AMX_NATIVE_CALL VoiceIsPlaying( AMX*, const cell* );
  static cell AMX_NATIVE_CALL GetFieldBGM( AMX*, const cell* );

  static cell AMX_NATIVE_CALL CallWildBattleCore( AMX*, const cell* );
  static cell AMX_NATIVE_CALL ChangeEventWildLose_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL GetWildBattleResult(AMX *, const cell *);
  static cell AMX_NATIVE_CALL CheckAmaimitsuEncountDataExist( AMX*, const cell* );
  static cell AMX_NATIVE_CALL CallAmaimitsuBattle( AMX*, const cell* );
  static cell AMX_NATIVE_CALL GetWildBattleEffectID( AMX *, const cell * );

  static cell AMX_NATIVE_CALL CallTrainerBattleCore( AMX*, const cell* );
  static cell AMX_NATIVE_CALL TrainerGetInfo(AMX *, const cell *);
  static cell AMX_NATIVE_CALL TrainerGetMsgID(AMX *, const cell *);
  static cell AMX_NATIVE_CALL TrainerGetScrID(AMX *, const cell *);
  static cell AMX_NATIVE_CALL TrainerGetEventID(AMX *, const cell *);
  static cell AMX_NATIVE_CALL ChangeEventTrainerLose_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL BattleGetResult_(AMX *, const cell *);

  static cell AMX_NATIVE_CALL EffectSetPos( AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectSetPosScaleRot(AMX *, const cell *);
  static cell AMX_NATIVE_CALL EffectSetChr( AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectSetTrainerEye( AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectWait_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectDel( AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectNextRotom( AMX*, const cell * );
  static cell AMX_NATIVE_CALL EffectAllDel(AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectUnloadResource(AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectFestivalFireStart(AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectFestivalFireEnd(AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectFestivalFireIsAlive(AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectGetItemStart(AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectTrial3Start(AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectTrial5Start(AMX *, const cell * );

  static cell AMX_NATIVE_CALL S5_EffectChange(AMX *, const cell *);
  static cell AMX_NATIVE_CALL S5_EffectFade(AMX *, const cell *);

  static cell AMX_NATIVE_CALL CallPokeFinderEvent( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ResultPokeFinderEvent( AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetPokeFinderVersion( AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetPokeFinderTotalScore( AMX *, const cell * );

  static cell AMX_NATIVE_CALL CallFieldDemoEvent_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallPokeSelect_( AMX *, const cell * );

  static cell AMX_NATIVE_CALL FieldTradeExcelLoad( AMX *, const cell * );
  static cell AMX_NATIVE_CALL FieldTradeExcelIsLoaded( AMX *, const cell * );
  static cell AMX_NATIVE_CALL FieldTradeExcelGetParam( AMX *, const cell * );
  static cell AMX_NATIVE_CALL FieldTradeExcelEnd( AMX *, const cell * );

  static cell AMX_NATIVE_CALL GetSelectMonsIsEgg( AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetSelectMonsNo( AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetSelectMonsSex( AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetSelectMonsSeikaku( AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetSelectMonsPlace( AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetSelectMonsIsCapture( AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetSelectMonsWazaNum( AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetSelectMonsWaza( AMX *, const cell * );

  static cell AMX_NATIVE_CALL IDPresentGetID( AMX *, const cell * );
  static cell AMX_NATIVE_CALL IDPresentGetParam( AMX *, const cell * );

  static cell AMX_NATIVE_CALL LayoutFadeIn( AMX *, const cell * );
  static cell AMX_NATIVE_CALL LayoutFadeOut( AMX *, const cell * );
  static cell AMX_NATIVE_CALL LayoutFadeIsEnd( AMX *, const cell * );
  
  static cell AMX_NATIVE_CALL FadeStateSave( AMX *, const cell * );
  static cell AMX_NATIVE_CALL FadeStateRecover( AMX *, const cell * );

  static cell AMX_NATIVE_CALL ChrMovePos_( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerSetFormToNormalForce( AMX *, const cell * );
  static cell AMX_NATIVE_CALL PlayerGetForm( AMX *, const cell * );
  static cell AMX_NATIVE_CALL PlayerGetRideMonsNo( AMX *, const cell * );

  

  static cell AMX_NATIVE_CALL PokerusCheckTemoti( AMX *, const cell * );

  static cell AMX_NATIVE_CALL BgPartsSetLoad( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BgPartsSetLoadTrial6Boss( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BgPartsSetDel( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BgPartsDisp( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BgPartsSetIsLoaded( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BgPartsSetIsDeleted( AMX *, const cell * );

  static cell AMX_NATIVE_CALL ChrChangeEye( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrChangeMouth( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrResetEye( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrResetMouth( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrEyeMotionInit_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrEyeMotionEntry_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrEyeMotionPlay_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrMotionCommandInit_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrMotionCommandEntry_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrMotionCommandPlay_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrEyeMotionIsPlaying_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrMouthMotionInit_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrMouthMotionEntry_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrMouthMotionPlay_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrMouthMotionIsPlaying_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrAutoBlink( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrAutoSpeak( AMX *, const cell * );
  
  static cell AMX_NATIVE_CALL ChrNeckRot_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrNeckRotIsFinish_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrNeckReset_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrNeckTarget_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrNeckIsChanged( AMX *, const cell * );

  static cell AMX_NATIVE_CALL PlayerSetDressUpHair_( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerSetDressUpEye_( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerSetDressUpLip_( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerGetHairStyle( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerGetHairColor( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerSetRideDressUp_( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerSetSaveDressUp_( AMX * amx, const cell * ptr );

  static cell AMX_NATIVE_CALL PlayerGetItemID( AMX * amx, const cell * ptr );

  static cell AMX_NATIVE_CALL PlayerPartsEditStart( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerPartsEditSet( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerPartsEditEnd( AMX * amx, const cell * ptr );

  static cell AMX_NATIVE_CALL MapBlockCalcPosSet( AMX *, const cell * );
  static cell AMX_NATIVE_CALL MapBlockCalcPosClear( AMX *, const cell * );
  static cell AMX_NATIVE_CALL MapBlockIsLoading( AMX *, const cell * );
  static cell AMX_NATIVE_CALL MapBlockSetMiddleModelDrawLineLimit( AMX *, const cell * );
  static cell AMX_NATIVE_CALL MapBlockClearMiddleModelDrawLineLimit( AMX *, const cell * );

  static cell AMX_NATIVE_CALL MapBlockHideMeshSystemOn( AMX *, const cell * );
  static cell AMX_NATIVE_CALL MapBlockHideMeshSystemOff( AMX *, const cell * );

  static cell AMX_NATIVE_CALL EffectSetLoc( AMX *, const cell * );
  static cell AMX_NATIVE_CALL EffectDisp( AMX *, const cell * );
  static cell AMX_NATIVE_CALL MayaCameraMotionPlay_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL MayaCameraMotionWait_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BloomChange( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrLookAtChrEye_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrLookAtChrEyePair_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrLookAtFieldActor_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrLookAtPos_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrResetLookAt_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrWaitLookAt_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrLookAtChrKeep( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrLookAtRideKeep( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ChrLookAtChrKeepClear( AMX *, const cell * );

  static cell AMX_NATIVE_CALL GetMonsNo( AMX *, const cell * );
  static cell AMX_NATIVE_CALL FieldItemUnregist( AMX *, const cell * );
  static cell AMX_NATIVE_CALL FieldItemTerminate( AMX *, const cell * );
  static cell AMX_NATIVE_CALL FieldItemIsHidden( AMX *, const cell * );
  static cell AMX_NATIVE_CALL FieldBerrySpotUnregist( AMX *, const cell * );


  static cell AMX_NATIVE_CALL BtlPokeMdlLoad_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BtlPokeMdlDel_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BtlPokeResorceUnload_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BtlPokeMdlIsDone_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BtlPokeMotionPlay_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BtlPokeMotionPlayIsEnd_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BtlPokeDisp_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BtlPokeShadowDisp_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BtlPokeAmbientLightOverWrite( AMX *, const cell * );
  static cell AMX_NATIVE_CALL BtlPokeAmbientLightRestore( AMX *, const cell * );

  static cell AMX_NATIVE_CALL InitMotionBlur( AMX *, const cell * );
  static cell AMX_NATIVE_CALL WaitInitMotionBlur( AMX *, const cell * );
  static cell AMX_NATIVE_CALL EndMotionBlur( AMX *, const cell * );
  static cell AMX_NATIVE_CALL OnMotionBlur( AMX *, const cell * );
  static cell AMX_NATIVE_CALL OffMotionBlur( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ControlMotionBlur( AMX *, const cell * );
  static cell AMX_NATIVE_CALL WaitControlMotionBlur( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ControlMotionBlurColor( AMX *, const cell * );
  static cell AMX_NATIVE_CALL WaitControlMotionBlurColor( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CircleSpotFade_( AMX *, const cell * );

  static cell AMX_NATIVE_CALL KeyInputTry_(AMX *, const cell * );

  static cell AMX_NATIVE_CALL FixAlolaOnlyFakeTime(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ClearAlolaOnlyFakeTime(AMX *, const cell * );
  static cell AMX_NATIVE_CALL IsAlolaTimeReverseSkillUse(AMX *, const cell * );

  static cell AMX_NATIVE_CALL RotomEventSet(AMX *, const cell * );
  static cell AMX_NATIVE_CALL IsRotomEvent(AMX *, const cell * );
  static cell AMX_NATIVE_CALL RotomPowerStart(AMX *, const cell * );
  static cell AMX_NATIVE_CALL RotomPowerClear(AMX *, const cell * );
  static cell AMX_NATIVE_CALL RotomGetRank(AMX *, const cell * );

  static cell AMX_NATIVE_CALL GetTargetActorEventId(AMX *, const cell * );

  static cell AMX_NATIVE_CALL PokecenInit(AMX *, const cell * );
  static cell AMX_NATIVE_CALL PokecenBallSetVisible(AMX *, const cell * );
  static cell AMX_NATIVE_CALL StaticModelMotionByStaticIdPlay_Pokecen(AMX *, const cell * );
  static cell AMX_NATIVE_CALL PokecenFinish(AMX *, const cell * );
  static cell AMX_NATIVE_CALL PokecenGimmickAnimeReq(AMX *, const cell * );
  static cell AMX_NATIVE_CALL PokecenGimmickAnimeIsEnd(AMX *, const cell * );

  static cell AMX_NATIVE_CALL ReportStart_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ReportWait_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL IsReportLock_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL SetTimerIcon(AMX *, const cell * );
  static cell AMX_NATIVE_CALL VanishTimerIcon(AMX *, const cell * );
  static cell AMX_NATIVE_CALL CheckWinAllSuspend(AMX *, const cell * );

  static cell AMX_NATIVE_CALL GetInterestPositionX_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetInterestPositionY_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL GetInterestPositionZ_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL CharKindCheck( AMX *, const cell * );

  static cell AMX_NATIVE_CALL CallMapJumpOtherWorldTime(AMX *, const cell * );
  static cell AMX_NATIVE_CALL CheckAlolaOtherWorld(AMX *, const cell * );

  static cell AMX_NATIVE_CALL CallEventPokemonRide(AMX *, const cell * );
  static cell AMX_NATIVE_CALL PlayerRideOffForce( AMX *, const cell * );
  static cell AMX_NATIVE_CALL PlayerRemakeFieldMenu( AMX *, const cell * );
  static cell AMX_NATIVE_CALL PlayerIsFrameRemakeFieldMenu( AMX *, const cell * );
  
  static cell AMX_NATIVE_CALL UseSprayContinue( AMX *, const cell * );
  static cell AMX_NATIVE_CALL SetDefaultName( AMX *, const cell * );

  static cell AMX_NATIVE_CALL MoveModelResourceLoad( AMX *, const cell * );
  static cell AMX_NATIVE_CALL MoveModelResourceIsLoaded( AMX *, const cell * );
  static cell AMX_NATIVE_CALL MoveModelResourceDel( AMX *, const cell * );

  static cell AMX_NATIVE_CALL PlayEncountEffect_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL WaitPlayEncountEffect_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL SetupEncountEffect_( AMX *, const cell * );

  static cell AMX_NATIVE_CALL CheckMapuku( AMX *, const cell * );
  static cell AMX_NATIVE_CALL EatMarasada( AMX *, const cell * );

  static cell AMX_NATIVE_CALL RegistLift( AMX *, const cell * );
  static cell AMX_NATIVE_CALL UnRegistLift( AMX *, const cell * );

  static cell AMX_NATIVE_CALL testtest(AMX *, const cell * );
  static cell AMX_NATIVE_CALL DebugZukanComp(AMX *, const cell * );

  static cell AMX_NATIVE_CALL ChangeTrialLight(AMX *, const cell * );

  static cell AMX_NATIVE_CALL GetWildBattleCaptureMonsNo(AMX *, const cell *);

  static cell AMX_NATIVE_CALL InactivateFieldScan(AMX *, const cell *);
  static cell AMX_NATIVE_CALL MdlShadowWaist( AMX *, const cell * );

  static cell AMX_NATIVE_CALL ChrGetJointPosX_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL ChrGetJointPosY_(AMX *, const cell *);
  static cell AMX_NATIVE_CALL ChrGetJointPosZ_(AMX *, const cell *);

  static cell AMX_NATIVE_CALL CheckEventPokemonRideTalkTargetActor(AMX *, const cell * ); //< 話しかけ対象の物体付近でライドできるかチェック。
  static cell AMX_NATIVE_CALL CallEventPokemonRideTalkTargetActor(AMX *, const cell * ); //< 話しかけ対象の物体付近でライドを実行する 
  static cell AMX_NATIVE_CALL IsPokemonRide(AMX *, const cell * ); //< paramのライドに乗っているかチェック
  static cell AMX_NATIVE_CALL IsCanExecuteKairikiPos(AMX *, const cell * ); //< @fix GFNMCat[2379] カイリキ岩の押せる位置にいるかチェック

  static cell AMX_NATIVE_CALL ChrLookingAtCamera(AMX *, const cell * ); //< キャラをカメラの方に向ける。(XZ平面)

  static cell AMX_NATIVE_CALL RequestTrafficDisp(AMX *, const cell * ); //!< 往来表示を操作する
  static cell AMX_NATIVE_CALL GetLunaSolDiveDistance(AMX *, const cell *); //!< ルナソルダイブ距離取得
  static cell AMX_NATIVE_CALL GetMantainSurfScore(AMX * amx, const cell * ptr); //!< マンタインサーフスコア取得
  static cell AMX_NATIVE_CALL GetMantainSurfTmpScore(AMX * amx, const cell * ptr); //!< マンタインサーフテンポラリスコア取得
  static cell AMX_NATIVE_CALL RotomRequestFirstTrigger(AMX * amx, const cell * ptr); //!< ロトムのお願い（初めて）達成させる　チュートリアル用

  static cell AMX_NATIVE_CALL PlayerSetSpaceDressUp_(AMX * amx, const cell * ptr);//宇宙服へ着替え（セーブしない）


  static cell AMX_NATIVE_CALL FureaiChrComeNear(AMX * amx, const cell * ptr);
  static cell AMX_NATIVE_CALL FureaiChrSetEnd(AMX * amx, const cell * ptr);
  static cell AMX_NATIVE_CALL FureaiChrSetSleepEnd(AMX * amx, const cell * ptr);
  static cell AMX_NATIVE_CALL ChrMotionSetSpeed(AMX * amx, const cell * ptr);


  static cell AMX_NATIVE_CALL ReqStaffList(AMX * amx, const cell * ptr);
  static cell AMX_NATIVE_CALL WaitStaffList(AMX * amx, const cell * ptr);

  static cell AMX_NATIVE_CALL OpenSakaki(AMX * amx, const cell * ptr);

  public:

  static f32 GetInterestSpeed(poke_3d::model::BaseModel* pCharaDrawInstance, gfl2::math::Vector3 lookPoint, f32 frame);
  static b32 LookAtPointBehindCare( const poke_3d::model::BaseModel* pCharaDrawInstance, f32 behind_range_deg, gfl2::math::Vector3 *pLookpoint );
  static b32 ChrRotTargetImpl(s32 event_id, u32 rot_frame, s32 target_event_id, b32 is_enable_idle_init, b32 is_compress_motion, b32 is_enable_turn_motion_init, b32 isEnableIdleWait = true );
  static b32 ChrRotPosImpl(s32 event_id, u32 rot_frame, const gfl2::math::Vector3& rPos, f32 diff, b32 is_enable_idle_init, b32 is_compress_motion, b32 is_enable_turn_motion_init, b32 isEnableIdleWait = true );
  static f32 DiffAngle(f32 &_fRadA, f32 &_fRadB);

  private:
  static bool WaitFunc_PokePartyNameInput( FieldScriptSystem* p_sys, u32 label, void* p_work );

  static s32 ChrGetJointIndex(poke_3d::model::BaseModel *_pChara, const u32 _locType);
  static gfl2::math::Matrix34 GetLocaterMatrix( poke_3d::model::BaseModel *_pChara, const u32 _locType );
  static gfl2::math::Vector3 GetLocaterPos( poke_3d::model::BaseModel *_pChara, const u32 _locType );
  static gfl2::math::Vector3 GetProjPosition( Camera::CameraManager* pCameraManager, gfl2::math::Vector3 &pos );
  static void LookAtPoint( GameSys::GameManager *_gman, const s32 _eventId, const gfl2::math::Vector3 &_lookpoint, const f32 _fSpeed, const bool _bAbsoluteFrame = false, b32 is_behind_care = false, f32 behind_range_deg = -1.0f );

  static b32 CheckMoveModelUseDynamicAnimation( FieldScriptSystem* p_sys, u32 dynamicAnimationId );

  static bool GetEventIDActorPos( FieldScriptSystem* p_sys, u32 eventID, gfl2::math::Vector3* p_out );

  static cell ChrMotionPlayFrameCore( FieldScriptSystem* pSys, u32 EventID, u32 uMotion, u32 startFrame, s32 interpFrame, s32 joint_type );
 
  static s32 ConvertGetItemIDToScriptEffectID( GetItemID getItemID);
  static s32 ConvertGetItemIDToGetItemIndex( GetItemID getItemID);
  static s32 CalcGetItemIDOffset( GetItemID getItemID);

  struct TalkMsgSeqData
  {
    u8    win_id;    ///< ウィンドウID
    u32   EventID;   ///< キャラのイベントID(0で無効値扱い)
    s32   str_id;    ///< メッセージID
    u16   win_type;  ///< ウィンドウ位置タイプ r:/home/niji/git_program/niji_project/prog/AppLib/include/Tool/TalkWindowType.h
    u16   win_pos;   ///< ウィンドウタイプ
    s32   win_pos_x; ///< ウィンドウ座標X
    s32   win_pos_y; ///< ウィンドウ座標Y
    s32   win_pos_z; ///< ウィンドウ座標Z
    s32   timer;     ///< タイマー
    bool  b_msgbuf;  ///< メッセージバッファ
    //s32   option;  ///< オプション WIN_OPTION_XXX : field/script/ScriptMessageDefine.h
    AMX* amx;
  };
  static bool WaitFunc_TalkMsgSeq( u32 label, void* p_work );

  ///WaitFunctions
  static bool WaitFunc_TimerIconClose( FieldScriptSystem* p_sys, u32 label, void* p_work );

  static bool WaitFunc_StaffList(FieldScriptSystem* p_sys, u32 label, void* p_work);
};


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

#endif // __SCRIPTFUNCSETCOMMON_H__ 重複定義防止
