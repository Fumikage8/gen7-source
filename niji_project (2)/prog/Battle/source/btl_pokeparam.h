//=============================================================================================
/**
 * @file  btl_pokeparam.h
 * @brief ポケモンXY バトルシステム  参加ポケモン戦闘用データ
 * @author  taya
 *
 * @date  2010.12.17  作成
 */
//=============================================================================================
#ifndef __BTL_POKEPARAM_H__
#define __BTL_POKEPARAM_H__

#include <util/include/gfl2_std_string.h>
#include <Battle/include/battle_DefaultPowerUp.h>

#include "./btl_common.h"
#include "./btl_server_const.h"
#include "./btl_sickcont.h"
#include "./btl_field.h"

GFL_NAMESPACE_BEGIN(btl)


class BTL_POKEPARAM {

  GFL_FORBID_COPY_AND_ASSIGN( BTL_POKEPARAM );

public:

  /**
   * @brief コンストラクタ
   */
  BTL_POKEPARAM( void );

  /**
   * @brief デストラクタ
   */
  ~BTL_POKEPARAM();



  /**
   * @brief セットアップに必要なパラメータ
   */
  struct SetupParam
  {
    pml::pokepara::PokemonParam* srcParam;            // 元となるポケモンパラメータ
    DefaultPowerUpDesc           defaultPowerUpDesc;  // 場に出た時点で適用する能力アップの設定
    u8                           pokeID;              // ポケモンのID
    u8                           friendship;          // なかよし度
    const FieldStatus*           fieldStatus;         // 関連づけるフィールド
  };

  /**
   * @brief セットアップ
   * @param setupParam  セットアップに必要なパラメータ
   */
  void Setup( const SetupParam& setupParam );

  /**
   * @brief コピーする
   * @param srcParam  コピー元のパラメータ
   */
  void CopyFrom( const BTL_POKEPARAM& srcParam );



  //--------------------------------------------------------------
  /**
   *  取得できる能力値
   */
  //--------------------------------------------------------------
  typedef enum {

    BPP_VALUE_NULL = 0,

    BPP_ATTACK_RANK,
    BPP_DEFENCE_RANK,
    BPP_SP_ATTACK_RANK,
    BPP_SP_DEFENCE_RANK,
    BPP_AGILITY_RANK,
    BPP_HIT_RATIO,
    BPP_AVOID_RATIO,

    //--- ここまで能力ランクアップ／ダウン効果の引数としても用いる ---
    //--- WazaRankEffect と同じ並び順のため、変更してはならない    ---
    BPP_ATTACK,
    BPP_DEFENCE,
    BPP_SP_ATTACK,
    BPP_SP_DEFENCE,
    BPP_AGILITY,
    BPP_HP,
    BPP_MAX_HP,
    BPP_LEVEL,
    BPP_TOKUSEI,                ///< とくせい
    BPP_TOKUSEI_EFFECTIVE,      ///< 効果のあるとくせい（いえき中ならPOKETOKUSEI_NULL）
    BPP_SEX,
    BPP_EXP,
    BPP_MONS_POW,               ///< 種族値( こうげき )
    BPP_MONS_AGILITY,           ///< 種族値( すばやさ )
    BPP_CALL_VALUE,             ///< 呼び値

    BPP_RANKVALUE_START = BPP_ATTACK_RANK,
    BPP_RANKVALUE_END = BPP_AVOID_RATIO,
    BPP_RANKVALUE_RANGE = (BPP_RANKVALUE_END - BPP_RANKVALUE_START) + 1,

  }ValueID;

  //--------------------------------------------------------------
  /**
   *  「こらえる（ワザダメージでHPが最低１残る）」ケース
   */
  //--------------------------------------------------------------
  typedef enum {
    KORAE_NONE = 0,          ///< こらえない
    KORAE_WAZA_DEFENDER,     ///< 防御側のワザによる（「こらえる」を使用など）
    KORAE_WAZA_ATTACKER,     ///< 攻撃側のワザによる（「みねうち」を使用など）
    KORAE_ITEM,              ///< 防御側の装備アイテム効果（きあいのタスキなど）
    KORAE_TOKUSEI_DEFENDER,  ///< 防御側のとくせいによる
    KORAE_FRIENDSHIP,        ///< かわいがり効果
  }KoraeruCause;

  enum {
    WAZADMG_REC_TURN_MAX = 3,  ///< ワザダメージレコード：何ターン分の記録を取るか？
    WAZADMG_REC_MAX      = 6,  ///< ワザダメージレコード：１ターンにつき、何件分まで記録するか？
  };

  //--------------------------------------------------------------
  /**
   *  ワザダメージレコード構造体
   */
  //--------------------------------------------------------------
  typedef struct {
    u16            wazaID;     ///< ワザID
    u16            damage;     ///< ダメージ量
    WazaDamageType damageType; ///< ダメージタイプ( なし・物理・特殊 )
    u8             wazaType;   ///< ワザタイプ（ほのお、みず、等々...）
    u8             pokeID;     ///< 攻撃したポケモンID
    BtlPokePos     pokePos;    ///< 攻撃したポケモン位置
//  }BPP_WAZADMG_REC;
  }WAZADMG_REC;

  static inline void WAZADMG_REC_Setup( WAZADMG_REC* rec, u8 pokeID, BtlPokePos pokePos, u16 wazaID, u8 wazaType, u16 damage, WazaDamageType damageType )
  {
    rec->wazaID = wazaID;
    rec->damage = damage;
    rec->damageType = damageType;
    rec->wazaType = wazaType;
    rec->pokeID = pokeID;
    rec->pokePos = pokePos;
  }

  //--------------------------------------------------------------
  /**
   *  ターン終了ごとにクリアされるフラグセット
   */
  //--------------------------------------------------------------
  typedef enum {

    TURNFLG_ACTION_START,           ///< 行動処理開始
    TURNFLG_ACTION_DONE,            ///< 行動処理終了
    TURNFLG_DAMAGED,                ///< ダメージ喰らった
    TURNFLG_WAZAPROC_DONE,          ///< ワザ処理を終了した
    TURNFLG_SHRINK,                 ///< ひるまされた
    TURNFLG_KIAI_READY,             ///< 気合いパンチ待機（ダメージで必ずひるむ）
    TURNFLG_KIAI_SHRINK,            ///< 気合いパンチ待機中ダメージ
    TURNFLG_MAMORU,                 ///< “まもる”発動
    TURNFLG_ITEM_CONSUMED,          ///< アイテムを使用して無くなった
    TURNFLG_ITEM_CANT_USE,          ///< 装備アイテム使えない
    TURNFLG_COMBIWAZA_READY,        ///< 合体ワザ準備中
    TURNFLG_TAMEHIDE_OFF,           ///< 溜めワザによる非表示状態を解除する必要あり
    TURNFLG_MOVED,                  ///< サイドチェンジ・ムーブを行った
    TURNFLG_TURNCHECK_SICK_PASSED,  ///< ターンチェック状態異常処理を経過した
    TURNFLG_HITRATIO_UP,            ///< ミクル効果によりヒット率上昇中
    TURNFLG_NAGETUKERU_USING,       ///< 「なげつける」の実行中
    TURNFLG_MAMORU_ONLY_DAMAGE_WAZA,///< “まもる”発動中だが、効果はダメージ技のみ
    TURNFLG_RESERVE_ITEM_SPEND,     ///< アイテム消費が確定している
    TURNFLG_APPEARED_BY_POKECHANGE, ///< ポケモン交代によって場に出た
    TURNFLG_CANT_ACTION,            ///< 反動で動けないターン( 動けなくなったターンではない )
    TURNFLG_TRAPPSHELL_READY,       ///< トラップシェル待機

    TURNFLG_MAX,

  }TurnFlag;

  //--------------------------------------------------------------
  /**
   *  生きて場にいる限りに保持されるフラグセット
   */
  //--------------------------------------------------------------
  #include "./btl_pokeparam_contflag.h"

  //--------------------------------------------------------------
  /**
   *  場から引っ込んでも死んでも継続されるフラグセット
   */
  //--------------------------------------------------------------
  typedef enum {

    PERMFLAG_ATE_KINOMI,         ///< きのみを食べた
    PERMFLAG_INTRUDE_IN,         ///< 乱入してきた
    PERMFLAG_LEVELUP,            ///< レベルアップした
    PERMFLAG_KIZUNAHENGE_DONE,   ///< きずなへんげした( サトシゲッコウガ用 )

    PERMFLAG_MAX,
    PERMFLAG_NULL = PERMFLAG_MAX,

  }PermFlag;

  //--------------------------------------------------------------
  /**
   *  各種カウンタ値
   *  場から引っ込んだり、死んだりするとリセットされます
   */
  //--------------------------------------------------------------
  typedef enum {

    COUNTER_TAKUWAERU,          ///< たくわえる段階数
    COUNTER_TAKUWAERU_DEF,      ///< たくわえるによって上がった防御ランク
    COUNTER_TAKUWAERU_SPDEF,    ///< たくわえるによって上がった特防ランク
    COUNTER_MAMORU,             ///< まもる・みきりカウンタ
    COUNTER_FREEFALL,           ///< フリーフォールで捕まえてるポケIDをセットする

    COUNTER_MAX,
  }Counter;

  //--------------------------------------------------------------
  /**
   *  ランク効果値の上限・下限・デフォルト値
   */
  //--------------------------------------------------------------
  enum {
    RANK_STATUS_MIN = 0,
    RANK_STATUS_MAX = 12,
    RANK_STATUS_DEFAULT = 6,
  };

  //--------------------------------------------------------------
  /**
   * @brief タイプ拡張の原因
   */
  //--------------------------------------------------------------
  enum ExTypeCause
  {
    EXTYPE_CAUSE_NONE,
    EXTYPE_CAUSE_HALLOWEEN,   // 技「ハロウィン」
    EXTYPE_CAUSE_MORINONOROI, // 技「もりののろい」
  };

  //--------------------------------------------------------------
  /**
   * @brief ねむりチェックモード
   */
  //--------------------------------------------------------------
  enum NemuriCheckMode
  {
    NEMURI_CHECK_ONLY_SICK,            // 状態異常「ねむり」のみ
    NEMURI_CHECK_INCLUDE_ZETTAINEMURI, // 特性「ぜったいねむり」を含む
  };



  u8   GetID( void ) const;
  u16  GetMonsNo( void ) const;
  int  GetValue( ValueID vid ) const;
  int  GetValue_Base( ValueID vid ) const;
  int  GetValue_Critical( ValueID vid ) const;
  u8   GetEffortValue( pml::pokepara::PowerID powerID ) const;
  bool IsEffortValueFull( void ) const;

  //@バトル変更箇所 へんしん時にフーパを取るため追加。
  //いじげんラッシュ以外の判定では使うな。
  u16  GetHenshinMonsNo( void ) const;

  const pml::pokepara::PokemonParam*  GetSrcData( void ) const;
  void                  SetViewSrcData( u8 fakeTargetPokeId, const pml::pokepara::PokemonParam* fakePP );
  const PokePara*       GetViewSrcData( void ) const;

  bool IsHPFull( void ) const;
  bool IsDead( void ) const;
  bool IsFightEnable( void ) const;
  bool IsMatchTokusei( TokuseiNo tokusei ) const;

  bool           CheckSick( WazaSick sickType ) const;
  bool           CheckNemuri( NemuriCheckMode checkMode ) const;
  pml::pokepara::Sick   GetPokeSick( void ) const;
  u16            GetSickParam( WazaSick sick ) const;
  BTL_SICKCONT   GetSickCont( WazaSick sick ) const;
  u8             GetSickTurnCount( WazaSick sick ) const;
  bool           IsSickLastTurn( WazaSick sickType ) const;
  bool           CheckMoudoku( void ) const;
  WazaID         GetWazaLockID( void ) const;
  bool           IsRankEffectValid( ValueID rankType, int volume ) const;
  int            RankEffectUpLimit( ValueID rankType ) const;
  int            RankEffectDownLimit( ValueID rankType ) const;
  bool           IsRankEffectDowned( void ) const;
  u8             GetCriticalRank( void ) const;
  u8             GetCriticalRankPure( void ) const;
  bool           GetBtlInFlag( void ) const;
  u16            GetItem( void ) const;
  u16            GetItemEffective( const FieldStatus& fldSim ) const;
  void           SetItem( u16 itemID );
  void           TurnCheck( void );
  void           FakeDisable( void );
  u8             GetFakeTargetPokeID( void ) const;
  void           HpMinus( u16 value );
  void           HpPlus( u16 value );
  void           HpZero( void );
  PokeTypePair   GetPokeType( void ) const;
  bool           IsMatchType( pml::PokeType type ) const;
  bool           IsWazaHide( void ) const;
  bool           IsUsingFreeFall( void ) const;
  void           SetWeight( u16 weight );
  u16            GetWeight( void ) const;
  bool           ChangePokeType( PokeTypePair type, ExTypeCause exTypeCause );
  void           ExPokeType( pml::PokeType type, ExTypeCause exTypeCause );
  pml::PokeType  GetExType( void ) const;
  bool           HaveExType( void ) const;
  ExTypeCause    GetExTypeCause( void ) const;
  void           ChangeTokusei( TokuseiNo tok );
  void           ChangeForm( u8 formNo, bool dontResetFormByOut=false );
  void           SetWazaSick( WazaSick sick, const BTL_SICKCONT& contParam );
  u8             RankUp( ValueID rankType, u8 volume );
  u8             RankDown( ValueID rankType, u8 volume );
  void           RankSet( ValueID rankType, u8 value );
  bool           RankRecover( void );
  void           RankReset( void );
  bool           RankUpReset( void );
  void           SetBaseStatus( ValueID vid, u16 value );
  void           CopyBatonTouchParams( BTL_POKEPARAM* user );
  bool           IsFakeEnable( void ) const;
  bool           CheckNemuriWakeUp( void );
  bool           CheckKonranWakeUp( void );
  BtlPokePos     GetPrevTargetPos( void ) const;
  WazaID         GetPrevOrgWazaID( void ) const;
  pml::PokeType       GetPrevWazaType( void ) const;
  WazaID         GetPrevWazaID( void ) const;
  bool           IsPrevWazaZenryoku( void ) const;
  u32            GetWazaContCounter( void ) const;
  void           UpdateWazaProcResult( BtlPokePos actTargetPos, pml::PokeType actWazaType, bool fActEnable, bool fActZenryoku, WazaID actWaza, WazaID orgWaza );
  void           CONFRONT_REC_Set( u8 pokeID );
  u8             CONFRONT_REC_GetCount( void ) const;
  u8             CONFRONT_REC_GetPokeID( u8 idx ) const;
  bool           CONFRONT_REC_IsMatch( u8 pokeID ) const;
  void           SetCaptureBallID( u16 ballItemID ) const;
  int            CalcSickDamage( WazaSick sick ) const;
  WazaNo         GetKodawariWazaID( void ) const;

  void           GetPPStatus( BTL_LEVELUP_INFO* info ) const;
  u16            GetTotalTurnCount( void ) const;
  void           IncTotalTurnCount( void );
  u16            GetTurnCount( void ) const;
  u16            GetAppearTurn( void ) const;
  u8             GetFormNo( void ) const{ return m_formNo; }
  u8             GetFriendship( void ) const { return m_friendship; }
  const DefaultPowerUpDesc& GetDefaultPowerUpDesc( void ) const;
  DirtType       GetDirtType( void ) const;
  void           SetDirtType( DirtType dirt );
  DamageCause    GetDeadCause( void ) const;
  u8             GetDeadCausePokeID( void ) const;
  void           SetDeadCause( DamageCause damageCause, u8 damageCausePokeID );
  void           ClearDeadCause( void );
  u8             GetKillCount( void ) const;
  void           SetKillCount( u8 killCount );
  void           IncKillCount( void );
  u8             GetSpActPriority( void ) const;
  void           SetSpActPriority( u8 priority );

  void           Confront_Set( u8 pokeID );
  u8             Confront_GetCount( void ) const;
  u8             Confront_GetPokeID( u8 idx ) const;

  bool        CONTFLAG_Get( ContFlag flagID ) const;
  void        CONTFLAG_Set( ContFlag flagID );
  void        CONTFLAG_Clear( ContFlag flagID );
  ContFlag    CONTFLAG_CheckWazaHide( void ) const;
  void        TURNFLAG_Set( TurnFlag flagID );
  bool        TURNFLAG_Get( TurnFlag flagID ) const;
  void        TURNFLAG_ForceOff( TurnFlag flagID );
  bool        PERMFLAG_Get( PermFlag flagID ) const;
  void        PERMFLAG_Set( PermFlag flagID );

  void        COUNTER_Set( Counter cnt, u8 value );
  void        COUNTER_Inc( Counter cnt );
  u8          COUNTER_Get( Counter cnt ) const;

  void        CombiWaza_ClearParam( void );

  bool        WazaSick_TurnCheck( WazaSick sick, BTL_SICKCONT* pOldContDest, bool* fCured );

  void        MIGAWARI_Create( u16 migawariHP );
  void        MIGAWARI_Delete( void );
  bool        MIGAWARI_IsExist( void ) const;
  u32         MIGAWARI_GetHP( void ) const;
  bool        MIGAWARI_AddDamage( u16* damage );

  bool        HENSIN_Set( const BTL_POKEPARAM* target );
  bool        HENSIN_Check( void ) const;
  bool        HENSIN_Check_Mega( void ) const;
  bool        HENSIN_Check_GensiKaiki(void) const;
  bool        HENSIN_Check_UltraBurst(void) const;

  void        CombiWaza_SetParam( u8 combiPokeID, WazaID combiUsedWaza );
  bool        CombiWaza_GetParam( u8* combiPokeID, WazaID* combiUsedWaza ) const;
  bool        CombiWaza_IsSetParam( void ) const;


  void   SetSrcPP( pml::pokepara::PokemonParam* pp );
  void   ReflectToPP( bool fDefaultForm );
  void   SetAppearTurn( u16 turn );
  void   Clear_ForIn( void );
  void   Clear_ForOut( void );
  void   Clear_ForDead( void );
  bool   AddExp( u32* expRest, BTL_LEVELUP_INFO* info );
  void   CurePokeSick( void );
  void   CureWazaSick( WazaSick sick );
  void   CureWazaSickDependPoke( u8 depend_pokeID );
  bool   AddCriticalRank( int value );
  void   SetCriticalRank( u8 rank );
  u32    GetExpMargin( void ) const;
  void   ReflectByPP( void );

  void   RemoveItem( void );
  void   ConsumeItem( u16 itemID );
  void   ClearConsumedItem( void );
  u16    GetConsumedItem( void ) const;
  fx32   GetHPRatio( void ) const;

  
  bool   WAZA_IsUsable( WazaID waza ) const;
  u8     WAZA_GetPPShort( u8 idx ) const;
  u8     WAZA_GetPPShort_Org( u8 idx ) const;
  bool   WAZA_CheckPPShortAny( void ) const;
  bool   WAZA_CheckPPShortAny_Org( void ) const;
  u16    WAZA_GetPP( u8 wazaIdx ) const;
  u16    WAZA_GetPP_ByNumber( WazaID waza ) const;
  u16    WAZA_GetPP_Org( u8 wazaIdx ) const;
  u16    WAZA_GetMaxPP( u8 wazaIdx ) const;
  u16    WAZA_GetMaxPP_Org( u8 wazaIdx ) const;
  bool   WAZA_IsPPFull( u8 wazaIdx, bool fOrg ) const;
  WazaID WAZA_IncrementPP( u8 wazaIdx, u8 value );
  WazaID WAZA_IncrementPP_Org( u8 wazaIdx, u8 value );
  void   WAZA_DecrementPP( u8 wazaIdx, u8 value );
  void   WAZA_DecrementPP_Org( u8 wazaIdx, u8 value );
  void   WAZA_SetUsedFlag( u8 wazaIdx );
  void   WAZA_SetUsedFlag_Org( u8 wazaIdx );
  bool   WAZA_IsLinkOut( u8 wazaIdx ) const;
  void   WAZA_UpdateID( u8 wazaIdx, WazaID waza, u8 ppMax, bool fPermenent );
  u8     WAZA_SearchIdx( WazaID waza ) const;

  u8      WAZA_GetCount( void ) const;
  u8      WAZA_GetCount_Org( void ) const;
  u8      WAZA_GetUsedCountInAlive( void ) const;
  u8      WAZA_GetUsedCount( void ) const;
  u8      WAZA_GetUsableCount( void ) const;
  WazaID  WAZA_GetID( u8 idx ) const;
  WazaID  WAZA_GetID_Org( u8 idx ) const;
  bool    WAZA_CheckUsedInAlive( u8 idx ) const;
  void    WAZA_Copy( BTL_POKEPARAM* bppDst );
  WazaID  WAZA_GetParticular( u8 idx, u8* PP, u8* PPMax ) const;


  void  WAZADMGREC_Add( const WAZADMG_REC* rec );
  u8    WAZADMGREC_GetCount( u8 turn_ridx ) const;
  bool  WAZADMGREC_Get( u8 turn_ridx, u8 rec_ridx, WAZADMG_REC* dst ) const;

  // メガ進化
  void  MegaEvo( void );
  bool  IsAbleToMegaEvo( void ) const;
  bool  IsMegaEvolved( void ) const;

  //@バトル変更箇所
  // ゲンシカイキ
  void  GensiKaiki(u8 formNo);
  bool  IsGensiKaiki(void ) const;

  //@momijiバトル変更箇所
  // ウルトラバースト
  void  UltraBurst();
  bool  IsUltraBurst(void) const;
  void  ResetUltraBurst();
  bool  IsAbleToUltraBurst(void) const;

  // 努力値・ポケルス
  bool HavePokerus( void ) const;
  void AddEffortPower( pml::pokepara::PowerID id, u8 value );


  #if PM_DEBUG
  void printHPDebug( u32 line );
  #endif


  // ポケモンID->フリーフォールターゲット用ポケIDに変換
//  inline u8 BPP_PokeIDtoFreeFallCounter( u8 pokeID )
  static inline u8 PokeIDtoFreeFallCounter( u8 pokeID )
  {
    return pokeID + 1;  // カウンタ値はデフォルトが0で怖いので、フリーフォール用には0ではない値を使う
  }
  // フリーフォールターゲット用ポケID->通常ポケモンIDに戻す
//  inline u8 BPP_FreeFallCounterToPokeID( u8 counter )
  static inline u8 FreeFallCounterToPokeID( u8 counter )
  {
    if( counter ){
      counter--;
      if( counter < BTL_POKEID_MAX ){
        return counter;
      }
    }
    return BTL_POKEID_NULL;
  }

private:
  //--------------------------------------------------------------
  /**
   *  ワザ
   */
  //--------------------------------------------------------------
  typedef struct {
    WazaNo  number;            ///< ワザナンバー
    u8      pp;                ///< PP値
    u8      ppMax;             ///< PP最大値
    u8      ppCnt;             ///< PP増加数
    u8      usedFlag    : 4;   ///< 使用したフラグ（死亡・入れ替えなどでクリア）
    u8      usedFlagFix : 4;   ///< 使用したフラグ（死亡・入れ替えなどでも保持）
  }WAZA_CORE;

  typedef struct {
    WAZA_CORE  truth;   ///< 真ワザワーク
    WAZA_CORE  surface; ///< 仮ワザワーク
    u8         fLinked; ///< 仮=真である状態ならtrue
  }WAZA_SET;

  //--------------------------------------------------------------
  /**
   *  コアパラメータ  - へんしんしても書き換わらない要素 -
   */
  //--------------------------------------------------------------
  typedef struct {
    pml::pokepara::PokemonParam*  ppSrc;
    const pml::pokepara::PokemonParam*  ppFake;

    u32   exp;
    u16   monsno;
    u16   hpMax;        ///< 最大HP
    u16   hp;
    u16   item;
    u16   usedItem;
    u16   defaultTokusei;
    u8    level;
    u8    myID;
    u8    mons_pow;     ///< 種族値( こうげき )
    u8    mons_agility; ///< 種族値( すばやさ )
    u16   defaultFormNo         : 5;
    u16   fHensin               : 1;
    u16   fFakeEnable           : 1;
    u16   fBtlIn                : 1;
    u16   fDontResetFormByByOut : 1;  ///< true なら、場から退場した際にデフォルトフォルムに戻さない
    u16   padding               : 7;

    BTL_SICKCONT   sickCont[ pml::wazadata::WAZASICK_MAX ];
    u8  wazaSickCounter[ pml::wazadata::WAZASICK_MAX ];

    u8  confrontRecCount;
    u8  confrontRec[ BTL_POKEID_MAX ];

    u16 totalTurnCount;        // 場に出たターンの総数( １ターン中に何回場に出ても１回とみなす )
    u8  fakeViewTargetPokeId;  // イリュージョン対象ポケモンのID

    DefaultPowerUpDesc defaultPowerUpDesc;  // 場に出た時点で適用される能力アップの設定( "ぬし" や "パラレルビースト" など )
    DirtType           dirtType;            // 汚れの種類

    DamageCause deadCause;        // 倒された原因
    u8          deadCausePokeID;  // 倒された原因となったポケモンのID
    u8          killCount;        // 相手を倒した数

  }CORE_PARAM;

  void CORE_PARAM_Copy( CORE_PARAM* dest, const CORE_PARAM& src ) const;

  //--------------------------------------------------------------
  /**
   *  基本パラメータ
   */
  //--------------------------------------------------------------
  typedef struct {

    u16 monsno;       ///< ポケモンナンバー

    u16 attack;       ///< こうげき
    u16 defence;      ///< ぼうぎょ
    u16 sp_attack;    ///< とくこう
    u16 sp_defence;   ///< とくぼう
    u16 agility;      ///< すばやさ

  //  u8  level;        ///< レベル
    u8  type1;        ///< タイプ１
    u8  type2;        ///< タイプ２
    u8  type_ex;      ///< 拡張タイプ
    u8  sex;          ///< 性別

    ExTypeCause type_ex_cause; // タイプ拡張の原因

  }BASE_PARAM;

  //--------------------------------------------------------------
  /**
   *  ランク効果
   */
  //--------------------------------------------------------------
  typedef struct {

    s8  attack;     ///< こうげき
    s8  defence;    ///< ぼうぎょ
    s8  sp_attack;  ///< とくこう
    s8  sp_defence; ///< とくぼう
    s8  agility;    ///< すばやさ
    s8  hit;        ///< 命中率
    s8  avoid;      ///< 回避率

  }VARIABLE_PARAM;

  enum {

    TURNFLG_BUF_SIZE = (TURNFLG_MAX/8)+(TURNFLG_MAX%8!=0),
    CONTFLG_BUF_SIZE = (CONTFLG_MAX/8)+(CONTFLG_MAX%8!=0),
    PERMFLG_BUF_SIZE = (PERMFLAG_MAX/8)+(PERMFLAG_MAX%8!=0),

    TURNCOUNT_NULL = BTL_TURNCOUNT_MAX+1,
  };

  //--------------------------------------------------------------
  /**
   *  努力値
   */
  //--------------------------------------------------------------
  typedef struct {

    u16   srcSum;   ///< 本体の努力値合計
    u8    srcHp;    ///< 本体のHP努力値
    u8    srcPow;   ///< 本体のこうげき努力値
    u8    srcDef;   ///< 本体のぼうぎょ努力値
    u8    srcAgi;   ///< 本体のすばやさ努力値
    u8    srcSpPow; ///< 本体のとくこう努力値
    u8    srcSpDef; ///< 本体のとくぼう努力値

    u8    bPokerus;  /// ポケルス感染しているフラグ
    u8    bModified; /// 編集したフラグ

  }DORYOKU_PARAM;

  CORE_PARAM      m_coreParam;
  BASE_PARAM      m_baseParam;
  VARIABLE_PARAM  m_varyParam;
  DORYOKU_PARAM   m_doryokuParam;
  WAZA_SET        m_waza[ PTL_WAZA_MAX ];

  u16  m_tokusei;
  u16  m_weight;

  u8  m_wazaCnt;
  u8  m_formNo;
  u8  m_friendship;
  u8  m_criticalRank;
  u8  m_usedWazaCount;
  u8  m_prevWazaType;
  u8  m_spActPriority;     ///< 行動の特殊優先度

  u16 m_turnCount;                  ///< 継続して戦闘に出ているカウンタ
  u16 m_appearedTurn;               ///< 戦闘に出たターンを記録
  u16 m_wazaContCounter;            ///< 同ワザ連続成功カウンタ（prevActWazaIDのワザを何回連続で成功させたか。直前失敗ならゼロ）

  BtlPokePos  m_prevTargetPos;          ///< 直前に狙った相手
  WazaNo      m_prevActWazaID;          ///< 直前に実行されたワザID（派生ワザを使った場合にprevSelectWazaIDと異なる）
  bool        m_isPrevActWazaZenryoku;  ///< 直前に実行されたワザが全力技か？
  WazaNo      m_prevSelectWazaID;       ///< 直前に選択したワザID（WAZALOCK状態等で強制的に選択されたワザ > クライアントが選択した所持ワザ）
  WazaNo      m_prevDamagedWaza;        ///< 直前にダメージを受けたワザ

  u8               m_turnFlag[ TURNFLG_BUF_SIZE ];
  u8               m_contFlag[ CONTFLG_BUF_SIZE ];
  u8               m_permFlag[ PERMFLG_BUF_SIZE ];
  u8               m_counter[  COUNTER_MAX ];
  WAZADMG_REC      m_wazaDamageRec[ WAZADMG_REC_TURN_MAX ][ WAZADMG_REC_MAX ];
  u8               m_dmgrecCount[ WAZADMG_REC_TURN_MAX ];
  u8               m_dmgrecTurnPtr;
  u8               m_dmgrecPtr;

  u16 m_migawariHP;

  WazaID  m_combiWazaID;
  u8      m_combiPokeID;

  const   FieldStatus*  m_fldSim;
//  u32 dmy;

  #if PM_DEBUG
  u8  m_pokeID_forDebugger;  ///< m_coreParam.myID と同じ値（ここに置いておくとデバッガで確認がとてもラク）
  #endif


  //-----------------------------------------------------------
  //-----------------------------------------------------------

  void henshinCopyFrom( const BTL_POKEPARAM& src );

  void setupBySrcData( const pml::pokepara::PokemonParam* srcPP, bool fReflectHP, bool fTokuseiUpdate );
  void setupBySrcDataBase( const pml::pokepara::PokemonParam* srcPP, bool fTypeUpdate );
  void clearHensin( void );

  u32  wazaWork_setupByPP( const pml::pokepara::PokemonParam* pp_src, bool fLinkSurface );
  void wazaWork_ReflectToPP( void );
  void wazaWork_ReflectFromPP( void );
  void wazaWork_ClearSurface( void );
  void wazaSet_ClearUsedFlag( WAZA_SET* waza );
  bool wazaCore_SetupByPP( WAZA_CORE* core, pml::pokepara::PokemonParam* pp, u8 index );

  void effrank_Init( VARIABLE_PARAM* rank );
  void effrank_Reset( VARIABLE_PARAM* rank );
  bool effrank_ResetRankUp( VARIABLE_PARAM* rank );
  bool effrank_Recover( VARIABLE_PARAM* rank );

  /**
   *  状態以上ワークのクリア方法（BitFlagです）
   */
  enum SickWorkClearCode {
    SICKWORK_CLEAR_ALL = 0,
    SICKWORK_CLEAR_WITHOUT_SLEEP = 1,
    SICKWORK_CLEAR_ONLY_WAZASICK = 2,
  };

  void clearWazaSickWork( uint32_t  clearCode );
  void clearCounter( void );
  void clearUsedWazaFlag( void );

  // ワザダメージレコード
  void dmgrec_ClearWork( void );
  void dmgrec_FwdTurn( void );
  // 対面記録
  void confrontRec_Clear( void );

  ValueID     convertValueID( ValueID vid ) const;
  s8          getRankVaryStatus( ValueID type, s8* min, s8* max ) const;
  void        cureDependSick( WazaSick sickID  );
  void        splitTypeCore( pml::PokeType* type1, pml::PokeType* type2 ) const;
  void        correctMaxHP( void );

  void   wazaWork_UpdateNumber( WAZA_SET* waza, WazaID nextNumber, u8 ppMax, bool fPermenent );
  void   wazaCore_UpdateNumber( WAZA_CORE* core, WazaID nextID, u8 ppMax );

  // 努力値関連
  void   doryoku_InitParam( DORYOKU_PARAM* work, pml::pokepara::PokemonParam* pp );
  void   doryoku_AddPower( DORYOKU_PARAM* work,  pml::pokepara::PowerID powID, u8 value );
  void   doryoku_PutToPP( DORYOKU_PARAM* work, pml::pokepara::PokemonParam* pp );
  u8*    doryoku_ParamIDtoValueAdrs( DORYOKU_PARAM* work, pml::pokepara::PowerID powID) const;

  void updateWeight( void );
  void resetSpActPriority( void );


  //---------------------------------------------------------------------------------------------
  // bitフラグバッファ処理
  //---------------------------------------------------------------------------------------------
  inline void flgbuf_clear( u8* buf, u32 size )
  {
    gfl2::std::MemClear( buf, size );
  }

  inline void flgbuf_set( u8* buf, u32 flagID )
  {
    u8 byte = flagID / 8;
    u8 bit = (1 << (flagID%8));
    buf[ byte ] |= bit;
  }

  inline void flgbuf_reset( u8* buf, u32 flagID )
  {
    u8 byte = flagID / 8;
    u8 bit = (1 << (flagID%8));
    buf[ byte ] &= (~bit);
  }

  inline bool flgbuf_get( const u8* buf, u32 flagID ) const
  {
    u8 byte = flagID / 8;
    u8 bit = (1 << (flagID%8));
    return (buf[ byte ] & bit) != 0;
  }


}; // class BTL_POKEPARAM

GFL_NAMESPACE_END(btl)
#endif    // #ifndef __BTL_POKEPARAM_H__
