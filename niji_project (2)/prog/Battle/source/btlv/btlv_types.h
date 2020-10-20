#if !defined( __BTLV_TYPES_H__ )
#define __BTLV_TYPES_H__
//======================================================================
/**
 * @file	btlv_types.h
 * @brief	バトル描画 タイプ定義
 * @author	ariizumi
 * @data	10/11/10
 */
//======================================================================
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>

GFL_NAMESPACE_BEGIN(btl)

#if PM_DEBUG
#define BTLV_DEBUG (1)
#else
#define BTLV_DEBUG (0)
#endif

class BattleViewSystem;
class BtlvUi;

static const int BTLV_VIEWPOS_MAX = 6;
static const int POKE_MODEL_NUM = 4;
static const int TRAINER_MODEL_NUM = 4;
static const int OBON_MODEL_NUM = 6;
static const int LALGE_OBON_MODEL_NUM = 2;
static const int BREAK_MODEL_NUM = 2;

typedef enum
{
  BCR_SEL_NONE = 0, //未選択
  BCR_SEL_DECIDE,   //汎用
  BCR_SEL_CANCEL,

  BCR_SEL_SKILL,    //行動
  BCR_SEL_ITEM,
  BCR_SEL_RUN,
  BCR_SEL_POKE,
  BCR_SEL_MOVE,   // 移動( トリプルバトル専用 )
  BCR_SEL_ROT_L,  // 左回転( ローテーションバトル用 )
  BCR_SEL_ROT_R,  // 右回転( ローテーションバトル用 )

  BCR_SEL_IDX1,     //技・ポケ
  BCR_SEL_IDX2,
  BCR_SEL_IDX3,
  BCR_SEL_IDX4,
  BCR_SEL_IDX5,
  BCR_SEL_IDX6,

  BCR_SEL_YES,  //2択用
  BCR_SEL_NO,

  BCR_SEL_WARUAGAKI,  // わるあがき用 BTS[388]
  
  BTLV_COMMAND_RESULT_MAX
}BTLV_COMMAND_RESULT;

//タイマーの種類
typedef enum
{
  BTLV_TIMER_TYPE_GAME_TIME = 0,  // 試合時間
  BTLV_TIMER_TYPE_CLIENT_TIME,    // 持ち時間
  BTLV_TIMER_TYPE_COMMAND_TIME,   // コマンド入力時間
  BTLV_TIMER_TYPE_MAX,
}BTLV_TIMER_TYPE;

//grp系
static const f32 LAYOUT_DOWN_OFS_APP = -290.0f;
static const f32 LAYOUT_DOWN_OFS = -1000.0f;

/**
 * @brief ２択選択モード
 */
 enum YesNoMode 
 {
   YESNO_MODE_YESNO,       //「はい」                「いいえ」
   YESNO_MODE_NEXT_POKE,   //「次のポケモンを使う」  「にげる」
   YESNO_MODE_POKE_CHANGE, //「ポケモンを入れ替える」「そのまま使う」
   YESNO_MODE_FORGET,      //「わすれさせる」        「わすれさせない」
 };


GFL_NAMESPACE_END(btl)


#endif // __BTLV_TYPES_H__
