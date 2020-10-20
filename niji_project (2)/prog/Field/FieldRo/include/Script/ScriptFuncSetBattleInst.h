//======================================================================
/**
 * @file ScriptFuncSetBattleInst.h
 * @date 2016/01/10 15:28:13
 * @author onoue_masayuki
 * @brief バトル施設命令セット
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __SCRIPTFUNCSETBATTLEINS_H_INCLUDED__
#define __SCRIPTFUNCSETBATTLEINS_H_INCLUDED__
#pragma once

#include <pawn\include\gfl2_Pawn.h>
#include "niji_reference_files/script/FieldPawnTypes.h"

GFL_NAMESPACE_BEGIN(GameSys)
class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )  ///< ScriptObject の定義に合わせて修正

/**
* @class FuncSetSodateya
* @brief バトル施設関連コマンド
*/
class FuncSetBattleInst
{
  GFL_FORBID_COPY_AND_ASSIGN(FuncSetBattleInst);
public:
  static const GFL_PAWN_FUNC_TABLE* GetTable( void );

private:
  static GameSys::GameManager* GetGameManager(AMX * amx);
  static int GetWinCountFromSave(GameSys::GameManager* p_gman,const Savedata::BattleTreeType type, const Savedata::BattleTreeRank rank);

  //-----------------------------------------------------------------------------
  /**
   *	@brief  ユーティリティワークを生成
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL BattleInstWorkCreate( AMX *, const cell * );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  ユーティリティワークを解放
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL BattleInstWorkDelete( AMX *, const cell * );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  ランク取得
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetRank( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  トレーナーセット
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL SetTrainer( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  戦闘開始
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL StartBattle( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  トレーナーＯＢＪ追加 グリッド
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL AddTrainerObj( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  着せ替えトレーナーOBJ追加
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL AddDressupTrainerObj( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  モデル読み込み待ち
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL WaitModelLoad( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  モデル情報削除
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL ClearModelData( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  連勝数インクリメント
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL AddWinCount( AMX *, const cell * );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  連勝数クリア
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL ClearWinCount( AMX *, const cell * );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  連勝数取得(通信時は自分の連勝数と不一致になる可能性がある)
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetWinCount( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  連勝数取得(セーブデータ)
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetMyWinCount( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  最大連勝数取得(セーブデータ)
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetMyMaxWinCount( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  勝ったか？
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL IsWin( AMX *, const cell * );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  対戦相手はボスだった？
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL IsBoss( AMX *, const cell * );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  キャラクターID取得
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetCharacterID( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  パートナーキャラクターID取得
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetPartnerCharacterID( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  パートナーキャラクター施設トレーナーID取得
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetPartnerTrainerID( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  選んだ友達ドレスアップキャラの性格を取得
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetCharTypeDressup( AMX *, const cell * );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  通信ＹＥＳＮＯ待ち
   */
  //----------------------------------------------------------------------------- 
  static cell AMX_NATIVE_CALL WaitCommYesNo( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  戦闘開始メッセージのＩＤを返す
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL BattleInst_GetBattleStartMsgId( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  戦闘開始メッセージのＩＤを返す
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL BattleInst_GetPartnerStartMsgId( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  パートナーの負けメッセージIDを返す
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL BattleInst_GetPartnerLoseMsgId( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  戦闘評価アプリコール
   */
  //----------------------------------------------------------------------------- 
  static cell AMX_NATIVE_CALL TrialHouse_CallBtlResultApp( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  ＢＧＭセット
   *
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL BattleInst_SetBgm( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  ＢＧＭ取得
   *
   */
  //----------------------------------------------------------------------------- 
  static cell AMX_NATIVE_CALL BattleInst_GetBgm( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  バトルハウス終了処理
   *
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL BattleInst_End( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
   *	@brief  通信中か？
   *
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL BattleInst_IsComm( AMX *, const cell * ptr );
  static cell AMX_NATIVE_CALL BattleInst_IsNetworkError( AMX *amx, const cell * ptr );
  static cell AMX_NATIVE_CALL BattleInst_GetCommCheckResult( AMX *amx, const cell * ptr );
  static cell AMX_NATIVE_CALL BattleInst_IsCommExit( AMX *amx, const cell * ptr );
  static cell AMX_NATIVE_CALL GetResultRegulationSelectApp_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL GetResultPokeListApp_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL GetResultNpcSelectApp_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL GetResultQuickMatchApp_( AMX*, const cell* );
  
  static cell AMX_NATIVE_CALL GetBattleRule( AMX*, const cell* );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  ＡＩマルチ　相方セットアップ
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL SetupAiPartner( AMX *, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  レギュレーション設定
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL SetRegulation( AMX* amx, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief ロイヤルマスターランク連勝数加算
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL AddWinCountRoyal( AMX *, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief ロイヤルマスターランク連勝数クリア
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL ClearWinCountRoyal( AMX *, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief ロイヤルマスターランク連勝数取得
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetWinCountRoyal( AMX *, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief ロイヤルランク解放
   * @note マスターランクまで解放されていた場合は何もしない
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL ReleaseNextRankRoyal( AMX *, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief ロイヤル解放ランク取得
   * @return 解放されているランク。スーパーまで解放されていればスーパーが返る
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetReleaseRankRoyal( AMX *, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief ロイヤル選択ランク取得
   * @return 選択されたランク。先にレギュレーション選択アプリを呼ぶこと
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetSelectRankRoyal( AMX *, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief ロイヤルプレイヤー順位取得
   * @return 順位(0オリジン) 1位:0、2位:1 …
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetPlayerRankingRoyal( AMX *, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief 最後に場に出ていたポケモンのパーティー内番号取得
   * @return 最後に場に出ていたポケモンのパーティー内番号(全滅して場にポケモンがいなかったら-1)
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetLastPokePartyIndexRoyal( AMX *, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief ボス戦か
   * @retval true ボス戦
   * @retval false それ以外
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL IsBossRoyal( AMX *, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief バトルロイヤル開始
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL StartBattleRoyal( AMX *, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief バトルロイヤル用の生成処理
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL CreateForRoyal( AMX *, const cell * ptr );

  //--------------------------------------------------------------------------
  /**
   * @brief バトルロイヤル用の破棄処理
   */
  //--------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL DeleteForRoyal( AMX *, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  基本データの送信
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL SendCommonData( AMX* amx, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  通信用対戦相手セット
  *
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL SetCommVsTariner( AMX *, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  対戦相手を交換
  *
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL TradeTarinerData( AMX *amx, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  通信切断
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL BattleInstDisconnect( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  バトル施設スカウトトレーナーがいっぱいか
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL IsBinstFullScoutTrainerData( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  既にスカウト済みのバトル施設スカウトトレーナーか？
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL IsBinstAllreadyScoutTrainerData( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  既にリーリエをスカウト済みか？
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL IsBinstAllreadyLilieScout(AMX *amx, const cell * ptr);
  //-----------------------------------------------------------------------------
  /**
  *	@brief  現在の敵をスカウト
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL ScoutCurrentTrainer( AMX *amx, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  リーリエをスカウト
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL ScoutLilie(AMX *amx, const cell * ptr);

  //-----------------------------------------------------------------------------
  /**
  *	@brief  選択したトレーナーを削除
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL RemoveSelectScoutTrainer( AMX *amx, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  トレーナータイプをワードセットに登録
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL WordSetBinstTrainerTypeNameFromTrainerNo( AMX *amx, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  トレーナー名をワードセットに登録
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL WordSetBinstTrainerNameFromTrainerNo( AMX *amx, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
   *	@brief  トレーナータイプ名とトレーナー名をワードセットに登録
   */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL WordSetBinstTrainerTypeNameAndTrainerNameFromTrainerNo( AMX *amx, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  バトルに参加したポケモン全員にリボンをセット
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL SetRibbonEntryPokeAll( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  バトル施設のご褒美用勝利カウンタをセット
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL SetPrizeWinCount( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  バトル施設のご褒美用勝利カウンタを取得
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL GetPrizeWinCount( AMX *amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  バトル施設のご褒美用勝利カウンタをセット
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL ClearPrizeWinCount( AMX *amx, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  ギミックをオープンする
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL OpenBattleInstGimmick( AMX* amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  ギミックをクローズする
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL CloseBattleInstGimmick( AMX* amx, const cell * ptr );
  //-----------------------------------------------------------------------------
  /**
  *	@brief  バトル施設参加のダイアリーをセット
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL SetBinstPokeDiary( AMX *amx, const cell * ptr );
  static cell AMX_NATIVE_CALL DEBUG_SetupSingleParty( AMX *amx, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  バトル施設用ポケモンニックネームワードセット
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL WordSetSelectPartyMonsNickName( AMX* amx, const cell * ptr );

  //-----------------------------------------------------------------------------
  /**
  *	@brief  親かチェック
  */
  //-----------------------------------------------------------------------------
  static cell AMX_NATIVE_CALL BattleInst_GetIsParent( AMX *amx, const cell * ptr );
};


GFL_NAMESPACE_END( FieldScript )
  GFL_NAMESPACE_END( Field )

#endif // __SCRIPTFUNCSETBATTLEINS_H_INCLUDED__
