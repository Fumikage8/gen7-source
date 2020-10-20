//============================================================================================
/**
 * @file    item_util.h
 * @brief   アイテムデータ処理
 * @author  Hiroyuki Nakamura
 * @date    11.05.23
 *
 * class    item::ITEM_DATA::
 */
//============================================================================================
#ifndef __PML_ITEM_UTIL_H__
#define __PML_ITEM_UTIL_H__

#pragma	once

#include "item.h"


namespace item {

/// 回復アイテムの種類
enum {
//	ITEM_TYPE_BTL_ST_UP = 0,	// 戦闘用ステータスアップ系
	ITEM_TYPE_ALLDETH_RCV = 0,	///< 全員瀕死回復
	ITEM_TYPE_LV_UP,						///< LvUp系
	ITEM_TYPE_NEMURI_RCV,				///< 眠り回復
	ITEM_TYPE_DOKU_RCV,					///< 毒回復
	ITEM_TYPE_YAKEDO_RCV,				///< 火傷回復
	ITEM_TYPE_KOORI_RCV,				///< 氷回復
	ITEM_TYPE_MAHI_RCV,					///< 麻痺回復
	ITEM_TYPE_KONRAN_RCV,				///< 混乱回復
	ITEM_TYPE_ALL_ST_RCV,				///< 全快
	ITEM_TYPE_MEROMERO_RCV,			///< メロメロ回復
	ITEM_TYPE_HP_RCV,						///< HP回復
	ITEM_TYPE_DEATH_RCV,				///< 瀕死回復

	//メッセージのステータスと順番依存！！
	ITEM_TYPE_HP_UP,						///< HP努力値UP
	ITEM_TYPE_ATC_UP,						///< 攻撃努力値UP
	ITEM_TYPE_DEF_UP,						///< 防御努力値UP
	ITEM_TYPE_SPA_UP,						///< 特攻努力値UP
	ITEM_TYPE_SPD_UP,						///< 特防努力値UP
	ITEM_TYPE_AGI_UP,						///< 素早さ努力値UP

	//メッセージのステータスと順番依存！！
	ITEM_TYPE_HP_DOWN,					///< HP努力値DOWN
	ITEM_TYPE_ATC_DOWN,					///< 攻撃努力値DOWN
	ITEM_TYPE_DEF_DOWN,					///< 防御努力値DOWN
	ITEM_TYPE_SPA_DOWN,					///< 特攻努力値DOWN
	ITEM_TYPE_SPD_DOWN,					///< 特防努力値DOWN
	ITEM_TYPE_AGI_DOWN,					///< 素早さ努力値DOWN

	ITEM_TYPE_EVO,							///< 進化系
	ITEM_TYPE_PP_UP,						///< ppUp系
	ITEM_TYPE_PP_3UP,						///< pp3Up系
	ITEM_TYPE_PP_RCV,						///< pp回復系

	ITEM_TYPE_ETC,							///< その他
};

extern u32 ITEM_UTIL_GetHealingItemType( const item::ITEM_DATA * item );
extern bool ITEM_UTIL_IsNeedSelectSkill( const item::ITEM_DATA * item );
extern bool ITEM_UTIL_IsDeathRecoverAllItem( const item::ITEM_DATA * item );

}	// item

#endif

