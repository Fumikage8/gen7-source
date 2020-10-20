//======================================================================
/**
 * @file Gimmick_Placement_DistortionLottery.cpp
 * @date 2016/12/12 11:39:48
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "System/include/GflUse.h"
#include "./DistortionLottery.h"
#include "./Parameter.h"
#include "../../Assert.h"
#if PM_DEBUG
#include "../../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Gimmick{
namespace Placement{
// parameter
// Distortion Type Table in First Mode: Row Size
static const u32 TYPE_TABLE_FIRST_ROWS = 5;
// Distortion Type Table in First Mode: Table
static const u8 TypeTableFirst[TYPE_TABLE_FIRST_ROWS * DISTORTION_TYPE__SIZE] = {
        // Ub, Cliff. Water, Cave, Plains
        4, 24, 24, 24, 24,
        16, 21, 21, 21, 21,
        32, 17, 17, 17, 17,
        44, 14, 14, 14, 14,
        100, 0, 0, 0, 0,
        };
// Distortion Type Table in Normal Mode
static const u8 TypeTableNormal[DISTORTION_TYPE__SIZE] = {
        // Ub, Cliff. Water, Cave, Plains
        4, 24, 24, 24, 24,
        };
// Distortion Rarity Table
// Distortion Rarity Table: Row Size
static const u32 RARITY_TABLE_ROWS = 10;
// Distortion Rarity Table: Boundary value of rows by travel distance
static const f32 RarityTableBoundaryDistanceList[RARITY_TABLE_ROWS - 1] = {
        1000.0e+2f,// 1000m
        1500.0e+2f,// 1500m
        2000.0e+2f,// 2000m
        2500.0e+2f,// 2500m
        3000.0e+2f,// 3000m
        3500.0e+2f,// 3500m
        4000.0e+2f,// 4000m
        4500.0e+2f,// 4500m
        5000.0e+2f,// 5000m
        };
// Distortion Rarity Table: Table
static const u8 RarityTable[RARITY_TABLE_ROWS * DISTORTION_RARITY__NONE] = {
        100,  0,  0,  0,// ~ 1000m
         78, 20,  2,  0,// 1000m ~ 1500m
         66, 30,  4,  0,// 1500m ~ 2000m
         52, 40,  8,  0,// 2000m ~ 2500m
         33, 50, 16,  1,// 2500m ~ 3000m
         32, 50, 16,  2,// 3000m ~ 3500m
         31, 50, 16,  3,// 3500m ~ 4000m
         30, 50, 16,  4,// 4000m ~ 5000m
         29, 50, 16,  5,// 4500m ~ 5000m
         29, 50, 16,  5,// 5000m ~
        };
// static function
// Row Index of Rarity Table
static u32 RarityTableRow(
            const f32& distance);
// Look Up Distortion Rarity Table
static const u8& LookUpRarityTable(
            const u32& row,
            const u32& column);
// Get Distortion Type Table
static const u8* GetTypeTable(
            const Mode& mode);
// Select Distortion Type
static DistortionType SelectType(
            const Mode& mode);
// Select Distortion Rarity
static DistortionRarity SelectRarity(
            const f32& distance);
// Select Distortion
static SelectedDistortion SelectDistortion(
            const Mode& mode,
            const f32& distance);
#if PM_DEBUG
// Selection in Fixed Mode
static SelectedDistortion SelectFixedMode();
#endif// PM_DEBUG

// constructor
DistortionLottery::DistortionLottery()
    : m_Mode(MODE__NONE){}
// Initialize
void DistortionLottery::Initialize(
            gfl2::heap::HeapBase* pHeap,
            const Mode& mode)
{
  // set mode
  m_Mode = mode;
  LUNA_SOL_DIVE_ASSERT(m_Mode != MODE__NONE);
}
// Terminate
void DistortionLottery::Terminate()
{
  m_Mode = MODE__NONE;
}
// Select
SelectedDistortion DistortionLottery::Select(
            const f32& distance) const
{
  ARAI_PRINT("Distortion Select: at %.5f[cm]\n", distance);
  const SelectedDistortion result = SelectDistortion(
              m_Mode,
              distance);
  ARAI_PRINT("  result: type %s, rarity %s\n",
              (result.type == DISTORTION_TYPE__UB)? "UB":
              (result.type == DISTORTION_TYPE__CLIFF)? "崖":
              (result.type == DISTORTION_TYPE__WATER)? "水":
              (result.type == DISTORTION_TYPE__CAVE)? "洞窟":
              (result.type == DISTORTION_TYPE__PLAINS)? "平地":
              "Error",
              (result.rarity == DISTORTION_RARITY__0)? "1":
              (result.rarity == DISTORTION_RARITY__1)? "2":
              (result.rarity == DISTORTION_RARITY__2)? "3":
              (result.rarity == DISTORTION_RARITY__3)? "4":
              "Error");
  return result;
}
// static function
// Row Index of Rarity Table
static u32 RarityTableRowIndex(
            const f32& distance)
{
  LUNA_SOL_DIVE_ASSERT(distance > -EPSILON);
  for(u32 i = 0; i< RARITY_TABLE_ROWS - 1; ++i)
  {
    if(distance < RarityTableBoundaryDistanceList[i])
    {
      return i;
    }
  }
  return RARITY_TABLE_ROWS - 1;
}
// Look Up Distortion Rarity Table
static const u8& LookUpRarityTable(
            const u32& row,
            const u32& column)
{
  LUNA_SOL_DIVE_ASSERT(row < RARITY_TABLE_ROWS);
  LUNA_SOL_DIVE_ASSERT(column < DISTORTION_RARITY__NONE);
  const u32 index = row * DISTORTION_RARITY__NONE + column;
  return RarityTable[index];
}
// Get Distortion Type Table
static const u8* GetTypeTable(
            const Mode& mode)
{
  LUNA_SOL_DIVE_ASSERT(mode != MODE__NONE);
  if(mode == MODE__FIRST)
  {// first mode
    const u32 recordCount = std::min(
                Parameter::Get().RecordCount(),
                TYPE_TABLE_FIRST_ROWS - 1);
    return TypeTableFirst + recordCount * DISTORTION_TYPE__SIZE;
  }
  else
  {// normal mode
    return TypeTableNormal;
  }
}
// Select Distortion Type
static DistortionType SelectType(
            const Mode& mode)
{
  const u8* const typeTable = GetTypeTable(mode);
  const u32 randomNumber = System::GflUse::GetPublicRand(100);
  u32 accumulation = 0;
  DistortionType result = DISTORTION_TYPE__SIZE;
  for(u32 i = 0; i < DISTORTION_TYPE__SIZE; ++i)
  {
    accumulation += typeTable[i];
    if(randomNumber < accumulation)
    {
      result = static_cast<DistortionType>(i);
      break;
    }
  }
  LUNA_SOL_DIVE_ASSERT(result != DISTORTION_TYPE__SIZE);
  ARAI_PRINT("  type: %02d -> %s\n",
              randomNumber,
              (result == DISTORTION_TYPE__UB)? "UB":
              (result == DISTORTION_TYPE__CLIFF)? "崖":
              (result == DISTORTION_TYPE__WATER)? "水":
              (result == DISTORTION_TYPE__CAVE)? "洞窟":
              (result == DISTORTION_TYPE__PLAINS)? "平地":
              "Error");
  return (result != DISTORTION_TYPE__SIZE)
          ? result
          : DISTORTION_TYPE__PLAINS;
}
// Select Distortion Rarity
static DistortionRarity SelectRarity(
            const f32& distance)
{
  const u32 randomNumber = System::GflUse::GetPublicRand(100);
  u32 accumulation = 0;
  DistortionRarity result = DISTORTION_RARITY__NONE;
  const u32 rowIndex = RarityTableRowIndex(distance);
  for(u32 i = 0; i < DISTORTION_RARITY__NONE; ++i)
  {
    accumulation += LookUpRarityTable(rowIndex, i);
    if(randomNumber < accumulation)
    {
      result = static_cast<DistortionRarity>(i);
      break;
    }
  }
  LUNA_SOL_DIVE_ASSERT(result != DISTORTION_RARITY__NONE);
  ARAI_PRINT("  rarity: %02d -> %s\n",
              randomNumber,
              (result == DISTORTION_RARITY__0)? "1":
              (result == DISTORTION_RARITY__1)? "2":
              (result == DISTORTION_RARITY__2)? "3":
              (result == DISTORTION_RARITY__3)? "4":
              "Error");
  return (result != DISTORTION_RARITY__NONE)
          ? result
          : DISTORTION_RARITY__0;
}
// Select Distortion
static SelectedDistortion SelectDistortion(
            const Mode& mode,
            const f32& distance)
{
#if PM_DEBUG
  if(Debug::Parameter::Get().isDistortionTypeFixed
          || Debug::Parameter::Get().isDistortionRarityFixed)
  {
    return SelectFixedMode();
  }
#endif// PM_DEBUG
  const SelectedDistortion result = {
              SelectType(mode),
              SelectRarity(distance)};
  return result;
}
#if PM_DEBUG
// Selection in Fixed Mode
static SelectedDistortion SelectFixedMode()
{
  const DistortionType type =
          Debug::Parameter::Get().isDistortionTypeFixed
          ? Debug::Parameter::Get().distortionTypeInFixedMode
          : static_cast<DistortionType>(System::GflUse::GetPublicRand(DISTORTION_TYPE__SIZE));
  const DistortionRarity rarity =
          Debug::Parameter::Get().isDistortionRarityFixed
          ? Debug::Parameter::Get().distortionRarityInFixedMode
          : static_cast<DistortionRarity>(System::GflUse::GetPublicRand(DISTORTION_RARITY__NONE));
  const SelectedDistortion result = {
              type,
              rarity};
  return result;
}
#endif// PM_DEBUG
}// namespace Placement
}// namespace Gimmick
}// namespace LunaSolDive
