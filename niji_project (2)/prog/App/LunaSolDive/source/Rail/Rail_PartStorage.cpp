//======================================================================
/**
 * @file Rail_PartStorage.cpp
 * @date 2016/10/24 11:36:05
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_BaseModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include "System/include/nijiAllocator.h"
#include "./PartStorage.h"
#include "../Assert.h"
#include "../Archive/Model.h"
#include "arc_index/LunaSolDiveModel.gaix"

namespace LunaSolDive{
namespace Rail{
// constructor
PartStorage::PartStorage()
    : m_Shape(PART_SHAPE__SIZE),
      m_PointNumber(0),
      m_PointList(NULL),
      m_Radius(0.0f),
      m_BeginVerticalDirection(gfl2::math::Vector3::GetZero()),
      m_ModelNumber(0),
      m_ModelList(NULL),
      m_pModelResource(NULL),
      m_pAppearingMotionResource(NULL),
      m_pLoopMotionResource(NULL),
      m_UsageStatus(0){}
// Initialize
void PartStorage::Initialize(
            gfl2::heap::HeapBase* pHeap,
            const PartShape& shape,
            const u32& modelNumber,
            const u32& pointNumber,
            const gfl2::math::Vector3* pointList,
            const f32& radius,
            const gfl2::math::Vector3& beginVerticalDirection)
{
  // initialize: Geometoric Parameter
  m_PointNumber = pointNumber;
  LUNA_SOL_DIVE_ASSERT(m_PointNumber > 1);
  m_PointList = pointList;
  LUNA_SOL_DIVE_ASSERT(m_PointList != NULL);
  m_Radius = radius;
  LUNA_SOL_DIVE_ASSERT(m_Radius > 0.0f);
  m_Shape = shape;
  LUNA_SOL_DIVE_ASSERT(m_Shape != PART_SHAPE__SIZE);
  // Set Vertical Direction
  const gfl2::math::Vector3 displacement = Point(1) - Point(0);
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(displacement.Length()));
  const gfl2::math::Vector3 direction = displacement.Normalize();
  const gfl2::math::Vector3 vector =
              beginVerticalDirection - direction * direction.Dot(beginVerticalDirection);
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(vector.Length()));
  m_BeginVerticalDirection = vector.Normalize();
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_BeginVerticalDirection.Length(), 1.0f));
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsAlmostZero(m_BeginVerticalDirection.Dot(direction)));
  // initialize: Model List
  m_ModelNumber = modelNumber;
  m_ModelList = GFL_NEW(pHeap) poke_3d::model::BaseModel[m_ModelNumber];
  LUNA_SOL_DIVE_ASSERT(m_ModelList != NULL);
}
// Terminate
void PartStorage::Terminate()
{
  // reset geometric parameter
  m_PointNumber = 0;
  GFL_SAFE_DELETE(m_PointList);
  m_Radius = 0.0f;
  m_BeginVerticalDirection = gfl2::math::Vector3::GetZero();
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
  // reset model paraemter
  m_ModelNumber = 0;
  m_UsageStatus = 0;
}
// Create
void PartStorage::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator)
{
  using namespace gfl2::renderingengine::scenegraph;
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  LUNA_SOL_DIVE_ASSERT(m_ModelList != NULL);
  // set Model Data
  const u32 modelIndex =
              (m_Shape == PART_SHAPE__STRAIGHT)
              ? GARC_LunaSolDiveModel_straight_GFBMDLP
              : (m_Shape == PART_SHAPE__RIGHT_CURVE)
              ? GARC_LunaSolDiveModel_curve_GFBMDLP
              : 0xfffffffff;
  const u32 appearingMotionIndex =
              (m_Shape == PART_SHAPE__STRAIGHT)
              ? GARC_LunaSolDiveModel_straight_in_GFBMOT
              : (m_Shape == PART_SHAPE__RIGHT_CURVE)
              ? GARC_LunaSolDiveModel_curve_in_GFBMOT
              : 0xfffffffff;
  const u32 loopMotionIndex =
              (m_Shape == PART_SHAPE__STRAIGHT)
              ? GARC_LunaSolDiveModel_straight_GFBMOT
              : (m_Shape == PART_SHAPE__RIGHT_CURVE)
              ? GARC_LunaSolDiveModel_curve_GFBMOT
              : 0xfffffffff;
  LUNA_SOL_DIVE_ASSERT(Archive::Model::Get().IsLoaded(modelIndex));
  LUNA_SOL_DIVE_ASSERT(Archive::Model::Get().IsLoaded(appearingMotionIndex));
  LUNA_SOL_DIVE_ASSERT(Archive::Model::Get().IsLoaded(loopMotionIndex));
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
// Shape
const PartShape& PartStorage::Shape() const
{
  return m_Shape;
}
// Point Number
const u32& PartStorage::PointNumber() const
{
  return m_PointNumber;
}
// Point
const gfl2::math::Vector3& PartStorage::Point(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_PointNumber);
  LUNA_SOL_DIVE_ASSERT(m_PointList != NULL);
  return m_PointList[index];
}
// Radius
const f32& PartStorage::Radius() const
{
  LUNA_SOL_DIVE_ASSERT(m_Radius > 0.0f);
  return m_Radius;
}
// Begin Vertical Direction
const gfl2::math::Vector3& PartStorage::BeginVerticalDirection() const
{
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_BeginVerticalDirection.Length(), 1.0f));
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsAlmostZero(
              m_BeginVerticalDirection.Dot(
                          Point(1) - Point(0))));
  return m_BeginVerticalDirection;
}
// Get Model pointer
poke_3d::model::BaseModel* PartStorage::GetModelPtr()
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
void PartStorage::ReleaseModelPtr(
            poke_3d::model::BaseModel* pModel)
{
  LUNA_SOL_DIVE_ASSERT(pModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_ModelList != NULL);
  const s32 index = pModel - m_ModelList;
  if(0 <= index && index < static_cast<s32>(m_ModelNumber))
  {
    LUNA_SOL_DIVE_ASSERT(GetUsageStatus(index));
    SetUsageStatus(index, false);
    m_ModelList[index].SetPosition(gfl2::math::Vector3::GetZero());
    m_ModelList[index].SetRotation(gfl2::math::Vector3::GetZero());
    m_ModelList[index].ChangeAnimationByResourceNode(NULL, 0);
    m_ModelList[index].ChangeAnimationByResourceNode(NULL, 1);
    return;
  }
  LUNA_SOL_DIVE_ASSERT(false);
}
// Get whether the resource are used or not
bool PartStorage::GetUsageStatus(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < 32);
  return (m_UsageStatus & (0x1 << index)) != 0x0;
}
// Set whether the resource are used or not
void PartStorage::SetUsageStatus(
            const u32& index,
            const bool& isUsed)
{
  LUNA_SOL_DIVE_ASSERT(index < 32);
  LUNA_SOL_DIVE_ASSERT(GetUsageStatus(index) != isUsed);
  const b32 mask = 0x1 << index;
  if(isUsed)
  {
    m_UsageStatus |= mask;
  }
  else
  {
    m_UsageStatus &= ~mask;
  }
}
}// namespace Rail
}// namespace LunaSolDive
