//======================================================================
/**
 * @file EnergyAura.h
 * @date 2016/12/01 11:43:46
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__PLAYER__ENERGY_AURA_H_INCLUDED__
#define __LUNA_SOL_DIVE__PLAYER__ENERGY_AURA_H_INCLUDED__
#pragma once


#include "../Base/SimpleModel.h"

namespace LunaSolDive{
// forward declaration
namespace Effect{
class Manager;
class Particle;
}// namespace Effect

namespace Player{
class EnergyAura: public Base::SimpleModel
{
  GFL_FORBID_COPY_AND_ASSIGN(EnergyAura);
public:
  // constructor
  EnergyAura();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              RenderingPipeline* pRenderingPipeline,
              Effect::Manager* pEffectManager);
  // Terminate
  void Terminate();
  // Create
  void Create(
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pAllocator);
  // Update
  void Update(
              const gfl2::math::Vector3& location,
              const gfl2::math::Quaternion& roation);
  // Update Energy
  void UpdateEnergy(
              const f32& energyRate);
  // Up
  void Up(
              const bool& isEnergyOverLimit);
  // Down
  void Down();
  // Vanish
  void Vanish();
  // Scale
  f32 Scale() const;
  // Visualize with Effect & SE
  void Activate();
  // Visualize (override)
  virtual void Visualize();
  // Non-Visualize (override)
  virtual void NonVisualize();
private:
  // State
  enum State
  {
    STATE__INACTIVE,
    STATE__ACTIVE,
    STATE__VANISH,
  };
  // Model (override)
  virtual poke_3d::model::BaseModel& Model() const;
  // Effect Manager
  Effect::Manager* m_pEffectManager;
  // Model pointer
  poke_3d::model::BaseModel* m_pModel;
  // Motion ResourceNode pointer
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pMotionNode;
  // State
  State m_State;
  // Effect: Up & Down
  Effect::Particle* m_pUpDownEffect;
  // Effect: Start
  Effect::Particle* m_pStartEffect;
  // Energy Rate
  f32 m_EnergyRate;
};
}// namespace Player
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__PLAYER__ENERGY_AURA_H_INCLUDED__
