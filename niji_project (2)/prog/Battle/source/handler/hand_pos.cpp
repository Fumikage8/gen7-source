//=============================================================================================
/**
 * @file  hand_pos.c
 * @brief ポケモンXY バトルシステム イベントファクター [位置エフェクト]
 * @author  taya
 *
 * @date  2011.01.27  作成
 */
//=============================================================================================

#include "../btl_str.h"
#include "../btl_ServerFlow.h"
#include "./hand_common.h"
#include "./hand_pos.h"


GFL_NAMESPACE_BEGIN(btl)
namespace handler {
namespace pos     {

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  WORKIDX_USER_POKEID = EVENT_HANDLER_WORK_ELEMS-1,
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
bool is_registable( BtlPosEffect effect, BtlPokePos pokePos );
const BtlEventHandlerTable* ADD_POS_Negaigoto( u32* numElems );
void handler_pos_Negaigoto( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work );
const BtlEventHandlerTable* ADD_POS_MikadukiNoMai( u32* numElems );
void handler_pos_MikadukiNoMai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work );
const BtlEventHandlerTable* ADD_POS_IyasiNoNegai( u32* numElems );
void handler_pos_IyasiNoNegai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work );
const BtlEventHandlerTable* ADD_POS_ZenryokuEffect_RepairHP( u32* numElems );
void handler_pos_ZenryokuEffect_RepairHP( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work );
void common_removeEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work );
const BtlEventHandlerTable* ADD_POS_DelayAttack( u32* numElems );
void handler_pos_DelayAttack( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work );
const BtlEventHandlerTable* ADD_POS_BatonTouch( u32* numElems );
void handler_pos_BatonTouch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work );
void handler_SpotLight_TemptTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work );
void handler_SpotLight_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work );

//=============================================================================================
/**
 * 位置エフェクトハンドラをシステムに追加
 *
 * @param   effect
 * @param   pos
 * @param   pokeID
 * @param   param
 *
 * @retval  BTL_EVENT_FACTOR*   追加されたイベントハンドラ（重複して追加できない場合NULL）
 */
//=============================================================================================
BTL_EVENT_FACTOR*  Add( BtlPosEffect effect, BtlPokePos pos, u8 pokeID, const int* param, u8 param_cnt )
{
  typedef const BtlEventHandlerTable* (*pEventAddFunc)( u32* numHandlers );

  static const struct {
    BtlPosEffect   eff;
    pEventAddFunc  func;
  }funcTbl[] = {
    { BTL_POSEFF_NEGAIGOTO,                 ADD_POS_Negaigoto               },
    { BTL_POSEFF_MIKADUKINOMAI,             ADD_POS_MikadukiNoMai           },
    { BTL_POSEFF_IYASINONEGAI,              ADD_POS_IyasiNoNegai            },
    { BTL_POSEFF_DELAY_ATTACK,              ADD_POS_DelayAttack             },
    { BTL_POSEFF_BATONTOUCH,                ADD_POS_BatonTouch              },
    { BTL_POSEFF_ZENRYOKU_EFFECT_REPAIR_HP, ADD_POS_ZenryokuEffect_RepairHP },
  };

  GFL_ASSERT(effect < BTL_POSEFF_MAX);

  for(u32 i=0; i<GFL_NELEMS(funcTbl); ++i)
  {
    if( funcTbl[i].eff == effect )
    {
      if( is_registable(effect, pos) )
      {
        BTL_EVENT_FACTOR* factor;
        const BtlEventHandlerTable* handlerTable;
        u32 numHandlers;

        handlerTable = funcTbl[i].func( &numHandlers );
        factor = EVENT_AddFactor( BTL_EVENT_FACTOR_POS, effect,
                BTL_EVPRI_POS_DEFAULT, 0, pos, handlerTable, numHandlers );
        if( factor )
        {
          for(u32 j=0; j<param_cnt; ++j){
            EVENT_FACTOR_SetWorkValue( factor, j, param[j] );
            BTL_PRINT("[HandPos] ハンドラ work[%d] にパラメータ %d を設定\n", j, param[j]);
          }
          EVENT_FACTOR_SetWorkValue( factor, WORKIDX_USER_POKEID, pokeID );
        }
        return factor;
      }
      break;
    }
  }

  GFL_ASSERT(0);
  return NULL;
}

//=============================================================================================
/**
 * 指定位置に指定エフェクトが登録されているか判定
 *
 * @param   eff
 * @param   pos
 *
 * @retval  bool
 */
//=============================================================================================
bool IsRegistered( BtlPosEffect eff, BtlPokePos pos )
{
  return !is_registable( eff, pos );
}

//----------------------------------------------------------------------------------
/**
 * 登録できる条件を満たしているか判定  ※同じ位置に同じエフェクトは１つまで
 *
 * @param   effect
 * @param   pokePos
 *
 * @retval  bool    条件を満たしていたらtrue
 */
//----------------------------------------------------------------------------------
bool is_registable( BtlPosEffect effect, BtlPokePos pokePos )
{
  BTL_EVENT_FACTOR* factor;

  factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_POS, pokePos );
  while( factor )
  {
    if( EVENT_FACTOR_GetSubID(factor) == effect ){
      return false;
    }
    factor = EVENT_GetNextFactor( factor );
  }
  return true;
}

/**
 * 位置エフェクトが自滅する時の共通処理（サーバに自滅を通知する）
 * @param handle
 * @param flowWk
 * @param myPos
 */
void common_removePosEffect( BTL_EVENT_FACTOR* handle, ServerFlow* flowWk, BtlPokePos myPos )
{
  BtlPosEffect effectType = (BtlPosEffect)(EVENT_FACTOR_GetSubID(handle));
  flowWk->Hnd_NotifyRemovePosEffect( effectType, myPos );
  EVENT_FACTOR_Remove( handle );
}

//--------------------------------------------------------------------------------------
/**
 *  ねがいごと
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_POS_Negaigoto( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,  handler_pos_Negaigoto      },  // ターンチェック開始ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ターンチェック開始ハンドラ
void handler_pos_Negaigoto( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work )
{
  enum {
    WORKIDX_SKIP_SPFAIL_CHECK = 0,
    WORKIDX_FIRST_TURN_FLAG,
    WORKIDX_TURN,
  };

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == BTL_POKEID_NULL)
  &&  (!flowWk->Hnd_CheckShowDown())
  ){
    // 初ターン呼び出し時、これまでの経過ターン数を work[ WORKIDX_TURN ] に保存
    if( work[ WORKIDX_FIRST_TURN_FLAG ] == 0 ){
      work[ WORKIDX_TURN ] = flowWk->Hnd_GetTurnCount( );
      work[ WORKIDX_FIRST_TURN_FLAG ] = 1;
      return;
    }
    else
    {
      int turnCnt = flowWk->Hnd_GetTurnCount( );
      if( turnCnt > work[ WORKIDX_TURN ] )
      {
        u8 targetPokeID = flowWk->Hnd_PokePosToPokeID( pokePos );
        if( targetPokeID != BTL_POKEID_NULL )
        {
          const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
          if( !target->IsHPFull() )
          {
            u8 userPokeID = work[ WORKIDX_USER_POKEID ];
            const BTL_POKEPARAM* user = flowWk->Hnd_GetPokeParam( userPokeID );

            BTL_HANDEX_PARAM_RECOVER_HP* hp_param;
            hp_param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, userPokeID ));
              hp_param->pokeID = targetPokeID;
              hp_param->recoverHP = calc::QuotMaxHP( user, 2 );
              hp_param->fFailCheckThru = work[ WORKIDX_SKIP_SPFAIL_CHECK ];
              HANDEX_STR_Setup( &hp_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Negaigoto );
              HANDEX_STR_AddArg( &hp_param->exStr, userPokeID );
            flowWk->HANDEX_Pop( hp_param );
          }
        }

        common_removePosEffect( myHandle, flowWk, pokePos );
      }
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  みかづきのまい
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_POS_MikadukiNoMai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_pos_MikadukiNoMai   },  // ポケ入場ハンドラ
    { BTL_EVENT_AFTER_MOVE, handler_pos_MikadukiNoMai   },  // ムーブ直後ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_pos_MikadukiNoMai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work )
{
  GFL_UNUSED(work);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( flowWk->Hnd_PokeIDtoPokePos( pokeID) == pokePos )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    bool fEnable = false;
//    u32 que_reserved_pos;
//    BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_VANISH_MSGWIN, BTL_POKEID_NULL );
//    que_reserved_pos = flowWk->Hnd_ReserveQuePos( SC_ACT_EFFECT_BYPOS );

    // 体力全回復
    if( !bpp->IsHPFull() ){
      fEnable = true;
    }

    // ポケ系状態異常回復
    if( bpp->GetPokeSick() != pml::pokepara::SICK_NULL ){
      fEnable = true;
    }

    // 全ワザPP全回復
    {
      u32 wazaCnt, i;
      u8 volume;
      wazaCnt = bpp->WAZA_GetCount( );
      for(i=0; i<wazaCnt; ++i)
      {
        volume = bpp->WAZA_GetPPShort( i );
        if( volume ){
          fEnable = true;
        }
      }
    }

    if( fEnable )
    {
      BTL_HANDEX_PARAM_ADD_EFFECT* eff_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, pokeID ));
        eff_param->effectNo = BTLEFF_MIKADUKINOMAI_KAIHUKU;
        eff_param->pos_from = pokePos;
        eff_param->pos_to = BTL_POS_NULL;
//        eff_param->reservedQuePos = que_reserved_pos;
//        eff_param->fQueReserve = true;
        eff_param->fMsgWinVanish = true;

        HANDEX_STR_Setup( &eff_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_MikadukiNoMai );
        HANDEX_STR_AddArg( &eff_param->exStr, pokeID );
      flowWk->HANDEX_Pop( eff_param );


      // 体力全回復
      if( !bpp->IsHPFull() )
      {
        BTL_HANDEX_PARAM_RECOVER_HP* hp_param;
        hp_param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
          hp_param->pokeID = pokeID;
          hp_param->recoverHP = bpp->GetValue(BTL_POKEPARAM::BPP_MAX_HP) - bpp->GetValue(BTL_POKEPARAM::BPP_HP);
        flowWk->HANDEX_Pop( hp_param );
      }

      // ポケ系状態異常回復
      if( bpp->GetPokeSick() != pml::pokepara::SICK_NULL )
      {
        BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
          param->poke_cnt = 1;
          param->pokeID[0] = pokeID;
          param->sickCode = WAZASICK_EX_POKEFULL;
        flowWk->HANDEX_Pop( param );
      }

      // 全ワザPP全回復
      {
        u32 wazaCnt, i;
        u8 volume;
        wazaCnt = bpp->WAZA_GetCount( );
        for(i=0; i<wazaCnt; ++i)
        {
          volume = bpp->WAZA_GetPPShort( i );
          if( volume )
          {
            BTL_HANDEX_PARAM_PP* param = reinterpret_cast<BTL_HANDEX_PARAM_PP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_PP, pokeID ));
              param->pokeID = pokeID;
              param->volume = volume;
              param->wazaIdx = i;
            flowWk->HANDEX_Pop( param );
          }
        }
      }
    }
    common_removePosEffect( myHandle, flowWk, pokePos );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  いやしのねがい
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_POS_IyasiNoNegai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_pos_IyasiNoNegai   },  // ダメージ補正
    { BTL_EVENT_AFTER_MOVE, handler_pos_IyasiNoNegai   },  // ムーブ直後ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief HP全回復　＆　状態異常回復
 * @param flowWk
 * @param targetPokeId           対象ポケモンのID
 * @param isHpRepairEnable       HPを回復させるか？
 * @param isSickRepairEnable     状態異常を回復させるか？
 * @param effectNo               効果があった場合に表示するエフェクト
 * @param effectMessageStrType   効果があった場合に表示するメッセージの文字列タイプ( BTL_STRTYPE_xxxx )
 * @param effectMessageStrId     効果があった場合に表示するメッセージの文字列ID( BTL_STRID_xxxx )
 */
void repair_hp_and_sick( 
  ServerFlow* flowWk,
  u8 targetPokeId,
  bool isHpRepairEnable, 
  bool isSickRepairEnable, 
  u16 effectNo,
  BtlStrType effectMessageStrType, 
  u32 effectMessageId )
{
  const BTL_POKEPARAM* targetPoke = flowWk->Hnd_GetPokeParam( targetPokeId );

  bool doRepairHp = ( isHpRepairEnable && !( targetPoke->IsHPFull() ) ); // 体力を全回復するか？
  bool doRepairSick = ( isSickRepairEnable && ( targetPoke->GetPokeSick() != pml::pokepara::SICK_NULL ) ); // ポケ系状態異常を回復するか？
  if( !( doRepairHp || doRepairSick ) ) {
    return;
  }

  BTL_HANDEX_PARAM_ADD_EFFECT* eff_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, targetPokeId ));
    eff_param->effectNo = effectNo;
    eff_param->pos_from = flowWk->Hnd_PokeIDtoPokePos( targetPokeId );
    eff_param->pos_to = BTL_POS_NULL;
    HANDEX_STR_Setup( &eff_param->exStr, effectMessageStrType, effectMessageId );
    HANDEX_STR_AddArg( &eff_param->exStr, targetPokeId );
  flowWk->HANDEX_Pop( eff_param );

  // 体力全回復
  if( doRepairHp )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* hp_param;
      hp_param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, targetPokeId ) );
      hp_param->pokeID = targetPokeId;
      hp_param->recoverHP = targetPoke->GetValue( BTL_POKEPARAM::BPP_MAX_HP ) - targetPoke->GetValue( BTL_POKEPARAM::BPP_HP );
    flowWk->HANDEX_Pop( hp_param );
  }

  // ポケ系状態異常回復
  if( doRepairSick )
  {
    BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, targetPokeId ) );
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeId;
      param->sickCode = WAZASICK_EX_POKEFULL;
    flowWk->HANDEX_Pop( param );
  }
}

void handler_pos_IyasiNoNegai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work )
{
  GFL_UNUSED(work);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( flowWk->Hnd_PokeIDtoPokePos( pokeID) != pokePos ) {
    return;
  }

  static const bool REPAIR_HP = true;
  static const bool REPAIR_SICK = true;
  repair_hp_and_sick( flowWk, pokeID, REPAIR_HP, REPAIR_SICK, BTLEFF_IYASINONEGAI_KAIHUKU, BTL_STRTYPE_SET, BTL_STRID_SET_IyasiNoNegai );
  common_removePosEffect( myHandle, flowWk, pokePos );
}

//--------------------------------------------------------------------------------------
/**
 *  全力効果「次に場に出た味方を回復する」
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_POS_ZenryokuEffect_RepairHP( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_pos_ZenryokuEffect_RepairHP  },  // ダメージ補正
    { BTL_EVENT_AFTER_MOVE, handler_pos_ZenryokuEffect_RepairHP  },  // ムーブ直後ハンドラ
    { BTL_EVENT_TURN_END,   common_removeEffect },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_pos_ZenryokuEffect_RepairHP( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work )
{
  GFL_UNUSED(work);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( flowWk->Hnd_PokeIDtoPokePos( pokeID) != pokePos ) {
    return;
  }

  static const bool REPAIR_HP = true;
  static const bool REPAIR_SICK = false;
  repair_hp_and_sick( flowWk, pokeID, REPAIR_HP, REPAIR_SICK, BTLEFF_IYASINONEGAI_KAIHUKU, BTL_STRTYPE_SET, BTL_STRID_SET_AuraEffect_FriendHPRecover_Receive );
  common_removePosEffect( myHandle, flowWk, pokePos );
}
void common_removeEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work )
{
  common_removePosEffect( myHandle, flowWk, pokePos );
}


//--------------------------------------------------------------------------------------
/**
 *  時間差ワザ攻撃（みらいよち、はめつのねがい等）
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_POS_DelayAttack( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,  handler_pos_DelayAttack   },  // ターンチェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ターンチェックハンドラ
void handler_pos_DelayAttack( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work )
{
  enum {
    WORKIDX_TURN = 0,
    WORKIDX_WAZAID,
    WORKIDX_ATK_POS,
  };

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == BTL_POKEID_NULL)
  &&  (!flowWk->Hnd_CheckShowDown())
  ){
    u16 turnCnt = flowWk->Hnd_GetTurnCount();
    if( turnCnt >= work[WORKIDX_TURN] )
    {
      u8 targetPokeID = flowWk->Hnd_PokePosToPokeID( pokePos );
      u8 attackerPokeID = work[WORKIDX_USER_POKEID];
      BTL_PRINT(" turnCount($d), 指定ターン数[%d]越え / targetPokeID=%d, atkPokeID=%d\n",
         turnCnt, work[WORKIDX_TURN], targetPokeID, attackerPokeID);
      if( (targetPokeID != BTL_POKEID_NULL)
      &&  (targetPokeID != attackerPokeID)
      ){
        const BTL_POKEPARAM* bppTarget = flowWk->Hnd_GetPokeParam( targetPokeID );
        if( !bppTarget->IsDead() )
        {
          BTL_HANDEX_PARAM_DELAY_WAZADMG* param;
          BTL_HANDEX_PARAM_MESSAGE* msg_param;

          msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, attackerPokeID ));
            HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_DelayAttack );
            HANDEX_STR_AddArg( &msg_param->str, targetPokeID );
            HANDEX_STR_AddArg( &msg_param->str, work[ WORKIDX_WAZAID ] );
          flowWk->HANDEX_Pop( msg_param );


          param = reinterpret_cast<BTL_HANDEX_PARAM_DELAY_WAZADMG *>( flowWk->HANDEX_Push( BTL_HANDEX_DELAY_WAZADMG, attackerPokeID ));
            param->attackerPokeID = attackerPokeID;
            param->targetPokeID = targetPokeID;
            param->attackerPos = (BtlPokePos)(work[ WORKIDX_ATK_POS ]);
            param->wazaID = static_cast<WazaNo>(work[ WORKIDX_WAZAID ]);
          flowWk->HANDEX_Pop( param );
        }
      }
      common_removePosEffect( myHandle, flowWk, pokePos );
    }
    else
    {
      BtlPokePos             pos         = static_cast<BtlPokePos>( pokePos );
      BtlPosEffect           posEffect   = static_cast<BtlPosEffect>( EVENT_FACTOR_GetSubID( myHandle ) );
      PosEffect::EffectParam effectParam = flowWk->Hnd_GetPosEffectParam( posEffect, pos );
      effectParam.DelayAttack.execTurnCount += 1;
      flowWk->Hnd_UpdatePosEffectParam( posEffect, pos, effectParam );
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  バトンタッチ
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_POS_BatonTouch( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN_BATONTOUCH,  handler_pos_BatonTouch   },  // ダメージ補正
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_pos_BatonTouch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokePos, int* work )
{
  u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( targetPokeID == flowWk->Hnd_PokePosToPokeID(pokePos) )
  {
    BTL_HANDEX_PARAM_BATONTOUCH* param = reinterpret_cast<BTL_HANDEX_PARAM_BATONTOUCH *>( flowWk->HANDEX_Push( BTL_HANDEX_BATONTOUCH, BTL_POKEID_NULL ));
      param->userPokeID = work[ WORKIDX_USER_POKEID ];
      param->targetPokeID = targetPokeID;
    flowWk->HANDEX_Pop( param );

    common_removePosEffect( myHandle, flowWk, pokePos );
  }
}




//----------------------------------------------------------------------------------
} // end of namespace 'pos'
} // end of namespace 'handler'
GFL_NAMESPACE_END(btl)

