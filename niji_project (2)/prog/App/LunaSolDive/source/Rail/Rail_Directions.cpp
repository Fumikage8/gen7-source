//======================================================================
/**
 * @file Rail_Directions.cpp
 * @date 2016/11/07 11:23:38
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <math/include/gfl2_Quaternion.h>
#include "./Directions.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Rail{
// constructor
Directions::Directions()
    : m_Point(gfl2::math::Vector3::GetZero()),
      m_Forward(gfl2::math::Vector3::GetZero()),
      m_Vertical(gfl2::math::Vector3::GetZero()){}
// constructor with Initialize
Directions::Directions(
            const gfl2::math::Vector3& point,
            const gfl2::math::Vector3& forward,
            const gfl2::math::Vector3& vertical)
{
  Initialize(point, forward, vertical);
}
// constructor with Initialize (direction only)
Directions::Directions(
            const gfl2::math::Vector3& forward,
            const gfl2::math::Vector3& vertical)
{
  Initialize(forward, vertical);
}
// Initialize
void Directions::Initialize(
            const gfl2::math::Vector3& point,
            const gfl2::math::Vector3& forward,
            const gfl2::math::Vector3& vertical)
{
  m_Point = point;
  Initialize(forward, vertical);
}
// Initialize (direction only)
void Directions::Initialize(
            const gfl2::math::Vector3& forward,
            const gfl2::math::Vector3& vertical)
{
  m_Forward = forward;
  m_Vertical = vertical;
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_Forward.Length(), 1.0f));
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_Vertical.Length(), 1.0f));
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsAlmostZero(m_Forward.Dot(m_Vertical), EPSILON * 1.0e+1f));
}
// Terminate
void Directions::Terminate()
{
  m_Point = gfl2::math::Vector3::GetZero();
  m_Forward = gfl2::math::Vector3::GetZero();
  m_Vertical = gfl2::math::Vector3::GetZero();
}
// check if this Directions is valid
bool Directions::IsValid() const
{
  return ! gfl2::math::IsAlmostZero(m_Forward.Length())
          && ! gfl2::math::IsAlmostZero(m_Vertical.Length());
}
// Point
const gfl2::math::Vector3& Directions::Point() const
{
  return m_Point;
}
// Forward Direction
const gfl2::math::Vector3& Directions::Forward() const
{
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_Forward.Length(), 1.0f));
  return m_Forward;
}
// Vertical Direction
const gfl2::math::Vector3& Directions::Vertical() const
{
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_Vertical.Length(), 1.0f));
  return m_Vertical;
}
// Horizontal Direction
gfl2::math::Vector3 Directions::Horizontal() const
{
  const gfl2::math::Vector3 vector = Forward().Cross(Vertical());
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(vector.Length(), 1.0f));
  return vector;
}
// Rotation Quaternion
gfl2::math::Quaternion Directions::RotationQuaternion() const
{
  const gfl2::math::Matrix34 matrix = gfl2::math::Matrix34(
              - Horizontal(),
              Vertical(),
              Forward())
              .SetTranslationVector(gfl2::math::Vector3::GetZero());
  gfl2::math::Quaternion quaternion;
  quaternion.MatrixToQuaternion(matrix.Transpose());
  return quaternion;
}
// Translation
void Directions::Translation(
            const gfl2::math::Vector3& displacement)
{
  m_Point += displacement;
}
}// namespace Rail
}// namespace LunaSolDive
