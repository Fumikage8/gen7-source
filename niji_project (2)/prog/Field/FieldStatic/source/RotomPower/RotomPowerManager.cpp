//============================================================================================
/**
* @file  RotomPowerManager.cpp
* @brief	ロトムパワー効果制御の管理
* @date	2017.1.24
* @author	munakata_kai
*
*/
//============================================================================================

#if defined(GF_PLATFORM_CTR)
#include <nn/os.h>
#elif defined(GF_PLATFORM_WIN32)
#endif
#include "GameSys/include/GameData.h"
#include "GameSys/include/NijiTime.h"
#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"
#include "Field/FieldStatic/include/RotomPower/RotomPowerTime.h"
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(RotomPower)

RotomPowerManager::RotomPowerManager(gfl2::heap::HeapBase * pHeap)
: m_isStartFlag(false)
, m_lastTick(0)
, m_passedSec(0)
, m_startTick(0)
, m_interval(0)
, m_pRotomPowerTime(NULL)
, m_rotomPowerID(ROTOM_POWER_ID_NULL)
, m_endRotomPowerID(ROTOM_POWER_ID_NULL)
#if PM_DEBUG
, m_rotomPowerTime(0)
#endif
{
  m_pRotomPowerTime = GFL_NEW(pHeap) RotomPowerTime();
}

RotomPowerManager::~RotomPowerManager()
{
  GFL_SAFE_DELETE(m_pRotomPowerTime);
}

//------------------------------------------------------------------
/**
* @brief ロトムパワーセット（発動はしない）
*/
//------------------------------------------------------------------
void RotomPowerManager::SetRotomPower(const ROTOM_POWER_ID powerId)
{
  m_endRotomPowerID = ROTOM_POWER_ID_NULL;
  // 不正なID判定
  if (m_rotomPowerID < ROTOM_POWER_ID_NULL && m_rotomPowerID >= ROTOM_POWER_ID_MAX)
  {
    GFL_ASSERT_MSG(0, "---rotom power error---\n");
    ClearRotomPower();
    return;
  }
  MUNAKATA_PRINT("---rotom power %d set---\n", powerId);
  // 効果セット
  m_rotomPowerID = powerId;

  GameSys::DeviceDate now_date;
  GFL_PRINT("---rotom power now_second %d---\n", now_date.GetSecond());

  // 一定周期でしか実行しない（下Update処理内でif (TICK_TIME_INTERVAL > ++m_interval){）ため、
  // 本体時間と効果時間に若干の誤差が生まれる。余剰時間中なら効果終了で対処
  s32 updateSecond = getCorrectionTime(now_date.GetSecond());

  s32 power_minute = POWER_TIME;
  // たまごふかポンのみ効果時間30分
  if (m_rotomPowerID == ROTOM_POWER_ID_EGG_INC)
  {
    power_minute = POWER_EGG_INC_TIME;
  }
  else if (m_rotomPowerID == ROTOM_POWER_ID_SOUGUU_DEC)
  {
    power_minute = POWER_SOUGUU_DEC_TIME;
  }

#if PM_DEBUG
  if (m_rotomPowerTime > 0)
  {
    m_pRotomPowerTime->SetRotomPowerTime(static_cast<u32>(m_rotomPowerTime), static_cast<u32>(updateSecond));
  }
  else
  {
    m_pRotomPowerTime->SetRotomPowerTime(power_minute, static_cast<u32>(updateSecond));
  }
#else
  m_pRotomPowerTime->SetRotomPowerTime(power_minute, static_cast<u32>(updateSecond));
#endif
}

//------------------------------------------------------------------
/**
* @brief ロトムパワークリア
*/
//------------------------------------------------------------------
void RotomPowerManager::ClearRotomPower()
{
  m_isStartFlag = false;
  m_lastTick = 0;
  m_passedSec = 0;
  m_startTick = 0;
  m_interval = 0;
  m_pRotomPowerTime->Initialize();
  m_rotomPowerID = ROTOM_POWER_ID_NULL;
}

//------------------------------------------------------------------
/**
* @brief ロトムパワー発動
*/
//------------------------------------------------------------------
void RotomPowerManager::Start()
{
  if (m_rotomPowerID < ROTOM_POWER_ID_NULL && m_rotomPowerID >= ROTOM_POWER_ID_MAX)
  {
    GFL_ASSERT_MSG(0, "---rotom power error---\n");
    ClearRotomPower();
    return;
  }
  GFL_PRINT("---rotom power start---\n");
  m_isStartFlag = true;
  m_lastTick    = 0;
  m_passedSec   = 0;
#if defined(GF_PLATFORM_CTR)
  m_startTick   = nn::os::Tick::GetSystemCurrent();
#elif defined(GF_PLATFORM_WIN32)
  m_startTick   = gfl2::system::Timer::PerformanceCounter::GetTick();
#endif
}

//------------------------------------------------------------------
/**
 * @brief ロトムパワー効果時間のカウント（メインループ内で毎回呼ぶ）EvTimeの更新より先に呼ばれる
 */
//------------------------------------------------------------------
void RotomPowerManager::Update( void )
{
	if( m_isStartFlag )
	{
	  // 一定周期でしか実行しない
	  if( TICK_TIME_INTERVAL > ++m_interval ){
	    return;
	  }
    m_interval = 0;

    s64 sec;
#if defined(GF_PLATFORM_CTR)
    s64 currentTick = nn::os::Tick::GetSystemCurrent();
#elif defined(GF_PLATFORM_WIN32)
    s64 currentTick = gfl2::system::Timer::PerformanceCounter::GetTick();
#endif
    s64 tick        = currentTick;

    // チックタイムの逆点現象チェック
    // Wifi接続時にtickは０クリアされる。
    if( m_lastTick > tick ){
      // 0クリアされる前のチック分も足しこむ。
      tick = tick + m_lastTick;
    }else{
      m_lastTick = tick;
    }

    sec = convertTicksToSeconds( tick - m_startTick );
		if( sec > m_passedSec )
		{
      m_pRotomPowerTime->CountDown(sec - m_passedSec);
      // カウント終了　※効果は継続
      if (m_pRotomPowerTime->IsEnd())
      {
        m_isStartFlag = false;
#if PM_DEBUG
        GameSys::DeviceDate now_date;
        GFL_PRINT("---rotom power count end now_second %d---\n", now_date.GetSecond());
#endif
      }

      // 通常時
      if( m_lastTick == tick ){
        m_passedSec = sec;
      }
      // 何かしらの処理でTickがクリアされた場合、再初期化。
      else
      {
        m_startTick = currentTick;
        m_passedSec = 0;
        m_lastTick  = currentTick;
      }
		}
	}
}


//------------------------------------------------------------------
/**
 * @brief スリープ復帰処理（スリープしていた時間はカウントしない）
 */
//------------------------------------------------------------------
void RotomPowerManager::RestartFromSleep(void)
{
  if( m_isStartFlag )
  {
#if defined(GF_PLATFORM_CTR)
    s64 currentTick = nn::os::Tick::GetSystemCurrent();
#elif defined(GF_PLATFORM_WIN32)
    s64 currentTick = gfl2::system::Timer::PerformanceCounter::GetTick();
#endif
    m_startTick = currentTick;
    m_passedSec = 0;
    m_lastTick  = currentTick;

    // @fix MMCat[185] ロトムパワー使用中にスリープやHOMEボタン押下を行うと、ゲージの減少と効果の終了時間が一致しなくなる
    GameSys::DeviceDate now_date;
    GFL_PRINT("---rotom power restart now_second %d---\n", now_date.GetSecond());
    // 余剰時間再計算
    m_pRotomPowerTime->CalcCorrectionTime(getCorrectionTime(now_date.GetSecond()));
    // カウント終了　※効果は継続
    if (m_pRotomPowerTime->IsEnd())
    {
      m_isStartFlag = false;
#if PM_DEBUG
      GameSys::DeviceDate now_date;
      GFL_PRINT("---rotom power count end now_second %d---\n", now_date.GetSecond());
#endif
    }
  }
}

//------------------------------------------------------------------
/**
* @brief EvTimeにおいて分更新処理（効果終了処理）
*/
//------------------------------------------------------------------
void RotomPowerManager::UpdateMinuteEvent()
{
  // EvTimeのUpdateMinuteEventで効果終了
  if ((!m_isStartFlag && m_pRotomPowerTime->IsEnd()) || m_pRotomPowerTime->IsCorrectionTime())
  {
    GFL_PRINT("---rotom power end---\n");
    m_endRotomPowerID = m_rotomPowerID;
    this->ClearRotomPower();
  }
}

//------------------------------------------------------------------
/**
* @brief ロトムパワーが発動しているか（時間が０でも発動している場合あり）
*/
//------------------------------------------------------------------
bool RotomPowerManager::IsRotomPower() const
{
  // こうかなし状態以外ならtrue
  if (m_rotomPowerID != ROTOM_POWER_ID_NULL)
  {
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------
/**
* @brief パワー計算：タマゴ孵化
* @param add_count   加算する値（下位8ビット少数）
* @return u32 パワー影響後の加算値（下位8ビット少数）
*/
//--------------------------------------------------------------------------
u32 RotomPowerManager::CalcHatch(const u32 add_count)
{
  u32 calc_add_count = add_count;

  if (m_rotomPowerID == ROTOM_POWER_ID_EGG_INC)
  {
    const u32 max = 0xffffffff;
    GFL_ASSERT(add_count < (max / POWER_EGG_INC));
    if (add_count < (max / POWER_EGG_INC))
    {
      calc_add_count = (add_count * POWER_EGG_INC) / 100;
    }
    else
    {
      calc_add_count = max / 100;
    }
  }

  MUNAKATA_PRINT("RotomPowerManager:パワー：タマゴ孵化 %d -> %d\n", add_count, calc_add_count);
  return calc_add_count;
}

//--------------------------------------------------------------------------
/**
* @brief パワー計算：商品の値段
* @param price     商品の値段
* @return u32 パワー影響後の商品の値段
*/
//--------------------------------------------------------------------------
u32 RotomPowerManager::CalcSale(const u32 price)
{
  u32 calc_price = price;

  if (m_rotomPowerID == ROTOM_POWER_ID_SALE)
  {
    const u32 max = 0xffffffff;
    GFL_ASSERT(price < (max / POWER_SALE));
    if (price < (max / POWER_SALE))
    {
      calc_price = (price * POWER_SALE) / 100;
    }
    else
    {
      calc_price = max / 100;
    }
  }

  MUNAKATA_PRINT("RotomPowerManager:パワー：商品の値段 %d -> %d\n", price, calc_price);
  return calc_price;
}

//--------------------------------------------------------------------------
/**
* @brief パワー計算：獲得おこづかい
* @param money   所持道具効果などの補正後のお小遣い
* @return u32 パワー影響後のおこづかい
*/
//--------------------------------------------------------------------------
u32 RotomPowerManager::CalcMoney(const u32 money)
{
  u32 calc_money = money;

  if (m_rotomPowerID == ROTOM_POWER_ID_MONEY)
  {
    calc_money = money * POWER_MONEY;
  }

  MUNAKATA_PRINT("RotomPowerManager:パワー：獲得お金 %d -> %d\n", money, calc_money);
  return calc_money;
}

//--------------------------------------------------------------------------
/**
* @brief パワー計算：獲得経験値
* @param exp   他人の、外国の、学習装置分配、の補正後の獲得経験値
* @return u32 パワー影響後の獲得経験値
*/
//--------------------------------------------------------------------------
u32 RotomPowerManager::CalcExp(const u32 exp)
{
  u32 calc_exp = exp;

  if (m_rotomPowerID == ROTOM_POWER_ID_EXP_INC)
  {
    const u32 max = 0xffffffff;
    GFL_ASSERT(exp < (max / POWER_EXP_INC));
    if (exp < (max / POWER_EXP_INC))
    {
      calc_exp = (exp * POWER_EXP_INC) / 100;
    }
    else
    {
      calc_exp = max / 100;
    }
  }

  MUNAKATA_PRINT("RotomPowerManager:パワー：獲得経験値 %d -> %d\n", exp, calc_exp);

  return calc_exp;
}

//--------------------------------------------------------------------------
/**
* @brief パワー計算：なつきどへの加算値
* @param natsuki   道具などの補正が入った後のなつき度加算値
* @return u32 パワー影響後のなつき度加算値
*/
//--------------------------------------------------------------------------
s32 RotomPowerManager::CalcNatsuki(const s32 natsuki)
{
  u32 calc_natsuki = natsuki;

  // なつき度が減算の時は影響させない
  if (m_rotomPowerID == ROTOM_POWER_ID_NATSUKI && natsuki > 0)
  {
    calc_natsuki = natsuki + POWER_NATSUKI;
  }

  MUNAKATA_PRINT("RotomPowerManager:パワー：なつき度 %d -> %d\n", natsuki, calc_natsuki);
  return calc_natsuki;
}

//--------------------------------------------------------------------------
/**
* @brief パワー計算：エンカウント率
* @param encount   現在のエンカウント率（上限100%）
* @return u32 パワー影響後のエンカウント率
*/
//--------------------------------------------------------------------------
u32 RotomPowerManager::CalcEncount(const u32 encount)
{
  u32 calc_encount = encount;

  // エンカウント率０ならば、そのまま０を返す
  if (calc_encount == 0)
  {
    return calc_encount;
  }

  // エンカウント率アップのパワーが発動しているか
  if (m_rotomPowerID == ROTOM_POWER_ID_SOUGUU_INC)
  {
    calc_encount = encount * POWER_SOUGUU_INC;
  }

  // 上限下限チェック
  if (calc_encount > 100)
  {
    calc_encount = 100;
  }

  MUNAKATA_PRINT("RotomPowerManager:パワー：エンカウント率 %d -> %d\n", encount, calc_encount);
  return calc_encount;
}

//--------------------------------------------------------------------------
/**
* @brief レベル計算：特性「やるき」の絶対発動パターン
* @param level    レベル
* @param maxLevel 最大レベル
* @return u32 パワー影響後のエンカウント率
*/
//--------------------------------------------------------------------------
int RotomPowerManager::CalcLevel(const int level, const int maxLevel)
{
  // レベルが既に最大レベルの場合は、何もしない
  if (level == maxLevel)
  {
    return level;
  }

  int after_level = level;

  // みっけポンが発動中の場合
  if (m_rotomPowerID == ROTOM_POWER_ID_SOUGUU_INC)
  {
    after_level = maxLevel;
  }

  MUNAKATA_PRINT("RotomPowerManager:パワー：レベル補正 %d -> %d\n", level, after_level);
  return after_level;
}

//--------------------------------------------------------------------------
/**
* @brief エンカウント回避判定
* @return bool 回避できるか（true:回避 false:回避不可）
*/
//--------------------------------------------------------------------------
bool RotomPowerManager::IsEncountAvoid()
{
  // かくれポンが発動中の場合
  if (m_rotomPowerID == ROTOM_POWER_ID_SOUGUU_DEC)
  {
    MUNAKATA_PRINT("RotomPowerManager:パワー：エンカウント回避発動\n");
    return true;
  }
  return false;
}

#if PM_DEBUG
//--------------------------------------------------------------------------
/**
* @brief デバッグ用ロトム効果時間設定
* @param time   ロトム効果時間
*/
//--------------------------------------------------------------------------
void RotomPowerManager::SetDebugRotomPowerTime(const s32 time)
{
  m_rotomPowerTime = time;
}
#endif

// 非公開
//------------------------------------------------------------------
/**
 * @brief システムチックを秒数に変換
 *
 * @param tickTime システムチック
 *
 * @return 秒数
 */
//------------------------------------------------------------------
inline s64 RotomPowerManager::convertTicksToSeconds(const s64 tickTime) const
{
  return tickTime / gfl2::system::Timer::PerformanceCounter::GetFrequency();  // CTRではnn::os::Tick::TICKS_PER_SECONDが返ってくる
}

//--------------------------------------------------------------------------
/**
* @brief 余剰時間計算
* @param s32 現在時刻
* @return s32 余剰時間
*/
//--------------------------------------------------------------------------
s32 RotomPowerManager::getCorrectionTime(const s32 sec)
{
  const s32 CORRECTION_TIME = 59;
  s32 updateSecond = CORRECTION_TIME - sec;
  updateSecond = gfl2::math::Clamp(updateSecond, 0, CORRECTION_TIME);
  return updateSecond;
}

GFL_NAMESPACE_END(RotomPower)
GFL_NAMESPACE_END(Field)
