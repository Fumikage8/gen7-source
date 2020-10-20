
#if !defined( __BTL_SERVERFLOW_H__ )
#define __BTL_SERVERFLOW_H__
//=============================================================================================
/**
 * @file  btl_ServerFlow.h
 * @brief ポケモンXY バトルシステム サーバコマンド生成処理
 * @author  taya
 *
 * @date  2010.12.26  作成
 */
//=============================================================================================
#pragma once

#include  <pml/include/pml_PokePara.h>

#include  "./btl_common.h"
#include  "./btl_pokeparam.h"
#include  "./btl_action.h"
#include  "./btl_field.h"
#include  "./btl_WazaRec.h"
#include  "./btl_DeadRec.h"
#include  "./btl_MainModule.h"
#include  "./btl_PokeCon.h"
#include  "./btl_PosPoke.h"
#include  "./btl_AffCounter.h"
#include  "./btl_EventFactor.h"
#include  "./btl_PokeSet.h"
#include  "./btl_EscapeInfo.h"
#include  "./btl_HEManager.h"
#include  "./btl_DmgAffRec.h"
#include  "./btl_ServerFlowSub.h"
#include  "./btl_server_const.h"
#include  "./btl_server_cmd.h"
#include  "./btl_server_TimeLimit.h"
#include  "./btl_SVCL_ACTION.h"
#include  "./btl_ACTION_ORDER_WORK.h"
#include  "./btl_PokemonBattleInRegister.h"
#include  "./btl_RoyalRankingContainer.h"
#include  "./btl_PartyAllDeadRecorder.h"
#include  "./btl_ActionRecorder.h"
#include  "./btl_SimpleEffectFailManager.h"
#include  "./btl_InsertWazaInfoStorage.h"
#include  "./btl_DamageProcParamStack.h"

GFL_NAMESPACE_BEGIN(btl)

class BTL_SERVER;
class ServerFlowSub;
class SideEffectStatus;
class IntrudeSystem;
class ReinforceSystem;
class BattleResultJudge;


/**
 *  サーバ本体、サーバフロー双方から参照するクライアントパラメータ
 */
typedef struct {

  Adapter*     adapter;
  BTL_PARTY*   party;
  u8           myID;
}SVCL_WORK;

enum {
  CLIENT_DISABLE_ID = 0xff,
};


class ServerFlow {
  GFL_FORBID_COPY_AND_ASSIGN( ServerFlow );

  friend class ServerFlowSub;

public:
  /**
   *  計算結果コード
   */
  typedef enum {
    RESULT_DEFAULT = 0,       ///< そのまま続行
    RESULT_POKE_CHANGE,       ///< ターン途中のポケモン入れ替えリクエスト
    RESULT_POKE_COVER,        ///< 空き位置にポケモンを投入するリクエスト
    RESULT_INTRUDE,           ///< 乱入バトル開始リクエスト
    RESULT_LEVELUP,           ///< レベルアップ処理
    RESULT_BTL_QUIT_SHOWDOWN, ///< バトル終了（決着）
    RESULT_BTL_QUIT_ESCAPE,   ///< バトル終了（逃げ）
    RESULT_BTL_QUIT_CAPTURE,  ///< バトル終了（ポケモン捕獲）
  } Result;

  typedef enum {

    SV_WAZAFAIL_NULL = 0,
    SV_WAZAFAIL_PP_ZERO,
    SV_WAZAFAIL_NEMURI,
    SV_WAZAFAIL_MAHI,
    SV_WAZAFAIL_KOORI,
    SV_WAZAFAIL_KONRAN,
    SV_WAZAFAIL_SHRINK,
    SV_WAZAFAIL_KIAI_SHRINK,
    SV_WAZAFAIL_MEROMERO,
    SV_WAZAFAIL_KANASIBARI,
    SV_WAZAFAIL_TYOUHATSU,
    SV_WAZAFAIL_ICHAMON,
    SV_WAZAFAIL_FUUIN,
    SV_WAZAFAIL_KAIHUKUHUUJI,
    SV_WAZAFAIL_HPFULL,
    SV_WAZAFAIL_FUMIN,
    SV_WAZAFAIL_NAMAKE,
    SV_WAZAFAIL_WAZALOCK,
    SV_WAZAFAIL_ENCORE,
    SV_WAZAFAIL_TOKUSEI,
    SV_WAZAFAIL_JURYOKU,
    SV_WAZAFAIL_TO_RECOVER,
    SV_WAZAFAIL_SABOTAGE,
    SV_WAZAFAIL_SABOTAGE_GO_SLEEP,
    SV_WAZAFAIL_SABOTAGE_SLEEPING,
    SV_WAZAFAIL_NO_REACTION,
    SV_WAZAFAIL_SKYBATTLE,
    SV_WAZAFAIL_OOAME,          // 天候「おおあめ」による、ほのお技失敗
    SV_WAZAFAIL_OOHIDERI,       // 天候「おおひでり」による、みず技失敗
    SV_WAZAFAIL_IJIGEN_FOOPA,   // 技「いじげんラッシュ」失敗( フーパ )
    SV_WAZAFAIL_IJIGEN_OTHER,   // 技「いじげんラッシュ」失敗( 汎用 )
    SV_WAZAFAIL_DARKHOLE_OTHER, // 技「ダークホール」ダークライ以外が使用して失敗
    SV_WAZAFAIL_ZIGOKUDUKI,     // 技「じごくづき」を受けたため音技が出せない
    SV_WAZAFAIL_TRAPSHELL,      // 技「トラップシェル」失敗
    SV_WAZAFAIL_UNEXPECTED_ERROR, // 予期しない条件が発生した時の強制失敗
    SV_WAZAFAIL_OTHER,

  }SV_WazaFailCause;

  /**
   *  ワザ強制有効化モード
   */
  typedef enum {
    WAZAENABLE_NONE = 0,
    WAZAENABLE_NORMAL,   ///< 通常
    WAZAENABLE_QUIT,     ///< 強制有効化後、処理中断
  }WazaForceEnableMode;

  /**
   *  状態異常化失敗コード
   */
  typedef enum {
    ADDSICK_FAIL_NULL = 0,
    ADDSICK_FAIL_ALREADY,     ///< すでに同じ状態になっている
    ADDSICK_FAIL_NO_EFFECT,   ///< 効果の無い相手
    ADDSICK_FAIL_OTHER,       ///< その他
//  }BtlAddSickFailCode;
  }AddSickFailCode;

  /**
   *  状態異常上書きモード
   */
  typedef enum {
    SICK_OW_NONE = 0,   ///< 上書きできない
    SICK_OW_POKESICK,   ///< 他の基本状態異常にかかっている時、無視して書き換え
    SICK_OW_STRONG,     ///< なんでも上書き
//  }BtlSickOverWriteMode;
  }SickOverWriteMode;

  //@バトル変更箇所
  /**
   *  天候技失敗判定
   */
  typedef enum {

    CHANGE_WEATHER_OK = 0,      ///< 成功
    CHANGE_WEATHER_FAIL = 1,    ///< 通常失敗(今まで通り
    CHANGE_WEATHER_FAIL_SANGO_WEATHER = 2,    ///< sango新天気失敗
  }ChangeWeatherResult;



  /**
   *  サーバフロー＆ハンドラから参照するワザパラメータ
   */
  typedef SVFL_WAZAPARAM  WAZAPARAM;


  /**
   *  コンストラクタ・デストラクタ
   */
  ServerFlow( BTL_SERVER* server,  MainModule* mainModule,  POKECON* pokeCon,
              SCQUE* que,          HeapHandle heapHandle
  );
  ~ServerFlow();


  void        ResetSystem( void );

  //-----------------------------------------------------
  //-----------------------------------------------------
  const MainModule*           Hnd_GetMainModule( void ) const;
  const BTL_POKEPARAM*        Hnd_GetPokeParam( u8 pokeID );
  u16                         Hnd_CalcAgility( const BTL_POKEPARAM* bpp, bool fTrickRoomApply );
  bool                        Hnd_CheckFieldEffect( FieldStatus::EffectType effType ) const;
  BtlPokePos                  Hnd_PokeIDtoPokePos( u8 pokeID ) const;
  u8                          Hnd_PokePosToPokeID( u8 pokePos );
  BtlPokePos                  Hnd_ReqWazaTargetAuto( u8 pokeID, WazaID waza );
  bool                        Hnd_GetThisTurnAction( u8 pokeID, BTL_ACTION_PARAM* dst );
  bool                        Hnd_IsMyActOrderLast( u8 pokeID );
  bool                        Hnd_IsMyAllActDone( u8 pokeID ) const;
  WazaID                      Hnd_GetPrevExeWaza( void ) const;
  bool                        Hnd_IsPrevExeZenryokuWaza( void ) const;
  void*                       Hnd_GetTmpWork( u32 size );
  const WazaRec*              Hnd_GetWazaRecord( void );
  u16                         Hnd_ReserveQuePos( ServerCmd cmd );
  const DeadRec*              Hnd_GetDeadRecord( void );
  u16                         Hnd_GetTurnCount( void ) const;
  u8                          Hnd_GetPokeChangeCount( u8 clientID );
  bool                        Hnd_IsExistPokemon( u8 pokeID );
  BtlRule                     Hnd_GetRule( void );
  BtlCompetitor               Hnd_GetCompetitor( void );
  BtlGround                   Hnd_GetGround( void );
  const BgComponentData&      Hnd_GetBgComponentData( void ) const;
  u32                         Hnd_GetFrontPosNum( u8 clientID );
  bool                        Hnd_IsMultiMode( void );
  bool                        Hnd_CheckSinkaMae( u8 pokeID );
  u16                         Hnd_CalcAgilityRank( const BTL_POKEPARAM* bpp_target, bool fTrickRoomEnable );
  bool                        Hnd_IsFloatingPoke( u8 pokeID );
  bool                        Hnd_CheckItemUsable( u8 pokeID );
  u16                         Hnd_GetUsableItem( u8 pokeID );
  BtlWeather                  Hnd_GetWeather( void );
  //@バトル変更箇所(エアロックを無視した本来の天気の取得 退場時のsango新天候戻しチェックにのみ使用
  BtlWeather                  Hnd_GetWeather_True( void );
  //@バトル変更箇所(sango新天候戻しでデフォルト天候がチェックしたかった。
  BtlWeather                  Hnd_GetDefaultWeather( void );
  u8                          Hnd_GetWeatherCausePokeID( void ) const;

  bool                        Hnd_GetDebugFlag( BtlDebugFlag flag );
  const SideEffectStatus*     Hnd_GetSideEffectStatus( BtlSide side, BtlSideEffect sideEffect ) const;
  bool                        Hnd_IsExistSideEffect( BtlSide side, BtlSideEffect sideEffect ) const;
  u32                         Hnd_GetSideEffectCount( BtlPokePos pos, BtlSideEffect sideEffect ) const;
  bool                        Hnd_IsExistPosEffect( BtlPokePos pos, BtlPosEffect effect );
  u8                          Hnd_GetFriendClientID( u8 clientID );
  bool                        Hnd_IsMemberOutIntr( void );
  void                        Hnd_AddMemberOutIntr( u8 pokeID );
  bool                        Hnd_IsTameHidePoke( u8 pokeID );
  bool                        Hnd_IsFreeFallUserPoke( u8 pokeID );
  bool                        Hnd_IsFreeFallPoke( u8 pokeID );
  u32                         Hnd_GetWeight( u8 pokeID );
  u32                         Hnd_GetRankEffSerial( void );
  bool                        Hnd_CheckShowDown( void );
  bool                        IsClientTrainerExist( u8 clientID ) const;
  u8                          Hnd_ExpandPokeID( BtlExPos exPos, u8* dst_pokeID );
  const BTL_PARTY*            Hnd_GetPartyData( u8 pokeID );
  const BTL_PARTY*            Hnd_GetFriendPartyData( u8 pokeID );
  BtlPokePos                  Hnd_GetExistFrontPokePos( u8 pokeID );
  BtlPokePos                  Hnd_GetPokeLastPos( u8 pokeID );
  u8                          Hnd_GetExistPokeID( BtlPokePos pos );
  u8                          Hnd_GetAllOpponentFrontPokeID( u8 basePokeID, u8* dst );
  bool                        Hnd_CheckExistTokuseiPokemon( TokuseiNo tokusei );
  BtlTypeAff                  Hnd_SimulationAffinity( u8 atkPokeID, u8 defPokeID, WazaID waza );
  BtlTypeAff                  Hnd_SimulationAffinityCanBench( u8 atkPokeID, u8 defPokeID, WazaID waza );
  BtlTypeAff                  Hnd_SimulationAffinityOnlyAttacker( u8 atkPokeID, u8 defPokeID, WazaID waza );
  BtlTypeAff                  Hnd_SimulationAffinityCore( u8 atkPokeID, u8 defPokeID, WazaID waza, bool onlyAttacker );
  BtlTypeAff                  Hnd_SimulationAffinityCanBenchCore( u8 atkPokeID, u8 defPokeID, WazaID waza, bool onlyAttacker );
  u32                         Hnd_SimulationDamage( u8 atkPokeID, u8 defPokeID, WazaID waza, bool fAffinity, bool fEnableRand );
  bool                        Hnd_IsSimulationMode( void );
  u8                          Hnd_GetClientCoverPosCount( u8 pokeID );
  u8                          Hnd_GetMyBenchIndex( u8 pokeID );
  bool                        Hnd_IsExistBenchPoke( u8 pokeID );
  bool                        Hnd_IsExistChangeEnablePoke( u8 pokeID );
  bool                        Hnd_CheckReservedMemberChangeAction( void ) const;
  void                        Hnd_SetWazaEffectIndex( u8 effIndex );
  bool                        Hnd_AddBonusMoney( u32 volume, u8 pokeID );
  void                        Hnd_SetMoneyDblUp( u8 pokeID, MoneyDblUpCause cause );
  void                        Hnd_AddMagicCoatAction( u8 pokeID, u8 targetPokeID );
  bool                        Hnd_FreeFallStart( u8 atkPokeID, u8 targetPokeID, const WAZAPARAM* wazaParam, bool* fFailMsgDisped );
  void                        Hnd_FreeFallRelease( u8 atkPokeID, bool bAppearSelf, bool bAppearTarget );
  void                        Hnd_FreeFallAppearTarget( u8 targetPokeID );
  bool                        Hnd_UseItemEquip( BTL_POKEPARAM* bpp, u8* fFailMsgDisped );
  void                        Hnd_EventSkillCall(  const BTL_POKEPARAM* bpp );
  bool                        Hnd_CheckForceNigeru( u8 pokeID );
  bool                        Hnd_IsMegaEvolveItem( MonsNo mons_no, u16 item_no );
  void                        Hnd_EnableWazaSubEffect( u8 pokeID );
  void                        Hnd_PlayWazaEffect( BtlPokePos atkPos, BtlPokePos defPos, WazaID waza, BtlvWazaEffect_TurnType turnType, bool bSyncEffect );
  bool                        Hnd_IsSkyBattleMode( void ) const;
  bool                        Hnd_CheckOccurAdditionalEffect( u8 atkPokeID, u8 defPokeID, u32 defaultPer );
  void                        Hnd_NotifyRemovePosEffect( BtlPosEffect effect, BtlPokePos pos );
  void                        Hnd_UpdatePosEffectParam( BtlPosEffect effect, BtlPokePos pos, const PosEffect::EffectParam& effectParam );
  const PosEffect::EffectParam& Hnd_GetPosEffectParam( BtlPosEffect effect, BtlPokePos pos ) const;
  bool                        Hnd_CheckActionRecord( u8 pokeId, u8 backTurnCount, ActionRecorder::ActionID actionId ) const;
  bool                        Hnd_GetStatusFlag( BTL_STATUS_FLAG statusFlag ) const;
  bool Hnd_InsertWazaAction( u8 pokeId, WazaNo wazaNo, BtlPokePos targetPos, bool isZenryokuWaza, const ActionDesc& actionDesc, uint infoID );
  const RoyalRankingContainer& Hnd_GetRoyalRaningContainer( void ) const;
  s16                         Hnd_GetSpecificSickFailStrID( WazaSick sickID ) const;




  //-----------------------------------------------------
  //-----------------------------------------------------
  const AffCounter&   GetAffCounter( void ) const { return m_affCounter; }
  const EscapeInfo*   GetEscapeInfoPointer( void ) const;
  BtlPokePos          GetCapturedPokePos( void ) const;


  bool    StartBtlIn( void );
  void    StartTurn_Boot( void );
  Result  StartTurn( const SVCL_ACTION*  clientAction );
  void    StartAfterPokeIn_Boot( void );
  Result  StartAfterPokeIn( const SVCL_ACTION* clientAction );
  void    ContinueAfterPokeChange_Boot( void );
  Result  ContinueAfterPokeChange( const SVCL_ACTION* clientAction );
  void    IntrudePokeIn( void );

  void*   HANDEX_Push( EventHandlerExhibition eq_type, u8 userPokeID );
  void    HANDEX_PushRun( EventHandlerExhibition eq_type, u8 userPokeID );
  void    HANDEX_Pop( void* handExWork );
  HandExResult  HandEx_Result( void );

  void       SetTimeLimit( const TimeLimit& timeLimit );
  BtlResult  ChecBattleResult( ResultCause* judgeCause ) const;
  bool       MakePlayerEscapeCommand( void );

  //BTL_SVF_SickDamageRecall
  void         SickDamageRecall( BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage );

  void         TurnCheckCallback_Field( FieldStatus::EffectType effect );


  u32          GetDeadPokeCountByClientID( u8 clientID ) const;


  // 乱入
  BtlIntrudeType GetIntrudeType( void ) const;
  BtlPokePos GetIntrudePokePos( void ) const;
  const pml::pokepara::PokemonParam& GetIntrudePokeParam( void ) const;


private:
  /**
   *  定数群
   */
  enum {
    EVENT_WORK_DEPTH      = 16,
    EVENT_WORK_TOTAL_SIZE = 512,

    HANDLER_EXHIBISION_WORK_TOTALSIZE = 500,    ///< ハンドラ反応情報を格納するワークのトータルサイズ
    NIGERU_COUNT_MAX = 30,                      ///< 「にげる」選択回数カウンタの最大値
    MEMBER_CHANGE_COUNT_MAX = 255,              ///< 入れ替えカウンタ最大値（バトル検定用）
    AFFCOUNTER_MAX = 9999,                      ///< 相性カウンタ最大値（バトル検定用）
    EVENT_HANDLER_TMP_WORK_SIZE = 320,          ///< ハンドラが一時的に利用するバッファのサイズ
  };

  typedef enum {

    SV_REACTION_NONE = 0,   ///< 反動なし
    SV_REACTION_DAMAGE,     ///< 与えたダメージの割合による反動
    SV_REACTION_HP,         ///< 自分の最大HPの割合による反動

  }ReactionType;

  /**
   *  ふきとばし系のワザ処理パターン
   */
  typedef enum {
    PUSHOUT_EFFECT_ESCAPE,    ///< 逃げたのと同じ（戦闘を終了する）
    PUSHOUT_EFFECT_CHANGE,    ///< 相手を入れ替えさせる（手持ちがいなければ失敗する）
    PUSHOUT_EFFECT_MUSTFAIL,  ///< 必ず失敗する
  }PushOutEffect;

  /**
   *  ダメージ計算結果格納用ワーク
   */
  typedef struct {

    u8 real_hit_count;
    u8 migawari_hit_count;
    u8 total_hit_count;

    fx32  dmg_ratio;

    struct {

      u16         damage;
      u8          pokeID;
      u8          affine;     ///< ダメージ相性（ BtlTypeAff @ btl_calc.h ）
      u8          koraeru;    ///< こらえ原因（ KoraeruCause @ btl_pokeparam.h )
      u8          fCritical  : 1;
      u8          fFixDamage : 1;
      u8          fMigawari  : 1;
      u8          fCriticalByFriendship  : 1;


    }record[ BTL_POS_NUM ];

  } CALC_DAMAGE_REC;

  /**
   *  ワザエフェクト発動管理
   */
  struct WAZAEFF_CTRL{
    WazaNo effectWazaID;   ///<
    u8     attackerPos;    ///< 発動開始位置
    u8     targetPos;      ///< 対象位置（位置が明確でないワザは BTL_POS_NULL ）
    u8     effectIndex;    ///< エフェクトIndex
    u8     fEnable  : 1;    ///< 発動確定フラグ
    u8     fDone    : 1;    ///< 発動確定フラグ
    u8     _pad     : 6;

    union {
      u32      Raw;

      struct  {
        u32    pokeCnt  : 5; ///< サブ演出（ワザエフェクト前の木の実演出）ポケ数
        u32    pokeID_1 : 5;
        u32    pokeID_2 : 5;
        u32    pokeID_3 : 5;
        u32    pokeID_4 : 5;
        u32    pokeID_5 : 5;
        u32    pad      : 2;
      };

    } subEff;

  };

  /**
   *  ワザ乗っ取りパラメータ
   */
  typedef struct {
    u8  robberCount;                    ///< 乗っ取るポケモン数
    u8  robberPokeID[ BTL_POS_NUM ];    ///< 乗っ取り側ポケモンID
    u8  targetPos[ BTL_POS_NUM ];
    u8  targetPokeID[ BTL_POS_NUM ];
  }WAZA_ROB_PARAM;

  /**
   * ヒットチェックパラメータ
   */
  typedef struct {

    u8    countMax;         ///< 最大ヒット回数
    u8    count;            ///< 現在処理している回数

    u8    fCheckEveryTime;  ///< 毎回、ヒットチェックを行うか
    u8    fPluralHitWaza;   ///< 複数回ヒットするワザか
    u8    fPutEffectCmd;    ///< ワザエフェクト生成済み
    u8    pluralHitAffinity;

  }HITCHECK_PARAM;

  /**
   *  他ワザ呼び出しパラメータ
   */
  typedef struct {
    WazaNo      wazaID;
    BtlPokePos  targetPos;
  }REQWAZA_WORK;

  /**
   * １アクションごと必要なパラメータスタック構造１件分
   */
  typedef struct {

    PokeSet           TargetOrg;
    PokeSet           Target;
    PokeSet           Friend;
    PokeSet           Enemy;
    PokeSet           Damaged;
    PokeSet           RobTarget;
    PokeSet           psetTmp;
    WAZAEFF_CTRL      effCtrl;
    WAZAPARAM         wazaParam;
    WAZAPARAM         wazaParamOrg;
    HITCHECK_PARAM    hitCheck;
    WAZA_ROB_PARAM    wazaRobParam;
    WAZA_ROB_PARAM    magicCoatParam;
    CALC_DAMAGE_REC   calcDmgFriend;
    CALC_DAMAGE_REC   calcDmgEnemy;
    BtlPokePos        defaultTargetPos;
    u8                fMemberChangeReserve;
    u8                fWazaFailMsgDisped;

  }POKESET_STACK_UNIT;

  /**
   *  戦闘に出ているポケモンデータに順番にアクセスする処理のワーク
   */
  typedef struct {
    u8 clientIdx;
    u8 pokeIdx;
    u8 endFlag;
  }FRONT_POKE_SEEK_WORK;

  /**
   *  ClientID 記録
   */
  typedef struct {
    u8  count;
    u8  clientID[ BTL_CLIENT_MAX ];
  }CLIENTID_REC;

  /**
   * 複数フラグを u32 で管理
   */
  typedef union {
    u32 raw;

    struct {
      u32 hitPluralPoke : 1;   ///< 複数ポケモンにヒットする
      u32 delayAttack   : 1;   ///< みらいよち等、遅延攻撃
      u32 _pad          : 29;
    };

    struct {
      u32 magicCoat    : 1;  ///< マジックコート跳ね返し攻撃中
      u32 yokodori     : 1;  ///< 横取り実行中
      u32 reqWaza      : 1;  ///< 派生ワザ呼び出し中
    };

  }FLAG_SET;


  /**
   *  ワザエフェクトのコマンド領域を予約し、その予約位置情報を記録する
   */
  struct WAZAEFF_RESERVE_POS{
    enum {
      INVALID_VALUE = 0xffff,
    };
    u16   prevEffectCmd;
    u16   mainEffectCmd;
    WAZAEFF_RESERVE_POS( void )
    {
      prevEffectCmd = INVALID_VALUE;
      mainEffectCmd = INVALID_VALUE;
    };

    bool IsValid( void ) const
    {
      return mainEffectCmd != INVALID_VALUE;
    }
  };


  /**
   *  命令無視パターン
   */
  typedef enum {

    SABOTAGE_NONE = 0,    ///< 命令無視しない
    SABOTAGE_OTHER_WAZA,  ///< 他のワザを勝手に出す
    SABOTAGE_GO_SLEEP,    ///< 眠ってしまう
    SABOTAGE_CONFUSE,     ///< 自分を攻撃
    SABOTAGE_DONT_ANY,    ///< 何もしない（メッセージランダム表示）
    SABOTAGE_SLEEPING,    ///< 眠ったまま命令無視（メッセージ表示）

  }SabotageType;

  /**
   *  溜めワザ判定結果
   */
  typedef enum {

    TAMEWAZA_NONE = 0,      ///< 溜めワザではない
    TAMEWAZA_START_FAIL,    ///< 溜め開始ターン：失敗
    TAMEWAZA_START_OK,      ///< 溜め開始ターン：成功
    TAMEWAZA_RELEASE_FAIL,  ///< 溜め解放ターン：失敗
    TAMEWAZA_RELEASE_OK,    ///< 溜め解放ターン：成功

  }TameWazaResult;
  

  /**
   * @brief ターンチェックの結果
   */
  enum TurnCheckResult
  {
    TURNCHECK_RESULT_DONE,       // ターンチェック終了
    TURNCHECK_RESULT_LEVELUP,    // レベルアップが起こった
    TURNCHECK_RESULT_POKECHANGE, // ポケモン入れ替えが起こった
    TURNCHECK_RESULT_QUIT,       // バトルを離脱した
  };

  /**
   *  BitFlags
   */
  typedef enum {

    FLOWFLG_FIRST_FIGHT,
    FLOWFLG_SET_WAZAEFFECT,
    FLOWFLG_MAX,
    FLOWFLG_BYTE_MAX = ((FLOWFLG_MAX/4)+((FLOWFLG_MAX&3)!=0)*4),

  }FlowFlag;

  /**
   *  イベント - ハンドラ間 連絡ワーク領域
   */
  class EventWorkStack {
  public:
    EventWorkStack( void ){};
    ~EventWorkStack( void ){};

    void   Init( void );
    void*  Push( u32 size );
    void   Pop( void* adrs );
  private:
    u8    m_work[ EVENT_WORK_TOTAL_SIZE ];
    u16   m_size[ EVENT_WORK_DEPTH ];
    u32   m_sp;
  };

  
  // 乱入に関するパラメータ
  struct IntrudeParam
  {
    BTL_CLIENT_ID                callClientID;     // 助けを呼ぶクライアントのID
    u8                           callPokeID;       // 助けを呼ぶポケモンのID
    BtlPokePos                   callPokePos;      // 助けを呼ぶポケモンの立ち位置
    BtlPokePos                   appearPokePos;    // 助けに来たポケモンの立ち位置
    pml::pokepara::PokemonParam* appearPokeParam;  // 助けに来たポケモンのパラメータ
    BtlIntrudeType               intrudeType;      // 乱入の種類( 乱入 or 援軍 )
  };
  void IntrudeParam_Clear( IntrudeParam* param ) const;



  BTL_SERVER*                    m_server;
  const MainModule*              m_mainModule;
  POKECON*                       m_pokeCon;
  SCQUE*                         m_que;
  HeapHandle                     m_heapHandle;
  u32                            m_turnCount;
  Result                         m_flowResult;
  FieldStatus*                   m_fieldStatus;
  WazaRec                        m_wazaRec;
  DeadRec                        m_deadRec;
  PartyAllDeadRecorder           m_partyAllDeadRecorder;
  WAZAEFF_CTRL*                  m_wazaEffCtrl;
  HITCHECK_PARAM*                m_hitCheckParam;
  WAZA_ROB_PARAM*                m_wazaRobParam;
  WAZA_ROB_PARAM*                m_magicCoatParam;
  EscapeInfo                     m_escInfo;
  TimeLimit                      m_timeLimit;
  PokemonBattleInRegister        m_pokemonBattleInRegister;
  RoyalRankingContainer          m_royalRankingContainer;
  IntrudeSystem*                 m_intrudeSystem;
  ReinforceSystem*               m_reinforceSystem;
  BattleResultJudge*             m_resultJudge;
  ActionRecorder*                m_actionRecorder;
  
  IntrudeParam                   m_intrudeParam;
  BTL_LEVELUP_INFO               m_lvupInfo;
  CLIENTID_REC                   m_clientIDRec;
  u32                            m_simulationCounter;
  u32                            m_wazaRankEffSerial;
  u8                             m_cmdBuildStep;
  u8                             m_actOrderStep;
  u8                             m_turnCheckStep;
  u8                             m_defaultTargetPos;

  u8      m_numActOrder;
  u8      m_numEndActOrder;
  u8      m_recalcActOrderIndex;
  bool    m_recalcActOrderNeed;
  u8      m_getPokePos;
  u8      m_nigeruCount;
  u8      m_wazaEffIdx;
  u8      m_MemberOutIntrPokeCount;
  u8      m_thruDeadMsgPokeID;
  u8      m_fQuitBattleRequest    : 1;
  u8      m_fMemberOutIntr        : 1;
  u8      m_fWinBGMPlayWild       : 1;
  u8      m_fEscMsgDisped         : 1;
  u8      _padding                : 1;
  u8      m_fWazaFailMsgDisped    : 1;
  u8      m_fKaifukuFujiMsgDisped : 1;
  u8      m_fWazaFailMsgRoundUp   : 1;

  u8*     m_fMemberChangeReserve;

  #if PM_DEBUG
  u8      m_fDebugClack           : 1;
  u8      m_fDebugDummy           : 7;
  #endif

  u8      m_MemberOutIntrPokeID[ BTL_POS_NUM ];
  u8      m_pokeInFlag [ BTL_POKEID_MAX ];
  u8      m_memberChangeCount[ BTL_CLIENT_MAX ];

  // ポケモン1体ごとに最大6アクション必要
  // 内訳は以下の通り
  // ・選択した行動(1)
  // ・メガ進化(1)
  // ・「さいはい」による追加技出し(1)
  // ・「おどりこ」による追加技出し(3)
  static const u8 MAX_ACT_ORDER_NUM = BTL_POS_NUM * 6;
  ACTION_ORDER_WORK   m_actOrder[ MAX_ACT_ORDER_NUM ];
  ACTION_ORDER_WORK   m_actOrderTmp;

  PosPoke             m_posPoke;
  // POKESET の STACK機構用（割り込みが発生するので actOrder 件数+1 分必要）
  PokeSet*             m_psetTargetOrg;
  PokeSet*             m_psetTarget;
  PokeSet*             m_psetFriend;
  PokeSet*             m_psetEnemy;
  PokeSet*             m_psetDamaged;
  PokeSet*             m_psetRobTarget;
  PokeSet*             m_psetTmp;
  CALC_DAMAGE_REC*     m_calcDmgFriend;
  CALC_DAMAGE_REC*     m_calcDmgEnemy;
  POKESET_STACK_UNIT   m_pokesetUnit[ BTL_POS_NUM+1 ];
  u32                  m_pokesetStackPtr;
  PokeSet              m_pokesetMemberInProc;
  WAZAPARAM*           m_wazaParam;
  WAZAPARAM*           m_wazaParamOrg;
  EventWorkStack       m_eventWork;
  ServerFlowSub        m_flowSub;
  pml::pokepara::EvolveManager   m_evolveManager;

  BTL_HANDEX_STR_PARAMS         m_strParam;
  DmgAffRec                     m_dmgAffRec;
  SabotageType                  m_currentSabotageType;
  HEManager                     m_HEManager;
  ServerFlowSub::CALC_EXP_WORK  m_calcExpWork[ BTL_PARTY_MEMBER_MAX ];
  ServerFlowSub::CALC_EXP_WORK  m_calcExpResult[ BTL_PARTY_MEMBER_MAX ];
  ServerFlowSub::POKE_CAPTURED_CONTEXT  m_capturedContext;

  AffCounter                    m_affCounter;
  WazaID                        m_prevExeWaza;
  bool                          m_isPrevExeZenryokuWaza;

  u8          m_flowFlags[ FLOWFLG_BYTE_MAX ];
  u8          m_handlerTmpWork[ EVENT_HANDLER_TMP_WORK_SIZE ];

  SimpleEffectFailManager   m_simpleEffectFailManager;
  InsertWazaInfoStorage     m_insertWazaInfoStorage;
  DamageProcParamStack      m_damageProcParamStack;

  u16    m_itemChangeCounter[ BTL_POKEID_MAX ];
  // ロトム関連
  bool m_isRecoverZenryoku;
  bool m_isUsedItemPon;
  bool m_isUsedTukamaePon;
  // @momiji追加分
  void UseItemPon(void);///<～ポン使用
  bool IsUsedItemPon(void) const;///<～ポン使用しているか
  fx32 RotomPowerCalcCapture(fx32 ratio_fx) const;///<ロトムパワーによる捕獲率補正
  void UseTukamaePon(void);///<つかまえポン使用
  bool IsUsedTukamaePon(void)const;///<つかまえポン使用しているか
  //-------------------------------------------------------
  void               clearWorks( void );

  void               PSetStack_Init( void );
  void               PSetStack_Push( void );
  void               PSetStack_Pop( void );
  void               psetstack_setup( u32 sp, bool fClear );

  void               FRONT_POKE_SEEK_InitWork( FRONT_POKE_SEEK_WORK* fpsw );
  bool               FRONT_POKE_SEEK_GetNext( FRONT_POKE_SEEK_WORK* fpsw, BTL_POKEPARAM** bpp );

  u8            getFriendship( const BTL_POKEPARAM* bpp );
  bool          checkDeadCmdPlural( BTL_POKEPARAM** bpp, u32 poke_cnt, u8* bDead );
  void          scproc_CheckDeadCmdAfterForPlural( BTL_POKEPARAM** bpp, u32 poke_cnt, u8* bDead );



  //@バトル変更箇所 失敗処理の分岐のため戻り値の追加
  ChangeWeatherResult scproc_ChangeWeather( BtlWeather weather, u8 turn, u8 turnUpCount, u8 causePokeID );
  ChangeWeatherResult scproc_ChangeWeatherCheck( BtlWeather weather, u8 turn );
  void scproc_ChangeWeatherCore( BtlWeather weather, u8 turn, u8 turnUpCount, u8 causePokeID );
  void          scproc_ChangeWeatherAfter( BtlWeather weather );
  void          scproc_MemberInCore( u8 clientID, u8 posIdx, u8 nextPokeIdx );
  bool          scproc_AfterMemberIn( void );
  HandExResult  scproc_HandEx_Result( void );
  bool          scproc_NigeruCmd_Root( BTL_POKEPARAM* bpp );
  void          scproc_MemberChange( BTL_POKEPARAM* outPoke, u8 nextPokeIdx );
  bool          scproc_MemberOutForChange( BTL_POKEPARAM* outPoke, bool fIntrDisable );
  void          scproc_MemberInForChange( u8 clientID, u8 posIdx, u8 next_poke_idx, bool fPutMsg );
  void          scproc_MemberInForCover( u8 clientID, u8 posIdx, u8 next_poke_idx, bool fPutMsg );
  bool          scproc_CheckExpGet( void );
  bool          scproc_CheckExpGetByCaptured( u8 playerClientID );
  void          scproc_DisplayOteireButton( void );
  void          scproc_CreateFurimukiCmd_FrontPoke( u8 playerClientID );
  void          scproc_MemberOutCore( BTL_POKEPARAM* outPoke, u16 effectNo );
  void          scproc_ClearPokeDependEffect( BTL_POKEPARAM* poke );
  void          scproc_FreeFall_CheckRelease( BTL_POKEPARAM* bpp, bool bAppearSelf, bool bAppearTarget );
  bool          scproc_CheckShowdown( void ) const;
  bool          scproc_IsTargetRemovedAll( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, const PokeSet* targets );
  void          scproc_AfterTokuseiChanged_Event( const BTL_POKEPARAM* bpp );
  void          scproc_AfterTokuseiChanged_Item( BTL_POKEPARAM* bpp, TokuseiNo prevTokusei, TokuseiNo nextTokusei );
  void          scproc_KintyoukanMoved( const BTL_POKEPARAM* bppMoved );
  void          scproc_CheckItemReaction( BTL_POKEPARAM* bpp, BtlItemReaction reactionType );
  bool          scproc_GetExp( BTL_PARTY* party, const ServerFlowSub::CALC_EXP_WORK* calcExp );
  void          scproc_AddExp( BTL_POKEPARAM* poke, u32 exp, BTL_LEVELUP_INFO* info );
  BTL_POKEPARAM* checkRightsOfGettinExp( BTL_PARTY* party, const ServerFlowSub::CALC_EXP_WORK* calcExp, u32 idx, u32* exp, bool *bGakusyuSoutiOnly );
  void          scproc_ActStart( const BTL_POKEPARAM* bpp, const BTL_ACTION_PARAM* action );
  void          scproc_BeforeFirstFight( void );
  void          scproc_Fight( BTL_POKEPARAM* attacker, BTL_ACTION_PARAM* action, ActionDesc* actionDesc, u32 handlerSubPri, int wazaPri );
  void          scproc_MegaEvo( BTL_POKEPARAM* poke );
  void          scproc_MoveCore( u8 clientID, u8 posIdx1, u8 posIdx2, bool fResetMove );
  void          scproc_AfterMove( u8 clientID, u8 posIdx1, u8 posIdx2 );
  void          scproc_AfterMoveCore( const BTL_POKEPARAM* bpp );
  TurnCheckResult scproc_TurnCheck( void );
  void          scproc_turncheck_friendship_cure( PokeSet* pokeSet );
  bool          scproc_turncheck_weather( PokeSet* pokeSet );
  bool          scproc_turncheck_sub( PokeSet* pokeSet, BtlEventType event_type );
  bool          scproc_turncheck_sick( PokeSet* pokeSet );
  void          scproc_turncheck_side( void );
  void          scproc_turncheck_field( void );
  bool          scproc_IntrudeCall( void );
  void          scproc_IntrudeCall_Help( bool* doCall, bool* doAppear, IntrudeParam* intrudeParam, BTL_CLIENT_ID callClientId );
  void          scproc_IntrudeCall_Reinforce( bool* doCall, bool* doAppear, IntrudeParam* intrudeParam, BTL_CLIENT_ID callClientId );
  void          scproc_IntrudeAppear( void );
  bool          scproc_SimpleDamage_CheckEnable( BTL_POKEPARAM* bpp, u32 damage, DamageCause damageCause );
  void          scproc_ViewEffect( u16 effectNo, BtlPokePos pos_from, BtlPokePos pos_to, bool fQueResereved, u32 reservedPos );
  bool scproc_SimpleDamage_Core( BTL_POKEPARAM* bpp, u32 damage, DamageCause damageCause, u8 damageCausePokeID, BTL_HANDEX_STR_PARAMS* str, bool doDeadProcess );

  bool          scproc_CheckDeadCmd( BTL_POKEPARAM* poke, bool bPluralMode, const MainModule::PGLRecParam* = NULL );
  bool          scproc_CheckFloating( const BTL_POKEPARAM* bpp, bool fHikouCheck );
  void scproc_WazaCall_Decide( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParamOrg, const WAZAPARAM* wazaParamAct );
  void          scproc_WazaExe_Decide( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, BtlEventType evType );
  void          scproc_EndWazaSeq( const BTL_POKEPARAM* attacker, WazaID waza, bool fWazaEnable, const ActionDesc* actionDesc );
  void          scproc_WazaNoEffectByFlying( const BTL_POKEPARAM* bpp );
  void          scproc_Fight_DamageProcStart( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, PokeSet* targets );
  void          scproc_AfterItemEquip( BTL_POKEPARAM* bpp, u16 itemID, bool bCheckKinomiAte );
  void          scproc_CallAfterItemEquipEvent( BTL_POKEPARAM* bpp, u16 itemID, bool bCheckKinomi );
  bool          scproc_UseItemEquip( BTL_POKEPARAM* bpp );
  void          scproc_ItemChange( BTL_POKEPARAM* bpp, u16 nextItemID, bool fConsume );
  bool scproc_RecoverHP( BTL_POKEPARAM* bpp, u16 recoverHP, bool fSkipSpFailCheck, bool fDispFailMsg, bool isEffectEnable );
  bool          scproc_RecoverHP_CheckFailBase( const BTL_POKEPARAM* bpp );
  bool          scproc_RecoverHP_CheckFailSP( const BTL_POKEPARAM* bpp, bool fDispFailMsg );
  void scproc_RecoverHP_Core( BTL_POKEPARAM* bpp, u16 recoverHP, bool isEffectEnable );
  bool          scproc_DrainCore( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 drainHP, bool fSkipSpFailCheck );
  bool          scproc_decrementPP( BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume );
  bool scproc_AddSickCheckFail( BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
                  WazaSick sick, const BTL_SICKCONT& sickCont, SickCause cause, u32 wazaSerial, SickOverWriteMode overWriteMode, bool fDispFailResult, bool fOtherEffectEnabled );
  void          scproc_AddSickCore( BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
                  WazaSick sick, const BTL_SICKCONT& sickCont, bool fDefaultMsgEnable, bool fItemReactionDisable, const BTL_HANDEX_STR_PARAMS* exStr );
  bool scproc_RankEffectCore_CheckEffective( u8 atkPokeID, BTL_POKEPARAM* target, WazaRankEffect effect, int volume, RankEffectCause cause, u8 wazaUsePokeID, u16 itemID, u32 rankEffSerial, bool fAlmost, bool bMigawariThrew, bool bCheckOnly );
  bool          scproc_RankEffectCore( u8 atkPokeID, BTL_POKEPARAM* target, WazaRankEffect effect, int volume, RankEffectCause cause, u8 wazaUsePokeID, u16 itemID, u32 rankEffSerial, bool fAlmost, bool fStdMsg, const BTL_HANDEX_STR_PARAMS* preMsg, bool bMigawariThrew, bool fViewEffect );
  void scproc_KillPokemon(BTL_POKEPARAM* bpp, u8 atkPokeID, DamageCause deadCause,  const MainModule::PGLRecParam* recParam, bool doDeadProcess );
  bool          scproc_FieldEffectCore( FieldStatus::EffectType effect, const BTL_SICKCONT& contParam, bool fAddDependTry );
  bool          scproc_ChangeGround( u8 pokeID, BtlGround ground, const BTL_SICKCONT& contParam );
  void          scproc_FieldEff_End( FieldStatus::EffectType effect );
  bool          scproc_NigeruCmdSub( BTL_POKEPARAM* bpp, bool fSkipNigeruCalc, bool forceNigeru );
  bool          scproc_checkForceNigeru( const BTL_POKEPARAM* bpp );
  bool          scproc_NigeruCore( BTL_POKEPARAM* bpp, bool fForceNigeru, bool fUseItem );
  void          scproc_checkNoEffect_sub( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec* affRec, BtlEventType eventID,  bool bCheckMustHit );
  bool          scproc_checkNoEffect_core( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, const DmgAffRec* affRec, BtlEventType eventID );
  bool          scproc_checkNoEffect_typeSp( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
  bool          scproc_checkNoEffect_SimpleSick( WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
  void scproc_checkMamoruNotEffect( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec* affRec );

  void          scproc_Fight_Damage_Root( const ActionDesc* actionDesc, const WAZAPARAM* wazaParam,
                  BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec& affRec, bool fDelayAttack );
  BtlTypeAff    scProc_checkWazaDamageAffinity( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam, bool fOnlyAttacker, bool* isNoEffectByFloatingStatus );
  BtlTypeAff    scproc_RewiteWazaAffinityByFloatingStatus( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, pml::PokeType wazaType, BtlTypeAff affinity, bool* isNoEffectByFloatingStatus );
  bool          scproc_AddShrinkCore( BTL_POKEPARAM* target, u32 per );
  void          scproc_Fight_Damage_Drain( WazaID waza, BTL_POKEPARAM* attacker, PokeSet* targets );
  void          scproc_Damage_Drain( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage );
  bool          scproc_PushOutCore( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
                  bool fForceChange, bool* fFailMsgDisped, u16 effectNo, bool fIgnoreLevel, BTL_HANDEX_STR_PARAMS* succeedMsg );
  bool          scproc_TameHideCancel(BTL_POKEPARAM* bpp, BTL_POKEPARAM::ContFlag hideContFlag , bool bOmitCancelAction);
  SV_WazaFailCause scproc_Fight_CheckReqWazaFail( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam );
  void          scproc_WazaExecuteFailed( BTL_POKEPARAM* attacker, WazaID waza, bool isZenryokuWaza, SV_WazaFailCause fail_cause );
  u32           scproc_CalcConfDamage( BTL_POKEPARAM* attacker );
  void          scproc_decrementPPUsedWaza( BTL_POKEPARAM* attacker,
                  WazaID waza, u8 wazaIdx, PokeSet* rec );
  void          scproc_Fight_Damage_Determine( BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam );
  void          scproc_Koraeru( BTL_POKEPARAM* bpp, BTL_POKEPARAM::KoraeruCause cause );
  void          scproc_MagicCoat_Root( WazaID actWaza );
  void          scproc_WazaExeRecordUpdate( const BTL_POKEPARAM* bpp, WazaID waza );
  TameWazaResult  scproc_Fight_TameWazaExe( BTL_POKEPARAM* attacker, const PokeSet* targetRec, const WAZAPARAM* wazaParam );
  bool          scproc_TameStartTurn( BTL_POKEPARAM* attacker, BtlPokePos atPos, const PokeSet* targetRec, const WAZAPARAM* wazaParam );
  void          scproc_TameLockClear( BTL_POKEPARAM* attacker );
  void          scproc_CheckMovedPokeAvoid( const BTL_POKEPARAM* attacker, PokeSet* targetRec, const WAZAPARAM* wazaParam, bool fDamageWaza );
  void          scproc_WazaExe_Effective( u8 pokeID, WazaID waza, const ActionDesc& actionDesc );
  void          scproc_WazaExe_NotEffective( u8 pokeID, WazaID waza, const ActionDesc& actionDesc );
  void          scproc_WazaExe_Done( u8 pokeID, WazaID waza, const ActionDesc& actionDesc );
  void scproc_WazaExe_SetDirt( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, PokeSet* targetRec );
  void scproc_SetDirt( BTL_POKEPARAM* poke, const WAZAPARAM* wazaParam );
  void          scproc_MigawariExclude( const WAZAPARAM* wazaParam,
                  const BTL_POKEPARAM* attacker, PokeSet* target, bool fDamage );
  void          scproc_Fight_SimpleEffect( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targetRec );
  bool scproc_WazaRankEffect_Common( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u32 wazaSerial, bool fAlmost, bool fOnlyCheck );
  void          scproc_Fight_SimpleSick( WazaID waza, BTL_POKEPARAM* attacker, PokeSet* targetRec );
  bool scproc_Fight_WazaSickCore( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
                  WazaID waza, WazaSick sick, BTL_SICKCONT sickCont, SickCause cause, u32 wazaSerial, bool fAlmost, bool fOtherEffectEnabled );
  bool scproc_AddSickRoot( BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
                  WazaSick sick, const BTL_SICKCONT& sickCont, SickCause sickCause, u32 wazaSerial, bool fAlmost, bool fOtherEffectEnabled, bool fDefaultMsgEnable );
  void          scproc_Fight_EffectSick( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targetRec );
  void          scproc_Fight_Ichigeki( const ActionDesc* actionDesc, const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets );
  void scproc_Ichigeki_Succeed( BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, BtlTypeAffAbout affAbout );
  void scEvent_IchigekiSucceed( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
  void scproc_Ichigeki_Korae( BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam,
                  BtlTypeAffAbout affAbout, BTL_POKEPARAM::KoraeruCause korae_cause, u16 damage );
  u16          scproc_Ichigeki_Migawari( BTL_POKEPARAM* target, const WAZAPARAM* wazaParam, BtlTypeAffAbout affAbout );
  bool         scproc_Migawari_Create( BTL_POKEPARAM* bpp );
  u16          scproc_Migawari_Damage( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 damage,
                 BtlTypeAff aff, CriticalType fCritical, const WAZAPARAM* wazaParam );
  void         scproc_Migawari_Delete( BTL_POKEPARAM* bpp );
  void         scproc_WazaAdditionalEffect( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage, bool fMigawriHit );
  void         scproc_WazaAdditionalEffect_User( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, bool fMigawriHit );
  void         scproc_WazaDamageSideAfter( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, u32 damage );
  bool         scproc_Fight_WazaExe( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, PokeSet* targetRec, const ActionDesc* actionDesc );
  void         scproc_Fight_Damage_AddEffect( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
  void         scproc_Fight_Damage_AddSick( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
  void         scproc_Fight_PushOut( WazaID waza, BTL_POKEPARAM* attacker, PokeSet* targetRec );
  void         scproc_Fight_SimpleRecover( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targetRec );
  void         scput_Fight_FieldEffect( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker );
  void         scput_Fight_Uncategory( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets );
  void         scput_Fight_Uncategory_SkillSwap( BTL_POKEPARAM* attacker, PokeSet* targetRec );
  void         scproc_WazaDamageReaction( BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender,
                 const WAZAPARAM* wazaParam, BtlTypeAff affinity, u32 damage, bool critical_flag, bool fMigawari );
  bool         scproc_FreeFall_Start( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam, bool* fFailMsgDisped );
  bool         scproc_FreeFall_Start_CheckGuard( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam );
  void         scproc_CheckShrink( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender );
  void         scproc_Fight_Damage_KoriCure( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets );
  void         scproc_Fight_Damage_Kickback( BTL_POKEPARAM* attacker, WazaID waza, u32 wazaDamage );
  bool         scproc_Fight_CheckCombiWazaReady( BTL_POKEPARAM* attacker, WazaID waza, BtlPokePos targetPos );
  bool scproc_Fight_CheckDelayWazaSet( BTL_POKEPARAM* attacker, WazaID waza, BtlPokePos targetPos, u8* fWazaEnable, ZenryokuWazaKind zWazaKind );
  void         scproc_Fight_DecideDelayWaza( const BTL_POKEPARAM* attacker );
  void         scproc_StartWazaSeq( const BTL_POKEPARAM* attacker, WazaID waza );
  bool         scproc_Check_WazaRob( const BTL_POKEPARAM* attacker, WazaID waza, bool isZenryokuWaza, const PokeSet* rec, WAZA_ROB_PARAM* robParam );
  bool scproc_Fight_CheckWazaExecuteFail_1st( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, u8 fWazaLock, const PokeSet* targets, const ActionDesc* actionDesc );
  SV_WazaFailCause scproc_Fight_CheckWazaExecuteFail_1st_sub( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, u8 fWazaLock, const PokeSet* targets, const ActionDesc* actionDesc );
  void          scproc_CheckWazaExe_NemuriCure( BTL_POKEPARAM* attacker, WazaID waza );
  bool          scproc_CheckWazaExe_KooriCure( BTL_POKEPARAM* attacker, WazaID waza );
  bool          scEvent_IsWazaMeltMustFail( const BTL_POKEPARAM* attacker, WazaID waza );
  bool          scproc_Fight_CheckConf( BTL_POKEPARAM* attacker );
  bool          scproc_Fight_CheckMeroMero( BTL_POKEPARAM* attacker );
  bool          scproc_Fight_CheckWazaExecuteFail_Funjin( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam );
  bool          scproc_Fight_CheckWazaExecuteFail_2nd( BTL_POKEPARAM* attacker, WazaID waza , const WAZAPARAM* wazaParam, const PokeSet* targets );
  bool          scproc_Fight_CheckWazaExecuteFail_3rd( BTL_POKEPARAM* attacker, WazaID waza , const WAZAPARAM* wazaParam, const PokeSet* targets );
  void          scproc_WazaRobRoot( BTL_POKEPARAM* attacker, WazaID actWaza, bool isZenryokuWaza );
  u32           scproc_Fight_Damage_SingleCount( const WAZAPARAM* wazaParam,
                  BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec& affRec, WAZAEFF_RESERVE_POS* que_reserve_pos, bool fDelayAttack );
  u32           scproc_Fight_Damage_PluralCount(const WAZAPARAM* wazaParam,
                  BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec& affRec , WAZAEFF_RESERVE_POS* que_reserve_pos);
  u32           scproc_Fight_Damage_side( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker,
                  PokeSet* targets, const CALC_DAMAGE_REC* dmgRec, HITCHECK_PARAM* hitCheckParam, FLAG_SET flagSet );
  u32           scproc_Fight_damage_side_core(
                  BTL_POKEPARAM* attacker, PokeSet* targets, const CALC_DAMAGE_REC* dmgRec,
                  const WAZAPARAM* wazaParam, HITCHECK_PARAM* hitCheckParam, fx32 dmg_ratio, FLAG_SET flagSet );
  void          scproc_checkAttackerDead( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam );
  void          scproc_PrevWazaDamage( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, u32 poke_cnt, BTL_POKEPARAM** bppAry );
  void          scproc_Fight_Damage_ToRecover( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, PokeSet* targets );
  void          scproc_Fight_DamageProcEnd( const ActionDesc* actionDesc, const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets, u32 dmgTotal, bool fDelayAttack );
  void          scproc_Fight_Damage_FriendActPinch( PokeSet* targets );
  void          scproc_Fight_UnDamageProcEnd( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker );
  void          scproc_MamoruSuccess( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam );
  void          scproc_AddEffortPower( BTL_POKEPARAM* bpp, const ServerFlowSub::CALC_EXP_WORK* calcExp );
  void          scproc_WazaMsg( const BTL_POKEPARAM* attacker, WazaID orgWazaID, WazaID actWazaID, ZenryokuWazaKind zenryokuWazaKind, const PokeSet* targets );
  bool          scproc_CheckWazaMsgEnable( const BTL_POKEPARAM* attacker, WazaID orgWazaID, WazaID actWazaID, ZenryokuWazaKind zenryokuWazaKind, const PokeSet* targets ) const;
  bool          scproc_CheckWazaMsgCustom( const BTL_POKEPARAM* attacker, WazaID orgWazaID, WazaID actWazaID, ZenryokuWazaKind zenryokuWazaKind, BTL_HANDEX_STR_PARAMS* str );
  void          scproc_ZenryokuEffect( BTL_POKEPARAM* zenryokuUsePoke, WazaID originalWazaID, ZenryokuEffectMessage* effectMessage );
  bool          scproc_ZenryokuEffect_RankUp( BTL_POKEPARAM* zenryokuUsePoke, WazaID originalWazaID, WazaRankEffect rankEffect, u8 rankUpVolume, ZenryokuEffectMessage* effectMessage );
  bool          scproc_ZenryokuEffect_RankUp_Critical( BTL_POKEPARAM* zenryokuUsePoke, u8 rankUpVolume, ZenryokuEffectMessage* effectMessage );
  bool          scproc_ZenryokuEffect_RankUp_Multi5( BTL_POKEPARAM* zenryokuUsePoke, u8 rankUpVolume, ZenryokuEffectMessage* effectMessage );
  bool          scproc_ZenryokuEffect_RankUp_Base( BTL_POKEPARAM* zenryokuUsePoke, WazaRankEffect rankEffect, u8 rankUpVolume, ZenryokuEffectMessage* effectMessage );
  bool          scproc_ZenryokuEffect_RankRecover( BTL_POKEPARAM* zenryokuUsePoke, ZenryokuEffectMessage* effectMessage );
  bool          scproc_ZenryokuEffect_HpRecover( BTL_POKEPARAM* zenryokuUsePoke, ZenryokuEffectMessage* effectMessage );
  bool          scproc_ZenryokuEffect_HpRecover_Pos( BTL_POKEPARAM* zenryokuUsePoke, ZenryokuEffectMessage* effectMessage );
  bool          scproc_ZenryokuEffect_Konoyubitomare( BTL_POKEPARAM* zenryokuUsePoke, ZenryokuEffectMessage* effectMessage );
  void          scproc_ZenryokuWazaStartMessage( const BTL_POKEPARAM& zenryokuUsePoke, ZenryokuWazaKind zenryokuWazaKind );
  void          scPut_ZenryokuWazaStartMessage( const BTL_POKEPARAM& zenryokuUsePoke );
  void          getRankEffectByZenryokuEffect( ZenryokuWazaEffect zenryokuEffect, WazaRankEffect* rankEffect, u8* rankUpVolume ) const;
  void          scproc_ZenryokuReqWazaMessage( const BTL_POKEPARAM* attacker, WazaID reqWaza, WazaID actWaza, WazaID actZenryokuWaza );
  void          scproc_DecideZenryokuWazaByReqWaza( WazaID* zenryokuWaza, ZenryokuWazaKind* zenryokuWazaKind, const BTL_POKEPARAM* attacker, WazaID zenryokuWazaBase );
  bool          scproc_AddPosEffect( BtlPokePos effectPos, BtlPosEffect posEffect, const PosEffect::EffectParam& effectParam, u8 dependPokeId, const s32* factorParam, u8 factorParamNum );
  void          scproc_IncrementPokemonTotalTurnCount( void );
  void          scproc_UpdateCurrentRoyalRanking( void );
  void          scproc_ResetSpActPriority( void );
  void          scproc_OnTurnStart( void );
  void          scproc_OnTurnEnd( void );
  void          scproc_SetFrontPokeDirt_ByWeather( void );
  void          scproc_SetPokeDirt_ByWeather( BTL_POKEPARAM* pokeParam, BtlWeather weather );
  void          scproc_UseTrainerItem( BTL_POKEPARAM* bpp, u16 itemID, u8 actParam, u8 shortPokeID );
  u32           scproc_CalcActionPriority( const ACTION_ORDER_WORK& order );
  bool          scproc_FakeBreak( BTL_POKEPARAM* poke, const BTL_HANDEX_STR_PARAMS* breakMessage );
  void          scproc_RecoverZenryoku( void );
  bool          scproc_Battlefes_addFieldEffect(const FieldStatus::EffectType effect, const BtlGround ground, BTL_SICKCONT& cont, const BTL_HANDEX_STR_PARAMS* exStr);

  u8          scproc_HandEx_TokWinIn( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_TokWinOut( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_useItem( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_recoverHP( const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
  u8          scproc_HandEx_drain( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_damage( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_shiftHP( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_recoverPP( const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
  u8          scproc_HandEx_decrementPP( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_cureSick( const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
  u8          scproc_HandEx_addSick( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_rankEffect( const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
  u8          scproc_HandEx_setRank( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_recoverRank( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_resetRank( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_resetRankUp( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_setStatus( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_kill( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_deadCheck( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_changeType( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_exType( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_message( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_setTurnFlag( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_resetTurnFlag( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_setContFlag( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_resetContFlag( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_setPermFlag( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_sideEffectAdd( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_sideEffectRemove( const BTL_HANDEX_PARAM_HEADER* param_header );
  bool        scproc_HandEx_sideEffectRemoveCore( BtlSide side, const u8* sideEffectBitFlag );
  u8          scproc_HandEx_sideEffectSleep( const BTL_HANDEX_PARAM_HEADER* param_header );
  bool        scproc_HandEx_sideEffectSleepCore( BtlSide side, const u8* sideEffectBitFlag, bool wakeUpFlag );
  u8          scproc_HandEx_addFieldEffect( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_removeFieldEffect( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_changeWeather( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_PosEffAdd( const BTL_HANDEX_PARAM_HEADER* param_header );
  void        scproc_HandEx_GetPosEffectFactorParam( s32* factorParam, u8* factorParamNum, const BTL_HANDEX_PARAM_HEADER* handExParamHeader ) const;
  u8          scproc_HandEx_tokuseiChange( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_setItem( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_swapItem( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_CheckItemEquip( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_ItemSP( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_consumeItem( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_updateWaza( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_counter( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_delayWazaDamage( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_quitBattle( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_changeMember( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_batonTouch( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_addShrink( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_relive( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_setWeight( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_pushOut( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_intrPoke( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_intrWaza( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_sendLast( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_swapPoke( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_hensin( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_fakeBreak( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_juryokuCheck( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_TameHideCancel( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_effectByPos( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_vanishMsgWin( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_changeForm( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_setWazaEffectIndex( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_setWazaEffectEnable( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_friendshipEffect( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_insertWazaAction( const BTL_HANDEX_PARAM_HEADER* param_header );
  u8          scproc_HandEx_setDirt( const BTL_HANDEX_PARAM_HEADER* param_header );
  


  u16           scEvent_CheckPushOutEffectNo( const BTL_POKEPARAM* attacker, WazaID waza );
  u16           scEvent_CalcAgility( const BTL_POKEPARAM* attacker, bool fTrickRoomApply );
  void          scEvent_AfterChangeWeather( BtlWeather weather );
  void          scEvent_AfterMemberIn( const BTL_POKEPARAM* bpp, BtlEventType eventID );
  void          scEvent_SkillCall( const BTL_POKEPARAM* bpp );
  void          scEvent_AfterMemberInPrev( void );
  void          scEvent_AfterMemberInComp( void );
  u8            scEvent_GetWazaPriority( WazaID waza, const BTL_POKEPARAM* bpp );
  u8            scEvent_CheckSpecialActPriority( const BTL_POKEPARAM* attacker );
  u32           scEvent_MemberChangeIntr( const BTL_POKEPARAM* outPoke );
  void          scEvent_MemberOutFixed( BTL_POKEPARAM* outPoke );
  void          scEvent_BeforeDead( const BTL_POKEPARAM* bpp );
  void          scEvent_CheckItemReaction( const BTL_POKEPARAM* bpp, BtlItemReaction reactionType );
  void          scEvent_ActProcStart( const BTL_POKEPARAM* bpp, const BTL_ACTION_PARAM* action );
  void          scEvent_BeforeFight( const BTL_POKEPARAM* bpp, WazaID waza );
  void          scEvent_ActProcEnd( const BTL_POKEPARAM* bpp, BtlAction actionCmd );
  void scEvent_ActProcCanceled( const BTL_POKEPARAM* bpp, const BTL_ACTION_PARAM* actionCmd );
  bool          scEvent_CheckInemuriFail( const BTL_POKEPARAM* bpp );
  void          scEvent_AfterMove( const BTL_POKEPARAM* bpp );
  BtlWeather    scEvent_GetWeather( void );
  int           scEvent_CheckWeatherReaction( const BTL_POKEPARAM* bpp, BtlWeather weather, u32 damage );
  bool          scEvent_CheckEnableSimpleDamage( const BTL_POKEPARAM* bpp, u32 damage, DamageCause damageCause );
  void          scEvent_TurnCheck( u8 pokeID, BtlEventType event_type );
  bool          scEvent_CheckFloating( const BTL_POKEPARAM* bpp, bool fHikouCheck );
  fx32          svEvent_GetWaitRatio( const BTL_POKEPARAM* bpp );
  bool          scEvent_SkipNigeruCalc( const BTL_POKEPARAM* bpp );
  bool          scEvent_CheckNigeruForbid( const BTL_POKEPARAM* bpp );
  bool          scEvent_NigeruExMessage( const BTL_POKEPARAM* bpp );
  void          scEvent_CheckCombiWazaExe( const BTL_POKEPARAM* attacker, WAZAPARAM* wazaParam );
  void scEvent_WazaCallDecide( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParamOrg, const WAZAPARAM* wazaParamAct );
  void          scEvent_WazaExeDecide( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, BtlEventType evType  );
  bool          scEvent_CheckDelayWazaSet( const BTL_POKEPARAM* attacker, BtlPokePos* targetPos );
  void          scEvent_DecideDelayWaza( const BTL_POKEPARAM* attacker );
  void          scEvent_StartWazaSeq( const BTL_POKEPARAM* attacker, WazaID waza );
  void          scEvent_EndWazaSeq( const BTL_POKEPARAM* attacker, WazaID waza, bool fWazaEnable, const ActionDesc* actionDesc );
  void          scEvent_WazaRob( const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza );
  void          scEvent_WazaReflect( const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza );
  bool          scEvent_CheckWazaRob( const BTL_POKEPARAM* attacker, WazaID waza, bool isZenryokuWaza, const PokeSet* targetRec, u8* robberPokeID, u8* robTargetPokeID );
  bool          scEvent_IsRemoveAllFailCancel( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam );
  bool          scEvent_WazaAffineCheckEnable( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker );
  bool          scEvent_CheckMigawariThrew( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza  );
  void          scEvent_WazaExeEnd_Common( u8 pokeID, WazaID waza, const ActionDesc& actionDesc, BtlEventType eventID );
  bool          scEvent_CheckNotEffect( const WAZAPARAM* wazaParam, BtlEventType eventID,
                    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const DmgAffRec* affRec, BTL_HANDEX_STR_PARAMS* customMsg, bool* fNoReaction );
  bool          scEvent_SkipAvoidCheck( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam );
  bool          scEvent_IsCalcHitCancel( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
  u32           scEvent_getHitPer( BtlEventType eventType, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam );
  bool          scEvent_CheckHit( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam, bool* bFriendshipActive );
  bool          scEvent_IchigekiCheck( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam );
  bool          scEvent_CheckDmgToRecover( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam );
  void          scEvent_DmgToRecover( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
  bool          scEvent_CheckCritical( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, bool* bFriendshipAct );
  u32           scEvent_CalcKickBack( const BTL_POKEPARAM* attacker, WazaID waza, u32 damage, bool* fMustEnable );
  void          scEvent_ItemEquip( const BTL_POKEPARAM* bpp );
  void          scEvent_ItemEquipTmp( const BTL_POKEPARAM* bpp, u8 atkPokeID );
  void          scEvent_WazaNoEffectByFlying( const BTL_POKEPARAM* bpp );
  BtlTypeAff    scEvent_CheckDamageAffinity( const BTL_POKEPARAM& attacker, const BTL_POKEPARAM& defender, pml::PokeType waza_type, bool onlyAttacker );
  BtlTypeAff    scEvent_CheckDamageAffinity( const BTL_POKEPARAM& attacker, const BTL_POKEPARAM& defender, pml::PokeType waza_type, pml::PokeType poke_type, bool onlyAttacker );
  bool          scEvent_ExeFailThrew( const BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause );
  SV_WazaFailCause  scEvent_CheckWazaExecute( BTL_POKEPARAM* attacker, WazaID waza, BtlEventType eventID , const WAZAPARAM* wazaParam, const PokeSet* targets );
  void          scEvent_NotifyAirLock( void );
  void          scEvent_CheckSideEffectParam( u8 userPokeID, BtlSideEffect effect, BtlSide side, BTL_SICKCONT* cont );
  void          scEvent_ChangeTokuseiBefore( u8 pokeID, TokuseiNo prev_tokuseiID, u16 next_tokuseiID );
  void          scEvent_ChangeTokuseiAfter( u8 pokeID );
  void          scEvent_ItemSetFailed( const BTL_POKEPARAM* bpp );
  void          scEvent_ItemSetDecide( const BTL_POKEPARAM* bpp, u16 nextItemID );
  void          scEvent_ItemSetFixed( const BTL_POKEPARAM* bpp );
  u32           scEvent_CalcWazaRecoverHP( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza );
  bool          scEvent_CheckItemSet( const BTL_POKEPARAM* bpp, u16 itemID );
  u32           scEvent_DecrementPPVolume( const BTL_POKEPARAM* attacker, u8 wazaIdx, WazaID waza, PokeSet* rec );
  void          scEvent_WazaDamageDetermine( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam );
  BTL_POKEPARAM::KoraeruCause scEvent_CheckKoraeru( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, bool fWazaDamage, u16* damage );
  void          scEvent_KoraeruExe( BTL_POKEPARAM* bpp, BTL_POKEPARAM::KoraeruCause cause );
  void          scEvent_PrevWazaDamage( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, u32 poke_cnt, BTL_POKEPARAM** bppAry );
  void          scEvent_WazaDamageSideAfter( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, u32 damage );
  void          scEvent_DamageProcStart( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, PokeSet* targets );
  void          scEvent_DamageProcEndSub( const BTL_POKEPARAM* attacker, PokeSet* targets,
                    const ActionDesc* actionDesc, const WAZAPARAM* wazaParam, bool fDelayAttack, bool fRealHitOnly, BtlEventType eventID );
  void          scEvent_DamageProcEnd( const BTL_POKEPARAM* attacker, PokeSet* targets,
                  const WAZAPARAM* wazaParam, bool fDelayAttack );
  void          scEvent_UnDamageProcEnd( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker );
  bool          scEvent_CalcDamage(
                  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam,
                  BtlTypeAff typeAff, fx32 targetDmgRatio, bool criticalFlag, bool fSimurationMode, bool bMigawariEffective, u16* dstDamage );
  u16           scEvent_getWazaPower( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam );
  u16           scEvent_getAttackPower( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam, bool criticalFlag, bool* bYakedoDisable );
  u16           scEvent_getDefenderGuard(
                  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
                  const WAZAPARAM* wazaParam, bool criticalFlag );
  fx32          scEvent_CalcTypeMatchRatio( const BTL_POKEPARAM* attacker, pml::PokeType waza_type );
  bool          scEvent_CheckItemEquipFail( const BTL_POKEPARAM* bpp, u16 itemID );
  void          scEvent_AfterItemEquip( const BTL_POKEPARAM* bpp, u16 itemID, bool bCheckKinomi );
  void          scEvent_KillHandler( const BTL_POKEPARAM* target, u8 atkPokeID );
  WazaSick      scEvent_CheckWazaAddSick( WazaID waza,
                  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_SICKCONT* pSickCont );
  WazaSick       scEvent_DecideSpecialWazaSick(
                   const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* str );
  void           scEvent_GetWazaSickAddStr( WazaSick sick,
                   const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* str );
  void           scEvent_WazaSickCont( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target,
                    WazaSick sick, BTL_SICKCONT* sickCont );
  bool          scEvent_WazaSick_CheckFail( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaSick sick, SickCause cause, bool* bCallFailedEvent );
  void          scEvent_AddSick_Failed( const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaSick sick, SickCause cause, u32 wazaSerial );
  void          scEvent_PokeSickFixed( const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker,
                  pml::pokepara::Sick sick, BTL_SICKCONT sickCont );
  void          scEvent_WazaSickFixed( const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaSick sick );
  void          scEvent_IekiFixed( const BTL_POKEPARAM* target );
  bool          scEvent_CheckAddRankEffectOccur( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
  void          scEvent_GetWazaRankEffectValue( WazaID waza, u32 idx,
                  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaRankEffect* effect, int* volume );
  int           scEvent_RankValueChange( const BTL_POKEPARAM* target, WazaRankEffect rankType,
                  u8 wazaUsePokeID, u16 itemID, int volume );
  bool          scEvent_CheckPushOutFail( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
  u8            scEvent_WazaWeatherTurnUp( BtlWeather weather, const BTL_POKEPARAM* attacker );
  bool          scEvent_CheckChangeWeather( BtlWeather weather, u8* turn );
  void          scEvent_FieldEffectCall( const BTL_POKEPARAM* attacker, WazaID waza );
  void          scEvent_WazaDamageReaction(
                  const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam, BtlTypeAff aff,
                  u32 damage, bool fCritical, bool fMigawari );
  bool          scEvent_UnCategoryWaza( const WAZAPARAM* wazaParam,
                  const BTL_POKEPARAM* attacker, PokeSet* targets, bool* fFailMsgEnable );
  u32          scEvent_SickDamage( BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage );
  bool         scEvent_GetReqWazaParam( BTL_POKEPARAM* attacker,
                  WazaID orgWazaid, BtlPokePos orgTargetPos, REQWAZA_WORK* reqWaza );
  bool         scEvent_CheckWazaMsgCustom( const BTL_POKEPARAM* attacker, WazaID orgWazaID, WazaID actWazaID, bool isActWazaZenryoku, BTL_HANDEX_STR_PARAMS* str );
  bool         scEvent_CheckZenryokuReqWazaMsgCustom( const BTL_POKEPARAM* attacker, WazaID reqWazaID, WazaID actWaza, WazaID actZenryokuWaza, BTL_HANDEX_STR_PARAMS* str );
  void          scEvent_GetWazaParam( WazaID waza, const BTL_POKEPARAM* attacker, WAZAPARAM* param );
  void          scEvent_AfterWazaParamFixed( const BTL_POKEPARAM* attacker, WazaID orgWazaID, WazaID actWazaID, bool isActWazaZenryoku );
  void          scEvent_CheckWazaExeFail( const BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause );
  WazaForceEnableMode scEvent_WazaExecuteStart( const ActionDesc* actionDesc, const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, PokeSet* rec );
  bool          scEvent_CheckMamoruBreak( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
  bool          scEvent_CheckTameFail( const BTL_POKEPARAM* attacker, const PokeSet* targetRec );
  bool          scEvent_CheckTameTurnSkip( const BTL_POKEPARAM* attacker, WazaID waza );
  bool          scEvent_TameStart( const BTL_POKEPARAM* attacker, const PokeSet* targetRec, const WAZAPARAM* wazaParam, u8* hideTargetPokeID, bool* fFailMsgDisped );
  void          scEvent_TameStartFixed( const BTL_POKEPARAM* attacker );
  void          scEvent_TameSkip( const BTL_POKEPARAM* attacker, WazaID waza );
  bool          scEvent_TameRelease( const BTL_POKEPARAM* attacker, const PokeSet* rec, WazaID waza );
  bool          scEvent_CheckPokeHideAvoid(const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza , bool* bEnableAvoidMsg );
  bool          scEvent_DecrementPP_Reaction( const BTL_POKEPARAM* attacker, u8 wazaIdx );
  void          scEvent_HitCheckParam(const BTL_POKEPARAM* attacker, WazaID waza, ZenryokuWazaKind zenryokuWazaKind, HITCHECK_PARAM* param , bool fDelayAttack);
  u32           scEvent_GetWazaShrinkPer( WazaID waza, const BTL_POKEPARAM* attacker );
  bool          scEvent_CheckShrink( const BTL_POKEPARAM* target, u32 per );
  void          scEvent_FailShrink( const BTL_POKEPARAM* target );
  bool          scEvent_CheckRankEffectSuccess( const BTL_POKEPARAM* target,
                  WazaRankEffect effect, u8 wazaUsePokeID, int volume, RankEffectCause cause, u32 wazaRankEffSerial );
  void          scEvent_RankEffect_Failed( const BTL_POKEPARAM* bpp, u32 wazaRankEffSerial );
  void          scEvent_RankEffect_Fix( u8 atkPokeID, const BTL_POKEPARAM* bpp, WazaRankEffect rankType, int volume );
  void          scEvent_WazaRankEffectFixed( const BTL_POKEPARAM* target, WazaID wazaID, WazaRankEffect effectID, int volume );
  u16           scEvent_RecalcDrainVolume( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, u16 volume );
  void          scEvent_MamoruSuccess( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam );
  BtlTypeAff    scEvent_RewriteWazaAffinity( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, pml::PokeType wazaType, BtlTypeAff affinity );
  bool          scEvent_AffineFloatingCancel( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
  bool          scEvent_Check_FreeFallStart_Guard( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam, BTL_HANDEX_STR_PARAMS* strParam );
  bool          scEvent_CheckAttackerDeadPreTarget( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam );
  u32           scEvent_CheckSpecialWazaAdditionalPer( u8 atkPokeID, u8 defPokeID, u32 defaultPer );
  void          scEvent_TameReleaseFailed( BTL_POKEPARAM* attacker, PokeSet* targetRec, const WAZAPARAM* wazaParam );
  //@バトル変更箇所
  void          scEvent_ChangePokeBefore( BTL_POKEPARAM* bpp );
  void          scEvent_GetZenryokuEffect( ZenryokuWazaEffect* zenryokuEffect, WazaRankEffect* rankEffect, u8* rankUpVolume, const BTL_POKEPARAM* bpp, WazaID originalWaza );
  void          scEvent_GetZenryokuWaza( WazaID* zenryokuWaza, ZenryokuWazaKind* zenryokuWazaKind, const BTL_POKEPARAM* attacker, WazaID zenryokuWazaBase );
  void          scEvent_ChangeGroundBefore( u8 pokeID, BtlGround ground, BTL_SICKCONT* contParam );
  void          scEvent_ChangeGroundAfter( u8 pokeID, BtlGround ground );
  void          scEvent_PokeDeadActionAfter( u8 daedPokeID );
  void          scEvent_PokeDeadAfter( u8 daedPokeID );
  bool          scEvent_CheckTokuseiChangeEnable( u8 targetPokeID, TokuseiNo nextTokusei, TokuseiChangeCause cause );
  void          scEvent_TokuseiChangeFailed( u8 targetPokeID, TokuseiNo nextTokusei, TokuseiChangeCause cause );
  bool          scEvent_CheckAddSickFailStdSkip( BTL_POKEPARAM* target, BTL_POKEPARAM* attacker, WazaSick sick, SickCause cause );
  void          scEvent_SimpleDamage_Before( const BTL_POKEPARAM& bpp, u32 damage );
  void          scEvent_SimpleDamage_After( const BTL_POKEPARAM& bpp, u32 damage );
  void          scEvent_SimpleDamage_Failed( const BTL_POKEPARAM& bpp, DamageCause damageCause, u8 damageCausePokeID );
  void          scEvent_FixConfDamage( const BTL_POKEPARAM* poke, u16* damage );
  void          scEvent_ConfDamageReaction( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
  bool          scEvent_CheckIchigekiGuard( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam );
  void          scEvent_AfterCritical( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
  void          scEvent_ReplaceActWaza( const BTL_POKEPARAM* attacker, WazaID originWaza, WazaID nextWaza );
  void          scEvent_TurnEnd( void );

  void          scPut_ResetContFlag( BTL_POKEPARAM* bpp, BTL_POKEPARAM::ContFlag flag );
  void          scPut_SetContFlag( BTL_POKEPARAM* bpp, BTL_POKEPARAM::ContFlag flag );
  void          scPut_SetPermFlag( BTL_POKEPARAM* bpp, BTL_POKEPARAM::PermFlag flag );
  void          scPut_MemberOutMessage( BTL_POKEPARAM* bpp );
  void          scPut_SetTurnFlag( BTL_POKEPARAM* bpp, BTL_POKEPARAM::TurnFlag flag );
  void          scPut_ResetTurnFlag( BTL_POKEPARAM* bpp, BTL_POKEPARAM::TurnFlag flag );
  void          scPut_IllusionSet( const CLIENTID_REC& rec );
  void          scPut_CureSick( BTL_POKEPARAM* bpp, WazaSick sick, BTL_SICKCONT* oldCont );
  void          scPut_Message_Set( const BTL_POKEPARAM* bpp, u16 strID );
  void          scPut_SetBppCounter( BTL_POKEPARAM* bpp, BTL_POKEPARAM::Counter counterID, u8 value );
  void          scPut_IncBppCounter( BTL_POKEPARAM* bpp, BTL_POKEPARAM::Counter counterID );
  void          scPut_IncKillCount( BTL_POKEPARAM* bpp );
  void          scPut_EffectByPokePos( const BTL_POKEPARAM* bpp, u16 effectNo );
  void          scPut_CantAction( BTL_POKEPARAM* bpp );
  void          scPut_ConfCheck( const BTL_POKEPARAM* bpp );
  void scPut_WeatherDamage( BTL_POKEPARAM* bpp, BtlWeather weather, int damage, u8 damageCausePokeID );
  void scPut_SimpleHp( BTL_POKEPARAM* bpp, int value, DamageCause damageCause, u8 damageCausePokeID, bool fEffectEnable );
  void          scPut_Message_StdEx( u16 strID, u32 argCnt, const int* args );
  void          scPut_Message_StdSE( u16 strID, u32 SENo, u32 argCnt, const int* args );
  void          scPut_Message_SetEx( u16 strID, u32 argCnt, const int* args );
  void          scPut_Message_SetSE( u16 strID, u32 SENo, u32 argCnt, const int* args );
  void scPut_DecreaseHP( BTL_POKEPARAM* bpp, u32 value, bool bWazaDamage, DamageCause damageCause, u8 damageCausePokeID  );
  void          scPut_WazaNoEffect( const BTL_POKEPARAM* defender );
  void          scPut_WazaNoEffectIchigeki( const BTL_POKEPARAM* defender );
  void          scPut_UseItemAct( BTL_POKEPARAM* bpp );
  void          scPut_TokWin_In( const BTL_POKEPARAM* bpp );
  void          scPut_TokWin_Out( const BTL_POKEPARAM* bpp );
  void          scPut_RecoverPP( BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume, bool fOrgWaza );
  void          scPut_DecrementPP( BTL_POKEPARAM* attacker, u8 wazaIdx, u8 vol );
  void          scPut_AddSickFail( const BTL_POKEPARAM* target, AddSickFailCode failCode, WazaSick sick, SickCause cause );
  void scPut_KillPokemon( BTL_POKEPARAM* bpp, u8 atkPokeID, DamageCause deadCause, u8 effType );
  void          scPut_AddSick( BTL_POKEPARAM* target, WazaSick sick, const BTL_SICKCONT& sickCont );
  void          scPut_RankEffectLimit( const BTL_POKEPARAM* target, WazaRankEffect effect, int volume );
  void          scPut_RankEffect( BTL_POKEPARAM* target, WazaRankEffect effect, int volume, u16 itemID, bool fStdMsg, bool fViewEffect );
  void          scPut_SideEffectOffMsg( BtlSide side, BtlSideEffect sideEffect );
  void          scPut_WazaFail( const BTL_POKEPARAM* attacker, WazaID waza );
  void          scPut_WazaAvoid( const BTL_POKEPARAM* defender, WazaID waza );
  void          scPut_WazaAvoidByFriendship( const BTL_POKEPARAM* defender, WazaID waza );
  void scPut_ReqWazaEffect( BTL_POKEPARAM* bpp, WazaID waza, BtlPokePos targetPos, ZenryokuWazaKind zWazaKind );
  void scPut_WazaEffect( WazaID waza, WAZAEFF_CTRL* effCtrl, const WAZAEFF_RESERVE_POS& que_reserve_pos, bool bSyndDamageEffect, ZenryokuWazaKind zWazaKind );
  void          scPut_WazaSubEffect( const WAZAEFF_CTRL* effCtrl, u16 reservedPos );
  void          scPut_WazaExecuteFailMsg( BTL_POKEPARAM* bpp, WazaID waza, bool isZenryokuWaza, SV_WazaFailCause cause );
  void          scPut_DamageAff( BtlTypeAff affinity );
  void          scPut_MeromeroAct( const BTL_POKEPARAM* bpp );
  void          scPut_ConfDamage( BTL_POKEPARAM* bpp, u32 damage );
  void          scPut_CurePokeSick( BTL_POKEPARAM* bpp, WazaSick sick, bool fStdMsg );
  void          scPut_WazaMsg( const BTL_POKEPARAM* attacker, WazaID waza, bool isZenryokuWaza );
  void scPut_WazaDamagePlural( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam,u32 poke_cnt, const BtlTypeAff* aff, BTL_POKEPARAM** bpp, const u16* damage, bool fTargetPlural );
  void          scPut_WazaAffinityMsg( const BTL_POKEPARAM* attacker, u32 poke_cnt, const BtlTypeAff* aff, BTL_POKEPARAM** bpp,  bool fTargetPlural );
  void          scPut_AffGoodFriendshipMsg( const BTL_POKEPARAM* bpp );
  u8            affmsg_abbrev( u8* pokeID, u32 numPokemons );
  void scproc_CriticalMsg( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, u32 poke_cnt, BTL_POKEPARAM** bpp, const CriticalType* critical_flg, bool fTargetPlural );
  bool          scPut_SideEffect_Add( BtlSide side, BtlSideEffect sideEffect, const BTL_SICKCONT& contParam );
  bool          scPut_SideEffect_Remove( BtlSide side, BtlSideEffect sideEffect );
  void          scPut_SideEffect_IncTurnCount( BtlSide side, BtlSideEffect sideEffect );
  void          scPut_PublishClientInformation_AppeardPokemon( const BTL_POKEPARAM& appeardPoke );
  void          scPut_IncTotalTurnCount( u8 pokeId );
  void          scPut_IncBattleTurnCount( void );
  void          scPut_SetDirt( BTL_POKEPARAM* poke, DirtType dirt );
  void          scPut_InsertWazaInfo( const BTL_HANDEX_PARAM_INSERT_WAZA_ACTION* param );
  void          scPut_SetSpActPriority( BTL_POKEPARAM* poke, u8 priority );



  BtlAction             ActOrder_Proc( ACTION_ORDER_WORK* actOrder );
  void                  ActOrder_Proc_Core( ACTION_ORDER_WORK* actOrder );
  bool                  ActOrder_IntrProc( u8 intrPokeID, u8 targetPokeID );
  void                  ActOrder_ForceDone( u8 pokeID );
  ACTION_ORDER_WORK*    ActOrderTool_SearchByPokeID(u8 pokeID, bool bSkipMegaEvo);
  ACTION_ORDER_WORK*    ActOrderTool_SearchNextByPokeID( const ACTION_ORDER_WORK* start, u8 pokeID, bool bSkipMegaEvo );
  ACTION_ORDER_WORK*    ActOrderTool_SearchByPokeID_Core( u8 pokeID, u8 startIdx, bool bSkipMegaEvo );
  ACTION_ORDER_WORK*    ActOrderTool_SearchByWazaID( WazaID waza, u8 startIndex );
  void                  ActOrderTool_SendToLast( const ACTION_ORDER_WORK* actOrder );
  int                   ActOrderTool_Intr( const ACTION_ORDER_WORK* actOrder, u32 intrIndex );
  s32                   ActOrderTool_Insert( const ACTION_ORDER_WORK& actOrder, u32 insertIndex );
  u8                    ActOrderTool_GetIndex( const ACTION_ORDER_WORK* actOrder );
  ACTION_ORDER_WORK*    ActOrderTool_SearchForCombiWaza( WazaID waza, u8 pokeID, BtlPokePos targetPos );
  bool                  ActOrderTool_CheckAllActDoneByPokeID( u8 pokeID ) const;
  bool                  ActOrderProc_OnlyPokeIn( const SVCL_ACTION* clientAction );
  u32                   ActOrderProc_Main( u32 startOrderIdx );
  bool                  ActOrder_IntrReserve( u8 intrPokeID );
  bool                  ActOrder_IntrReserveByWaza( WazaID waza );
  bool                  ActOrder_SendLast( u8 pokeID );

  bool           handexSub_putString( const BTL_HANDEX_STR_PARAMS* strParam );
  void           handEx_Exe( BTL_HANDEX_PARAM_HEADER* handEx_header );

  void        CALCDMG_Set(CALC_DAMAGE_REC* dmgRec, BTL_POKEPARAM* attacker, PokeSet* target,
                const WAZAPARAM* wazaParam, const DmgAffRec& affRec, fx32 dmg_ratio , AffCounter* affCounter);
  u32         CALCDMG_GetCount( const CALC_DAMAGE_REC* dmgRec );
  u32         CALCDMG_GetMigawariHitPoke( const CALC_DAMAGE_REC* rec,
                BTL_POKEPARAM** bppAry, u16* dmgAry, BtlTypeAff* affAry, CriticalType* criticalFlagAry );
  u32         CALCDMG_GetAllHitPoke( const CALC_DAMAGE_REC* rec, BTL_POKEPARAM** bppAry );
  u32         CALCDMG_GetDamageSum( const CALC_DAMAGE_REC* rec );
  u32         CALCDMG_GetRealHitPoke( const CALC_DAMAGE_REC* rec,
                BTL_POKEPARAM** bppAry, u16* dmgAry, BtlTypeAff* affAry, CriticalType* criticalFlagAry, BTL_POKEPARAM::KoraeruCause* koraeCauseAry );

  bool        isSkyBattleFailWaza( WazaID waza );
  bool isWazaFailByKodawariLock( const BTL_POKEPARAM* attacker, WazaID waza ) const;
  void        scPut_NotifyPGLRecord( const BTL_POKEPARAM* defPoke, const MainModule::PGLRecParam* pgl_param );


  bool   IsBppExist( const BTL_POKEPARAM* bpp ) const;
  bool   IsMustHit( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );

  bool isTurnEnd( Result flowResult ) const;
  void updateIntrudeSystemOnTurnStart( void );
  void applyWazaDamageToIntrudeSystem( const BTL_POKEPARAM& defender, BtlTypeAff affinity );
  void registerFrontPokemonBattleIn( void );
  void clearPokeCantActionFlag( void );
  void generateActionOrder( const SVCL_ACTION* clientAction, ACTION_ORDER_WORK* actionOrder, u8* actionOrderNum, bool* recalcActOrderNeed, u8* recalcActOrderIndex );
  bool getHeadWazaActIndex( u8* headWazaActIndex, const ACTION_ORDER_WORK* actionOrder, u8* actionOrderNum ) const;
  void setRecalcPriorityRequestByMegaEvolve( ACTION_ORDER_WORK* actionOrder, u8 actionOrderNum ) const;
  void setRecalcPriorityRequest( ACTION_ORDER_WORK* actionOrder, u8 actionOrderNum, u8 targetPokeID, BtlAction targetAction ) const;
  u8 storeClientAction( const SVCL_ACTION* clientAction, ACTION_ORDER_WORK* order );
  void sortClientAction( ACTION_ORDER_WORK* order, u8 numAction, bool isSpPriEventEnable );
  void sortClientAction( ACTION_ORDER_WORK* order, u8 headIndex, u8 tailIndex, bool isSpPriEventEnable );
  void recordPokeDead( u8 pokeId );
  bool isSkipBattleAfterShowdown( void ) const;
  fx32 getDamageRatioByTargetCount( const PokeSet& targets ) const;
  u8 getFightEnableBenchPokeNum( u8 pokeID );

  void   sortActionSub( ACTION_ORDER_WORK* order, u8 headIndex, u8 tailIndex );
  WazaID checkEncoreWazaChange( const BTL_POKEPARAM* bpp, const BTL_ACTION_PARAM* action, const ActionDesc* actionDesc );
  bool   checkPlayerSideAlive( void );
  bool   getexp_make_cmd( BTL_PARTY* party, const ServerFlowSub::CALC_EXP_WORK* calcExp );
  bool   reqChangePokeForServer( CLIENTID_REC* rec );
  SabotageType  checkSabotageType( const BTL_POKEPARAM* bpp );
  void   storeFrontPokeOrderAgility( PokeSet* pokeSet );
  bool   checkPokeDeadFlagAllOn( u8 pokeID );
  u32    checkExistEnemyMaxLevel( void );
  u32    wazaSerial_Inc( void );
  bool   perOccur( u8 per );
  WazaSick   trans_sick_code( const BTL_POKEPARAM* bpp, BtlWazaSickEx* exCode );
  AddSickFailCode  addsick_check_fail_overwrite( const BTL_POKEPARAM* target, WazaSick sick, SickOverWriteMode overWriteMode );
  AddSickFailCode  addsick_check_fail_std( const BTL_POKEPARAM* target, WazaSick sick, const BTL_SICKCONT& sickCont );
  void   flowsub_CheckPokeHideAvoid( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, PokeSet* targets );
  void   flowsub_CheckTypeAffinity( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, PokeSet* targets, DmgAffRec* affRec );
  void   flowsub_CheckNoEffect_TypeAffinity( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec& affRec, ActionRecorder* actionRecorder );
  void flowsub_CheckNotEffect_Guard( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec& affRec, ActionRecorder* actionRecorder );
  void   flowsub_CheckNotEffect_Tokusei( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec& affRec, ActionRecorder* actionRecorder );
  void   flowsub_CheckNotEffect_Affinity( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec& affRec );
  void   flowsub_CheckNoEffect_SimpleSick( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets );
  void   flowsub_CheckNoEffect_SimpleEffect( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets );
  void   flowsub_checkWazaAvoid( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, PokeSet* targets, ActionRecorder* actionRecorder );
  void   putSimpleEffectFailMsg( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam, u32 wazaSerial, SimpleEffectFailManager::Result failResult );
  PushOutEffect check_pushout_effect( const BTL_POKEPARAM* attacker );
  int           get_pushout_nextpoke_idx( const SVCL_WORK* clwk );
  u32         roundDamage( const BTL_POKEPARAM* bpp, u32 damage );
  bool        checkPlayersPoke( const BTL_POKEPARAM* bpp );
  void         wazaDmgRec_Add( BtlPokePos atkPos, const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam, u16 damage );
  void         wazaRobParam_Init( WAZA_ROB_PARAM* param );
  void         wazaRobParam_Add( WAZA_ROB_PARAM* param, u8 robberPokeID, u8 targetPokeID, BtlPokePos targetPos );
  void         reserveWazaEffPos( WAZAEFF_RESERVE_POS* reservePos );
  bool isKillCountIncrementEnable( u8 deadPokeID, u8 deadCausePokeID, DamageCause deadCause ) const;
  bool isKillCountEffectEnable( bool isKillCountInc ) const;

  void  scEvent_checkNoEffectBySideEffectGuard_Begin( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets );
  void  scEvent_checkNoEffectBySideEffectGuard_End( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets );

  

  void  wazaEffCtrl_Init( WAZAEFF_CTRL* ctrl );
  void  wazaEffCtrl_Setup( WAZAEFF_CTRL* ctrl, const BTL_POKEPARAM* attacker, const PokeSet* rec );
  void  wazaEffCtrl_ChangeAtkPos( WAZAEFF_CTRL* ctrl, BtlPokePos atkPos );
  /**
   *  ワザエフェクト発動管理：エフェクトに使うワザナンバーの差し替え（合体ワザ用）
   */
  inline void wazaEffCtrl_ChangeEffectWazaID( WAZAEFF_CTRL* ctrl, WazaID waza )
  {
    ctrl->effectWazaID = waza;
  }
  /**
   *  ワザエフェクト発動管理：エフェクトに使うワザナンバーの取得（合体ワザ用）
   */
  inline WazaNo wazaEffCtrl_GetEffectWazaID( const WAZAEFF_CTRL* ctrl )
  {
    return ctrl->effectWazaID;
  }

  /**
   *  ワザエフェクト発動管理：発動確定
   */
  inline void wazaEffCtrl_SetEnable( WAZAEFF_CTRL* ctrl )
  {
    ctrl->fEnable = true;
  }
  /**
   *  ワザエフェクト発動管理：有効で、既に発動したことを通知（複数回ヒット）
   */
  inline void wazaEffCtrl_SetEnableDummy( WAZAEFF_CTRL* ctrl )
  {
    if( !(ctrl->fEnable) )
    {
      ctrl->fEnable = true;
      ctrl->fDone = true;
    }
  }
  /**
   *  ワザエフェクト発動管理：発動確定するかチェック
   */
  inline bool wazaEffCtrl_IsEnable( const WAZAEFF_CTRL* ctrl )
  {
    return ctrl->fEnable;
  }
  /**
   *  ワザエフェクト発動管理：既にエフェクトコマンド生成済みチェック
   */
  inline bool wazaEffCtrl_IsDone( const WAZAEFF_CTRL* ctrl )
  {
    return ctrl->fDone;
  }
  inline void wazaEffCtrl_SetEffectIndex( WAZAEFF_CTRL* ctrl, u8 index )
  {
    ctrl->effectIndex = index;
  }
  inline void wazaEffCtrl_AddSubEffectPoke( WAZAEFF_CTRL* ctrl, u8 pokeID )
  {
    switch( ctrl->subEff.pokeCnt ){
      case 0: ctrl->subEff.pokeID_1 = pokeID; break;
      case 1: ctrl->subEff.pokeID_2 = pokeID; break;
      case 2: ctrl->subEff.pokeID_3 = pokeID; break;
      case 3: ctrl->subEff.pokeID_4 = pokeID; break;
      case 4: ctrl->subEff.pokeID_5 = pokeID; break;
      default:
        return;
    }
    ++(ctrl->subEff.pokeCnt);
  }
  inline void wazaEffCtrl_ClearSubEffectParams( WAZAEFF_CTRL* ctrl )
  {
    ctrl->subEff.Raw = 0;
  }
  inline bool wazaEffCtrl_IsValidSubEffectParams( WAZAEFF_CTRL* ctrl )
  {
    return (ctrl->subEff.pokeCnt != 0 );
  }

  BtlPokePos convertReqWazaTargetPos( WazaID orgWaza, BtlPokePos defaultTargetPos ) const;



  //----------------------------------------------------------------------------------------------
  // サーバーフローフラグ処理
  //----------------------------------------------------------------------------------------------
  inline void FlowFlg_Set( FlowFlag flg )
  {
    m_flowFlags[ flg ] = 1;
  }
  inline void FlowFlg_Clear( FlowFlag flg )
  {
    m_flowFlags[ flg ] = 0;
  }
  inline bool FlowFlg_Get( FlowFlag flg )
  {
    return ( m_flowFlags[ flg ] != 0 );
  }
  inline void FlowFlg_ClearAll( void )
  {
    for(u32 i=0; i<GFL_NELEMS(m_flowFlags); ++i)
    {
      m_flowFlags[ i ] = 0;
    }
  }
  inline int roundValue( int val, int min, int max )
  {
    if( val < min ){ val = min; }
    if( val > max ){ val = max; }
    return val;
  }
  inline int roundMin( int val, int min )
  {
    if( val < min ){ val = min; }
    return val;
  }
  inline int roundMax( int val, int max )
  {
    if( val > max ){ val = max; }
    return val;
  }

  /**
   *  ヒットチェック：最初のヒットか判定
   */
  inline bool HITCHECK_IsFirstTime( const HITCHECK_PARAM* param )
  {
    return param->count == 0;
  }
  /**
   *  複数回ヒットするか判定
   */
  inline bool HITCHECK_IsPluralHitWaza( const HITCHECK_PARAM* param )
  {
    if( param->fPluralHitWaza ){
      return true;
    }
    if( param->countMax > 1 ){
      return true;
    }
    return false;
  }
  /**
   *  複数回ヒットするが、もともとは単発ワザである判定（とくせいなどの効果で複数回ヒットしている）
   */
  inline bool HITCHECK_IsPluralHitException( const HITCHECK_PARAM* param )
  {
    BTL_PRINT("複数回ヒットフラグ=%d, ヒット回数最大=%d なので...", param->fPluralHitWaza, param->countMax);
    if( param->fPluralHitWaza == false)
    {
      if( param->countMax > 1 ){
        BTL_PRINT(" 拡張ヒットです\n");
        return true;
      }
    }
    BTL_PRINT(" 拡張ヒットではない\n");
    return false;
  }
  /**
   *
   */
  inline bool HITCHECK_CheckWazaEffectPuttable( HITCHECK_PARAM* param )
  {
    //if( param->fPutEffectCmd == true ){//警告C4805抑制のため条件文修正
    if( param->fPutEffectCmd ){
      param->fPutEffectCmd = true;
      return true;
    }
    return true;
  }
  /**
   *
   */
  inline void HITCHECK_SetPluralHitAffinity( HITCHECK_PARAM* param, BtlTypeAff affinity )
  {
    if( param->pluralHitAffinity == pml::battle::TypeAffinity::TYPEAFF_1 ){
      param->pluralHitAffinity = affinity;
    }
  }

  //----------------------------------------------------------------------------------------------
  // 装備アイテム変更カウンタ処理
  //----------------------------------------------------------------------------------------------
  inline u8 ItemChangeCounter_Get( const BTL_POKEPARAM* bpp )
  {
    u8 pokeID = bpp->GetID();
    if( pokeID >= GFL_NELEMS(m_itemChangeCounter) ){
      GFL_ASSERT(0);
      return 0;
    }
    return m_itemChangeCounter[ pokeID ];
  }
  inline void ItemChangeCounter_Increment( const BTL_POKEPARAM* bpp )
  {
    u8 pokeID = bpp->GetID();
    if( pokeID >= GFL_NELEMS(m_itemChangeCounter) ){
      GFL_ASSERT(0);
      return;
    }
    // カンスト処理をしていない。以下理由。
    //  - ターンごとにゼロクリアするのでまず不要。
    //  - 万一 u16 上限に達した場合でも「呼び出すたびに値が変わる」方が想定どおりの動作となる。
    m_itemChangeCounter[ pokeID ]++;
  }
  inline void ItemChangeCounter_Clear( void )
  {
    gfl2::std::MemClear( m_itemChangeCounter, sizeof(m_itemChangeCounter) );
  }

  static int checkIntrudePokeIndex( const POKECON* pokeCon );



};  // class ServerFlow

//----------------------------------------------------------------------------------------------
// 重複ポケモンナンバーを省略可能なポケモンID配列
//----------------------------------------------------------------------------------------------
class SetMonsNoPokeID {
  GFL_FORBID_COPY_AND_ASSIGN(SetMonsNoPokeID);

  public:
    SetMonsNoPokeID( const POKECON* pokeCon ) : m_pokeCon(pokeCon)
    {
      Clear();
    };
    ~SetMonsNoPokeID(){};

    /**
     * 内容初期化
     */
    void Clear( void )
    {
      m_count = 0;
      m_dummyID = BTL_POKEID_NULL;
      for(u32 i = 0; i < GFL_NELEMS(m_pokeID); ++i) {
        m_pokeID[i] = BTL_POKEID_NULL;
      }
    }

    /**
     * ID１件追加
     * @param ID  追加するID
     * @return 追加後の格納ID数
     */
    u8 Add( u8 ID )
    {
      if( m_count < GFL_NELEMS(m_pokeID) ){
        m_pokeID[ m_count++ ] = ID;
      }
      return m_count;
    }

    /**
     * 格納されているID数を取得
     * @return
     */
    u8 Count( void ) const
    {
      return m_count;
    }

    /**
     * 配列アクセス
     */
    u8&  operator[]( u32 idx ){
      if( idx < m_count ){
        return m_pokeID[ idx ];
      }
      return m_dummyID;
    }

    /**
     * 重複しているモンスターナンバーのIDを削除
     * @return  削除後、残っているID数
     */
    u8 Abbreviation( void )
    {
      for(u32 i=1; i<m_count; ++i)
      {
        if( isExistSameMonsNoPrev(i) )
        {
          closeUp( i, m_count );
          --m_count;
          --i;
          continue;
        }
      }
      return m_count;
    }


  private:
    const POKECON* m_pokeCon;
    u8             m_pokeID[ BTL_POSIDX_MAX ];
    u8             m_count;
    u8             m_dummyID;

    /**
     * idx より前に、idxと同じモンスターナンバーがいるか判定
     * @return いたらtrue
     */
    bool isExistSameMonsNoPrev( u8 idx )
    {
      const BTL_POKEPARAM* target = m_pokeCon->GetPokeParamConst( m_pokeID[idx]);
      if( !target ){
        return false;
      }
      for(u32 i=0; i<idx; ++i){
        const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParamConst( m_pokeID[i]);
        if( bpp && bpp->GetMonsNo() == target->GetMonsNo() ){
          return true;
        }
      }
      return false;
    }

    void closeUp( u8 idx, u8 poke_count )
    {
      // 警告C4018抑制のためループカウンタをsignedにしている
      for(s32 i=idx; i<(poke_count-1); ++i){
        m_pokeID[i] = m_pokeID[i+1];
      }
      if( poke_count ){
        m_pokeID[ poke_count - 1 ] = BTL_POKEID_NULL;
      }
    }

};

GFL_NAMESPACE_END(btl)
#endif // __BTL_SERVERFLOW_H__
