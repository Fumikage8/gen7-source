//======================================================================
/**
 * @file GImmick_EnergyStorage.cpp
 * @date 2016/11/16 13:10:58
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
#include "./Energy.h"
#include "./EnergyStorage.h"
#include "./Parameter.h"
#include "../Assert.h"
#include "../Archive/Model.h"
#include "arc_index/LunaSolDiveModel.gaix"

namespace LunaSolDive{
namespace Gimmick{
// constructor
EnergyStorage::EnergyStorage()
      : ObjectStorage(),
        m_ModelNumber(0),
        m_ModelList(NULL),
        m_pModelResource(NULL),
        m_pAppearingMotionResource(NULL),
        m_pLoopMotionResource(NULL),
        m_pVanishingMotionResource(NULL),
        m_UsageStatus(0){}
// Initialize
void EnergyStorage::Initialize(
            gfl2::heap::HeapBase* pHeap,
            const u32& modelNumber)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  // initialize Base
  ObjectStorage::Initialize();
  // set parameter
  m_ModelNumber = modelNumber;
  LUNA_SOL_DIVE_ASSERT(m_ModelNumber <= 32);
  // initialize Parameter
  m_ModelList = NULL;
  m_UsageStatus = 0;
  // initialize Model List
  m_ModelList = GFL_NEW(pHeap) poke_3d::model::BaseModel[m_ModelNumber];
  LUNA_SOL_DIVE_ASSERT(m_ModelList != NULL);
}
// Terminate
void EnergyStorage::Terminate()
{
  // delete Model List
  if(m_ModelList != NULL)
  {
    for(u32 i = 0; i < m_ModelNumber; ++i)
    {
      m_ModelList[i].Destroy();
    }
    GFL_DELETE_ARRAY(m_ModelList);
    m_ModelList = NULL;
  }
  // delete ResourceNode
  GFL_SAFE_DELETE(m_pModelResource);
  GFL_SAFE_DELETE(m_pAppearingMotionResource);
  GFL_SAFE_DELETE(m_pLoopMotionResource);
  GFL_SAFE_DELETE(m_pVanishingMotionResource);
  // reset Parameter
  m_ModelNumber = 0;
  m_UsageStatus = 0;
  // terminate Base
  ObjectStorage::Terminate();
}
// Create Model
void EnergyStorage::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator)
{
  using namespace gfl2::renderingengine::scenegraph;
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  LUNA_SOL_DIVE_ASSERT(m_ModelList != NULL);
  // set Model Data
  const u32 modelIndex = GARC_LunaSolDiveModel_energy_GFBMDLP;
  const u32 appearingMotionIndex = GARC_LunaSolDiveModel_energy_in_GFBMOT;
  const u32 loopMotionIndex = GARC_LunaSolDiveModel_energy_GFBMOT;
  const u32 vanishingMotionIndex = GARC_LunaSolDiveModel_energy_out_GFBMOT;
  LUNA_SOL_DIVE_ASSERT(Archive::Model::Get().IsLoaded(modelIndex));
  LUNA_SOL_DIVE_ASSERT(Archive::Model::Get().IsLoaded(appearingMotionIndex));
  LUNA_SOL_DIVE_ASSERT(Archive::Model::Get().IsLoaded(loopMotionIndex));
  LUNA_SOL_DIVE_ASSERT(Archive::Model::Get().IsLoaded(vanishingMotionIndex));
  // create Model ResourceNode
  resource::GfBinaryMdlDataPack binaryModel;
  binaryModel.SetModelData(static_cast<c8*>(
              Archive::Model::Get().Data(modelIndex)));
  LUNA_SOL_DIVE_ASSERT(m_pModelResource == NULL);
  m_pModelResource = resource::ResourceManager::CreateResourceNode(
              pAllocator,
              &binaryModel);
  LUNA_SOL_DIVE_ASSERT(m_pModelResource != NULL);
  // create Motion ResourceNode
  resource::AnimationResourceNodeFactory motionFactory;
  LUNA_SOL_DIVE_ASSERT(m_pAppearingMotionResource == NULL);
  m_pAppearingMotionResource = motionFactory.CreateResourceNode(
              pAllocator,
              Archive::Model::Get().Data(appearingMotionIndex),
              0);
  LUNA_SOL_DIVE_ASSERT(m_pAppearingMotionResource != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pLoopMotionResource == NULL);
  m_pLoopMotionResource = motionFactory.CreateResourceNode(
              pAllocator,
              Archive::Model::Get().Data(loopMotionIndex),
              0);
  LUNA_SOL_DIVE_ASSERT(m_pLoopMotionResource != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pVanishingMotionResource == NULL);
  m_pVanishingMotionResource = motionFactory.CreateResourceNode(
              pAllocator,
              Archive::Model::Get().Data(vanishingMotionIndex),
              0);
  LUNA_SOL_DIVE_ASSERT(m_pVanishingMotionResource != NULL);
  // create Model
  for(u32 i = 0; i < m_ModelNumber; ++i)
  {
    m_ModelList[i].Create(
                pAllocator,
                pHeap,
                m_pModelResource);
    m_ModelList[i].SetPosition(gfl2::math::Vector3::GetZero());
    m_ModelList[i].SetRotation(gfl2::math::Vector3::GetZero());
  }
}
// Gimmick Type (override)
GimmickType EnergyStorage::Type() const
{
  return GIMMICK_TYPE__ENERGY;
}
// Set Up
void EnergyStorage::SetUp(
            Energy* pTarget,
            RenderingPipeline* pRenderingPipeline,
            const Rail::Location& location)
{
  LUNA_SOL_DIVE_ASSERT(pTarget != NULL);
  pTarget->Initialize(
              this,
              pRenderingPipeline,
              location);
  pTarget->SetModel();
}
// Get Model pointer
poke_3d::model::BaseModel* EnergyStorage::GetModelPtr()
{
  LUNA_SOL_DIVE_ASSERT(m_ModelList != NULL);
  for(u32 i = 0; i < m_ModelNumber; ++i)
  {
    if(! GetUsageStatus(i))
    {
      SetUsageStatus(i, true);
      // set appearing motion in slot 0
      m_ModelList[i].ChangeAnimationByResourceNode(
                  m_pAppearingMotionResource,
                  0);
      // set loop motion in slot 1
      m_ModelList[i].ChangeAnimationByResourceNode(
                  m_pLoopMotionResource,
                  1);
      return &m_ModelList[i];
    }
  }
  LUNA_SOL_DIVE_ASSERT(false);
  return NULL;
}
// Release Model pointer
void EnergyStorage::ReleaseModelPtr(
            poke_3d::model::BaseModel* pModel)
{
  LUNA_SOL_DIVE_ASSERT(pModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_ModelList != NULL);
  const s32 index = pModel - m_ModelList;
  LUNA_SOL_DIVE_ASSERT(0 <= index && index < static_cast<s32>(m_ModelNumber));
  if(0 <= index && index < static_cast<s32>(m_ModelNumber))
  {
    LUNA_SOL_DIVE_ASSERT(GetUsageStatus(index));
    SetUsageStatus(index, false);
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
void EnergyStorage::SetVanishingMotion(
            poke_3d::model::BaseModel* pModel)
{
  LUNA_SOL_DIVE_ASSERT(pModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pVanishingMotionResource != NULL);
  LUNA_SOL_DIVE_ASSERT(
              0 <= pModel - m_ModelList
              && pModel - m_ModelList < static_cast<s32>(m_ModelNumber));
  if(pModel != NULL)
  {
    // set vanishing motion in slot 0
    pModel->ChangeAnimationByResourceNode(
                m_pVanishingMotionResource,
                0);
    // loop motion has been set in slot 1
    LUNA_SOL_DIVE_ASSERT(
                pModel->GetAnimationResourceNode(1)
                == m_pLoopMotionResource);
  }
}
// Get whether the resource are used or not
bool EnergyStorage::GetUsageStatus(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < 32);
  return (m_UsageStatus & (0x1 << index)) != 0x0;
}
// Set whether the resource are used or not
void EnergyStorage::SetUsageStatus(
            const u32& index,
            const bool& isUsed)
{
  LUNA_SOL_DIVE_ASSERT(index < 32);
  LUNA_SOL_DIVE_ASSERT(GetUsageStatus(index) != isUsed);
  const b32 maskBits = 0x1 << index;
  if(isUsed)
  {
    m_UsageStatus |= maskBits;
  }
  else
  {
    m_UsageStatus &= ~maskBits;
  }
}
// Distance at the energy is activatied (override)
const f32& EnergyStorage::ActivationDistance() const
{
  return Parameter::Get().EnergyActivationDistance();
}
// Distance at the energy is inactivatied (override)
const f32& EnergyStorage::InactivationDistance() const
{
  return Parameter::Get().EnergyInactivationDistance();
}
}// namespace Gimmick
}// namespace LunaSolDive
