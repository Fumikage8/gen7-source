//======================================================================
/**
 * @file DistortionVortex.h
 * @date 2017/02/21 16:01:46
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__DISTORTION_VORTEX_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__DISTORTION_VORTEX_H_INCLUDED__
#pragma once

#include "../Base/SimpleModel.h"

namespace LunaSolDive{
// forward declaration
namespace Rail{
class Location;
}// namespace Rail

namespace Gimmick{
class DistortionVortex: public Base::SimpleModel
{
  GFL_FORBID_COPY_AND_ASSIGN(DistortionVortex);
public:
  // constructor
  DistortionVortex();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              RenderingPipeline* pRenderingPipeline);
  // Terminate
  void Terminate();
  // Create
  void Create(
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pAllocator);
  // Set Up
  void SetUp(
              const Rail::Location& location);
  // Update
  void Update();
  // Visualize (override)
  virtual void Visualize();
private:
  // Model (override)
  virtual poke_3d::model::BaseModel& Model() const;
  // Model
  poke_3d::model::BaseModel* m_pModel;
  // Motion ResourceNode
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pMotionNode;
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__DISTORTION_VORTEX_H_INCLUDED__
