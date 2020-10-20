//============================================================================================
/**
 * @file  PlayTimeManager.h
 * @brief	プレイ時間情報の管理
 * @date	2011.12.29
 * @author	Hisanao Suzuki
 *
 */
//============================================================================================
#if !defined( PLAYTIME_MANAGER_H_INCLUDED )
#define PLAYTIME_MANAGER_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <base/include/gfl2_Singleton.h>


//-----------------------------------------------------------------------------
// クラス前方参照宣言
//-----------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(Savedata)
class PlayTime;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(GameSys)

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class PlayTimeManager : public gfl2::base::Singleton<PlayTimeManager>
{
private:
  // GetSystemCurrent()をコールする間隔
  // ※上記関数は負荷が大きい
  static const int  TICK_TIME_INTERVAL = 20;
private:
  bool m_isStartFlag; // 計測開始フラグ
  s64  m_lastTick;    // 前回計測時のチック数
  s64  m_passedSec;   // 経過チック数
  s64  m_startTick;   // 計測開始時のチック数
  u32  m_interval;    // 計測周期カウント

public:
  //------------------------------------------------------------------
  /**
   * @brief 初期化（ゲームメインループの前に１回だけ呼ぶ）
   */
  //------------------------------------------------------------------
  void Initialize( void );

  //------------------------------------------------------------------
  /**
   * @brief プレイ時間のカウント開始（ゲーム開始直後に１回だけ呼ぶ）
   */
  //------------------------------------------------------------------
  void Start( void );

  //------------------------------------------------------------------
  /**
   * @brief プレイ時間のカウント（メインループ内で毎回呼ぶ）
   *
   * @param time    プレイ時間セーブデータポインタ
   */
  //------------------------------------------------------------------
  void Update( Savedata::PlayTime* playTime );

  //------------------------------------------------------------------
  /**
   * @brief スリープ復帰処理（スリープしていた時間はカウントしない）
   */
  //------------------------------------------------------------------
  void Restert( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief システムチックを秒数に変換
   *
   * @param tickTime システムチック
   *
   * @return 秒数
   */
  //------------------------------------------------------------------
  inline s64 ConvertTicksToSeconds( s64 tickTime );

};

GFL_NAMESPACE_END(GameSys)


/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( GameSys::PlayTimeManager );


#endif	// PLAYTIME_MANAGER_H_INCLUDED
