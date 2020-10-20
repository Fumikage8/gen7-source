//=============================================================================
/**
 *
 *  @file   ScriptFuncSetInit.h
 *  @brief  スクリプト命令セット：初期化スクリプト用命令セット
 *  @author Miyuki Iwasawa
 *  @data   2015.04.29
 *
 */
//=============================================================================
#ifndef __SCRIPTFUNCSETINIT_H__
#define __SCRIPTFUNCSETINIT_H__

#include <pawn\include\gfl2_Pawn.h>
#include <Fade/include/gfl2_FadeManager.h>

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
class FuncSetInit
{
  GFL_FORBID_COPY_AND_ASSIGN( FuncSetInit );
public:
  static const GFL_PAWN_FUNC_TABLE* GetTable( void );
  static const GFL_PAWN_FUNC_TABLE* GetTableForRegulation( void );

  static u8 CheckWazaOshieMons( pml::pokepara::PokemonParam* p_pp, u32 mode, WazaNo wazano = WAZANO_NULL );
  static s32 GetPokePartyPokemonParam( u16 pos, u16 idx, u16 param, pml::PokeParty* party, GameSys::GameData* p_gameData );

  // レギュレーション用
  static cell AMX_NATIVE_CALL PokePartyGetParamForRegulation( AMX *, const cell * );

private:
  // イベントフラグ
  static cell AMX_NATIVE_CALL FlagGet( AMX *, const cell * );
  static cell AMX_NATIVE_CALL FlagSet( AMX *, const cell * );
  static cell AMX_NATIVE_CALL FlagReset( AMX *, const cell * );
  static cell AMX_NATIVE_CALL WorkGet( AMX *, const cell * );
  static cell AMX_NATIVE_CALL WorkSet( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ZoneIsPokecen( AMX *, const cell * );
  static cell AMX_NATIVE_CALL IsContinueRecover( AMX *, const cell *);

  // システム操作
  static cell AMX_NATIVE_CALL CommandNOP( AMX *, const cell * );
  static cell AMX_NATIVE_CALL Assert_( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL ReserveScript( AMX* amx, const cell * ptr );
  static cell AMX_NATIVE_CALL IsReserveScriptSetting( AMX* amx, const cell * ptr );
  static cell AMX_NATIVE_CALL WeakReserveScript( AMX* amx, const cell * ptr );

  static cell AMX_NATIVE_CALL CallBackRegist_( AMX*, const cell *);
  static cell AMX_NATIVE_CALL CallBackGetScriptID_( AMX*, const cell *);

  static gfl2::Fade::FADE_TYPE ConvertFadeType( s32 type, gfl2::math::Vector4* col_s, gfl2::math::Vector4* col_e );
  static cell AMX_NATIVE_CALL FadeIn_( AMX* amx, const cell * ptr );
  static cell AMX_NATIVE_CALL FadeOut_( AMX* amx, const cell * ptr );
  static cell AMX_NATIVE_CALL FadeOutColor( AMX* amx, const cell * ptr );
  static cell AMX_NATIVE_CALL FadeWait_( AMX* amx, const cell * ptr );
  static cell AMX_NATIVE_CALL FadeCheck_( AMX* amx, const cell * ptr );
  
  static cell AMX_NATIVE_CALL FieldOpen_( AMX* amx, const cell * ptr );
  static cell AMX_NATIVE_CALL FieldClose_( AMX* amx, const cell * ptr );

  static cell AMX_NATIVE_CALL LoadFieldDLL_( AMX* amx, const cell * ptr );
  static cell AMX_NATIVE_CALL UnloadFieldDLL_( AMX* amx, const cell * ptr );

  static cell AMX_NATIVE_CALL MapChangeNoFieldCore_( AMX* amx, const cell * ptr );

  static cell AMX_NATIVE_CALL SetEventBasePos( AMX* amx, const cell * ptr );
  static cell AMX_NATIVE_CALL ResetEventBasePos( AMX* amx, const cell * ptr );
  static cell AMX_NATIVE_CALL CheckEventBasePosEnable( AMX* amx, const cell * ptr );

  // アイテム関連
  static cell AMX_NATIVE_CALL ItemAdd( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ItemSub( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ItemGetNum( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ItemAddCheck( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ItemIsWazaMachine( AMX * , const cell * );
  static cell AMX_NATIVE_CALL ItemIsMegaStone( AMX * , const cell * );
  static cell AMX_NATIVE_CALL ItemGetCategory( AMX * , const cell * );
  static cell AMX_NATIVE_CALL ItemGetNutsID( AMX * , const cell * );
  static cell AMX_NATIVE_CALL ItemBallID2ItemID( AMX * , const cell * );
  static cell AMX_NATIVE_CALL PocketGetID( AMX * , const cell * );
  static cell AMX_NATIVE_CALL PocketGetTotalCount( AMX * , const cell * );

  static cell AMX_NATIVE_CALL DressupItemAddDel_( AMX *, const cell * );

  static cell AMX_NATIVE_CALL PokeBeansAddSub( AMX * , const cell * );
  static cell AMX_NATIVE_CALL PokeBeansNumGet( AMX * , const cell * );

  static cell AMX_NATIVE_CALL GetTimeZone( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetHour( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetMinute( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetSecond( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetDay( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetMonth( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetWeek( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL IsPenaltyTime( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetDateTimeToDay( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetRealTimeToDay( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetDateTimeToSecond( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetRealTimeToSecond( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetGameStartTimeToSecond( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetBirthdayMonth( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL GetBirthdayDay( AMX *, const cell * ptr );

  static cell AMX_NATIVE_CALL RomGetVersion( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL RomGetLanguageID( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL RegionGet( AMX *, const cell * ptr );

  // ポケモン関係
  static cell AMX_NATIVE_CALL PokemonAdd(AMX *, const cell *);
  static cell AMX_NATIVE_CALL PokePartyRemoveMember(AMX *, const cell *);
  static cell AMX_NATIVE_CALL PokePartyGetParam(AMX* amx, const cell * ptr);
  static cell AMX_NATIVE_CALL PokePartySetParam(AMX* amx, const cell * ptr);
  static cell AMX_NATIVE_CALL PokePartyAddParam(AMX* amx, const cell * ptr);
  static cell AMX_NATIVE_CALL PokePartyLevelUp(AMX* amx, const cell * ptr);
  static cell AMX_NATIVE_CALL PokePartyGetCount( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL PokePartyGetCountEx( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL PokePartyRecoverAll(AMX *, const cell *);
  static cell AMX_NATIVE_CALL PokePartyIsOwner( AMX *, const cell * );
  static cell AMX_NATIVE_CALL PokePartyHaveWaza(AMX *, const cell *);
  static cell AMX_NATIVE_CALL PokePartyGetWazaCount(AMX *, const cell *);
  static cell AMX_NATIVE_CALL PokePartyGetWazaID(AMX *, const cell *);
  static cell AMX_NATIVE_CALL PokePartySetWaza(AMX *, const cell *);
  static cell AMX_NATIVE_CALL PokePartyCheckWazaRemind(AMX *, const cell *);
  static cell AMX_NATIVE_CALL PokePartySetDefaultName( AMX*, const cell * );
  static cell AMX_NATIVE_CALL PokePartyRecover(AMX *, const cell *);
  static cell AMX_NATIVE_CALL PokePartyGetTrainingDone(AMX* amx, const cell * ptr);
  static cell AMX_NATIVE_CALL PokePartySetTrainingDone(AMX* amx, const cell * ptr);
  static cell AMX_NATIVE_CALL PokePartyGetMezameruPowerType(AMX* amx, const cell * ptr);
  static cell AMX_NATIVE_CALL Shoki3GetPersonalRnd(AMX* amx, const cell * ptr);

  static cell AMX_NATIVE_CALL BoxIsFull(AMX *, const cell *);
  static cell AMX_NATIVE_CALL BoxIsBattleTeamIn(AMX *, const cell *);
  static cell AMX_NATIVE_CALL BoxMonsNoExists(AMX *, const cell *);

  static cell AMX_NATIVE_CALL BoxGetPokeCount(AMX *, const cell *);
  static cell AMX_NATIVE_CALL WazaOshieCheck(AMX *, const cell *);
  static cell AMX_NATIVE_CALL WazaOshieGetWazaID(AMX *, const cell *);

  static cell AMX_NATIVE_CALL PokePartyToBox( AMX*, const cell * );
  static cell AMX_NATIVE_CALL PokePartyChangeFormNo( AMX*, const cell * );
  static cell AMX_NATIVE_CALL PokePartyChangeRotomFormNo( AMX*, const cell * );
  static cell AMX_NATIVE_CALL PokePartyChangeTorimianFormNo( AMX*, const cell * );

  // 図鑑
  static cell AMX_NATIVE_CALL ZukanGetOpenFlag(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ZukanGetCount(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ZukanCheckGetFlag(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ZukanCheckSeeFlag(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ZukanIsComplete(AMX *, const cell * );

  // トレーナー
  static cell AMX_NATIVE_CALL TrainerBattleGetRule( AMX *, const cell * );

  static cell AMX_NATIVE_CALL PlayerSetFieldMenu( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerGetFieldMenu( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerGetSex( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerGetMoney( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerSetMoney( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerAddMoney( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerSubMoney( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerGetBP( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerSetBP( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerGetZoneID( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerGetID( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerSetTrainerPassStamp( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerResetTrainerPassStamp( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerCheckTrainerPassStamp( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerBallThrowTypeSet( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL PlayerBallThrowTypeGet( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL GetPlaceNameIDFormZoneID( AMX * amx, const cell * ptr );

  // レコード
  static cell AMX_NATIVE_CALL RecordGet( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL RecordAdd( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL RecordSet( AMX * amx, const cell * ptr );

  // 全力リングフラグ操作
  static cell AMX_NATIVE_CALL ZenryokuRingFlagSet( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL ZenryokuRingFlagGet( AMX * amx, const cell * ptr );
  // メガリングフラグ操作
  static cell AMX_NATIVE_CALL MegaRingFlagSet( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL MegaRingFlagGet( AMX * amx, const cell * ptr );
  // しまめぐりのしるし操作
  static cell AMX_NATIVE_CALL ShimameguriNoShirushiFlagSet( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL ShimameguriNoShirushiFlagGet( AMX * amx, const cell * ptr );

  // ポケモンダイアリー
  static cell AMX_NATIVE_CALL PokeDiarySet( AMX * amx, const cell * ptr );

  //=============================================
  //手持ち配布ポケモンチェック
  static cell AMX_NATIVE_CALL PokePartyCheckDistribution(AMX* amx, const cell * ptr);
  static cell AMX_NATIVE_CALL MagianaQREventEnableCheck(AMX* amx, const cell * ptr);
  static cell AMX_NATIVE_CALL M20PikatyuQREventEnableCheck(AMX* amx, const cell * ptr);

  //=============================================
  //育て屋 タマゴが発生しているか？ initスクリプトで呼び出したいのでFuncsetProgFieldから引っ越した
  static cell AMX_NATIVE_CALL SodateyaIsEggExist(AMX * amx, const cell * ptr);

  /* ------------------------------------------------------------------------- */
  /**
   * @brief ジガルデセル関連
  */
  /* ------------------------------------------------------------------------- */
  static cell AMX_NATIVE_CALL ZigarudeCellGet( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL ZigarudeCellAddCheck( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL ZigarudeCellSubCheck( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL ZigarudeCellAdd( AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL ZigarudeCellSub( AMX * amx, const cell * ptr );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 無線通信チェック
   */
  /* -----------------------  -------------------------------------------------- */
  static cell AMX_NATIVE_CALL CheckWLANSwitch( AMX* amx, const cell * ptr );


  // 指定ゾーンに関連したフライフラグをセットする
  static cell AMX_NATIVE_CALL FlyFlagSet( AMX * amx, const cell * ptr );

  // ワープ
  static cell AMX_NATIVE_CALL WarpIDSet( AMX * amx, const cell * ptr );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief エスケープロケーション(穴抜けの脱出先)の明示的記録 
   *
   * @param zone_id 脱出先ゾーンID
   * @param x       脱出先X
   * @param y       脱出先Y(floatなので注意)
   * @param z       脱出先Z
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
  static cell AMX_NATIVE_CALL SetEscapeLocation( AMX * amx, const cell * ptr );


  // ジョインフェスタ
  static cell AMX_NATIVE_CALL IsJoinFestaReward(AMX *, const cell * );
  static cell AMX_NATIVE_CALL IsJoinFestaAttractionOpen(AMX *, const cell * );
  static cell AMX_NATIVE_CALL IsJoinFestaAttractionPlay(AMX *, const cell * );
  static cell AMX_NATIVE_CALL RecreateFieldPersonalList(AMX *, const cell * );
  static cell AMX_NATIVE_CALL JoinFestaAttractionRideOff(AMX *, const cell * );
  static cell AMX_NATIVE_CALL JoinFestaResetRecruiter(AMX *, const cell * );

  //不思議なおくりもの
  static cell AMX_NATIVE_CALL PostmanGetGiftCount(AMX *, const cell * );
  static cell AMX_NATIVE_CALL PostmanGetGiftParam(AMX *, const cell * );
  static cell AMX_NATIVE_CALL PostmanSetGiftReceive(AMX *, const cell * );
  static cell AMX_NATIVE_CALL PostmanSetGiftUsedFlag(AMX *, const cell * );

  static cell AMX_NATIVE_CALL DebugChrEditListAdd_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL DebugSetSceneEditMode(AMX *, const cell * );
  static cell AMX_NATIVE_CALL DebugGetSceneEditMode(AMX *, const cell * );

  static cell AMX_NATIVE_CALL SetNormalAlolaOffset(AMX *, const cell * );


  //便利
  static cell AMX_NATIVE_CALL MathGetDiffAngleAbs(AMX *, const cell * );
  static cell AMX_NATIVE_CALL MathGetTargetAngle(AMX *, const cell * );
  static cell AMX_NATIVE_CALL MathGetVectorLength(AMX *, const cell * );
  static cell AMX_NATIVE_CALL MathGetEasingValue(AMX *, const cell * );
  static cell AMX_NATIVE_CALL MathGetVectorX(AMX *, const cell * );
  static cell AMX_NATIVE_CALL MathGetVectorZ(AMX *, const cell * );

};

GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );

#endif // __SCRIPTFUNCSETINIT_H__
