//======================================================================
/**
 * @file Player_MotionController.cpp
 * @date 2016/12/02 22:42:10
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <model/include/gfl2_DressUpModel.h>
#include "./MotionController.h"
#include "./MotionStorage.h"
#include "./Parameter.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Player{
// constructor
MotionController::MotionController()
    : m_Version(VERSION__NONE),
      m_pStorage(NULL),
      m_Speed(MOTION_SPEED__SIZE),
      m_Direction(MOTION_DIRECTION__SIZE),
      m_IsTurning(false),
      m_IsReseting(false),
      m_ElevationAngle(0.0f),
      m_pHeroModel(NULL),
      m_pPokemonModel(NULL){}
// Initialize
void MotionController::Initialize(
            const Version& version,
            const MotionStorage* pStorage)
{
  m_Version = version;
  LUNA_SOL_DIVE_ASSERT(m_Version != VERSION__NONE);
  m_pStorage = pStorage;
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
}
// Terminate
void MotionController::Terminate()
{
  m_Version = VERSION__NONE;
  m_pStorage = NULL;
  m_Speed = MOTION_SPEED__SIZE;
  m_Direction = MOTION_DIRECTION__SIZE;
  m_IsTurning = false;
  m_IsReseting = false;
  m_ElevationAngle = 0.0f;
  m_pHeroModel = NULL;
  m_pPokemonModel = NULL;
}
// Set Initial Motion
void MotionController::SetInitialMotion()
{
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pHeroModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pPokemonModel != NULL);
  m_Speed = MOTION_SPEED__RUN;
  m_Direction = MOTION_DIRECTION__STRAIGHT;
  m_pHeroModel->ChangeAnimationByResourceNode(
              m_pStorage->Hero(m_Speed, m_Direction, false, false));
  m_pPokemonModel->ChangeAnimationByResourceNode(
              m_pStorage->Pokemon(m_Speed, m_Direction, false, false));
}
// Update
void MotionController::Update(
            const MotionSpeed& speed,
            const MotionDirection& direction,
            const Run::ResetSignal& reset,
            const f32& elevationAngle)
{
  LUNA_SOL_DIVE_ASSERT(speed != MOTION_SPEED__SIZE);
  LUNA_SOL_DIVE_ASSERT(direction != MOTION_DIRECTION__SIZE);
  LUNA_SOL_DIVE_ASSERT(m_pHeroModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pPokemonModel != NULL);
  // update elevation angle
  m_ElevationAngle +=
          Parameter::Get().ElevationAngleChangeRate()
          * (elevationAngle - m_ElevationAngle);
  // change motion
  ChangeMotion(speed, direction, reset);
  // update motion
  m_pHeroModel->UpdateAnimation();
  m_pPokemonModel->UpdateAnimation();
}
// Change State
MotionController::ChangeType MotionController::ChangeState(
            const MotionSpeed& speed,
            const MotionDirection& direction,
            const Run::ResetSignal& reset)
{
  LUNA_SOL_DIVE_ASSERT(speed != MOTION_SPEED__SIZE);
  LUNA_SOL_DIVE_ASSERT(direction != MOTION_DIRECTION__SIZE);
  LUNA_SOL_DIVE_ASSERT(m_pHeroModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pPokemonModel != NULL);
  // Whether motion change is necessary or not
  ChangeType result = CHANGE_TYPE__NONE;
  if(m_IsReseting)
  {// reset end
    if(m_pHeroModel->IsAnimationLastFrame())
    {
      m_IsReseting = false;
      m_IsTurning = false;
      result = CHANGE_TYPE__RESET_END;
    }
  }
  else if(reset != Run::RESET_SIGNAL__NONE)
  {// reset begin
    m_IsReseting = true;
    m_IsTurning = false;
    result = CHANGE_TYPE__RESET_BEGIN;
  }
  else if(m_Speed != speed)
  {// speed
    m_IsTurning = false;
    result = CHANGE_TYPE__SPEED;
  }
  else if(m_Direction != direction)
  {// direction
    m_IsTurning = (direction != MOTION_DIRECTION__STRAIGHT);
    result = CHANGE_TYPE__DIRECTION;
  }
  else if(m_IsTurning)
  {// turn end
    if(m_pHeroModel->IsAnimationLastFrame())
    {
      m_IsTurning = false;
      result = CHANGE_TYPE__TURN_END;
    }
  }
  m_Speed = speed;
  m_Direction = direction;
  return result;
}
// Brend Frame
u32 MotionController::BrendFrame(
            const ChangeType& type,
            const MotionSpeed& previousSpeed) const
{
  LUNA_SOL_DIVE_ASSERT(type != CHANGE_TYPE__NONE);
  const MotionBrendFrames& list =
          (m_Version == VERSION__SOL)
          ? Parameter::Get().MotionBrendFramesSol()
          : Parameter::Get().MotionBrendFramesLuna();
  if(type == CHANGE_TYPE__RESET_END)
  {// reset end
    return (m_Speed == MOTION_SPEED__WALK)
            ? list.reset_end_walk
            : (m_Speed == MOTION_SPEED__RUN)
            ? list.reset_end_run
            : list.reset_end_glide;
  }
  else if(type == CHANGE_TYPE__RESET_BEGIN)
  {// reset begin
    return (m_Speed == MOTION_SPEED__WALK)
            ? list.reset_begin_walk
            : (m_Speed == MOTION_SPEED__RUN)
            ? list.reset_begin_run
            : list.reset_begin_glide;
  }
  else if(type == CHANGE_TYPE__SPEED)
  {// speed
    return (previousSpeed == MOTION_SPEED__WALK)
            ? ((m_Speed == MOTION_SPEED__RUN)? list.walk_to_run: list.walk_to_glide)
            : (previousSpeed == MOTION_SPEED__RUN)
            ? ((m_Speed == MOTION_SPEED__WALK)? list.run_to_walk: list.run_to_glide)
            : ((m_Speed == MOTION_SPEED__WALK)? list.glide_to_walk: list.glide_to_run);
  }
  else if(type == CHANGE_TYPE__DIRECTION)
  {// direction
    return (m_Speed == MOTION_SPEED__WALK)
            ? list.direction_change_walk
            : (m_Speed == MOTION_SPEED__RUN)
            ? list.direction_change_run
            : list.direction_change_glide;
  }
  else if(type == CHANGE_TYPE__TURN_END)
  {// turn end
    return (m_Speed == MOTION_SPEED__WALK)
            ? list.turn_end_walk
            : (m_Speed == MOTION_SPEED__RUN)
            ? list.turn_end_run
            : list.turn_end_glide;
  }
  else
  {// none
    return 0;
  }
}
// Change Motion
void MotionController::ChangeMotion(
            const MotionSpeed& speed,
            const MotionDirection& direction,
            const Run::ResetSignal& reset)
{
  LUNA_SOL_DIVE_ASSERT(speed != MOTION_SPEED__SIZE);
  LUNA_SOL_DIVE_ASSERT(direction != MOTION_DIRECTION__SIZE);
  LUNA_SOL_DIVE_ASSERT(m_pHeroModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pPokemonModel != NULL);
  // change state
  const MotionSpeed previousSpeed = m_Speed;
  const ChangeType type = ChangeState(
              speed,
              direction,
              reset);
  if(type != CHANGE_TYPE__NONE)
  {
    ARAI_PRINT("Motion Change\n");
    ARAI_PRINT("  speed: %s\n",
                (m_Speed == MOTION_SPEED__WALK)? "Walk":
                (m_Speed == MOTION_SPEED__RUN)? "Run":
                (m_Speed == MOTION_SPEED__GLIDE)? "Glide":
                "Error");
    ARAI_PRINT("  direction: %s\n",
                (m_Direction == MOTION_DIRECTION__STRAIGHT)? "Straight":
                (m_Direction == MOTION_DIRECTION__LEFT)? "Left":
                (m_Direction == MOTION_DIRECTION__RIGHT)? "Right":
                "Error");
    ARAI_PRINT("  turn: %s\n", m_IsTurning? "True": "False");
    ARAI_PRINT("  reset: %s\n", m_IsReseting? "True": "False");
    const u32 changeFrame = BrendFrame(
                type,
                previousSpeed);
    // change hero's motion
    m_pHeroModel->ChangeAnimationSmoothByResourceNode(
                m_pStorage->Hero(
                            m_Speed,
                            m_Direction,
                            m_IsTurning,
                            m_IsReseting),
                changeFrame);
    // change pokemon's motion
    m_pPokemonModel->ChangeAnimationSmoothByResourceNode(
                m_pStorage->Pokemon(
                            m_Speed,
                            m_Direction,
                            m_IsTurning,
                            m_IsReseting),
                changeFrame);
  }
}
// Register the Hero's model
void MotionController::RegisterHeroModel(
            poke_3d::model::DressUpModel* pHeroModel)
{
  m_pHeroModel = pHeroModel;
  LUNA_SOL_DIVE_ASSERT(m_pHeroModel != NULL);
}
// Register the Pokemon's model
void MotionController::RegisterPokemonModel(
            poke_3d::model::CharaModel* pPokemonModel)
{
  m_pPokemonModel = pPokemonModel;
  LUNA_SOL_DIVE_ASSERT(m_pPokemonModel != NULL);
}
// Motion Speed
const MotionSpeed& MotionController::Speed() const
{
  return m_Speed;
}
// Motion Direction
const MotionDirection& MotionController::Direction() const
{
  return m_Direction;
}
// Elevation Angle
const f32& MotionController::ElevationAngle() const
{
  return m_ElevationAngle;
}
}// namespace Player
}// namespace LunaSolDive
