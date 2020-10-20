//======================================================================
/**
 * @file Gimmick_DistortionStorage.cpp
 * @date 2016/11/16 17:26:53
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_BaseModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include "System/include/nijiAllocator.h"
#include "./Distortion.h"
#include "./DistortionStorage.h"
#include "./Parameter.h"
#include "../Assert.h"
#include "../Archive/Model.h"
#include "../Effect/Manager.h"
#include "../Rail/Manager.h"
#include "arc_index/LunaSolDiveEffect.gaix"
#include "arc_index/LunaSolDiveModel.gaix"

namespace LunaSolDive{
namespace Gimmick{
// constructor
DistortionStorage::DistortionStorage()
    : ObjectStorage(),
      m_pRailManager(NULL),
      m_pEffectManager(NULL),
      m_ModelNumber(0),
      m_ModelList(NULL),
      m_ModelResourceList(NULL),
      m_AppearingMotionResourceList(NULL),
      m_LoopMotionResourceList(NULL),
      m_VanishingMotionResourceList(NULL),
      m_UsageStatus(0){}
// Initialize
void DistortionStorage::Initialize(
            gfl2::heap::HeapBase* pHeap,
            const Rail::Manager* pRailManager,
            Effect::Manager* pEffectManager,
            const u32& modelNumber)
{
  using namespace gfl2::renderingengine::scenegraph;
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  // initialize Base
  ObjectStorage::Initialize();
  // set pointer
  m_pRailManager = pRailManager;
  LUNA_SOL_DIVE_ASSERT(m_pRailManager != NULL);
  m_pEffectManager = pEffectManager;
  LUNA_SOL_DIVE_ASSERT(m_pEffectManager != NULL);
  // set parameter
  m_ModelNumber = modelNumber;
  LUNA_SOL_DIVE_ASSERT(ResourceNumber() <= 64);
  // initialize Model List
  m_ModelList = GFL_NEW(pHeap) poke_3d::model::BaseModel[ResourceNumber()];
  LUNA_SOL_DIVE_ASSERT(m_ModelList != NULL);
  // initialize ResourceNode List
  m_ModelResourceList = GFL_NEW(pHeap) resource::ResourceNode*[DISTORTION_TYPE__SIZE];
  LUNA_SOL_DIVE_ASSERT(m_ModelResourceList != NULL);
  m_AppearingMotionResourceList = GFL_NEW(pHeap) resource::ResourceNode*[DISTORTION_TYPE__SIZE];
  LUNA_SOL_DIVE_ASSERT(m_AppearingMotionResourceList != NULL);
  m_LoopMotionResourceList = GFL_NEW(pHeap) resource::ResourceNode*[DISTORTION_TYPE__SIZE];
  LUNA_SOL_DIVE_ASSERT(m_LoopMotionResourceList != NULL);
  m_VanishingMotionResourceList = GFL_NEW(pHeap) resource::ResourceNode*[DISTORTION_TYPE__SIZE];
  LUNA_SOL_DIVE_ASSERT(m_VanishingMotionResourceList != NULL);
  for(u32 i = 0; i < DISTORTION_TYPE__SIZE; ++i)
  {
    m_ModelResourceList[i] = NULL;
    m_AppearingMotionResourceList[i] = NULL;
    m_LoopMotionResourceList[i] = NULL;
    m_VanishingMotionResourceList[i] = NULL;
  }
}
// Terminate
void DistortionStorage::Terminate()
{
  // delete Model List
  const u32 resourceNumber = ResourceNumber();
  if(m_ModelList != NULL)
  {
    for(u32 i = 0; i < resourceNumber; ++i)
    {
      m_ModelList[i].Destroy();
    }
    GFL_DELETE_ARRAY(m_ModelList);
    m_ModelList = NULL;
  }
  // delete Model ResourceNode List
  if(m_ModelResourceList != NULL)
  {
    for(u32 i = 0; i < DISTORTION_TYPE__SIZE; ++i)
    {
      GFL_SAFE_DELETE(m_ModelResourceList[i]);
    }
    GFL_DELETE_ARRAY(m_ModelResourceList);
    m_ModelResourceList = NULL;
  }
  // delte Motion ResourceNode List
  if(m_AppearingMotionResourceList != NULL)
  {
    for(u32 i = 0; i < DISTORTION_TYPE__SIZE; ++i)
    {
      GFL_SAFE_DELETE(m_AppearingMotionResourceList[i]);
    }
    GFL_DELETE_ARRAY(m_AppearingMotionResourceList);
    m_AppearingMotionResourceList = NULL;
  }
  if(m_LoopMotionResourceList != NULL)
  {
    for(u32 i = 0; i < DISTORTION_TYPE__SIZE; ++i)
    {
      GFL_SAFE_DELETE(m_LoopMotionResourceList[i]);
    }
    GFL_DELETE_ARRAY(m_LoopMotionResourceList);
    m_LoopMotionResourceList = NULL;
  }
  if(m_VanishingMotionResourceList != NULL)
  {
    for(u32 i = 0; i < DISTORTION_TYPE__SIZE; ++i)
    {
      GFL_SAFE_DELETE(m_VanishingMotionResourceList[i]);
    }
    GFL_DELETE_ARRAY(m_VanishingMotionResourceList);
    m_VanishingMotionResourceList = NULL;
  }
  // reset parameter
  m_pRailManager = NULL;
  m_pEffectManager = NULL;
  m_ModelNumber = 0;
  m_UsageStatus = 0;
  // terminate Base
  ObjectStorage::Terminate();
}
// Create Model
void DistortionStorage::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator)
{
  using namespace gfl2::renderingengine::scenegraph;
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  LUNA_SOL_DIVE_ASSERT(m_ModelList != NULL);
  LUNA_SOL_DIVE_ASSERT(m_ModelResourceList != NULL);
  LUNA_SOL_DIVE_ASSERT(m_AppearingMotionResourceList != NULL);
  LUNA_SOL_DIVE_ASSERT(m_LoopMotionResourceList != NULL);
  LUNA_SOL_DIVE_ASSERT(m_VanishingMotionResourceList != NULL);
  resource::AnimationResourceNodeFactory motionFactory;
  for(u32 i = 0; i < DISTORTION_TYPE__SIZE; ++i)
  {
    const DistortionType type = static_cast<DistortionType>(i);
    const u32 modelIndex =
                (type == DISTORTION_TYPE__UB)
                ? GARC_LunaSolDiveModel_yugami01_GFBMDLP
                : (type == DISTORTION_TYPE__CLIFF)
                ? GARC_LunaSolDiveModel_yugami02_GFBMDLP
                : (type == DISTORTION_TYPE__WATER)
                ? GARC_LunaSolDiveModel_yugami03_GFBMDLP
                : (type == DISTORTION_TYPE__CAVE)
                ? GARC_LunaSolDiveModel_yugami04_GFBMDLP
                : GARC_LunaSolDiveModel_yugami05_GFBMDLP;
    const u32 appearingMotionIndex =
                (type == DISTORTION_TYPE__UB)
                ? GARC_LunaSolDiveModel_yugami01_in_GFBMOT
                : (type == DISTORTION_TYPE__CLIFF)
                ? GARC_LunaSolDiveModel_yugami02_in_GFBMOT
                : (type == DISTORTION_TYPE__WATER)
                ? GARC_LunaSolDiveModel_yugami03_in_GFBMOT
                : (type == DISTORTION_TYPE__CAVE)
                ? GARC_LunaSolDiveModel_yugami04_in_GFBMOT
                : GARC_LunaSolDiveModel_yugami05_in_GFBMOT;
    const u32 loopMotionIndex =
                (type == DISTORTION_TYPE__UB)
                ? GARC_LunaSolDiveModel_yugami01_GFBMOT
                : (type == DISTORTION_TYPE__CLIFF)
                ? GARC_LunaSolDiveModel_yugami02_GFBMOT
                : (type == DISTORTION_TYPE__WATER)
                ? GARC_LunaSolDiveModel_yugami03_GFBMOT
                : (type == DISTORTION_TYPE__CAVE)
                ? GARC_LunaSolDiveModel_yugami04_GFBMOT
                : GARC_LunaSolDiveModel_yugami05_GFBMOT;
    const u32 vanishingMotionIndex =
                (type == DISTORTION_TYPE__UB)
                ? GARC_LunaSolDiveModel_yugami01_out_GFBMOT
                : (type == DISTORTION_TYPE__CLIFF)
                ? GARC_LunaSolDiveModel_yugami02_out_GFBMOT
                : (type == DISTORTION_TYPE__WATER)
                ? GARC_LunaSolDiveModel_yugami03_out_GFBMOT
                : (type == DISTORTION_TYPE__CAVE)
                ? GARC_LunaSolDiveModel_yugami04_out_GFBMOT
                : GARC_LunaSolDiveModel_yugami05_out_GFBMOT;
    LUNA_SOL_DIVE_ASSERT(Archive::Model::Get().IsLoaded(modelIndex));
    LUNA_SOL_DIVE_ASSERT(Archive::Model::Get().IsLoaded(appearingMotionIndex));
    LUNA_SOL_DIVE_ASSERT(Archive::Model::Get().IsLoaded(loopMotionIndex));
    LUNA_SOL_DIVE_ASSERT(Archive::Model::Get().IsLoaded(vanishingMotionIndex));
    // create Model ResourceNode
    resource::GfBinaryMdlDataPack binaryModel;
    binaryModel.SetModelData(static_cast<c8*>(
                Archive::Model::Get().Data(modelIndex)));
    LUNA_SOL_DIVE_ASSERT(m_ModelResourceList[i] == NULL);
    m_ModelResourceList[i] = resource::ResourceManager::CreateResourceNode(
                pAllocator,
                &binaryModel);
    LUNA_SOL_DIVE_ASSERT(m_ModelResourceList[i] != NULL);
    // create Motion ResourceNode
    LUNA_SOL_DIVE_ASSERT(m_AppearingMotionResourceList[i] == NULL);
    m_AppearingMotionResourceList[i] = motionFactory.CreateResourceNode(
                pAllocator,
                Archive::Model::Get().Data(appearingMotionIndex),
                0);
    LUNA_SOL_DIVE_ASSERT(m_AppearingMotionResourceList[i] != NULL);
    LUNA_SOL_DIVE_ASSERT(m_LoopMotionResourceList[i] == NULL);
    m_LoopMotionResourceList[i] = motionFactory.CreateResourceNode(
                pAllocator,
                Archive::Model::Get().Data(loopMotionIndex),
                0);
    LUNA_SOL_DIVE_ASSERT(m_LoopMotionResourceList[i] != NULL);
    LUNA_SOL_DIVE_ASSERT(m_VanishingMotionResourceList[i] == NULL);
    m_VanishingMotionResourceList[i] = motionFactory.CreateResourceNode(
                pAllocator,
                Archive::Model::Get().Data(vanishingMotionIndex),
                0);
    LUNA_SOL_DIVE_ASSERT(m_VanishingMotionResourceList[i] != NULL);
    // create Model
    for(u32 k = 0; k < m_ModelNumber; ++k)
    {
      const u32 index = ResourceIndex(type, k);
      m_ModelList[index].Create(
                  pAllocator,
                  pHeap,
                  m_ModelResourceList[i]);
      m_ModelList[index].SetPosition(gfl2::math::Vector3::GetZero());
      m_ModelList[index].SetRotation(gfl2::math::Vector3::GetZero());
      m_ModelList[index].SetAnimationLoop(true);
    }
  }
}
// Gimmick Type (override)
GimmickType DistortionStorage::Type() const
{
  return GIMMICK_TYPE__DISTORTION;
}
// Set Up
void DistortionStorage::SetUp(
            Distortion* pTarget,
            RenderingPipeline* pRenderingPipeline,
            const DistortionType& type,
            const DistortionRarity& rarity,
            const Rail::Location& location)
{
  LUNA_SOL_DIVE_ASSERT(pTarget != NULL);
  LUNA_SOL_DIVE_ASSERT(! pTarget->IsValid());
  LUNA_SOL_DIVE_ASSERT(type != DISTORTION_TYPE__SIZE);
  LUNA_SOL_DIVE_ASSERT(rarity != DISTORTION_RARITY__NONE);
  const f32 distanceOffset =
          location.TravelDistance() - m_pRailManager->Location().TravelDistance();
  const Rail::Location collisionLocation = m_pRailManager->Location(
              distanceOffset - Parameter::Get().DistortionVanishDistance(),
              location.NormalizedCrossSectionPoint());
  if(! collisionLocation.IsValid()) return;
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(
              location.TravelDistance() - collisionLocation.TravelDistance(),
              Parameter::Get().DistortionVanishDistance(),
              Parameter::Get().DistortionVanishDistance() * 1.0e-4f));
  pTarget->Initialize(
              this,
              pRenderingPipeline,
              type,
              rarity,
              location,
              collisionLocation,
              CreateRarityEffect(rarity));
  pTarget->SetModel();
}
// Get Model pointer
poke_3d::model::BaseModel* DistortionStorage::GetModelPtr(
            const DistortionType& type)
{
  LUNA_SOL_DIVE_ASSERT(type != DISTORTION_TYPE__SIZE);
  LUNA_SOL_DIVE_ASSERT(m_ModelList != NULL);
  for(u32 i = 0; i < m_ModelNumber; ++i)
  {
    if(! GetUsageStatus(type, i))
    {
      SetUsageStatus(type, i, true);
      poke_3d::model::BaseModel* const pModel = &m_ModelList[ResourceIndex(type, i)];
      // set appearance motion in slot 1
      pModel->ChangeAnimationByResourceNode(
                  m_AppearingMotionResourceList[type],
                  0);
      // set loop motion in slot 1
      pModel->ChangeAnimationByResourceNode(
                  m_LoopMotionResourceList[type],
                  1);
      return pModel;
    }
  }
  return NULL;
}
// Release Model pointer
void DistortionStorage::ReleaseModelPtr(
            poke_3d::model::BaseModel* pModel)
{
  LUNA_SOL_DIVE_ASSERT(pModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_ModelList != NULL);
  const s32 index = pModel - m_ModelList;
  LUNA_SOL_DIVE_ASSERT(0 <= index && index < static_cast<s32>(ResourceNumber()));
  if(0 <= index && index < static_cast<s32>(ResourceNumber()))
  {
    const DistortionType type = static_cast<DistortionType>(index / m_ModelNumber);
    const u32 innerIndex = index % m_ModelNumber;
    LUNA_SOL_DIVE_ASSERT(GetUsageStatus(type, innerIndex));
    SetUsageStatus(type, innerIndex , false);
    m_ModelList[index].SetScale(1.0f, 1.0f, 1.0f);
    m_ModelList[index].SetPosition(gfl2::math::Vector3::GetZero());
    m_ModelList[index].SetRotation(gfl2::math::Vector3::GetZero());
    m_ModelList[index].ChangeAnimationByResourceNode(NULL, 0);
    m_ModelList[index].ChangeAnimationByResourceNode(NULL, 1);
    return;
  }
  LUNA_SOL_DIVE_ASSERT(false);
}
// Set Vanishing Motion to Model
void DistortionStorage::SetVanishingMotion(
            const DistortionType& type,
            poke_3d::model::BaseModel* pModel)
{
  LUNA_SOL_DIVE_ASSERT(type != DISTORTION_TYPE__SIZE);
  LUNA_SOL_DIVE_ASSERT(pModel != NULL);
  LUNA_SOL_DIVE_ASSERT(
              0 <= pModel - m_ModelList
              && pModel - m_ModelList < static_cast<s32>(ResourceNumber()));
  if(pModel != NULL)
  {
    // set vanishing motion in slot 0
    LUNA_SOL_DIVE_ASSERT(m_VanishingMotionResourceList[type] != NULL);
    pModel->ChangeAnimationByResourceNode(
                m_VanishingMotionResourceList[type],
                0);
    // loop motion has been set in slot 1
    LUNA_SOL_DIVE_ASSERT(
                pModel->GetAnimationResourceNode(1)
                == m_LoopMotionResourceList[type]);
  }
}
// The number vof model & resource node
u32 DistortionStorage::ResourceNumber() const
{
  return m_ModelNumber * DISTORTION_TYPE__SIZE;
}
// The index of model & resource node
u32 DistortionStorage::ResourceIndex(
          const DistortionType& type,
          const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(type != DISTORTION_TYPE__SIZE);
  LUNA_SOL_DIVE_ASSERT(index < m_ModelNumber);
  return type * m_ModelNumber + index;
}
// Get whether the resource are used or not
bool DistortionStorage::GetUsageStatus(
            const DistortionType& type,
            const u32& index) const
{
  const u32 effectiveIndex = ResourceIndex(type, index);
  LUNA_SOL_DIVE_ASSERT(effectiveIndex < 64);
  return (m_UsageStatus & (0x1ull << effectiveIndex)) != 0x0;
}
// Set whether the resource are used or not
void DistortionStorage::SetUsageStatus(
            const DistortionType& type,
            const u32& index,
            const bool& isUsed)
{
  const u32 effectiveIndex = ResourceIndex(type, index);
  LUNA_SOL_DIVE_ASSERT(effectiveIndex < 64);
  const u64 maskBits = 0x1ull << effectiveIndex;
  if(isUsed)
  {
    m_UsageStatus |= maskBits;
  }
  else
  {
    m_UsageStatus &= ~maskBits;
  }
}
// Create Rarity Effect
Effect::Particle* DistortionStorage::CreateRarityEffect(
            const DistortionRarity& rarity) const
{
  LUNA_SOL_DIVE_ASSERT(m_pEffectManager != NULL);
  LUNA_SOL_DIVE_ASSERT(rarity != DISTORTION_RARITY__NONE);
  if(rarity == DISTORTION_RARITY__0)
  {
    return NULL;
  }
  else
  {
    const u32 effectID =
                (rarity == DISTORTION_RARITY__1)
                ? GARC_LunaSolDiveEffect_ea_dive_warp_grade01_lo_lz_BPTCL
                : (rarity == DISTORTION_RARITY__2)
                ? GARC_LunaSolDiveEffect_ea_dive_warp_grade02_lo_lz_BPTCL
                : GARC_LunaSolDiveEffect_ea_dive_warp_grade03_lo_lz_BPTCL;
    return m_pEffectManager->Create(effectID);
  }
}
// Distance at the distortion activation (override)
const f32& DistortionStorage::ActivationDistance() const
{
  return Parameter::Get().DistortionActivationDistance();
}
// Distance at the distortion inactivation (override)
const f32& DistortionStorage::InactivationDistance() const
{
  return Parameter::Get().DistortionInactivationDistance();
}
}// namespace Gimmick
}// namespace LunaSolDive
