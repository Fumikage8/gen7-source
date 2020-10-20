#include "pml/include/item/item_util.h"


namespace item {

// 状態異常回復フラグ定義
static const u32 RCV_FLG_SLEEP = 0x01;		// 眠り回復
static const u32 RCV_FLG_POISON = 0x02;		// 毒回復
static const u32 RCV_FLG_BURN = 0x04;			// 火傷回復
static const u32 RCV_FLG_ICE = 0x08;			// 氷回復
static const u32 RCV_FLG_PARALYZE = 0x10;	// 麻痺回復
static const u32 RCV_FLG_PANIC = 0x20;		// 混乱回復
static const u32 RCV_FLG_ALL = 0x3f;			// 全快

//--------------------------------------------------------------------------------------------
/**
 * 回復タイプチェック
 *
 * @param item  アイテム番号
 *
 * @return  回復タイプ
 */
//--------------------------------------------------------------------------------------------
u32 ITEM_UTIL_GetHealingItemType( const item::ITEM_DATA * item )
{
  s32 prm;

  // ワークタイプをチェック
	if( item->GetParam( item::ITEM_DATA::PRM_ID_WORK_TYPE ) != item::ITEM_DATA::WKTYPE_POKEUSE ){
		return ITEM_TYPE_ETC;			// その他
	}
	// 全員瀕死回復
	if( item->GetParam( item::ITEM_DATA::PRM_ID_ALL_DEATH_RCV ) != 0 ){
		return ITEM_TYPE_ALLDETH_RCV;
	}
	// LvUp系
	if( item->GetParam( item::ITEM_DATA::PRM_ID_LV_UP ) != 0 ){
		return ITEM_TYPE_LV_UP;
	}
	// Status回復系
	prm = item->GetParam( item::ITEM_DATA::PRM_ID_SLEEP_RCV );							// 眠り
	prm += ( item->GetParam( item::ITEM_DATA::PRM_ID_POISON_RCV ) << 1 );		// 毒
	prm += ( item->GetParam( item::ITEM_DATA::PRM_ID_BURN_RCV ) << 2 );			// 火傷
	prm += ( item->GetParam( item::ITEM_DATA::PRM_ID_ICE_RCV ) << 3 );			// 氷
	prm += ( item->GetParam( item::ITEM_DATA::PRM_ID_PARALYZE_RCV ) << 4 );	// 麻痺
	prm += ( item->GetParam( item::ITEM_DATA::PRM_ID_PANIC_RCV ) << 5 );		// 混乱
	switch( prm ){
	case RCV_FLG_SLEEP:			// 眠り
		return ITEM_TYPE_NEMURI_RCV;
	case RCV_FLG_POISON:		// 毒
		return ITEM_TYPE_DOKU_RCV;
	case RCV_FLG_BURN:			// 火傷
		return ITEM_TYPE_YAKEDO_RCV;
	case RCV_FLG_ICE:				// 氷
		return ITEM_TYPE_KOORI_RCV;
	case RCV_FLG_PARALYZE:	// 麻痺
		return ITEM_TYPE_MAHI_RCV;
	case RCV_FLG_PANIC:			// 混乱
		return ITEM_TYPE_KONRAN_RCV;
	case RCV_FLG_ALL:				// 全快
		if( item->GetParam( item::ITEM_DATA::PRM_ID_HP_RCV ) != 0 ){
			return ITEM_TYPE_HP_RCV;  // HP回復 ( 瀕死も )
		}else{
			return ITEM_TYPE_ALL_ST_RCV;
		}
	}
	// メロメロ回復
	if( item->GetParam( item::ITEM_DATA::PRM_ID_MEROMERO_RCV ) != 0 ){
		return ITEM_TYPE_MEROMERO_RCV;
	}
	// HP回復
	if( item->GetParam( item::ITEM_DATA::PRM_ID_HP_RCV ) != 0 ){
		return ITEM_TYPE_HP_RCV;
	}
	// 瀕死回復
	if( item->GetParam( item::ITEM_DATA::PRM_ID_DEATH_RCV ) != 0){
		return ITEM_TYPE_DEATH_RCV;
	}
	// ドーピング系
	// HP努力値
	prm = item->GetParam( item::ITEM_DATA::PRM_ID_HP_EXP_POINT );
	if( prm > 0 ){
		return ITEM_TYPE_HP_UP;
	}
	if( prm < 0 ){
		return ITEM_TYPE_HP_DOWN;
	}
	// 攻撃努力値
	prm = item->GetParam( item::ITEM_DATA::PRM_ID_POWER_EXP_POINT );
	if( prm > 0 ){
		return ITEM_TYPE_ATC_UP;
	}
	if( prm < 0 ){
		return ITEM_TYPE_ATC_DOWN;
	}
	// 防御努力値
	prm = item->GetParam( item::ITEM_DATA::PRM_ID_DEFENCE_EXP_POINT );
	if( prm > 0 ){
		return ITEM_TYPE_DEF_UP;
	}
	if( prm < 0 ){
		return ITEM_TYPE_DEF_DOWN;
	}
	// 特攻努力値
	prm = item->GetParam( item::ITEM_DATA::PRM_ID_SP_ATTACK_EXP_POINT );
	if( prm > 0 ){
		return ITEM_TYPE_SPA_UP;
	}
	if( prm < 0 ){
		return ITEM_TYPE_SPA_DOWN;
	}
	// 素早さ努力値
	prm = item->GetParam( item::ITEM_DATA::PRM_ID_AGILITY_EXP_POINT );
	if( prm > 0 ){
		return ITEM_TYPE_AGI_UP;
	}
	if( prm < 0 ){
		return ITEM_TYPE_AGI_DOWN;
	}
	// 特防努力値
	prm = item->GetParam( item::ITEM_DATA::PRM_ID_SP_DEFENCE_EXP_POINT );
	if( prm > 0 ){
		return ITEM_TYPE_SPD_UP;
	}
	if( prm < 0 ){
		return ITEM_TYPE_SPD_DOWN;
	}
	// 進化系
	if( item->GetParam( item::ITEM_DATA::PRM_ID_EVOLUTION ) != 0 ){
		return ITEM_TYPE_EVO;
	}
	// ppUp系
	if( item->GetParam( item::ITEM_DATA::PRM_ID_PP_UP ) != 0 ){
		return ITEM_TYPE_PP_UP;
	}
	// pp3Up系
	if( item->GetParam( item::ITEM_DATA::PRM_ID_PP_3UP ) != 0 ){
		return ITEM_TYPE_PP_3UP;
	}
	// pp回復系
	if( item->GetParam( item::ITEM_DATA::PRM_ID_PP_RCV ) != 0 ||
			item->GetParam( item::ITEM_DATA::PRM_ID_ALL_PP_RCV ) != 0 ){
		return ITEM_TYPE_PP_RCV;
	}

	return ITEM_TYPE_ETC;
}

// 技選択が必要か？
bool ITEM_UTIL_IsNeedSelectSkill( const item::ITEM_DATA * item )
{
	u32	type = item::ITEM_UTIL_GetHealingItemType( item );

	switch( type ){
	case ITEM_TYPE_PP_UP:		// ppUp系
		return true;
	case ITEM_TYPE_PP_3UP:	// pp3Up系
		return true;
	case ITEM_TYPE_PP_RCV:	// pp回復系
		// 単体使用か？
		if( item->GetParam( item::ITEM_DATA::PRM_ID_PP_RCV ) != 0 ){
			return true;
		}
	}
	return false;
}

// 全体復活アイテムか？ と、言うか聖なる灰か？
bool ITEM_UTIL_IsDeathRecoverAllItem( const item::ITEM_DATA * item )
{
  if( item->GetParam( item::ITEM_DATA::PRM_ID_WORK_TYPE ) != item::ITEM_DATA::WKTYPE_POKEUSE )
  {
    return false;
  }
  if( item->GetParam( item::ITEM_DATA::PRM_ID_ALL_DEATH_RCV ) != 0 )
  {
    return true;
  }
  return false;
}


}	// item
