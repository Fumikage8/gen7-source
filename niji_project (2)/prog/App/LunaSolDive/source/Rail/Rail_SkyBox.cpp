//======================================================================
/**
 * @file Rail_SkyBox.cpp
 * @date 2016/11/24 16:29:49
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_BaseModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNodeFactory.h>
#include "System/include/nijiAllocator.h"
#include "./Location.h"
#include "./SkyBox.h"
#include "../Assert.h"
#include "../RenderingPipeline.h"
#include "../Archive/Model.h"
#include "arc_index/LunaSolDiveModel.gaix"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Rail{
// constructor
SkyBox::SkyBox()
    : SimpleModel(),
      m_pModel(NULL),
      m_pAnimationNode(NULL){}
// Initialize
void SkyBox::Initialize(
            gfl2::heap::HeapBase* pHeap,
            RenderingPipeline* pRenderingPipeline)
{
  // initialize Base
  SimpleModel::Initialize(
              pRenderingPipeline);
  // initialize Model
  m_pModel = GFL_NEW(pHeap) poke_3d::model::BaseModel();
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
}
// Terminate
void SkyBox::Terminate()
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
  GFL_SAFE_DELETE(m_pAnimationNode);
  // terminate Base
  SimpleModel::Terminate();
}
// Create
void SkyBox::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator)
{
  using namespace gfl2::renderingengine::scenegraph;
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  // create model
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  const u32 modelIndex = GARC_LunaSolDiveModel_skybox_GFBMDLP;
  SimpleModel::Create(
              pHeap,
              pAllocator,
              Archive::Model::Get().Data(modelIndex));
  // create Animation ResourceNode
  const u32 animationIndex = GARC_LunaSolDiveModel_skybox_GFBMOT;
  resource::AnimationResourceNodeFactory animationFactory;
  m_pAnimationNode = animationFactory.CreateResourceNode(
             pAllocator,
             Archive::Model::Get().Data(animationIndex),
             0);
  // set animation
  m_pModel->ChangeAnimationByResourceNode(
              m_pAnimationNode);
  m_pModel->SetAnimationLoop(true);
  // visualize
  Visualize();
}
// Update
void SkyBox::Update(
            const Location& location)
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  // update Location
  m_pModel->SetPosition(
              location.Rail().Point());
  m_pModel->SetRotationQuat(
              gfl2::math::Quaternion(location.Vertical(), PI)
              * location.RotationQuaternion());
  // update Animation
  m_pModel->UpdateAnimation();
}
// Visualize (override)
void SkyBox::Visualize()
{
#if PM_DEBUG
  if(! Debug::Parameter::Get().modelVisualizeSkyBox)
  {
    OverwriteVisualizeFlag(true);
    return;
  }
#endif// PM_DEBUG
  if(! IsVisible())
  {
    OverwriteVisualizeFlag(true);
    RenderingPipelinePtr()->AddSkyBox(
                m_pModel->GetModelInstanceNode());
  }
}
// Model (override)
poke_3d::model::BaseModel& SkyBox::Model() const
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  return *m_pModel;
}
}// namespace Run
}// namespace LunaSolDive
