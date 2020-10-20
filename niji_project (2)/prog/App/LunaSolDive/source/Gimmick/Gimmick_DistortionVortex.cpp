//======================================================================
/**
 * @file Gimmick_DistortionVortex.cpp
 * @date 2017/02/21 16:07:22
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>

#include <model/include/gfl2_BaseModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNodeFactory.h>
#include "System/include/nijiAllocator.h"
#include "./DistortionVortex.h"
#include "../Assert.h"
#include "../RenderingPipeline.h"
#include "../Archive/Model.h"
#include "../Rail/Location.h"
#include "arc_index/LunaSolDiveModel.gaix"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Gimmick{
// constructor
DistortionVortex::DistortionVortex()
    : SimpleModel(),
      m_pModel(NULL),
      m_pMotionNode(NULL){}
// Initialize
void DistortionVortex::Initialize(
            gfl2::heap::HeapBase* pHeap,
            RenderingPipeline* pRenderingPipeline)
{
  // initialize base
  SimpleModel::Initialize(
              pRenderingPipeline);
  // initialzie model
  LUNA_SOL_DIVE_ASSERT(m_pModel == NULL);
  m_pModel = GFL_NEW(pHeap) poke_3d::model::BaseModel();
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
}
// Terminate
void DistortionVortex::Terminate()
{
  // delete Model
  if(m_pModel != NULL)
  {
    if(IsVisible())
    {
      NonVisualize();
    }
    m_pModel->Destroy();
    GFL_DELETE(m_pModel);
    m_pModel = NULL;
  }
  GFL_SAFE_DELETE(m_pMotionNode);
  // terminate base
  SimpleModel::Terminate();
}
// Crate
void DistortionVortex::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator)
{
  using namespace gfl2::renderingengine::scenegraph;
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  // create model
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  const u32 modelIndex = GARC_LunaSolDiveModel_ea_dive_gate_inaura01_GFBMDLP;
  SimpleModel::Create(
              pHeap,
              pAllocator,
              Archive::Model::Get().Data(modelIndex));
  // create Animation ResourceNode
  const u32 animationIndex = GARC_LunaSolDiveModel_ea_dive_gate_inaura01_GFBMOT;
  resource::AnimationResourceNodeFactory animationFactory;
  m_pMotionNode = animationFactory.CreateResourceNode(
             pAllocator,
             Archive::Model::Get().Data(animationIndex),
             0);
}
// Set UP
void DistortionVortex::SetUp(
            const Rail::Location& location)
{
  // set location
  m_pModel->SetPosition(
              location.Point());
  m_pModel->SetRotationQuat(
              gfl2::math::Quaternion(location.Vertical(), PI)
                      * location.RotationQuaternion());
  // set scale
  const f32 scale = 3.0f;
  m_pModel->SetScale(scale, scale, scale);
  // set motion
  m_pModel->ChangeAnimationByResourceNode(
              m_pMotionNode);
}
// Update
void DistortionVortex::Update()
{
  // update motion
  m_pModel->UpdateAnimation();
}
// Visualize (override)
void DistortionVortex::Visualize()
{
#if PM_DEBUG
  if(! Debug::Parameter::Get().modelVisualizeDistortionVortex)
  {
    OverwriteVisualizeFlag(true);
    return;
  }
#endif// PM_DEBUG
  if(! IsVisible())
  {
    OverwriteVisualizeFlag(true);
    RenderingPipelinePtr()->AddObject(
                m_pModel->GetModelInstanceNode(),
                false);
  }
}
// Model (override)
poke_3d::model::BaseModel& DistortionVortex::Model() const
{
  LUNA_SOL_DIVE_ASSERT(m_pModel!= NULL);
  return *m_pModel;
}
}// namespace Gimmick
}// namespace LunaSolDive
