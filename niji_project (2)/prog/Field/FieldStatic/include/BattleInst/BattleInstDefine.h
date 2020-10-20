﻿//==============================================================================
/**
 * @file	BattleInstDefine.h
 * @brief	バトル施設の定義類
 * @author	onoue_masayuki
 * @date	2016/01/27
 */
// =============================================================================

#ifndef __BATTLEINSTDEFINE_H__
#define __BATTLEINSTDEFINE_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include "niji_reference_files/script/FieldPawnTypes.h"

//-----------------------------------------------------------------------------

namespace BattleInst{

//-------------------------------------
/// 関数の結果
//=====================================
typedef enum
{
  RESULT_CONTINUE,    ///< 動作継続中
  RESULT_FINISH,      ///< 動作終了
}RESULT;


//-------------------------------------
/// 関数の結果
//=====================================
typedef enum
{
  COMMAND_ID_SEND_COMMDATA,
  COMMAND_ID_SEND_TRAINER_DATA,
  COMMAND_ID_YES_NO,
  COMMAND_ID_SEND_CHECK_RESULT,
}CommCommandID;

/**
 *  @brief  定義クラス
 */
class BattleInstDefine
{
public:

  /* ----------------------------------------------------------------------------------------- */
  /**
   *  @brief 動的に出現するモデルの最大数
   */
  /* ----------------------------------------------------------------------------------------- */
  static const u32 BINST_MODEL_DISP_MAX = 3;  

  /* ----------------------------------------------------------------------------------------- */
  /**
   *  @brief モデル表示範囲（半径）
   */
  /* ----------------------------------------------------------------------------------------- */
  static const u32 BINST_DISP_DISTANCE = 1000;  

  /* ----------------------------------------------------------------------------------------- */
  /**
   *  @brief モデルのeventIDオフセット値
   *  @note   フィールドの動作モデルにはeventIDがあり、それはユニークである必要がある。
   */
  /* ----------------------------------------------------------------------------------------- */
  static const u32 MODEL_EVENT_ID_OFFSET = Field::BATTLE_INST_EVENT_ID_OFFSET;

  /* ----------------------------------------------------------------------------------------- */
  /**
   *  @brief 名前の最大文字数
   */
  /* ----------------------------------------------------------------------------------------- */
  static const u32 STRLEN_BATTLE_INST_NAME = 20;


  /* ----------------------------------------------------------------------------------------- */
  /**
   *  @brief 選べるBGMの最大数 momiji追加
   */
  /* ----------------------------------------------------------------------------------------- */
  static const u32 BINST_BGM_COUNT = 19 + 25;

  /* ----------------------------------------------------------------------------------------- */
  /**
  *  @brief リーリエの登録番号
  */
  /* ----------------------------------------------------------------------------------------- */
  static const u32 BINST_LILIE_DATA_ID = 206;

};

} // namespace BattleInst

#endif // __BATTLEINSTDEFINE_H__
