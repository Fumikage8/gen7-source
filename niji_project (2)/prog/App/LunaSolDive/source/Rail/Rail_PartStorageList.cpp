//======================================================================
/**
 * @file Rail_PartStorageList.cpp
 * @date 2016/10/24 17:08:26
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <heap/include/gfl2_Heap.h>
#include "./Factory.h"
#include "./Parameter.h"
#include "./Part.h"
#include "./PartStorage.h"
#include "./PartStorageList.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Rail{
// parameter
static const u32 ModelNumber = 2;
// constructor
PartStorageList::PartStorageList()
    : m_Size(0),
      m_Elements(NULL),
      m_pRenderingPipeline(NULL){}
// Initialize
void PartStorageList::Initialize(
            gfl2::heap::HeapBase* pHeap,
            RenderingPipeline* pRenderingPipeline)
{
  // set pointer
  m_pRenderingPipeline = pRenderingPipeline;
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  // initialize: Part Storage
  InitializePartStorage(
              pHeap);
#if PM_DEBUG
  // Shape Check
  for(s32 i = 0; i < PART_SHAPE__SIZE; ++i)
  {
    LUNA_SOL_DIVE_ASSERT(m_Elements[i].Shape() == i);
  }
#endif // PM_DEBUG
}
// Initialize: Part Storage (temporary)
void PartStorageList::InitializePartStorage(
            gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(m_Elements == NULL);
  const f32& radius = Parameter::Get().Radius();
  m_Size = PART_SHAPE__SIZE;
  m_Elements = GFL_NEW(pHeap) PartStorage[PART_SHAPE__SIZE];
  // create: Straight
  const u32 straightLineSize = 10;
  gfl2::math::Vector3* const straightLine = GFL_NEW(pHeap) gfl2::math::Vector3[straightLineSize];
  for(u32 i = 0; i < straightLineSize; ++i)
  {
    straightLine[i] = gfl2::math::Vector3(
                0.0f, 0.0f, -10000.0f * i / (straightLineSize - 1));
  }
  m_Elements[0].Initialize(
              pHeap,
              PART_SHAPE__STRAIGHT,
              ModelNumber,
              straightLineSize,
              straightLine,
              radius,
              gfl2::math::Vector3::GetYUnit());
  // create: Curve (temporaty)
  const u32 curveLineSize = 10;
  gfl2::math::Vector3* const curveLine = GFL_NEW(pHeap) gfl2::math::Vector3[curveLineSize];
  for(u32 i = 0; i < curveLineSize; ++i)
  {
    curveLine[i] = gfl2::math::Vector3(
                10000.0f * (1.0f - gfl2::math::FCos((PI / 2.0f) * i / (curveLineSize - 1))),
                0.0f,
                - 10000.0f * gfl2::math::FSin((PI / 2.0f) * i / (curveLineSize - 1)));
  }
  m_Elements[1].Initialize(
              pHeap,
              PART_SHAPE__RIGHT_CURVE,
              ModelNumber,
              curveLineSize,
              curveLine,
              radius,
              gfl2::math::Vector3::GetYUnit());
}
// Create
void PartStorageList::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator)
{
  // Create Model
  for(u32 i = 0; i < m_Size; ++i)
  {
    m_Elements[i].Create(
                pHeap,
                pAllocator);
  }
}
// Terminate
void PartStorageList::Terminate()
{
  // delete Elements
  if(m_Elements != NULL)
  {
    for(u32 i = 0; i < m_Size; ++i)
    {
      m_Elements[i].Terminate();
    }
    GFL_DELETE_ARRAY(m_Elements);
    m_Elements = NULL;
  }
  // reset parameter
  m_Size = 0;
  // unset pointer
  m_pRenderingPipeline = NULL;
}
// Size
const u32& PartStorageList::Size() const
{
  return m_Size;
}
// Get
const PartStorage& PartStorageList::Get(
            const PartShape& shape) const
{
  LUNA_SOL_DIVE_ASSERT(shape != PART_SHAPE__SIZE);
  LUNA_SOL_DIVE_ASSERT(m_Elements != NULL);
  return m_Elements[shape];
}
// Get pointer
PartStorage* PartStorageList::GetPtr(
            const PartShape& shape) const
{
  LUNA_SOL_DIVE_ASSERT(shape != PART_SHAPE__SIZE);
  LUNA_SOL_DIVE_ASSERT(m_Elements != NULL);
  return &m_Elements[shape];
}
// Set Up
void PartStorageList::SetUp(
            Part* pTarget,
            const PartSetUpParameter& parameter) const
{
  LUNA_SOL_DIVE_ASSERT(pTarget != NULL);
  LUNA_SOL_DIVE_ASSERT(! pTarget->IsValid());
  pTarget->Initialize(
              GetPtr(parameter.shape),
              m_pRenderingPipeline);
  pTarget->Rotation(
              parameter.angle);
}
}// namespace Rail
}// namespace LunaSolDive
