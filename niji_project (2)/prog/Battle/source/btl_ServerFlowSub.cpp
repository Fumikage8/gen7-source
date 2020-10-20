//=============================================================================================
/**
 * @file  btl_ServerFlowSub.cpp
 * @brief ポケモンXY バトルシステム サーバコマンド生成処理サブ
 * @author  taya
 *
 * @date  2011.02.01  作成
 */
//=============================================================================================
#include <niji_conv_header/message/msg_btl_std.h>
#include <niji_conv_header/message/msg_btl_set.h>
#include <pml/include/pmlib.h>
#include <pml/include/item/item.h>
#include <Savedata/include/MyStatus.h>
#include <GameSys/include/TimeZone.h>

#include "./btl_common.h"
#include "./btl_MainModule.h"
#include "./btl_event.h"
#include "./btl_str.h"
#include "./btl_calc.h"
#include "./btl_tables.h"
#include "./btl_server.h"
#include "./btl_server_cmd.h"
#include "./btl_ServerFlow.h"
#include "./btl_ServerFlowSub.h"
#include "./btl_ExpCalculator.h"
#include "./btl_WazaTargetSelector.h"
#include "./btl_IntrudeSystem.h"
#include "./btl_UltraBeast.h"
#include "./btl_Capture.h"
#include "./btl_RecordSave.h"

#include "./handler/hand_side.h"
#include "./handler/hand_common.h"

#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

GFL_NAMESPACE_BEGIN(btl)


u8 ServerFlowSub::RegisterTargets( ServerFlow* flwk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
              const SVFL_WAZAPARAM* wazaParam, PokeSet* rec )
{
  BtlRule rule = flwk->m_mainModule->GetRule();

  u8 intrPokeID = BTL_POKEID_NULL;
  bool isTargetTemptEnable = false;


  if( !(wazaParam->fMagicCoat) ){
    intrPokeID = scEvent_GetWazaTargetIntr( flwk, attacker, wazaParam );
  }

  rec->Clear();
  flwk->m_defaultTargetPos = BTL_POS_NULL;

  WazaTargetSelector::SelectWazaTargets( flwk->m_mainModule, flwk->m_pokeCon, attacker, targetPos, wazaParam, intrPokeID, rec, &isTargetTemptEnable );


  // 技「このゆびとまれ」特性「よびみず」等による技ターゲットの割り込み
  if( ( rec->GetCount() == 1 ) &&
      ( isTargetTemptEnable ) )
  {
    BTL_POKEPARAM* target = checkTemptPokemon( flwk, rec->Get(0), attacker, wazaParam );
    rec->Clear();
    rec->Add( target );
  }

  if( rec->GetCount() == 1 )
  {
    BTL_POKEPARAM* bpp = rec->Get( 0 );
    if( bpp ){
      flwk->m_defaultTargetPos = flwk->m_posPoke.GetPokeLastPos( bpp->GetID() );
    }
    if( correctTargetDead(flwk, rule, attacker, wazaParam, targetPos, rec) == false ){
    }
  }

  return rec->GetCount();
}


/**
 *  対象ポケモンが死んでいた時の対象修正
 *
 *  @retval   bool 補正の必要なし or 補正できたらtrue ／ 補正対象個所にもポケモンが居ない場合false
 */
bool ServerFlowSub::correctTargetDead( ServerFlow* flwk, BtlRule rule, const BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, BtlPokePos targetPos, PokeSet* rec )
{
  // 戦闘位置が陣営あたり２以上（ダブル、乱入）でないなら補正不要
  u8 numFrontPos = flwk->m_mainModule->GetOpponentFrontPosNum( flwk->m_mainModule->PokeIDtoClientID(attacker->GetID()) );
  if( numFrontPos <= 1 ){
    return true;
  }

  // 元々の対象が１体でないなら補正不要
  if( rec->GetCount() != 1 ){
    return true;
  }

  // 元々の対象が攻撃ポケの敵ではない or 生きているなら補正不要
  BTL_POKEPARAM* defaulTarget = rec->Get( 0 );
  if( !(defaulTarget->IsDead())
  ||  (flwk->m_mainModule->IsFriendPokeID(attacker->GetID(), defaulTarget->GetID()))
  ){
    return true;
  }

  // 以下、元々の対象が攻撃ポケの敵であり、かつ死んでいる場合のチェック
  {
    u8 nextTargetPos[ BTL_EXIST_POS_MAX ];
    BTL_POKEPARAM* nextTarget[ BTL_POSIDX_MAX ];

    u8 aliveCnt;


    // 元々の対象は死んでるので除外しておく
    rec->Remove( defaulTarget );

    // 同範囲の生きているポケモンをリストアップ
    aliveCnt = correctTargetDeadSub_storeNextTargets(
                  flwk,
                  nextTargetPos,
                  nextTarget,    GFL_NELEMS(nextTarget),
                  attacker,
                  wazaParam
               );

    // 生きているポケモンがいなければ補正失敗
    if( aliveCnt == 0 ){
      return false;
    }

    u32  targetIdx = correctTargetDeadSub_decideTarget( targetPos, aliveCnt, nextTarget, nextTargetPos );

    // 補正完了
    if( nextTarget[targetIdx] != NULL ){
      rec->Add( nextTarget[ targetIdx ] );
      return true;
    }
  }

  return false;
}

//=============================================================================================
/**
 * 補正後の対象ポケモン候補から１体を選ぶ
 *
 * @param   defaultTargetPos    元々の狙った位置ID
 * @param   targetListCount     狙うポケモンの候補数
 * @param   targets   狙うポケモンの候補
 * @param   targetPos 狙うポケモンの位置ID（targets と同期している）
 *
 * @retval  u8    狙うポケモンのIndex （ targets 内 ）
 */
//=============================================================================================
u8 ServerFlowSub::correctTargetDeadSub_decideTarget(
   BtlPokePos   defaultTargetPos,
   u8   targetListCount,
   BTL_POKEPARAM**  targets,
   u8*  targetPos
){

  // 候補が２体じゃなければ１体のはずなのでリストの先頭を狙う
  if( targetListCount != 2 ){
    return 0;
  }

  // 以下、候補が２体の場合
  // 元々のターゲットの位置を狙う
  for( u32 i=0; i<targetListCount; ++i )
  {
    if( targetPos[i] == defaultTargetPos )
    {
      return i;
    }
  }

  GFL_ASSERT(0);
  return 0;
}

//=============================================================================================
/**
 * ワザの対象ポケモンが死んでいた時、そのワザの対象範囲に含まれる他のポケモンを列挙する
 *
 * @param        flwk                     ServerFlow
 * @param[out]   dstNextTargetPos         対象範囲の位置IDを格納する配列
 * @param[out]   dstNextTarget            対象範囲のポケモンパラメータを格納する配列
 * @param        dstNextTargetElems       dstNextTarget の要素数
 * @param        attacker                 攻撃ポケモンパラメータ
 * @param        wazaParam                使用ワザパラメータ
 *
 * @retval  u8    列挙されたポケモン数
 */
//=============================================================================================
u8 ServerFlowSub::correctTargetDeadSub_storeNextTargets(
  ServerFlow* flwk,
  u8* dstNextTargetPos,
  BTL_POKEPARAM**  dstNextTarget, u8 dstNextTargetElems,
  const BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam )
{
  BtlPokePos  atPos = flwk->m_mainModule->PokeIDtoPokePos( flwk->m_pokeCon, attacker->GetID() );
  BtlExPos    exPos;

  // 元々の対象範囲（トリプルも想定）
  if( WAZADATA_GetFlag(wazaParam->wazaID, pml::wazadata::WAZAFLAG_TripleFar) ){
    exPos = EXPOS_MAKE( BTL_EXPOS_FULL_ENEMY, atPos );
  }else{
    exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, atPos );
  }

  // 元々の対象範囲から生きてるポケを列挙
  u8 nextTargetPosCnt = flwk->m_mainModule->ExpandBtlPos( exPos, dstNextTargetPos );

  for(u32 i=0; i<dstNextTargetElems; ++i){
    dstNextTarget[i] = NULL;
  }

  u8 aliveCnt = 0;
  for(u32 i=0; i<nextTargetPosCnt; ++i)
  {
    dstNextTarget[aliveCnt] = flwk->m_pokeCon->GetFrontPokeData( dstNextTargetPos[i] );
    if( !dstNextTarget[aliveCnt]->IsDead() ){
      dstNextTargetPos[ aliveCnt ] = dstNextTargetPos[i]; // 位置情報もIndexを揃えておく
      ++aliveCnt;
    }
  }

  return aliveCnt;
}

/**
 *  「このゆびとまれ」などで強制的にターゲットとして割り込むポケモンのチェック
 *
 *  @return   割り込みがあれば割り込みポケモンパラメータ／なければ target がそのまま返る
 */
BTL_POKEPARAM* ServerFlowSub::checkTemptPokemon( ServerFlow* flwk, BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam )
{
  u8 targetPokeID = target->GetID( );
  u8 temptPokeID = checkTemptPokeID( flwk, attacker, wazaParam, targetPokeID );
  if( temptPokeID != BTL_POKEID_NULL ){
    target = flwk->m_pokeCon->GetPokeParam( temptPokeID );
  }
  BTL_N_Printf( DBGSTR_SVFL_DoubleTargetRegister, target->GetID() );
  return target;
}
/**
 *  ターゲット割り込みポケモンIDを取得
 */
u8 ServerFlowSub::checkTemptPokeID( ServerFlow* flwk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, u8 targetPokeID )
{
  TemptTargetCause temptCause;

  u32 hem_state = flwk->m_HEManager.PushState();
  u8 temptPokeID = scEvent_GetWazaTargetTempt( flwk, attacker, wazaParam, targetPokeID, &temptCause );
  scEvent_WazaTargetTemptEnd( flwk, attacker, temptPokeID, wazaParam, temptCause );
  flwk->m_HEManager.PopState( hem_state );
  return temptPokeID;
}

//----------------------------------------------------------------------------------
/**
 * [Event] ワザターゲットを自動決定（ターゲット動的確定の前）
 *
 * @param   flwk
 * @param   attacker
 * @param   wazaParam
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
u8 ServerFlowSub::scEvent_GetWazaTargetIntr( ServerFlow* flwk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam )
{
  u8 pokeID = BTL_POKEID_NULL;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_POKEID_DEF, pokeID );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FIX_FLAG, false );
    EVENT_CallHandlers( flwk, BTL_EVENT_DECIDE_TARGET );
    pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  EVENTVAR_Pop();
  return pokeID;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザターゲットを強制書き換え（ターゲット動的確定の後、さらに書き換え -> このゆびとまれなど）
 *
 * @param      flwk
 * @param      attacker
 * @param      wazaParam
 * @param[out] outTemptCause  ターゲット誘導の要因( 誘導が起こらなかった場合, TEMPT_TARGET_PRIORITY_NULL )
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
u8 ServerFlowSub::scEvent_GetWazaTargetTempt( ServerFlow* flwk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, u8 defaultTargetPokeID, TemptTargetCause* outTemptCause )
{
  u8 pokeID;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetValue( BTL_EVAR_TEMPT_TARGET_CAUSE, TEMPT_TARGET_CAUSE_NONE );
    EVENTVAR_SetValue( BTL_EVAR_TEMPT_TARGET_PRIORITY, TEMPT_TARGET_PRIORITY_NULL );
    EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, defaultTargetPokeID );
    EVENT_CallHandlers( flwk, BTL_EVENT_TEMPT_TARGET );
    pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    *outTemptCause = static_cast<TemptTargetCause>( EVENTVAR_GetValue( BTL_EVAR_TEMPT_TARGET_CAUSE ) );
  EVENTVAR_Pop();

  if( pokeID != defaultTargetPokeID ){
    return pokeID;
  }
  return BTL_POKEID_NULL;
}
//----------------------------------------------------------------------------------
/**
 * @brief [Event] ワザターゲットを強制書き換え終了
 * @param flwk
 * @param attacker      技を撃つポケモン
 * @param temptPokeId   技を引き付けたポケモンのID
 * @param wazaParam     使用する技
 * @param temptCause    ターゲット誘導の要因
 */
//----------------------------------------------------------------------------------
void ServerFlowSub::scEvent_WazaTargetTemptEnd( ServerFlow* flwk, const BTL_POKEPARAM* attacker, u16 temptPokeId, const SVFL_WAZAPARAM* wazaParam, TemptTargetCause temptCause )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, temptPokeId );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_TEMPT_TARGET_CAUSE, temptCause );
    EVENT_CallHandlers( flwk, BTL_EVENT_TEMPT_TARGET_END );
  EVENTVAR_Pop();
}



/*====================================================================================================*/
/*                                                                                                    */
/*  トレーナーアイテム使用                                                                            */
/*                                                                                                    */
/*====================================================================================================*/


//=============================================================================================
/**
 *
 *
 * @param   flwk
 * @param   bpp
 * @param   itemID
 * @param   actParam
 * @param   targetIdx
 *
 * @retval  TrItemResult
 */
//=============================================================================================
TrItemResult ServerFlowSub::TrainerItemProc( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID,
                        u8 actParam, u8 shortPokeID, POKE_CAPTURED_CONTEXT* capContext )
{
  enum {
    RANGE_FULL,   ///< 前衛・後衛・控えに全て効く
    RANGE_VIEW,   ///< 前衛・後衛（画面に見えている）に効く
    RANGE_FRONT,  ///< 前衛にのみ効く
  };
  enum {
    AREA_FRONT,    ///< 前衛
    AREA_BACK,     ///< 後衛（ローテのみ）
    AREA_RESERVE,  ///< 控え
  };

  typedef u8 (ServerFlowSub::*pItemEffFunc)( ServerFlow*, BTL_POKEPARAM*, u16, int, u8 );


  // 通常アイテム効果＆範囲テーブル
  static const struct {
    u16            effect;
    u8             range;
    u8             fShooterOnly;
    pItemEffFunc   func;
  }ItemEffectTable[] = {
    { item::ITEM_DATA::PRM_ID_SLEEP_RCV,     RANGE_FULL,  false, &ServerFlowSub::ItemEff_SleepRcv      },   // 眠り回復
    { item::ITEM_DATA::PRM_ID_POISON_RCV,    RANGE_FULL,  false, &ServerFlowSub::ItemEff_PoisonRcv     },   // 毒回復
    { item::ITEM_DATA::PRM_ID_BURN_RCV,      RANGE_FULL,  false, &ServerFlowSub::ItemEff_YakedoRcv     },   // 火傷回復
    { item::ITEM_DATA::PRM_ID_ICE_RCV,       RANGE_FULL,  false, &ServerFlowSub::ItemEff_KooriRcv      },   // 氷回復
    { item::ITEM_DATA::PRM_ID_PARALYZE_RCV,  RANGE_FULL,  false, &ServerFlowSub::ItemEff_MahiRcv       },   // 麻痺回復
    { item::ITEM_DATA::PRM_ID_PANIC_RCV,     RANGE_FULL,  false, &ServerFlowSub::ItemEff_KonranRcv     },   // 混乱回復
    { item::ITEM_DATA::PRM_ID_MEROMERO_RCV,  RANGE_FULL,  false, &ServerFlowSub::ItemEff_MeromeroRcv   },   // メンタル回復
    { item::ITEM_DATA::PRM_ID_ABILITY_GUARD, RANGE_VIEW,  false, &ServerFlowSub::ItemEff_EffectGuard   },   // 能力ガード
    { item::ITEM_DATA::PRM_ID_DEATH_RCV,     RANGE_FULL,  false, &ServerFlowSub::ItemEff_Relive        },   // 瀕死回復
    { item::ITEM_DATA::PRM_ID_ATTACK_UP,     RANGE_VIEW,  false, &ServerFlowSub::ItemEff_AttackRank    },   // 攻撃力アップ
    { item::ITEM_DATA::PRM_ID_DEFENCE_UP,    RANGE_VIEW,  false, &ServerFlowSub::ItemEff_DefenceRank   },   // 防御力アップ
    { item::ITEM_DATA::PRM_ID_SP_ATTACK_UP,  RANGE_VIEW,  false, &ServerFlowSub::ItemEff_SPAttackRank  },   // 特攻アップ
    { item::ITEM_DATA::PRM_ID_SP_DEFENCE_UP, RANGE_VIEW,  false, &ServerFlowSub::ItemEff_SPDefenceRank },   // 特防アップ
    { item::ITEM_DATA::PRM_ID_AGILITY_UP,    RANGE_VIEW,  false, &ServerFlowSub::ItemEff_AgilityRank   },   // 素早さアップ
    { item::ITEM_DATA::PRM_ID_HIT_UP,        RANGE_VIEW,  false, &ServerFlowSub::ItemEff_HitRank       },   // 命中率アップ
    { item::ITEM_DATA::PRM_ID_CRITICAL_UP,   RANGE_VIEW,  false, &ServerFlowSub::ItemEff_CriticalUp    },   // クリティカル率アップ
    { item::ITEM_DATA::PRM_ID_PP_RCV,        RANGE_FULL,  false, &ServerFlowSub::ItemEff_PP_Rcv        },   // PP回復
    { item::ITEM_DATA::PRM_ID_ALL_PP_RCV,    RANGE_FULL,  false, &ServerFlowSub::ItemEff_AllPP_Rcv     },   // PP回復（全ての技）
    { item::ITEM_DATA::PRM_ID_HP_RCV,        RANGE_FULL,  false, &ServerFlowSub::ItemEff_HP_Rcv        },   // HP回復
  };

  u8 clientID = MainModule::PokeIDtoClientID( bpp->GetID() );
  u8 targetPokeID = BTL_POKEID_NULL;
  BTL_POKEPARAM* target = NULL;
  u8 targetArea = AREA_FRONT;
  BtlPokePos targetPos = BTL_POS_NULL;
  int i, itemParam;
  u32 hem_state;


  // 対象ポケモンの各種情報決定
  if( shortPokeID != BTL_PARTY_MEMBER_MAX )
  {
    targetPokeID = MainModule::ShortIDtoPokeID( clientID, shortPokeID );

    BTL_PARTY* party = flwk->m_pokeCon->GetPartyData( clientID );
    u8 frontPosCount = flwk->m_mainModule->GetClientFrontPosCount( clientID );   
    u8 targetIdx = party->GetMemberIndex( targetPokeID );

    target = flwk->m_pokeCon->GetPokeParam( targetPokeID );
    targetPos = flwk->m_posPoke.GetPokeExistPos( targetPokeID );
    targetArea = (targetIdx < frontPosCount)? AREA_FRONT : AREA_RESERVE;
  }

  // ○○は××を使った！  メッセージ
  SCQUE_PUT_MSG_STD( flwk->m_que, BTL_STRID_STD_UseItem_Self, clientID, itemID );


  // ビビリだま
  if( itemID == ITEM_BIBIRIDAMA )
  {
    if( ( flwk->m_intrudeSystem == NULL ) ||
        ( flwk->m_intrudeSystem->IsBibiridamaUsed() ) )
    {
      SCQUE_PUT_MSG_STD( flwk->m_que, BTL_STRID_STD_UseItem_NoEffect );  // しかし　こうかが　なかった！
    }
    else
    {
      flwk->m_intrudeSystem->SetBibiridamaUsed();
      flwk->m_mainModule->DecrementPlayerItem( clientID, itemID );
      SCQUE_PUT_MSG_STD( flwk->m_que, BTL_STRID_STD_Bibiridama ); // ビビリだま　は　野生のポケモンを　不安にさせる！
    }

    return TRITEM_RESULT_NORMAL;
  }

  // つかまえぽん
  if (itemID == ITEM_TUKAMAEPON)
  {
    // 既に使用済み
    // 野生戦ではない
    // ぬし戦である
    // 試練中である
    // 保護区である
    // 捕獲不可戦闘である
    // 上記の場合は失敗
    if (flwk->IsUsedTukamaePon() || 
        flwk->m_mainModule->GetCompetitor() != BTL_COMPETITOR_WILD ||
        flwk->m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_VS_NUSI) ||
        flwk->m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_SIREN_BATTLE) ||
        flwk->m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_IN_POKE_SANCTUARY) ||
        flwk->m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_NO_CAPTURE))
    {
      SCQUE_PUT_MSG_STD(flwk->m_que, BTL_STRID_STD_UseItem_NoEffect);  // しかし　こうかが　なかった！
    }
    else
    {
      flwk->UseTukamaePon();
      flwk->UseItemPon();
      u16  que_reserve_pos = scmd::SCQUE_RESERVE_Pos(flwk->m_que, SC_ACT_EFFECT_BYPOS);
      scmd::SCQUE_PUT_ReservedPos(flwk->m_que, que_reserve_pos, SC_ACT_EFFECT_BYPOS, targetPos, BTLEFF_ROTOM_POWER);
      SCQUE_PUT_MSG_STD(flwk->m_que, BTL_STRID_STD_Captureup);  // ポケモンを　つかまえやすくなった！
      // アイテム消費
      flwk->m_mainModule->DecrementPlayerItem(clientID, itemID);
    }
    return TRITEM_RESULT_NORMAL;
  }

  // かいふくポン・経験値ポン・ほじょぽん
  if (itemID >= ITEM_KAIHUKUPON && itemID <= ITEM_HOZYOPON)
  {
    static const struct {
      u16            effect;
      pItemEffFunc   func;
    }RotoponItemEffectTable[] = {
      { item::ITEM_DATA::PRM_ID_HP_RCV, &ServerFlowSub::ItemEff_HP_Rcv },                // HP回復
      { item::ITEM_DATA::PRM_ID_ALL_PP_RCV, &ServerFlowSub::ItemEff_AllPP_Rcv },         // PP回復（全ての技）
      { item::ITEM_DATA::PRM_ID_ATTACK_UP, &ServerFlowSub::ItemEff_AttackRank },         // 攻撃力アップ
      { item::ITEM_DATA::PRM_ID_DEFENCE_UP, &ServerFlowSub::ItemEff_DefenceRank },       // 防御力アップ
      { item::ITEM_DATA::PRM_ID_SP_ATTACK_UP, &ServerFlowSub::ItemEff_SPAttackRank },    // 特攻アップ
      { item::ITEM_DATA::PRM_ID_SP_DEFENCE_UP, &ServerFlowSub::ItemEff_SPDefenceRank },  // 特防アップ
      { item::ITEM_DATA::PRM_ID_AGILITY_UP, &ServerFlowSub::ItemEff_AgilityRank },       // 素早さアップ
    };
    const u32 FRONT_POKEMON_MAX = 2;  // 場に出ているポケモン最大数
    // @fix MMCat[205]マルチバトルでロトムパワーを使用すると、とまる
    const u32 frontPosNum = flwk->m_mainModule->GetClientFrontPosCount(clientID);
    BTL_POKEPARAM* targets[FRONT_POKEMON_MAX] = { NULL, NULL };
    u32 targetsPokeIDIndex = 0;       // 場に出ているポケモン数
    // 場に出ているポケモン検索
    for (u32 posIndex(0); posIndex < frontPosNum; ++posIndex)
    {
      BtlPokePos pos = flwk->m_mainModule->GetClientPokePos(clientID, posIndex);
      BTL_POKEPARAM* poke = flwk->m_pokeCon->GetFrontPokeData(pos);
      // 場に出ている
      if (flwk->m_posPoke.GetExistPokeID(pos) != BTL_POKEID_NULL && poke != NULL)
      {
        targets[targetsPokeIDIndex] = poke;
        targetPos = pos;
        ++targetsPokeIDIndex;
      }
    }

    // 効果発動処理
    bool fEffectivePoke[FRONT_POKEMON_MAX] = { false, false };
    bool fEffective = false;
    u16  que_reserve_pos = scmd::SCQUE_RESERVE_Pos(flwk->m_que, SC_ACT_EFFECT_BYPOS);
    for (i = 0; i < targetsPokeIDIndex; ++i)
    {
      hem_state = flwk->m_HEManager.PushState();
      for (u32 tableIndex(0); tableIndex < GFL_NELEMS(RotoponItemEffectTable); ++tableIndex)
      {
        itemParam = calc::ITEM_GetParam(itemID, RotoponItemEffectTable[tableIndex].effect);
        if (itemParam)
        {
          BTL_PRINT("アイテムID=%d, 効果=%dあり\n", itemID, itemParam);
          if ((this->*(RotoponItemEffectTable[tableIndex].func))(flwk, targets[i], itemID, itemParam, actParam)){
            fEffective = true;
            fEffectivePoke[i] = true;
          }
        }
      }
      flwk->m_HEManager.PopState(hem_state);
    }
    // 使用効果あり
    if (fEffective)
    {
      scmd::SCQUE_PUT_ReservedPos(flwk->m_que, que_reserve_pos, SC_ACT_EFFECT_BYPOS, targetPos, BTLEFF_ROTOM_POWER);
      // 効果発動したポケモンだけなつき度アップ
      for (i = 0; i < targetsPokeIDIndex; ++i)
      {
        if (fEffectivePoke[i])
        {
          flwk->m_mainModule->CalcNatsukiItemUse(targets[i], itemID);
        }
      }
      // アイテム消費
      flwk->m_mainModule->DecrementPlayerItem(clientID, itemID);
      flwk->UseItemPon();
    }
    else
    {
      SCQUE_PUT_MSG_STD(flwk->m_que, BTL_STRID_STD_UseItem_NoEffect);
    }
    return TRITEM_RESULT_NORMAL;
  }

  // ボール投げならボール投げシーケンスへ
  if( calc::ITEM_GetParam(itemID, item::ITEM_DATA::PRM_ID_ITEM_TYPE) == ITEMTYPE_BALL )
  {
    RecordSave::IncBallThrowCount( *flwk->m_mainModule );

    bool bUsedBall, bCaptured;

    bCaptured = scproc_TrainerItem_BallRoot( flwk, bpp, itemID, &bUsedBall, capContext );
    if( bUsedBall ){
     flwk->m_mainModule->DecrementPlayerItem( clientID, itemID );
    }

    if( bCaptured ){
      return TIITEM_RESULT_CAPTURED;
    }else{
      return TRITEM_RESULT_NORMAL;
    }
  }
  // 逃げアイテムなら逃げシーケンスへ
  if( calc::ITEM_GetParam(itemID, item::ITEM_DATA::PRM_ID_B_FUNC) == ITEMUSE_BTL_ESCAPE )
  {
    flwk->m_mainModule->DecrementPlayerItem( clientID, itemID );
    return TRITEM_RESULT_ESCAPE;
  }


  // その場に効く道具か判定
  {
    // 前衛にあたるIndexで生きてるなら何でも効く
    bool fEffective = ((targetArea == AREA_FRONT) && (targetPos!=BTL_POS_NULL));

    BTL_PRINT("checking effective item = %d,  area=%d, pos=%d\n", fEffective, targetArea, targetPos );

    if( fEffective == false )
    {
      for(i=0; i<GFL_NELEMS(ItemEffectTable); ++i)
      {
        if( calc::ITEM_GetParam(itemID, ItemEffectTable[i].effect) )
        {
          u8 range = ItemEffectTable[i].range;

          // アイテム自体が何処でもOK型 -> 効く
          if( range == RANGE_FULL ){
            fEffective = true;
            break;
          }
          // アイテムが可視範囲のみOK型 -> 控えでなく生きてるなら効く
          if( range == RANGE_VIEW )
          {
            if( (targetArea < AREA_RESERVE)
            &&  (targetPos != BTL_POS_NULL)
            ){
              fEffective = true;
              break;
            }
          }
        }
      }
    }

    // 効かないよ
    if( !fEffective ){
      SCQUE_PUT_MSG_STD( flwk->m_que, BTL_STRID_STD_ItemNoEffPos );
      return TRITEM_RESULT_NORMAL;
    }
  }

  // 回復アイテムの使用回数をインクリメント
  RecordSave::IncRecoverItemUseCount( *flwk->m_mainModule, itemID );

  // 通常のアイテム処理
  {
    bool fEffective = false;
    u16 effectNo = ( targetPos == BTL_POS_NULL ) ? ( BTLEFF_BENCH_RECOVERY ) : ( BTLEFF_USE_ITEM );
    ServerCmd serverCmd = ( effectNo == BTLEFF_BENCH_RECOVERY ) ? ( SC_ACT_EFFECT_SIMPLE ) : ( SC_ACT_EFFECT_BYPOS );
    u16  que_reserve_pos = scmd::SCQUE_RESERVE_Pos( flwk->m_que, serverCmd );

    hem_state = flwk->m_HEManager.PushState();
    for(i=0; i<GFL_NELEMS(ItemEffectTable); ++i)
    {
      itemParam = calc::ITEM_GetParam( itemID, ItemEffectTable[i].effect );
      if( itemParam )
      {
        BTL_PRINT("アイテムID=%d, 効果=%dあり Idx=%d\n", itemID, itemParam, i);
        if( (this->*(ItemEffectTable[i].func))(flwk, target, itemID, itemParam, actParam) ){
          fEffective = true;
        }
      }
    }
    // 使用効果あり
    if( fEffective )
    {
      if( serverCmd == SC_ACT_EFFECT_SIMPLE )
      {
        scmd::SCQUE_PUT_ReservedPos( flwk->m_que, que_reserve_pos, serverCmd, effectNo );
      }
      else if( serverCmd == SC_ACT_EFFECT_BYPOS )
      {
        scmd::SCQUE_PUT_ReservedPos( flwk->m_que, que_reserve_pos, serverCmd, targetPos, effectNo );
      }
      else
      {
        GFL_ASSERT(0);
      }

      flwk->m_mainModule->DecrementPlayerItem( clientID, itemID );
      flwk->m_mainModule->CalcNatsukiItemUse( target, itemID );
    }
    else
    {
      SCQUE_PUT_MSG_STD( flwk->m_que, BTL_STRID_STD_UseItem_NoEffect );
    }
    flwk->m_HEManager.PopState( hem_state );
  }

  SCQUE_PUT_ACT_EffectSimple( flwk->m_que, BTLEFF_CAMERA_INIT ); // USE_ITEM SHOOTER_EFFECT のカメラ状態を初期化

  return TRITEM_RESULT_NORMAL;
}

//----------------------------------------------------------------------------------
/**
 * ボール使用ルート
 *
 * @param   flwk
 * @param   bpp
 * @param   itemID
 * @param[out]   bUsedBall    ボールを消費したら true
 *
 * @retval  捕獲成功したら true
 */
//----------------------------------------------------------------------------------
bool ServerFlowSub::scproc_TrainerItem_BallRoot( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, bool* bUsedBall, POKE_CAPTURED_CONTEXT* capContext )
{
  BtlPokePos  targetPos = BTL_POS_NULL;
  BTL_POKEPARAM* targetPoke = NULL;

  *bUsedBall = false;
  capContext->Clear();

  // 投げる位置（生きているポケモンをシークして最初のヒット位置）を決める
  {
    u8 posAry[ BTL_EXIST_POS_MAX ];
    u8 basePos, posCnt, i;
    BtlExPos exPos;

    basePos = flwk->m_mainModule->PokeIDtoPokePos( flwk->m_pokeCon, bpp->GetID() );
    exPos = EXPOS_MAKE( BTL_EXPOS_FULL_ENEMY, basePos );
    posCnt = flwk->m_mainModule->ExpandBtlPos( exPos, posAry );

    for(i=0; i<posCnt; ++i)
    {
      targetPoke = flwk->m_pokeCon->GetFrontPokeData( posAry[i] );
      if( !targetPoke->IsDead() ){
        targetPos = posAry[i];
        break;
      }
    }
  }

  // 野生戦 = 通常処理
  if( flwk->m_mainModule->GetCompetitor() == BTL_COMPETITOR_WILD )
  {
    u8 fSuccess = false;

    if( targetPos != BTL_POS_NULL )
    {
      u8 yure_cnt, fCritical, fZukanRegister;

      fSuccess = calcCapturePokemon( flwk, bpp, targetPoke, itemID, &yure_cnt, &fCritical );

      if( fSuccess )
      {
        flwk->m_flowResult = ServerFlow::RESULT_BTL_QUIT_CAPTURE;
        flwk->m_getPokePos = targetPos;
        fZukanRegister = !flwk->m_mainModule->IsZukanRegistered( targetPoke );
        targetPoke->SetCaptureBallID( itemID );
      }
      else{
        fZukanRegister = false;
      }

      SCQUE_PUT_ACT_BallThrow( flwk->m_que, targetPos, yure_cnt, fSuccess, fZukanRegister, fCritical, itemID );
      capContext->Set( targetPos, fSuccess, fZukanRegister );
    }

    *bUsedBall = true;
    return fSuccess;

  }
  // トレーナー戦では失敗（ボールは減らない）
  else
  {
    if( targetPos != BTL_POS_NULL ){
      SCQUE_PUT_ACT_BallThrowForbidden( flwk->m_que, targetPos, itemID );
    }
    return false;
  }
}
//------------------------------------------------------------------
//  捕獲抽選の回数、１回あたりの突破率などを調整するためのテスト関数
//------------------------------------------------------------------
#if PM_DEBUG
bool ServerFlowSub::calcCapturePokemonTest( ServerFlow* flwk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke,u16 ballID,
    u8* yure_cnt, u8* fCritical )
{
  static const u8 ratioPtn[] = { 255, 190, 70, 3 };
  static const u8 hpRatio[] =  { 100, 50, 1 };
  static const fx32 ballRatio[] = { FX32_CONST(1.0), FX32_CONST(2.0), FX32_CONST(4.0) };
  static const char* ballRatioStr[] = { "1.0", "2.0", "4.0" };
  static const u8 checkCnt[]    = { 3, 4 };
  static const char* checkCntStr[] = { "4", "5.33" };
  u32 cap_cnt;

  for(u32 i0 = 0; i0 < GFL_NELEMS(ratioPtn); ++i0 ){
    for(u32 i1 = 0; i1 < GFL_NELEMS(hpRatio); ++i1 ){
      for(u32 i2 = 0; i2 < GFL_NELEMS(ballRatio); ++i2 ){
        BTL_PRINT( "==== 捕獲値 %d, 残りHP率 %d %% のポケモンに 補正値 %s のボールを10000回投げる ====\n",
          ratioPtn[i0], hpRatio[i1], ballRatioStr[i2] );

        for(u32 i3 = 0; i3 < GFL_NELEMS(checkCnt); ++i3 )
        {
          BTL_PRINT( "( %d 回チェック／%s 乗根 のケース)\n", checkCnt[i3], checkCntStr[i3] );
          cap_cnt = 0;
          for(u32 i=0; i<10000; ++i)
          {
            cap_cnt += calcCapturePokemonTestSub( flwk, ratioPtn[i0], hpRatio[i1], ballRatio[i2], checkCnt[i3], i==0 );
          }
          BTL_PRINT("捕獲成功-> %4d / 10000 回\n", cap_cnt);
        }
        BTL_PRINT("\n\n");
      }

    }
  }
  return false;
}
bool ServerFlowSub::calcCapturePokemonTestSub( ServerFlow* flwk, u8 pokeRatio, u8 hpRatio, fx32 ballRatio, u8 checkCount, bool printFlg )
{
  u32 hp_base  = 300;
  u32 hp_value = ( hp_base - hpRatio * 2 );
  fx32 capture_value = FX32_CONST( hp_value );

  if( printFlg ){
      BTL_PRINT("  捕獲値：初期 %08x(%d) ..MaxHPx3=%d, HPVal=%d\n", capture_value, capture_value>>FX32_SHIFT, hp_base, hp_value );
  }

  // 種族ごとの捕獲値計算
  {
    capture_value *= pokeRatio;
    if( printFlg ){
      BTL_PRINT("  種族補正 %08x(%d) ..（ratio=%d)\n",
          capture_value, capture_value>>FX32_SHIFT, pokeRatio);
    }
  }


   // ボールによる捕獲率補正
   {
      capture_value = FX_Mul( capture_value, ballRatio );
      if( printFlg ){
        BTL_PRINT("  ボール補正 %08x(%d) .. (ballRatio=%08x)\n",
                capture_value, capture_value>>FX32_SHIFT, ballRatio);
      }
   }

  capture_value /= hp_base;
  capture_value = FX_Div( FX32_CONST(255), capture_value );

  if( checkCount == 3 )
  {
    capture_value = FX_Sqrt( capture_value );
    capture_value = FX_Sqrt( capture_value );
    if( printFlg ){
      BTL_PRINT("  捕獲値（4乗根)： %08x(%d)\n", capture_value, capture_value>>FX32_SHIFT );
    }
  }
  else
  {
    f32 v = 1.0f / 5.33f;
    capture_value = FX_POW( capture_value, FX32_CONST(v) );
    if( printFlg ){
      BTL_PRINT("  捕獲値（5.33乗根)： %08x(%d)\n", capture_value, capture_value>>FX32_SHIFT );
    }
  }

  capture_value = FX_Div( FX32_CONST(65536), capture_value );

  s32 check_value = FX_Whole( capture_value );
  if( printFlg ){
    double r = (double)(check_value) / 65535.0f;
    r *= 100;
    if( r >= 100.0f ){
      r = 100.0f;
    }
    BTL_PRINT("  捕獲計算値： %d (抽選１回あたり %6.3f%% x %d）\n", check_value, r, checkCount );
  }

  for(u32 i=0; i<checkCount; ++i)
  {
    s32 r = static_cast<s32>( calc::GetRand(65536) );
    if( r >= check_value ){
      return false;
    }
  }
  return true;
}
#endif

//----------------------------------------------------------------------------------
/**
 * ボール投げ効果計算
 *
 * @param   flwk
 * @param   myPoke
 * @param   targetPoke
 * @param   ballID
 * @param   yure_cnt    [out] 揺れ演出の発生回数
 * @param   fCritical   [out] クリティカルフラグ
 *
 * @retval  bool    捕獲できる場合はtrue
 */
//----------------------------------------------------------------------------------
bool ServerFlowSub::calcCapturePokemon( ServerFlow* flwk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke,u16 ballID,
    u8* yure_cnt, u8* fCritical )
{
  *fCritical = false;

#if PM_DEBUG
  if( flwk->m_mainModule->GetDebugFlag( BTL_DEBUGFLAG_MUST_CRITICAL_CAPTURE ) )
  {
    *fCritical = true;
    *yure_cnt = 1;
    return true;
  }
#endif

  if( (ballID == ITEM_MASUTAABOORU)
  ||  (flwk->m_mainModule->IsMustCaptureMode())
  ){
    *yure_cnt = BTL_CAPTURE_BALL_YURE_CNT_MAX;
    return true;
  }
  else
  {
    u32 hp_base = targetPoke->GetValue( BTL_POKEPARAM::BPP_MAX_HP ) * 3;
    u32 hp_value = ( hp_base - targetPoke->GetValue(BTL_POKEPARAM::BPP_HP) * 2 );
    fx32 capture_value = FX32_CONST( hp_value );
    BTL_PRINT("捕獲値：初期 %08x(%d) ..MaxHPx3=%d, HPVal=%d\n", capture_value, capture_value>>FX32_SHIFT, hp_base, hp_value );

    // 草むらＢ補正
    if( flwk->m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_HIGH_LV_ENC) )
    {
      fx32 kusa_ratio = getKusamuraCaptureRatio( flwk );
      capture_value = FX_Mul( capture_value, kusa_ratio );
    }

    // 種族ごとの捕獲値計算
    {
      u16 mons_no = targetPoke->GetMonsNo();
      u16 form_no = targetPoke->GetFormNo();
      s32 ratio = calc::PERSONAL_GetParam( mons_no, form_no, pml::personal::PARAM_ID_get_rate );

      // ヘビーボールなら捕獲値を補正
      if( ballID == ITEM_HEBIIBOORU )
      {
        const u16 weight = targetPoke->GetWeight();
        ratio += Capture::GetHeavyBallCaptureCorrectValue( weight );
        if( ratio <= 0 )
        {
          ratio = 1;
        }
      }

      capture_value *= ratio;
      BTL_PRINT("捕獲値：種族補正 %08x(%d) ..（monsno=%d, ratio=%d)\n", capture_value, capture_value>>FX32_SHIFT, mons_no, ratio);
    }

    // ボールによる捕獲率補正
    {
      fx32 ball_ratio = calcBallCaptureRatio( flwk, myPoke, targetPoke, ballID );
      capture_value = FX_Mul( capture_value, ball_ratio );
      BTL_PRINT("捕獲値：ボール補正 %08x(%d) .. (ballID=%d, ratio=%08x)\n", capture_value, capture_value>>FX32_SHIFT, ballID, ball_ratio);
    }


    capture_value /= hp_base;
    BTL_PRINT("捕獲値： %08x(%d)\n", capture_value, capture_value>>FX32_SHIFT );


    // 状態異常による補正
    switch( targetPoke->GetPokeSick() ){
    case pml::pokepara::SICK_NEMURI:
    case pml::pokepara::SICK_KOORI:
      capture_value = FX_Mul( capture_value, FX32_CONST(2.5) );
      break;
    case pml::pokepara::SICK_DOKU:
    case pml::pokepara::SICK_MAHI:
    case pml::pokepara::SICK_YAKEDO:
      capture_value = FX_Mul( capture_value, FX32_CONST(1.5) );
      break;
    }

    // ロトムパワー補正
    capture_value = flwk->RotomPowerCalcCapture(capture_value);

    // クリティカルチェック
    *fCritical = checkCaptureCritical( flwk, capture_value );

    if( capture_value >= FX32_CONST(255) ){
      *yure_cnt = (*fCritical)? 1 : BTL_CAPTURE_BALL_YURE_CNT_MAX;
      return true;
    }
    else
    {
      BTL_PRINT("捕獲値（A)： %08x(%d)\n", capture_value, capture_value>>FX32_SHIFT );

      capture_value = FX_Div( FX32_CONST(255), capture_value );

      BTL_PRINT("捕獲値（B-1)： %08x(%d)\n", capture_value, capture_value>>FX32_SHIFT );

    #if 0
      capture_value = FX_Sqrt( capture_value );
      capture_value = FX_Sqrt( capture_value );
    #else
      {
        f32 v = 1.0f / 5.33f;
        capture_value = FX_POW( capture_value, FX32_CONST(v) );
        BTL_PRINT("捕獲値（5.33乗根)： %08x(%d)\n", capture_value, capture_value>>FX32_SHIFT );
      }
    #endif

      BTL_PRINT("捕獲値（B-2)： %08x(%d)\n", capture_value, capture_value>>FX32_SHIFT );

      capture_value = FX_Div( FX32_CONST(65536), capture_value );
      BTL_PRINT("捕獲値（C)： %08x(%d)\n", capture_value, capture_value>>FX32_SHIFT );

      s32 check_value = FX_Whole( capture_value );

      BTL_PRINT("捕獲計算値： %d\n", check_value );


      *yure_cnt = 0;
      u32 check_count = (*fCritical)? 1 : (BTL_CAPTURE_BALL_YURE_CNT_MAX + 1);
      for(u32 i=0; i<check_count; ++i)
      {
        s32 r = calc::GetRand(65536);
        BTL_PRINT("捕獲乱数(%d) = %d\n", i, r);
        if( r < check_value )
        {
          if( (*yure_cnt) < BTL_CAPTURE_BALL_YURE_CNT_MAX ){
            ++(*yure_cnt);
          }
        }else{
          return false;
        }
      }
      return true;
    }
  }
}
/**
 *  草むら補正率を取得
 */
fx32 ServerFlowSub::getKusamuraCaptureRatio( ServerFlow* flwk )
{
  u32 capturedPokeCnt = flwk->m_mainModule->GetZukanCapturedCount();

  if( capturedPokeCnt > 600 ){
    return FX32_CONST(1);
  }
  if( capturedPokeCnt > 450 ){
    return FX32_CONST(0.9f);
  }
  if( capturedPokeCnt > 300 ){
    return FX32_CONST(0.8f);
  }
  if( capturedPokeCnt > 150 ){
    return FX32_CONST(0.7f);
  }
  if( capturedPokeCnt > 30 ){
    return FX32_CONST(0.5f);
  }

  return FX32_CONST(0.3f);
}

/**
 *  ボールによる捕獲率計算
 */
fx32 ServerFlowSub::calcBallCaptureRatio( ServerFlow* flwk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke, u16 ballID )
{
  fx32 ball_ratio = getBallCaptureRatio( flwk, myPoke, targetPoke, ballID );

  // ウルトラビーストよる捕獲率補正
  // 投げるボールがウルトラボール以外で、捕獲対象がウルトラビーストなら、捕獲率を補正
  if( ( ballID != ITEM_URUTORABOORU ) &&
      ( UltraBeast::IsUltraBeast( *targetPoke ) ) )
  {
    ball_ratio = FX32_CONST(0.1);
  }

  return ball_ratio;
}

/**
 *  ボールによる捕獲率取得
 */
fx32 ServerFlowSub::getBallCaptureRatio( ServerFlow* flwk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke, u16 ballID )
{
  GFL_UNUSED( myPoke );

  switch( ballID ){
  case ITEM_SUUPAABOORU:  return FX32_CONST(1.5f);
  case ITEM_HAIPAABOORU:  return FX32_CONST(2.0f);

  case ITEM_NETTOBOORU: // ネットボール
    if( targetPoke->IsMatchType(POKETYPE_MIZU) || targetPoke->IsMatchType(POKETYPE_MUSHI) ){
      return FX32_CONST(3.5);
    }
    break;

  case ITEM_DAIBUBOORU: // ダイブボール
    {
      const BTL_FIELD_SITUATION* fieldSituation = flwk->m_mainModule->GetFieldSituation();
      if( ( fieldSituation->bgComponent.enableDiveBall                        ) ||
          ( flwk->m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_FISHING ) ) ||
          ( flwk->m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_SWIM )    ) )
      {
        return FX32_CONST(3.5f);
      }
    }
    break;

  case ITEM_NESUTOBOORU: // ネストボール
    {
      u16 targetLevel = targetPoke->GetValue( BTL_POKEPARAM::BPP_LEVEL );

      if( targetLevel < 30 )
      {
        u16 levelDiff = 41 - targetLevel;
        if( levelDiff > 40 ){
          levelDiff = 40;
        }
        return FX32_CONST(levelDiff) / 10;
      }
    }
    break;

  case ITEM_RIPIITOBOORU: // リピートボール
    if( flwk->m_mainModule->IsZukanRegistered(targetPoke) ){
      return FX32_CONST(3.5);
    }
    break;

  case ITEM_TAIMAABOORU:  // タイマーボール
    {
      fx32 ratio = FX32_CONST(1) + FX32_CONST(0.3f) * flwk->m_turnCount;
      if( ratio > FX32_CONST(4) ){
        ratio = FX32_CONST(4);
      }
      return ratio;
    }

  case ITEM_DAAKUBOORU:  // ダークボール
    {
      const BTL_FIELD_SITUATION*    fieldSituation = flwk->m_mainModule->GetFieldSituation();
      const GameSys::TimeZone::Zone timeZone       = GameSys::TimeZone::GetNowTimeZone();
      if( ( fieldSituation->bgComponent.enableDarkBall ) ||
          ( timeZone == GameSys::TimeZone::EVENING     ) ||
          ( timeZone == GameSys::TimeZone::NIGHT       ) ||
          ( timeZone == GameSys::TimeZone::MIDNIGHT    ) )
      {
        return FX32_CONST(3);
      }
    }
    break;

  case ITEM_KUIKKUBOORU:  // クイックボール
    if( flwk->m_turnCount == 0 )
    {
      return FX32_CONST(5);
    }
    break;

  case ITEM_SUPIIDOBOORU:  // スピードボール
    if( 100 <= targetPoke->GetValue( BTL_POKEPARAM::BPP_MONS_AGILITY ) )
    {
      return FX32_CONST(4);
    }
    break;

  case ITEM_MUUNBOORU:  // ムーンボール
    {
      MonsNo monsno = static_cast<MonsNo>( targetPoke->GetMonsNo() );
      FormNo formno = static_cast<FormNo>( targetPoke->GetFormNo() );
      if( Capture::CheckMoonBallEffective( monsno, formno ) )
      {
        return FX32_CONST(4);
      }
    }
    break;

  case ITEM_RABURABUBOORU:  // ラブラブボール
    {
      MonsNo my_monsno     = static_cast<MonsNo>( myPoke->GetMonsNo() );
      Sex    my_sex        = static_cast<Sex>( myPoke->GetValue( BTL_POKEPARAM::BPP_SEX ) );
      MonsNo target_monsno = static_cast<MonsNo>( targetPoke->GetMonsNo() );
      Sex    target_sex    = static_cast<Sex>( targetPoke->GetValue( BTL_POKEPARAM::BPP_SEX ) );
      if( ( my_monsno == target_monsno ) &&
          ( my_sex != target_sex ) )
      {
        return FX32_CONST(8);
      }
    }
    break;

  case ITEM_RUAABOORU:  // ルアーボール
    if( flwk->m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_FISHING ) )
    {
      return FX32_CONST(5);
    }
    break;

  case ITEM_REBERUBOORU:  // レベルボール
    {
      u8 my_level = myPoke->GetValue( BTL_POKEPARAM::BPP_LEVEL );
      u8 target_level = targetPoke->GetValue( BTL_POKEPARAM::BPP_LEVEL );
      return Capture::GetLevelBallCaptureRatio( my_level, target_level );
    }



  case ITEM_URUTORABOORU:  // ウルトラボール
    return ( UltraBeast::IsUltraBeast( *targetPoke ) ) ? ( FX32_CONST(5) ) : ( FX32_CONST(0.1) );

  }
  return FX32_CONST(1);
}

//----------------------------------------------------------------------------------
/**
 * 捕獲クリティカル発生チェック
 *
 * @param   flwk
 * @param   capture_value   計算後の捕獲変数（固定小数 0〜255）
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool ServerFlowSub::checkCaptureCritical( ServerFlow* flwk, fx32 capture_value )
{
  u32 capturedPokeCnt = flwk->m_mainModule->GetZukanCapturedCount();

  fx32 ratio = 0;

  if( capturedPokeCnt > 600 ){
    ratio = FX32_CONST(2.5);
  }else if(  capturedPokeCnt > 450 ){
    ratio = FX32_CONST(2.0f);
  }else if(  capturedPokeCnt > 300 ){
    ratio = FX32_CONST(1.5f);
  }else if(  capturedPokeCnt > 150 ){
    ratio = FX32_CONST(1.0f);
  }else if(  capturedPokeCnt > 30 ){
    ratio = FX32_CONST(0.5f);
  }else{
    return false;
  }

  if( capture_value > FX32_CONST(255) ){
    capture_value = FX32_CONST(255);
  }
  capture_value = FX_Mul( capture_value, ratio ) / 6;
  if( static_cast<s32>(calc::GetRand(256)) < FX_Whole(capture_value) )
  {
    return true;
  }
  return false;
}

/**
 *  TrainerItemProc で野生ポケモンを捕獲した後、経験値処理をした後のコマンドを生成する
 */
void  ServerFlowSub::CreateCapturedCmd( SCQUE* que, const POKE_CAPTURED_CONTEXT* context )
{
  if( context->bCaptured )
  {
    SCQUE_PUT_ACT_BallThrow_Captured( que, context->targetPos, context->bZukanRegister );
  }
}


// アイテム効果：ねむり回復
u8 ServerFlowSub::ItemEff_SleepRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Cure( flwk, bpp, itemID, itemParam, pml::wazadata::WAZASICK_NEMURI );
}
u8 ServerFlowSub::ItemEff_PoisonRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Cure( flwk, bpp, itemID, itemParam, pml::wazadata::WAZASICK_DOKU );
}
u8 ServerFlowSub::ItemEff_YakedoRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Cure( flwk, bpp, itemID, itemParam, pml::wazadata::WAZASICK_YAKEDO );
}
u8 ServerFlowSub::ItemEff_KooriRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Cure( flwk, bpp, itemID, itemParam, pml::wazadata::WAZASICK_KOORI );
}
u8 ServerFlowSub::ItemEff_MahiRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Cure( flwk, bpp, itemID, itemParam, pml::wazadata::WAZASICK_MAHI );
}
u8 ServerFlowSub::ItemEff_KonranRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Cure( flwk, bpp, itemID, itemParam, pml::wazadata::WAZASICK_KONRAN );
}
u8 ServerFlowSub::ItemEff_MeromeroRcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  GFL_ASSERT_MSG( itemID==ITEM_AKAIBIIDORO , "Not RedVidro!!!\n");
  return ItemEff_Common_Cure( flwk, bpp, itemID, itemParam, pml::wazadata::WAZASICK_MEROMERO );
  //@バトル変更箇所 田谷さん確認済み
  //下記の関数はメンタルハーブ専用となっているが、実際は通っていない。(hand_itemのcommon_useForSick)
  //下の関数だと赤いビードロでうまく動かなかったので、汎用の関数を呼ぶようにする。
  //念のためビードロ以外のアイテムで通った時アサートが出るようにしかけておく。
  //return ItemEff_Mental_Cure( flwk, bpp, itemID, itemParam, pml::wazadata::WAZASICK_MEROMERO );
}
u8 ServerFlowSub::ItemEff_EffectGuard( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  GFL_UNUSED( itemID );
  GFL_UNUSED( itemParam );

  BtlSide side = flwk->m_mainModule->PokeIDtoSide( bpp->GetID() );
  BTL_SICKCONT cont = SICKCONT_MakeTurn( BTL_POKEID_NULL, 5 );
  if( flwk->scPut_SideEffect_Add( side, BTL_SIDEEFF_SIROIKIRI, cont) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>
                                        ( flwk->HANDEX_Push( BTL_HANDEX_MESSAGE, BTL_POKEID_NULL ) );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_SiroiKiri );
      HANDEX_STR_AddArg( &param->str, side );
    flwk->HANDEX_Pop( param );
    return true;
  }
  return false;
}
u8 ServerFlowSub::ItemEff_Relive( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  if( bpp->IsDead())
  {
    u8 pokeID = bpp->GetID();
    BTL_HANDEX_PARAM_RELIVE* param = reinterpret_cast<BTL_HANDEX_PARAM_RELIVE*>
                                                ( flwk->HANDEX_Push( BTL_HANDEX_RELIVE, pokeID ) );
      param->pokeID = pokeID;
      itemParam = calc::ITEM_GetParam( itemID, item::ITEM_DATA::PRM_ID_HP_RCV_POINT );
      switch( itemParam ){
      case item::ITEM_DATA::ITEM_RECOVER_HP_FULL:
        param->recoverHP = bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP ); break;
      case item::ITEM_DATA::ITEM_RECOVER_HP_HALF:
        param->recoverHP = calc::QuotMaxHP( bpp, 2 ); break;
      case item::ITEM_DATA::ITEM_RECOVER_HP_QUOT:
        param->recoverHP = calc::QuotMaxHP( bpp, 4 ); break;
      default:
        param->recoverHP = static_cast<u16>(calc::ITEM_GetParam( itemID, item::ITEM_DATA::PRM_ID_HP_RCV_POINT )); break;
      }

      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Relive );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flwk->HANDEX_Pop( param );

    return true;
  }
  return false;
}
u8 ServerFlowSub::ItemEff_AttackRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Rank( flwk, bpp, itemID, itemParam, BTL_POKEPARAM::BPP_ATTACK_RANK );
}
u8 ServerFlowSub::ItemEff_DefenceRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Rank( flwk, bpp, itemID, itemParam, BTL_POKEPARAM::BPP_DEFENCE_RANK );
}
u8 ServerFlowSub::ItemEff_SPAttackRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Rank( flwk, bpp, itemID, itemParam, BTL_POKEPARAM::BPP_SP_ATTACK_RANK );
}
u8 ServerFlowSub::ItemEff_SPDefenceRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Rank( flwk, bpp, itemID, itemParam, BTL_POKEPARAM::BPP_SP_DEFENCE_RANK );
}
u8 ServerFlowSub::ItemEff_AgilityRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Rank( flwk, bpp, itemID, itemParam, BTL_POKEPARAM::BPP_AGILITY_RANK );
}
u8 ServerFlowSub::ItemEff_HitRank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  return ItemEff_Common_Rank( flwk, bpp, itemID, itemParam, BTL_POKEPARAM::BPP_HIT_RATIO );
}
u8 ServerFlowSub::ItemEff_CriticalUp( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  GFL_UNUSED( itemID );

  u8 result = false;
  if( !bpp->CONTFLAG_Get(BTL_POKEPARAM::CONTFLG_KIAIDAME) )
  {
    bpp->CONTFLAG_Set( BTL_POKEPARAM::CONTFLG_KIAIDAME );
    SCQUE_PUT_OP_SetContFlag( flwk->m_que, bpp->GetID(), BTL_POKEPARAM::CONTFLG_KIAIDAME );

    result = true;
  }
  if( itemParam > 1 ){
    --itemParam;
    if( bpp->AddCriticalRank(itemParam) ){
      SCQUE_PUT_OP_AddCritical( flwk->m_que, bpp->GetID(), itemParam );
      result = true;
    }
  }
  if( result )
  {
    SCQUE_PUT_MSG_SET( flwk->m_que, BTL_STRID_SET_KiaiDame, bpp->GetID() );
    return true;
  }
  return false;
}
u8 ServerFlowSub::ItemEff_PP_Rcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( itemParam );

  if( bpp->WAZA_GetCount_Org() > actParam )
  {
    u8 pokeID = bpp->GetID( );
    u8 volume = bpp->WAZA_GetPPShort_Org( actParam );
    u32 ppValue = calc::ITEM_GetParam( itemID, item::ITEM_DATA::PRM_ID_PP_RCV_POINT );
    if( ppValue != item::ITEM_DATA::ITEM_RECOVER_PP_FULL ){
      if( volume > ppValue ){
        volume = ppValue;
      }
    }
    if( volume )
    {
      BTL_HANDEX_PARAM_PP* param = reinterpret_cast<BTL_HANDEX_PARAM_PP*>
                                    (flwk->HANDEX_Push( BTL_HANDEX_RECOVER_PP, pokeID ));
        param->volume = volume;
        param->pokeID = pokeID;
        param->wazaIdx = actParam;
        param->fDeadPokeEnable = true;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_PP_Recover );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, bpp->WAZA_GetID_Org(actParam) );
      flwk->HANDEX_Pop( param );
      return 1;
    }
  }
  return 0;
}
u8 ServerFlowSub::ItemEff_AllPP_Rcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );
  GFL_UNUSED( itemParam );

  u8 ppValue = static_cast<u8>(calc::ITEM_GetParam( itemID, item::ITEM_DATA::PRM_ID_PP_RCV_POINT ));

  u8 bEffective = makeCmd_RecoverAllPP( flwk, bpp, ppValue, true );

  return bEffective;
}
//------------------------------------------------------------
/**
 *  全ワザPP回復コマンド生成（道具、アシストパワー共用）
 */
//------------------------------------------------------------
bool ServerFlowSub::makeCmd_RecoverAllPP( ServerFlow* flwk, BTL_POKEPARAM* bpp, u8 recoverLimit, bool bStdMsg )
{
  u8 pokeID = bpp->GetID();
  bool bEffective = false;

  u32 cnt = bpp->WAZA_GetCount_Org();

  for(u32 i=0; i<cnt; ++i)
  {
    u32 volume = bpp->WAZA_GetPPShort_Org( i );
    if( (recoverLimit != 0) && (volume > recoverLimit) ){
      volume = recoverLimit;
    }

    if( volume )
    {
      BTL_HANDEX_PARAM_PP* param = reinterpret_cast<BTL_HANDEX_PARAM_PP*>
                                    ( flwk->HANDEX_Push( BTL_HANDEX_RECOVER_PP, pokeID ) );
        param->wazaIdx = i;
        param->pokeID = pokeID;
        param->volume = volume;
        param->fDeadPokeEnable = true;
      flwk->HANDEX_Pop( param );
      bEffective = true;
    }
  }

  if( bEffective )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>
                  ( flwk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ) );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_PP_AllRecover );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flwk->HANDEX_Pop( msg_param );
  }

  return bEffective;
}
u8 ServerFlowSub::ItemEff_HP_Rcv( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( actParam );

  if( !bpp->IsHPFull()
  &&  !bpp->IsDead()
  ){
    itemParam = calc::ITEM_GetParam( itemID, item::ITEM_DATA::PRM_ID_HP_RCV_POINT );
    u32 recoverHP;
    switch( itemParam ){
    case item::ITEM_DATA::ITEM_RECOVER_HP_FULL:
      recoverHP = bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP ); break;
    case item::ITEM_DATA::ITEM_RECOVER_HP_HALF:
      recoverHP = calc::QuotMaxHP( bpp, 2 ); break;
    case item::ITEM_DATA::ITEM_RECOVER_HP_QUOT:
      recoverHP = calc::QuotMaxHP( bpp, 4 ); break;
    default:
      recoverHP = itemParam; break;
    }

    makeCmd_RecoverHP( flwk, bpp, recoverHP, true );

    if( flwk->getFriendship(bpp) >= 150 )
    {
      u8 pokeID = bpp->GetID();

      BTL_HANDEX_PARAM_FRIENDSHIP_EFFECT* frParam = reinterpret_cast<BTL_HANDEX_PARAM_FRIENDSHIP_EFFECT*>
                                                          ( flwk->HANDEX_Push( BTL_HANDEX_FRIENDSHIP_EFFECT, pokeID ) );
      frParam->pokeID  = pokeID;
      frParam->effType = FREFF_FURIMUKI;
      HANDEX_STR_Setup( &frParam->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_FR_ItemRecover );
      HANDEX_STR_AddArg( &frParam->exStr, pokeID );
      HANDEX_STR_AddArg( &frParam->exStr, MainModule::PokeIDtoClientID(pokeID) );
      flwk->HANDEX_Pop( frParam );
    }

    return 1;
  }
  return 0;
}
//------------------------------------------------------------
/**
 *  HP回復コマンド生成（道具、アシストパワー共用）
 */
//------------------------------------------------------------
void ServerFlowSub::makeCmd_RecoverHP( ServerFlow* flwk, BTL_POKEPARAM* bpp, u32 recoverHP, bool bStdMsg )
{
  u8 pokeID = bpp->GetID( );
  BTL_HANDEX_PARAM_RECOVER_HP* param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP*>
                          ( flwk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ) );
    param->pokeID = pokeID;
    param->fFailCheckThru = true;
    param->recoverHP = recoverHP;
    if( bStdMsg ){
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    }
  flwk->HANDEX_Pop( param );
}

/**
 *  状態異常治し系共通
 */
u8 ServerFlowSub::ItemEff_Common_Cure( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID )
{
  GFL_UNUSED( itemParam );
  GFL_UNUSED( itemID );

  if( !bpp->IsDead() )
  {
    if( bpp->CheckSick(sickID) )
    {
      makeCmd_CureSick( flwk, bpp, sickID, true );
      return true;
    }
  }
  return false;
}
//------------------------------------------------------------
/**
 *  状態治しコマンド生成（道具、アシストパワー共用）
 */
//------------------------------------------------------------
void ServerFlowSub::makeCmd_CureSick( ServerFlow* flwk, BTL_POKEPARAM* bpp, WazaSick sickID, bool bStdMsg )
{
  BTL_HANDEX_PARAM_CURE_SICK* param;
  u8 pokeID = bpp->GetID();
  param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK*>
      ( flwk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ) );
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->sickCode = static_cast<BtlWazaSickEx>(sickID);
    if( !bStdMsg ){
      param->fStdMsgDisable = true;
    }
  flwk->HANDEX_Pop( param );
}
/**
 *  メンタルハーブ専用
 */
u8 ServerFlowSub::ItemEff_Mental_Cure( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID )
{
  GFL_UNUSED( itemID );
  GFL_UNUSED( itemParam );

  if( !bpp->IsDead() && (!flwk->m_posPoke.IsExist( bpp->GetID())) )
  {
    u8  result = 0;
    u32 idx = 0;
    for ever
    {
      sickID = tables::GetMentalSickID( idx++ );
      if( sickID == pml::wazadata::WAZASICK_NULL ){
        break;
      }
      if( bpp->CheckSick(sickID) )
      {
        u8 pokeID = bpp->GetID( );
        BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK*>
                                              (flwk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ) );
          param->poke_cnt = 1;
          param->pokeID[0] = pokeID;
          param->sickCode = static_cast<BtlWazaSickEx>(sickID);
        flwk->HANDEX_Pop( param );
        result = 1;
      }
    }
    return result;
  }
  return false;

}
/**
 *  ランクアップ系共通
 */
u8 ServerFlowSub::ItemEff_Common_Rank( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, BTL_POKEPARAM::ValueID rankType )
{
  GFL_UNUSED( itemID );

  u8 pokeID = bpp->GetID( );

  if( (flwk->m_posPoke.GetPokeExistPos(pokeID) != BTL_POS_NULL )
  &&  !bpp->IsDead()
  ){
    if( bpp->IsRankEffectValid(rankType, itemParam) )
    {
      makeCmd_RankEffect( flwk, bpp, (WazaRankEffect)rankType, itemParam );
      return 1;
    }
  }
  else
  {
    BTL_Printf("場に出ていないのでランク効果なし\n");
  }

  return 0;
}

//------------------------------------------------------------
/**
 *  ランク効果コマンド生成（道具、アシストパワー共用）
 */
//------------------------------------------------------------
void ServerFlowSub::makeCmd_RankEffect( ServerFlow* flwk, BTL_POKEPARAM* bpp, WazaRankEffect rankType, int volume )
{
  u8 pokeID = bpp->GetID();

  BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT*>
                                        ( flwk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ) );
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->rankType = rankType;
    param->rankVolume = volume;
    param->fAlmost = true;
  flwk->HANDEX_Pop( param );
}

/**
 *  シューター専用：アイテムコール
 */
u8 ServerFlowSub::ShooterEff_ItemCall( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( itemID );
  GFL_UNUSED( itemParam );
  GFL_UNUSED( actParam );

  u16 equipItemID = bpp->GetItem();
  if( !tables::CheckItemCallNoEffect(equipItemID) )
  {
    u8 fFailMsgDisped;
    u8 result;

    BTL_N_Printf( DBGSTR_SVFL_UseItemCall, bpp->GetID() );

    result = flwk->Hnd_UseItemEquip( bpp, &fFailMsgDisped );
    if( !result ){
      return (fFailMsgDisped)? SHOOTER_REACTION_FAIL_MSG : SHOOTER_REACTION_NONE;
    }
    return SHOOTER_REACTION_ENABLE;
  }
  return SHOOTER_REACTION_NONE;
}
/**
 *  シューター専用：スキルコール
 */
u8 ServerFlowSub::ShooterEff_SkillCall( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( itemID );
  GFL_UNUSED( itemParam );
  GFL_UNUSED( actParam );

  if( bpp->GetValue(BTL_POKEPARAM::BPP_TOKUSEI) != TOKUSEI_KAWARIMONO )
  {
    u32 hem_state = flwk->m_HEManager.PushState();
    HandExResult  result;

    flwk->Hnd_EventSkillCall( bpp );
    result = flwk->HandEx_Result();
    flwk->m_HEManager.PopState( hem_state );

    if( result == HandExResult_Enable ){
      return SHOOTER_REACTION_ENABLE;
    }
  }
  return SHOOTER_REACTION_NONE;
}
/**
 *  シューター専用：アイテムドロップ
 */
u8 ServerFlowSub::ShooterEff_ItemDrop( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( itemID );
  GFL_UNUSED( itemParam );
  GFL_UNUSED( actParam );

  if( !HandCommon_CheckCantChangeItemPoke(flwk, bpp->GetID()) )
  {
    u16 equipItemID = bpp->GetItem();
    if( equipItemID != ITEM_DUMMY_DATA )
    {
      u32 hem_state = flwk->m_HEManager.PushState();
      {
        BTL_HANDEX_PARAM_SET_ITEM* param;
        u8 pokeID = bpp->GetID( );

        param = reinterpret_cast<BTL_HANDEX_PARAM_SET_ITEM*>
                      ( flwk->HANDEX_Push( BTL_HANDEX_SET_ITEM, pokeID ) );
          param->pokeID = pokeID;
          param->itemID = ITEM_DUMMY_DATA;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Shooter_ItemDrop );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
          HANDEX_STR_AddArg( &param->exStr, equipItemID );
        flwk->HANDEX_Pop( param );
      }
      flwk->m_HEManager.PopState( hem_state );
      return SHOOTER_REACTION_ENABLE;
    }
  }
  return SHOOTER_REACTION_NONE;
}
/**
 *  シューター専用：フラットコール
 */
u8 ServerFlowSub::ShooterEff_FlatCall( ServerFlow* flwk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  GFL_UNUSED( itemID );
  GFL_UNUSED( itemParam );
  GFL_UNUSED( actParam );

  u32 hem_state = flwk->m_HEManager.PushState();
  {
    BTL_HANDEX_PARAM_RESET_RANK* reset_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    u8 pokeID = bpp->GetID();

    reset_param = reinterpret_cast<BTL_HANDEX_PARAM_RESET_RANK*>
                  ( flwk->HANDEX_Push( BTL_HANDEX_RESET_RANK, pokeID ) );
      reset_param->poke_cnt = 1;
      reset_param->pokeID[0] = pokeID;
    flwk->HANDEX_Pop( reset_param );

    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>
                  ( flwk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ) );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Shooter_FlatCall );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flwk->HANDEX_Pop( msg_param );
  }
  flwk->m_HEManager.PopState( hem_state );
  return SHOOTER_REACTION_ENABLE;
}

/*====================================================================================================*/
/*                                                                                                    */
/*  経験値計算処理                                                                                    */
/*                                                                                                    */
/*====================================================================================================*/

//----------------------------------------------------------------------------------
/**
 * 経験値計算結果を専用ワークに出力
 *
 * @param   flwk
 * @param   party     プレイヤー側パーティ
 * @param   deadPoke  しんだポケモン
 * @param   result    [out] 計算結果格納先
 *
 * @retval  bool      経験値を取得する場合 true
 */
//----------------------------------------------------------------------------------
bool ServerFlowSub::CalcExp( BtlRule rule, ServerFlow* flwk, BTL_PARTY* party, const BTL_POKEPARAM* deadPoke, CALC_EXP_WORK* result )
{
  // 経験値計算
  ExpCalculator::CalcExp( 
    result,
    *( flwk->m_mainModule->GetPlayerStatus() ),
    *( flwk->m_mainModule ),
    *party,
    *deadPoke );


  // 努力値計算
  u16 memberCount = party->GetMemberCount();
  for(uint32_t i=0; i<memberCount; ++i)
  {
    if( result[i].exp )
    {
      BTL_POKEPARAM* bpp = party->GetMemberData( i );
      calcDoryokuExp( rule, bpp, deadPoke, &result[i] );
    }
  }

  for(uint32_t i=0; i<BTL_PARTY_MEMBER_MAX; ++i)
  {
    if( result[i].exp ){
      return true;
    }
  }
  return false;
}

/**
 *  努力値計算、結果を result に書き込み
 */
void ServerFlowSub::calcDoryokuExp( BtlRule rule, BTL_POKEPARAM* bpp, const BTL_POKEPARAM* deadPoke, CALC_EXP_WORK* result )
{
  /**
   *  ステータスIndex
   */
  enum {
    _HP = 0,
    _POW,
    _DEF,
    _AGI,
    _SPOW,
    _SDEF,
    _PARAM_MAX,
  };

  /**
   *  関連パラメータ（上記ステータスIndex順）
   */
  const struct {
    pml::personal::ParamID       personalParamID;   ///< パーソナル贈与努力値ID
    pml::pokepara::PowerID       pokeParamID;       ///< pml::pokepara::PokemonParam 努力値ID
    u16                          boostItemID;       ///< 努力値加算アイテムID
  }RelationTbl[] = {

    { pml::personal::PARAM_ID_pains_hp,    pml::pokepara::POWER_HP,     ITEM_PAWAAUEITO   },  // HP
    { pml::personal::PARAM_ID_pains_atk,   pml::pokepara::POWER_ATK,    ITEM_PAWAARISUTO  },  // 攻撃
    { pml::personal::PARAM_ID_pains_def,   pml::pokepara::POWER_DEF,    ITEM_PAWAABERUTO  },  // 防御
    { pml::personal::PARAM_ID_pains_agi,   pml::pokepara::POWER_AGI,    ITEM_PAWAAANKURU  },  // 素早さ
    { pml::personal::PARAM_ID_pains_spatk, pml::pokepara::POWER_SPATK,  ITEM_PAWAARENZU   },  // 特攻
    { pml::personal::PARAM_ID_pains_spdef, pml::pokepara::POWER_SPDEF,  ITEM_PAWAABANDO   },  // 特防

  };

  u16 mons_no = deadPoke->GetMonsNo();
  u16 form_no = deadPoke->GetFormNo();

  u8  exp[ _PARAM_MAX ];

  // 基本努力値をパーソナルから取得
  for(u32 i=0; i<_PARAM_MAX; ++i){
    exp[ i ] = calc::PERSONAL_GetParam( mons_no, form_no, RelationTbl[i].personalParamID );
  }

  // きょうせいギプスで倍
  if( bpp->GetItem() == ITEM_KYOUSEIGIPUSU )
  {
    for(u32 i=0; i<_PARAM_MAX; ++i){
      exp[ i ] *= 2;
    }
  }

  // 各種、努力値増加アイテム分を加算
  for(u32 i=0; i<_PARAM_MAX; ++i)
  {
    if( bpp->GetItem() == RelationTbl[i].boostItemID ){
      exp[ i ] += static_cast<u8>(calc::ITEM_GetParam( RelationTbl[i].boostItemID, item::ITEM_DATA::PRM_ID_ATTACK ));
    }
  }


  // ポケルスで倍
  if( bpp->HavePokerus() )
  {
    for(u32 i=0; i<_PARAM_MAX; ++i){
      exp[ i ] *= 2;
    }
  }

  // 乱入バトルなら倍
  if( rule == BTL_RULE_INTRUDE )
  {
    for(u32 i=0; i<_PARAM_MAX; ++i){
      exp[ i ] *= 2;
    }
  }

  #if 0
  // Srcデータに反映
  {
    pml::pokepara::PokemonParam* pp = (pml::pokepara::PokemonParam*)bpp->GetSrcData();
    bool fFastMode = pp->StartFastMode();


      // 努力値合計を取得
      for(i=0; i<_PARAM_MAX; ++i)
      {
        if( exp[i] )
        {
          u32 sum = exp[i] + pp->GetEffortPower( RelationTbl[i].pokeParamID );
          if( sum > pml::MAX_EFFORT_POWER ){
            sum = pml::MAX_EFFORT_POWER;
          }
          pp->ChangeEffortPower( RelationTbl[i].pokeParamID, sum );
        }
      }

    pp->EndFastMode( fFastMode );
  }
  #else
  // result に反映
  for( u32 i=0; i<_PARAM_MAX; ++i )
  {
    switch( RelationTbl[i].pokeParamID ){
    case pml::pokepara::POWER_HP:     result->hp     = exp[i]; break;
    case pml::pokepara::POWER_ATK:    result->pow    = exp[i]; break;
    case pml::pokepara::POWER_DEF:    result->def    = exp[i]; break;
    case pml::pokepara::POWER_AGI:    result->agi    = exp[i]; break;
    case pml::pokepara::POWER_SPATK:  result->sp_pow = exp[i]; break;
    case pml::pokepara::POWER_SPDEF:  result->sp_def = exp[i]; break;
    }
  }
  #endif
}



GFL_NAMESPACE_END(btl)

#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif
