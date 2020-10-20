//============================================================================================
/**
 * @file		ItemRecover.cpp
 * @brief		アイテム使用時のポケモン回復処理
 * @author	Hiroyuki Nakamura
 * @date		11.11.19
 */
//============================================================================================
//#include <gflib.h>
//#include <item/item.h>
#include "PokeTool/include/ItemRecover.h"
#include "PokeTool/include/NatsukiManager.h"


GFL_NAMESPACE_BEGIN(PokeTool)

//============================================================================================
//	定数定義
//============================================================================================
static const u32 TEMP_BUFLEN = 10;	// 計算用ワーク数

static const u32 RCV_25PER = 253;		// 25%回復
static const u32 RCV_HALF = 254;		// 半回復
static const u32 RCV_ALL = 255;			// 全回復

static const u32 PRMEXP_ONE_MAX = pml::MAX_EFFORT_POWER;		    // 努力値の最大値
static const u32 PRMEXP_MAX =     pml::MAX_TOTAL_EFFORT_POWER;  // 努力値の合計の最大値
static const u32 ITEM_PRMEXP_MAX = 100;		// 道具でアップする努力値の最大値

static const u32 PP_UP_WAZAMAX_MIN = 5;		// PPアップで上げられる技の最大PPの最小値


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static bool PP_RcvCheck( const pml::pokepara::PokemonParam * pp, u32 pos );
static bool PP_Recover( pml::pokepara::PokemonParam * pp, u32 pos, u32 rcv );
static bool PP_Up( pml::pokepara::PokemonParam * pp, u32 pos, u32 cnt );
static void HP_Recover( pml::pokepara::PokemonParam * pp, u32 hp, u32 max, u32 rcv );
static s32 PrmExp_Up( s32 exp, s32 other, s32 up, s32 limit );
static bool Friend_UpCheck( const pml::pokepara::PokemonParam * pp, item::ITEM_DATA * item );
//static bool Friend_Up( pml::pokepara::PokemonParam * pp, s32 now, s32 prm, u16 itemNo , u16 place, gfl2::heap::HeapBase * heap );

static bool RecoverCheck( const pml::pokepara::PokemonParam * pp, item::ITEM_DATA * dat, u16 pos );
static bool Recover( pml::pokepara::PokemonParam * pp, item::ITEM_DATA * dat, u16 pos, u16 place );


//--------------------------------------------------------------------------------------------
/**
 * @brief		道具使用チェック処理 ( PokemonParam )
 *
 * @param		pp		回復するポケモンのデータ
 * @param		item	使用するアイテム
 * @param		pos		回復位置（技位置など）
 * @param		heap	ヒープ
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_RCV_RecoverCheck( const pml::pokepara::PokemonParam * pp, u16 item, u16 pos, gfl2::heap::HeapBase * heap )
{
  item::ITEM_DATA * dat = GFL_NEW_LOW( heap ) item::ITEM_DATA( item, heap );
  bool result = RecoverCheck( pp, dat, pos );
  GFL_DELETE dat;
  return result;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		道具使用チェック処理 ( PokeParty )
 *
 * @param		party		回復するポケモンのデータ
 * @param		item		使用するアイテム
 * @param		pos1		ポケモンデータの中の位置
 * @param		pos2		回復位置（技位置など）
 * @param		heap		ヒープ
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_RCV_PokeParty_RecoverCheck( pml::PokeParty * party, u16 item, u8 pos1, u8 pos2, gfl2::heap::HeapBase * heap )
{
  return ITEM_RCV_RecoverCheck( party->GetMemberPointerConst(pos1), item, pos2, heap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   道具使用チェック処理 ( PokemonParam/ITEM_MANAGER )
 *
 * @param   pp        回復するポケモンのデータ
 * @param   item      使用するアイテム
 * @param   pos       回復位置（技位置など）
 * @param   manager   アイテムマネージャー
 *
 * @retval  "true = 使用可能"
 * @retval  "false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_RCV_RecoverCheck( const pml::pokepara::PokemonParam * pp, u16 item, u16 pos, itemman::ITEM_MANAGER * manager )
{
  item::ITEM_DATA dat;
  manager->GetData( item, dat );
  return RecoverCheck( pp, &dat, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   道具使用チェック処理 ( PokeParty/ITEM_MANAGER )
 *
 * @param   party     回復するポケモンのデータ
 * @param   item      使用するアイテム
 * @param   pos1      ポケモンデータの中の位置
 * @param   pos2      回復位置（技位置など）
 * @param   manager   アイテムマネージャー
 *
 * @retval  "true = 使用可能"
 * @retval  "false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_RCV_PokeParty_RecoverCheck( pml::PokeParty * party, u16 item, u8 pos1, u8 pos2, itemman::ITEM_MANAGER * manager )
{
  return ITEM_RCV_RecoverCheck( party->GetMemberPointerConst(pos1), item, pos2, manager );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   道具使用チェック処理
 *
 * @param   pp    回復するポケモンのデータ
 * @param   dat   アイテムデータ
 * @param   pos   回復位置（技位置など）
 *
 * @retval  "true = 使用可能"
 * @retval  "false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
static bool RecoverCheck( const pml::pokepara::PokemonParam * pp, item::ITEM_DATA * dat, u16 pos )
{
  s32 tmp[TEMP_BUFLEN];
  u32 tmp_u32[1];

  // ワークタイプをチェック
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_WORK_TYPE ) != item::ITEM_DATA::WKTYPE_POKEUSE ){
		return false;
	}

	tmp[0] = pp->GetSick();

	// 眠り回復
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_SLEEP_RCV ) != 0 ){
		if( ( tmp[0] == pml::pokepara::SICK_NEMURI ) != 0 ){
			return true;
		}
	}
	// 毒回復
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_POISON_RCV ) != 0 ){
		if( ( tmp[0] == pml::pokepara::SICK_DOKU ) != 0 ){
			return true;
		}
	}
	// 火傷回復
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_BURN_RCV ) != 0 ){
		if( ( tmp[0] == pml::pokepara::SICK_YAKEDO ) != 0 ){
			return true;
		}
	}
	// 氷回復
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_ICE_RCV ) != 0 ){
		if( ( tmp[0] == pml::pokepara::SICK_KOORI ) != 0 ){
			return true;
		}
	}
	// 麻痺回復
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_PARALYZE_RCV ) != 0 ){
		if( ( tmp[0] == pml::pokepara::SICK_MAHI ) != 0 ){
			return true;
		}
	}

	tmp_u32[0] = pp->GetHp();

	// 瀕死回復 or 全員瀕死回復
	if( ( dat->GetParam(item::ITEM_DATA::PRM_ID_DEATH_RCV) != 0 || dat->GetParam(item::ITEM_DATA::PRM_ID_ALL_DEATH_RCV) != 0 ) &&
			dat->GetParam( item::ITEM_DATA::PRM_ID_LV_UP ) == 0 ){
		if( tmp_u32[0] == 0 ){
			return true;
		}
	// HP回復
	}else if( dat->GetParam( item::ITEM_DATA::PRM_ID_HP_RCV ) != 0 ){
		if( tmp_u32[0] != 0 && tmp_u32[0] < pp->GetMaxHp() ){
			return true;
		}
	}

	// レベルアップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_LV_UP ) != 0 ){
		if( pp->GetLevel() < pml::MAX_POKE_LEVEL ){
			return true;
		}
	}

	//進化チェック(でも今回pokeListで処理してます
/*
  // 進化
  if( ITEM_GetBufParam( dat, ITEM_PRM_EVOLUTION ) != 0 ){
    if( PokeShinkaCheck( NULL, pp, ITEM_SHINKA_CHECK, item ,NULL) != 0 ){
      GFL_HEAP_FreeMemory( dat );
      return true;
    }
  }
*/

	// PPアップ or PPアップ（３段階）
	if( dat->GetParam(item::ITEM_DATA::PRM_ID_PP_UP) != 0 || dat->GetParam(item::ITEM_DATA::PRM_ID_PP_3UP) != 0 ){
		if( pp->GetWazaPPUpCount(pos) < pml::MAX_WAZAPP_UPCOUNT && pp->GetWazaMaxPP(pos) >= PP_UP_WAZAMAX_MIN ){
			return true;
		}
	}
	// PP回復
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_PP_RCV ) != 0 ){
		if( PP_RcvCheck( pp, pos ) == true ){
			return true;
		}
	}
	// PP回復（全ての技）
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_ALL_PP_RCV ) != 0 ){
		for( tmp[0]=0; tmp[0]<pml::MAX_WAZA_NUM; tmp[0]++ ){
			if( PP_RcvCheck( pp, tmp[0] ) == true ){
				return true;
			}
		}
	}

	tmp[0] = pp->GetEffortPower( pml::pokepara::POWER_HP );
  tmp[1] = pp->GetEffortPower( pml::pokepara::POWER_ATK );
  tmp[2] = pp->GetEffortPower( pml::pokepara::POWER_DEF );
  tmp[3] = pp->GetEffortPower( pml::pokepara::POWER_AGI );
  tmp[4] = pp->GetEffortPower( pml::pokepara::POWER_SPATK );
  tmp[5] = pp->GetEffortPower( pml::pokepara::POWER_SPDEF );
	tmp[7] = dat->GetParam( item::ITEM_DATA::PRM_ID_EXP_LIMIT_FLAG );
	tmp[9] = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5];

	// HP努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_HP_EXP ) != 0 ){
		tmp[6] = dat->GetParam( item::ITEM_DATA::PRM_ID_HP_EXP_POINT );
		if( tmp[6] > 0 && pp->GetMonsNo() != MONSNO_NUKENIN ){
			if( !( tmp[0] >= ITEM_PRMEXP_MAX && tmp[7] == 0 ) && tmp[0] < PRMEXP_ONE_MAX && tmp[9] < PRMEXP_MAX ){
				return true;
			}
		}else if( tmp[6] < 0 ){
			if( tmp[0] > 0 ){
				return true;
			}
			if( Friend_UpCheck( pp, dat ) == true ){
				return true;
			}
		}
	}
	// 攻撃努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_POWER_EXP ) != 0 ){
		tmp[6] = dat->GetParam( item::ITEM_DATA::PRM_ID_POWER_EXP_POINT );
		if( tmp[6] > 0 ){
			if( !( tmp[1] >= ITEM_PRMEXP_MAX && tmp[7] == 0 ) && tmp[1] < PRMEXP_ONE_MAX && tmp[9] < PRMEXP_MAX ){
				return true;
			}
		}else if( tmp[6] < 0 ){
			if( tmp[1] > 0 ){
				return true;
			}
			if( Friend_UpCheck( pp, dat ) == true ){
				return true;
			}
		}
	}
	// 防御努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_DEFENCE_EXP ) != 0 ){
		tmp[6] = dat->GetParam( item::ITEM_DATA::PRM_ID_DEFENCE_EXP_POINT );
		if( tmp[6] > 0 ){
			if( !( tmp[2] >= ITEM_PRMEXP_MAX && tmp[7] == 0 ) && tmp[2] < PRMEXP_ONE_MAX && tmp[9] < PRMEXP_MAX ){
				return true;
			}
		}else if( tmp[6] < 0 ){
			if( tmp[2] > 0 ){
				return true;
			}
			if( Friend_UpCheck( pp, dat ) == true ){
				return true;
			}
		}
	}
	// 素早さ努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_AGILITY_EXP ) != 0 ){
		tmp[6] = dat->GetParam( item::ITEM_DATA::PRM_ID_AGILITY_EXP_POINT );
		if( tmp[6] > 0 ){
			if( !( tmp[3] >= ITEM_PRMEXP_MAX && tmp[7] == 0 ) && tmp[3] < PRMEXP_ONE_MAX && tmp[9] < PRMEXP_MAX ){
				return true;
			}
		}else if( tmp[6] < 0 ){
			if( tmp[3] > 0 ){
				return true;
			}
			if( Friend_UpCheck( pp, dat ) == true ){
				return true;
			}
		}
	}
	// 特攻努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_SP_ATTACK_EXP ) != 0 ){
		tmp[6] = dat->GetParam( item::ITEM_DATA::PRM_ID_SP_ATTACK_EXP_POINT );
		if( tmp[6] > 0 ){
			if( !( tmp[4] >= ITEM_PRMEXP_MAX && tmp[7] == 0 ) && tmp[4] < PRMEXP_ONE_MAX && tmp[9] < PRMEXP_MAX ){
				return true;
			}
		}else if( tmp[6] < 0 ){
			if( tmp[4] > 0 ){
				return true;
			}
			if( Friend_UpCheck( pp, dat ) == true ){
				return true;
			}
		}
	}
	// 特防努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_SP_DEFENCE_EXP ) != 0 ){
		tmp[6] = dat->GetParam( item::ITEM_DATA::PRM_ID_SP_DEFENCE_EXP_POINT );
		if( tmp[6] > 0 ){
			if( !( tmp[5] >= ITEM_PRMEXP_MAX && tmp[7] == 0 ) && tmp[5] < PRMEXP_ONE_MAX && tmp[9] < PRMEXP_MAX ){
				return true;
			}
		}else if( tmp[6] < 0 ){
			if( tmp[5] > 0 ){
				return true;
			}
			if( Friend_UpCheck( pp, dat ) == true ){
				return true;
			}
		}
	}

  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		回復処理 ( PokemonParam )
 *
 * @param		pp		回復するポケモンのデータ
 * @param		item	使用するアイテム
 * @param		pos		回復位置（技位置など）
 * @param		place	場所
 * @param		heap	ヒープ
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_RCV_Recover(
      pml::pokepara::PokemonParam * pp, u16 item, u16 pos, u16 place, gfl2::heap::HeapBase * heap )
{
  item::ITEM_DATA * dat = GFL_NEW_LOW( heap ) item::ITEM_DATA( item, heap );
  bool result = Recover( pp, dat, pos, place );
  GFL_DELETE dat;
  return result;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		回復処理 ( PokeParty )
 *
 * @param		party		回復するポケモンのデータ
 * @param		item		使用するアイテム
 * @param		pos1		ポケモンデータの中の位置
 * @param		pos2		回復位置（技位置など）
 * @param		place		場所
 * @param		heap		ヒープ
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_RCV_PokeParty_Recover(
      pml::PokeParty * party, u16 item, u8 pos1, u8 pos2, u16 place, gfl2::heap::HeapBase * heap )
{
	return ITEM_RCV_Recover( party->GetMemberPointer(pos1), item, pos2, place, heap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   回復処理 ( PokemonParam/ITEM_MANAGER )
 *
 * @param   pp        回復するポケモンのデータ
 * @param   item      使用するアイテム
 * @param   pos       回復位置（技位置など）
 * @param   place     場所
 * @param   manager   アイテムマネージャー
 *
 * @retval  "true = 使用可能"
 * @retval  "false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_RCV_Recover(
      pml::pokepara::PokemonParam * pp, u16 item, u16 pos, u16 place, itemman::ITEM_MANAGER * manager )
{
  item::ITEM_DATA dat;
  manager->GetData( item, dat );
  return Recover( pp, &dat, pos, place );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   回復処理 ( PokeParty/ITEM_MANAGER )
 *
 * @param   party     回復するポケモンのデータ
 * @param   item      使用するアイテム
 * @param   pos1      ポケモンデータの中の位置
 * @param   pos2      回復位置（技位置など）
 * @param   place     場所
 * @param   manager   アイテムマネージャー
 *
 * @retval  "true = 使用可能"
 * @retval  "false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_RCV_PokeParty_Recover(
      pml::PokeParty * party, u16 item, u8 pos1, u8 pos2, u16 place, itemman::ITEM_MANAGER * manager )
{
	return ITEM_RCV_Recover( party->GetMemberPointer(pos1), item, pos2, place, manager );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   回復処理
 *
 * @param   pp        回復するポケモンのデータ
 * @param   item      使用するアイテム
 * @param   pos       回復位置（技位置など）
 * @param   place     場所
 * @param   apm       アシストパワーマネージャ
 *
 * @retval  "true = 使用可能"
 * @retval  "false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
static bool Recover( pml::pokepara::PokemonParam * pp, item::ITEM_DATA * dat, u16 pos, u16 place )
{
  s32 tmp[TEMP_BUFLEN];

  // ワークタイプをチェック
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_WORK_TYPE ) != item::ITEM_DATA::WKTYPE_POKEUSE ){
		return false;
	}

	bool	use_flg = false;
	bool	dat_flg = false;

  tmp[0] = pp->GetSick();
  tmp[1] = tmp[0];

	// 眠り回復
	if( tmp[0] == pml::pokepara::SICK_NEMURI && dat->GetParam( item::ITEM_DATA::PRM_ID_SLEEP_RCV ) != 0 ){
		tmp[1] = pml::pokepara::SICK_NULL;
		dat_flg = true;
	}
	// 毒回復
	if( tmp[0] == pml::pokepara::SICK_DOKU && dat->GetParam( item::ITEM_DATA::PRM_ID_POISON_RCV ) != 0 ){
		tmp[1] = pml::pokepara::SICK_NULL;
		dat_flg = true;
	}
	// 火傷回復
	if( tmp[0] == pml::pokepara::SICK_YAKEDO && dat->GetParam( item::ITEM_DATA::PRM_ID_BURN_RCV ) != 0 ){
		tmp[1] = pml::pokepara::SICK_NULL;
		dat_flg = true;
	}
	// 氷回復
	if( tmp[0] == pml::pokepara::SICK_KOORI && dat->GetParam( item::ITEM_DATA::PRM_ID_ICE_RCV ) != 0 ){
		tmp[1] = pml::pokepara::SICK_NULL;
		dat_flg = true;
	}
	// 麻痺回復
	if( tmp[0] == pml::pokepara::SICK_MAHI && dat->GetParam( item::ITEM_DATA::PRM_ID_PARALYZE_RCV ) != 0 ){
		tmp[1] = pml::pokepara::SICK_NULL;
		dat_flg = true;
	}
	if( tmp[0] != tmp[1] ){
		pp->RecoverSick();
		use_flg = true;
	}

	tmp[0] = pp->GetHp();
	tmp[1] = pp->GetMaxHp();

	// 瀕死回復 or 全員瀕死回復
	if( ( dat->GetParam(item::ITEM_DATA::PRM_ID_DEATH_RCV) != 0 || dat->GetParam(item::ITEM_DATA::PRM_ID_ALL_DEATH_RCV) != 0 ) &&
			dat->GetParam( item::ITEM_DATA::PRM_ID_LV_UP ) == 0 ){
		if( tmp[0] == 0 ){
			HP_Recover( pp, tmp[0], tmp[1], dat->GetParam(item::ITEM_DATA::PRM_ID_HP_RCV_POINT) );
			use_flg = true;
		}
		dat_flg = true;
	// HP回復
	}else if( dat->GetParam(item::ITEM_DATA::PRM_ID_HP_RCV ) != 0 ){
		if( tmp[0] != 0 && tmp[0] < tmp[1] ){
			HP_Recover( pp, tmp[0], tmp[1], dat->GetParam(item::ITEM_DATA::PRM_ID_HP_RCV_POINT) );
			use_flg = true;
		}
		dat_flg = true;
	}

	tmp[2] = pp->GetLevel();

	// レベルアップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_LV_UP ) != 0 ){
		if( tmp[2] < pml::MAX_POKE_LEVEL ){
			pp->LevelUp( 1 );
			use_flg = true;
		}
		dat_flg = true;
	}

	// 進化
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_EVOLUTION ) != 0 ){
		dat_flg = true;
	}

	// PPアップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_PP_UP ) != 0 ){
		if( PP_Up( pp, pos, 1 ) == true ){
			use_flg = true;
		}
		dat_flg = true;
	// PPアップ（３段階）
	}else if( dat->GetParam( item::ITEM_DATA::PRM_ID_PP_3UP ) != 0 ){
		if( PP_Up( pp, pos, pml::MAX_WAZAPP_UPCOUNT ) == true ){
			use_flg = true;
		}
		dat_flg = true;
	}

	// PP回復
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_PP_RCV ) != 0 ){
		if( PP_Recover( pp, pos, dat->GetParam(item::ITEM_DATA::PRM_ID_PP_RCV_POINT) ) == true ){
			use_flg = true;
		}
		dat_flg = true;
	// PP回復（全ての技）
	}else if( dat->GetParam( item::ITEM_DATA::PRM_ID_ALL_PP_RCV ) != 0 ){
		for( tmp[0]=0; tmp[0]<pml::MAX_WAZA_NUM; tmp[0]++ ){
			if( PP_Recover( pp, tmp[0], dat->GetParam(item::ITEM_DATA::PRM_ID_PP_RCV_POINT) ) == true ){
				use_flg = true;
			}
		}
		dat_flg = true;
	}

	tmp[0] = pp->GetEffortPower( pml::pokepara::POWER_HP );
  tmp[1] = pp->GetEffortPower( pml::pokepara::POWER_ATK );
  tmp[2] = pp->GetEffortPower( pml::pokepara::POWER_DEF );
  tmp[3] = pp->GetEffortPower( pml::pokepara::POWER_AGI );
  tmp[4] = pp->GetEffortPower( pml::pokepara::POWER_SPATK );
  tmp[5] = pp->GetEffortPower( pml::pokepara::POWER_SPDEF );
	tmp[8] = dat->GetParam( item::ITEM_DATA::PRM_ID_EXP_LIMIT_FLAG );

	// HP努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_HP_EXP ) != 0 ){
		tmp[7] = dat->GetParam( item::ITEM_DATA::PRM_ID_HP_EXP_POINT );
		// 「ヌケニン」だと努力値UPはさせない
/*	swanの修正だと、もし、努力値以外に影響があったら使えないことになってしまうので再修正
		if( tmp[7] > 0 && pp->GetMonsNo() == MONSNO_NUKENIN ){
			return false;
		}
		tmp[6] = PrmExp_Up( tmp[0], (tmp[1]+tmp[2]+tmp[3]+tmp[4]+tmp[5]), tmp[7], tmp[8] );
		if( tmp[6] != -1 ){
			tmp[0] = tmp[6];
			pp->ChangeEffortPower( pml::pokepara::POWER_HP, tmp[0] );
			use_flg = true;
		}
		if( tmp[7] > 0 ){ dat_flg = true; }
*/
		if( !( tmp[7] > 0 && pp->GetMonsNo() == MONSNO_NUKENIN ) ){
			tmp[6] = PrmExp_Up( tmp[0], (tmp[1]+tmp[2]+tmp[3]+tmp[4]+tmp[5]), tmp[7], tmp[8] );
			if( tmp[6] != -1 ){
				tmp[0] = tmp[6];
				pp->ChangeEffortPower( pml::pokepara::POWER_HP, tmp[0] );
				use_flg = true;
			}
			if( tmp[7] > 0 ){ dat_flg = true; }
		}
	}
	// 攻撃努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_POWER_EXP ) != 0 ){
		tmp[7] = dat->GetParam( item::ITEM_DATA::PRM_ID_POWER_EXP_POINT );
		tmp[6] = PrmExp_Up( tmp[1], (tmp[0]+tmp[2]+tmp[3]+tmp[4]+tmp[5]), tmp[7], tmp[8] );
		if( tmp[6] != -1 ){
			tmp[1] = tmp[6];
			pp->ChangeEffortPower( pml::pokepara::POWER_ATK, tmp[1] );
			use_flg = true;
		}
		if( tmp[7] > 0 ){ dat_flg = true; }
	}
	// 防御努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_DEFENCE_EXP ) != 0 ){
		tmp[7] = dat->GetParam( item::ITEM_DATA::PRM_ID_DEFENCE_EXP_POINT );
		tmp[6] = PrmExp_Up( tmp[2], (tmp[0]+tmp[1]+tmp[3]+tmp[4]+tmp[5]), tmp[7], tmp[8] );
		if( tmp[6] != -1 ){
			tmp[2] = tmp[6];
			pp->ChangeEffortPower( pml::pokepara::POWER_DEF, tmp[2] );
			use_flg = true;
		}
		if( tmp[7] > 0 ){ dat_flg = true; }
	}
	// 素早さ努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_AGILITY_EXP ) != 0 ){
		tmp[7] = dat->GetParam( item::ITEM_DATA::PRM_ID_AGILITY_EXP_POINT );
		tmp[6] = PrmExp_Up( tmp[3], (tmp[0]+tmp[1]+tmp[2]+tmp[4]+tmp[5]), tmp[7], tmp[8] );
		if( tmp[6] != -1 ){
			tmp[3] = tmp[6];
			pp->ChangeEffortPower( pml::pokepara::POWER_AGI, tmp[3] );
			use_flg = true;
		}
		if( tmp[7] > 0 ){ dat_flg = true; }
	}
	// 特攻努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_SP_ATTACK_EXP ) != 0 ){
		tmp[7] = dat->GetParam( item::ITEM_DATA::PRM_ID_SP_ATTACK_EXP_POINT );
		tmp[6] = PrmExp_Up( tmp[4], (tmp[0]+tmp[1]+tmp[2]+tmp[3]+tmp[5]), tmp[7], tmp[8] );
		if( tmp[6] != -1 ){
			tmp[4] = tmp[6];
			pp->ChangeEffortPower( pml::pokepara::POWER_SPATK, tmp[4] );
			use_flg = true;
		}
		if( tmp[7] > 0 ){ dat_flg = true; }
	}
	// 特防努力値アップ
	if( dat->GetParam( item::ITEM_DATA::PRM_ID_SP_DEFENCE_EXP ) != 0 ){
		tmp[7] = dat->GetParam( item::ITEM_DATA::PRM_ID_SP_DEFENCE_EXP_POINT );
		tmp[6] = PrmExp_Up( tmp[5], (tmp[0]+tmp[1]+tmp[2]+tmp[3]+tmp[4]), tmp[7], tmp[8] );
		if( tmp[6] != -1 ){
			tmp[5] = tmp[6];
			pp->ChangeEffortPower( pml::pokepara::POWER_SPDEF, tmp[5] );
			use_flg = true;
		}
		if( tmp[7] > 0 ){ dat_flg = true; }
	}

	if( use_flg == false && dat_flg == true ){
		return false;
	}

	NatsukiManager::CalcWhenUseItem( pp, place, dat );

	return use_flg;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		PP回復チェック
 *
 * @param		pp		回復するポケモンのデータ
 * @param		pos		回復位置
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
static bool PP_RcvCheck( const pml::pokepara::PokemonParam * pp, u32 pos )
{
	if( pos >= pp->GetWazaCount() ){
		return false;
	}
	if( pp->GetWazaPP(pos) >= pp->GetWazaMaxPP(pos) ){
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		PP回復
 *
 * @param		pp		回復するポケモンのデータ
 * @param		pos		回復位置（技位置など）
 * @param		rcv		回復値
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
static bool PP_Recover( pml::pokepara::PokemonParam * pp, u32 pos, u32 rcv )
{
	if( pos >= pp->GetWazaCount() ){
		return false;
	}
	if( pp->GetWazaPP(pos) >= pp->GetWazaMaxPP(pos) ){
		return false;
	}
	pp->RecoverWazaPP( pos, rcv );

	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * PPアップ
 *
 * @param pp    回復するポケモンのデータ
 * @param pos   回復位置（技位置など）
 * @param cnt   アップ値
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
static bool PP_Up( pml::pokepara::PokemonParam * pp, u32 pos, u32 cnt )
{
	u32	ppc = pp->GetWazaPPUpCount( pos );

	if( ppc == pml::MAX_WAZAPP_UPCOUNT ){
		return false;
	}

	u32	mpp = pp->GetWazaMaxPP( pos );

	if( mpp < PP_UP_WAZAMAX_MIN ){
		return false;
	}

	ppc += cnt;
	if( ppc > pml::MAX_WAZAPP_UPCOUNT ){
		ppc = pml::MAX_WAZAPP_UPCOUNT;
	}

	pp->SetWazaPPUpCount( pos, ppc );
	pp->RecoverWazaPP( pos, pp->GetWazaMaxPP(pos)-mpp );	// 増えた分を回復

	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * HP回復
 *
 * @param pp    回復するポケモンのデータ
 * @param hp    現在のHP
 * @param max   最大HP
 * @param rcv   回復値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void HP_Recover( pml::pokepara::PokemonParam * pp, u32 hp, u32 max, u32 rcv )
{
	if( max == 1 ){
		rcv = 1;
	}else if( rcv == RCV_ALL ){
		rcv = max;
	}else if( rcv == RCV_HALF ){
		rcv = max/2;
	}else if( rcv == RCV_25PER ){
		rcv = max/4;
	}
	pp->RecoverHp( rcv );
}

//--------------------------------------------------------------------------------------------
/**
 * 努力値Up計算
 *
 * @param exp   現在の努力値
 * @param other その他の努力値の合計
 * @param up    アップする値
 * @param limit 限界制御
 *
 * @retval  "-1 = アップしない"
 * @retval  "0 != 計算結果（セットする努力値）"
 */
//--------------------------------------------------------------------------------------------
static s32 PrmExp_Up( s32 exp, s32 other, s32 up, s32 limit )
{
	if( exp == 0 && up < 0 ){
		return -1;
	}
	if( exp == PRMEXP_ONE_MAX && up > 0 ){
		return -1;
	}
	if( exp >= ITEM_PRMEXP_MAX && limit == 0 && up > 0 ){
		return -1;
	}
	if( (exp+other) >= PRMEXP_MAX && up > 0 ){
		return -1;
	}

	exp += up;
	if( up > 0 && exp > ITEM_PRMEXP_MAX && limit == 0 ){
		exp = ITEM_PRMEXP_MAX;
	}else if( exp < 0 ){
		exp = 0;
	}
	if( (exp+other) > PRMEXP_MAX ){
		exp = PRMEXP_MAX - other;
	}

	return exp;
}

//--------------------------------------------------------------------------------------------
/**
 * なつき度が上がるか
 *
 * @param pp    ポケモンのデータ
 * @param prm   変更値
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 *
 * @li  下がる場合は無視
 */
//--------------------------------------------------------------------------------------------
static bool Friend_UpCheck( const pml::pokepara::PokemonParam * pp, item::ITEM_DATA * item )
{
	u32	now = pp->GetFamiliarity();

	if( now >= pml::MAX_FAMILIARITY ){
		return false;
	}

	// なつき度１
	if( now < 100 ){
		if( item->GetParam( item::ITEM_DATA::PRM_ID_FRIEND1 ) != 0 ){
			if( item->GetParam( item::ITEM_DATA::PRM_ID_FRIEND1_POINT ) > 0 ){
				return true;
			}
		}
		return false;
	// なつき度２
	}else if( now >= 100 && now < 200 ){
		if( item->GetParam( item::ITEM_DATA::PRM_ID_FRIEND2 ) != 0 ){
			if( item->GetParam( item::ITEM_DATA::PRM_ID_FRIEND2_POINT ) > 0 ){
				return true;
			}
		}
		return false;
	// なつき度３
	}else if( now >= 200 && now < 255 ){
		if( item->GetParam( item::ITEM_DATA::PRM_ID_FRIEND3 ) != 0 ){
			if( item->GetParam( item::ITEM_DATA::PRM_ID_FRIEND3_POINT ) > 0 ){
				return true;
			}
		}
		return false;
	}

	return false;
}

//--------------------------------------------------------------------------------------------
/**
 * なつき度Up
 *
 * @param pp    ポケモンのデータ
 * @param now   現在の値
 * @param prm   変更値
 * @param place 場所
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
/*
static bool Friend_Up( pml::pokepara::PokemonParam * pp, s32 now, s32 prm, u16 itemNo , u16 place, gfl2::heap::HeapBase * heap )
{
	if( now == pml::MAX_FAMILIARITY && prm > 0 ){
		return false;
	}
	if( now == 0 && prm < 0 ){
		return false;
	}
//	NATSUKI_CalcUseItem( pp , itemNo , place , heap );
	return true;
}
*/

GFL_NAMESPACE_END(PokeTool)
