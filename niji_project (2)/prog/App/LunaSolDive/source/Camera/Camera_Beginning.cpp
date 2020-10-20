//======================================================================
/**
 * @file Camera_Beginning.cpp
 * @date 2017/01/06 13:10:25
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <math/include/gfl2_Matrix44.h>
#include "System/include/GflUse.h"
#include "./Beginning.h"
#include "./Parameter.h"
#include "../Assert.h"
#include "../Sound/Manager.h"
#include "../Sound/Parameter.h"

namespace LunaSolDive{
namespace Camera{
// static function
// Overtaken Operation
static gfl2::math::Vector3 Overtaken(
            const f32& progressRate,
            const gfl2::math::Vector3& begin,
            const gfl2::math::Vector3& end);
// calculate Directions
static Rail::Directions ToDirections(
            const gfl2::math::Vector3& eyePoint,
            const gfl2::math::Vector3& aimPoint,
            const gfl2::math::Vector3& upwardDirection);
// constructor
Beginning::Beginning()
    : Base(),
      m_State(STATE__INITIAL),
      m_FrameCount(0),
      m_EyePoint(gfl2::math::Vector3::GetZero()),
      m_AimPoint(gfl2::math::Vector3::GetZero())
{
  m_EyeRelayPoint.initial = gfl2::math::Vector3::GetZero();
  m_EyeRelayPoint.entry_end = gfl2::math::Vector3::GetZero();
  m_EyeRelayPoint.zoom_in_start = gfl2::math::Vector3::GetZero();
  m_EyeRelayPoint.terminate = gfl2::math::Vector3::GetZero();
  m_AimRelayPoint.initial = gfl2::math::Vector3::GetZero();
  m_AimRelayPoint.entry_end = gfl2::math::Vector3::GetZero();
  m_AimRelayPoint.zoom_in_start = gfl2::math::Vector3::GetZero();
  m_AimRelayPoint.terminate = gfl2::math::Vector3::GetZero();
}
// Initialize
void Beginning::Initialize(
            const Version& version)
{
  LUNA_SOL_DIVE_ASSERT(version != VERSION__NONE);
  // initialize Relay Point
  if(version == VERSION__SOL)
  {// Sol version
    m_EyeRelayPoint.initial = gfl2::math::Vector3(409.228f, 104.529f, 747.54f);
    m_AimRelayPoint.initial = gfl2::math::Vector3(-24.016f, 96.458f, 612.135f);
    m_EyeRelayPoint.entry_end = gfl2::math::Vector3(409.228f, 104.529f, 231.3f);
    m_AimRelayPoint.entry_end = gfl2::math::Vector3(-24.016f, 96.458f, 95.895f);
    m_EyeRelayPoint.zoom_in_start = gfl2::math::Vector3(0.0f, 249.448f, -4007.1f);
    m_AimRelayPoint.zoom_in_start = gfl2::math::Vector3(0.0f, 97.724f, 558.126f);
    m_EyeRelayPoint.terminate = gfl2::math::Vector3(
                0.0f,
                Parameter::Get().CameraVertical(),
                Parameter::Get().CameraForward());
    m_AimRelayPoint.terminate = gfl2::math::Vector3(
                0.0f,
                Parameter::Get().TargetVertical(),
                Parameter::Get().TargetForward());
  }
  else
  {// Luna version
    m_EyeRelayPoint.initial = gfl2::math::Vector3(253.007f, 142.965f, 1402.08f);
    m_AimRelayPoint.initial = gfl2::math::Vector3(-90.019f, 51.626f, 1221.8f);
    m_EyeRelayPoint.entry_end = gfl2::math::Vector3(348.763f, 184.353f, 201.358f);
    m_AimRelayPoint.entry_end = gfl2::math::Vector3(-11.414f, 88.448f, 12.064f);
    m_EyeRelayPoint.zoom_in_start = gfl2::math::Vector3(0.0f, 209.163f, -4007.1f);
    m_AimRelayPoint.zoom_in_start = gfl2::math::Vector3(0.0f, 97.724f, 558.125f);
    m_EyeRelayPoint.terminate = gfl2::math::Vector3(
                0.0f,
                Parameter::Get().CameraVertical(),
                Parameter::Get().CameraForward());
    m_AimRelayPoint.terminate = gfl2::math::Vector3(
                0.0f,
                Parameter::Get().TargetVertical(),
                Parameter::Get().TargetForward());
  }
  // set state
  m_State = STATE__ENTRY;
}
// Terminate
void Beginning::Terminate()
{
  m_State = STATE__INITIAL;
  m_FrameCount = 0;
  m_EyeRelayPoint.initial = gfl2::math::Vector3::GetZero();
  m_EyeRelayPoint.entry_end = gfl2::math::Vector3::GetZero();
  m_EyeRelayPoint.zoom_in_start = gfl2::math::Vector3::GetZero();
  m_EyeRelayPoint.terminate = gfl2::math::Vector3::GetZero();
  m_AimRelayPoint.initial = gfl2::math::Vector3::GetZero();
  m_AimRelayPoint.entry_end = gfl2::math::Vector3::GetZero();
  m_AimRelayPoint.zoom_in_start = gfl2::math::Vector3::GetZero();
  m_AimRelayPoint.terminate = gfl2::math::Vector3::GetZero();
  m_PlayerLocation.Terminate();
  m_EyePoint = gfl2::math::Vector3::GetZero();
  m_AimPoint = gfl2::math::Vector3::GetZero();
  m_Directions.Terminate();
}
// Update
void Beginning::Update(
            const Rail::Directions& location)
{
  ARAI_PRINT("BeginningCamera: %s, %03d\n",
              (m_State == STATE__ENTRY)? "Entry":
              (m_State == STATE__ENTRY_END)? "EntryEnd":
              (m_State == STATE__OVERTAKEN)? "Overtaken":
              (m_State == STATE__ZOOM_IN)? "ZoomIn":
              "Error",
              m_FrameCount);
  // update Player Location
  m_PlayerLocation = location;
  // update view parameter
  if(m_State == STATE__ENTRY)
  {// Entry
    const u32& totalFrames = Parameter::Get().BeginningEntryFrames();
    LUNA_SOL_DIVE_ASSERT(totalFrames > 1);
    const f32 progressRate = static_cast<f32>(m_FrameCount) / (totalFrames - 1);
    m_EyePoint =
            m_EyeRelayPoint.initial * (1.0f - progressRate)
            + m_EyeRelayPoint.entry_end * progressRate;
    m_AimPoint =
            m_AimRelayPoint.initial * (1.0f - progressRate)
            + m_AimRelayPoint.entry_end * progressRate;
    ++m_FrameCount;
    if(m_FrameCount >= totalFrames)
    {// change: Entry -> Entry End
      m_State = STATE__ENTRY_END;
      m_FrameCount = 0;
    }
  }
  else if(m_State == STATE__ENTRY_END)
  {// Entry End
    const u32& totalFrames = Parameter::Get().BeginningEntryEndFrames();
    LUNA_SOL_DIVE_ASSERT(totalFrames > 0);
    m_EyePoint = m_EyeRelayPoint.entry_end;
    m_AimPoint = m_AimRelayPoint.entry_end;
    ++m_FrameCount;
    if(m_FrameCount >= totalFrames)
    {// change: Entry End -> Overtaken
      m_State = STATE__OVERTAKEN;
      m_FrameCount = 0;
    }
  }
  else if(m_State == STATE__OVERTAKEN)
  {// Overtaken
    const u32& totalFrames = Parameter::Get().BeginningOvertakenFrames();
    LUNA_SOL_DIVE_ASSERT(totalFrames > 1);
    const f32 progressRate = static_cast<f32>(m_FrameCount) / (totalFrames - 1);
    m_EyePoint = Overtaken(
                progressRate,
                m_EyeRelayPoint.entry_end,
                m_EyeRelayPoint.zoom_in_start);
    m_AimPoint = Overtaken(
                progressRate,
                m_AimRelayPoint.entry_end,
                m_AimRelayPoint.zoom_in_start);
    // register BGM volume control
    if(m_FrameCount + Sound::Parameter::Get().VolumeControlActivateEnergyAura().down_frames == totalFrames)
    {
      Sound::Manager::Get().RegisterVolumeControl(
                  Sound::Parameter::Get().VolumeControlActivateEnergyAura());
    }
    ++m_FrameCount;
    if(m_FrameCount >= totalFrames)
    {// change: Overtaken -> Zoom In
      m_State = STATE__ZOOM_IN;
      m_FrameCount = 0;
    }
  }
  else if(m_State == STATE__ZOOM_IN)
  {// Zoom In
    const u32& totalFrames = Parameter::Get().BeginningZoomInFrames();
    LUNA_SOL_DIVE_ASSERT(totalFrames > 1);
    const f32 progressRate = static_cast<f32>(m_FrameCount) / (totalFrames - 1);
    m_EyePoint =
            m_EyeRelayPoint.zoom_in_start * (1.0f - progressRate)
            + m_EyeRelayPoint.terminate * progressRate;
    m_AimPoint =
            m_AimRelayPoint.zoom_in_start * (1.0f - progressRate)
            + m_AimRelayPoint.terminate * progressRate;
    ++m_FrameCount;
    if(m_FrameCount >= totalFrames)
    {// change: Zoom In -> Terminate
      m_State = STATE__TERMINATE;
      m_FrameCount = 0;
    }
  }
  // update directions
  m_Directions = ToDirections(
              EyePoint(),
              AimPoint(),
              m_PlayerLocation.Vertical());
}
// Projection Matrix (override)
gfl2::math::Matrix44 Beginning::ProjectionMatrix(
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
gfl2::math::Matrix34 Beginning::ViewMatrix() const
{
  LUNA_SOL_DIVE_ASSERT(m_PlayerLocation.IsValid());
  return gfl2::math::Matrix34::GetLookAt(
              EyePoint(),
              AimPoint(),
              m_PlayerLocation.Vertical());
}
// Directions (override)
const Rail::Directions& Beginning::Directions() const
{
  return m_Directions;
}
// Whether camera operation has been finished
bool Beginning::IsFinished() const
{
  return m_State == STATE__TERMINATE;
}
// Whether camera is zooming in or not
bool Beginning::IsZoomIn() const
{
  return m_State == STATE__ZOOM_IN;
}
// Number of frames in camera operation
u32 Beginning::TotalFrameNumber() const
{
  return Parameter::Get().BeginningEntryFrames()
          + Parameter::Get().BeginningEntryEndFrames()
          + Parameter::Get().BeginningOvertakenFrames()
          + Parameter::Get().BeginningZoomInFrames();
}
// Eye Point
gfl2::math::Vector3 Beginning::EyePoint() const
{
  return m_PlayerLocation.Point()
          + m_PlayerLocation.RotationQuaternion().Transform(m_EyePoint);
}
// Aim Point
gfl2::math::Vector3 Beginning::AimPoint() const
{
  return m_PlayerLocation.Point()
          + m_PlayerLocation.RotationQuaternion().Transform(m_AimPoint);
}
// static function
// Overtaken Operation
static gfl2::math::Vector3 Overtaken(
            const f32& progressRate,
            const gfl2::math::Vector3& begin,
            const gfl2::math::Vector3& end)
{
  LUNA_SOL_DIVE_ASSERT(- EPSILON < progressRate && progressRate < 1.0f + EPSILON);
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(begin.Length()));
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(end.Length()));
  // parameter
  const f32& angleIncreasingRate = Parameter::Get().BeginningOvertakenAngleIncreasingRate();
  const f32& radiusIncreasingRate = Parameter::Get().BeginningOvertakenRadiusIncreasingRate();
  LUNA_SOL_DIVE_ASSERT(angleIncreasingRate > 0.0f);
  LUNA_SOL_DIVE_ASSERT(radiusIncreasingRate > 0.0f);
  // x,z element
  const gfl2::math::Vector3 beginXZ = gfl2::math::Vector3(begin).SetY(0.0f);
  const gfl2::math::Vector3 endXZ = gfl2::math::Vector3(end).SetY(0.0f);
  // normalized vector
  const gfl2::math::Vector3 normalizedBeginXZ = beginXZ.Normalize();
  const gfl2::math::Vector3 normalizedEndXZ = endXZ.Normalize();
  // angle displacement
  const f32 sin = gfl2::math::Clamp(
              normalizedBeginXZ.Cross(normalizedEndXZ).GetY(),
              -1.0f,
              1.0f);
  const f32 cos = gfl2::math::Clamp(
              normalizedBeginXZ.Dot(normalizedEndXZ),
              -1.0f,
              1.0f);
  const f32 angleDisplacement =
              (sin > 0.0f)
              ? gfl2::math::AcosRad(cos)
              : PI2 - gfl2::math::AcosRad(cos);
  // angle
  const f32 angleMovementRate =
          (1.0f - std::exp(- angleIncreasingRate * progressRate))
          / (1.0f - std::exp(- angleIncreasingRate));
  const f32 angle =
          angleDisplacement * angleMovementRate;
  // rotation quaternion
  gfl2::math::Quaternion rotationQuaternion(
              gfl2::math::Vector3::GetYUnit(),
              angle);
  // radius
  const f32 beginRadius = beginXZ.Length();
  const f32 endRadius = endXZ.Length();
  const f32 radiusMovementRate =
          (std::exp(radiusIncreasingRate * progressRate) - 1.0f)
          / (std::exp(radiusIncreasingRate) - 1.0f);
  const f32 radius =
          beginRadius
          + (endRadius - beginRadius) * radiusMovementRate;
  // y element
  const f32 y =
          (1.0f - progressRate) * begin.GetY()
          + progressRate * end.GetY();
  return rotationQuaternion.Transform(normalizedBeginXZ) * radius
          + gfl2::math::Vector3::GetYUnit() * y;
}
// Calculate Directions
static Rail::Directions ToDirections(
            const gfl2::math::Vector3& eyePoint,
            const gfl2::math::Vector3& aimPoint,
            const gfl2::math::Vector3& upwardDirection)
{
  const gfl2::math::Vector3 forwardVector = aimPoint - eyePoint;
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(forwardVector.Length()));
  const gfl2::math::Vector3 forwardDirection = forwardVector.Normalize();
  const gfl2::math::Vector3 verticalVector =
              upwardDirection
              - forwardDirection * forwardDirection.Dot(upwardDirection);
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(verticalVector.Length()));
  const gfl2::math::Vector3 verticalDirection = verticalVector.Normalize();
  return Rail::Directions(
              eyePoint,
              forwardDirection,
              verticalDirection);
}
}// namespace Camera
}// namespace LunaSolDive
