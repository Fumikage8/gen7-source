//======================================================================
/**
 * @file Pokemon.h
 * @date 2016/09/07 19:20:18
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__PLAYER__POKEMON_H_INCLUDED__
#define __LUNA_SOL_DIVE__PLAYER__POKEMON_H_INCLUDED__
#pragma once

#include "./MotionType.h"
#include "../Version.h"
#include "../Base/SimpleModel.h"
#include "../Rail/Directions.h"

// forward declaration
namespace poke_3d{
namespace model{
class CharaModel;
class CharaModelFactory;
}// namespace poke_3d
}// namespace model

namespace LunaSolDive{
// forward declaration
namespace Effect{
class Manager;
class Particle;
}// namespace Effect

namespace Player{
// forward declaration
class MotionController;

class Pokemon: public Base::SimpleModel
{
  GFL_FORBID_COPY_AND_ASSIGN(Pokemon);
public:
  // constructor
  Pokemon();
  // Initialize
  void Initialize(
              const Version& version,
              gfl2::heap::HeapBase* pHeap,
              RenderingPipeline* pRenderingPipeline,
              Effect::Manager* pEffectManager,
              MotionController* pMotionController);
  // Terminate
  void Terminate();
  // Create
  void Create(
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pAllocator,
              poke_3d::model::CharaModelFactory* pCharaModelFactory);
  // Update
  void Update(
              const Rail::Directions& location,
              const gfl2::math::Vector3& offset,
              const f32& elevationAngle);
  // Update Energy
  void UpdateEnergy(
              const f32& energy);
  // Visualize (override)
  virtual void Visualize();
  // Non-Visualize (override)
  virtual void NonVisualize();
  // Waist Location
  const Rail::Directions& Waist() const;
  // Scale: getter
  f32 GetScale() const;
  // Scale: setter
  void SetScale(
              const f32& value);
  // Translation (override)
  virtual void Translation(
              const gfl2::math::Vector3& displacement);
private:
  // Model (override)
  virtual poke_3d::model::BaseModel& Model() const;
  // Version
  Version m_Version;
  // Model
  poke_3d::model::CharaModel* m_pModel;
  // Effect Manager
  Effect::Manager* m_pEffectManager;
  // Waist Location
  Rail::Directions m_Waist;
  // Effect: Energy Aura at Body
  Effect::Particle* m_pEffectBodyAura;
  // Effect: Trajectory
  Effect::Particle* m_pEffectTrajectory;
  // Effect: High Speed Trajectory
  Effect::Particle* m_pEffectHighSpeedTrajectory;
  // Effect: Energy Aura at Foot (Sol only)
  Effect::Particle* m_pEffectFootAura;
};
}// namespace Player
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__PLAYER__POKEMON_H_INCLUDED__
