//======================================================================
/**
 * @file Rail_InitialPart.cpp
 * @date 2017/01/05 18:41:26
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_BaseModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNodeFactory.h>
#include "System/include/nijiAllocator.h"
#include "./Directions.h"
#include "./InitialPart.h"
#include "../Assert.h"
#include "../Archive/Model.h"
#include "../Effect/Manager.h"
#include "../Effect/Particle.h"
#include "arc_index/LunaSolDiveEffect.gaix"
#include "arc_index/LunaSolDiveModel.gaix"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Rail{
// constructor
InitialPart::InitialPart()
    : SimpleModel(),
      m_pModel(NULL),
      m_pMotionNode(NULL),
      m_pEffectManager(NULL),
      m_pEffect(NULL){}
// Initialze
void InitialPart::Initialize(
            gfl2::heap::HeapBase* pHeap,
            RenderingPipeline* pRenderingPipeline,
            Effect::Manager* pEffectManager)
{
  // initialize Base
  SimpleModel::Initialize(
              pRenderingPipeline);
  // initialize Model
  m_pModel = GFL_NEW(pHeap) poke_3d::model::BaseModel();
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  // set pointer
  m_pEffectManager = pEffectManager;
  LUNA_SOL_DIVE_ASSERT(m_pEffectManager != NULL);
}
// Terminate
void InitialPart::Terminate()
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
  // delete Effect
  GFL_SAFE_DELETE(m_pEffect);
  // unset pointer
  m_pEffectManager = NULL;
  // terminate Base
  SimpleModel::Terminate();
}
// Create
void InitialPart::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator,
            const Directions& directions)
{
  using namespace gfl2::renderingengine::scenegraph;
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  // create model
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  const u32 modelIndex = GARC_LunaSolDiveModel_hole_GFBMDLP;
  SimpleModel::Create(
              pHeap,
              pAllocator,
              Archive::Model::Get().Data(modelIndex));
  // create Animation ResourceNode
  const u32 animationIndex = GARC_LunaSolDiveModel_hole_GFBMOT;
  resource::AnimationResourceNodeFactory animationFactory;
  m_pMotionNode = animationFactory.CreateResourceNode(
             pAllocator,
             Archive::Model::Get().Data(animationIndex),
             0);
  // set motion
  m_pModel->ChangeAnimationByResourceNode(
              m_pMotionNode);
  m_pModel->SetAnimationLoop(true);
  // set directions
  m_pModel->SetPosition(directions.Point());
  m_pModel->SetRotationQuat(
              gfl2::math::Quaternion(directions.Vertical(), PI)
              * directions.RotationQuaternion());
  // initial motion update
  m_pModel->UpdateAnimation();
}
// Update
void InitialPart::Update()
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  if(IsVisible())
  {
    // update motion
    m_pModel->UpdateAnimation();
  }
}
// Visualize (override)
void InitialPart::Visualize()
{
  // effect
  if(! IsVisible())
  {
    // kill previous effect
    if(m_pEffect != NULL)
    {
      m_pEffect->Kill();
      GFL_DELETE(m_pEffect);
      m_pEffect = NULL;
    }
    // crate effect
    LUNA_SOL_DIVE_ASSERT(m_pEffect == NULL);
    m_pEffect = m_pEffectManager->Create(
                GARC_LunaSolDiveEffect_ea_dive_warphole_col_grain_lo_lz_BPTCL);
    if(m_pEffect != NULL)
    {
      m_pEffect->Start();
      m_pEffect->Update(
                  m_pModel->GetPosition(),
                  m_pModel->GetRotationQuat(),
                  1.0f);
    }
  }
  // visualize Model
#if PM_DEBUG
  if(! Debug::Parameter::Get().modelVisualizeInitialRail)
  {
    OverwriteVisualizeFlag(true);
    return;
  }
#endif// PM_DEBUG
  SimpleModel::Visualize();
}
// Non-Visualize (override)
void InitialPart::NonVisualize()
{
  // end effect
  if(IsVisible())
  {
    if(m_pEffect != NULL)
    {
      m_pEffect->End();
    }
  }
  // non-visualize model
  SimpleModel::NonVisualize();
}
// Model (override)
poke_3d::model::BaseModel& InitialPart::Model() const
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  return *m_pModel;
}
}// namespace Rail
}// namespace LunaSolDive
