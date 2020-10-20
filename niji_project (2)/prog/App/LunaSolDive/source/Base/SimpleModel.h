//======================================================================
/**
 * @file SimpleModel.h
 * @date 2016/12/02 13:46:00
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__BASE__SIMPLE_MODEL_H_INCLUDED__
#define __LUNA_SOL_DIVE__BASE__SIMPLE_MODEL_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>

// forward declaration
namespace gfl2{
namespace math{
class Quaternion;
class Vector3;
}// namespace math
namespace renderingengine{
namespace scenegraph{
namespace resource{
class ResourceNode;
}// namespace resource
}// namespace scenegraph
}// namespace renderingengine
}// namespace gfl2
namespace poke_3d{
namespace model{
class BaseModel;
}// namespace model
}// namespace poke_3d
namespace System{
class nijiAllocator;
}// namespace System

namespace LunaSolDive{
// forward declaration
class RenderingPipeline;

namespace Base{
class SimpleModel
{
  GFL_FORBID_COPY_AND_ASSIGN(SimpleModel);
public:
  // constructor
  SimpleModel();
  // destructor
  virtual ~SimpleModel() = 0;
  // Update
  void Update(
              const gfl2::math::Vector3& location,
              const gfl2::math::Quaternion& rotation);
  // Whether this Model is visible or not
  const bool& IsVisible() const;
  // Visualize
  virtual void Visualize();
  // Non-visualize
  virtual void NonVisualize();
  // Translation
  virtual void Translation(
              const gfl2::math::Vector3& displacement);
protected:
  // Initialize
  void Initialize(
              RenderingPipeline* pRenderingPipeline);
  // Terminate
  void Terminate();
  // Create
  void Create(
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pAllocator,
              void* data);
  // Overwrite visualize flag
  void OverwriteVisualizeFlag(
              const bool& value);
  // pointer to RenderingPipeline
  RenderingPipeline* RenderingPipelinePtr() const;
private:
  // Model
  virtual poke_3d::model::BaseModel& Model() const = 0;
  // pointer to RenderingPipeline
  RenderingPipeline* m_pRenderingPipeline;
  // Model ResourceNode
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pModelResource;
  // Whether this Model is visible or not
  bool m_IsVisible;
};
}// namespace Model
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__BASE__SIMPLE_MODEL_H_INCLUDED__
