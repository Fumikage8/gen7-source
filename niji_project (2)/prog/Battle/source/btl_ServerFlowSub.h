#if !defined( __BTL_SERVERFLOWSUB_H__ )
#define __BTL_SERVERFLOWSUB_H__
//=============================================================================================
/**
 * @file  btl_ServerFlowSub.h
 * @brief ポケモンXY バトルシステム サーバコマンド生成処理サブ
 * @author  taya
 *
 * @date  2011.02.01  作成
 */
//=============================================================================================
#pragma once

#include  "./btl_Common.h"
#include  "./btl_MainModule.h"
#include  "./btl_PokeSet.h"
#include  "./btl_PokeParam.h"
#include  "./btl_server_const.h"

GFL_NAMESPACE_BEGIN(btl)

class ServerFlow;
class DeadRec;

class ServerFlowSub {
  GFL_FORBID_COPY_AND_ASSIGN( ServerFlowSub );

public:
  /**
   *  経験値計算用ワーク
   */
  struct CALC_EXP_WORK{
    u32  exp;            ///< もらえる経験値
    u8   hp;             ///< HP努力値
    u8   pow;            ///< こうげき努力値
    u8   def;            ///< ぼうぎょ努力値
    u8   agi;            ///< すばやさ努力値
    u8   sp_pow;         ///< とくこう努力値
    u8   sp_def;         ///< とくぼう努力値
    u8   fBonus;         ///< 多めにもらえたフラグ
    u8   bGakusyuSouti;  ///< がくしゅうそうちによってもらえたフラグ（単体）
    u8   bGakusyuSoutiOnly;  ///< がくしゅうそうちによってのみ得た経験値である

    // 初期化
    void Clear( void )
    {
      exp = 0;
      fBonus = false;
      bGakusyuSouti = false;
      bGakusyuSoutiOnly = true;
      hp = pow = def = agi = sp_pow = sp_def = 0;
    }

    // 合算
    void Marge( const CALC_EXP_WORK& rhs )
    {
      exp += rhs.exp;
      hp  += rhs.hp;
      pow += rhs.pow;
      def += rhs.def;
      agi += rhs.agi;
      sp_pow += rhs.sp_pow;
      sp_def += rhs.sp_def;
      if( !fBonus ){
        fBonus = rhs.fBonus;
      }
      if( !rhs.bGakusyuSouti ){
        bGakusyuSoutiOnly = false;
      }
    }
  };

  /**
   *  ボール投げ→捕獲成功コマンドに必要な情報を格納しておく
   * （経験値処理後、再呼び出しすることで後半部分のコマンド作成に利用する）
   */
  struct POKE_CAPTURED_CONTEXT {
    public:
    BtlPokePos  targetPos;
    bool  bCaptured;
    bool  bZukanRegister;

    void Clear( void ){
      targetPos = BTL_POS_NULL;
      bCaptured = false;
      bZukanRegister = false;
    };
    void Set( BtlPokePos pos, bool _bCaptured, bool _bZukanRegister ){
      targetPos = pos;
      bCaptured = _bCaptured;
      bZukanRegister = _bZukanRegister;
    };
  };


  ServerFlowSub( void ){ };
  ~ServerFlowSub(){ };


  u8              RegisterTargets( ServerFlow* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
                    const SVFL_WAZAPARAM* wazaParam, PokeSet* rec );

  /**
   *  「どうぐ」使用時の各種処理、コマンド生成
   */
  TrItemResult    TrainerItemProc( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, u8 actParam, u8 targetIdx, POKE_CAPTURED_CONTEXT* capturedContext );

  /**
   *  TrainerItemProc で野生ポケモンを捕獲した後、経験値処理後のコマンドを生成する
   */
  void            CreateCapturedCmd( SCQUE* que, const POKE_CAPTURED_CONTEXT* context );

  bool            CalcExp( BtlRule rule, ServerFlow* flwk, BTL_PARTY* party, const BTL_POKEPARAM* deadPoke, CALC_EXP_WORK* result );



private:




  bool  correctTargetDead( ServerFlow* flwk, BtlRule rule, const BTL_POKEPARAM* attacker,
          const SVFL_WAZAPARAM* wazaParam, BtlPokePos targetPos, PokeSet* rec );

  u8 correctTargetDeadSub_storeNextTargets(
    ServerFlow* flwk,
    u8* dstNextTargetPos,
    BTL_POKEPARAM**  dstNextTarget, u8 dstNextTargetElems,
    const BTL_POKEPARAM* attacker,
    const SVFL_WAZAPARAM* wazaParam );

  u8 correctTargetDeadSub_decideTarget(
       BtlPokePos   defaultTargetPos,
       u8   targetListCount,
       BTL_POKEPARAM**  targets,
       u8*  targetPos );



  BTL_POKEPARAM* checkTemptPokemon( ServerFlow* flwk, BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam );
  u8             checkTemptPokeID( ServerFlow* flwk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, u8 targetPokeID );

  u8    scEvent_GetWazaTargetIntr( ServerFlow* flwk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam );
  u8    scEvent_GetWazaTargetTempt( ServerFlow* flwk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, u8 defaultTargetPokeID, TemptTargetCause* outTemptCause );
  void  scEvent_WazaTargetTemptEnd( ServerFlow* flwk, const BTL_POKEPARAM* attacker, u16 targetPokeId, const SVFL_WAZAPARAM* wazaParam, TemptTargetCause temptCause );

  bool  scproc_TrainerItem_BallRoot( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, bool* bUsedBall, POKE_CAPTURED_CONTEXT* capContext );
  bool  calcCapturePokemon( ServerFlow* flwk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke,u16 ballID,
          u8* yure_cnt, u8* fCritical );

  #if PM_DEBUG
  bool  calcCapturePokemonTest( ServerFlow* flwk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke,u16 ballID,
          u8* yure_cnt, u8* fCritical );
  bool  calcCapturePokemonTestSub( ServerFlow* flwk, u8 pokeRatio, u8 hpRatio, fx32 ballRatio, u8 checkCount, bool printFlag );
  #endif

  fx32  getKusamuraCaptureRatio( ServerFlow* flwk );
  fx32  calcBallCaptureRatio( ServerFlow* flwk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke, u16 ballID );
  fx32  getBallCaptureRatio( ServerFlow* flwk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke, u16 ballID );
  bool  checkCaptureCritical( ServerFlow* flwk, fx32 capture_value );
  u8    ItemEff_SleepRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_PoisonRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_YakedoRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_KooriRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_MahiRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_KonranRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_MeromeroRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_EffectGuard( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_Relive( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_AttackRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_DefenceRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_SPAttackRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_SPDefenceRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_AgilityRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_HitRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_CriticalUp( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_PP_Rcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_AllPP_Rcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_HP_Rcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ItemEff_Common_Cure( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID );
  u8    ItemEff_Mental_Cure( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID );
  u8    ItemEff_Common_Rank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, BTL_POKEPARAM::ValueID rankType );
  u8    ShooterEff_ItemCall( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ShooterEff_SkillCall( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ShooterEff_ItemDrop( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
  u8    ShooterEff_FlatCall( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );

  void calcDoryokuExp( BtlRule rule, BTL_POKEPARAM* bpp, const BTL_POKEPARAM* deadPoke, CALC_EXP_WORK* result );

  void makeCmd_RecoverHP( ServerFlow* flwk, BTL_POKEPARAM* bpp, u32 recoverHP, bool bStdMsg );
  bool makeCmd_RecoverAllPP( ServerFlow* flwk, BTL_POKEPARAM* bpp, u8 recoverLimit, bool bStdMsg );
  void makeCmd_CureSick( ServerFlow* flwk, BTL_POKEPARAM* bpp, WazaSick sickID, bool bStdMsg );
  void makeCmd_RankEffect( ServerFlow* flwk, BTL_POKEPARAM* bpp, WazaRankEffect rankType, int volume );



  /**
   *  シューターアイテム使用結果
   */
  typedef enum {
    SHOOTER_REACTION_NONE = 0,    ///< 効果なし、リアクションなし
    SHOOTER_REACTION_ENABLE,      ///< 効果あり
    SHOOTER_REACTION_FAIL_MSG,    ///< 効果なし、リアクションあり（回復封じメッセージなど）
  }ShooterReactionType;


};
GFL_NAMESPACE_END(btl)
#endif // __BTL_SERVERFLOWSUB_H__
