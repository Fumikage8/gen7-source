//=============================================================================
/**
 * @file   battle_DefaultPowerUp.h
 * @date   2015/09/08 18:07:47
 * @author obata_toshihiro
 * @brief  「ぬし」や「パラレルビースト」など、最初から能力アップした状態についての定義
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BATTLE_DEFAULTPOWERUP_H_INCLUDED
#define BATTLE_DEFAULTPOWERUP_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>

/**
 * @brief 能力アップの理由
 */
enum DefaultPowerUpReason
{
  DEFAULT_POWERUP_REASON_NONE,         // 能力アップなし
  DEFAULT_POWERUP_REASON_NUSI,         // "ぬし"　だから
  DEFAULT_POWERUP_REASON_ULTRA_BEAST,  // "ウルトラビースト" だから
  DEFAULT_POWERUP_REASON_BOSS,         // "代表ビースト" だから
  DEFAULT_POWERUP_REASON_NUM,
};


/**
 * @brief 能力アップの内容
 */
struct DefaultPowerUpDesc
{
  DefaultPowerUpReason reason;             // 能力アップの理由
  u8                   rankUp_Attack;      // 能力ランク上昇量( こうげき )
  u8                   rankUp_Defense;     // 能力ランク上昇量( ぼうぎょ )
  u8                   rankUp_SpAttack;    // 能力ランク上昇量( とくこう )
  u8                   rankUp_SpDefense;   // 能力ランク上昇量( とくぼう )
  u8                   rankUp_Agility;     // 能力ランク上昇量( すばやさ )
  gfl2::math::Vector4  aura_color;         // オーラの色
};


/**
 * @brief 能力アップの設定を、能力アップが無い状態にする
 * @param desc  操作対象のインスタンス
 */
extern void DEFAULT_POWERUP_DESC_Clear( DefaultPowerUpDesc* desc );

/**
 * @brief 中身をコピーする
 * @param[out] dest  コピー先
 * @param      src   コピー元
 */
extern void DEFAULT_POWERUP_DESC_Copy( DefaultPowerUpDesc* dest, const DefaultPowerUpDesc& src );

/**
 * @brief 能力ランクが上昇するパラメータの数を取得する
 * @param desc  取得対象のデータ
 */
extern u32 DEFAULT_POWERUP_DESC_GetRankUpParamCount( const DefaultPowerUpDesc& desc );

/**
 * @brief 能力ランクの上昇値の最大値を取得する
 * @param desc  取得対象のデータ
 */
extern u8 DEFAULT_POWERUP_DESC_GetMaxRankUpValue( const DefaultPowerUpDesc& desc );



#endif // BATTLE_DEFAULTPOWERUP_H_INCLUDED