//=============================================================================
/**
 * @file   battle_DefaultPowerUp.cpp
 * @date   2015/09/08 18:07:47
 * @author obata_toshihiro
 * @brief  「ぬし」や「パラレルビースト」など、最初から能力アップした状態についての定義
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <Battle/include/battle_DefaultPowerUp.h>



//-----------------------------------------------------------------------------
/**
 * @brief 能力アップの設定を、能力アップが無い状態にする
 * @param desc  操作対象のインスタンス
 */
//-----------------------------------------------------------------------------
void DEFAULT_POWERUP_DESC_Clear( DefaultPowerUpDesc* desc )
{
  desc->reason           = DEFAULT_POWERUP_REASON_NONE;
  desc->rankUp_Attack    = 0; 
  desc->rankUp_Defense   = 0;
  desc->rankUp_SpAttack  = 0;
  desc->rankUp_SpDefense = 0;
  desc->rankUp_Agility   = 0;
  desc->aura_color.Set(1.0f,1.0f,1.0f,1.0f);
}

//-----------------------------------------------------------------------------
/**
 * @brief 中身をコピーする
 * @param[out] dest  コピー先
 * @param      src   コピー元
 */
//-----------------------------------------------------------------------------
void DEFAULT_POWERUP_DESC_Copy( DefaultPowerUpDesc* dest, const DefaultPowerUpDesc& src )
{
  dest->reason           = src.reason;   
  dest->rankUp_Attack    = src.rankUp_Attack;  
  dest->rankUp_Defense   = src.rankUp_Defense; 
  dest->rankUp_SpAttack  = src.rankUp_SpAttack;
  dest->rankUp_SpDefense = src.rankUp_SpDefense;
  dest->rankUp_Agility   = src.rankUp_Agility;
  dest->aura_color       = src.aura_color;
}

//-----------------------------------------------------------------------------
/**
 * @brief 能力ランクが上昇するパラメータの数を取得する
 * @param desc  取得対象のデータ
 */
//-----------------------------------------------------------------------------
u32 DEFAULT_POWERUP_DESC_GetRankUpParamCount( const DefaultPowerUpDesc& desc )
{
  u32 count = 0;

  if( 0 < desc.rankUp_Attack ) {
    ++count;
  }

  if( 0 < desc.rankUp_Defense ) {
    ++count;
  }

  if( 0 < desc.rankUp_SpAttack ) {
    ++count;
  }

  if( 0 < desc.rankUp_SpDefense ) {
    ++count;
  }

  if( 0 < desc.rankUp_Agility ) {
    ++count;
  }

  return count;
}

//-----------------------------------------------------------------------------
/**
 * @brief 能力ランクの上昇値の最大値を取得する
 * @param desc  取得対象のデータ
 */
//-----------------------------------------------------------------------------
u8 DEFAULT_POWERUP_DESC_GetMaxRankUpValue( const DefaultPowerUpDesc& desc )
{
  u8 max = 0;

  if( max < desc.rankUp_Attack ) {
    max = desc.rankUp_Attack;
  }

  if( max < desc.rankUp_Defense ) {
    max = desc.rankUp_Defense;
  }

  if( max < desc.rankUp_SpAttack ) {
    max = desc.rankUp_SpAttack;
  }

  if( max < desc.rankUp_SpDefense ) {
    max = desc.rankUp_SpDefense;
  }

  if( max < desc.rankUp_Agility ) {
    max = desc.rankUp_Agility;
  }

  return max;
}