﻿#if PM_DEBUG
//=============================================================================
/**
 * @file    DebugBattleCommon.h
 * @brief   デバッグバトルProc用共通シンボル定義など
 * @author  obata_toshihiro
 * @data    2015.02.25
 * @note    sango_projectから移植
 */
//=============================================================================
#if !defined( DEBUG_BATTLE_COMMON_H_INCLUDED )
#define DEBUG_BATTLE_COMMON_H_INCLUDED

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(battle)


/**
 *  バトルモード設定
 */
enum BtlMode {
  SINGLE_WILD,
  SINGLE_TRAINER,
  SINGLE_COMM,
  DOUBLE_TRAINER,
  DOUBLE_TRAINER_TAG,
  DOUBLE_TRAINER_MULTI,
  DOUBLE_COMM,
  DOUBLE_COMM_MULTI,
  DOUBLE_COMM_AI_MULTI,
  ROYAL_TRAINER,
  ROYAL_COMM,

  BTLMODE_MAX,
};

enum RecMode {
  REC_NONE,
  REC_REC,
  REC_PLAY,
  REC_BTL0,
  REC_BTL1,
  REC_MODE_MAX,
};

enum CommMode {
  COMM_PARENT,
  COMM_CHILD_1,
  COMM_CHILD_2,
  COMM_CHILD_3,
  COMM_MODE_MAX,
};

enum MsgSpeed {
  MSG_SLOW,
  MSG_NORMAL,
  MSG_FAST,
  MSG_VERY_FAST,
  MSG_SPEED_MAX,
};

enum ChangeRule {
  CHANGE_IREKAE,
  CHANGE_KATINUKI,
  CHANGERULE_MAX,
};



/**
  * @brief ポケモンパラメータのID
  *
  * @note ITEM_P1_1 ～ ITEM_POKE_REINFORCE_4　の並びと一致する必要があります。
  */
 enum PokeParamID
 {
   POKEPARAM_PARTY_1_POKE_1,
   POKEPARAM_PARTY_1_POKE_2,
   POKEPARAM_PARTY_1_POKE_3,
   POKEPARAM_PARTY_1_POKE_4,
   POKEPARAM_PARTY_1_POKE_5,
   POKEPARAM_PARTY_1_POKE_6,
   POKEPARAM_PARTY_2_POKE_1,
   POKEPARAM_PARTY_2_POKE_2,
   POKEPARAM_PARTY_2_POKE_3,
   POKEPARAM_PARTY_2_POKE_4,
   POKEPARAM_PARTY_2_POKE_5,
   POKEPARAM_PARTY_2_POKE_6,
   POKEPARAM_PARTY_3_POKE_1,
   POKEPARAM_PARTY_3_POKE_2,
   POKEPARAM_PARTY_3_POKE_3,
   POKEPARAM_PARTY_3_POKE_4,
   POKEPARAM_PARTY_3_POKE_5,
   POKEPARAM_PARTY_3_POKE_6,
   POKEPARAM_PARTY_4_POKE_1,
   POKEPARAM_PARTY_4_POKE_2,
   POKEPARAM_PARTY_4_POKE_3,
   POKEPARAM_PARTY_4_POKE_4,
   POKEPARAM_PARTY_4_POKE_5,
   POKEPARAM_PARTY_4_POKE_6,
   POKEPARAM_INTRUDE,
   POKEPARAM_REINFORCE_1,
   POKEPARAM_REINFORCE_2,
   POKEPARAM_REINFORCE_3,
   POKEPARAM_REINFORCE_4,
   POKEPARAM_NUM,
 };




/**
  *  選択アイテムID
  */
enum DBItemID {
  ITEM_P1_1,
  ITEM_P1_2,
  ITEM_P1_3,
  ITEM_P1_4,
  ITEM_P1_5,
  ITEM_P1_6,
  ITEM_P2_1,
  ITEM_P2_2,
  ITEM_P2_3,
  ITEM_P2_4,
  ITEM_P2_5,
  ITEM_P2_6,
  ITEM_P3_1,
  ITEM_P3_2,
  ITEM_P3_3,
  ITEM_P3_4,
  ITEM_P3_5,
  ITEM_P3_6,
  ITEM_P4_1,
  ITEM_P4_2,
  ITEM_P4_3,
  ITEM_P4_4,
  ITEM_P4_5,
  ITEM_P4_6,
  ITEM_POKE_INTRUDE,
  ITEM_POKE_REINFORCE_1,
  ITEM_POKE_REINFORCE_2,
  ITEM_POKE_REINFORCE_3,
  ITEM_POKE_REINFORCE_4,
  ITEM_BTLMODE,
  ITEM_MSGSPEED,
  ITEM_CHANGERULE,
  ITEM_STAMP_COUNT,
  ITEM_COMMMODE,
  ITEM_EFFENABLE,
  ITEM_BTLHOUSE_ENABLE,
  ITEM_RECMODE,
  ITEM_RECIDX,
  ITEM_LOAD,
  ITEM_SAVE,
  ITEM_SAVE_SLOT,
  ITEM_TIME_GAME_M,
  ITEM_TIME_GAME_S,
  ITEM_TIME_CLIENT_M,
  ITEM_TIME_CLIENT_S,
  ITEM_TIME_COMMAND,
  ITEM_BACKGRND_NEAR,
  ITEM_BACKGRND_FAR,
  ITEM_BGM,
  ITEM_REINFORCE_COND_1,
  ITEM_REINFORCE_COND_2,
  ITEM_REINFORCE_COND_3,
  ITEM_REINFORCE_COND_4,
  ITEM_REINFORCE_COUNT_1,
  ITEM_REINFORCE_COUNT_2,
  ITEM_REINFORCE_COUNT_3,
  ITEM_REINFORCE_COUNT_4,
  ITEM_SW_MUST_ADD,
  ITEM_SW_MUST_TOK,
  ITEM_SW_MUST_ITEM,
  ITEM_SW_MUST_CRITICAL,
  ITEM_SW_MUST_CRITICAL_CAPTURE,
  ITEM_SW_HP_EVER,
  ITEM_SW_PP_EVER,
  ITEM_SW_HIT_100,
  ITEM_SW_DAMAGE_RAND_DISABLE,
  ITEM_SW_STARTEFF_SKIP,
  ITEM_SW_AI_CTRL,
  ITEM_SW_DISABLE_CRITICAL,
  ITEM_SW_INTRUDE_FORCE_CALL,
  ITEM_SW_INTRUDE_FORCE_APPEAR,
  ITEM_AI_BASE,
  ITEM_AI_STRONG,
  ITEM_AI_EXPERT,
  ITEM_AI_DOUBLE,
  ITEM_AI_ALLOWANCE,
  ITEM_AI_ROYAL,
  ITEM_AI_INTRUDE,
  ITEM_AI_ITEM,
  ITEM_AI_POKE_CHANGE,
  ITEM_DEFAULT_WEATHER,
  ITEM_SW_SKY_BTL,
  ITEM_SW_SAKASA_BTL,
  ITEM_SW_MUST_CAPTURE,
  ITEM_SW_GAKUSYU,
  ITEM_SW_MEGARING_OFF,
  ITEM_SW_ZENRYOKURING_OFF,
  ITEM_SW_ZUKAN_DONT_CLEAR,
  ITEM_SW_SAVE_DONT_CLEAR,
  ITEM_SW_RESOURCE_HIO,
  ITEM_SW_SIREN_BATTLE,
  ITEM_SW_VS_NUSI,
  ITEM_SW_INTRUDE_ENABLE,
  ITEM_SW_REINFORCE_ENABLE,
  ITEM_SW_VS_ULTRA_BEAST,
  ITEM_SW_VS_BOSS,
  ITEM_SW_FUNC_WAZA_CHECK_MODE,
  ITEM_FUNC_RESET_LANG,
  ITEM_FUNC_BTLFES_FIELD,
  ITEM_FUNC_BTLFES_FLAG,


//    ITEM_BTL_OBJ,
//    ITEM_WEATHER,

  ITEM_MAX,

  //
  ITEM_BITFLAG_BEGIN = ITEM_SW_MUST_ADD,
  ITEM_BITFLAG_END   = ITEM_SW_FUNC_WAZA_CHECK_MODE,
  ITEM_BITFLAG_COUNT = (ITEM_BITFLAG_END - ITEM_BITFLAG_BEGIN) + 1,
};

/**
  *  選択アイテムタイプ
  */
enum DebugBtlItemType {
  DBITEM_POKEMON,
  DBITEM_STR,
  DBITEM_NUM,
  DBITEM_LABEL,
};


/**
  *  定数群
  */
enum {
  PARTY_MAX = 4,
  PARTY_MEMBER_MAX = 6,
};


GFL_NAMESPACE_END(battle)
GFL_NAMESPACE_END(debug)


#endif // DEBUG_BATTLE_COMMON_H_INCLUDED
#endif // PM_DEBUG