//======================================================================
/**
 * @file Gimmick_Placement_Parameter.cpp
 * @date 2017/01/31 15:18:05
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Savedata/include/Record.h"
#include "./Parameter.h"
#include "../../Assert.h"
#if PM_DEBUG
#include "../../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Gimmick{
namespace Placement{
// Initialize
void Parameter::Initialize(
            gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Parameter();
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
#if PM_DEBUG
  Debug::Parameter::Get().pGimmickPlacementParameter = pInstance;
#endif// PM_DEBUG
}
// Terminate
void Parameter::Terminate()
{
  GFL_SAFE_DELETE(pInstance);
}
// getter
const Parameter& Parameter::Get()
{
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  return *pInstance;
}
// instance
Parameter* Parameter::pInstance = NULL;
// constructor
Parameter::Parameter()
{
  Reset();
}
// Reset
void Parameter::Reset()
{
  m_RecordCount = Savedata::GetRecord(Savedata::Record::RECID_LUNASOL_DIVE_CNT);
  m_UnitLength = 15.0e+2f;
  m_BlockSize = 4;
  m_AheadUnits = 10;
  // Initial Placement
  m_InitialPlacementRange = 300.0e+2f;
  m_InitialPlacementForbiddenRange = 50.0e+2f;
}
// Record Count
const u32& Parameter::RecordCount() const
{
  return m_RecordCount;
}
// Unit Length
const f32& Parameter::UnitLength() const
{
  return m_UnitLength;
}
// Block Size
const u32& Parameter::BlockSize() const
{
  return m_BlockSize;
}
// Number of units ahead should be placed
const u32& Parameter::AheadUnits() const
{
  return m_AheadUnits;
}
// Initial Placement: Range
const f32& Parameter::InitialPlacementRange() const
{
  return m_InitialPlacementRange;
}
// Initial Placement: Forbidden Range
const f32& Parameter::InitialPlacementForbiddenRange() const
{
  return m_InitialPlacementForbiddenRange;
}
}// namespace Placement
}// namespace Gimmick
}// namespace LunaSolDive
