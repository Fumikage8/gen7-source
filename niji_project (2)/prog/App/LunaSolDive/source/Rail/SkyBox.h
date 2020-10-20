//======================================================================
/**
 * @file SkyBox.h
 * @date 2016/11/24 16:23:51
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__SKYBOX_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__SKYBOX_H_INCLUDED__
#pragma once

#include "../Base/SimpleModel.h"

namespace LunaSolDive{
namespace Rail{
// forward declaration
class Location;

class SkyBox: public Base::SimpleModel
{
  GFL_FORBID_COPY_AND_ASSIGN(SkyBox);
public:
  // constructor
  SkyBox();
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
  // Update
  void Update(
              const Location& location);
  // Visualize (override)
  virtual void Visualize();
private:
  // Model (override)
  virtual poke_3d::model::BaseModel& Model() const;
  // Model pointer
  poke_3d::model::BaseModel* m_pModel;
  // Animation ResourceNode pointer
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pAnimationNode;
};
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__SKYBOX_H_INCLUDED__
