//======================================================================
/**
 * @file Run_InputCTR.cpp
 * @date 2016/09/29 19:51:09
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#ifdef GF_PLATFORM_CTR
#include <debug/include/gfl2_DebugPrint.h>
#include <math/include/gfl2_MathTriangular.h>
#include <ui/include/gfl2_UI_Accelerometer.h>
#include <ui/include/gfl2_UI_Button.h>
#include <ui/include/gfl2_UI_DeviceManager.h>
#include <ui/include/gfl2_UI_Gyroscope.h>
#include <ui/include/gfl2_UI_VectorDevice.h>
#include "./InputCTR.h"
#include "./Parameter.h"
#include "../Assert.h"
#include "../Sound/Manager.h"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Run{
// constructor
InputCTR::InputCTR()
    : Input(),
      m_Maneuver(MANEUVER__NONE),
      m_pAccelerometer(NULL),
      m_pGyroscope(NULL),
      m_Aim(gfl2::math::Vector2::GetZero()),
      m_DirectionConvertMatrix(gfl2::math::Matrix34::GetZero()),
      m_DirectionX(gfl2::math::Vector3::GetZero()),
      m_DirectionY(gfl2::math::Vector3::GetZero()),
      m_DirectionZ(gfl2::math::Vector3::GetZero()),
      m_YawAngle(0.0f),
      m_PitchAngle(0.0f),
      m_RollAngle(0.0f),
      m_Acceleration(gfl2::math::Vector3::GetZero()),
      m_IsInitialDirectionSet(false),
      m_IsInResetProcess(false),
      m_ResetProcessCount(0),
      m_ResetIntervalCount(0),
      m_ResetSignal(RESET_SIGNAL__NONE){}
// Initialize (override)
void InputCTR::Initialize(
            const Maneuver& maneuver,
            gfl2::heap::HeapBase* pHeap,
            gfl2::ui::DeviceManager* pDeviceManager)
{
  Input::Initialize(
              maneuver,
              pHeap,
              pDeviceManager);
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  m_Maneuver = maneuver;
  LUNA_SOL_DIVE_ASSERT(m_Maneuver != MANEUVER__NONE);
  if(m_Maneuver == MANEUVER__GYROSCOPE)
  {
    // create AccelerometerReader
    DeviceManager().BootAccelerometer(
                pHeap,
                gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD);
    // create GyroscopeReader
    DeviceManager().BootGyroscope(
                pHeap,
                gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);
    // set Accelerometer
    LUNA_SOL_DIVE_ASSERT(m_pAccelerometer == NULL);
    m_pAccelerometer = DeviceManager().GetAccelerometer(
                gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD);
    LUNA_SOL_DIVE_ASSERT(m_pAccelerometer != NULL);
    // set Gyroscope pointer
    LUNA_SOL_DIVE_ASSERT(m_pGyroscope == NULL);
    m_pGyroscope = DeviceManager().GetGyroscope(
                gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);
    LUNA_SOL_DIVE_ASSERT(m_pGyroscope != NULL);
    // disable: Zero Play
    m_pGyroscope->DisableZeroPlayParam();
    // enable: Zero Play Drift
    m_pGyroscope->EnableZeroDrift();
    // enable: Acceleration Revision
    m_pGyroscope->EnableRevisionByAcceleration();
  }
}
// Terminate (override)
void InputCTR::Terminate()
{
  if(m_Maneuver == MANEUVER__GYROSCOPE)
  {
    // delete GyroscopeReader
    DeviceManager().StopGyroscope(
                gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);
    // delete AccelerometerReader
    DeviceManager().StopAccelerometer(
                gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD);
    // unset pointer
    m_pAccelerometer = NULL;
    m_pGyroscope = NULL;
  }
  // reset parameter
  m_Maneuver = MANEUVER__NONE;
  m_Aim = gfl2::math::Vector2::GetZero();
  m_DirectionConvertMatrix = gfl2::math::Matrix34::GetZero();
  m_DirectionX = gfl2::math::Vector3::GetZero();
  m_DirectionY = gfl2::math::Vector3::GetZero();
  m_DirectionZ = gfl2::math::Vector3::GetZero();
  m_Acceleration = gfl2::math::Vector3::GetZero();
  m_PitchAngle = 0.0f;
  m_YawAngle = 0.0f;
  m_RollAngle = 0.0f;
  m_IsInitialDirectionSet = false;
  m_IsInResetProcess = false;
  m_ResetProcessCount = 0;
  m_ResetIntervalCount = 0;
  m_ResetSignal = RESET_SIGNAL__NONE;
  // terminate Base
  Input::Terminate();
}
// Update (override)
void InputCTR::Update()
{
  LUNA_SOL_DIVE_ASSERT(m_Maneuver != MANEUVER__NONE);
#if PM_DEBUG
  if(m_Maneuver == MANEUVER__STICK
          || Debug::Parameter::Get().isStickMode)
#else
  if(m_Maneuver == MANEUVER__STICK)
#endif// PM_DEBUG
  {// Stick Maneuver
    UpdateStick();
  }
  else
  {// Gyroscope Maneuver
    UpdateGyroscope();
  }
}
// Update: GyroscopeManeuver
void InputCTR::UpdateGyroscope()
{
  LUNA_SOL_DIVE_ASSERT(m_Maneuver == MANEUVER__GYROSCOPE);
  LUNA_SOL_DIVE_ASSERT(m_pGyroscope != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pAccelerometer != NULL);
  // check if Gyroscope is valid
  if(! m_pGyroscope->IsValid())
  {
    return;
  }
  // check whether initial direction is set
  if(! m_IsInitialDirectionSet) return;
  // update Acceleration (excluding gravitational acceleration)
  gfl2::math::VEC3 tempAcceleration;
  m_pAccelerometer->GetAcceleration(&tempAcceleration);
  m_Acceleration = gfl2::math::Vector3(
              tempAcceleration.x + m_pGyroscope->GetDirectionOfX().y,
              tempAcceleration.y + m_pGyroscope->GetDirectionOfY().y,
              tempAcceleration.z + m_pGyroscope->GetDirectionOfZ().y);
#if PM_DEBUG
  // output raw data
  Debug::Parameter::Get().gyroscopeSpeed = gfl2::math::Vector3(
              m_pGyroscope->GetSpeed().x,
              m_pGyroscope->GetSpeed().y,
              m_pGyroscope->GetSpeed().z);
  Debug::Parameter::Get().gyroscopeRawAngle = gfl2::math::Vector3(
              m_pGyroscope->GetAngle().x,
              m_pGyroscope->GetAngle().y,
              m_pGyroscope->GetAngle().z);
  Debug::Parameter::Get().gyroscopeRawDirectionX = gfl2::math::Vector3(
              m_pGyroscope->GetDirectionOfX().x,
              m_pGyroscope->GetDirectionOfX().y,
              m_pGyroscope->GetDirectionOfX().z);
  Debug::Parameter::Get().gyroscopeRawDirectionY = gfl2::math::Vector3(
              m_pGyroscope->GetDirectionOfY().x,
              m_pGyroscope->GetDirectionOfY().y,
              m_pGyroscope->GetDirectionOfY().z);
  Debug::Parameter::Get().gyroscopeRawDirectionZ = gfl2::math::Vector3(
              m_pGyroscope->GetDirectionOfZ().x,
              m_pGyroscope->GetDirectionOfZ().y,
              m_pGyroscope->GetDirectionOfZ().z);
  Debug::Parameter::Get().gyroscopeRawAcceleration = gfl2::math::Vector3(
              tempAcceleration.x,
              tempAcceleration.y,
              tempAcceleration.z);
#endif// PM_DEBUG
  // reset Initial Directions
  m_ResetSignal = RESET_SIGNAL__NONE;
  if(m_ResetIntervalCount > 0)
  {
    --m_ResetIntervalCount;
  }
  if(! m_IsInResetProcess
          && m_ResetIntervalCount == 0)
  {
    if(Button().IsTrigger(gfl2::ui::BUTTON_A))
    {
      GFL_PRINT("============ Reset Gyroscope Directions: begin ============\n");
      m_IsInResetProcess = true;
      m_ResetProcessCount = 0;
      m_ResetSignal = RESET_SIGNAL__PUSH;
      // play pokemon voice
      Sound::Manager::Get().PlayPokemonVoice();
    }
  }
  if(m_IsInResetProcess)
  {
    ++m_ResetProcessCount;
    if(m_ResetProcessCount > Parameter::Get().ResetWaitFrame())
    {
      GFL_PRINT("============ Reset Gyroscope Directions: end   ============\n");
      SetInitialDirection();
      m_IsInResetProcess = false;
      m_ResetIntervalCount = Parameter::Get().ResetIntervalFrame();
    }
    else
    {
      m_Aim = gfl2::math::Vector2::GetZero();
      return;
    }
  }
  // update Direction
  m_DirectionX =
          m_DirectionConvertMatrix
          * gfl2::math::Vector3(
                        m_pGyroscope->GetDirectionOfX().x,
                        m_pGyroscope->GetDirectionOfX().y,
                        m_pGyroscope->GetDirectionOfX().z);
  m_DirectionY =
          m_DirectionConvertMatrix
          * gfl2::math::Vector3(
                        m_pGyroscope->GetDirectionOfY().x,
                        m_pGyroscope->GetDirectionOfY().y,
                        m_pGyroscope->GetDirectionOfY().z);
  m_DirectionZ =
          m_DirectionConvertMatrix
          * gfl2::math::Vector3(
                        m_pGyroscope->GetDirectionOfZ().x,
                        m_pGyroscope->GetDirectionOfZ().y,
                        m_pGyroscope->GetDirectionOfZ().z);
  // update Angle
  /*
  Rotation Matrix (Order: Pitch(θ) -> Yaw(φ) -> Roll(ψ))
  ⎡ cos(φ)cos(ψ)     sin(θ)sin(φ)cos(ψ) + cos(θ)sin(ψ)    -cos(θ)sin(φ)cos(ψ)⋅+ sin(θ)sin(ψ)⎤
  ⎢                                                                                                     ⎥
  ⎢-cos(φ)sin(ψ)    -sin(θ)sin(φ)sin(ψ) + cos(θ)cos(ψ)     cos(θ)sin(φ)sin(ψ) + sin(θ)cos(ψ)⎥
  ⎢                                                                                                     ⎥
  ⎣ sin(φ)           -sin(θ)cos(φ)                             cos(θ)cos(φ)⋅                       ⎦
  */
  m_PitchAngle = gfl2::math::Atan2Rad(
              - m_DirectionZ.GetY(),
              m_DirectionZ.GetZ());
  m_YawAngle = gfl2::math::Atan2Rad(
              m_DirectionZ.GetX(),
              std::hypot(m_DirectionZ.GetY(), m_DirectionZ.GetZ()));
  m_RollAngle = gfl2::math::Atan2Rad(
              - m_DirectionY.GetX(),
              m_DirectionX.GetX());
#if PM_DEBUG
  // output adjusted data
  Debug::Parameter::Get().gyroscopeAdjustedAngle = gfl2::math::Vector3(
              m_PitchAngle,
              m_YawAngle,
              m_RollAngle);
  Debug::Parameter::Get().gyroscopeAdjustedDirectionX = m_DirectionX;
  Debug::Parameter::Get().gyroscopeAdjustedDirectionY = m_DirectionY;
  Debug::Parameter::Get().gyroscopeAdjustedDirectionZ = m_DirectionZ;
  Debug::Parameter::Get().gyroscopeAdjustedAcceleration = m_Acceleration;
#endif// PM_DEBUG
  // update Aim
  m_Aim = gfl2::math::Vector2(
              gfl2::math::Clamp(
                      - m_YawAngle / Parameter::Get().YawAngleRange(),
                      -1.0f,
                      1.0f),
              gfl2::math::Clamp(
                      - m_PitchAngle / Parameter::Get().PitchAngleRange(),
                      -1.0f,
                      1.0f));
  if(m_Aim.Length() > 1.0f)
  {
    m_Aim = m_Aim.Normalize();
  }
  LUNA_SOL_DIVE_ASSERT(m_Aim.Length() < 1.0f + EPSILON);
}
// Update: Stick Maneuver
void InputCTR::UpdateStick()
{
  m_Aim = gfl2::math::Vector2(
              Stick().GetX(),
              Stick().GetY());
  if(m_Aim.Length() > 1.0f)
  {
    m_Aim = m_Aim.Normalize();
  }
  LUNA_SOL_DIVE_ASSERT(m_Aim.Length() <1.0f + EPSILON);
}
// Aim (override)
const gfl2::math::Vector2& InputCTR::Aim() const
{
  return m_Aim;
}
// Set Initial Direction (override)
void InputCTR::SetInitialDirection()
{
  if(m_Maneuver == MANEUVER__GYROSCOPE)
  {
    m_DirectionConvertMatrix = gfl2::math::Matrix34::GetZero();
    m_DirectionConvertMatrix.SetElem(0, 0, m_pGyroscope->GetDirectionOfX().x);
    m_DirectionConvertMatrix.SetElem(0, 1, m_pGyroscope->GetDirectionOfX().y);
    m_DirectionConvertMatrix.SetElem(0, 2, m_pGyroscope->GetDirectionOfX().z);
    m_DirectionConvertMatrix.SetElem(1, 0, m_pGyroscope->GetDirectionOfY().x);
    m_DirectionConvertMatrix.SetElem(1, 1, m_pGyroscope->GetDirectionOfY().y);
    m_DirectionConvertMatrix.SetElem(1, 2, m_pGyroscope->GetDirectionOfY().z);
    m_DirectionConvertMatrix.SetElem(2, 0, m_pGyroscope->GetDirectionOfZ().x);
    m_DirectionConvertMatrix.SetElem(2, 1, m_pGyroscope->GetDirectionOfZ().y);
    m_DirectionConvertMatrix.SetElem(2, 2, m_pGyroscope->GetDirectionOfZ().z);
    // set flag
    m_IsInitialDirectionSet = true;
  }
}
// Reset Signal (override)
ResetSignal InputCTR::ResetSignal() const
{
  return m_ResetSignal;
}
}// namespace Run
}// namespace LunaSolDive
#endif// GF_PLATFORM_CTR
