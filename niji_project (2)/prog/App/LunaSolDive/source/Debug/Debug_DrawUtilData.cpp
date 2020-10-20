//======================================================================
/**
 * @file Debug_DrawUtilData.cpp
 * @date 2017/03/14 18:40:16
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#include "./DrawUtilData.h"
#include "../Assert.h"
#include "../Gimmick/Parameter.h"
#include "../Gimmick/Collision/Cylinder.h"
#include "../Gimmick/Collision/Hemisphere.h"
#include "../Rail/Section.h"

namespace LunaSolDive{
namespace Debug{
// parameter
static const u32 RAIL_SECTION_LIST_SIZE = 4;
// constructor
DrawUtilData::DrawUtilData()
    : m_PresentLocation(),
      m_InputPoint(gfl2::math::Vector3::GetZero()),
      m_CollisionPoint(gfl2::math::Vector3::GetZero()),
      m_RailSectionListSize(0),
      m_RailSectionList(NULL),
      m_GimmickEnergyCollisionListSize(0),
      m_GimmickEnergyCollisionList(NULL),
      m_GimmickObstacleCollisionListSize(0),
      m_GimmickObstacleCollisionList(NULL),
      m_GimmickDistortionCollisionListSize(0),
      m_GimmickDistortionCollisionList(NULL){}
// Initialize
void DrawUtilData::Initialize(
            gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  m_RailSectionList = GFL_NEW(pHeap) Rail::Section[RAIL_SECTION_LIST_SIZE];
  m_GimmickEnergyCollisionList = GFL_NEW(pHeap) Gimmick::Collision::Cylinder[Gimmick::Parameter::ENERGY_NUMBER];
  m_GimmickObstacleCollisionList = GFL_NEW(pHeap) Gimmick::Collision::Cylinder[Gimmick::Parameter::OBSTACLE_NUMBER];
  m_GimmickDistortionCollisionList = GFL_NEW(pHeap) Gimmick::Collision::Hemisphere[Gimmick::Parameter::DISTORTION_NUMBER];
}
// Terminate
void DrawUtilData::Terminate()
{
  Reset();
  GFL_SAFE_DELETE(m_RailSectionList);
  GFL_SAFE_DELETE(m_GimmickEnergyCollisionList);
  GFL_SAFE_DELETE(m_GimmickObstacleCollisionList);
  GFL_SAFE_DELETE(m_GimmickDistortionCollisionList);
}
// Reset
void DrawUtilData::Reset()
{
  m_PresentLocation.Terminate();
  m_InputPoint = gfl2::math::Vector3::GetZero();
  m_CollisionPoint = gfl2::math::Vector3::GetZero();
  m_RailSectionListSize = 0;
  m_GimmickEnergyCollisionListSize = 0;
  m_GimmickObstacleCollisionListSize = 0;
  m_GimmickDistortionCollisionListSize = 0;
};
// Present Location: getter
const Rail::Location& DrawUtilData::PresentLocation() const
{
  return m_PresentLocation;
}
// Present Location: setter
void DrawUtilData::PresentLocation(
            const Rail::Location& value)
{
  m_PresentLocation = value;
}
// Input Point: getter
const gfl2::math::Vector3& DrawUtilData::InputPoint() const
{
  return m_InputPoint;
}
// Input Point: setter
void DrawUtilData::InputPoint(
            const gfl2::math::Vector3& value)
{
  m_InputPoint = value;
}
// Collision Point: getter
const gfl2::math::Vector3& DrawUtilData::CollisionPoint() const
{
  return m_CollisionPoint;
}
// Collision Point: setter
void DrawUtilData::CollisionPoint(
            const gfl2::math::Vector3& value)
{
  m_CollisionPoint = value;
}
// Rail::Section List Size
const u32& DrawUtilData::RailSectionListSize() const
{
  return m_RailSectionListSize;
}
// Rail::Section: getter
const Rail::Section& DrawUtilData::RailSection(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_RailSectionListSize);
  return m_RailSectionList[index];
}
// Rail::Section: setter
void DrawUtilData::RailSection(
            const Rail::Section& value)
{
  LUNA_SOL_DIVE_ASSERT(m_RailSectionListSize < RAIL_SECTION_LIST_SIZE);
  if(m_RailSectionListSize < RAIL_SECTION_LIST_SIZE)
  {
    m_RailSectionList[m_RailSectionListSize] = value;
    ++m_RailSectionListSize;
  }
}
// Gimmick::Energy Collision List Size
const u32& DrawUtilData::GimmickEnergyCollisionListSize() const
{
  return m_GimmickEnergyCollisionListSize;
}
// Gimmick::Energy Collision: getter
const Gimmick::Collision::Cylinder& DrawUtilData::GimmickEnergyCollision(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_GimmickEnergyCollisionListSize);
  return m_GimmickEnergyCollisionList[index];
}
// Gimmick::Energy Collision: setter
void DrawUtilData::GimmickEnergyCollision(
            const Gimmick::Collision::Cylinder& value)
{
  LUNA_SOL_DIVE_ASSERT(m_RailSectionListSize < Gimmick::Parameter::ENERGY_NUMBER);
  if(m_GimmickEnergyCollisionListSize < Gimmick::Parameter::ENERGY_NUMBER)
  {
    m_GimmickEnergyCollisionList[m_GimmickEnergyCollisionListSize] = value;
    ++m_GimmickEnergyCollisionListSize;
  }
}
// Gimmick::Obstacle Collision List Size
const u32& DrawUtilData::GimmickObstacleCollisionListSize() const
{
  return m_GimmickObstacleCollisionListSize;
}
// Gimmick::Obstacle Collision: getter
const Gimmick::Collision::Cylinder& DrawUtilData::GimmickObstacleCollision(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_GimmickObstacleCollisionListSize);
  return m_GimmickObstacleCollisionList[index];
}
// Gimmick::Obstacle Collision: setter
void DrawUtilData::GimmickObstacleCollision(
            const Gimmick::Collision::Cylinder& value)
{
  LUNA_SOL_DIVE_ASSERT(m_RailSectionListSize < Gimmick::Parameter::OBSTACLE_NUMBER);
  if(m_GimmickObstacleCollisionListSize < Gimmick::Parameter::OBSTACLE_NUMBER)
  {
    m_GimmickObstacleCollisionList[m_GimmickObstacleCollisionListSize] = value;
    ++m_GimmickObstacleCollisionListSize;
  }
}
// Gimmick::Distortion Collision List Size
const u32& DrawUtilData::GimmickDistortionCollisionListSize() const
{
  return m_GimmickDistortionCollisionListSize;
}
// Gimmick::Distortion Collision: getter
const Gimmick::Collision::Hemisphere& DrawUtilData::GimmickDistortionCollision(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_GimmickDistortionCollisionListSize);
  return m_GimmickDistortionCollisionList[index];
}
// Gimmick::Distortion Collision: setter
void DrawUtilData::GimmickDistortionCollision(
            const Gimmick::Collision::Hemisphere& value)
{
  LUNA_SOL_DIVE_ASSERT(m_RailSectionListSize < Gimmick::Parameter::DISTORTION_NUMBER);
  if(m_GimmickDistortionCollisionListSize < Gimmick::Parameter::DISTORTION_NUMBER)
  {
    m_GimmickDistortionCollisionList[m_GimmickDistortionCollisionListSize] = value;
    ++m_GimmickDistortionCollisionListSize;
  }
}
}// namespace Debug
}// namespace LunaSolDive
#endif// PM_DEBUG
