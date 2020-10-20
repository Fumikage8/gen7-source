//======================================================================
/**
 * @file Manager.h
 * @date 2016/11/29 12:09:10
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__PLAYER__MANAGER_H_INCLUDED__
#define __LUNA_SOL_DIVE__PLAYER__MANAGER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "./MotionType.h"
#include "../Version.h"
#include "../Rail/Directions.h"
#include "../Run/Signal.h"

// forward declaration
namespace gfl2{
namespace fs{
class AsyncFileManager;
}// namespace fs
}// namespace gfl2
namespace poke_3d{
namespace model{
class CharaModelFactory;
struct DressUpParam;
}// namespace model
}// namespace poke_3d
namespace System{
class nijiAllocator;
}// namespace System

namespace LunaSolDive{
// forward declaration
class RenderingPipeline;
namespace Effect{
class Manager;
class Particle;
}// namespace Effect
namespace Run{
class Aim;
class Energy;
}// namespace Run
namespace Player{
// forward declaration
class EnergyAura;
class Hero;
class MotionController;
class MotionStorage;
class Pokemon;

class Manager{
  GFL_FORBID_COPY_AND_ASSIGN(Manager);
public:
  // constructor
  Manager();
  // Initialize
  void Initialize(
              const Version& version,
              const HeroSex& sex,
              gfl2::heap::HeapBase* pHeap,
              RenderingPipeline* pRenderingPipeline,
              Effect::Manager* pEffectManager);
  // Terminate
  void Terminate();
  // Create
  void Create(
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pAllocator,
              gfl2::fs::AsyncFileManager* pFileManager,
              const poke_3d::model::DressUpParam& dressUpParameter);
  // Update at the Beginning stage
  void UpdateBeginning(
              const Rail::Directions& location);
  // Update at the Main stage
  void UpdateMain(
              const Rail::Directions& location,
              const Run::Energy& energy,
              const Run::Aim& aim,
              const Run::ResetSignal& reset);
  // Update at the Ending stage
  void UpdateEnding(
              const Rail::Directions& location,
              const f32& scaleChangeRate);
  // Power Up
  void PowerUp(
              const bool& isEnergyOverLimit);
  // Power Down
  void PowerDown();
  // Damage
  void Damage();
  // Whether the player is dameged or not
  const bool& IsDamaged() const;
  // Activate EnergyAura
  void ActivateEnergyAura();
  // Update EnergyAura
  void UpdateEnergyAura(
              const gfl2::math::Vector3& cameraLocation);
  // Update Energy
  void UpdateEnergy(
              const f32& energy);
  // change from Main to Ending
  void ChangeMainToEnding();
  // Location
  const Rail::Directions& Location() const;
  // Origin
  Rail::Directions OriginLocation() const;
  // Hero: Spine2 joint
  const Rail::Directions& HeroSpine2() const;
  // Scale
  f32 Scale() const;
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement);
private:
  // Update: Common
  void UpdateCommon();
  // Version
  Version m_Version;
  // Sex of the hero
  HeroSex m_Sex;
  // ChraModelFactory pointer
  poke_3d::model::CharaModelFactory* m_pCharaModelFactory;
  // Effect Manager pointer
  Effect::Manager* m_pEffectManager;
  // MotionStorage pointer
  MotionStorage* m_pMotionStorage;
  // MotionController pointer
  MotionController* m_pMotionController;
  // Hero pointer
  Hero* m_pHero;
  // Pokemon pointer
  Pokemon* m_pPokemon;
  // Energy Aura pointer
  EnergyAura* m_pEnergyAura;
  // Location
  Rail::Directions m_Location;
  // Effect: Power Up / Power Down
  Effect::Particle* m_pEffectPower;
  // Effect: Damage
  Effect::Particle* m_pEffectDamage;
  // Whether the player is dameged or not
  bool m_IsDamaged;
  // Number of frames after receiving damage
  u32 m_DamageFrameCount;
  // Energy
  f32 m_Energy;
#if PM_DEBUG
public:
  // Restart: change GameOver -> Main
  void Restart();
#endif// PM_DEBUG
};
}// namespace Player
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__PLAYER__MANAGER_H_INCLUDED__
