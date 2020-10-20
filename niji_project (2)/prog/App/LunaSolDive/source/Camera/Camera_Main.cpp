//======================================================================
/**
 * @file Camera_Main.cpp
 * @date 2016/12/13 15:43:51
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <math/include/gfl2_Matrix44.h>
#include "System/include/GflUse.h"
#include "./Main.h"
#include "./Parameter.h"
#include "../Assert.h"
#include "../Effect/Manager.h"
#include "../Effect/Particle.h"
#include "../Rail/Location.h"
#include "../Rail/Manager.h"
#include "arc_index/LunaSolDiveEffect.gaix"

namespace LunaSolDive{
namespace Camera{
// static function
// Trace
static gfl2::math::Vector2 Trace(
            const gfl2::math::Vector2& location,
            const gfl2::math::Vector2& destination,
            const f32 maxMovement);
// constructor
Main::Main()
    : Base(),
      m_pRailManager(NULL),
      m_pEffectManager(NULL),
      m_CrossSectionPoint(gfl2::math::Vector2::GetZero()),
      m_pEffect(NULL),
      m_IsDamageVibrationContinue(false),
      m_DamageVibrationCount(0),
      m_IsGravityVibrationContinue(false),
      m_GravityVibrationCount(0),
      m_Acceleration(ACCELERATION__NONE),
      m_AccelerationDisplacement(0.0f){}
// Initialize
void Main::Initialize(
            const Rail::Manager* pRailManager,
            Effect::Manager* pEffectManager)
{
  m_pRailManager = pRailManager;
  LUNA_SOL_DIVE_ASSERT(m_pRailManager != NULL);
  m_pEffectManager = pEffectManager;
  LUNA_SOL_DIVE_ASSERT(m_pEffectManager != NULL);
}
// Terminate
void Main::Terminate()
{
  m_pRailManager = NULL;
  m_pEffectManager = NULL;
  m_CrossSectionPoint = gfl2::math::Vector2::GetZero();
  m_Directions.Terminate();
  GFL_SAFE_DELETE(m_pEffect);
  m_IsDamageVibrationContinue = false;
  m_DamageVibrationCount = 0;
  m_IsGravityVibrationContinue = false;
  m_GravityVibrationCount = 0;
  m_Acceleration = ACCELERATION__NONE;
  m_AccelerationDisplacement = 0.0f;
}
// Set Up
void Main::SetUp(
            const Rail::Location& location)
{
  UpdateImpl(
              location,
              2.0f);
}
// Update
void Main::Update(
            const Rail::Location& location)
{
  UpdateImpl(
              location,
              Parameter::Get().TraceMovement());
}
// Update: Implement
void Main::UpdateImpl(
            const Rail::Location& location,
            const f32& maxMovingRate)
{
  LUNA_SOL_DIVE_ASSERT(location.IsValid());
  // delay trace
  m_CrossSectionPoint = Trace(
              m_CrossSectionPoint,
              location.NormalizedCrossSectionPoint(),
              maxMovingRate);
  const gfl2::math::Vector2 cameraCrossSectionPoint = gfl2::math::Vector2(
              m_CrossSectionPoint.GetX() * Parameter::Get().TraceHorizontalRange(),
              m_CrossSectionPoint.GetY() * Parameter::Get().TraceVerticalRange());
  const Rail::Location cameraLocation = m_pRailManager->Location(
              0.0f,
              cameraCrossSectionPoint);
  const Rail::Location aimLocation = m_pRailManager->Location(
              0.0f,
              location.NormalizedCrossSectionPoint().SetX(0.0f));
  // view parameter
  gfl2::math::Vector3 eyePoint =
          cameraLocation.Point()
          + location.Forward() * Parameter::Get().CameraForward()
          + location.Vertical() * Parameter::Get().CameraVertical();
  const gfl2::math::Vector3 aimPoint =
          aimLocation.Point()
          + aimLocation.Forward() * Parameter::Get().TargetForward()
          + aimLocation.Vertical() * Parameter::Get().TargetVertical();
  // acceleration
  if(m_Acceleration != ACCELERATION__NONE)
  {
    const f32& kickstartRate = Parameter::Get().AccelerationKickstartRate();
    const f32& destination =
            (m_Acceleration == ACCELERATION__ACCELERATION)
            ? Parameter::Get().AccelerationDisplacement()
            : Parameter::Get().DecelerationDisplacement();
    const f32& endThreshold = Parameter::Get().AccelerationEndThreshold();
    m_AccelerationDisplacement +=
            kickstartRate * (destination - m_AccelerationDisplacement);
    if(gfl2::math::Abs(destination - m_AccelerationDisplacement)
                <= endThreshold * gfl2::math::Abs(destination))
    {
      m_Acceleration = ACCELERATION__NONE;
    }
  }
  else
  {
    const f32& attenuationRate = Parameter::Get().AccelerationAttenuationRate();
    m_AccelerationDisplacement =
            (gfl2::math::Abs(m_AccelerationDisplacement) < EPSILON)
            ? 0.0f
            : m_AccelerationDisplacement * attenuationRate;
  }
  eyePoint += location.Forward() * m_AccelerationDisplacement;
  // gravity vibration
  if(m_IsGravityVibrationContinue)
  {
    const f32 phase = PI2 * m_GravityVibrationCount / Parameter::Get().GravityVibrationPeriod();
    eyePoint +=
            location.Horizontal()
            * Parameter::Get().GravityVibrationAmplitude() * gfl2::math::SinRad(phase);
    ++m_GravityVibrationCount;
  }
  // direction
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero((aimPoint - eyePoint).Length()));
  const gfl2::math::Vector3 forward = (aimPoint - eyePoint).Normalize();
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(forward.Cross(location.Vertical()).Length()));
  gfl2::math::Vector3 vertical =
              (location.Vertical()
                  - forward * forward.Dot(location.Vertical()))
              .Normalize();
  // damage vibration
  if(m_IsDamageVibrationContinue)
  {
    const f32 time = m_DamageVibrationCount;
    const f32 amplitude =
            Parameter::Get().DamageVibrationAmplitude()
            * std::exp(- time / Parameter::Get().DamageVibrationDampingFactor());
    const f32 phase = PI2 * time / Parameter::Get().DamageVibrationPeriod();
    const gfl2::math::Quaternion damageVibration(
                forward,
                amplitude * gfl2::math::SinRad(phase));
    vertical = damageVibration.Transform(vertical);
    ++m_DamageVibrationCount;
    if(amplitude < 0.01f)
    {
      m_IsDamageVibrationContinue = false;
    }
  }
  // update directions
  m_Directions.Initialize(
            eyePoint,
            forward,
            vertical);
  // update effect
  if(m_pEffect != NULL && m_pEffect->IsAlive())
  {
    m_pEffect->Update(
                m_Directions.Point()
                        + m_Directions.Forward() * Parameter::Get().EffectDistance(),
                m_Directions.RotationQuaternion(),
                1.0f);
  }
}
// Damage
void Main::Damage()
{
  // create effect
  GFL_SAFE_DELETE(m_pEffect);
  m_pEffect = m_pEffectManager->Create(
              GARC_LunaSolDiveEffect_ea_dive_cam_hurdle_hit_lz_BPTCL);
  if(m_pEffect != NULL)
  {
    m_pEffect->Start();
  }
  m_IsDamageVibrationContinue = true;
  m_DamageVibrationCount = 0;
}
// Start Gravity Vibration
void Main::StartGravityVibration()
{
  if(! m_IsGravityVibrationContinue)
  {
    m_IsGravityVibrationContinue = true;
    m_GravityVibrationCount = 0;
  }
}
// Stop Gravity Vibration
void Main::StopGravityVibration()
{
  if(m_IsGravityVibrationContinue)
  {
    m_IsGravityVibrationContinue = false;
  }
}
// Accelerate
void Main::Accelerate()
{
  m_Acceleration = ACCELERATION__ACCELERATION;
}
// Decelerate
void Main::Decelerate()
{
  m_Acceleration = ACCELERATION__DECELERATION;
}
// Projection Matrix (override)
gfl2::math::Matrix44 Main::ProjectionMatrix(
            const gfl2::gfx::CtrDisplayNo& displayNo) const
{
  const gfl2::gfx::DisplayDeviceHandle& displayHandle = System::GflUse::m_HandleList[displayNo];
  const f32 aspect =
          (gfl2::gfx::GFGL::GetDisplayHeight(displayHandle) != 0)
          ? static_cast<f32>(gfl2::gfx::GFGL::GetDisplayWidth(displayHandle))
              / static_cast<f32>(gfl2::gfx::GFGL::GetDisplayHeight(displayHandle))
          : 0.0f;
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(aspect));
  return gfl2::math::Matrix44::GetPerspective(
              Parameter::Get().FieldOfViewY(),
              aspect,
              Parameter::Get().NearClip(),
              Parameter::Get().FarClip(),
              true);
}
// View Matrix (override)
gfl2::math::Matrix34 Main::ViewMatrix() const
{
  LUNA_SOL_DIVE_ASSERT(m_Directions.IsValid());
  return gfl2::math::Matrix34::GetLookAt(
              m_Directions.Point(),
              m_Directions.Point() + m_Directions.Forward(),
              m_Directions.Vertical());
}
// Directions (override)
const Rail::Directions& Main::Directions() const
{
  return m_Directions;
}
// Translation
void Main::Translation(
            const gfl2::math::Vector3& displacement)
{
  m_Directions.Translation(displacement);
  // translation of effects is executed by Effect::Manager
}
// static function
// Trace
static gfl2::math::Vector2 Trace(
            const gfl2::math::Vector2& location,
            const gfl2::math::Vector2& destination,
            const f32 maxMovement)
{
  return ((destination - location).Length() > maxMovement)
          ? location + (destination - location).Normalize() * maxMovement
          : destination;
}
}// namespace Camera
}// namespace LunaSolDive
