//======================================================================
/**
 * @file Gimmick_Storage.cpp
 * @date 2016/11/01 15:20:48
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./Storage.h"
#include "../Assert.h"
#include "arc_def_index/arc_def.h"
#include "arc_index/LunaSolDiveModel.gaix"

namespace LunaSolDive{
namespace Gimmick{
// constructor
Storage::Storage()
    : m_pRenderingPipeline(NULL){};
// Initialize
void Storage::Initialize(
            gfl2::heap::HeapBase* pHeap,
            RenderingPipeline* pRenderingPipeline,
            const Rail::Manager* pRailManager,
            Effect::Manager* pEffectManager,
            const u32& energyNumber,
            const u32& obstacleNumber,
            const u32& distortionNumber)
{
  // set RenderingPipeline pointer
  m_pRenderingPipeline = pRenderingPipeline;
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  // initialize Storage
  m_EnergyStorage.Initialize(
              pHeap,
              energyNumber);
  m_ObstacleStorage.Initialize(
              pHeap,
              obstacleNumber);
  m_DistortionStorage.Initialize(
              pHeap,
              pRailManager,
              pEffectManager,
              distortionNumber);
}
// Terminate
void Storage::Terminate()
{
  // terminate Storage
  m_EnergyStorage.Terminate();
  m_ObstacleStorage.Terminate();
  m_DistortionStorage.Terminate();
  // unset pointer
  m_pRenderingPipeline = NULL;
}
// Create Model
void Storage::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator)
{
  m_EnergyStorage.Create(
              pHeap,
              pAllocator);
  m_ObstacleStorage.Create(
              pHeap,
              pAllocator);
  m_DistortionStorage.Create(
              pHeap,
              pAllocator);
}
// Set Up: Energy
void Storage::SetUp(
            Energy* pTarget,
            const Rail::Location& location)
{
  m_EnergyStorage.SetUp(
              pTarget,
              m_pRenderingPipeline,
              location);
}
// Set Up: Obstacle
void Storage::SetUp(
            Obstacle* pTarget,
            const Rail::Location& location)
{
  m_ObstacleStorage.SetUp(
              pTarget,
              m_pRenderingPipeline,
              location);
}
// Set Up: Distortion
void Storage::SetUp(
            Distortion* pTarget,
            const DistortionType& type,
            const DistortionRarity& rarity,
            const Rail::Location& location)
{
  m_DistortionStorage.SetUp(
              pTarget,
              m_pRenderingPipeline,
              type,
              rarity,
              location);
}
}// namespace Gimmick
}// namespace LunaSolDive
