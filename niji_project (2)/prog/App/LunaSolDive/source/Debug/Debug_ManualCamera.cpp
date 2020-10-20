//======================================================================
/**
 * @file ManualCamera.cpp
 * @date 2017/01/10 12:23:51
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#include <math/include/gfl2_Matrix44.h>
#include "System/include/GflUse.h"
#include "./ManualCamera.h"
#include "../Assert.h"
#include "../Camera/Parameter.h"

namespace LunaSolDive{
namespace Debug{
// constructor
ManualCamera::ManualCamera()
    : Base(),
      m_PitchAngle(0.0f),
      m_YawAngle(0.0f),
      m_Distance(0.0f){}
// Initialize
void ManualCamera::Initialize(
            const Rail::Directions& directions)
{
  LUNA_SOL_DIVE_ASSERT(directions.IsValid());
  m_InitialDirections = directions;
  m_PresentDirections = m_InitialDirections;
  m_PitchAngle = 0.0f;
  m_YawAngle = 0.0f;
  m_Distance = 500.0f;
  m_Displacement = gfl2::math::Vector3::GetZero();
}
// Terminate
void ManualCamera::Terminate()
{
  m_InitialDirections.Terminate();
  m_PresentDirections.Terminate();
  m_PitchAngle = 0.0f;
  m_YawAngle = 0.0f;
  m_Distance = 0.0f;
  m_Displacement = gfl2::math::Vector3::GetZero();
}
// Update
void ManualCamera::Update(
            const gfl2::ui::Button& button,
            const gfl2::math::Vector2& stick)
{
  // parameter
  const f32 angleMove = gfl2::math::ConvDegToRad(1.0f);
  const f32 distanceMove = 5.0f;
  const f32 translationMove = 10.0f;
  // update Angle
  m_PitchAngle += stick.GetY() * - angleMove;
  m_YawAngle += stick.GetX() * angleMove;
  // update Distance
  if(button.IsHold(gfl2::ui::BUTTON_A))
  {
    m_Distance += distanceMove;
  }
  if(button.IsHold(gfl2::ui::BUTTON_Y))
  {
    m_Distance -= distanceMove;
  }
  // rotation
  const gfl2::math::Quaternion pitchRotation(
              m_InitialDirections.Horizontal(),
              m_PitchAngle);
  const gfl2::math::Quaternion yawRotation(
              m_InitialDirections.Vertical(),
              m_YawAngle);
  // direction
  const gfl2::math::Vector3 forwardDirection =
          yawRotation.Transform(
                  pitchRotation.Transform(m_InitialDirections.Forward()));
  const gfl2::math::Vector3 verticalDirection =
          yawRotation.Transform(
                  pitchRotation.Transform(m_InitialDirections.Vertical()));
  const gfl2::math::Vector3 horizontalDirection =
          yawRotation.Transform(
                  pitchRotation.Transform(m_InitialDirections.Horizontal()));
  // transform
  if(button.IsHold(gfl2::ui::BUTTON_UP))
  {
    m_Displacement += verticalDirection * translationMove;
  }
  if(button.IsHold(gfl2::ui::BUTTON_DOWN))
  {
    m_Displacement -= verticalDirection * translationMove;
  }
  if(button.IsHold(gfl2::ui::BUTTON_RIGHT))
  {
    m_Displacement += horizontalDirection * translationMove;
  }
  if(button.IsHold(gfl2::ui::BUTTON_LEFT))
  {
    m_Displacement -= horizontalDirection * translationMove;
  }
  if(button.IsHold(gfl2::ui::BUTTON_X))
  {
    m_Displacement += forwardDirection * translationMove;
  }
  if(button.IsHold(gfl2::ui::BUTTON_B))
  {
    m_Displacement -= forwardDirection * translationMove;
  }
  // update directions
  m_PresentDirections.Initialize(
              m_InitialDirections.Point() + m_Displacement,
              forwardDirection,
              verticalDirection);
}
// Projection Matrix (override)
gfl2::math::Matrix44 ManualCamera::ProjectionMatrix(
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
              Camera::Parameter::Get().FieldOfViewY(),
              aspect,
              Camera::Parameter::Get().NearClip(),
              Camera::Parameter::Get().FarClip(),
              true);
}
// View Matrix (override)
gfl2::math::Matrix34 ManualCamera::ViewMatrix() const
{
  return gfl2::math::Matrix34::GetLookAt(
              m_PresentDirections.Point()
                      - m_PresentDirections.Forward() * m_Distance,
              m_PresentDirections.Point(),
              m_PresentDirections.Vertical());
}
// Directions (override)
const Rail::Directions& ManualCamera::Directions() const
{
  return m_PresentDirections;
}
}// namespace Debug
}// namespace LunaSolDive
#endif// PM_DEBUG
