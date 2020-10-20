//======================================================================
/**
 * @file Gimmick_Placement_OrderList.cpp
 * @date 2017/03/02 17:27:58
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./OrderList.h"
#include "../../Assert.h"

namespace LunaSolDive{
namespace Gimmick{
namespace Placement{
// constructor
OrderList::OrderList()
    : m_Size(0),
      m_List(NULL){}
// Initialize
void OrderList::Initialize(
            gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  m_Size = 19;
  m_List = GFL_NEW(pHeap) Order[m_Size];
  {
    u32 i = 0;
    m_List[i++].Initialize(
                0.0f,
                300.0e+2f,
                false,
                1, 0.80f, 0.80f,
                0, 0.00f, 0.00f,
                0, 0.00f, 0.00f);
    m_List[i++].Initialize(
                300.0e+2f,
                600.0e+2f,
                false,
                1, 0.80f, 0.40f,
                1, 0.20f, 0.20f,
                0, 0.00f, 0.00f);
    m_List[i++].Initialize(
                600.0e+2f,
                1000.0e+2f,
                false,
                1, 0.40f, 0.30f,
                1, 0.25f, 0.30f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                1000.0e+2f,
                1500.0e+2f,
                false,
                1, 0.30f, 0.30f,
                1, 0.35f, 0.35f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                1500.0e+2f,
                2000.0e+2f,
                false,
                1, 0.25f, 0.25f,
                1, 0.40f, 0.40f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                2000.0e+2f,
                2100.0e+2f,
                false,
                1, 0.20f, 0.20f,
                2, 1.00f, 1.00f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                2100.0e+2f,
                2500.0e+2f,
                false,
                1, 0.20f, 0.20f,
                1, 0.45f, 0.45f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                2500.0e+2f,
                2600.0e+2f,
                false,
                1, 0.20f, 0.20f,
                2, 1.00f, 1.00f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                2600.0e+2f,
                3000.0e+2f,
                false,
                1, 0.15f, 0.15f,
                1, 0.50f, 0.50f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                3000.0e+2f,
                3100.0e+2f,
                false,
                1, 0.125f, 0.125f,
                2, 1.00f, 1.00f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                3100.0e+2f,
                3500.0e+2f,
                false,
                1, 0.125f, 0.125f,
                1, 0.55f, 0.55f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                3500.0e+2f,
                3600.0e+2f,
                false,
                1, 0.10f, 0.10f,
                2, 1.00f, 1.00f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                3600.0e+2f,
                4000.0e+2f,
                false,
                1, 0.10f, 0.10f,
                1, 0.60f, 0.60f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                4000.0e+2f,
                4100.0e+2f,
                false,
                1, 0.075f, 0.075f,
                2, 1.00f, 1.00f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                4100.0e+2f,
                4500.0e+2f,
                false,
                1, 0.075f, 0.075f,
                1, 0.65f, 0.65f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                4500.0e+2f,
                4600.0e+2f,
                false,
                1, 0.05f, 0.05f,
                2, 1.00f, 1.00f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                4600.0e+2f,
                5000.0e+2f,
                false,
                1, 0.05f, 0.05f,
                1, 0.70f, 0.70f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                5000.0e+2f,
                5100.0e+2f,
                false,
                1, 0.05f, 0.05f,
                2, 1.00f, 1.00f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    m_List[i++].Initialize(
                5100.0e+2f,
                0.0f,
                true,
                0, 0.00f, 0.00f,
                1, 0.75f, 0.75f,
                1, 1.0f / 3.0f, 1.0f / 3.0f);
    LUNA_SOL_DIVE_ASSERT(i == m_Size);
  }
#if PM_DEBUG
  // check
  LUNA_SOL_DIVE_ASSERT(m_Size > 0);
  LUNA_SOL_DIVE_ASSERT(m_List[0].IsFirst());
  for(u32 i = 1; i < m_Size; ++i)
  {
    LUNA_SOL_DIVE_ASSERT(m_List[i - 1].DistanceEnd() == m_List[i].DistanceBegin());
  }
  LUNA_SOL_DIVE_ASSERT(m_List[m_Size - 1].IsLast());
#endif// PM_DEBUG
}
// Terminate
void OrderList::Terminate()
{
  m_Size = 0;
  GFL_SAFE_DELETE(m_List);
}
// Get
const Order& OrderList::Get(
            const f32& distance)
{
  for(u32 i = 0; i < m_Size; ++i)
  {
    if(m_List[i].IsInRange(distance))
    {
      return m_List[i];
    }
  }
  LUNA_SOL_DIVE_ASSERT(false);
  return m_List[m_Size - 1];
}
}// namespace Placement
}// namespace Gimmick
}// namespace LunaSolDive
