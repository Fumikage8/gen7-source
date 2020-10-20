//======================================================================
/**
 * @file Run_Energy.cpp
 * @date 2016/11/10 18:18:42
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <algorithm>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_MathCommon.h>
#include "./Energy.h"
#include "./Parameter.h"
#include "../Assert.h"
#if PM_DEBUG
#include "GameSys/include/GameManager.h"
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Run{
// constructor
Energy::Energy()
    : m_Limit(1.0f),
      m_Rate(0.0f),
      m_IsOverLimit(false){}
// Initialize
void Energy::Initialize(
            const f32& rate)
{
  // initialize Energy Rate
  m_Rate = rate;
  LUNA_SOL_DIVE_ASSERT(0.0f <= m_Rate && m_Rate <= m_Limit);
}
// Terminate
void Energy::Terminate()
{
  // unset Parameter
  m_Limit = 1.0f;
  m_Rate = 0.0f;
  m_IsOverLimit = false;
}
// Update
void Energy::Update(
            const f32& gain,
            const f32& loss)
{
  LUNA_SOL_DIVE_ASSERT(gain >= 0.0f);
  LUNA_SOL_DIVE_ASSERT(loss >= 0.0f);
  m_Rate +=
          gain
          - loss
          - 1.0f / Parameter::Get().EnergyAttenuationFrames();
#if PM_DEBUG
  // manually up/down
  if(Debug::Parameter::Get().isManuallyEnergyUpDownEnabled)
  {
    const gfl2::ui::Button* const pButton =
            GFL_SINGLETON_INSTANCE(GameSys::GameManager)
            ->GetUiDeviceManager()
            ->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    if(pButton->IsTrigger(gfl2::ui::BUTTON_UP))
    {
      m_Rate += 0.1f;
    }
    if(pButton->IsTrigger(gfl2::ui::BUTTON_DOWN))
    {
      m_Rate -= 0.1f;
    }
  }
  // constant mode
  if(Debug::Parameter::Get().isConstantEnergyMode)
  {
    m_Rate = Debug::Parameter::Get().energyInConstantEnergyMode;
  }
#endif// PM_DEBUG
  // update limit
  m_IsOverLimit = false;
  if(m_Rate > m_Limit)
  {
    m_Limit = std::min(
                m_Limit + Parameter::Get().EnergyLimitIncrease(),
                Parameter::Get().EnergyLimitLimit());
    m_IsOverLimit = true;
  }
  m_Rate = gfl2::math::clamp(
              m_Rate,
              0.0f,
              m_Limit);
  LUNA_SOL_DIVE_ASSERT(0.0f <= m_Rate && m_Rate <= m_Limit);
}
// check if the Energy is empty
bool Energy::IsEmpty() const
{
  return m_Rate < EPSILON;
}
// Speed
f32 Energy::Speed() const
{
  const f32& max = Parameter::Get().ForwardSpeedMax();
  const f32& min = Parameter::Get().ForwardSpeedMin();
  return min + (max - min) * Rate();
}
// Moving Rate on the Vertical Plane
f32 Energy::VerticalMovingRate() const
{
  if(IsEmpty())
  {
    return Parameter::Get().CrossSectionSpeedEmpty();
  }
  else
  {
    const f32& max = Parameter::Get().CrossSectionSpeedMax();
    const f32& min = Parameter::Get().CrossSectionSpeedMin();
    return min + (max - min) * Rate();
  }
}
// Limit of Rate
const f32& Energy::Limit() const
{
  return m_Limit;
}
// Rate
const f32& Energy::Rate() const
{
  LUNA_SOL_DIVE_ASSERT(0.0f <= m_Rate && m_Rate <= m_Limit);
  return m_Rate;
}
// Whether energy is over the limit
const bool& Energy::IsOverLimit() const
{
  return m_IsOverLimit;
}
}// namespace Run
}// namespace LunaSolDive
