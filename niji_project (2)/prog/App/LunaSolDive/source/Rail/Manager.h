//======================================================================
/**
 * @file Manager.h
 * @date 2016/10/17 11:56:24
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__MANAGER_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__MANAGER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "./Location.h"
#include "./Section.h"

// forward declaration
namespace System{
class nijiAllocator;
}// namespace System

namespace LunaSolDive{
// forward declaration
class RenderingPipeline;
namespace Effect{
class Manager;
}// namespace Effect

namespace Rail{
// forward declaration
class Factory;
class InitialPart;
class Part;
class PartStorageList;
class SkyBox;
class TravelDistanceCounter;

class Manager{
  GFL_FORBID_COPY_AND_ASSIGN(Manager);
public:
  // constructor
  Manager();
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
  void Update();
  // Progress
  void Progress(
              const f32& forward,
              const f32& radiusRate,
              const f32& angle);
  // Progress
  void Progress(
              const f32& forward,
              const gfl2::math::Vector2& normalizedVerticalLocation);
  // Present Location
  const Location& Location() const;
  // Location
  Rail::Location Location(
              const f32& forward,
              const f32& radiusRate,
              const f32& angle) const;
  // Location
  Rail::Location Location(
              const f32& forward,
              const gfl2::math::Vector2& normalizedVerticalLocation) const;
  // Initial Directions
  const Directions& InitialDirections() const;
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement);
private:
  // Update Location
  void UpdateLocation();
  // Initialize Part & Section
  void InitializePartSection();
  // Create Section
  Section CreateSection(
              const s32& indexDifference) const;
  // Part pointer
  Part* PartPtr(
              const s32& indexDifference) const;
  // to Next Section
  void ToNextSection();
  // to Next Part
  void ToNextPart();
  // Part Storage List pointer
  PartStorageList* m_pPartStorageList;
  // SkyBox pointer
  SkyBox* m_pSkyBox;
  // Factory pointer
  Factory* m_pFactory;
  // Present Section
  Section m_PresentSection;
  // Part List
  Part* m_PartList;
  // Index of the present part in PartList
  u32 m_PresentPartIndex;
  // Initial Part
  InitialPart* m_pInitialPart;
  // Initial Directions
  Directions m_InitialDirections;
  // Internal Coordinates
  InternalCoordinates m_InternalCoordinates;
  // Present Location
  Rail::Location m_PresentLocation;
  // Travel Distance Counter
  TravelDistanceCounter* m_pTravelDistanceCounter;
};
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__MANAGER_H_INCLUDED__
