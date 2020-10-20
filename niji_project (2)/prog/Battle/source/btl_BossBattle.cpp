//=============================================================================
/**
 * @file   btl_BossBattle.cpp
 * @date   2016/01/24 14:31:37
 * @author obata_toshihiro
 * @brief  代表ビースト戦
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_BossBattle.h"

#include <Battle/include/battle_DefaultPowerUp.h>
#include "./btl_common.h"


GFL_NAMESPACE_BEGIN( btl )


namespace
{

  /**
   * @brief ボスの能力アップ設定
   */
  struct BossPowerUpDesc
  {
    MonsNo monsno;
    WazaRankEffect rankType;
    u8 rankValue;
    f32 auraColor_r;
    f32 auraColor_g;
    f32 auraColor_b;
  };

  static const BossPowerUpDesc BOSS_POWERUP_TABLE[] = 
  {
    { MONSNO_PIKUSII,    pml::wazadata::RANKEFF_SP_DEFENCE, 1, 1.0f, 1.0f, 1.0f },
    { MONSNO_DOREDHIA,   pml::wazadata::RANKEFF_SP_ATTACK,  1, 1.0f, 1.0f, 1.0f },
    { MONSNO_MUUMAAZI,   pml::wazadata::RANKEFF_AGILITY,    1, 1.0f, 1.0f, 1.0f },
    { MONSNO_MIROKAROSU, pml::wazadata::RANKEFF_ATTACK,     1, 1.0f, 1.0f, 1.0f },
    { MONSNO_RESSAA2,    pml::wazadata::RANKEFF_DEFENCE,    1, 1.0f, 1.0f, 1.0f },
  };


  void ApplyBossPowerUpDesc( DefaultPowerUpDesc* dest, MonsNo monsno );
  void ApplyBossPowerUpDesc( DefaultPowerUpDesc* dest, const BossPowerUpDesc& desc );

  /**
   * @brief ボスの能力アップ設定を適用する
   * @param[out] dest    適用対象の能力アップ設定
   * @param      monsno  適用対象ポケモンのモンスターナンバー
   */
  void ApplyBossPowerUpDesc( DefaultPowerUpDesc* dest, MonsNo monsno )
  {
    for( u32 i=0; i<GFL_NELEMS(BOSS_POWERUP_TABLE); ++i )
    {
      if( BOSS_POWERUP_TABLE[i].monsno == monsno )
      {
        ApplyBossPowerUpDesc( dest, BOSS_POWERUP_TABLE[i] );
        break;
      }
    }
  }

  /**
   * @brief ボスの能力アップ設定を適用する
   * @param[out] dest  適用対象の能力アップ設定
   * @param      desc  適用対象のボス用能力アップ設定
   */
  void ApplyBossPowerUpDesc( DefaultPowerUpDesc* dest, const BossPowerUpDesc& desc )
  {
    DEFAULT_POWERUP_DESC_Clear( dest );

    dest->reason = DEFAULT_POWERUP_REASON_BOSS;
    dest->aura_color.Set( desc.auraColor_r, desc.auraColor_g, desc.auraColor_b );

    switch( desc.rankType )
    {
    case pml::wazadata::RANKEFF_ATTACK:     dest->rankUp_Attack    = desc.rankValue;  break;
    case pml::wazadata::RANKEFF_DEFENCE:    dest->rankUp_Defense   = desc.rankValue;  break;
    case pml::wazadata::RANKEFF_SP_ATTACK:  dest->rankUp_SpAttack  = desc.rankValue;  break;
    case pml::wazadata::RANKEFF_SP_DEFENCE: dest->rankUp_SpDefense = desc.rankValue;  break;
    case pml::wazadata::RANKEFF_AGILITY:    dest->rankUp_Agility   = desc.rankValue;  break;
    }
  }

}
 

//-----------------------------------------------------------------------------
/**
 * @brief ボス戦用のデフォルト能力アップを適用する
 * @param[out] setupParam  適用対象のセットアップパラメータ
 */
//-----------------------------------------------------------------------------
void BossBattle::ApplyDefaultPowerUp( BATTLE_SETUP_PARAM* setupParam )
{
  static const u8 TARGET_CLIENT_ID = BTL_CLIENT_ENEMY1;

  const PokeParty* party = setupParam->party[ TARGET_CLIENT_ID ];
  const u8 memberNum = party->GetMemberCount();

  for( u32 memberIndex=0; memberIndex<memberNum; ++memberIndex )
  {
    DefaultPowerUpDesc* powerUpDesc = &( setupParam->partyParam[ TARGET_CLIENT_ID ].defaultPowerUpDesc[ memberIndex ] );
    const pml::pokepara::PokemonParam* poke = party->GetMemberPointerConst( memberIndex );
    const MonsNo monsno = poke->GetMonsNo();
    ApplyBossPowerUpDesc( powerUpDesc, monsno );
  }
}



GFL_NAMESPACE_END( btl )