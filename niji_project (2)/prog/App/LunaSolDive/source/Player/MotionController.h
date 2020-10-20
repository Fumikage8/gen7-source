//======================================================================
/**
 * @file MotionController.h
 * @date 2016/12/02 22:39:29
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__PLAYER__MOTION_CONTROLLER_H_INCLUDED__
#define __LUNA_SOL_DIVE__PLAYER__MOTION_CONTROLLER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "./MotionType.h"
#include "../Version.h"
#include "../Run/Signal.h"

// forward declaration
namespace poke_3d{
namespace model{
class CharaModel;
class DressUpModel;
}// namespace model
}// namespace poke_3d

namespace LunaSolDive{
namespace Player{
// forward declaration
class MotionStorage;

class MotionController
{
  GFL_FORBID_COPY_AND_ASSIGN(MotionController);
public:
  // constructor
  MotionController();
  // Initialize
  void Initialize(
              const Version& version,
              const MotionStorage* pStorage);
  // Terminate
  void Terminate();
  // Set Initial Motion
  void SetInitialMotion();
  // Update
  void Update(
              const MotionSpeed& speed,
              const MotionDirection& direction,
              const Run::ResetSignal& reset,
              const f32& elevationAngle);
  // Register the Hero's model
  void RegisterHeroModel(
              poke_3d::model::DressUpModel* pHeroModel);
  // Register the Pokemon's model
  void RegisterPokemonModel(
              poke_3d::model::CharaModel* pPokemonModel);
  // Motion Speed
  const MotionSpeed& Speed() const;
  // Motion Direction
  const MotionDirection& Direction() const;
  // Elevation Angle
  const f32& ElevationAngle() const;
private:
  // ChangeType
  enum ChangeType
  {
    CHANGE_TYPE__NONE,
    CHANGE_TYPE__RESET_END,
    CHANGE_TYPE__RESET_BEGIN,
    CHANGE_TYPE__SPEED,
    CHANGE_TYPE__DIRECTION,
    CHANGE_TYPE__TURN_END,
  };
  // Change State
  ChangeType ChangeState(
              const MotionSpeed& speed,
              const MotionDirection& direction,
              const Run::ResetSignal& signal);
  // Brend Frame
  u32 BrendFrame(
              const ChangeType& type,
              const MotionSpeed& previousSpeed) const;
  // Change Motion
  void ChangeMotion(
              const MotionSpeed& speed,
              const MotionDirection& direction,
              const Run::ResetSignal& reset);
  // Version
  Version m_Version;
  // pointer to Motion Storage
  const MotionStorage* m_pStorage;
  // Motion Speed
  MotionSpeed m_Speed;
  // Motion Direction
  MotionDirection m_Direction;
  // Whether it is turning or not
  bool m_IsTurning;
  // Whether it is reseting or not
  bool m_IsReseting;
  // Elevation Angle
  f32 m_ElevationAngle;
  // Pointer to the Hero's model
  poke_3d::model::DressUpModel* m_pHeroModel;
  // Pointer to the Pokemon's model
  poke_3d::model::CharaModel* m_pPokemonModel;
};
}// namespace Player
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__PLAYER__MOTION_CONTROLLER_H_INCLUDED__
