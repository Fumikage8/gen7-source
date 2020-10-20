//=============================================================================================
/**
 * @file    hand_side.cpp
 * @brief   ポケモンXY バトルシステム イベントファクター [サイドエフェクト]
 * @author  taya
 *
 * @date  2011.01.12  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#include "../btl_calc.h"
#include "../btl_ServerFlow.h"
#include "../btl_SideEffectStatus.h"
#include "hand_common.h"
#include "hand_side.h"

GFL_NAMESPACE_BEGIN(btl)
namespace handler {
namespace side    {

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
u8 getMyAddCounter( const BTL_EVENT_FACTOR* myHandle, const ServerFlow* serverFlow, BtlSide side );
const BtlEventHandlerTable* ADD_SIDE_Refrector( u32* numElems );
void handler_side_Refrector( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_Hikarinokabe( u32* numElems );
void handler_side_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
void common_wallEffect( ServerFlow* flowWk, u8 mySide, WazaDamageType dmgType );
fx32 common_wallEffect_GetDamageRatio( BtlRule rule );
const BtlEventHandlerTable* ADD_SIDE_Sinpinomamori( u32* numElems );
void handler_side_SinpiNoMamori_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
void handler_side_SinpiNoMamori_FixFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_SiroiKiri( u32* numElems );
void handler_side_SiroiKiri_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
void handler_side_SiroiKiri_FixFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_Oikaze( u32* numElems );
void handler_side_Oikaze( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_Omajinai( u32* numElems );
void handler_side_Omajinai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_StealthRock( u32* numElems );
void handler_side_StealthRock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_WideGuard( u32* numElems );
void handler_side_WideGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_FastGuard( u32* numElems );
void handler_side_FastGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
void handler_side_FastGuard_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
void handler_side_FastGuard_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_TatamiGaeshi( u32* numElems );
void handler_side_TatamiGaeshi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
void handler_side_TatamiGaeshi_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
void handler_side_Tatamigaeshi_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* ADD_SIDE_TrickGuard( u32* numElems );
void handler_side_TrickGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_Makibisi( u32* numElems );
void handler_side_Makibisi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_Dokubisi( u32* numElems );
void handler_side_Dokubisi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_Rainbow( u32* numElems );
void handler_Rainbow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
void handler_Rainbow_Shrink( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_Burning( u32* numElems );
void handler_side_Burning( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_Moor( u32* numElems );
void handler_side_Moor( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_NebaNebaNet( u32* numElems );
void handler_side_NebaNebaNet( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_AuroraVeil( u32* numElems );
void handler_side_AuroraVeil( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );
const BtlEventHandlerTable* ADD_SIDE_SpotLight( u32* numElems );
void handler_SpotLight_TemptTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work );



enum 
{
  WORKIDX_SICKCONT_HIGH = EVENT_HANDLER_WORK_ELEMS - 1,  // サイドエフェクト発生時の BTL_SICKCONT( 上位32bit )
  WORKIDX_SICKCONT_LOW  = EVENT_HANDLER_WORK_ELEMS - 2,  // サイドエフェクト発生時の BTL_SICKCONT( 下位32bit )
};


/**
 * @brief イベントファクターのワークから、サイドエフェクト発生時のSICKCONTを取得する
 * @param[out] sickcont  取得したSICKCONTの格納先
 * @param      work      取得対象のワーク
 */
void GetSickCont( BTL_SICKCONT* sickcont, int* work )
{
  SICKCONT_Unite32bit( sickcont, work[ WORKIDX_SICKCONT_HIGH ], work[ WORKIDX_SICKCONT_LOW ] );
}

/**
 * @brief 指定したサイドエフェクトのイベントファクターを取得する
 * @param side         取得対象の陣営
 * @param sideEffect   取得対象のサイドエフェクト
 * @retval NULL  指定したサイドエフェクトのイベントファクターが存在しない場合
 */
BTL_EVENT_FACTOR* GetEventFactor( BtlSide side, BtlSideEffect sideEffect )
{
  BTL_EVENT_FACTOR* factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_SIDE, side );

  while( factor )
  {
    if( EVENT_FACTOR_GetSubID( factor ) == sideEffect ) {
      return factor;
    }
    factor = EVENT_GetNextFactor( factor );
  }

  return NULL;
}



//=============================================================================================
/**
 * ハンドラ１件追加
 *
 * @param   side
 * @param   sideEffect
 * @param   contParam
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR*  Add( BtlSide side, BtlSideEffect sideEffect, const BTL_SICKCONT& contParam )
{
  typedef const BtlEventHandlerTable* (*pEventAddFunc)( u32* numHandlers );

  static const struct {
    BtlSideEffect  eff;
    pEventAddFunc  func;
  }funcTbl[] = {
    { BTL_SIDEEFF_REFRECTOR,      ADD_SIDE_Refrector     },
    { BTL_SIDEEFF_HIKARINOKABE,   ADD_SIDE_Hikarinokabe  },
    { BTL_SIDEEFF_SINPINOMAMORI,  ADD_SIDE_Sinpinomamori },
    { BTL_SIDEEFF_SIROIKIRI,      ADD_SIDE_SiroiKiri     },
    { BTL_SIDEEFF_OIKAZE,         ADD_SIDE_Oikaze        },
    { BTL_SIDEEFF_OMAJINAI,       ADD_SIDE_Omajinai      },
    { BTL_SIDEEFF_MAKIBISI,       ADD_SIDE_Makibisi      },
    { BTL_SIDEEFF_DOKUBISI,       ADD_SIDE_Dokubisi      },
    { BTL_SIDEEFF_STEALTHROCK,    ADD_SIDE_StealthRock   },
    { BTL_SIDEEFF_WIDEGUARD,      ADD_SIDE_WideGuard     },
    { BTL_SIDEEFF_FASTGUARD,      ADD_SIDE_FastGuard     },
    { BTL_SIDEEFF_RAINBOW,        ADD_SIDE_Rainbow       },
    { BTL_SIDEEFF_BURNING,        ADD_SIDE_Burning       },
    { BTL_SIDEEFF_MOOR,           ADD_SIDE_Moor          },
    { BTL_SIDEEFF_NEBANEBANET,    ADD_SIDE_NebaNebaNet   },
    { BTL_SIDEEFF_TATAMIGAESHI,   ADD_SIDE_TatamiGaeshi  },
    { BTL_SIDEEFF_TRICKGUARD,     ADD_SIDE_TrickGuard    },
    { BTL_SIDEEFF_AURORAVEIL,     ADD_SIDE_AuroraVeil    },
    { BTL_SIDEEFF_SPOTLIGHT,      ADD_SIDE_SpotLight     },
  };

  GFL_ASSERT(side < BTL_SIDE_NUM);
  GFL_ASSERT(sideEffect < BTL_SIDEEFF_MAX);

  // 既に登録済み
  {
    BTL_EVENT_FACTOR* factor = GetEventFactor( side, sideEffect );
    if( factor != NULL ) {
      return factor;
    }
  }

  for(u32 i=0; i<GFL_NELEMS(funcTbl); ++i)
  {
    if( funcTbl[i].eff == sideEffect )
    {
      u32 numHandlers;
      const BtlEventHandlerTable* handlerTable = funcTbl[i].func( &numHandlers );
      BTL_EVENT_FACTOR* factor = EVENT_AddFactor( BTL_EVENT_FACTOR_SIDE, sideEffect, BTL_EVPRI_SIDE_DEFAULT, 0, side, handlerTable, numHandlers );
      GFL_ASSERT_STOP(factor);

      // サイドエフェクト発生時のSICKCONTをワークにセット
      {
        u32 high = 0;
        u32 low = 0;
        SICKCONT_Split32bit( contParam, &high, &low );
        EVENT_FACTOR_SetWorkValue( factor, WORKIDX_SICKCONT_HIGH, high );
        EVENT_FACTOR_SetWorkValue( factor, WORKIDX_SICKCONT_LOW, low );
      }

      return factor;
    }
  }

  GFL_ASSERT_STOP(0);
  return NULL;
}
//=============================================================================================
/**
 * ハンドラ１件削除
 *
 * @param   side
 * @param   sideEffect
 *
 * @retval  bool
 */
//=============================================================================================
bool Remove( BtlSide side, BtlSideEffect sideEffect )
{
  BTL_EVENT_FACTOR* factor = GetEventFactor( side, sideEffect );

  if( factor )
  {
    EVENT_FACTOR_Remove( factor );
    return true;
  }

  return false;
}
//=============================================================================================
/**
 * ハンドラ一時停止
 * @param side
 * @param sideEffect
 * @return  この呼び出しにより一時停止したら true
 */
//=============================================================================================
bool Sleep( BtlSide side, BtlSideEffect sideEffect )
{
  BTL_EVENT_FACTOR* factor = GetEventFactor( side, sideEffect );

  if( factor )
  {
    return EVENT_FACTOR_Sleep( factor );
  }

  return false;
}
//=============================================================================================
/**
 * ハンドラ一時停止を解除
 * @param side
 * @param sideEffect
 * @return  この呼び出しにより一時停止を解除したら true
 */
//=============================================================================================
bool Weak( BtlSide side, BtlSideEffect sideEffect )
{
  BTL_EVENT_FACTOR* factor = GetEventFactor( side, sideEffect );

  if( factor )
  {
    return EVENT_FACTOR_Weak( factor );
  }

  return false;
}


//=============================================================================================
/**
 * 特定サイドエフェクトが存在しているか判定
 *
 * @param   side
 * @param   effect
 *
 * @retval  BOOL
 */
//=============================================================================================
bool IsExist( BtlSide side, BtlSideEffect effect )
{
  BTL_EVENT_FACTOR* factor = GetEventFactor( side, effect );
  return ( factor != NULL );
}


//----------------------------------------------------------------------------------
/**
 * 自分の重ねがけカウンタ値を取得
 *
 * @param   myHandle
 * @param   side
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
u8 getMyAddCounter( const BTL_EVENT_FACTOR* myHandle, const ServerFlow* serverFlow, BtlSide side )
{
  BtlSideEffect sideEffect = static_cast<BtlSideEffect>( EVENT_FACTOR_GetSubID( myHandle ) );
  const SideEffectStatus* sideEffectStatus = serverFlow->Hnd_GetSideEffectStatus( side, sideEffect );
  return sideEffectStatus->GetAddCount();
}

//--------------------------------------------------------------------------------------
/**
 *  リフレクター
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_Refrector( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,  handler_side_Refrector   },  // ダメージ補正
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_Refrector( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_wallEffect( flowWk, mySide, pml::wazadata::DMG_PHYSIC );
}
//--------------------------------------------------------------------------------------
/**
 *  ひかりのかべ
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_Hikarinokabe( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,  handler_side_HikariNoKabe   },  // ダメージ補正
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_wallEffect( flowWk, mySide, pml::wazadata::DMG_SPECIAL );
}
//--------------------------------------------------------------------------------------
/**
 *  オーロラベール
 *
 * ５ターンの間、敵の物理攻撃、特殊攻撃のダメージを半減させる。
 * ダブルバトルでは0.66％の軽減になる
 * 「リフレクター・ひかりのかべ」と共存するが、
 * 「オーロラベール」と「ひかりのかべ」、オーロラベール」と「リフレクター」の効果は重ならず、ダメージ半減のままである。
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_AuroraVeil( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3, handler_side_AuroraVeil },  // ダメージ補正
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_AuroraVeil( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_wallEffect( flowWk, mySide, pml::wazadata::DMG_PHYSIC );
  common_wallEffect( flowWk, mySide, pml::wazadata::DMG_SPECIAL );
}
/**
 *  リフレクター・ひかりのかべ・オーロラベール共通
 */
void common_wallEffect( ServerFlow* flowWk, u8 mySide, WazaDamageType dmgType )
{
  BtlEvVarLabel wallEffectFlag = ( dmgType == pml::wazadata::DMG_PHYSIC ) ? ( BTL_EVAR_WALL_EFFECT_FLAG_PHYSIC ) : ( BTL_EVAR_WALL_EFFECT_FLAG_SPECIAL );

  u8 defPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(defPokeID) == mySide )
  {
    if( (EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == dmgType)
    &&  (EVENTVAR_GetValue(BTL_EVAR_CRITICAL_FLAG) == false)
    &&  (EVENTVAR_GetValue(wallEffectFlag) == false)
    ){
      BtlRule rule = flowWk->Hnd_GetRule( );
      fx32 ratio = common_wallEffect_GetDamageRatio( rule );
      EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
      EVENTVAR_RewriteValue( wallEffectFlag, true );
    }
  }
}

/**
 * @brief 「リフレクター・ひかりのかべ・オーロラベール」によるダメージ補正倍率を取得する
 * @param rule  バトルのルール
 */
fx32 common_wallEffect_GetDamageRatio( BtlRule rule )
{
  if( rule == BTL_RULE_SINGLE )
  {
    return FX32_CONST( 0.5f );
  }

  return FX32_CONST( 0.667f );
}


//--------------------------------------------------------------------------------------
/**
 *  しんぴのまもり
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_Sinpinomamori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,  handler_side_SinpiNoMamori_CheckFail   },  // 状態異常失敗チェック
    { BTL_EVENT_ADDSICK_FAILED,     handler_side_SinpiNoMamori_FixFail     },  // 失敗確定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_SinpiNoMamori_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

  if( (flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide)
  &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID)  // 自分で自分に打つワザ（ねむる等）には効かない
  ){
    WazaSick  sickID = static_cast<WazaSick>(EVENTVAR_GetValue( BTL_EVAR_SICKID ));
    if( calc::IsBasicSickID(sickID)
    ||  (sickID == pml::wazadata::WAZASICK_KONRAN)
    ||  (sickID == pml::wazadata::WAZASICK_AKUBI)
    ){
      work[0] = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    }
  }
}
void handler_side_SinpiNoMamori_FixFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(mySide);
  if( work[0] )
  {
    u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SinpiNoMamori_Exe );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );
    work[0] = 0;
  }
}
//--------------------------------------------------------------------------------------
/**
 *  しろいきり
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_SiroiKiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK,  handler_side_SiroiKiri_CheckFail   },  // ランク増減失敗チェック
    { BTL_EVENT_RANKEFF_FAILED,      handler_side_SiroiKiri_FixFail     },  // 失敗確定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_SiroiKiri_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide)
  &&  (EVENTVAR_GetValue(BTL_EVAR_VOLUME) < 0)
  &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID)
  ){
    work[0] = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
void handler_side_SiroiKiri_FixFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(mySide);
  if( work[0] )
  {
    int wazaSerial = EVENTVAR_GetValue( BTL_EVAR_WAZA_SERIAL );
    if( (wazaSerial == WAZASERIAL_NULL)
    ||  (work[1] != wazaSerial)
    ){
      u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
        BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push(BTL_HANDEX_MESSAGE, pokeID));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SiroiKiri_Exe );
        HANDEX_STR_AddArg( &param->str, pokeID );
        HANDEX_STR_SetFailMsgFlag( &param->str );
      flowWk->HANDEX_Pop( param );

      work[1] = wazaSerial;
    }

    work[0] = 0;
  }
}
//--------------------------------------------------------------------------------------
/**
 *  おいかぜ
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_Oikaze( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,  handler_side_Oikaze  },  // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// すばやさ計算ハンドラ
void handler_side_Oikaze( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide)
  ){
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  おまじない
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_Omajinai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK,  handler_side_Omajinai  },  // クリティカルチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_Omajinai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( (flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide)
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ステルスロック
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_StealthRock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_side_StealthRock  },  // メンバー入場ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_StealthRock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide)
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    BtlTypeAff  affinity = calc::TypeAffPair( POKETYPE_IWA, bpp->GetPokeType() );
    u32 denom;

    switch( affinity ){
    case pml::battle::TypeAffinity::TYPEAFF_1:
    default:
      denom =  8;
      break;
    case pml::battle::TypeAffinity::TYPEAFF_1_2:   denom =  16;  break;
    case pml::battle::TypeAffinity::TYPEAFF_1_4:   denom =  32;  break;
    case pml::battle::TypeAffinity::TYPEAFF_1_8:   denom =  64;  break;
    case pml::battle::TypeAffinity::TYPEAFF_1_16:  denom =  128;  break;

    case pml::battle::TypeAffinity::TYPEAFF_1_32:
    case pml::battle::TypeAffinity::TYPEAFF_1_64:
        denom =  256;
        break;

    case pml::battle::TypeAffinity::TYPEAFF_2:   denom =  4; break;
    case pml::battle::TypeAffinity::TYPEAFF_4:   denom =  2; break;

    case pml::battle::TypeAffinity::TYPEAFF_8:
    case pml::battle::TypeAffinity::TYPEAFF_16:
    case pml::battle::TypeAffinity::TYPEAFF_32:
    case pml::battle::TypeAffinity::TYPEAFF_64:
      denom =  1;
      break;
    }

    // ダメージ
    {
      BTL_SICKCONT cont;
      GetSickCont( &cont, work );

      BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, BTL_POKEID_NULL ));
        param->pokeID = pokeID;
        param->damage = calc::QuotMaxHP( bpp, denom );
        param->damageCause = DAMAGE_CAUSE_STEALTHROCK;
        param->damageCausePokeID = cont.causePokeID;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_StealthRockDamage );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }

    // ポケモンに汚れをセット
    {
      BTL_HANDEX_PARAM_SET_DIRT* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_DIRT *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_DIRT, BTL_POKEID_NULL ));
        param->pokeID = pokeID;
        param->dirtType = pml::pokepara::DIRT_TYPE_SAND;
      flowWk->HANDEX_Pop( param );
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ワイドガード
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_WideGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD,  handler_side_WideGuard                     },  // 防御系サイドエフェクトによるワザ無効化
    { BTL_EVENT_WAZA_DMG_PROC_Z,               handler_side_TatamiGaeshi_DmgZ             },  // Z技に対するダメージ補正
    { BTL_EVENT_AFTER_CRITICAL,                handler_side_Tatamigaeshi_MsgAfterCritical },  // 急所メッセージ後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_WideGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 防御ポケが自分サイドで
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide)
  {
    // ワザが「敵全体」「自分以外全部」の効果範囲かつ(全力技＆攻撃技)ではないなら無効化
    WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    WazaTarget  targetType = static_cast<WazaTarget>(WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_TARGET ));
    // @fix MMCat[46]ジャラランガの専用Z技でワイドガードを突破できない
    if(((targetType == pml::wazadata::TARGET_ENEMY_ALL) || (targetType == pml::wazadata::TARGET_OTHER_ALL)) && 
       !(EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZA_FLAG ) && WAZADATA_IsDamage(waza) )
      )
    {
      if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
      {
        // 無効化エフェクト
        {
          const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
          if( !bpp->IsWazaHide() )
          {
            BTL_HANDEX_PARAM_ADD_EFFECT* eff_param;
            eff_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, pokeID ));
              eff_param->effectNo = BTLEFF_GUARD;
              eff_param->pos_from = flowWk->Hnd_PokeIDtoPokePos( pokeID );
              eff_param->pos_to = BTL_POS_NULL;
              eff_param->fMsgWinVanish = true;
            flowWk->HANDEX_Pop( eff_param );
          }
        }

        //「○○は　ワイドガードで　まもられた！」
        {
          BTL_HANDEX_STR_PARAMS* strParam = (BTL_HANDEX_STR_PARAMS*)EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
          HANDEX_STR_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_WideGuard );
          HANDEX_STR_AddArg( strParam, pokeID );
        }
      }
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ファストガード
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_FastGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD, handler_side_FastGuard                   },  // 防御系サイドエフェクトによるワザ無効化
    { BTL_EVENT_FREEFALL_START_GUARD,         handler_side_FastGuard                   },  // フリーフォール溜めターンガードチェック
    { BTL_EVENT_WAZA_DMG_PROC_Z,              handler_side_FastGuard_DmgZ              },  // Z技に対するダメージ補正
    { BTL_EVENT_AFTER_CRITICAL,               handler_side_FastGuard_MsgAfterCritical  },  // 急所メッセージ後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_FastGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 防御ポケが自分サイドで
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide)
  {
    // ワザの使用ポケが対象ポケではなく、ワザ優先度が１以上の場合、ガードする
    if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
    {
      WazaID  waza = (WazaID)(EVENTVAR_GetValue(BTL_EVAR_WAZAID));
      if( (EVENTVAR_GetValue(BTL_EVAR_WAZA_PRI) >= 1)
      &&  (WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_Mamoru))
      )
      {
        if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
        {
          // 無効化エフェクト
          {
            const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
            if( !bpp->IsWazaHide() )
            {
              BTL_HANDEX_PARAM_ADD_EFFECT* eff_param;
              eff_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, pokeID ));
                eff_param->effectNo = BTLEFF_GUARD;
                eff_param->pos_from = flowWk->Hnd_PokeIDtoPokePos( pokeID );
                eff_param->pos_to = BTL_POS_NULL;
                eff_param->fMsgWinVanish = true;
              flowWk->HANDEX_Pop( eff_param );
            }
          }

          //「○○は　ファストガードで　まもられた！」
          {
            BTL_HANDEX_STR_PARAMS* strParam = (BTL_HANDEX_STR_PARAMS*)EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
            HANDEX_STR_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_FastGuard );
            HANDEX_STR_AddArg( strParam, pokeID );
          }
        }
      }
    }
  }
}

// Z技に対するダメージ補正
void handler_side_FastGuard_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( ( flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide ) &&
      ( EVENTVAR_GetValue(BTL_EVAR_WAZA_PRI) >= 1 ) )
  {
    if( EVENTVAR_RewriteValue( BTL_EVAR_RATIO_EX, FX32_CONST(0.25) ) )
    {
      work[0] = 1;
    }
  }
}

// 急所メッセージ後
void handler_side_FastGuard_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);

  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );

  if( ( side == mySide ) &&
      ( work[0] == 1 ) )
  {
     BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
       HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_protectpierce );  //「こうげきを　まもりきれずに　ダメージを　うけた！」
       HANDEX_STR_AddArg( &param->str, pokeID );
     flowWk->HANDEX_Pop( param );

     work[0] = 0;
  }
}


//--------------------------------------------------------------------------------------
/**
 *  たたみがえし
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_TatamiGaeshi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_MAMORU,  handler_side_TatamiGaeshi                   },  // ワザ無効化（まもると同一）
    { BTL_EVENT_FREEFALL_START_GUARD,   handler_side_TatamiGaeshi                   },  // フリーフォール溜めターンガードチェック
    { BTL_EVENT_WAZA_DMG_PROC_Z,        handler_side_TatamiGaeshi_DmgZ              },  // Z技に対するダメージ補正
    { BTL_EVENT_AFTER_CRITICAL,         handler_side_Tatamigaeshi_MsgAfterCritical  },  // 急所メッセージ後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_TatamiGaeshi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 防御ポケが自分サイドで
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide)
  {
    // ワザがダメージワザなら無効化
    WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( (WAZADATA_IsDamage(waza) )
    ){
      if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
      {
        // 無効化エフェクト
        {
          const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
          if( !bpp->IsWazaHide() )
          {
            BTL_HANDEX_PARAM_ADD_EFFECT* eff_param;
            eff_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, pokeID ));
              eff_param->effectNo = BTLEFF_GUARD;
              eff_param->pos_from = flowWk->Hnd_PokeIDtoPokePos( pokeID );
              eff_param->pos_to = BTL_POS_NULL;
              eff_param->fMsgWinVanish = true;
            flowWk->HANDEX_Pop( eff_param );
          }
        }

        //「○○は　たたみがえしで　ふせがれた！」
        {
          BTL_HANDEX_STR_PARAMS* strParam = (BTL_HANDEX_STR_PARAMS*)EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
          HANDEX_STR_Setup( strParam, BTL_STRTYPE_STD, BTL_STRID_STD_TatamiGaeshi );
          HANDEX_STR_AddArg( strParam, waza );
        }

        EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );  // まもったメッセージ抑制
      }
    }
  }
}

// Z技に対するダメージ補正
void handler_side_TatamiGaeshi_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide )
  {
    if( EVENTVAR_RewriteValue( BTL_EVAR_RATIO_EX, FX32_CONST(0.25) ) )
    {
      // 貫通メッセージを２体分表示できるようにするため、インクリメント方式に変更
      // workの配列数は７だが自軍のポケモン数は最大でも２匹なので範囲外アクセスにはなりえない
      // pokeID保存
      ++work[0];
      if ((work[0] > 0) && (work[0] < EVENT_HANDLER_WORK_ELEMS))
      {
        work[work[0]] = pokeID;
      }
      else
      {
        GFL_ASSERT_STOP(0);//ありえない状況
      }
    }
  }
}

// 急所メッセージ後
void handler_side_Tatamigaeshi_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);

  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  bool isTargetPoke = false;

  if (side == mySide)
  {
    for (u32 index(1); index < (work[0] + 1); ++index)
    {
      if (work[index] == pokeID)
      {
        isTargetPoke = true;
        work[index] = 0;
        break;
      }
    }

    if (isTargetPoke)
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>(flowWk->HANDEX_Push(BTL_HANDEX_MESSAGE, pokeID));
      HANDEX_STR_Setup(&param->str, BTL_STRTYPE_SET, BTL_STRID_SET_protectpierce);  //「こうげきを　まもりきれずに　ダメージを　うけた！」
      HANDEX_STR_AddArg(&param->str, pokeID);
      flowWk->HANDEX_Pop(param);
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 *  トリックガード
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_TrickGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD,  handler_side_TrickGuard  },  // 防御系サイドエフェクトによるワザ無効化
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_TrickGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 防御ポケが自分サイドで
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide)
  {
    // ワザが非ダメージワザ かつ ワザ使用者!=対象者 なら無効化
    WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    pml::wazadata::WazaTarget  targetType = static_cast<pml::wazadata::WazaTarget>(EVENTVAR_GetValue( BTL_EVAR_TARGET_TYPE ));
    if( (!WAZADATA_IsDamage(waza) )
    &&  (pokeID != EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
    &&  (targetType != pml::wazadata::TARGET_ALL)         // ほろびのうたなど、全ポケ対象のワザは防げない
    &&  (targetType != pml::wazadata::TARGET_FRIEND_ALL)  // じばそうさなど、味方全ポケ対象のワザは防げない
    ){
      if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
      {
        BTL_HANDEX_STR_PARAMS* strParam = (BTL_HANDEX_STR_PARAMS*)EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
        HANDEX_STR_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_TrickGuard );
        HANDEX_STR_AddArg( strParam, pokeID );
      }
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 *  まきびし
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_Makibisi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_side_Makibisi  },  // 入場チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_Makibisi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(work);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide )
  {
    if( !flowWk->Hnd_IsFloatingPoke(pokeID) )
    {
      u8 add_counter = getMyAddCounter( myHandle, flowWk, mySide );
      u8 denom;
      switch( add_counter ) {
      case 1:
      default:
        denom = 8; break;
      case 2:
        denom = 6; break;
      case 3:
        denom = 4; break;
      }

      // ダメージ
      {
        BTL_SICKCONT cont;
        GetSickCont( &cont, work );

        BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, BTL_POKEID_NULL ));
          const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
          param->pokeID = pokeID;
          param->damage = calc::QuotMaxHP( bpp, denom );
          param->damageCause = DAMAGE_CAUSE_MAKIBISI;
          param->damageCausePokeID = cont.causePokeID;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_MakibisiDamage );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
        flowWk->HANDEX_Pop( param );
      }

      // ポケモンに汚れをセット
      {
        BTL_HANDEX_PARAM_SET_DIRT* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_DIRT *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_DIRT, BTL_POKEID_NULL ));
          param->pokeID = pokeID;
          param->dirtType = pml::pokepara::DIRT_TYPE_SAND;
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  どくびし
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_Dokubisi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_side_Dokubisi  },  // 入場チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_Dokubisi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(work);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide )
  {
    if( !flowWk->Hnd_IsFloatingPoke( pokeID ) )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

      if( bpp->IsMatchType(POKETYPE_DOKU) )
      {
        BTL_HANDEX_PARAM_SIDEEFF_REMOVE* side_param;
        side_param = reinterpret_cast<BTL_HANDEX_PARAM_SIDEEFF_REMOVE *>( flowWk->HANDEX_Push( BTL_HANDEX_SIDEEFF_REMOVE, pokeID ));
          side_param->side = mySide;
          calc::BITFLG_Construction( side_param->flags, sizeof(side_param->flags) );
          calc::BITFLG_Set( side_param->flags, BTL_SIDEEFF_DOKUBISI );
        flowWk->HANDEX_Pop( side_param );
      }
      else
      {
        BTL_SICKCONT cont;
        GetSickCont( &cont, work );

        BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, BTL_POKEID_NULL ));

          param->sickID = pml::wazadata::WAZASICK_DOKU;
          if( getMyAddCounter(myHandle, flowWk, mySide) > 1 ){
            param->sickCont = SICKCONT_MakeMoudokuCont( cont.causePokeID );
          }else{
            param->sickCont = SICKCONT_MakePermanent( cont.causePokeID );
          }

          param->pokeID = pokeID;
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 *  ねばねばネット
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_NebaNebaNet( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_side_NebaNebaNet  },  // 入場チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_side_NebaNebaNet( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(work);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide )
  {
    if( !flowWk->Hnd_IsFloatingPoke( pokeID ) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, BTL_POKEID_NULL ));
        param->poke_cnt   = 1;
        param->pokeID[0]  = pokeID;
        param->rankType   = pml::wazadata::RANKEFF_AGILITY;
        param->rankVolume = -1;
        param->fMsgPreEffect = true;
        param->bMigawariThrew = true;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_NebaNebaNet );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 *  スポットライト
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_SpotLight( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TEMPT_TARGET,    handler_SpotLight_TemptTarget },  // ターゲット誘導
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ターゲット誘導
void handler_SpotLight_TemptTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  BTL_SICKCONT cont;
  GetSickCont( &cont, work );

  u8 targetPokeID = SICKCONT_GetPokeID( cont );
  if( targetPokeID == BTL_POKEID_NULL ){
    return;
  }
  
  if( !flowWk->Hnd_IsExistPokemon(targetPokeID) ){
    return;
  }

  HandCommon_KonoyubiTomare_TemptTarget( myHandle, flowWk, targetPokeID, TEMPT_TARGET_PRIORITY_WAZA_2, TEMPT_TARGET_CAUSE_OTHER, work );
}



//--------------------------------------------------------------------------------------
/**
 *  合体ワザ効果：虹
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_Rainbow( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK,         handler_Rainbow },  // 追加効果（状態異常）チェックハンドラ
    { BTL_EVENT_ADD_RANK_TARGET,  handler_Rainbow },  // 追加効果（ランク効果）チェックハンドラ
    { BTL_EVENT_WAZA_SHRINK_PER,  handler_Rainbow_Shrink },  // ひるみチェックハンドラ

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Rainbow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

  if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(atkPokeID) == mySide )
  {
    u16 per = EVENTVAR_GetValue( BTL_EVAR_ADD_PER ) * 2;
    EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, per );
  }
}
void handler_Rainbow_Shrink( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

  if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(atkPokeID) == mySide )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_FLAG, true );
  }
}


//--------------------------------------------------------------------------------------
/**
 *  合体ワザ効果：火の海
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_Burning( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,  handler_side_Burning  },  // ターンチェック開始ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ターンチェック開始ハンドラ
void handler_side_Burning( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );

  if( (pokeID != BTL_POKEID_NULL)
  &&  (flowWk->Hnd_IsExistPokemon( pokeID))
  &&  (!flowWk->Hnd_CheckShowDown())
  ){
    if( flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( !bpp->IsMatchType(POKETYPE_HONOO) )
      {
        BTL_HANDEX_PARAM_DAMAGE* param;

        // エフェクト発動させる
        {
          BTL_HANDEX_PARAM_ADD_EFFECT*  viewEff_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, pokeID ));
            viewEff_param->pos_from = flowWk->Hnd_PokeIDtoPokePos( pokeID );
            viewEff_param->pos_to   = BTL_POS_NULL;
            viewEff_param->effectNo = BTLEFF_BURNING;
          flowWk->HANDEX_Pop( viewEff_param );
        }

        BTL_SICKCONT cont;
        GetSickCont( &cont, work );

        param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, BTL_POKEID_NULL ));
          param->pokeID = pokeID;
          param->damage = calc::QuotMaxHP( bpp, 8 );
          param->damageCause = DAMAGE_CAUSE_BURNING;
          param->damageCausePokeID = cont.causePokeID;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_BurningDamage );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 *  合体ワザ効果：湿原
 */
//--------------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_SIDE_Moor( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,  handler_side_Moor  },  // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// すばやさ計算ハンドラ
void handler_side_Moor( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 mySide, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  u8 pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) == mySide)
  ){
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.25) );
  }
}


//----------------------------------------------------------------------------------
} // end of namespace 'side'
} // end of namespace 'handler'
GFL_NAMESPACE_END(btl)


