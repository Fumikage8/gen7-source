//============================================================================================
/**
* @file	RotomPowerTime.cpp
* @brief	ロトムパワー効果時間
* @date	2017.1.24
* @author	munakata_kai
*
*/
//============================================================================================
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "Field/FieldStatic/include/RotomPower/RotomPowerTime.h"
#include <math/include/gfl2_math_control.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEventManager.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(RotomPower)

//---------------------------------------------------------------------------
/**
 * @brief	コンストラクタ
 */
//---------------------------------------------------------------------------
RotomPowerTime::RotomPowerTime()
{
	this->Initialize();
}

//---------------------------------------------------------------------------
/**
 * @brief	デストラクタ
 */
//---------------------------------------------------------------------------
RotomPowerTime::~RotomPowerTime()
{
}

//---------------------------------------------------------------------------
/**
 * @brief	ロトムパワー効果時間の初期化処理
 */
//---------------------------------------------------------------------------
void RotomPowerTime::Initialize()
{
  m_minute = 0;
  m_seconds = 0;
  m_maxLife = 0;
  m_isEnd = false;
  m_isStart = false;
  m_correctionTime = 0;
  m_isCorrectionTime = false;
}

//---------------------------------------------------------------------------
/**
 * @brief	ロトムパワー効果時間のセット
 * @param	min		分
 * @param	sec		秒
 */
//---------------------------------------------------------------------------
void RotomPowerTime::SetRotomPowerTime( const u32 min, const u32 sec )
{
  GFL_ASSERT(min <= PTIME_MINUTE_MAX);
  GFL_ASSERT(sec <= PTIME_SECONDS_MAX);
  m_minute = min;
  m_seconds = sec;
  m_maxLife = this->GetLife();
  m_isEnd = false;
  m_isStart = true;
  m_correctionTime = sec;
  m_isCorrectionTime = false;
  GFL_PRINT("---rotom power correction time = %d---\n", m_correctionTime);
  GFL_PRINT("rotom power time start!\n");
}

//---------------------------------------------------------------------------
/**
 * @brief ロトムパワー効果時間のカウントダウン
 * @param	passed_sec	経過時間
 */
//---------------------------------------------------------------------------
void RotomPowerTime::CountDown( const u32 passed_sec )
{
  // 開始してない又は終わった場合はカウントダウンしない
  if (!m_isStart || m_isEnd)
  {
    GFL_ASSERT(0);
    return;
  }

  s32 min = m_minute;
  s32 sec = m_seconds - passed_sec;

  // @fix MMCat[185] ロトムパワー使用中にスリープやHOMEボタン押下を行うと、ゲージの減少と効果の終了時間が一致しなくなる
  // if( sec < 0 )判定だと計算前の残り効果時間が０分５８秒で
  // 経過時間passed_secが５８秒となる場合、secが０秒となるため下記の分岐処理に入れないため、効果が終わらない
	if( sec <= 0 )
	{
    s32 calcSec = (static_cast<s32>(gfl2::math::Abs(sec) % (PTIME_SECONDS_MAX + 1)));
    // calcSec == 0 の時は、(-1 + (sec / (PTIME_SECONDS_MAX + 1)))の先頭-1がいらない
    // この分岐を入れないとcalcSec==0の時、１分多く時間が減ってしまう
    if (calcSec == 0)
    {
      min += (sec / static_cast<s32>(PTIME_SECONDS_MAX + 1));
    }
    // １分減るのは確定（前者）２分以上（secが-61等）減らないかを後者で確認
    else
    {
      min += (-1 + (sec / static_cast<s32>(PTIME_SECONDS_MAX + 1)));
    }
    // ２分以上（secが-61等）の場合の処理
    sec = -calcSec;
    // @fix MMCat[185] ロトムパワー使用中にスリープやHOMEボタン押下を行うと、ゲージの減少と効果の終了時間が一致しなくなる
    // if (min < 0 && sec < 0)だとminが0秒以下でsecが0秒となるパターンでm_isEnd=trueとなる処理に入れない→効果が終わらない
    if ((min < 0 && sec < 0) || (min <= 0 && sec == 0))
    {
      min = 0;
      sec = 0;
    }
    else if (sec < 0)
    {
      sec = sec + (PTIME_SECONDS_MAX + 1);
    }
	}

  // 計測終了判定
  if (min <= 0 && sec <= 0)
  {
    m_isEnd = true;
    GFL_PRINT("rotom power time end!\n");
  }
  GFL_ASSERT(min >= 0 && sec >= 0);

  // 余剰時間か
  // イベント中は余剰時間フラグをＯＮにしない＝イベント終了時に即効果時間終了となる場合を回避
  // このUpdate後にGameEventManagerのイベント更新があるため、イベント終了後の次フレームで余剰時間フラグはＯＮとなる
  if (!m_isCorrectionTime &&
      !GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameEventManager()->IsExists() &&
      sec <= m_correctionTime && 
      min <= 0)
  {
    GFL_PRINT("---rotom power correction time(%d)(sec = %d) start---\n", m_correctionTime, sec);
    m_isCorrectionTime = true;
  }

	m_minute = min;
	m_seconds = sec;
}

//---------------------------------------------------------------------------
/**
* @brief ロトムパワー効果時間が終了したか否か
* @return bool
*/
//---------------------------------------------------------------------------
bool RotomPowerTime::IsEnd() const
{
  return (m_isEnd && m_isStart);
}

//---------------------------------------------------------------------------
/**
* @brief ロトムパワー効果時間が補正時間中か否か
* @return bool
*/
//---------------------------------------------------------------------------
bool RotomPowerTime::IsCorrectionTime() const
{
  return (m_isCorrectionTime && m_isStart);
}

//---------------------------------------------------------------------------
/**
* @brief ロトムパワー補正時間計算
* @param s32 余剰時間
*/
//---------------------------------------------------------------------------
void RotomPowerTime::CalcCorrectionTime(const s32 correctionTime)
{
  GFL_ASSERT(!IsEnd());
  // 補正時間変更
  m_correctionTime = correctionTime;
  GFL_PRINT("---rotom power calc correction time = %d---\n", m_correctionTime);
  // 残り時間再計算
  s32 subTime = m_seconds - correctionTime;
  if (subTime < 0)
  {
    // 例 m_seconds 30秒→59秒(correctionTime)変更→この場合分も関与する
    subTime += (PTIME_SECONDS_MAX + 1);
    // CountDown後 計測終了となっている場合あり
  }
  // 上記のelse例 m_seconds 30秒→10秒(correctionTime)変更
  CountDown(subTime);

  if (!IsEnd() && m_minute <= 0)
  {
    // 残り時間が59秒以下となった場合に、即時効果終了させないため
    GFL_PRINT("---rotom power correction time cancel---\n");
    m_isCorrectionTime = false;
  }

  if (m_maxLife > subTime)
  {
    m_maxLife -= subTime;
    GFL_ASSERT(m_maxLife >= GetLife());
  }
  else
  {
    // 下画面で割合計算に使用しているため０になってはならない
    m_maxLife = 1;
  }
  GFL_ASSERT_MSG(((m_seconds == correctionTime) || m_isEnd ), "WARNING : CalcCorrectionTimeが意図しない挙動をしました！");
  GFL_PRINT("---rotom power minute = %d seconds time = %d---\n", m_minute, m_seconds);
  GFL_PRINT("---rotom power max life = %d---\n", m_maxLife);
}

GFL_NAMESPACE_END(RotomPower)
GFL_NAMESPACE_END(Field)
