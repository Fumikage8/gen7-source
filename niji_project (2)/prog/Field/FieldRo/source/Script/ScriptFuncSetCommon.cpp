//==============================================================================
/**
 * @file        ScriptFuncSetCommon.cpp
 * @brief       要約説明
    
        このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/04/13(月) 18:52:48
 */
//==============================================================================

#include <pml/include/item/item.h>
#include <animation/include/gfl2_AnimationId.h>

#include "GameSys/include/GameManager.h"
#include "Applib/include/Util/AppRenderingManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/SaveData.h"
#include "Savedata/include/ResortSave.h"
#include "Savedata/include/QRReaderSave.h"
#include <arc_def_index/arc_def.h>

#include "Sound/include/Sound.h"

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/RegularObject.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/event/EventMapJump.h"
#include "Field/FieldRo/source/EventCheck/FieldEventCheck.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldBgEventActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldActionEventActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldItemActor.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficManager.h"

#include "Field/FieldRo/include/Window/FieldYesNoWindow.h"
#include "Field/FieldRo/include/Window/FieldTalkWindow.h"
#include "Field/FieldRo/include/Window/FieldListMenuWindow.h"

#include "Field/FieldStatic/include/Script/ScriptFuncSetFieldInit.h"
#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"

// field camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"

// field terrain
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainBgPartsManager.h"

// field event
#include "Field/FieldStatic/include/Event/FieldEventPokeFinderForPlacement.h"

#include "Field/FieldRo/include/Subscreen/FieldSubScreenProcManager.h"
#include "Field/FieldStatic/include/Item/FieldItemManager.h"

// Env
#include "Field/FieldRo/include/FieldEnvManager.h"

//ID Kuji
#include "PokeTool/include/PokeTool.h"
#include "PokeTool/include/NakayoshiManager.h"

#include "App/Box/include/BoxAppParam.h"
#include "App/AppEvent/include/Box/BoxEvent.h"

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include <PokeTool/include/PokeMemo.h>

// Effect
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFestivalFire.h"

// MotionBlur
#include "System/include/MotionBlur.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

// ジョインフェスタマップ遷移
#include "NetStatic/NetEvent/include/JoinFestaOpenEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaCloseEvent.h"
// ジョインフェスタギミック
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
// ジョインフェスタパーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
// ジョインフェスタ動的動作モデルロードシステム
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModel.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModelManager.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFDynamicLoadSystem.h"


// field gimmick
#include "Field/FieldRos/FieldGimmickLift/include/FieldGimmickLift.h"
#include "Field/FieldRo/include/Gimmick/FieldGimmickKairikyRock.h"

// TrialModel
#include "Field/FieldRo/include/TrialModel/FieldTrialModelFactory.h"

#include <niji_conv_header/field/FieldGimmickTypes.h>
#include <niji_conv_header/field/script/ScrOffsetId.h>
#include "niji_conv_header/message/script/msg_main_event_0010.h"
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#include "Field/FieldRo/include/Script/ScriptEndCheck.h"
// ロトムパワー
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"
#include "AppLib/include/Tool/AppToolRotomFriendship.h"

#include "App/AppEvent/include/FieldMenu/FieldMenuGameData.h" // for RotomRequest


// fureai
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeNpc.h"
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiTool.h"

//エンディング用スタッフリスト
#include "App/StaffList/include/FieldStaffList.h"

// バトルフェス用
#include <Savedata/include/BattleFesSave.h>

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )

/// 未実装中のものが warning に含まれるので
//#define UNDER_CONSTRUCTION

//class FieldScriptSystem;

s32         FuncSetCommon::m_iWork[ WORK_SIZE ];
f32         FuncSetCommon::m_fAngle;
#if DEBUG_NECKLIMIT
f32         FuncSetCommon::m_fLimitAngH;
f32         FuncSetCommon::m_fLimitAngL;
f32         FuncSetCommon::m_fLimitAngH2;
f32         FuncSetCommon::m_fLimitAngL2;
#endif

//------------------------------------------------------------------------------
/**
 * @func        GetTable
 * @brief       命令テーブルセットを取得.
 * @author      N.Takeda
 * @date        2015/04/13(月) 20:57:50
 *
 * @return      const GFL_PAWN_FUNC_TABLE*  
 */
//------------------------------------------------------------------------------
const GFL_PAWN_FUNC_TABLE   *FuncSetCommon::GetTable( void )
{
  static const GFL_PAWN_FUNC_TABLE s_FuncTable[] = {
    GFL_PAWN_FUNC( GlobalCall )
    GFL_PAWN_FUNC( GlobalCallDebugOnetime )
    GFL_PAWN_FUNC( GlobalGetChildParam )
    GFL_PAWN_FUNC( GlobalSetChildParam )
    GFL_PAWN_FUNC( GlobalGetParentParam )
    GFL_PAWN_FUNC( GlobalSetParentParam )
    
    GFL_PAWN_FUNC( ParallelCall )
    GFL_PAWN_FUNC( ParallelCallIsFinished )
    GFL_PAWN_FUNC( ParallelCallIsFinishedAll )

    GFL_PAWN_FUNC( TrainerIDGet )

    GFL_PAWN_FUNC( MapChangeCore_ )
    GFL_PAWN_FUNC( MapChangeEscape_ )
    GFL_PAWN_FUNC( PlaceNameRequest )
    GFL_PAWN_FUNC( JoinFestaOpen )
    GFL_PAWN_FUNC( JoinFestaClose )

    GFL_PAWN_FUNC( ABKeyWait_ )
    GFL_PAWN_FUNC( LastKeyWait_ )
    GFL_PAWN_FUNC( SlidePadWait_ )
    GFL_PAWN_FUNC( InputKeyIsHold )
    GFL_PAWN_FUNC( InputKeyIsTrigger )
    GFL_PAWN_FUNC( InputKeyIsRelease )
  

    GFL_PAWN_FUNC( ExtraMsgInstant_ )
    GFL_PAWN_FUNC( ExtraMsgLoad_ )
    GFL_PAWN_FUNC( ExtraMsgIsLoaded_ )
    GFL_PAWN_FUNC( ExtraMsgUnload )
    GFL_PAWN_FUNC( ExtraMsgSwap_ )

    GFL_PAWN_FUNC( YesNoWin_Seq )
    GFL_PAWN_FUNC( TalkMdlStartInit_ )
    GFL_PAWN_FUNC( TalkMdlEndInit_ )
    GFL_PAWN_FUNC( TalkMdlGetRepairAngle_ )
    GFL_PAWN_FUNC( TalkMsg_Seq )
    GFL_PAWN_FUNC( TalkMsg_SeqWait )
    GFL_PAWN_FUNC( TalkMsg_VisubleCursor )
    GFL_PAWN_FUNC( MsgWinCloseNo )
    GFL_PAWN_FUNC( ListMenuInit_Seq )
    GFL_PAWN_FUNC( ListMenuAdd )
    GFL_PAWN_FUNC( ListMenuStart_Seq )
    GFL_PAWN_FUNC( DebugListMenuAdd_ )

    GFL_PAWN_FUNC( PG_WordSetRegister )
    GFL_PAWN_FUNC( WordSetNumber )
    GFL_PAWN_FUNC( WordSetMoney )
    
    GFL_PAWN_FUNC( SpMotionPacLoad )
    GFL_PAWN_FUNC( SpMotionPacDel )
    GFL_PAWN_FUNC( SpMotionPacLoad_ )
    GFL_PAWN_FUNC( SpMotionPacDel_ )
    GFL_PAWN_FUNC( SpMotionPacLoadForKisekae )
    GFL_PAWN_FUNC( SpMotionPacDelForKisekae )

    GFL_PAWN_FUNC( SpChrLoad )
    GFL_PAWN_FUNC( SpChrCreate )
    GFL_PAWN_FUNC( SpChrDel )
    GFL_PAWN_FUNC( SpChrUnLoad )

    
    GFL_PAWN_FUNC( ChrMove_ )
    GFL_PAWN_FUNC( ChrMagnetActor_ )
    GFL_PAWN_FUNC( ChrActionCommandIsPlaying_ )
    GFL_PAWN_FUNC( ChrMotionGetPlayNo )
    GFL_PAWN_FUNC( ChrMotionPlayFrame_ )
    GFL_PAWN_FUNC( ChrMotionPlayFrameDirCorrect_ )
    GFL_PAWN_FUNC( ChrMotionIsPlaying_ )
    GFL_PAWN_FUNC( ChrMotionPause )
    
    GFL_PAWN_FUNC( ChrGetDataIndexMax_ )
    GFL_PAWN_FUNC( ChrGetEventIdByDataIndex_ )
    
    GFL_PAWN_FUNC( ChrRot_ )
    GFL_PAWN_FUNC( ChrRotFrame )
    GFL_PAWN_FUNC( ChrRotVec3_ )
    GFL_PAWN_FUNC( ChrRotTarget_ )
    GFL_PAWN_FUNC( ChrStepJump_ )
    GFL_PAWN_FUNC( ChrRotReq )
    GFL_PAWN_FUNC( ChrRotTargetReq )
    GFL_PAWN_FUNC( ChrRotReqClear )
    GFL_PAWN_FUNC( ChrRotPos )
    GFL_PAWN_FUNC( ChrRotFieldActor )
    
    GFL_PAWN_FUNC( ChrFastRot_ )
    GFL_PAWN_FUNC( ChrFastRotVec3_ )
    GFL_PAWN_FUNC( ChrFastRotTarget_ )
    GFL_PAWN_FUNC( ChrFastRotReq )
    GFL_PAWN_FUNC( ChrFastRotTargetReq )
    GFL_PAWN_FUNC( ChrFastRotPos )
    GFL_PAWN_FUNC( ChrFastRotFieldActor )
    GFL_PAWN_FUNC( ChrFastRotFrame )

    GFL_PAWN_FUNC( EvCameraStart )
    GFL_PAWN_FUNC( EvCameraEnd )
    GFL_PAWN_FUNC( EvCameraMoveWait_ )
    GFL_PAWN_FUNC( EvCameraRecordStart_ )
    GFL_PAWN_FUNC( EvCameraRecordEnd_ )
    GFL_PAWN_FUNC( EvCameraMovePos )
    GFL_PAWN_FUNC( EvCameraMoveDist )
    GFL_PAWN_FUNC( EvCameraMoveOffset )
    GFL_PAWN_FUNC( EvCameraMoveLookAt )
    GFL_PAWN_FUNC( EvCameraMoveLookAtChr )
    GFL_PAWN_FUNC( EvCameraMoveLookAtChrEasy )
    GFL_PAWN_FUNC( EvCameraMoveLookAtChrEasyPair )
    GFL_PAWN_FUNC( EvCameraMoveLookAtChrEasyPairDistance )
    GFL_PAWN_FUNC( EvCameraShake )
    GFL_PAWN_FUNC( EvCameraShakeLoop )
    GFL_PAWN_FUNC( EvCameraShakeEnd )
    GFL_PAWN_FUNC( EvCameraAnimationLoad )
    GFL_PAWN_FUNC( EvCameraAnimationUnLoad )
    GFL_PAWN_FUNC( EvCameraAnimationIsLoading )
    GFL_PAWN_FUNC( EvCameraAnimationPlay )
    GFL_PAWN_FUNC( EvCameraAnimationIsPlaying )
    GFL_PAWN_FUNC( FieldCameraShake )
    GFL_PAWN_FUNC( FieldCameraReset )
    GFL_PAWN_FUNC( GlobalCameraShake )

    GFL_PAWN_FUNC( SEPlay )
    GFL_PAWN_FUNC( SEChangeVolume )
    GFL_PAWN_FUNC( SEIsPlaying )
    GFL_PAWN_FUNC( SEStop )
    GFL_PAWN_FUNC( MEPlay )
    GFL_PAWN_FUNC( MEIsPlaying )
    GFL_PAWN_FUNC( MEReturnBGM )
    GFL_PAWN_FUNC( BGMIsPlaying )
    GFL_PAWN_FUNC( BGMChangeNowMap )
    GFL_PAWN_FUNC( BGMStartEvent )
    GFL_PAWN_FUNC( BGMChangeEvent )
    GFL_PAWN_FUNC( BGMEndEvent )
    GFL_PAWN_FUNC( BGMStartBattleEvent )
    GFL_PAWN_FUNC( BGMChangeBattleEvent )
    GFL_PAWN_FUNC( BGMVolumeChange )
    GFL_PAWN_FUNC( BGMVolumeRecover )
    GFL_PAWN_FUNC( BGMPause )
    GFL_PAWN_FUNC( BGMPauseRelease )
    GFL_PAWN_FUNC( VoicePlay_ )
    GFL_PAWN_FUNC( VoicePlay )
    GFL_PAWN_FUNC( VoiceIsPlaying )
    GFL_PAWN_FUNC( GetFieldBGM )

    GFL_PAWN_FUNC( CallWildBattleCore )
    GFL_PAWN_FUNC( ChangeEventWildLose_ )
    GFL_PAWN_FUNC( GetWildBattleResult )
    GFL_PAWN_FUNC( CheckAmaimitsuEncountDataExist )
    GFL_PAWN_FUNC( CallAmaimitsuBattle )
    GFL_PAWN_FUNC( GetWildBattleEffectID )

    GFL_PAWN_FUNC( CallTrainerBattleCore )
    GFL_PAWN_FUNC( TrainerGetInfo )
    GFL_PAWN_FUNC( TrainerGetMsgID )
    GFL_PAWN_FUNC( TrainerGetScrID )
    GFL_PAWN_FUNC( TrainerGetEventID )
    GFL_PAWN_FUNC( ChangeEventTrainerLose_ )
    GFL_PAWN_FUNC( BattleGetResult_ )
    
    GFL_PAWN_FUNC( EffectSetPos )
    GFL_PAWN_FUNC( EffectSetPosScaleRot )
    GFL_PAWN_FUNC( EffectSetChr )
    GFL_PAWN_FUNC( EffectSetTrainerEye )
    GFL_PAWN_FUNC( EffectWait_ )
    GFL_PAWN_FUNC( EffectDel )
    GFL_PAWN_FUNC( EffectNextRotom )
    GFL_PAWN_FUNC( EffectAllDel )
    GFL_PAWN_FUNC( EffectUnloadResource )
    GFL_PAWN_FUNC( EffectFestivalFireStart )
    GFL_PAWN_FUNC( EffectFestivalFireEnd )
    GFL_PAWN_FUNC( EffectFestivalFireIsAlive )
    GFL_PAWN_FUNC( EffectGetItemStart )
    GFL_PAWN_FUNC( EffectTrial3Start )
    GFL_PAWN_FUNC( EffectTrial5Start )
    
    GFL_PAWN_FUNC( S5_EffectChange )
    GFL_PAWN_FUNC( S5_EffectFade )

    GFL_PAWN_FUNC( CallFieldDemoEvent_ )
    GFL_PAWN_FUNC( CallPokeFinderEvent )
    GFL_PAWN_FUNC( ResultPokeFinderEvent )
    GFL_PAWN_FUNC( GetPokeFinderVersion )
    GFL_PAWN_FUNC( GetPokeFinderTotalScore )

    GFL_PAWN_FUNC( FieldTradeExcelLoad )
    GFL_PAWN_FUNC( FieldTradeExcelIsLoaded )
    GFL_PAWN_FUNC( FieldTradeExcelGetParam )
    GFL_PAWN_FUNC( FieldTradeExcelEnd )

    GFL_PAWN_FUNC( GetSelectMonsIsEgg )
    GFL_PAWN_FUNC( GetSelectMonsNo )
    GFL_PAWN_FUNC( GetSelectMonsSex )
    GFL_PAWN_FUNC( GetSelectMonsSeikaku )
    GFL_PAWN_FUNC( GetSelectMonsPlace )
    GFL_PAWN_FUNC( GetSelectMonsIsCapture )
    GFL_PAWN_FUNC( GetSelectMonsWazaNum )
    GFL_PAWN_FUNC( GetSelectMonsWaza )

    GFL_PAWN_FUNC( IDPresentGetID )
    GFL_PAWN_FUNC( IDPresentGetParam )

    GFL_PAWN_FUNC( LayoutFadeOut )
    GFL_PAWN_FUNC( LayoutFadeIn )
    GFL_PAWN_FUNC( LayoutFadeIsEnd )
    
    GFL_PAWN_FUNC( FadeStateSave )
    GFL_PAWN_FUNC( FadeStateRecover )

    GFL_PAWN_FUNC( PlayerGetForm )
    GFL_PAWN_FUNC( PlayerGetRideMonsNo )
    GFL_PAWN_FUNC( PlayerSetFormToNormalForce )
    
    GFL_PAWN_FUNC( PokerusCheckTemoti )

    GFL_PAWN_FUNC( BgPartsSetLoad )
    GFL_PAWN_FUNC( BgPartsSetLoadTrial6Boss )
    GFL_PAWN_FUNC( BgPartsSetDel )
    GFL_PAWN_FUNC( BgPartsDisp )
    GFL_PAWN_FUNC( BgPartsSetIsLoaded )
    GFL_PAWN_FUNC( BgPartsSetIsDeleted )
    GFL_PAWN_FUNC( ChrChangeEye )
    GFL_PAWN_FUNC( ChrChangeMouth )
    GFL_PAWN_FUNC( ChrResetEye )
    GFL_PAWN_FUNC( ChrResetMouth )
    GFL_PAWN_FUNC( ChrEyeMotionInit_ )
    GFL_PAWN_FUNC( ChrEyeMotionEntry_ )
    GFL_PAWN_FUNC( ChrEyeMotionPlay_ )
    GFL_PAWN_FUNC( ChrMotionCommandInit_ )
    GFL_PAWN_FUNC( ChrMotionCommandEntry_ )
    GFL_PAWN_FUNC( ChrMotionCommandPlay_ )
    GFL_PAWN_FUNC( ChrEyeMotionIsPlaying_ )
    GFL_PAWN_FUNC( ChrMouthMotionInit_ )
    GFL_PAWN_FUNC( ChrMouthMotionEntry_ )
    GFL_PAWN_FUNC( ChrMouthMotionPlay_ )
    GFL_PAWN_FUNC( ChrMouthMotionIsPlaying_ )
    GFL_PAWN_FUNC( ChrAutoBlink )
    GFL_PAWN_FUNC( ChrAutoSpeak )
    GFL_PAWN_FUNC( ChrNeckRot_ )
    GFL_PAWN_FUNC( ChrNeckTarget_ )
    GFL_PAWN_FUNC( ChrNeckReset_ )
    GFL_PAWN_FUNC( ChrNeckRotIsFinish_ )
    GFL_PAWN_FUNC( ChrNeckIsChanged )
    GFL_PAWN_FUNC( PlayerSetDressUpHair_ )
    GFL_PAWN_FUNC( PlayerSetDressUpEye_ )
    GFL_PAWN_FUNC( PlayerSetDressUpLip_ )
    GFL_PAWN_FUNC( PlayerGetHairStyle )
    GFL_PAWN_FUNC( PlayerGetHairColor )
    GFL_PAWN_FUNC( PlayerSetRideDressUp_ )
    GFL_PAWN_FUNC( PlayerSetSaveDressUp_ )
    GFL_PAWN_FUNC( PlayerGetItemID )
    GFL_PAWN_FUNC( PlayerPartsEditStart )
    GFL_PAWN_FUNC( PlayerPartsEditSet )
    GFL_PAWN_FUNC( PlayerPartsEditEnd )

    GFL_PAWN_FUNC( MapBlockCalcPosSet )
    GFL_PAWN_FUNC( MapBlockCalcPosClear )
    GFL_PAWN_FUNC( MapBlockIsLoading )
    GFL_PAWN_FUNC( MapBlockSetMiddleModelDrawLineLimit )
    GFL_PAWN_FUNC( MapBlockClearMiddleModelDrawLineLimit )
    
    GFL_PAWN_FUNC( MapBlockHideMeshSystemOn )
    GFL_PAWN_FUNC( MapBlockHideMeshSystemOff )
    
    GFL_PAWN_FUNC( EffectSetLoc )
    GFL_PAWN_FUNC( EffectDisp )
    GFL_PAWN_FUNC( MayaCameraMotionPlay_ )
    GFL_PAWN_FUNC( MayaCameraMotionWait_ )
    GFL_PAWN_FUNC( BloomChange )
    GFL_PAWN_FUNC( ChrLookAtChrEye_ )
    GFL_PAWN_FUNC( ChrLookAtChrEyePair_ )
    GFL_PAWN_FUNC( ChrLookAtFieldActor_ )
    GFL_PAWN_FUNC( ChrLookAtPos_ )
    GFL_PAWN_FUNC( ChrResetLookAt_ )
    GFL_PAWN_FUNC( ChrWaitLookAt_ )
    GFL_PAWN_FUNC( ChrLookAtChrKeep )
    GFL_PAWN_FUNC( ChrLookAtRideKeep )
    GFL_PAWN_FUNC( ChrLookAtChrKeepClear )

    GFL_PAWN_FUNC( GetMonsNo )
    GFL_PAWN_FUNC( FieldItemUnregist )
    GFL_PAWN_FUNC( FieldItemTerminate )
    GFL_PAWN_FUNC( FieldItemIsHidden )
    GFL_PAWN_FUNC( FieldBerrySpotUnregist )

    GFL_PAWN_FUNC( BtlPokeMdlLoad_ )
    GFL_PAWN_FUNC( BtlPokeMdlDel_ )
    GFL_PAWN_FUNC( BtlPokeResorceUnload_ )
    GFL_PAWN_FUNC( BtlPokeMdlIsDone_ )
    GFL_PAWN_FUNC( BtlPokeMotionPlay_ )
    GFL_PAWN_FUNC( BtlPokeMotionPlayIsEnd_ )
    GFL_PAWN_FUNC( BtlPokeDisp_ )
    GFL_PAWN_FUNC( BtlPokeShadowDisp_ )
    GFL_PAWN_FUNC( BtlPokeAmbientLightOverWrite )
    GFL_PAWN_FUNC( BtlPokeAmbientLightRestore )

    GFL_PAWN_FUNC(InitMotionBlur)
    GFL_PAWN_FUNC(WaitInitMotionBlur)
    GFL_PAWN_FUNC(EndMotionBlur)
    GFL_PAWN_FUNC(OnMotionBlur)
    GFL_PAWN_FUNC(OffMotionBlur)
    GFL_PAWN_FUNC(ControlMotionBlur)
    GFL_PAWN_FUNC(WaitControlMotionBlur)
    GFL_PAWN_FUNC(ControlMotionBlurColor)
    GFL_PAWN_FUNC(WaitControlMotionBlurColor)
    GFL_PAWN_FUNC(CircleSpotFade_)

    GFL_PAWN_FUNC( KeyInputTry_ )

    GFL_PAWN_FUNC( FixAlolaOnlyFakeTime )
    GFL_PAWN_FUNC( ClearAlolaOnlyFakeTime )
    GFL_PAWN_FUNC( IsAlolaTimeReverseSkillUse )
    
    GFL_PAWN_FUNC( RotomEventSet )
    GFL_PAWN_FUNC( IsRotomEvent )
    GFL_PAWN_FUNC( RotomPowerStart )
    GFL_PAWN_FUNC( RotomPowerClear )
    GFL_PAWN_FUNC( RotomGetRank )
    
    GFL_PAWN_FUNC( GetTargetActorEventId )
    
    GFL_PAWN_FUNC( GetInterestPositionX_ )
    GFL_PAWN_FUNC( GetInterestPositionY_ )
    GFL_PAWN_FUNC( GetInterestPositionZ_ )
    GFL_PAWN_FUNC( CharKindCheck )

    GFL_PAWN_FUNC( PokecenInit )
    GFL_PAWN_FUNC( PokecenBallSetVisible )
    GFL_PAWN_FUNC( StaticModelMotionByStaticIdPlay_Pokecen )
    GFL_PAWN_FUNC( PokecenFinish )
    GFL_PAWN_FUNC( PokecenGimmickAnimeReq )
    GFL_PAWN_FUNC( PokecenGimmickAnimeIsEnd )

    GFL_PAWN_FUNC( ReportStart_ )
    GFL_PAWN_FUNC( ReportWait_ )
    GFL_PAWN_FUNC( IsReportLock_ )
    GFL_PAWN_FUNC( SetTimerIcon )
    GFL_PAWN_FUNC( VanishTimerIcon )
    GFL_PAWN_FUNC( CheckWinAllSuspend )
    
    GFL_PAWN_FUNC( CallMapJumpOtherWorldTime )
    GFL_PAWN_FUNC( CheckAlolaOtherWorld )

    GFL_PAWN_FUNC( CallEventPokemonRide )
    GFL_PAWN_FUNC( PlayerRideOffForce )
    GFL_PAWN_FUNC( PlayerRemakeFieldMenu )
    GFL_PAWN_FUNC( PlayerIsFrameRemakeFieldMenu )

    GFL_PAWN_FUNC( UseSprayContinue )
    GFL_PAWN_FUNC( SetDefaultName )

    GFL_PAWN_FUNC( MoveModelResourceLoad )
    GFL_PAWN_FUNC( MoveModelResourceIsLoaded )
    GFL_PAWN_FUNC( MoveModelResourceDel )

    GFL_PAWN_FUNC( PlayEncountEffect_ )
    GFL_PAWN_FUNC( WaitPlayEncountEffect_ )
    GFL_PAWN_FUNC( SetupEncountEffect_ )
    GFL_PAWN_FUNC( CheckMapuku )
    GFL_PAWN_FUNC( EatMarasada )

    GFL_PAWN_FUNC( RegistLift )
    GFL_PAWN_FUNC( UnRegistLift )

    GFL_PAWN_FUNC( ChrMovePos_ )

    GFL_PAWN_FUNC( testtest )
    GFL_PAWN_FUNC( DebugZukanComp )

    GFL_PAWN_FUNC( ChangeTrialLight )

    GFL_PAWN_FUNC( GetWildBattleCaptureMonsNo )

    GFL_PAWN_FUNC( InactivateFieldScan )
    GFL_PAWN_FUNC( MdlShadowWaist )

    GFL_PAWN_FUNC( ChrGetJointPosX_ )
    GFL_PAWN_FUNC( ChrGetJointPosY_ )
    GFL_PAWN_FUNC( ChrGetJointPosZ_ )

    GFL_PAWN_FUNC( CheckEventPokemonRideTalkTargetActor )
    GFL_PAWN_FUNC( CallEventPokemonRideTalkTargetActor )
    GFL_PAWN_FUNC( IsPokemonRide )
    GFL_PAWN_FUNC( IsCanExecuteKairikiPos )
    GFL_PAWN_FUNC( ChrLookingAtCamera )

    GFL_PAWN_FUNC( RequestTrafficDisp )

    GFL_PAWN_FUNC( GetLunaSolDiveDistance )
    GFL_PAWN_FUNC( GetMantainSurfScore )
    GFL_PAWN_FUNC( GetMantainSurfTmpScore )
    GFL_PAWN_FUNC( PlayerSetSpaceDressUp_ )
    GFL_PAWN_FUNC( RotomRequestFirstTrigger )
    GFL_PAWN_FUNC( FureaiChrComeNear )
    GFL_PAWN_FUNC( FureaiChrSetEnd )
    GFL_PAWN_FUNC( FureaiChrSetSleepEnd )
    GFL_PAWN_FUNC( ChrMotionSetSpeed )
    
    GFL_PAWN_FUNC( ReqStaffList )
    GFL_PAWN_FUNC( WaitStaffList )

    GFL_PAWN_FUNC( OpenSakaki )

    GFL_PAWN_FUNC_END,
  };
  
  return s_FuncTable;
}


// SCRのRidePokemonIDとFieldのRidePokemonIDの変換テーブル
static const Field::RIDE_POKEMON_ID sc_RidePokemonID[ SCR_RIDE_POKEMON_NUM ] = 
{
  RIDE_KENTAROSU, // ケンタロス
  RIDE_SAMEHADA,  // サメハダー
  RIDE_MUURANDO,  // ムーランド
  RIDE_LAPLACE,   // ラプラス
  RIDE_KAIRIKY,   // カイリキー  
  RIDE_RIZAADON,  // リザードン
  RIDE_ROBA2,     // バンバドロ
};

s32 FuncSetCommon::ChrGetJointIndex( poke_3d::model::BaseModel *_pChara, const u32 _locType )
{
  s32 joint_index = -1;
  switch (_locType)
  {
  case CHR_LOC_HEAD:
  case CHR_LOC_HEAD_W:
    joint_index = _pChara->GetJointIndex("Loc_Head");
    break;
  case CHR_LOC_EYE:
  case CHR_LOC_EYE_W:
    joint_index = _pChara->GetJointIndex("Loc_Eye");
    break;
  case CHR_LOC_MOUTH:
  case CHR_LOC_MOUTH_W:
    joint_index = _pChara->GetJointIndex("Loc_Mouth");
    break;
  case CHR_LOC_CAM_HEAD:
  case CHR_LOC_CAM_HEAD_W:
    joint_index = _pChara->GetJointIndex("CamTgtHead");
    break;
  case CHR_LOC_CAM_BUST:
  case CHR_LOC_CAM_BUST_W:
    joint_index = _pChara->GetJointIndex("CamTgtBust");
    break;
  case CHR_LOC_CAM_BODY:
  case CHR_LOC_CAM_BODY_W:
    joint_index = _pChara->GetJointIndex("CamTgtBody");
    break;
  case CHR_LOC_CAM_FOOT:
  case CHR_LOC_CAM_FOOT_W:
    joint_index = _pChara->GetJointIndex("CamTgtFoot");
    break;
  case CHR_JOINT_WAIST:
  case CHR_JOINT_WAIST_W:
    joint_index = _pChara->GetJointIndex("Waist");
    break;
  case CHR_LOC_POSITION:
  default:
    break;
  }

  return joint_index;
}


// ジョイントのワールド行列を取得.
gfl2::math::Matrix34 FuncSetCommon::GetLocaterMatrix( poke_3d::model::BaseModel *_pChara, const u32 _locType )
{
  // @note 対象モデルのトラバースを実行。対象がカリングされている場合、jointの行列が更新されていない事がある。
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast( _pChara->GetModelInstanceNode() );

  gfl2::math::Matrix34  locatorMtx;
  switch( _locType )
  {
  case CHR_LOC_HEAD:
  case CHR_LOC_HEAD_W:
    locatorMtx = _pChara->GetJointWorldMatrix( _pChara->GetJointIndex( "Loc_Head" ) );
    break;
  case CHR_LOC_EYE:
  case CHR_LOC_EYE_W:
    locatorMtx = _pChara->GetJointWorldMatrix( _pChara->GetJointIndex( "Loc_Eye" ) );
    break;
  case CHR_LOC_MOUTH:
  case CHR_LOC_MOUTH_W:
    locatorMtx = _pChara->GetJointWorldMatrix( _pChara->GetJointIndex( "Loc_Mouth" ) );
    break;
  case CHR_LOC_CAM_HEAD:
  case CHR_LOC_CAM_HEAD_W:
    locatorMtx = _pChara->GetJointWorldMatrix( _pChara->GetJointIndex( "CamTgtHead" ) );
    break;
  case CHR_LOC_CAM_BUST:
  case CHR_LOC_CAM_BUST_W:
    locatorMtx = _pChara->GetJointWorldMatrix( _pChara->GetJointIndex( "CamTgtBust" ) );
    break;
  case CHR_LOC_CAM_BODY:
  case CHR_LOC_CAM_BODY_W:
    locatorMtx = _pChara->GetJointWorldMatrix( _pChara->GetJointIndex( "CamTgtBody" ) );
    break;
  case CHR_LOC_CAM_FOOT:
  case CHR_LOC_CAM_FOOT_W:
    locatorMtx = _pChara->GetJointWorldMatrix( _pChara->GetJointIndex( "CamTgtFoot" ) );
    break;
  case CHR_JOINT_WAIST:
  case CHR_JOINT_WAIST_W:
    locatorMtx = _pChara->GetJointWorldMatrix( _pChara->GetJointIndex( "Waist" ) );
    break;
  case CHR_LOC_POSITION:
  default:
    locatorMtx = gfl2::math::Matrix34::GetTranslation( _pChara->GetPosition() );
    break;
  }
  return locatorMtx;
}

// ジョイントのワールドPosを取得.
gfl2::math::Vector3 FuncSetCommon::GetLocaterPos( poke_3d::model::BaseModel *_pChara, const u32 _locType )
{
  gfl2::math::Matrix34  locatorMtx = GetLocaterMatrix(_pChara, _locType);
  return locatorMtx.GetElemPosition();
}

//--------------------------------------------------------------
/**
 * @brief   射影後座標を取得
 * @param   pCameraManager カメラマネージャ
 * @param　 pos   ３D空間座標
 * @return  gfl2::math::Vector3 変換後座標 
 */
//--------------------------------------------------------------
gfl2::math::Vector3 FuncSetCommon::GetProjPosition( Camera::CameraManager* pCameraManager, gfl2::math::Vector3 &pos )
{
  gfl2::math::Matrix44 mtx_proj;
  gfl2::math::Matrix34 mtx_view;
  pCameraManager->GetMainViewCamera()->GetProjectionMatrix( &mtx_proj );
  pCameraManager->GetMainViewCamera()->GetViewMatrix( &mtx_view );
  gfl2::math::Vector4 view_pos = mtx_view * pos;
  {
    view_pos.x = static_cast<f32>(static_cast<s32>(view_pos.x));
    view_pos.y = static_cast<f32>(static_cast<s32>(view_pos.y));
    view_pos.z = static_cast<f32>(static_cast<s32>(view_pos.z));
  }
  gfl2::math::Vector4 proj_pos = mtx_proj * view_pos;
  {
    proj_pos.x = static_cast<f32>(static_cast<s32>(proj_pos.x));
    proj_pos.y = static_cast<f32>(static_cast<s32>(proj_pos.y));
    proj_pos.z = static_cast<f32>(static_cast<s32>(proj_pos.z));
  }
  gfl2::math::Vector4 dst_pos = proj_pos;

  gfl2::math::Vector3 screen_pos( 0.0f, 0.0f, 0.0f);
  dst_pos.x = dst_pos.x / dst_pos.w;
  dst_pos.y = dst_pos.y / dst_pos.w;
  screen_pos.x = static_cast<f32>( static_cast<s32>( dst_pos.x * 200.0f ) );
  screen_pos.y = static_cast<f32>( static_cast<s32>( dst_pos.y * 120.0f ) );

  return screen_pos;

}


f32 FuncSetCommon::GetInterestSpeed( poke_3d::model::BaseModel* pCharaDrawInstance, gfl2::math::Vector3 lookPoint, f32 frame )
{
  /// Head から回転のみを Matrix34 に取り出し.
  gfl2::math::Matrix34  locatorEyeMtx = GetLocaterMatrix(pCharaDrawInstance, CHR_LOC_HEAD);
  gfl2::math::Vector3 headPos = locatorEyeMtx.GetElemPosition();
  gfl2::math::Vector3 headRot = locatorEyeMtx.GetElemRotationZYX_fix();
  locatorEyeMtx = gfl2::math::Matrix34::GetIdentity();
  locatorEyeMtx = gfl2::math::Matrix34::GetRotationZYX(headRot.z, headRot.y, headRot.x);

  /// 現在の向きから、注視点までの角を得る.
  gfl2::math::Vector3 Dir = lookPoint - headPos;
  gfl2::math::Matrix34 rotChr = locatorEyeMtx;
  gfl2::math::Vector3 v1(0, 0, 1);

  // GFCHECK[2976]:スクリプト記述とタイミングにより、0除算が発生する可能性があるため念のためケア
  if(Dir.Length() < 0.0001)
  {
    return 0.01;
  }

  Dir.Normalize();
  v1 = rotChr * v1;
  f32 angle = gfl2::math::FACos(Dir.Dot(v1) / (Dir.Length()*v1.Length()));
  if (v1.x < 0)  angle = (PI - angle) + PI;  //反転
  if (angle >= PI)  angle = angle - PI*2.f; ///< 180～359 -> -180～-1

#if 1
  if (pCharaDrawInstance->GetInterestType(0) == gfl2::renderingengine::InterestType::XY)
  {
    f32 limitAngH = pCharaDrawInstance->GetInterestLimitAngleHigh(0);
    f32 limitAngL = pCharaDrawInstance->GetInterestLimitAngleLow(0);
    if (angle > limitAngH) angle = limitAngH;
    if (angle < limitAngL) angle = limitAngL;
  }
#endif
  f32 speed = gfl2::math::FAbs(angle / frame );
  return speed;
}

//------------------------------------------------------------------------------
/**
 * @func        LookAtPoint
 * @brief       LookAtポイント関数.
 * @author      N.Takeda
 * @date        2015/09/02(水) 19:03:10
 *
 * @param[in,out] GameSys::GameManager*       _gman           .
 * @param[in]     const s32                   _eventId        ID.
 * @param[in]     const gfl2::math::Vector3&  _lookpoint      ポイント.
 * @param[in]     const f32                   _fSpeed         速度またはフレーム.
 * @param[in]     const bool                  _bAbsoluteFrame false:frame は 1frmで動く角度. true:frame は動かすframe数(省略するとfalse).
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void FuncSetCommon::LookAtPoint( GameSys::GameManager *_gman, const s32 _eventId, const gfl2::math::Vector3 &_lookpoint, const f32 _fSpeed, const bool _bAbsoluteFrame /* = false */, b32 is_behind_care,  f32 behind_range_deg )
{
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = _gman->GetGameData()->GetFieldCharaModelManager();
  
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( _eventId );
  Field::MoveModel::FieldMoveModel* pChar = pFieldMoveModelManager->GetFieldMoveModel( ChrID ) ;
  gfl2::math::Vector3 lookPoint( _lookpoint );


  if( pChar==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", _eventId );
    return;
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetCharaDrawInstance();
  
  if( is_behind_care )
  {
    // @fix GFNMcat[3166]:椅子座りNPCに真後ろから話しかけた際、会話モーション中に首が左右にカクつく対処。対象NPCが振り向かない場合用に、真後ろ扱いとするレンジに注目座標を加工するオプションを実装
    LookAtPointBehindCare( pCharaDrawInstance, behind_range_deg, &lookPoint );
  }


#if 0//def PM_DEBUG
  pCharaDrawInstance->SetInterestLimitAngleHigh( 0, gfl2::math::ConvDegToRad(180) );//テスト　エクソシスト.
  pCharaDrawInstance->SetInterestLimitAngleHigh2( 0, gfl2::math::ConvDegToRad(180) );//テスト　エクソシスト.
#endif  // PM_DEBUG
  
  if( _bAbsoluteFrame == false )
  {
    pCharaDrawInstance->SetInterestSpeed( 0, gfl2::math::ConvDegToRad( _fSpeed ) );   ///< speed は1回のUpdateで回転させる角度（ラジアン）
  }
  else
  {
    /// Head から回転のみを Matrix34 に取り出し.
    gfl2::math::Matrix34  locatorEyeMtx = GetLocaterMatrix( pCharaDrawInstance, CHR_LOC_HEAD );
    gfl2::math::Vector3 headPos = locatorEyeMtx.GetElemPosition();
    gfl2::math::Vector3 headRot = locatorEyeMtx.GetElemRotationZYX_fix();
    locatorEyeMtx = gfl2::math::Matrix34::GetIdentity();
    locatorEyeMtx = gfl2::math::Matrix34::GetRotationZYX( headRot.z, headRot.y, headRot.x );
    
    /// 現在の向きから、注視点までの角を得る.
    gfl2::math::Vector3 Dir = lookPoint - headPos;
    gfl2::math::Matrix34 rotChr = locatorEyeMtx;
    gfl2::math::Vector3 v1( 0, 0, 1 );
    
    Dir.Normalize();
    v1 = rotChr * v1;
    f32 angle = gfl2::math::FACos(Dir.Dot(v1)/(Dir.Length()*v1.Length()));
    if( v1.x < 0 )  angle = (PI - angle) + PI;  //反転
    if( angle >= PI )  angle = angle - PI*2.f; ///< 180～359 -> -180～-1
    
#if 1
    if( pCharaDrawInstance->GetInterestType( 0 ) == gfl2::renderingengine::InterestType::XY )
    {
      f32 limitAngH = pCharaDrawInstance->GetInterestLimitAngleHigh( 0 );
      f32 limitAngL = pCharaDrawInstance->GetInterestLimitAngleLow( 0 );
      if( angle > limitAngH ) angle = limitAngH;
      if( angle < limitAngL ) angle = limitAngL;
    }
#endif
    f32 frame = gfl2::math::FAbs(angle / _fSpeed);
    pCharaDrawInstance->SetInterestSpeed( 0, frame );   ///< frame は1回のUpdateで回転させる角度（ラジアン）
    if( _fSpeed == 1.f ){
      for(u32 ii=1 ; ii<pCharaDrawInstance->GetInterestJointCount() ; ii++)
      {
        pCharaDrawInstance->SetInterestSpeed( ii, frame );
      }
    }
  }
  
  if( pChar )
  {
    pChar->EnableInterest( lookPoint );
  }
  
}

/**
 * @brief       注目座標が指定キャラの真後ろ扱いとするレンジに収まっている場合、注目座標をレンジ外に加工する処理
 *
 * @param       pCharaDrawInstance  指定キャラ
 * @param       behind_range_deg    真後ろ扱いとするレンジ(度、片翼)、-1でプログラムデフォルト値
 * @param       pLookpoint          注目座標
 *
 * @return      bool  TRUE:加工された / FALSE:加工されていない
 */
b32 FuncSetCommon::LookAtPointBehindCare( const poke_3d::model::BaseModel* pCharaDrawInstance, f32 behind_range_deg, gfl2::math::Vector3 *pLookpoint )
{
  if( behind_range_deg <= 0.0f )
  {
    behind_range_deg = 80.0f;
  }

  // キャラクターの向きベクトルを取得
  gfl2::math::Matrix34  nowRotMat;
  gfl2::math::Vector3 nowRotVec(0.0f,0.0,1.0f);
  pCharaDrawInstance->GetRotationQuat().QuaternionToMatrix( nowRotMat );  
  nowRotVec   = nowRotMat.TransformNormal(nowRotVec);
  nowRotVec.y = 0.0f;
  nowRotVec   = nowRotVec.Normalize();

  // 目標向きベクトルを取得
  gfl2::math::Vector3 charToLookVec( *pLookpoint - pCharaDrawInstance->GetPosition() );
  charToLookVec.y = 0.0f;
  charToLookVec = charToLookVec.Normalize();

  // 今の向きから注目方向の角度を求める
  f32 dot = charToLookVec.Dot( nowRotVec );
  f32 angleToTargetDeg = gfl2::math::ConvRadToDeg( atan2( gfl2::math::Vector2( nowRotVec.x, nowRotVec.z ).Cross( gfl2::math::Vector2( charToLookVec.x, charToLookVec.z ) ), dot ) );

  // 真後ろのレンジ内の場合、レンジの辺に吸着する
  if( angleToTargetDeg >= 180.0f - behind_range_deg || angleToTargetDeg <= (-180.0f + behind_range_deg ) )
  {
    f32 rotDeg = 0.0f;
    if( angleToTargetDeg >= 0.0f )
    {
      rotDeg = ( 180.0f - behind_range_deg ) - angleToTargetDeg;
    }
    else
    {
      rotDeg = ( -180.0f + behind_range_deg ) - angleToTargetDeg;
    }

    gfl2::math::Vector3 charToLookVecWork( *pLookpoint - pCharaDrawInstance->GetPosition() );
    charToLookVecWork = gfl2::math::Matrix34::GetRotationY( gfl2::math::ConvDegToRad( rotDeg ) * -1.0f ).TransformCoord( charToLookVecWork );
    *pLookpoint = pCharaDrawInstance->GetPosition() + charToLookVecWork;
    return true;
  }

  return false;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプトコマンド後処理チェック関数を回す下準備
 */
/* ------------------------------------------------------------------------- */
void FuncSetCommon::SetupScriptFuncEndCheck( GameSys::GameManager* p_gman )
{
  FieldScriptSystem     *p_sys          = p_gman->GetFieldScriptSystem(); 
//  SystemSingletones     *p_swk          = p_sys->GetSystemWork()->GetSingletones();
  Fieldmap              *pFieldMap     = p_gman->GetFieldmap();

  FuncSetCommon::ClearWork();

  if ( pFieldMap == NULL )
  {
    if( p_sys->GetBgPartsCount() ){
      //残っていたらセキュリティを起動する
      p_sys->GetSystemWork()->OnCheckBit( SCR_END_CHECK_BIT_BG_PARTS );
    }  
    return;
  }
  //ここから下はフィールドがあるときだけ
  {
    Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
    Terrain::TerrainBgPartsManager* pBgPartsManager = pTerrainManager->GetBgPartsManager();

    if( pBgPartsManager->CountBgParts() > 0 ){
      //残っていたらセキュリティを起動する
      p_sys->GetSystemWork()->OnCheckBit( SCR_END_CHECK_BIT_BG_PARTS );
    }

    // OFFにしたままはNG
    if( pTerrainManager->IsHideMeshSystemOn() == false )
    {
      p_sys->GetSystemWork()->OnCheckBit( SCR_END_CHECK_BIT_HIDE_MESH_SYSTEM );
    }
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプトコマンド後処理チェック関数
 *
 * @return true:完了, false:未完了
 *
 * @note trueが返るまで呼び続ける必要がある
 * @note fieldmapが破棄された状態で呼ばれることもあるので注意！
 */
/* ------------------------------------------------------------------------- */
bool FuncSetCommon::CheckScriptFuncEnd( GameSys::GameManager* p_gman )
{
  // ../script/ScrEndCheckBit.h のSCR_END_CHECK_BITと一致させる
  const SCR_END_CHECK_FUNC sc_CheckEndFuncTbl[] =
  {
    &SCREND_CheckEndWin,
    &SCREND_CheckEndEventCamera,
    &SCREND_CheckEndBgParts,
    &SCREND_CheckEndMapBlockCalcPos, 
    &SCREND_CheckEndMotionBlur, 
    &SCREND_CheckEndMapBlockHideMeshSystemOn, 
    NULL, // テーブル終了検出用
  };

#if PM_DEBUG
  static const char* sc_DebugErrorMsg[] = {
    "ウィンドウクローズ",
    "イベントカメラ",
    "BgParts未解放",
    "MapBlockCalcPosが設定されたまま",
    "MotionBlurが破棄されてない",
    "MapBlockHideMeshSystemOnを実行する必要がある",
    "NULL",
  };
#if 0
  // テーブルの数を一致させる
  NN_COMPILER_ASSERT( GFL_NELEMS(sc_CheckEndFuncTbl) == GFL_NELEMS(sc_DebugErrorMsg) );
#endif
#endif

  FieldScriptSystem * p_sys = p_gman->GetFieldScriptSystem();

  // 全てのチェック関数を回す

  u32 id = 0;

  SCR_END_CHECK_BIT bit = SCR_END_CHECK_BIT_NONE;
  do
  {
    bool rc;
    SCR_END_CHECK_FUNC func;

    bit = static_cast<SCR_END_CHECK_BIT>( 1 << id );

    // ファンクション取得
    func = sc_CheckEndFuncTbl[id];
    // 終了チェック
    if( func == NULL )
    {
      // 全ファンクション終了
#if PM_DEBUG
      p_sys->GetSystemWork()->SetEndCheckID( -1 );
#endif
      break;
    }

    // ビットチェック
    if( p_sys->GetSystemWork()->CheckBit( bit ) )
    {
#if PM_DEBUG
      if ( p_sys->GetSystemWork()->GetEndCheckID() != static_cast<s32>(id) )
      {
        SCRIPT_ASSERT_MSG( 0, "============ %s::EndFunc Not Call ===========\n",
            sc_DebugErrorMsg[id] );
        p_sys->GetSystemWork()->SetEndCheckID( id );
      }
#endif

      // ファンクション実行
      rc = func( p_gman, &m_iWork[id] );
      if( rc )
      {
        // 次のファンクションへ
        id++;
      }
      else
      {
        return false; // 一度処理を返す
      }
    }
    else
    {
      // 次のファンクションへ
      id++;
    }
  }while( id < GFL_NELEMS(sc_CheckEndFuncTbl) );

  /// LayoutFadeをクリア.
  if( p_sys->GetEffect() )
  {
    if( p_gman->GetFieldmap() )
    {
      Field::Effect::EffectManager  *pEffMgr = p_gman->GetFieldmap()->GetEffectManager();
      pEffMgr->DeleteEffect( p_sys->GetEffect() );
    }
    p_sys->SetEffect( NULL );
  }

  // エフェクトの非常駐リソースを破棄
  p_sys->GetScriptEffectManager()->UnloadDynamicResource();
  
  /// 動的に読み込んだ動作モデルリソース破棄.
  p_sys->GetScriptMoveModelManager()->UnloadResourceForce();
  
  // 1フレームで完了するチェックを実行
  CheckOntime( p_gman );

  return true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプトコマンド フィールド破棄時処理チェック関数
 */
/* ------------------------------------------------------------------------- */
void FuncSetCommon::ScriptFuncTerminateFieldTop( GameSys::GameManager* p_gman )
{
  FieldScriptSystem *p_sys = p_gman->GetFieldScriptSystem();
  SystemSingletones *p_swk = p_sys->GetSystemWork()->GetSingletones();
  Fieldmap          *pFieldmap = p_gman->GetFieldmap();

  //会話ウィンドウの残存チェック
  {
    if( p_swk->m_pFieldTalkWindow[0] ){
      SCRIPT_ASSERT_MSG( 0,"メッセージウィンドウ0がクローズされていません\n");
      p_swk->m_pFieldTalkWindow[0]->CloseMsgWin();
    }
    if( p_swk->m_pFieldTalkWindow[1] ){
      SCRIPT_ASSERT_MSG( 0,"メッセージウィンドウ1がクローズされていません\n");
      p_swk->m_pFieldTalkWindow[1]->CloseMsgWin();
    }
  }

  {
    if ( p_sys->GetSystemWork()->CheckBit( SCR_END_CHECK_BIT_MOTION_BLUR ) )
    {
      //FieldmapのTerminateがケアでRenderOffとTerminateしてくれるので、ここではフラグだけ取り下げる
      SCRIPT_WARNING_MSG( 0,"モーションブラーを描画しっぱなし\n");
      p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_MOTION_BLUR ); 
    }
  }
  
  /// LayoutFade保持ポインタの初期化.
  if( p_sys->GetEffect() )
  {
    if( p_gman->GetFieldmap() ){
      Field::Effect::EffectManager  *pEffMgr = p_gman->GetFieldmap()->GetEffectManager();
      if( pEffMgr ) pEffMgr->DeleteEffect( p_sys->GetEffect() );
    }
    p_sys->SetEffect( NULL );
  }
  
}

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプトコマンド フィールド破棄時処理関数
 */
/* ------------------------------------------------------------------------- */
void FuncSetCommon::ScriptFuncTerminateFieldBottom( GameSys::GameManager* p_gman )
{
  FieldScriptSystem *p_sys = p_gman->GetFieldScriptSystem();
  SystemSingletones *p_swk = p_sys->GetSystemWork()->GetSingletones();

  //カメラコントローラーの後処理
  {
    p_sys->SetCameraController(NULL);
    p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_EVENT_CAMERA );
  }
  //会話ウィンドウの破棄
  {
    GFL_SAFE_DELETE( p_swk->m_pFieldTalkWindow[0] );
    GFL_SAFE_DELETE( p_swk->m_pFieldTalkWindow[1] );
    p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_WIN_OPEN );
  }
  
}


//-----------------------------------------------------------------------------
/**
 * @brief グローバルコール
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GlobalCall(AMX *amx, const cell * ptr)
{
  cell scrid = ptr[1];
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  pSys->CallChildScript( p_obj, scrid );
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief グローバルコール デバッグ用ワンタイム実行
 * コマンドの皮は残るが、製品版ではNopと化す仕様
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GlobalCallDebugOnetime(AMX *amx, const cell * ptr)
{
#if PM_DEBUG
  cell scrid = ptr[1];
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  pSys->DebugCallChildScriptOnetime( p_obj, scrid );
#endif
  return 0;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief グローバルコール時の子パラメータを取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::GlobalGetChildParam(AMX* amx, const cell * ptr)
{
  cell index = ptr[1];
  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  return p_obj->GetWork()->GetChildGlobalParam( index );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief グローバルコール時の子パラメータを設定
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::GlobalSetChildParam(AMX* amx, const cell * ptr)
{
  cell index = ptr[1];
  cell value = ptr[2];
  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  p_obj->GetWork()->SetChildGlobalParam( index, value );
  return 0;
}
/* ------------------------------------------------------------------------- */
/**
 * @brief グローバルコール時の親パラメータを取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::GlobalGetParentParam(AMX* amx, const cell * ptr)
{
  cell index = ptr[1];
  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  return p_obj->GetWork()->GetParentGlobalParam( index );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief グローバルコール時の親パラメータを設定
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::GlobalSetParentParam(AMX* amx, const cell * ptr)
{
  cell index = ptr[1];
  cell value = ptr[2];
  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  p_obj->GetWork()->SetParentGlobalParam( index, value );
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief 並列コール
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ParallelCall(AMX *amx, const cell * ptr)
{
  cell scrid = ptr[1];

  u32 param0 = ptr[2];
  u32 param1 = ptr[3];
  u32 param2 = ptr[4];
  u32 param3 = ptr[5];

  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  RegularObject* p_obj = ScriptObject::GetClass<RegularObject*>(amx);
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  return pSys->CallParallelScript( p_obj, scrid, p_heap, param0, param1, param2, param3 );
}

//-----------------------------------------------------------------------------
/**
 * @brief 並列コール、指定ハンドルの並列スクリプトが終了しているかチェック
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ParallelCallIsFinished(AMX *amx, const cell *ptr)
{
  s32 handle = ptr[1];
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  RegularObject* p_obj = ScriptObject::GetClass<RegularObject*>(amx);
  return (!p_obj->IsParallelObjectExists( handle ) );
}

//-----------------------------------------------------------------------------
/**
 * @brief 並列コール、全ての並列スクリプトが終了しているかチェック
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ParallelCallIsFinishedAll(AMX *amx, const cell *ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  RegularObject* p_obj = ScriptObject::GetClass<RegularObject*>(amx);
  return ( p_obj->GetParallelObjectCount() == 0 );
}

//------------------------------------------------------------------------------
/**
 * @func        MapChangeCore_
 * @brief       マップチェンジコア
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MapChangeCore_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();

  ZoneID  zoneId = ptr[1];
  f32     posX   = amx_ctof( ptr[2] );
  f32     posY   = amx_ctof( ptr[3] );
  f32     posZ   = amx_ctof( ptr[4] );
  u32     isForceEnd = ptr[5];
  u32     isFadeEnable = ptr[6];
  bool    isBGMKeep = GFL_BOOL_CAST(ptr[7]);
  bool    isPlaceNameEnable = GFL_BOOL_CAST(ptr[8]);
  bool    isFixedPos = GFL_BOOL_CAST(ptr[9]);
  bool    isUpdateEvTime = GFL_BOOL_CAST(ptr[10]);

  gfl2::math::Vector3 pos(posX,posY,posZ);

  EventMapJumpCore::EventMapJumpType type = EventMapJumpCore::EV_MAPJUMP_BLACK_FADE;
  if( isFadeEnable == false ){
    type = EventMapJumpCore::EV_MAPJUMP_NO_EFFECT;
  }

  if( isForceEnd ){
    EventMapJumpCore* p_event = EventMapJump::CreateMapJumpPos( pSys->GetGameManager(), zoneId, pos, type, isPlaceNameEnable );
    p_event->SetBGMKeepFlag(isBGMKeep);
    p_event->SetUpdateEvTime(isUpdateEvTime);
    
    pSys->EntryNextEvent(p_event);
    pSys->ForceEnd();
  }else{
    EventMapJumpCore* p_event = EventMapJump::CallMapJumpPos( pSys->GetGameManager(), zoneId, pos, type, isPlaceNameEnable );
    p_event->SetBGMKeepFlag(isBGMKeep);
    p_event->SetUpdateEvTime(isUpdateEvTime);
  }

  //プレイヤーのフリー移動記録
  {
    Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

    // NPCはチェック対象外なので、一旦チェックを全て落とす
    pFieldMoveModelManager->ClearCheckSafetyPositionForEventScript();

    MoveModel::FieldMoveModelPlayer* pPlayer = 
      static_cast<MoveModel::FieldMoveModelPlayer*>( pFieldMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

    // 主人公のみ、チェック対応
    if( pPlayer )
    {
      if (isFixedPos)
      {
        pPlayer->EnablePointMoveForEventScript();
      }
      else
      {
        pPlayer->EnableFreeMoveForEventScript();
      }
    }
  }


  pSys->Suspend();
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        MapChangeEscape_
 * @brief       脱出マップチェンジコア
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MapChangeEscape_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  EventMapJumpCore::EventMapJumpType type = EventMapJumpCore::EV_MAPJUMP_NO_EFFECT;
//  GameSys::GameEvent* p_event = 
  EventMapJump::CallMapJumpEscape( pSys->GetGameManager(), type );
  pSys->Suspend();
  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        WaitFunc_StaffList
* @brief       スタッフロールリスト表示完了待ち
*/
//------------------------------------------------------------------------------
bool FuncSetCommon::WaitFunc_StaffList(FieldScriptSystem* p_sys, u32 label, void* p_work)
{
  GameSys::GameManager  *p_gman = p_sys->GetGameManager();
  return p_gman->GetFieldmap()->GetFieldStaffroll()->IsAllRequestEnd();
}

//------------------------------------------------------------------------------
/**
 * @func        PlaceNameRequest
 * @brief       地名表示
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlaceNameRequest( AMX* amx, const cell* ptr )
{
  //Alias
  ScriptObject*         p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager* p_gman   = p_obj->GetGameManager();

  //Param
  bool force_f = GFL_BOOL_CAST(ptr[1]);

  //Request : PlaceName
  p_gman->GetFieldmap()->GetEffectManager()->RequestStartPlaceName( force_f );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        JoinFestaOpen
 * @brief       ジョインフェスタへ遷移する
 * @param       第1引数　城外か城内か（0:城外 1:城内）
 * @param       第2引数　遷移後の主人公X座標
 * @param       第3引数　遷移後の主人公Z座標
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::JoinFestaOpen( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();

  bool    isRoom = GFL_BOOL_CAST(ptr[1]);
  f32     posX   = amx_ctof( ptr[2] );
  f32     posZ   = amx_ctof( ptr[3] );

  gfl2::math::Vector3 pos(posX,0.0f,posZ);

  NetEvent::JoinFestaOpenEvent::StartEvent( pSys->GetGameManager(), true, isRoom, &pos, true );

  pSys->Suspend();
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        JoinFestaClose
 * @brief       ジョインフェスタから通常フィールドへ戻る
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::JoinFestaClose( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();

  NetEvent::JoinFestaCloseEvent::StartEvent( pSys->GetGameManager(), true );

  pSys->Suspend();
  return NULL;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief WaitFunc:キーウェイト
 */
/* ------------------------------------------------------------------------- */
static bool WaitFunc_KeyWait( FieldScriptSystem* p_sys, u32 label, void* p_work )
{
  GameSys::GameManager  *p_gman     = p_sys->GetGameManager();
  gfl2::ui::Button      *uiButton   = p_gman->GetUiDeviceManager()->GetButton(0);
  gfl2::ui::TouchPanel  *uiPanel    = p_gman->GetUiDeviceManager()->GetTouchPanel(0);
  gfl2::ui::VectorDevice  *uiStick  = p_gman->GetUiDeviceManager()->GetStick(0);

  u32 button = label;
  
  if( button == 0 )
  {
    if ( uiButton->IsTrigger(gfl2::ui::BUTTON_A | gfl2::ui::BUTTON_B | gfl2::ui::BUTTON_X | gfl2::ui::BUTTON_CROSS) ||
         uiPanel->IsTouchTrigger() ||
         uiStick->GetTrigger()
         )
    {
      if ( uiButton->IsTrigger(gfl2::ui::BUTTON_X) )  p_sys->SetLastKeyXMenuOpen( true );
      return true;
    }
  }else
  {
    if ( uiButton->IsTrigger(label) || uiPanel->IsTouchTrigger() )
    {
      return true;
    }
  }
  return false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief WaitFunc:スライドパッド
 */
/* ------------------------------------------------------------------------- */
static bool WaitFunc_SlidePadWait( FieldScriptSystem* p_sys, u32 label, void* p_work )
{
  GameSys::GameManager  *p_gman     = p_sys->GetGameManager();
//  gfl2::ui::Button      *uiButton   = p_gman->GetUiDeviceManager()->GetButton(0);
//  gfl2::ui::TouchPanel  *uiPanel    = p_gman->GetUiDeviceManager()->GetTouchPanel(0);
  gfl2::ui::VectorDevice  *uiStick  = p_gman->GetUiDeviceManager()->GetStick(0);

  return (uiStick->GetTrigger() != 0);
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ABキー待ち
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ABKeyWait_( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  pSys->SetWaitFunc( amx, WaitFunc_KeyWait, gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B, NULL );

  pSys->Suspend();

  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ラストキー待ち
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::LastKeyWait_( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  pSys->SetWaitFunc( amx, WaitFunc_KeyWait, 0, NULL );

  pSys->Suspend();

  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  スライドパッド待ち
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SlidePadWait_( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  pSys->SetWaitFunc( amx, WaitFunc_SlidePadWait, 0, NULL );

  pSys->Suspend();

  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  デバイス入力取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::InputKeyIsHold( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  gfl2::ui::Button        *uiButton = p_gman->GetUiDeviceManager()->GetButton(0);

  u32 keys = ptr[1];

  u32 win_keys = keys & (SCR_BUTTON_WIN_L2|SCR_BUTTON_WIN_R2);
  u32 ctr_keys = keys & (0xFFFFFFFF ^ (SCR_BUTTON_WIN_L2|SCR_BUTTON_WIN_R2));

  u32 count=0;

#if defined(GF_PLATFORM_WIN32)
  if( win_keys ){
    if( (win_keys&SCR_BUTTON_WIN_L2) && (gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::L2 )) ){
      ++count;
    }
    if( (win_keys&SCR_BUTTON_WIN_R2) && (gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::R2 )) ){
      ++count;
    }
  }
#endif
  if ( uiButton->IsHold(ctr_keys) ){
    ++count;
  }

  return count;
}

cell AMX_NATIVE_CALL FuncSetCommon::InputKeyIsTrigger( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  gfl2::ui::Button        *uiButton = p_gman->GetUiDeviceManager()->GetButton(0);

  u32 keys = ptr[1];

  u32 win_keys = keys & (SCR_BUTTON_WIN_L2|SCR_BUTTON_WIN_R2);
  u32 ctr_keys = keys & (0xFFFFFFFF ^ (SCR_BUTTON_WIN_L2|SCR_BUTTON_WIN_R2));

  u32 count=0;

#if defined(GF_PLATFORM_WIN32)
  if( win_keys ){
    if( (win_keys&SCR_BUTTON_WIN_L2) && (gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::L2 )) ){
      ++count;
    }
    if( (win_keys&SCR_BUTTON_WIN_R2) && (gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::R2 )) ){
      ++count;
    }
  }
#endif
  if ( uiButton->IsTrigger(ctr_keys) ){
    ++count;
  }

  return count;
}

cell AMX_NATIVE_CALL FuncSetCommon::InputKeyIsRelease( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  gfl2::ui::Button        *uiButton = p_gman->GetUiDeviceManager()->GetButton(0);

  u32 keys = ptr[1];

  u32 win_keys = keys & (SCR_BUTTON_WIN_L2|SCR_BUTTON_WIN_R2);
  u32 ctr_keys = keys & (0xFFFFFFFF ^ (SCR_BUTTON_WIN_L2|SCR_BUTTON_WIN_R2));

  u32 count=0;

#if defined(GF_PLATFORM_WIN32)
  if( win_keys ){
    if( (win_keys&SCR_BUTTON_WIN_L2) && (gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::L2 )) ){
      ++count;
    }
    if( (win_keys&SCR_BUTTON_WIN_R2) && (gfl2::system::InputDevice::Pad::IsPush( 0, gfl2::system::InputDevice::Button::R2 )) ){
      ++count;
    }
  }
#endif
  if ( uiButton->IsRelease(ctr_keys) ){
    ++count;
  }

  return count;
}


//------------------------------------------------------------------------------
/**
* @brief       ポケファインダー起動
*
* @param[in,out] AMX*          amx .
* @param[in]     const cell *  ptr ポインタ.
*
* @return      イベント結果(今は0固定)
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::CallPokeFinderEvent( AMX *amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  SystemSingletones       *pSwk     = pSys->GetSystemWork()->GetSingletones();
  gfl2::heap::HeapBase    *p_heap   = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

  GFL_ASSERT( !pSwk->m_pTemp );
  pSwk->m_pTemp = GFL_NEW(p_heap) Event::FINDER_RESULT;

  Event::EventPokeFinderForPlacement::SetupData data;
  data.spot_id = ptr[1];
  data.mons_no = ptr[2];
  data.out_res = reinterpret_cast<Event::FINDER_RESULT*>(pSwk->m_pTemp);
  Event::EventPokeFinderForPlacement *pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Event::EventPokeFinderForPlacement>( p_gman->GetGameEventManager() );
  pEvent->Initialize( data );

  // ウェイト
  pSys->Suspend();

  return 0;
}
//------------------------------------------------------------------------------
/**
* @brief       ポケファインダー結果取得
*
* @param[in,out] AMX*          amx .
* @param[in]     const cell *  ptr ポインタ.
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ResultPokeFinderEvent( AMX *amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  SystemSingletones       *pSwk     = pSys->GetSystemWork()->GetSingletones();
  Work                    *pWork    = p_obj->GetWork();

  u32 ret_wk = ptr[1];

  GFL_ASSERT( pSwk->m_pTemp );
  Event::FINDER_RESULT *result = reinterpret_cast<Event::FINDER_RESULT*>( pSwk->m_pTemp );
  pWork->SetTempWorkValue( ret_wk, *result );

  GFL_SAFE_DELETE( pSwk->m_pTemp );

  return 0;
}
//------------------------------------------------------------------------------
/**
* @brief       ポケファインダーバージョン取得
*
* @param[in,out] AMX*          amx .
* @param[in]     const cell *  ptr ポインタ.
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetPokeFinderVersion( AMX *amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();

  Savedata::PokeFinderSave *pSave   = p_gman->GetGameData()->GetPokeFinderSave();
  
  // バージョンはデータ的には0開始なので+1で数値を操作する
  return pSave->GetCameraVersion() +1;
}
//------------------------------------------------------------------------------
/**
* @brief       ポケファインダー総評価得点取得
*
* @param[in,out] AMX*          amx .
* @param[in]     const cell *  ptr ポインタ.
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetPokeFinderTotalScore( AMX *amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();

  Savedata::PokeFinderSave *pSave   = p_gman->GetGameData()->GetPokeFinderSave();
  
  return pSave->GetTotalValue();
}

//------------------------------------------------------------------------------
/**
 * @func        選択されたポケモンの情報を取得
 * @brief       GetSelect～系で手持ちやボックスからポケモンを取得するためのユーティリティ
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 * @note        GFL_NEWして返すのでDELETEする事
 * @note        複数の値を取得する場合はStartでNewしてGetで取得してEndでDelするような仕組みの方が良いかもしれない
 */
//------------------------------------------------------------------------------
pml::pokepara::CoreParam* GetSelectMonsParam(u32 select_pos, u32 select_index)
{
  //Alias
  GameSys::GameManager* p_gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  GameSys::GameData*    p_game_data = p_gman->GetGameData();
  pml::PokeParty*       p_party = p_game_data->GetPlayerParty();
  Savedata::BoxPokemon* p_box_pokemon = p_game_data->GetBoxPokemon();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT );

  pml::pokepara::CoreParam* pPoke = NULL;
  switch( select_pos )
  {
    //手持ち
    case App::Box::RETURN_TEMOTI:
    {
      pPoke = p_party->GetMemberPointer( select_index );
      if( pPoke == NULL )
      {
        GFL_ASSERT( 0 );
        break;
      }
      pPoke = GFL_NEW( p_heap ) pml::pokepara::CoreParam( p_heap, *pPoke );
      break;
    }
    default:  //BOXを選んだ 0～（今作のボックス上限数）までがBOXになります
    {
      if( select_pos >= Savedata::BoxPokemon::TRAY_MAX )
      {
        GFL_ASSERT(0);
        break;
      }
      if( select_index >= Savedata::BoxPokemon::TRAY_POKE_MAX )
      {
        GFL_ASSERT(0);
        break;
      }
      pPoke = p_box_pokemon->GetPokemon( select_pos, select_index, p_heap );
      GFL_ASSERT( pPoke );
      break;
    }
  }

  return pPoke;
}

//------------------------------------------------------------------------------
/**
 * @func        選んだポケモンの情報の取得：タマゴか否か
 * @brief       エクセルの読み込み。読み込み待ちと破棄処理も呼ぶ事
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetSelectMonsIsEgg( AMX * amx, const cell * ptr )
{
  //Param
  u32 select_pos  = ptr[1];
  u32 select_index  = ptr[2];

  //ポケモンの情報取得
  pml::pokepara::CoreParam* pPoke = GetSelectMonsParam(select_pos, select_index);

  //値の取得
  u32 result = (u32)pPoke->IsEgg( pml::pokepara::CHECK_BOTH_EGG );

  //後処理
  GFL_SAFE_DELETE( pPoke );

  return result;
}

//------------------------------------------------------------------------------
/**
 * @func        選んだポケモンの情報の取得：種類
 * @brief       エクセルの読み込み。読み込み待ちと破棄処理も呼ぶ事
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetSelectMonsNo( AMX * amx, const cell * ptr )
{
  //Param
  u32 select_pos  = ptr[1];
  u32 select_index  = ptr[2];

  //ポケモンの情報取得
  pml::pokepara::CoreParam* pPoke = GetSelectMonsParam(select_pos, select_index);

  //値の取得
  u32 result = (u32)pPoke->GetMonsNo();

  //後処理
  GFL_SAFE_DELETE( pPoke );

  return result;
}

//------------------------------------------------------------------------------
/**
 * @func        選んだポケモンの情報の取得：性別
 * @brief       エクセルの読み込み。読み込み待ちと破棄処理も呼ぶ事
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetSelectMonsSex( AMX * amx, const cell * ptr )
{
  //Param
  u32 select_pos  = ptr[1];
  u32 select_index  = ptr[2];

  //ポケモンの情報取得
  pml::pokepara::CoreParam* pPoke = GetSelectMonsParam(select_pos, select_index);

  //値の取得
  u32 result = (u32)pPoke->GetSex();

  //後処理
  GFL_SAFE_DELETE( pPoke );

  return result;
}

//------------------------------------------------------------------------------
/**
 * @func        選んだポケモンの情報の取得：性格
 * @brief       エクセルの読み込み。読み込み待ちと破棄処理も呼ぶ事
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetSelectMonsSeikaku( AMX * amx, const cell * ptr )
{
  //Param
  u32 select_pos  = ptr[1];
  u32 select_index  = ptr[2];

  //ポケモンの情報取得
  pml::pokepara::CoreParam* pPoke = GetSelectMonsParam(select_pos, select_index);

  //値の取得
  u32 result = (u32)pPoke->GetSeikaku();

  //後処理
  GFL_SAFE_DELETE( pPoke );

  return result;
}

//------------------------------------------------------------------------------
/**
 * @func        選んだポケモンの情報の取得：地名（捕獲した場所＆タマゴから産まれた場所）
 * @brief       エクセルの読み込み。読み込み待ちと破棄処理も呼ぶ事
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetSelectMonsPlace( AMX * amx, const cell * ptr )
{
  //Param
  u32 select_pos  = ptr[1];
  u32 select_index  = ptr[2];

  //ポケモンの情報取得
  pml::pokepara::CoreParam* pPoke = GetSelectMonsParam(select_pos, select_index);

  //値の取得
  u32 result = (u32)pPoke->GetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE);

  //後処理
  GFL_SAFE_DELETE( pPoke );

  return result;
}

//------------------------------------------------------------------------------
/**
 * @func        選んだポケモンの情報の取得：捕獲したかタマゴから産まれたか（trueなら捕獲、falseならタマゴ）
 * @brief       エクセルの読み込み。読み込み待ちと破棄処理も呼ぶ事
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetSelectMonsIsCapture( AMX * amx, const cell * ptr )
{
  //Param
  u32 select_pos  = ptr[1];
  u32 select_index  = ptr[2];

  //ポケモンの情報取得
  pml::pokepara::CoreParam* pPoke = GetSelectMonsParam(select_pos, select_index);

  //値の取得
  u32 is_from_egg = (u32)pPoke->GetMemories(pml::pokepara::MEMORIES_EGG_TAKEN_PLACE);

  //後処理
  GFL_SAFE_DELETE( pPoke );

  return !is_from_egg;
}

//------------------------------------------------------------------------------
/**
 * @func        選んだポケモンの情報の取得：技の数
 * @brief       エクセルの読み込み。読み込み待ちと破棄処理も呼ぶ事
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetSelectMonsWazaNum( AMX * amx, const cell * ptr )
{
  //Param
  u32 select_pos  = ptr[1];
  u32 select_index  = ptr[2];

  //ポケモンの情報取得
  pml::pokepara::CoreParam* pPoke = GetSelectMonsParam(select_pos, select_index);

  //値の取得
  u32 result = (u32)pPoke->GetWazaCount();

  //後処理
  GFL_SAFE_DELETE( pPoke );

  return result;
}

//------------------------------------------------------------------------------
/**
 * @func        選んだポケモンの情報の取得：技
 * @brief       エクセルの読み込み。読み込み待ちと破棄処理も呼ぶ事
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetSelectMonsWaza( AMX * amx, const cell * ptr )
{
  //Param
  u32 select_pos  = ptr[1];
  u32 select_index  = ptr[2];
  u32 slot_index  = ptr[3];

  //ポケモンの情報取得
  pml::pokepara::CoreParam* pPoke = GetSelectMonsParam(select_pos, select_index);

  //値の取得
  u32 result = (u32)pPoke->GetWazaNo(slot_index);

  //後処理
  GFL_SAFE_DELETE( pPoke );

  return result;
}

//------------------------------------------------------------------------------
/**
 * @func        IDくじ：今日のナンバーの取得
 * @brief       日付に応じた乱数の取得
 * @author      ohhira_syo
 * @date        2015/12/02(水) 14:28:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::IDPresentGetID( AMX* amx, const cell * ptr )
{
  //Const
  const u32 MAX = 99999;

  //乱数を返す
  //GFNMCat[1774]:IDくじの乱数をRandomGroupからPublicRandに変更。
  //- 再現性を意図的になくすため
  u32 result = System::GflUse::GetPublicRand(MAX+1);

  //デバッグ機能
#if PM_DEBUG
  //当選番号を外部から設定されていたらそっちを採用
  if(Field::Debug::DebugTypes::s_IdPresentForceNumber){
    result =
      (Field::Debug::DebugTypes::s_IdPresentForceNumber1 * 1) + 
      (Field::Debug::DebugTypes::s_IdPresentForceNumber2 * 10) + 
      (Field::Debug::DebugTypes::s_IdPresentForceNumber3 * 100) + 
      (Field::Debug::DebugTypes::s_IdPresentForceNumber4 * 1000) + 
      (Field::Debug::DebugTypes::s_IdPresentForceNumber5 * 10000);
  }
#endif

  return result;
}

/*====================================================*/
/*  IDPresentGetParam用の関数：一致桁数を計算         */
/*  val1, val2   比較する数値                         */
/*  return:  一致桁数（0～5）                         */
/*====================================================*/
static u8 check_hit_keta(u32 val1, u32 val2)
{
  u8  i, hit;

  hit = 0;

  for(i = 0; i < 5; i++)
  {
    if((val1%10) != (val2%10)){ break; }
    val1 /= 10;
    val2 /= 10;
    hit++;
  }

  //デバッグ機能
#if PM_DEBUG
  //ヒット数を外部から設定されていたらそっちを採用
  //- 負の数であれば通常通り
  if(0 <= Field::Debug::DebugTypes::s_IdPresentForceHitNum){
    hit = Field::Debug::DebugTypes::s_IdPresentForceHitNum;
  }
#endif

  return hit;
}
//------------------------------------------------------------------------------
/**
 * @func        IDくじ：ナンバーにヒットしたポケモンの検索など
 * @brief       「ヒットしたか」「最大ヒット数」「PCでヒットしたか」を調べる
 * @author      ohhira_syo
 * @date        2015/12/02(水) 14:28:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::IDPresentGetParam( AMX* amx, const cell * ptr )
{
  //Param
  u32 id = ptr[1];
  int wset_buf_idx = ptr[2];
  cell* buf; amx_GetAddr( amx, ptr[3], &buf );

  //Alias
  ScriptObject*         p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager* p_gman   = p_obj->GetGameManager();
  GameSys::GameData* p_gdata  = p_gman->GetGameData();
  Savedata::ResortSave* p_resort = p_gdata->GetSaveData()->GetResort();
  Savedata::BoxPokemon* p_boxpoke = p_gdata->GetBoxPokemon();
//  Savedata::BattleBox*  p_battle_box = p_gdata->GetBattleBox();
  FieldScriptSystem*    pSys = FieldScriptSystem::GetInstance();
  print::WordSet*       wset = pSys->GetSystemWork()->GetWordSet();
  gfl2::heap::HeapBase*  p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT);


  //最大ヒット数
  u32 max_num = 0;

  //ポケモンの情報取得のための入れ物
  pml::pokepara::CoreParam* p_cp = GFL_NEW( p_heap ) pml::pokepara::CoreParam( p_heap );

  // てもち検索
  {
    pml::PokeParty* p_party = p_gman->GetGameData()->GetPlayerParty();
    u16 cnt = p_party->GetMemberCount();

    for ( u32 i = 0; i < cnt; ++i )
    {
      pml::pokepara::PokemonParam* p_pp = p_party->GetMemberPointer(i);

      // タマゴチェック
      if ( p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false )
      {
        u32 poke_id; PokeTool::GetDrawID(p_pp, &poke_id);
        u8 hit_num = check_hit_keta( id, poke_id );

        if ( hit_num > 0 && max_num < hit_num )
        {
          max_num = hit_num;

          //ラベル用にポケモンのニックネームを設定しておく
          wset->RegisterPokeNickName( wset_buf_idx, p_pp );
        }
      }
    }
  }

  // PC検索
  bool is_hit_pc = false;
  {
    for ( u32 tray_no = 0; tray_no < Savedata::BoxPokemon::TRAY_MAX; ++tray_no )
    {
      for ( u32 pos = 0; pos < Savedata::BoxPokemon::TRAY_POKE_MAX; ++pos )
      {
        // 存在チェック
        if ( p_boxpoke->CheckPokemon( tray_no, pos, p_cp ) == false )
        {
          continue;
        }

        //GFL_ASSERT( pos < savedata::BoxPokemon::TRAY_POKE_MAX );
        p_boxpoke->GetPokemon( p_cp, tray_no, pos );

        // タマゴチェック
        if ( p_cp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
        {
          continue;
        }

        u32 poke_id; PokeTool::GetDrawID(p_cp, &poke_id);
        u8 hit_num = check_hit_keta( id, poke_id );

        if ( hit_num > 0 && max_num < hit_num )
        {
          max_num = hit_num;
          is_hit_pc = true;

          //ラベル用にポケモンのニックネームを設定しておく
          wset->RegisterPokeNickName( wset_buf_idx, p_cp );
        }
      }
    }
  }
  //ポケリゾート検索
  //- ヒットした場合、PC（ボックス）検索と同じ扱いにする
  {
    //BOXから実際に移動させてるのは「おんせん」と「うんどう」のみ

    //おんせん
    for(int i = 0; i < Savedata::ResortSave::STOCKPOKEMON_MAX; ++i){
      p_resort->GetHotSpaPokemon(i, p_cp);

      //居なければスキップ
      if(p_cp->GetMonsNo() == MONSNO_NULL){
        continue;
      }

      //一応タマゴチェック
      if ( p_cp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ){
        continue;
      }

      u32 poke_id; PokeTool::GetDrawID(p_cp, &poke_id);
      u8 hit_num = check_hit_keta( id, poke_id );

      if ( hit_num > 0 && max_num < hit_num )
      {
        max_num = hit_num;
        is_hit_pc = true;

        //ラベル用にポケモンのニックネームを設定しておく
        wset->RegisterPokeNickName( wset_buf_idx, p_cp );
      }
    }

    //うんどう
    for(int i = 0; i < Savedata::ResortSave::STOCKPOKEMON_MAX; ++i){
      p_resort->GetGimPokemon(i, p_cp);

      //居なければスキップ
      if(p_cp->GetMonsNo() == MONSNO_NULL){
        continue;
      }

      //一応タマゴチェック
      if ( p_cp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ){
        continue;
      }

      u32 poke_id; PokeTool::GetDrawID(p_cp, &poke_id);
      u8 hit_num = check_hit_keta( id, poke_id );

      if ( hit_num > 0 && max_num < hit_num )
      {
        max_num = hit_num;
        is_hit_pc = true;

        //ラベル用にポケモンのニックネームを設定しておく
        wset->RegisterPokeNickName( wset_buf_idx, p_cp );
      }
    }
  }
/*
  // バトルボックス検索
  {
    for( u32 i = 0;i < savedata::BattleBox::TRAY_POKE_MAX; ++i ){
      if( p_battle_box->CheckPokemon( i, p_cp ) == false ){
        continue;
      }
      p_battle_box->GetPokemon( p_cp, i );

      //バトルボックスにタマゴはいないハズだが一応タマゴチェック
      if ( p_cp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ) {
        continue;
      }

      u32 poke_id; PokeTool::GetDrawID(p_cp, &poke_id);
      u8 hit_num = check_hit_keta( id, poke_id );

      if ( hit_num > 0 && max_num < hit_num )
      {
        max_num = hit_num;
        is_hit_pc = true;

        //ラベル用にポケモンのニックネームを設定しておく
        wset->RegisterPokeNickName( wset_buf_idx, p_cp );
      }
    }
  }
*/
  //後処理
  GFL_SAFE_DELETE( p_cp );

  //スクリプトに渡す
  buf[0] = ( max_num > 0 ); // IsIDMatch
  buf[1] = max_num;   // MatchNum
  buf[2] = is_hit_pc; // IsPC(バトルボックス含む)

  return NULL;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief BgPartsのデストラクタ
 *
 * @param p_gman ゲームマネージャへのポインタ
 *
 * @return true:終了, false:継続
 */
/* ------------------------------------------------------------------------- */
bool FuncSetCommon::SCREND_CheckEndBgParts( GameSys::GameManager* p_gman, s32* seq )
{
  FieldScriptSystem     *p_sys          = p_gman->GetFieldScriptSystem(); 
  SystemSingletones     *p_swk          = p_sys->GetSystemWork()->GetSingletones();
  Fieldmap              *pFieldMap     = p_gman->GetFieldmap();

  if ( pFieldMap == NULL )
  {
    //フィールドがないとき 記憶のみ解除
    p_sys->ClearBgPartsParam();
    p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_BG_PARTS );
    return true;
  }

  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  Terrain::TerrainBgPartsManager* pBgPartsManager = pTerrainManager->GetBgPartsManager();
  
  switch( *seq ){
  case 0:
    pBgPartsManager->UnLoadBgPartsAll();
    p_sys->ClearBgPartsParam();
    (*seq)++;
    return false;
  case 1:
    if( pBgPartsManager->CountBgParts() > 0 ){
      return false;
    }    
    p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_BG_PARTS );
    (*seq)++;
    break;
  }


  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        BgPartsSetLoad
 * @brief       BgPartsSet読み込み関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:24
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BgPartsSetLoad( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();

  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  Terrain::TerrainBgPartsManager* pBgPartsManager = pTerrainManager->GetBgPartsManager();
  
  s32     BgPartsSetID     = ptr[1];
  

  gfl2::heap::HeapBase *eventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  pBgPartsManager->LoadBgPartsAsync( BgPartsSetID, eventDeviceHeap, System::GflUse::GetEventDeviceAllocator() );

  // 記憶
  p_sys->SetBgPartsLoad( BgPartsSetID, true );
  return 0;
}

cell AMX_NATIVE_CALL FuncSetCommon::BgPartsSetLoadTrial6Boss( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();

  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  Terrain::TerrainBgPartsManager* pBgPartsManager = pTerrainManager->GetBgPartsManager();
  
  s32     BgPartsSetID     = ptr[1];
  

  gfl2::heap::HeapBase *fieldExtDeviceHeap = p_gman->GetGameData()->GetFieldArea()->GetFieldExtHeap();
  System::nijiAllocator* fieldExtDeviceHeapAllocator = p_gman->GetGameData()->GetFieldArea()->GetFieldExtHeapAllocator();
  GFL_ASSERT_STOP( fieldExtDeviceHeap );

  pBgPartsManager->LoadBgPartsAsync( BgPartsSetID, fieldExtDeviceHeap, fieldExtDeviceHeapAllocator );

  // 記憶
  p_sys->SetBgPartsLoad( BgPartsSetID, false );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        BgPartsSetDel
 * @brief       BgPartsSetDel関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:26
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BgPartsSetDel( AMX *amx, const cell *ptr )
{
  ScriptObject              *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager      *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem         *p_sys    = p_gman->GetFieldScriptSystem();
  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  Terrain::TerrainBgPartsManager* pBgPartsManager = pTerrainManager->GetBgPartsManager();
  
  s32         BgPartsSetID = ptr[1];

  //表示強制Off
  pBgPartsManager->SetVisible( BgPartsSetID, false );
  p_sys->SetBgPartsVisible( BgPartsSetID, false );

  pBgPartsManager->UnLoadBgParts( BgPartsSetID );

  // 記憶
  p_sys->SetBgPartsUnLoad( BgPartsSetID );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        BgPartsDisp
 * @brief       BgPartsDisp関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:40
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BgPartsDisp( AMX *amx, const cell *ptr )
{
  ScriptObject              *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager      *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem         *p_sys    = p_gman->GetFieldScriptSystem();
  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  Terrain::TerrainBgPartsManager* pBgPartsManager = pTerrainManager->GetBgPartsManager();

  s32         BgPartsID = ptr[1];
  bool        disp = GFL_BOOL_CAST(ptr[2]);
  
  pBgPartsManager->SetVisible( BgPartsID, disp );

  // 記憶
  p_sys->SetBgPartsVisible( BgPartsID, disp );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        BgPartsSetIsLoaded
 * @brief       BgPartsSetIs読み込みed関数.
 * @author      N.Takeda
 * @date        2015/09/08(火) 15:23:22
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BgPartsSetIsLoaded( AMX *amx, const cell *ptr )
{
  ScriptObject              *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager      *p_gman   = p_obj->GetGameManager();
  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  Terrain::TerrainBgPartsManager* pBgPartsManager = pTerrainManager->GetBgPartsManager();
  
  return !pBgPartsManager->IsLoading();
}

//------------------------------------------------------------------------------
/**
 * @func        BgPartsSetIsDeleted
 * @brief       BgPartsSetIs読み込みed関数.
 * @author      N.Takeda
 * @date        2015/09/08(火) 15:23:22
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BgPartsSetIsDeleted( AMX *amx, const cell *ptr )
{
  ScriptObject              *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager      *p_gman   = p_obj->GetGameManager();
  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  Terrain::TerrainBgPartsManager* pBgPartsManager = pTerrainManager->GetBgPartsManager();
  
  return (pBgPartsManager->CountBgParts() == 0);
}



//------------------------------------------------------------------------------
/**
 * @func        SCREND_CheckEndMapBlockCalcPos
 * @brief       SCREND_CheckEndMapBlockCalcPos関数.
 * @author      Miyuki Iwasawa
 *
 * @param[in,out] GameSys::GameManager* p_gman  .
 *
 * @return      bool  TRUE:成 / FALSE:否
 */
//------------------------------------------------------------------------------
bool FuncSetCommon::SCREND_CheckEndMapBlockCalcPos(  GameSys::GameManager* p_gman, s32* seq )
{
  FieldScriptSystem *p_sys     = p_gman->GetFieldScriptSystem();
  Fieldmap          *pFieldmap = p_gman->GetFieldmap();

  if( !pFieldmap ){
    p_sys->ClearTerrainBlockControlPosition();
    p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_MAPBLOCK_CALC_POS ); 
    return true;
  }
  
  Terrain::TerrainManager*  pTerrainManager = pFieldmap->GetTerrainManager();

  switch( *seq ){
  case 0:
    pTerrainManager->ClearExternalBlockControlPoint();
    (*seq)++;
    return false;
  case 1:
    if( pTerrainManager->IsBlockLodChanging() ){
      return false; //LODの再読み込みが完了するまで待つ
    }
    p_sys->ClearTerrainBlockControlPosition();
    p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_MAPBLOCK_CALC_POS ); 
    (*seq)++;
    break;
  default:
    break;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        MapBlockCalcPosSet
 * @brief       MapBlockCalcPosSet関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:41
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 *
 * @com 相対座標対応コマンド
 *
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MapBlockCalcPosSet( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  
  f32         posX   = amx_ctof( ptr[1] );
  f32         posY   = amx_ctof( ptr[2] );
  f32         posZ   = amx_ctof( ptr[3] );
  
  gfl2::math::Vector3 pos = p_sys->CalcEventPositionWorld( posX, posY, posZ );
  pTerrainManager->SetExternalBlockControlPointByPosition( pos );

  // 地形が破棄されても継続されるように記憶する
  p_sys->SetTerrainBlockControlPosition( pos );
  p_sys->GetSystemWork()->OnCheckBit( SCR_END_CHECK_BIT_MAPBLOCK_CALC_POS );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        MapBlockCalcPosClear
 * @brief       MapBlockCalcPosClear関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:42
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MapBlockCalcPosClear( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();

  pTerrainManager->ClearExternalBlockControlPoint();
  p_sys->ClearTerrainBlockControlPosition();
  p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_MAPBLOCK_CALC_POS );

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        MapBlockIsLoading
 * @brief       MapBlockIs読み込みing関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:43
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MapBlockIsLoading( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  
  return pTerrainManager->IsBlockLodChanging();
}

//------------------------------------------------------------------------------
/**
 * @brief       Middleモデルの表示ライン制限を設定
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MapBlockSetMiddleModelDrawLineLimit( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  s32         limit = ptr[1];
  
  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  
  pTerrainManager->SetMiddleModelDrawLineLimit( limit );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       Middleモデルの表示ライン制限をクリア(=全表示)
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MapBlockClearMiddleModelDrawLineLimit( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  
  pTerrainManager->ClearMiddleModelDrawLineLimit();

  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief  カメラ接触でメッシュの非表示にする機能、再有効化ケア
 *
 * @param p_gman ゲームマネージャへのポインタ
 *
 * @return true:終了, false:継続
 */
/* ------------------------------------------------------------------------- */
bool FuncSetCommon::SCREND_CheckEndMapBlockHideMeshSystemOn( GameSys::GameManager* p_gman, s32* seq )
{
  FieldScriptSystem     *p_sys          = p_gman->GetFieldScriptSystem(); 
  SystemSingletones     *p_swk          = p_sys->GetSystemWork()->GetSingletones();
  Fieldmap              *pFieldMap     = p_gman->GetFieldmap();

  if ( pFieldMap == NULL )
  {
    return true;
  }

  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();
  pTerrainManager->HideMeshSystemOn( true );
  p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_HIDE_MESH_SYSTEM );
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief       カメラ接触でメッシュの非表示にする機能を有効化
 * @fix GFMcat[4831]イベント中に透過メッシュが不要に消える、対処。透過機能をOFFする機能を追加。スクリプトに公開
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MapBlockHideMeshSystemOn( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();

  b32         oneTime = ptr[1];
  pTerrainManager->HideMeshSystemOn( oneTime );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       カメラ接触でメッシュの非表示にする機能を無効化
 * @fix GFMcat[4831]イベント中に透過メッシュが不要に消える、対処。透過機能をOFFする機能を追加。スクリプトに公開
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MapBlockHideMeshSystemOff( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  Fieldmap*                 pFieldMap       = p_gman->GetFieldmap();
  Terrain::TerrainManager*  pTerrainManager = pFieldMap->GetTerrainManager();

  b32         oneTime = ptr[1];
  pTerrainManager->HideMeshSystemOff( oneTime );

  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        FieldItemUnregist
 * @brief       アイテム登録解除
 * @author      H.Ikawa
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::FieldItemUnregist( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::FieldResident* pFieldResident = p_gman->GetFieldResident();
  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  u16         eventId = ptr[1];

  // @fix GFNMCat[957] ドロップアイテムのEVENTIDがスクリプト動的生成NPCのEVENTIDとかぶる
  if( (eventId >= FieldItemManager::DROP_ITEM_EVENT_ID_OFFSET) && (eventId < FieldItemManager::DROP_ITEM_EVENT_ID_OFFSET + FieldItemManager::MAX_DROP_ITEM) )
  {
    pPlacementDataManager->UnRegistDropItem( eventId ); // 内部で配置モデルインスタンスの破棄、配置モデルアクターの破棄を行います。
  }
  else
  {
    pPlacementDataManager->UnRegistZoneEventIDItem( eventId );
    pPlacementDataManager->UnRegistZoneEventIDStaticModel( eventId );
  }
  return true;
}


//------------------------------------------------------------------------------
/**
 * @func        FieldItemTerminate
 * @brief       アイテム削除
 * @author      H.Ikawa
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::FieldItemTerminate( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::FieldResident* pFieldResident = p_gman->GetFieldResident();
  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  u16         eventId = ptr[1];

  // @fix GFNMCat[957] ドロップアイテムのEVENTIDがスクリプト動的生成NPCのEVENTIDとかぶる
  if( (eventId >= FieldItemManager::DROP_ITEM_EVENT_ID_OFFSET) && (eventId < FieldItemManager::DROP_ITEM_EVENT_ID_OFFSET + FieldItemManager::MAX_DROP_ITEM) )
  {
    pPlacementDataManager->DeleteDropItem( eventId );
    p_gman->GetGameData()->GetFieldItemManager()->TerminateDropItemData( eventId );
  }
  else
  {
    // ItemはUnRegistZoneItemのみでOK
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        FieldItemIsHidden
 * @brief       隠しアイテムがチェック
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::FieldItemIsHidden( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::FieldResident* pFieldResident = p_gman->GetFieldResident();
  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  u16         eventId = ptr[1];

  // 配置アイテムのサーチ
  FieldItemActor* pItem =  reinterpret_cast<FieldItemActor*>(pPlacementDataManager->SearchZoneItem( eventId ));
  if( pItem )
  {
    return FieldItemAccessor::IsHiddenShapeType( pItem->GetData()->shapeType );
  }
  // 配置アイテム以外は、見た目ありと判定する。
  else
  {
    return false;
  }
}

cell AMX_NATIVE_CALL FuncSetCommon::FieldBerrySpotUnregist( AMX *amx, const cell *ptr )
{
  Field::FieldResident* pFieldResident = ScriptObject::GetClass<ScriptObject*>( amx )->GetGameManager()->GetFieldResident();
  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  u16 berrySpotID = ptr[1];

  pPlacementDataManager->UnRegisterZoneBerryPoint(berrySpotID);
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        KeyInputTry_
 * @brief       KeyInputTry_関数.
 * @author      N.Takeda
 * @date        2015/10/30(金) 17:07:08
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::KeyInputTry_(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  gfl2::ui::TouchPanel    *uiPanel    = p_gman->GetUiDeviceManager()->GetTouchPanel(0);
  gfl2::ui::Button        *uiButton = p_gman->GetUiDeviceManager()->GetButton(0);
  
  s32   mode = ptr[1];
  u32   button = 0;

  switch( mode )
  {
  case 0:
  default:
    button = gfl2::ui::BUTTON_A;
    break;
  }
  
  if ( uiButton->IsTrigger(button) || uiPanel->IsTouchTrigger() ){
    return 1;
  }
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        FixAlolaOnlyFakeTime
 * @brief       FixAlolaOnlyFakeTime関数.
 * @author      N.Takeda
 * @date        2015/11/10(火) 17:37:02
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::FixAlolaOnlyFakeTime(AMX * amx, const cell * ptr )
{
  s32 hour    = ptr[1];
  s32 minute  = ptr[2];
  
  GameSys::AlolaTime  cAlolaTime;
  cAlolaTime.Initialize();
  cAlolaTime.FixAlolaOnlyFakeTime( hour, minute );
  
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        ClearAlolaOnlyFakeTime
 * @brief       AlolaOnlyFakeTimeのクリア.
 * @author      N.Takeda
 * @date        2015/11/10(火) 17:37:06
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ClearAlolaOnlyFakeTime(AMX * amx, const cell * ptr )
{
  GameSys::AlolaTime  cAlolaTime;
  
  cAlolaTime.Initialize();
  cAlolaTime.ClearAlolaOnlyFakeTime();
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        IsAlolaTimeReverseSkillUse
 * @brief       AlolaTimeReverseスキルUseの可否を得る.
 * @author      N.Takeda
 * @date        2015/11/10(火) 17:37:04
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::IsAlolaTimeReverseSkillUse(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  const GameSys::GameData* cp_gdata = p_gman->GetGameData();
  
  MonsNo  monsNo = static_cast<MonsNo>(ptr[1]);
  
  return GameSys::AlolaTime::IsAlolaTimeJumpOtherWorldSkillUse( monsNo, cp_gdata );
}

//------------------------------------------------------------------------------
/**
 * @func        RotomEventSet
 * @brief       RotomEventのセット.
 * @author      N.Takeda
 * @date        2015/11/12(木) 13:29:02
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::RotomEventSet(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32 msg_id = ptr[1];
  
  Field::SubScreen::FieldSubScreenProcManager *pp = p_gman->GetFieldmap()->GetFieldSubScreenManager();
  if( pp )  pp->SetRotomEvent( msg_id );
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        IsRotomEvent
 * @brief       RotomEventの可否を得る.
 * @author      N.Takeda
 * @date        2015/11/12(木) 13:29:04
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::IsRotomEvent(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  Field::SubScreen::FieldSubScreenProcManager *pp = p_gman->GetFieldmap()->GetFieldSubScreenManager();
  
  return ( pp ) ? pp->IsRotomEvent() : false;
}

//------------------------------------------------------------------------------
/**
* @func        RotomPowerStart
* @brief       ロトムパワー効果発動（カウント開始）
* @author      munakata_kai
* @date        2017/1/31
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::RotomPowerStart(AMX * amx, const cell * ptr)
{
  ScriptObject                         *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager                 *p_gman = p_obj->GetGameManager();
  Field::RotomPower::RotomPowerManager *p_rotom = p_gman->GetRotomPowerManager();

  u32 item_no = ptr[1];
  GFL_ASSERT((static_cast<s32>(item_no)-static_cast<s32>(ITEM_TAMAGOHUKAPON)) >= 0);
  GFL_ASSERT((static_cast<s32>(item_no)-static_cast<s32>(ITEM_TAMAGOHUKAPON)) < Field::RotomPower::ROTOM_POWER_ID_MAX);
  u32 rotom_power_id = Field::RotomPower::ROTOM_POWER_ID_EGG_INC + (item_no - ITEM_TAMAGOHUKAPON);

  p_rotom->SetRotomPower(static_cast<Field::RotomPower::ROTOM_POWER_ID>(rotom_power_id));
  p_rotom->Start();

  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        RotomPowerClear
* @brief       ロトムパワー効果削除（発動中のみ削除）
* @author      munakata_kai
* @date        2017/4/20
* @fix MMCat[173] ロトムパワー使用中に他のロトムパワーを使用した後、効果発動時のメッセージを送るまで古いロトムパワーのゲージが表示されたままになっている
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::RotomPowerClear(AMX * amx, const cell * ptr)
{
  ScriptObject                         *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager                 *p_gman = p_obj->GetGameManager();
  Field::RotomPower::RotomPowerManager *p_rotom = p_gman->GetRotomPowerManager();

  // 発動中の場合
  if (p_rotom->IsRotomPower())
  {
    p_rotom->ClearRotomPower();
    p_rotom->ClearEndRotomPowerID();
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
* @brief       ロトム親密度取得
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::RotomGetRank(AMX * amx, const cell * ptr)
{
  ScriptObject                         *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager                 *p_gman = p_obj->GetGameManager();
  GameSys::GameData                    *p_gdata = p_gman->GetGameData();

  STATIC_ASSERT( Field::FieldScript::SCR_ROTOM_RANK_MAX == Savedata::FieldMenu::ROTOM_RANK_MAX );

  Field::EventWork* evwork = p_gdata->GetEventWork();
  return p_gdata->GetFieldMenu()->GetRotomRank(App::Tool::RotomFriendship::GetDataIndex(evwork));
}

//------------------------------------------------------------------------------
/**
 * @func        GetTargetActorEventId
 * @brief       最後に話しかけた ActorId を得る.
 * @author      N.Takeda
 * @date        2015/11/12(木) 22:33:13
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetTargetActorEventId(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  Work                    *p_script_work = p_obj->GetWork();
  
  u32 actorID = p_script_work->GetTargetActorID();

  if( actorID == Work::TARGET_ACTOR_ID_NONE ){
    SCRIPT_ASSERT_MSG( 0, "話しかけ起動では無いのに話しかけ対象Actorを取得しようとしました\n");
    //無効値はイベントIDの無効値0なので、そのまま返しても問題ない
  }
  
  return p_script_work->GetTargetActorID();
}


//------------------------------------------------------------------------------
/**
 * @func        CallMapJumpOtherWorldTime
 * @brief       CallMapJumpOtherWorldTime関数.
 * @author      N.Takeda
 * @date        2015/11/27(金) 16:23:31
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::CallMapJumpOtherWorldTime(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32   zoneId = ptr[1];
  f32   posX   = amx_ctof( ptr[2] );
  f32   posY   = amx_ctof( ptr[3] );
  f32   posZ   = amx_ctof( ptr[4] );
  GameSys::AlolaTime::AlolaOtherWorldType   type = static_cast<GameSys::AlolaTime::AlolaOtherWorldType>(ptr[5]);
  bool  isCall = GFL_BOOL_CAST(ptr[6]);
  
  gfl2::math::Vector3 pos( posX, posY, posZ );
  if( isCall ){
    EventMapJump::CallMapJumpOtherWorldTime( p_gman, zoneId, pos, type );
  }else{

    EventMapJumpCore* p_event = EventMapJump::CreateMapJumpOtherWorldTime( 
      p_gman, zoneId, pos, type, 
      EventMapJumpCore::EV_MAPJUMP_NO_EFFECT,   // @fix GFNMCat[1014]
      true, false );
    p_sys->EntryNextEvent(p_event);
    p_sys->ForceEnd();
  }
  p_sys->Suspend();
  
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        CheckAlolaOtherWorld
 * @brief       AlolaOtherWorldのチェック.
 * @author      N.Takeda
 * @date        2015/11/27(金) 16:23:33
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::CheckAlolaOtherWorld(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  return GameSys::AlolaTime::CheckAlolaOtherWorld( p_gman->GetGameData() );
}


//------------------------------------------------------------------------------
/**
 * @func        CallEventPokemonRide
 * @brief       ライドに乗るイベントを起動する。
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::CallEventPokemonRide(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  u32 pokemonID = ptr[1];

  if( pokemonID >= SCR_RIDE_POKEMON_NUM )
  {
    GFL_ASSERT( pokemonID < SCR_RIDE_POKEMON_NUM );
    return 0;
  }

  pokemonID = sc_RidePokemonID[ pokemonID ];  // SCR_RIDE_POKEMON_IDからField::RIDE_POKEMON_IDに変換

  Field::EventPokemonRideOn* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOn>( p_gman->GetGameEventManager() );
  pEvent->SetPokemonRideOnID( p_gman, static_cast<Field::RIDE_POKEMON_ID>(pokemonID) );

  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        PlayerRideOffForce
 * @brief       プレイヤーRideOffForce関数.
 * @author      N.Takeda
 * @date        2016/02/24(水) 20:45:10
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerRideOffForce( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  //Field::EventPokemonRideOff* pEvent = 
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOff>( p_gman->GetGameEventManager() );
  
  return 0;
};

//-----------------------------------------------------------------------------
/**
 * @brief フィールドメニューを再生成
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerRemakeFieldMenu( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman  = p_sys->GetGameManager();
  Fieldmap* p_fieldmap = p_gman->GetFieldmap();

  //フィールドマップがある場合、下画面再構築
  if( p_fieldmap != NULL ){
    Field::SubScreen::FieldSubScreenProcManager* p_subsreen = p_fieldmap->GetFieldSubScreenManager();
    p_subsreen->RemakeFrame();
    p_sys->Suspend();

    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief フィールドメニュー再生成待ち
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerIsFrameRemakeFieldMenu( AMX * amx, const cell * ptr )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*  p_gman  = p_sys->GetGameManager();
  Fieldmap* p_fieldmap = p_gman->GetFieldmap();

  //フィールドマップがある場合、下画面の再生成待ち
  if( p_fieldmap == NULL ){
    return false; //生成待ちをするコマンドなので、無効の場合は即効でfalseを返す
  }
  Field::SubScreen::FieldSubScreenProcManager* p_subsreen = p_fieldmap->GetFieldSubScreenManager();

  if( p_subsreen->IsFieldMenuFrameRemake() == false &&
      p_subsreen->IsFieldMenuCreate() )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief レポート書き込みスタート 
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ReportStart_(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  GameSys::GameData       *p_gdata  = p_gman->GetGameData();
  Savedata::PlayTime      *playTime = p_gdata->GetPlayTime();
  gfl2::heap::HeapBase    *p_heap   = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT );

  //前回の保存日付を退避(セーブに失敗した時に書き戻す用)
  playTime->PushSaveTime();
  playTime->SetSaveTime( p_gdata->GetGameTime() );

  p_gdata->SaveStart( p_heap->GetLowerHandle() );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief レポート書き込み待ち 
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ReportWait_(AMX * amx, const cell * ptr )
{
  FieldScriptSystem       *pSys    = FieldScriptSystem::GetInstance();
  ScriptObject            *p_obj   = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman  = p_obj->GetGameManager();
  GameSys::GameData       *p_gdata = p_gman->GetGameData();

  gfl2::fs::FsCtrBackupSystem::Result res = p_gdata->SaveMain();

  // セーブ成功
  if( gfl2::fs::FsCtrBackupSystem::RESULT_OK == res )
  {
    return REPORT_TRY_SUCCESS;
  }
  // セーブ失敗
  else if( gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE != res )
  {
    Savedata::PlayTime *playTime = p_gdata->GetPlayTime();

    //失敗したので最後の保存日付をもとに戻す
    playTime->PopSaveTime();
    return REPORT_TRY_NG;
  }

  //セーブ待ち中
  pSys->Suspend();
  return REPORT_TRY_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief レポートが上書き禁止かどうかを返す 
 *
 *  @retval true   ロックされていて上書き不可
 *  @retval false  上書き可能
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::IsReportLock_(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  GameSys::GameData       *p_gdata  = p_gman->GetGameData();

  return p_gdata->IsSaveLock();
}



//-----------------------------------------------------------------------------
/**
 *  @brief  最後に使用したスプレーを再度使用する
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::UseSprayContinue( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  EventWork               *evwork = p_gman->GetGameData()->GetEventWork();

  u16 item_no = evwork->GetMushiyokeItemNo();

  // スプレーか確認
  if( !(item_no == ITEM_GOORUDOSUPUREE ||
        item_no == ITEM_SIRUBAASUPUREE ||
        item_no == ITEM_MUSIYOKESUPUREE )
    )
  {
    GFL_ASSERT_MSG(0,"セーブされているアイテム番号がスプレーではない\n");
    return 0;
  }

  // 使用チェック
  if( evwork->CanUseMushiyoke() )
  {
    gfl2::heap::HeapBase  *p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    Savedata::MyItem      *p_myitem = p_gman->GetGameData()->GetMyItem();
    
    // 減らせるかチェック
    if( p_myitem->SubCheck( item_no, 1 ) != false )
    {
      // アイテムを減らす
      p_myitem->Sub( item_no, 1 );
      
      // スプレー効果発動
      item::ITEM_DATA item_data( item_no, p_heap );
      u16 count = item_data.GetParam( item::ITEM_DATA::PRM_ID_ATTACK ) * 10;  // @fix GFNMCat[2535]: アイテムデータは１バイトなので、10倍する
      
      evwork->SetMushiyokeCount( item_no, count );
    }
    else
    {
      GFL_ASSERT_MSG(0,"スプレーを減らすことができない\n");
    }

    // スプレー使用音
    Sound::PlaySE( SEQ_SE_SYS_018 );

    return item_no;
  }
  else
  {
    GFL_ASSERT_MSG(0,"スプレーの2重使用\n");
  }

  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        SetDefaultName
 * @brief       Default名のセット.
 * @author      N.Takeda
 * @date        2016/01/05(火) 16:13:50
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SetDefaultName(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  s32   pos = ptr[1];
  
  gfl2::heap::HeapBase        *p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  pml::PokeParty              *party = p_gman->GetGameData()->GetPlayerParty();
  pml::pokepara::PokemonParam *p_pp = party->GetMemberPointer( pos );
  gfl2::str::StrBuf           *strBuf;
  
  strBuf = GFL_NEW(p_heap) gfl2::str::StrBuf(System::STRLEN_POKEMON_NAME + System::EOM_LEN, p_heap);
  
  pml::personal::GetMonsName( strBuf, p_pp->GetMonsNo() );
  p_pp->SetNickName( strBuf );
  
  GFL_SAFE_DELETE( strBuf );
  
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        CheckMapuku
 * @brief       Mapukuのチェック.
 * @author      N.Takeda
 * @date        2016/01/06(水) 13:15:50
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::CheckMapuku( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  s32   pos = ptr[1];
  s32   max = ptr[2];
  
  pml::PokeParty              *party = p_gman->GetGameData()->GetPlayerParty();
  pml::pokepara::PokemonParam *p_pp = party->GetMemberPointer( pos );
  
  return (p_pp->GetFeed() >= max );
};


//------------------------------------------------------------------------------
/**
 * @func        EatMarasada
 * @brief       EatMarasada関数.
 * @author      N.Takeda
 * @date        2016/01/06(水) 13:15:51
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EatMarasada( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  s32   pos = ptr[1];
  s32   point = ptr[2];   /// 加算するなかよし度.
  
  pml::PokeParty              *party = p_gman->GetGameData()->GetPlayerParty();
  pml::pokepara::PokemonParam *p_pp = party->GetMemberPointer( pos );
  
  Savedata::MyStatus* p_status = p_gman->GetGameData()->GetPlayerStatus();
  PokeTool::nakayoshi::UpdateFriendship( p_pp, p_status, point );

  /// 満腹度MAX.
  p_pp->SetFeed( pml::MAX_FEED );
  
  GFL_PRINT( "nakayosid: +%d\n", point );
  
  return point;
};


//------------------------------------------------------------------------------
/**
 * @func        RegistLift
 * @brief       昇降機ギミックへの登録
 * @author      saita_kazuki
 * @date        2016/01/19
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::RegistLift( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj      = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman     = p_obj->GetGameManager();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();


  u32 gimmickID = p_fieldmap->GetFieldGimmickID( Fieldmap::GIMMICK_IDX_ZONE);
  if( gimmickID != GIMMICK_TYPE_LIFT)
  {
    GFL_ASSERT_MSG( 0, "Gimmick type is not GIMMICK_TYPE_LIFT\n");
    return NULL;
  }

  FieldGimmickLift* pGimmick = static_cast<FieldGimmickLift*>( p_fieldmap->GetFieldGimmick( Fieldmap::GIMMICK_IDX_ZONE));

  FieldGimmickLift::RegistParam param;
  param.mode = static_cast<GimmickLiftMode>( ptr[1]);
  param.staticModelEventID = ptr[2];
  for( u32 i = 0; i < FieldGimmickLift::REGIST_MOVE_MODEL_MAX; ++i)
  {
    param.moveModelEventID[i] = ptr[3+i];
  }

  pGimmick->Regist( param);
  
  return NULL;
};

//------------------------------------------------------------------------------
/**
 * @func        UnRegistLift
 * @brief       昇降機ギミックへの登録解除
 * @author      saita_kazuki
 * @date        2016/01/19
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::UnRegistLift( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj      = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman     = p_obj->GetGameManager();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();


  u32 gimmickID = p_fieldmap->GetFieldGimmickID( Fieldmap::GIMMICK_IDX_ZONE);
  if( gimmickID != GIMMICK_TYPE_LIFT)
  {
    GFL_ASSERT_MSG( 0, "Gimmick type is not GIMMICK_TYPE_LIFT\n");
    return NULL;
  }

  FieldGimmickLift* pGimmick = static_cast<FieldGimmickLift*>( p_fieldmap->GetFieldGimmick( Fieldmap::GIMMICK_IDX_ZONE));

  pGimmick->UnRegist();
  
  return NULL;
};

//  r:/home/niji/git_program/niji_project/prog/Field/FieldStatic/source/Time/EvTime.cpp(358):   void EvTime::UpdateDayEvent(GameSys::GameData * gdata, s32 diff_days, b32 is_first, b32 is_field )
//  EvTime  otherwork



//=========================================================================================
/*
    デバッグ時のみ機能が有効なコマンド群
    コマンドの存在はFinalでも消えないが、機能的には何もしないNopとなる
*/
//=========================================================================================

cell AMX_NATIVE_CALL FuncSetCommon::testtest(AMX * amx, const cell * ptr )
{
#if 0//PM_DEBUG
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  s32   param1 = ptr[1];
//  s32   param2 = ptr[2];
//  s32   param3 = ptr[3];
  
#endif  // PM_DEBUG
  return 0;
}

cell AMX_NATIVE_CALL FuncSetCommon::DebugZukanComp(AMX * amx, const cell * ptr )
{
#if PM_DEBUG
  //Param
  ZukanCtrlType type = static_cast<ZukanCtrlType>(ptr[1]);

  PokeTool::ExtendData::LocalArea local_area = PokeTool::ExtendData::LOCAL_AREA_A;
  switch(type){
  case ZUKANCTRL_TYPE_ALOLA:     local_area = PokeTool::ExtendData::LOCAL_AREA_A; break;
  case ZUKANCTRL_TYPE_ALOLA_I01: local_area = PokeTool::ExtendData::LOCAL_AREA_B; break;
  case ZUKANCTRL_TYPE_ALOLA_I02: local_area = PokeTool::ExtendData::LOCAL_AREA_C; break;
  case ZUKANCTRL_TYPE_ALOLA_I03: local_area = PokeTool::ExtendData::LOCAL_AREA_D; break;
  case ZUKANCTRL_TYPE_ALOLA_I04: local_area = PokeTool::ExtendData::LOCAL_AREA_E; break;
  }

  //Alias
  GameSys::GameManager* p_gman      = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  GameSys::GameData*    p_game_data = p_gman->GetGameData();
  Savedata::MyStatus*   p_my_status = p_game_data->GetPlayerStatus();
  Savedata::ZukanData*  p_zukan     = p_game_data->GetZukanData();
  gfl2::heap::HeapBase* p_heap      = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

  //図鑑データのロード
  PokeTool::ExtendData* pPokeToolExtendData = GFL_NEW( p_heap ) PokeTool::ExtendData(p_heap);
  pPokeToolExtendData->Load(p_heap);


  //該当タイプの図鑑の全ポケモンを設定して埋める
  pml::pokepara::PokemonParam* pp;
  u32 num = pPokeToolExtendData->GetLocalAreaMonsMax(local_area);
  for(u32 i = 0; i < num; ++i){
    u32 chihou_no = i+1;
    MonsNo monsno = static_cast<MonsNo>(pPokeToolExtendData->GetZenkokuNumber(chihou_no, local_area));

    pp = GFL_NEW( p_heap ) pml::pokepara::PokemonParam( p_heap, monsno, 98, pml::pokepara::INIT_SPEC_ID_RANDOM );
    PokeTool::PokeMemo::SetFromCapture( pp, p_my_status, 6, p_heap->GetLowerHandle() );
    pp->SetWaza( 0, WAZANO_AKUATEERU );    // アクアテール
    pp->SetLangId(System::GetLang());
    pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
    p_zukan->SetPokeSee(*pp);
    p_zukan->SetPokeGet(*pp);
    GFL_DELETE pp;
  }

  //後始末
  GFL_SAFE_DELETE( pPokeToolExtendData );

#endif  // PM_DEBUG
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        ChangeTrialLight
 * @brief       試練ライト方向変更リクエスト
 * @author      miyachi_soichi
 * @date        2016/02/10
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChangeTrialLight(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::Fieldmap         *p_map    = p_gman->GetFieldmap();

  Field::TrialModel::TrialModelFactory *pFactory = p_map->GetTrialModelFactory();

  f32 x = amx_ctof(ptr[1]);
  f32 y = amx_ctof(ptr[2]);
  f32 z = amx_ctof(ptr[3]);

  pFactory->RequestDirectionalLight( x, y, z );

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        InactivateFieldScan
 * @brief       QRスキャンの終了処理
 * @author      saita_kazuki
 * @date        2016/02/23
 * @fix GFNMCat[1002] フィールドスキャンで戦ったあと、フィールドスキャン状態が終了しない のため追加
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::InactivateFieldScan(AMX * amx, const cell * ptr)
{
  ScriptObject*               p_obj   = ScriptObject::GetClass<ScriptObject*>( amx );
  Savedata::QRReaderSaveData* p_save  = p_obj->GetGameManager()->GetGameData()->GetQRReaderSave();

  p_save->InactivateFieldScan();

  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        MdlShadowWaist
 * @brief       モデルの腰に影を追従させるフラグのセット.
 * @author      N.Takeda
 * @date        2016/03/16(水) 17:09:29
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MdlShadowWaist(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  
  u32         EventID = ptr[1];                   ///< chr_id
  bool        bFlag = GFL_BOOL_CAST(ptr[2]);
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  Field::MoveModel::FieldMoveModel  *pChar = pFieldMoveModelManager->GetFieldMoveModel( ChrID ) ;
  pChar->SetAttachShadowToOrigin( bFlag );
  
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        ChrGetJointPosX_
 * @brief       ChrGetJointPosX_関数.
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrGetJointPosX_(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  u32         EventID = ptr[1];                   ///< chr_id
  u32         LocatorID = ptr[2];                 ///< CHR_JOINT~ CHR_LOC~
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetCharaDrawInstance();
  gfl2::math::Vector3 pos = GetLocaterPos( pCharaDrawInstance, LocatorID );
  pos = p_sys->CalcEventPositionOffset( pos );
  
  return amx_ftoc(pos.x);
}


//------------------------------------------------------------------------------
/**
 * @func        ChrGetJointPosY_
 * @brief       ChrGetJointPosY_関数.
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrGetJointPosY_(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  u32         EventID = ptr[1];                   ///< chr_id
  u32         LocatorID = ptr[2];                 ///< CHR_JOINT~ CHR_LOC~
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetCharaDrawInstance();
  gfl2::math::Vector3 pos = GetLocaterPos( pCharaDrawInstance, LocatorID );
  pos = p_sys->CalcEventPositionOffset( pos );
  
  return amx_ftoc(pos.y);
}


//------------------------------------------------------------------------------
/**
 * @func        ChrGetJointPosZ_
 * @brief       ChrGetJointPosZ_関数.
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrGetJointPosZ_(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  u32         EventID = ptr[1];                   ///< chr_id
  u32         LocatorID = ptr[2];                 ///< CHR_JOINT~ CHR_LOC~
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetCharaDrawInstance();
  gfl2::math::Vector3 pos = GetLocaterPos( pCharaDrawInstance, LocatorID );
  pos = p_sys->CalcEventPositionOffset( pos );
  
  return amx_ftoc(pos.z);
}

//< 話しかけ対象の物体付近でライドできるかチェック。
cell AMX_NATIVE_CALL FuncSetCommon::CheckEventPokemonRideTalkTargetActor(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  Work                    *p_script_work = p_obj->GetWork();
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();

  u32         RideID = ptr[1];                   ///< ride_id

  if( RideID >= SCR_RIDE_POKEMON_NUM )
  {
    GFL_ASSERT( RideID < SCR_RIDE_POKEMON_NUM );
    return 0;
  }

  RideID = sc_RidePokemonID[ RideID ];  // SCR_RIDE_POKEMON_IDからField::RIDE_POKEMON_IDに変換

  p_script_work->SetWorkValue( SCWK_RET0, false );
  p_script_work->SetWorkValue( SCWK_TEMP0, false );
  p_script_work->SetWorkValue( SCWK_TEMP1, 0 );
  p_script_work->SetWorkValue( SCWK_TEMP2, 0 );
  p_script_work->SetWorkValue( SCWK_TEMP3, 0 );

  Field::FieldActorType type = p_script_work->GetTargetActorType();

  //ActorType Check
  switch(type)
  {
  case FIELD_ACTOR_TYPE_ACTION_TALK: 
    break;

  case FIELD_ACTOR_TYPE_MOVEMODEL: 
    break;

  default:
    GFL_ASSERT_STOP(0);  /// それ以外は考えられない。
    break;
  }

  // まずは今の座標で乗れないかチェック
  RIDE_CHECK_RESULT result = EventPokemonRideOn::IsPokemonRideOnEnable( p_gman, static_cast<Field::RIDE_POKEMON_ID>(RideID) );
  if( result == RCR_OK )
  {
    p_script_work->SetWorkValue( SCWK_RET0, true );   // 乗れる。
  }


  return result;
}

//< 話しかけ対象の物体付近でライドを実行する 
cell AMX_NATIVE_CALL FuncSetCommon::CallEventPokemonRideTalkTargetActor(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  Work                    *p_script_work = p_obj->GetWork();
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();

  u32         RideID = ptr[1];                   ///< ride_id


  if( RideID >= SCR_RIDE_POKEMON_NUM )
  {
    GFL_ASSERT( RideID < SCR_RIDE_POKEMON_NUM );
    return 0;
  }

  RideID = sc_RidePokemonID[ RideID ];  // SCR_RIDE_POKEMON_IDからField::RIDE_POKEMON_IDに変換


  Field::EventPokemonRideOn* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOn>( p_gman->GetGameEventManager() );
  pEvent->SetPokemonRideOnID( p_gman, static_cast<Field::RIDE_POKEMON_ID>(RideID) );

  return 0;
}

// 指定のライドに乗っているかチェック
cell AMX_NATIVE_CALL FuncSetCommon::IsPokemonRide(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();

  u32         RideID = ptr[1];                   ///< ride_id

  b32 result = (EventPokemonRideTool::GetPokemonRideOnID( p_gman ) == RideID);

  return result;
}

cell AMX_NATIVE_CALL FuncSetCommon::IsCanExecuteKairikiPos(AMX * amx, const cell *  ptr ) //< @fix GFNMCat[2379] カイリキ岩の押せる位置にいるかチェック
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap* pFieldmap = p_gman->GetFieldmap();

  // ギミックの取得
  if( pFieldmap->GetFieldGimmickID( Fieldmap::GIMMICK_IDX_ZONE ) == GIMMICK_TYPE_NONE )
  {
    // NONEのところには岩のギミックが入っている。
    FieldGimmick * p_gimmick = pFieldmap->GetFieldGimmick( Fieldmap::GIMMICK_IDX_ZONE );
    if( p_gimmick )
    {
      FieldGimmickKairikyRock* p_kairiki_gimmick = static_cast<FieldGimmickKairikyRock*>(p_gimmick);
      if( p_kairiki_gimmick )
      {
        return p_kairiki_gimmick->IsKairikyEventLocation(); // カイリキ岩の押せる状況か？
      }
    }
  }

  return false;
}

/**
 *  @breif キャラクターをカメラの方に向ける。
 // @fix NMCat[133] しげみの洞窟試練の間にて「あなぬけのヒモ」を使うと、脱出後の主人公がカメラの方向に向いていない
 */
cell AMX_NATIVE_CALL FuncSetCommon::ChrLookingAtCamera(AMX * amx, const cell * ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID   = ptr[1];
  float angle = 0.0f;
  
  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }

  gfl2::math::Matrix34 mtx_view;
  pSys->GetGameManager()->GetFieldmap()->GetCameraManager()->GetMainViewCamera()->GetViewMatrix( &mtx_view );
  gfl2::math::Matrix34 inv = mtx_view.Inverse();
  gfl2::math::Vector3 angle_z = inv.GetElemAxisZ();

  if( angle_z.LengthSq() > EPSILON )
  {
    angle_z = angle_z.Normalize();
    angle = gfl2::math::Atan2Deg( angle_z.GetX(), angle_z.GetZ() );

    gfl2::math::SRT srt = pCharaModel->GetLocalSRT();
    srt.rotation = gfl2::math::Quaternion( gfl2::math::Vector3(0,1,0), angle );
    pCharaModel->SetLocalSRT( srt );
  }
  else
  {
    GFL_ASSERT_STOP( 0 ); // カメラがY方向を見ている。
  }

  return angle;
}

/**
 *  @brief  往来キャラクタの表示を操作する
 */
cell AMX_NATIVE_CALL FuncSetCommon::RequestTrafficDisp(AMX * amx, const cell * ptr )
{
  FieldScriptSystem *pScriptSystem = FieldScriptSystem::GetInstance();

  bool bDisp = GFL_BOOL_CAST(ptr[1]);

  Fieldmap *pFieldmap = pScriptSystem->GetGameManager()->GetFieldmap();
  if( pFieldmap )
  {
    Field::TrafficManager *pTrafficManager = pFieldmap->GetTrafficManager();
    if( pTrafficManager )
    {
      pTrafficManager->RequestScriptDisp( bDisp );
    }
  }

  return 0;
}

/**
*  @brief  ルナソルダイブ距離取得
*/
cell AMX_NATIVE_CALL FuncSetCommon::GetLunaSolDiveDistance(AMX * amx, const cell * ptr)
{
  FieldScriptSystem *pScriptSystem = FieldScriptSystem::GetInstance();
  u32 distance = pScriptSystem->GetGameManager()->GetGameData()->GetLunaSolDiveDistance();
  return distance;
}

/**
*  @brief  マンタインサーフスコア取得
*/
cell AMX_NATIVE_CALL FuncSetCommon::GetMantainSurfScore(AMX * amx, const cell * ptr)
{
  FieldScriptSystem *pScriptSystem = FieldScriptSystem::GetInstance();
  Savedata::Misc *pSave = pScriptSystem->GetGameManager()->GetGameData()->GetMisc();
  u32 course_id = ptr[1];

  return pSave->GetSurfRecord(course_id);
}

/**
*  @brief  マンタインサーフテンポラリスコア取得
*/
cell AMX_NATIVE_CALL FuncSetCommon::GetMantainSurfTmpScore(AMX * amx, const cell * ptr)
{
  FieldScriptSystem *pScriptSystem = FieldScriptSystem::GetInstance();
  u32 score = pScriptSystem->GetGameManager()->GetGameData()->GetSurfScore();
  return score;
}

/**
*  @brief  ロトムのお願い（初めて）を達成させる。チュートリアル用
*/
cell AMX_NATIVE_CALL FuncSetCommon::RotomRequestFirstTrigger(AMX * amx, const cell * ptr)
{
  App::FieldMenu::PlayData::SetRotomRequestAchieved(Savedata::FieldMenu::ROTOM_REQ_TUTORIAL);
  return 0;
}

cell AMX_NATIVE_CALL FuncSetCommon::FureaiChrComeNear( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID   = ptr[1];
  b32   isRunEnable = ptr[2];
  b32   isCheckOnly = ptr[3]; // @fix GFMMcat[267]:会話開始の位置によって、なでなでの位置がずれることがある、対処。角度差が小さい場合はモーション無しで回転するように変更。待ちチェック用に移動情報のみを取得するオプションを追加
  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  s32   walkCount = ptr[3] - 1;         ///< 歩数

  s32 resultValue = 0;

  gfl2::math::Vector3 pos;
  resultValue = Field::MoveModel::FieldMoveCodeFureaiTypeA::GetComeNearPosition(pCharaModel->GetMoveCodeWork(), &pos, isRunEnable);
	
  /// 回転.
  gfl2::math::Vector3 axis( 0, 1, 0 );
  gfl2::math::Quaternion quat( axis, 0.0f );
  f32 thresholdAngle = 360.0f;
  
  switch (resultValue)
  {
  case PokemonFureai::FPF_COME_NEAR_TYPE_NO_MOVE:
  case PokemonFureai::FPF_COME_NEAR_TYPE_NO_MOVE_NG:
  {
    return resultValue;
  }

  case PokemonFureai::FPF_COME_NEAR_TYPE_WALK:
  case PokemonFureai::FPF_COME_NEAR_TYPE_WALK_NG:
  {
    if (!isCheckOnly)
    {
      pCharaModel->SetActionCommandTargetMove(Field::MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE, true, pos, quat, false, true, true, thresholdAngle, false, true);//  ang_enable, true, true, thresholdAngle, false, true);
    }
    return resultValue;
  }

  case PokemonFureai::FPF_COME_NEAR_TYPE_RUN:
  case PokemonFureai::FPF_COME_NEAR_TYPE_RUN_NG:
  {
    if (!isCheckOnly)
    {
      Field::MoveModel::FieldMoveCodeFureaiTypeA::SETTING::CAME_NEAR* pCameNearSetting = Field::MoveModel::FieldMoveCodeFureaiTypeA::GetCameNearSettingData(pCharaModel->GetMoveCodeWork());
      pCharaModel->SetActionCommandTargetMove(Field::MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE, false, pos, quat, false, true, true, thresholdAngle, false, true, true

#if 1
        //pCharaModel->SetActionCommandTargetMove(Field::MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE, false, pos, quat, false, true, true, thresholdAngle, false, true, true
        , false, 0.0f, 4
        );
#endif
    }
    return resultValue;
  }
  }

  return PokemonFureai::FPF_COME_NEAR_TYPE_NO_MOVE_NG;
}

cell AMX_NATIVE_CALL FuncSetCommon::FureaiChrSetEnd(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  u32   end_type = ptr[2];             ///< 未使用

  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return false;
  }


	Field::MoveModel::FieldMoveCodeFureaiTypeA::SetEndStatus(pCharaModel->GetMoveCodeWork(), end_type );
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetCommon::FureaiChrSetSleepEnd(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  u32   end_type = ptr[2];             ///< 未使用

  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return false;
  }

  Field::MoveModel::FieldMoveCodeFureaiTypeA::SetSleepEndStatus(pCharaModel->GetMoveCodeWork() );

  Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pCharaModel->GetActorUserWork());
  pFieldMoveModelActor->StopFrureaiSleep();

	PokemonFureai::SetSleepEnd( PokemonFureai::GetFureaiNo(pCharaModel), true );
  return NULL;
}

cell AMX_NATIVE_CALL FuncSetCommon::ReqStaffList(AMX* amx, const cell* ptr)
{
  u32 id = ptr[1];
  App::StaffList::BgPutMode side = (App::StaffList::BgPutMode)ptr[2];
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  pSys->GetGameManager()->GetFieldmap()->GetFieldStaffroll()->SetRequest(id, side);
  return 0;
}

cell AMX_NATIVE_CALL FuncSetCommon::WaitStaffList(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  pSys->SetWaitFunc(amx, WaitFunc_StaffList, 0, NULL);
  pSys->Suspend();
  return true;
}

cell AMX_NATIVE_CALL FuncSetCommon::OpenSakaki(AMX* amx, const cell* ptr)
{
  FieldScriptSystem*                  pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*               pGameManager = pSys->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  // trueで来てはいけない
  GFL_ASSERT(isEvent != true);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  pBattleFesSave->OpenSakaki();
  return NULL;
}



GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

