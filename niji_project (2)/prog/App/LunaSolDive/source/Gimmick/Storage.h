//======================================================================
/**
 * @file Storage.h
 * @date 2016/11/01 13:20:35
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__STORAGE_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__STORAGE_H_INCLUDED__
#pragma once

#include "./DistortionStorage.h"
#include "./EnergyStorage.h"
#include "./ObstacleStorage.h"

namespace LunaSolDive{
namespace Gimmick{
class Storage
{
  GFL_FORBID_COPY_AND_ASSIGN(Storage);
public:
  // constructor
  Storage();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              RenderingPipeline* pRenderingPipeline,
              const Rail::Manager* pRailManager,
              Effect::Manager* pEffectManager,
              const u32& energyNumber,
              const u32& obstacleNumber,
              const u32& distortionNumber);
  // Terminate
  void Terminate();
  // Create Model
  void Create(
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pAllocator);
  // Set Up: Energy
  void SetUp(
              Energy* pTarget,
              const Rail::Location& location);
  // Set Up: Obstacle
  void SetUp(
              Obstacle* pTarget,
              const Rail::Location& location);
  // Set Up: Distortion
  void SetUp(
              Distortion* pTarget,
              const DistortionType& type,
              const DistortionRarity& rarity,
              const Rail::Location& location);
private:
  // RenderingPipeline pointer
  RenderingPipeline* m_pRenderingPipeline;
  // Energy Storage
  EnergyStorage m_EnergyStorage;
  // Obstacle Storage
  ObstacleStorage m_ObstacleStorage;
  // Distortion Storage
  DistortionStorage m_DistortionStorage;
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__STORAGE_H_INCLUDED__
