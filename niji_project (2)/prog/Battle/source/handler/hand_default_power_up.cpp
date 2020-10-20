//=============================================================================
/**
 * @file   hand_default_power_up.cpp
 * @date   2015/09/09 11:14:44
 * @author obata_toshihiro
 * @brief  イベントファクター( デフォルト能力アップ )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <battle/include/battle_DefaultPowerUp.h>
#include "../btl_str.h"
#include "../btl_ServerFlow.h"
#include "../btl_pokeparam.h"
#include "./hand_common.h"
#include "./hand_default_power_up.h"


namespace btl {
namespace handler {
namespace default_power_up {


void removeFactorForce( u8 pokeId, DefaultPowerUpReason powerUpReason );

const BtlEventHandlerTable* ADD_Nusi( u32* numElems );
void handler_Nusi_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* serverFlow, u8 pokeId, int* factorWork );
void applyNusiRankUp( ServerFlow* serverFlow, u8 pokeId, WazaRankEffect rankType, s8 rankVolume );



/**
 * @brief イベントハンドラテーブルを取得する関数の型
 * @param[out]  ハンドラテーブルの要素数
 * @return ハンドラテーブル
 */
typedef const BtlEventHandlerTable* (*pEventAddFunc)( u32* numHandlers );


static const struct 
{
  DefaultPowerUpReason  powerUpReason; // 能力アップする理由
  pEventAddFunc         addFunc;       // ハンドラテーブルを取得する関数
}
FACTOR_TABLE[] = 
{
  { DEFAULT_POWERUP_REASON_NUSI,        ADD_Nusi },
  { DEFAULT_POWERUP_REASON_ULTRA_BEAST, ADD_Nusi },
  { DEFAULT_POWERUP_REASON_BOSS,        ADD_Nusi },
};



//-----------------------------------------------------------------------------
/**
 * @brief デフォルト能力アップに対するイベントファクターをシステムに追加する
 *
 * @param poke   能力アップするポケモン
 *
 * @retval true   ハンドラを追加した
 * @retval false  ハンドラを追加できなかった
 */
//-----------------------------------------------------------------------------
bool Add( const BTL_POKEPARAM* poke )
{
  const DefaultPowerUpReason powerUpReason = poke->GetDefaultPowerUpDesc().reason;

  if( DEFAULT_POWERUP_REASON_NUM <= powerUpReason ) {
    GFL_ASSERT(0);
    return false;
  }

  for( u32 i=0; i<GFL_NELEMS(FACTOR_TABLE); ++i )
  {
    if( FACTOR_TABLE[i].powerUpReason != powerUpReason ) {
      continue;
    }

    u32 numHandlers;
    const BtlEventHandlerTable* handlerTable = FACTOR_TABLE[i].addFunc( &numHandlers );
    u16 subId = powerUpReason;
    u32 subPriority = poke->GetValue_Base( BTL_POKEPARAM::BPP_AGILITY );
    u8 dependId = poke->GetID();
    BTL_EVENT_FACTOR* factor = EVENT_AddFactor( BTL_EVENT_FACTOR_DEFAULT_POWERUP, subId, BTL_EVPRI_ZENRYOKUWAZA_DEFAULT, subPriority, dependId, handlerTable, numHandlers );
    return ( factor != NULL );
  }

  return false;
}


//-----------------------------------------------------------------------
/**
 * @brief デフォルト能力アップに対するイベントファクターをシステムから除外する
 *
 * @param poke   能力アップするポケモン
 */
//-----------------------------------------------------------------------
void Remove( const BTL_POKEPARAM* poke )
{
  u8 pokeId = poke->GetID();
  DefaultPowerUpReason reason = poke->GetDefaultPowerUpDesc().reason;
  removeFactorForce( pokeId, reason );
}

/**
 * @brief 指定したイベントファクタを削除する
 * @param pokeId          能力アップするポケモンのID
 * @param powerUpReason   能力アップする理由
 */
void removeFactorForce( u8 pokeId, DefaultPowerUpReason powerUpReason )
{
  BTL_EVENT_FACTOR* factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_DEFAULT_POWERUP, pokeId );
  while( factor )
  {
    if( EVENT_FACTOR_GetSubID(factor) == powerUpReason ) 
    {
      EVENT_FACTOR_Remove( factor );
      factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_DEFAULT_POWERUP, pokeId );
    }
    else
    {
      factor = EVENT_GetNextFactor( factor );
    }
  }
}





//-----------------------------------------------------------------------------
/**
 * デフォルト能力アップ「ぬし」
 */
//-----------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Nusi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN_DEFAULT_POWERUP,  handler_Nusi_MemberIn },  // 個別ポケ入場直後、デフォルト能力アップ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief 個別ポケ入場直後、デフォルト能力アップ　イベント
 *
 * @param myHandle
 * @param serverFlow
 * @param pokeId       能力アップ対象ポケモンのID
 * @param factorWork   自身のファクターで使用できる作業領域
 */
void handler_Nusi_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* serverFlow, u8 pokeId, int* factorWork )
{
  u8 targetPokeId = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( targetPokeId != pokeId ) {
    return;
  }

  const BTL_POKEPARAM* pokeParam = serverFlow->Hnd_GetPokeParam( pokeId );
  const DefaultPowerUpDesc& powerUpDesc = pokeParam->GetDefaultPowerUpDesc();

  // 能力ランク上昇
  applyNusiRankUp( serverFlow, pokeId, pml::wazadata::RANKEFF_ATTACK,     powerUpDesc.rankUp_Attack );
  applyNusiRankUp( serverFlow, pokeId, pml::wazadata::RANKEFF_DEFENCE,    powerUpDesc.rankUp_Defense );
  applyNusiRankUp( serverFlow, pokeId, pml::wazadata::RANKEFF_SP_ATTACK,  powerUpDesc.rankUp_SpAttack );
  applyNusiRankUp( serverFlow, pokeId, pml::wazadata::RANKEFF_SP_DEFENCE, powerUpDesc.rankUp_SpDefense );
  applyNusiRankUp( serverFlow, pokeId, pml::wazadata::RANKEFF_AGILITY,    powerUpDesc.rankUp_Agility );
}

/**
 * @brief 「ぬし」としてのランク上昇効果を適用する
 * @param serverFlow  使用する ServerFlow
 * @param pokeId      対象ポケモンのID
 * @param rankType    上昇するパラメータ( こうげき、ぼうぎょ　など )
 * @param rankVolume  上昇するランク数
 */
void applyNusiRankUp( ServerFlow* serverFlow, u8 pokeId, WazaRankEffect rankType, s8 rankVolume )
{
  if( rankVolume <= 0 )
  {
    return;
  }

  BTL_HANDEX_PARAM_RANK_EFFECT* rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( serverFlow->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeId ));
    rank_param->rankType           = rankType;
    rank_param->rankVolume         = rankVolume;
    rank_param->fAlmost            = false;
    rank_param->fWazaEffect        = false;
    rank_param->bMigawariThrew     = true;
    rank_param->bMigawariCheck     = false;
    rank_param->fStdMsgDisable     = true;
    rank_param->fViewEffectDisable = true;  // ランク上昇エフェクトを再生しません
    rank_param->poke_cnt           = 1;
    rank_param->pokeID[0]          = pokeId;
  serverFlow->HANDEX_Pop( rank_param );
}



} // namespace pos
} // namespace handler
} // namespace btl

