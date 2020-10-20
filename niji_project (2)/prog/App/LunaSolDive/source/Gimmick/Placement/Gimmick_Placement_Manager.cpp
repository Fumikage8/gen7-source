//======================================================================
/**
 * @file Gimmick_Placement_Manager.cpp
 * @date 2016/12/08 17:40:17
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "System/include/GflUse.h"
#include "./DistortionLottery.h"
#include "./Manager.h"
#include "./OrderList.h"
#include "./Parameter.h"
#include "../Energy.h"
#include "../Distortion.h"
#include "../ObjectList.h"
#include "../Obstacle.h"
#include "../Storage.h"
#include "../../Assert.h"
#include "../../Rail/Manager.h"

namespace LunaSolDive{
namespace Gimmick{
namespace Placement{
// static function
// Get Random f32 (range [0.0: 1.0])
static f32 GetRandomF32();
// Select Cross-Section Point
static gfl2::math::Vector2 SelectCrossSectionPoint(
            const GimmickType& type);
// constructor
Manager::Manager()
    : m_Mode(MODE__NONE),
      m_pRailManager(NULL),
      m_pStorage(NULL),
      m_pObjectList(NULL),
      m_pDistortionLottery(NULL),
      m_pOrderList(NULL),
      m_TriggerPoint(0.0f){}
// Initialize
void Manager::Initialize(
            gfl2::heap::HeapBase* pHeap,
            const Mode& mode,
            const Rail::Manager* pRailManager,
            Storage* pStorage,
            const ObjectList* pObjectList)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  m_Mode = mode;
  LUNA_SOL_DIVE_ASSERT(m_Mode != MODE__NONE);
  // set pointer
  m_pRailManager = pRailManager;
  LUNA_SOL_DIVE_ASSERT(m_pRailManager != NULL);
  m_pStorage = pStorage;
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  m_pObjectList = pObjectList;
  LUNA_SOL_DIVE_ASSERT(m_pObjectList != NULL);
  // initialize DistortionLottery
  m_pDistortionLottery = GFL_NEW(pHeap) DistortionLottery();
  LUNA_SOL_DIVE_ASSERT(m_pDistortionLottery != NULL);
  m_pDistortionLottery->Initialize(
              pHeap,
              mode);
  // initialize OrderList
  m_pOrderList = GFL_NEW(pHeap) OrderList();
  LUNA_SOL_DIVE_ASSERT(m_pOrderList != NULL);
  m_pOrderList->Initialize(
              pHeap);
}
// Terminate
void Manager::Terminate()
{
  // delete DistortionLottery
  if(m_pDistortionLottery != NULL)
  {
    m_pDistortionLottery->Terminate();
    GFL_SAFE_DELETE(m_pDistortionLottery);
    m_pDistortionLottery = NULL;
  }
  // delete OrderList
  if(m_pOrderList != NULL)
  {
    m_pOrderList->Terminate();
    GFL_SAFE_DELETE(m_pOrderList);
    m_pOrderList = NULL;
  }
  // unset pointer
  m_pRailManager = NULL;
  m_pStorage = NULL;
  m_pObjectList = NULL;
  // reset parameter
  m_Mode = MODE__NONE;
  m_TriggerPoint = 0.0f;
}
// Initial Placement
void Manager::InitialPlacement()
{
  const s32 unitsNumber = std::ceil(
              (Parameter::Get().InitialPlacementRange()
                      - Parameter::Get().InitialPlacementForbiddenRange()
                      + EPSILON)
              / UnitLength());
  LUNA_SOL_DIVE_ASSERT(unitsNumber >= 0);
  for(s32 i = 0; i < unitsNumber; ++i)
  {
    const f32 distance =
            Parameter::Get().InitialPlacementForbiddenRange()
            + i * UnitLength();
    ARAI_PRINT("execute initial placement: %.5f[cm]\n", distance);
    const Order& order = m_pOrderList->Get(distance);
    ExecuteOrder(
                order,
                distance);
  }
  m_TriggerPoint = std::max(
              Parameter::Get().InitialPlacementForbiddenRange()
              + (unitsNumber - Parameter::Get().AheadUnits()) * UnitLength(),
              0.0f);
}
// Update
void Manager::Update()
{
  if(m_TriggerPoint < m_pRailManager->Location().TravelDistance())
  {
    Execute();
  }
}
// Execute placement
void Manager::Execute()
{
  const f32 distance =
          m_TriggerPoint + Parameter::Get().AheadUnits() * UnitLength();
  ARAI_PRINT("execute placement: %.5f[cm]\n", distance);
  const Order& order = m_pOrderList->Get(distance);
  ExecuteOrder(
              order,
              distance);
  // increment trigger point
  m_TriggerPoint += UnitLength();
}
// Execute placement as ordered
void Manager::ExecuteOrder(
            const Order& order,
            const f32& distance)
{
  LUNA_SOL_DIVE_ASSERT(order.IsInRange(distance));
  // energy
  for(u32 i = 0; i < order.EnergyTrialNumber(); ++i)
  {
    if(GetRandomF32() < order.EnergyProbability(distance))
    {
      PlaceEnergyRandom(distance);
    }
  }
  // obstacle
  for(u32 i = 0; i < order.ObstacleTrialNumber(); ++i)
  {
    if(GetRandomF32() < order.ObstacleProbability(distance))
    {
      PlaceObstacleRandom(distance);
    }
  }
  // distortion
  for(u32 i = 0; i < order.DistortionTrialNumber(); ++i)
  {
    if(GetRandomF32() < order.DistortionProbability(distance))
    {
      PlaceDistortionRandom(distance);
    }
  }
}
// Place Energy at random
void Manager::PlaceEnergyRandom(
            const f32& distance) const
{
  if(Energy* const pEnergy = m_pObjectList->FreeEnergyPtr())
  {
    const f32 forward =
            distance - m_pRailManager->Location().TravelDistance()
            + BlockSize() * UnitLength() * GetRandomF32();
    const gfl2::math::Vector2 point = SelectCrossSectionPoint(
                pEnergy->Type());
    const Rail::Location location = m_pRailManager->Location(
                forward,
                point);
    if(location.IsValid()
            && m_pObjectList->IsPlacementPossible(
                          pEnergy->Type(),
                          location))
    {
      m_pStorage->SetUp(
                  pEnergy,
                  location);
    }
  }
}
// Place Obstacle at random
void Manager::PlaceObstacleRandom(
            const f32& distance) const
{
  if(Obstacle* const pObstacle = m_pObjectList->FreeObstaclePtr())
  {
    const f32 forward =
            distance - m_pRailManager->Location().TravelDistance()
            + BlockSize() * UnitLength() * GetRandomF32();
    const gfl2::math::Vector2 point = SelectCrossSectionPoint(
                pObstacle->Type());
    const Rail::Location location = m_pRailManager->Location(
                forward,
                point);
    if(location.IsValid()
            && m_pObjectList->IsPlacementPossible(
                          pObstacle->Type(),
                          location))
    {
      m_pStorage->SetUp(
                  pObstacle,
                  location);
    }
  }
}
// Place Distortion at random
void Manager::PlaceDistortionRandom(
            const f32& distance) const
{
  if(Distortion* const pDistortion = m_pObjectList->FreeDistortionPtr())
  {
    const SelectedDistortion distortion = m_pDistortionLottery->Select(distance);
    const f32 forward =
            distance - m_pRailManager->Location().TravelDistance()
            + BlockSize() * UnitLength() * GetRandomF32();
    const gfl2::math::Vector2 point =
            (m_Mode == MODE__FIRST && distortion.type == DISTORTION_TYPE__UB)
            ? gfl2::math::Vector2::GetZero()
            : SelectCrossSectionPoint(pDistortion->Type());
    const Rail::Location location = m_pRailManager->Location(
                forward,
                point);
    if(location.IsValid()
            && m_pObjectList->IsPlacementPossible(
                          pDistortion->Type(),
                          location))
    {
      m_pStorage->SetUp(
                  pDistortion,
                  distortion.type,
                  distortion.rarity,
                  location);
    }
  }
}
// Unit Length
const f32& Manager::UnitLength() const
{
  return Parameter::Get().UnitLength();
}
// Block Size
const u32& Manager::BlockSize() const
{
  return Parameter::Get().BlockSize();
}
// static function
// Get Random f32 (range [0.0: 1.0])
static f32 GetRandomF32()
{
  return static_cast<f32>(System::GflUse::GetPublicRand()) / 0xffffffff;
}
// Select Cross Section Point
static gfl2::math::Vector2 SelectCrossSectionPoint(
            const GimmickType& type)
{
  LUNA_SOL_DIVE_ASSERT(type != GIMMICK_TYPE__SIZE);
  if(type == GIMMICK_TYPE__DISTORTION)
  {// Distortion
    const f32 radius = 0.75f + 0.25f * GetRandomF32();
    const f32 angle = PI2 * GetRandomF32();
    return gfl2::math::Vector2(
                radius * gfl2::math::FCos(angle),
                radius * gfl2::math::FSin(angle));
  }
  else
  {// Energy or Obstacle
    const u32 maxTrialNumber = 10;
    u32 i = 0;
    while(i < maxTrialNumber)
    {
      const f32 x = -1.0f + 2.0f * GetRandomF32();
      const f32 y = -1.0f + 2.0f * GetRandomF32();
      if(std::hypot(x, y) <= 1.0f)
      {
        return gfl2::math::Vector2(x, y);
      }
      ++i;
    }
    return gfl2::math::Vector2::GetZero();
  }
}
}// namespace Placement
}// namespace Gimmick
}// namespace LunaSolDive
