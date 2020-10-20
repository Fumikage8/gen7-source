//======================================================================
/**
 * @file Base_SimpleModel.cpp
 * @date 2016/12/02 14:42:45
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_BaseModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include "System/include/nijiAllocator.h"
#include "./SimpleModel.h"
#include "../Assert.h"
#include "../RenderingPipeline.h"

namespace LunaSolDive{
namespace Base{
// constructor
SimpleModel::SimpleModel()
    : m_pRenderingPipeline(NULL),
      m_pModelResource(NULL),
      m_IsVisible(false){}
// destructor
SimpleModel::~SimpleModel(){}
// Initialize
void SimpleModel::Initialize(
            RenderingPipeline* pRenderingPipeline)
{
  m_pRenderingPipeline = pRenderingPipeline;
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
}
// Terminate
void SimpleModel::Terminate()
{
  LUNA_SOL_DIVE_ASSERT(! m_IsVisible);
  GFL_SAFE_DELETE(m_pModelResource);
  m_pRenderingPipeline = NULL;
  m_IsVisible = false;
}
// Create
void SimpleModel::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator,
            void* data)
{
  using namespace gfl2::renderingengine::scenegraph::resource;
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  LUNA_SOL_DIVE_ASSERT(data != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pModelResource == NULL);
  GfBinaryMdlDataPack binaryModel;
  binaryModel.SetModelData(static_cast<c8*>(data));
  // create Model ResourceNode
  m_pModelResource = ResourceManager::CreateResourceNode(
                      pAllocator,
                      &binaryModel);
  LUNA_SOL_DIVE_ASSERT(m_pModelResource != NULL);
  // create Model
  Model().Create(
              pAllocator,
              pHeap,
              m_pModelResource);
  Model().SetPosition(gfl2::math::Vector3::GetZero());
  Model().SetRotation(gfl2::math::Vector3::GetZero());
}
// Update
void SimpleModel::Update(
            const gfl2::math::Vector3& location,
            const gfl2::math::Quaternion& rotation)
{
  Model().SetPosition(location);
  Model().SetRotationQuat(rotation);
}
// Whether this Model is visible or not
const bool& SimpleModel::IsVisible() const
{
  return m_IsVisible;
}
// Overwrite visualize flag
void SimpleModel::OverwriteVisualizeFlag(
            const bool& value)
{
  m_IsVisible = value;
}
// Visualize
void SimpleModel::Visualize()
{
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  if(! m_IsVisible)
  {
    m_pRenderingPipeline->AddObject(
                Model().GetModelInstanceNode(),
                true);
    m_IsVisible = true;
  }
}
// Non-visualize
void SimpleModel::NonVisualize()
{
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  if(m_IsVisible)
  {
    m_pRenderingPipeline->RemoveObject(
                Model().GetModelInstanceNode());
    m_IsVisible = false;
  }
}
// Translation
void SimpleModel::Translation(
            const gfl2::math::Vector3& displacement)
{
  Model().SetPosition(
              Model().GetPosition() + displacement);
}
// pointer to RenderingPipeline
RenderingPipeline* SimpleModel::RenderingPipelinePtr() const
{
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  return m_pRenderingPipeline;
}
}// namespace Model
}// namespace LunaSolDive
