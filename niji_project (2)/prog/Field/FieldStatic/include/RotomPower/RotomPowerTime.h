//============================================================================================
/**
 * @file	RotomPowerTime.h
 * @brief	ロトムパワー効果時間
 * @date	2017.1.24
 * @author	munakata_kai
 *
 */
//============================================================================================
#if !defined( ROTOMPOWER_TIME_H_INCLUDED )
#define ROTOMPOWER_TIME_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(RotomPower)

class RotomPowerTime
{
  GFL_FORBID_COPY_AND_ASSIGN(RotomPowerTime); ///< コンストラクタ + 代入禁止
public:
  //---------------------------------------------------------------------------
  //		ロトムパワー効果時間の最大値の定義
  //---------------------------------------------------------------------------
  static const u32 PTIME_MINUTE_MAX	= (59);			///<ロトムパワー効果時間（分）の最大値
  static const u32 PTIME_SECONDS_MAX	= (59);			///<ロトムパワー効果時間（秒）の最大値

private:
  u32  m_minute;          // ロトムパワー効果時間（分）
  u32  m_seconds;         // ロトムパワー効果時間（秒）
  u32  m_maxLife;         // ロトムパワー効果時間
  bool m_isEnd;           // ロトムパワー効果時間が終了したか否か
  bool m_isStart;         // 計測開始フラグ
  u32  m_correctionTime;  // 効果時間補正分の時間
  bool m_isCorrectionTime;// 補正時間中か

public:
  RotomPowerTime();
  virtual ~RotomPowerTime();
  void Initialize();
  void CountDown(const u32 passed_sec);
  //---------------------------------------------------------------------------
  /**
  * @brief ロトムパワー効果残り時間(分)のみ取得（残り２分５３秒なら２を取得）
  * @return u8
  */
  //---------------------------------------------------------------------------
  inline u32 GetMinute() const { return m_minute; }
  //---------------------------------------------------------------------------
  /**
  * @brief ロトムパワー効果残り時間(秒)のみ取得（残り２分５３秒なら５３を取得）
  * @return u8
  */
  //---------------------------------------------------------------------------
  inline u32 GetSeconds() const { return m_seconds; }

  //---------------------------------------------------------------------------
  /**
  * @brief ロトムパワー効果残り時間を秒で取得（残り２分５３秒なら１７３を取得）
  * @return u32
  */
  //---------------------------------------------------------------------------
  inline u32 GetLife() const { return (m_minute * 60 + m_seconds); }

  //---------------------------------------------------------------------------
  /**
  * @brief ロトムパワー効果の最大時間（効果時間）取得
  * @return u32
  */
  //---------------------------------------------------------------------------
  inline u32 GetMaxLife() const { return m_maxLife; }

  //---------------------------------------------------------------------------
  /**
  * @brief ロトムパワー効果時間が終了したか否か
  * @return bool
  */
  //---------------------------------------------------------------------------
  bool IsEnd() const;

  //---------------------------------------------------------------------------
  /**
   * @brief ロトムパワー効果時間のセット
   * @param min   分
   * @param sec   秒
   */
  //---------------------------------------------------------------------------
  void SetRotomPowerTime(const u32 min, const u32 sec);

  //---------------------------------------------------------------------------
  /**
  * @brief ロトムパワー効果時間が補正時間中か否か
  * @return bool
  */
  //---------------------------------------------------------------------------
  bool IsCorrectionTime() const;

  //---------------------------------------------------------------------------
  /**
  * @brief ロトムパワー補正時間計算
  * @return bool
  */
  //---------------------------------------------------------------------------
  void CalcCorrectionTime(const s32 correctionTime);
};

GFL_NAMESPACE_END(RotomPower)
GFL_NAMESPACE_END(Field)

#endif  // ROTOMPOWER_TIME_H_INCLUDED
