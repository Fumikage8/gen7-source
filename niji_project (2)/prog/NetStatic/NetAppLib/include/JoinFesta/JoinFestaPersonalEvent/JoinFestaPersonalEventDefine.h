//==============================================================================
/**
 * @file	JoinFestaPersonalEventDefine.h
 * @brief	ジョインフェスタパーソナルイベント定義
 * @author	ichiraku_katsuhiko
 * @date	2015/11/09　12:47
 */
// =============================================================================
#ifndef __JOIN_FESTA_PERSONAL_EVENT_DEFINE_H__
#define __JOIN_FESTA_PERSONAL_EVENT_DEFINE_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

// JoinFestaFriendKey
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaPersonalEventDefine
{
public:

  //-----------------------------------------------------------------------------
  /* ----------------------------------------------------------------------------------------- */
  /**
   *  @brief 発動するパーソナルイベントの最大数
   */
  /* ----------------------------------------------------------------------------------------- */
  static const u32 ACTUATE_PERSONAL_EVENT_MAX = 10;

  /* ----------------------------------------------------------------------------------------- */
  /**
   *  @brief パーソナルイベントの発動抽選間隔（秒）
   */
  /* ----------------------------------------------------------------------------------------- */
  static const u32 PERSONAL_EVENT_LOTTERY_TIME = 3600; // 60分

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

  /**
   *  @enum Difficulty
   *        難易度
   */ 
  enum Difficulty{
    DIFFICULTY_EASY,     //! 低
    DIFFICULTY_NORMAL,   //! 中
    DIFFICULTY_HARD,     //! 高
  };

  /**
   *  @enum State
   *        イベント状態
   */ 
  enum State{
    STATE_DISABLE,        //! イベント無効（そもそもイベントをしない）
    STATE_LOTTERY_NO,     //! 抽選していない
    STATE_LOTTERY_OK,     //! 抽選済み
    STATE_FINISH,         //! イベント終了した
  };

  /**
   * @struct SlotData
   *         1スロットのデータ
   */
  struct SlotData{
    u8    state;        //!< イベント状態
    u8    difficulty;   //!< 難易度
    u8    dummy[6];
    JoinFestaFriendKey friendKey;   //!< イベント発動していた場合、対象者のフレンドキー
  };

};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

#endif // __JOIN_FESTA_PERSONAL_EVENT_DEFINE_H__
