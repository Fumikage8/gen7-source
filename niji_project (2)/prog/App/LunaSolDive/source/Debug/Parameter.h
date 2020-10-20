//======================================================================
/**
 * @file Parameter.h
 * @date 2016/12/26 12:41:41
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#if !defined __LUNA_SOL_DIVE__DEBUG__PARAMETER_H_INCLUDED__
#define __LUNA_SOL_DIVE__DEBUG__PARAMETER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Vector3.h>
#include "App/LunaSolDive/include/DistortionType.h"
#include "../Rail/PartShape.h"

namespace LunaSolDive{
// forward declaration
namespace Camera{
class Parameter;
}// namespace Camera
namespace Gimmick{
class Parameter;
namespace Placement{
class Parameter;
}// namespace Placement
}// namespace Gimmick
namespace Player{
class Parameter;
}// namespace Player
namespace Rail{
class Parameter;
}// namespace Rail
namespace Run{
class Parameter;
}// namespace Run
namespace Sound{
class Parameter;
}// namespace Sound

namespace Debug{
// forward declaration
class DrawUtil;
class ManualCamera;
class Timer;

class Parameter
{
  GFL_FORBID_COPY_AND_ASSIGN(Parameter);
public:
  // Initialize
  static void Initialize();
  // Terminate
  static void Terminate();
  // getter
  static Parameter& Get();
  // Debug Mode
  bool isDebugMode;
  // Travel Distance
  f32 travelDistance;
  // Energy
  f32 energy;
  // Limit of Energy
  f32 energyLimit;
  // Forward Speed
  f32 forwardSpeed;
  // Moving Rate on the Vertical Plane
  f32 verticalSpeed;
  // Aim Input Point
  gfl2::math::Vector2 aimInput;
  // Aim Point
  gfl2::math::Vector2 aimPoint;
  // Mode: Stick Mode
  bool isStickMode;
  // Mode: Manually Energy Up/Down
  bool isManuallyEnergyUpDownEnabled;
  // Mode: Constant Energy
  bool isConstantEnergyMode;
  // Mode: Energy value in constant energy mode
  f32 energyInConstantEnergyMode;
  // Mode: Fixed Distortion Type
  bool isDistortionTypeFixed;
  // Mode: Distortion Type in fixed mode
  DistortionType distortionTypeInFixedMode;
  // Mode: Fixed Distortion Rarity mode
  bool isDistortionRarityFixed;
  // Mode: Distortion Rarity in fixed mode
  DistortionRarity distortionRarityInFixedMode;
  // Mode: Set Placement Probability to 0
  bool isPlacementProbabilitySet0;
  // Mode: Set Placement Probability to 1
  bool isPlacementProbabilitySet1;
  // Mode: Fixed Rail Shape
  bool isRailShapeFixed;
  // Mode: Rail Shape in fixed mode
  Rail::PartShape railShapeInFixedMode;
  // Mode: Fixed Rail Angle
  bool isRailAngleFixed;
  // Mode: Rail Angle in fixed mode
  f32 railAngleInFixedMode;
  // Mode: Overwrite Output Distance
  bool isOutputDistanceOverwritten;
  // Mode: Distance used for Overwriting
  u32 distanceForOverwriting;
  // Mode: Manual Camera Enable
  bool isManualCameraEnable;
  // Mode: random dress up
  bool isRandomDressUp;
  // Mode: enable assert
  bool isAssertEnabled;
  // initial value: gfl2::debug::GetAssertEnable()
  bool initialIsAssertEnabled;
  // Gyroscope: Speed
  gfl2::math::Vector3 gyroscopeSpeed;
  // Gyroscope: Angle (raw)
  gfl2::math::Vector3 gyroscopeRawAngle;
  // Gyroscope: Direction x (raw)
  gfl2::math::Vector3 gyroscopeRawDirectionX;
  // Gyroscope: Direction y (raw)
  gfl2::math::Vector3 gyroscopeRawDirectionY;
  // Gyroscope: Direction x (raw)
  gfl2::math::Vector3 gyroscopeRawDirectionZ;
  // Gyroscope: Acceleration (raw)
  gfl2::math::Vector3 gyroscopeRawAcceleration;
  // Gyroscope: Angle (adjusted)
  gfl2::math::Vector3 gyroscopeAdjustedAngle;
  // Gyroscope: Direction x (adjusted)
  gfl2::math::Vector3 gyroscopeAdjustedDirectionX;
  // Gyroscope: Direction y (adjusted)
  gfl2::math::Vector3 gyroscopeAdjustedDirectionY;
  // Gyroscope: Direction x (adjusted)
  gfl2::math::Vector3 gyroscopeAdjustedDirectionZ;
  // Gyroscope: Acceleration (adjusted)
  gfl2::math::Vector3 gyroscopeAdjustedAcceleration;
  // Model: Visualize SkyBox
  bool modelVisualizeSkyBox;
  // Model: Visualize Rail::Part
  bool modelVisualizeRail;
  // Model: Visualize Rail::InitialPart
  bool modelVisualizeInitialRail;
  // Model: Visualize Hero
  bool modelVisualizeHero;
  // Model: Visualize Pokemon
  bool modelVisualizePokemon;
  // Model: Visualize EnergyAura
  bool modelVisualizeEnergyAura;
  // Model: Visualize Energy
  bool modelVisualizeEnergy;
  // Model: Visualize Obstacle
  bool modelVisualizeObstacle;
  // Model: Visualize Distortion
  bool modelVisualizeDistortion;
  // Model: Visualize DistortionVortex
  bool modelVisualizeDistortionVortex;
  // pointer to Camera::Parameter
  Camera::Parameter* pCameraParameter;
  // pointer to Gimmick::Parameter
  Gimmick::Parameter* pGimmickParameter;
  // pointer to Gimmick::Placement::Parameter
  Gimmick::Placement::Parameter* pGimmickPlacementParameter;
  // pointer to Player::Parameter
  Player::Parameter* pPlayerParameter;
  // pointer to Rail::Parameter
  Rail::Parameter* pRailParameter;
  // pointer to Run::Parameter
  Run::Parameter* pRunParameter;
  // pointer to Sound::Parameter
  Sound::Parameter* pSoundParameter;
  // Translation: whether manulal trigger is enable or not
  bool isTranslationTriggerEnable;
  // Manual Camera
  ManualCamera* pManualCamera;
  // Whether the Manual Camera is being used or not
  bool isManualCameraBeingUsed;
  // Timer
  Timer* pTimer;
  // DrawUtil
  DrawUtil* pDrawUtil;
private:
  // instance
  static Parameter* pInstance;
  // constructor
  explicit Parameter(
              gfl2::heap::HeapBase* pHeap);
  // destructor
  ~Parameter();
};
}// namespace Debug
}// namespace LunaSolDive
#endif// __LUNA_SOL_DIVE__DEBUG__PARAMETER_H_INCLUDED__
#endif// PM_DEBUG
