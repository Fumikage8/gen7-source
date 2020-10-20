//======================================================================
/**
 * @file Hero.h
 * @date 2016/12/02 12:40:15
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__PLAYER__HERO_H_INCLUDED__
#define __LUNA_SOL_DIVE__PLAYER__HERO_H_INCLUDED__
#pragma once

#include "../Version.h"
#include "../Rail/Directions.h"

// forward declarations
namespace gfl2{
namespace fs{
class AsyncFileManager;
}// namespace fs
}// namespace gfl2
namespace poke_3d{
namespace model{
class BaseModel;
class CharaModelFactory;
class DressUpModel;
class DressUpModelResourceManager;
struct DressUpParam;
}// namespace model
}// namespace poke_3d

namespace LunaSolDive{
// forward declaration
class RenderingPipeline;

namespace Player{
// forward declaration
class MotionController;

class Hero
{
  GFL_FORBID_COPY_AND_ASSIGN(Hero);
public:
  // constructor
  Hero();
  // Initialize
  void Initialize(
              const HeroSex& sex,
              gfl2::heap::HeapBase* pHeap,
              RenderingPipeline* pRenderingPipeline,
              MotionController* pMotionController);
  // Terminate
  void Terminate();
  // Create
  void Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator,
            gfl2::fs::AsyncFileManager* pFileManager,
            poke_3d::model::CharaModelFactory* pCharModelFactory,
            const poke_3d::model::DressUpParam& dressUpParameter);
  // Update
  void Update(
              const Rail::Directions& location,
              const f32& elevationAngle);
  // Whether this Model is visible or not
  const bool& IsVisible() const;
  // Visualize
  void Visualize();
  // Non-Visualize
  void NonVisualize();
  // Spine2 Location
  const Rail::Directions& Spine2() const;
  // Offset
  const gfl2::math::Vector3& Offset() const;
  // Scale: getter
  f32 GetScale() const;
  // Scale: setter
  void SetScale(
              const f32& value);
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement);
private:
  // Offset Parameter
  struct OffsetParameter
  {
    f32 forward;
    f32 vertical;
    f32 horizontal;
  };
  // Sex of the hero
  HeroSex m_Sex;
  // RenderingPipeline pointer
  RenderingPipeline* m_pRenderingPipeline;
  // Model pointer
  poke_3d::model::DressUpModel* m_pModel;
  // DressUpModelResourceManager pointer
  poke_3d::model::DressUpModelResourceManager* m_pDressUpManager;
  // Spine2 Location
  Rail::Directions m_Spine2;
  // Offset
  gfl2::math::Vector3 m_Offset;
  // Offset Prameter (the Point of Hero's Spine2 Joint at initial Frame)
  OffsetParameter m_OffsetParameter;
  // Whether this Model is visible or not
  bool m_IsVisible;
  // Whether Offset Parameter is set or not
  bool m_IsOffsetParameterSet;
};
}// namespace Player
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__PLAYER__HERO_H_INCLUDED__
