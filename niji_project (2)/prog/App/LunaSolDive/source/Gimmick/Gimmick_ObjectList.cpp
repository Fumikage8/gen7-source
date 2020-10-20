//======================================================================
/**
 * @file Gimmick_ObjectList.cpp
 * @date 2016/11/28 22:32:50
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./Distortion.h"
#include "./Energy.h"
#include "./ObjectList.h"
#include "./Obstacle.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Gimmick{
// static function
// Whether the distortions overlap each other not
static bool IsDistortionOverlapping(
            const Rail::Location& location0,
            const Rail::Location& location1);
// constructor
ObjectList::ObjectList()
    : m_EnergyNumber(0),
      m_EnergyList(NULL),
      m_ObstacleNumber(0),
      m_ObstacleList(NULL),
      m_DistortionNumber(0),
      m_DistortionList(NULL){}
// Initialize
void ObjectList::Initialize(
            gfl2::heap::HeapBase* pHeap,
            const u32& energyNumber,
            const u32& obstacleNumber,
            const u32& distortionNumber)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  // initialize Energy
  m_EnergyNumber = energyNumber;
  m_EnergyList = GFL_NEW(pHeap) Gimmick::Energy[m_EnergyNumber];
  LUNA_SOL_DIVE_ASSERT(m_EnergyNumber != NULL);
  // initialize Obstacle
  m_ObstacleNumber = obstacleNumber;
  m_ObstacleList = GFL_NEW(pHeap) Gimmick::Obstacle[m_ObstacleNumber];
  LUNA_SOL_DIVE_ASSERT(m_ObstacleNumber != NULL);
  // initialize Distortion
  m_DistortionNumber = distortionNumber;
  m_DistortionList = GFL_NEW(pHeap) Gimmick::Distortion[m_DistortionNumber];
  LUNA_SOL_DIVE_ASSERT(m_DistortionNumber != NULL);
}
// Termiante
void ObjectList::Terminate()
{
  // delete Energy
  if(m_EnergyList != NULL)
  {
    for(u32 i = 0; i < m_EnergyNumber; ++i)
    {
      m_EnergyList[i].Terminate();
    }
    GFL_DELETE_ARRAY(m_EnergyList);
    m_EnergyList = NULL;
  }
  // delete Obstacle
  if(m_ObstacleList != NULL)
  {
    for(u32 i = 0; i < m_ObstacleNumber; ++i)
    {
      m_ObstacleList[i].Terminate();
    }
    GFL_DELETE_ARRAY(m_ObstacleList);
    m_ObstacleList = NULL;
  }
  // delete Distortion
  if(m_DistortionList != NULL)
  {
    for(u32 i = 0; i < m_DistortionNumber; ++i)
    {
      m_DistortionList[i].Terminate();
    }
    GFL_DELETE_ARRAY(m_DistortionList);
    m_DistortionList = NULL;
  }
  // reset parameter
  m_EnergyNumber = 0;
  m_ObstacleNumber = 0;
  m_DistortionNumber = 0;
}
// The number of energy
const u32& ObjectList::EnergyNumber() const
{
  return m_EnergyNumber;
}
// Energy
Energy& ObjectList::Energy(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_EnergyNumber);
  return m_EnergyList[index];
}
// The number of obstacle
const u32& ObjectList::ObstacleNumber() const
{
  return m_ObstacleNumber;
}
// Obstacle
Obstacle& ObjectList::Obstacle(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_ObstacleNumber);
  return m_ObstacleList[index];
}
// The number of distortion
const u32& ObjectList::DistortionNumber() const
{
  return m_DistortionNumber;
}
// Distortion
Distortion& ObjectList::Distortion(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_DistortionNumber);
  return m_DistortionList[index];
}
// Free Energy pointer
Energy* ObjectList::FreeEnergyPtr() const
{
  LUNA_SOL_DIVE_ASSERT(m_EnergyList != NULL);
  for(u32 i = 0; i < m_EnergyNumber; ++i)
  {
    if(! m_EnergyList[i].IsValid())
    {
      return &m_EnergyList[i];
    }
  }
  return NULL;
}
// Free Obstacle pointer
Obstacle* ObjectList::FreeObstaclePtr() const
{
  LUNA_SOL_DIVE_ASSERT(m_ObstacleList != NULL);
  for(u32 i = 0; i < m_ObstacleNumber; ++i)
  {
    if(! m_ObstacleList[i].IsValid())
    {
      return &m_ObstacleList[i];
    }
  }
  return NULL;
}
// Free Distortion pointer
Distortion* ObjectList::FreeDistortionPtr() const
{
  LUNA_SOL_DIVE_ASSERT(m_DistortionList != NULL);
  for(u32 i = 0; i < m_DistortionNumber; ++i)
  {
    if(! m_DistortionList[i].IsValid())
    {
      return &m_DistortionList[i];
    }
  }
  return NULL;
}
// Translation
void ObjectList::Translation(
            const gfl2::math::Vector3& displacement) const
{
  // energy
  for(u32 i = 0; i < m_EnergyNumber; ++i)
  {
    if(! m_EnergyList[i].IsValid()) continue;
    m_EnergyList[i].Translation(displacement);
  }
  // obstacle
  for(u32 i = 0; i < m_ObstacleNumber; ++i)
  {
    if(! m_ObstacleList[i].IsValid()) continue;
    m_ObstacleList[i].Translation(displacement);
  }
  // distortion
  for(u32 i = 0; i < m_DistortionNumber; ++i)
  {
    if(! m_DistortionList[i].IsValid()) continue;
    m_DistortionList[i].Translation(displacement);
  }
}
// Whether the object can be placed or not
bool ObjectList::IsPlacementPossible(
            const GimmickType& type,
            const Rail::Location& location) const
{
  const f32 energyRadius = 120.0f;
  const f32 obstacleRadius = 120.0f;
  const f32 distortionRadius = 450.0f;
  const f32 objectRadius =
          (type == GIMMICK_TYPE__ENERGY)
          ? energyRadius
          : (type == GIMMICK_TYPE__OBSTACLE)
          ? obstacleRadius
          : distortionRadius;
  // Energy
  for(u32 i = 0; i < m_EnergyNumber; ++i)
  {
    const Gimmick::Energy& energy = Energy(i);
    if(! energy.IsValid()) continue;
    const f32 distance = (energy.Location().Point() - location.Point()).Length();
    if(distance < objectRadius + energyRadius)
    {
      return false;
    }
  }
  // Obstacle
  for(u32 i = 0; i < m_ObstacleNumber; ++i)
  {
    const Gimmick::Obstacle& obstacle = Obstacle(i);
    if(! obstacle.IsValid()) continue;
    const f32 distance = (obstacle.Location().Point() - location.Point()).Length();
    if(distance < objectRadius + obstacleRadius)
    {
      return false;
    }
  }
  // Distortion
  for(u32 i = 0; i < m_DistortionNumber; ++i)
  {
    const Gimmick::Distortion& distortion = Distortion(i);
    if(! distortion.IsValid()) continue;
    const f32 distance = (distortion.Location().Point() - location.Point()).Length();
    if((distance < objectRadius + distortionRadius)
            || (type == GIMMICK_TYPE__DISTORTION
                  && IsDistortionOverlapping(distortion.Location(), location)))
    {
      return false;
    }
  }
  return true;
}
// Whether the distortions overlap each other not
static bool IsDistortionOverlapping(
            const Rail::Location& location0,
            const Rail::Location& location1)
{
  // threshold
  const f32 sideThreshold = 1.0f;
  const f32 distanceThreshold = 40.0e+2f;
  // interval
  const f32 distanceInterval = gfl2::math::FAbs(
              location1.TravelDistance() - location0.TravelDistance());
  const f32 sideInterval = (
          location1.NormalizedCrossSectionPoint()
          - location0.NormalizedCrossSectionPoint()
          ).Length();
  // judge
  return sideInterval < sideThreshold
          && distanceInterval < distanceThreshold;
}
}// namespace Gimmick
}// namespace LunaSolDive
