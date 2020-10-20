//======================================================================
/**
 * @file ColliderCylinder.cpp
 * @date 2016/10/14 16:31:14
 * @author fang_yicheng
 * @brief シリンダーコライダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "../Model/ModelBase.h"
#include "ColliderCylinder.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Collision)

ColliderCylinder::ColliderCylinder()
{
  m_type    = TYPE_CYLINDER;
  m_radius  = 0.0f;
  m_height  = 0.0f;
}

ColliderCylinder::~ColliderCylinder()
{
}

f32 ColliderCylinder::GetRadius()
{
  return m_radius;
}
 
void ColliderCylinder::SetRadius(f32 radius)
{
  m_radius = radius;
}

f32 ColliderCylinder::GetHeight()
{
  return m_height;
}

void ColliderCylinder::SetHeight(f32 height)
{
  m_height = height;
}

ColliderBase::Line ColliderCylinder::GetCenterLine()
{
  gfl2::math::Vector3 halfVec = m_pBaseModel->GetTransY() * m_height * 0.5f;
  return Line(GetPosition() - halfVec, GetPosition() + halfVec);
}

// 指定した中心点から指定した方向の直径線分を取得
ColliderBase::Line ColliderCylinder::GetDiameterLine(const gfl2::math::Vector3 &center, const gfl2::math::Vector3 &direction)
{
  gfl2::math::Vector3 halfVec = direction * m_radius;
  return Line(center - halfVec, center + halfVec);
}

bool ColliderCylinder::TestSphereCollider(ColliderSphere *pTargetCollider)
{
  // シリンダー→球体の当たり判定は使わないため、未実装
  HOU_PRINT("[ColliderCylinder::TestSphereCollider]Not Implement.\n");
  return false;
}

bool ColliderCylinder::TestCylinderCollider(ColliderCylinder *pTargetCollider)
{
  // シリンダー→シリンダーの当たり判定は使わないため、未実装
  HOU_PRINT("[ColliderCylinder::TestCylinderCollider]Not Implement.\n");
  return false;
}

GFL_NAMESPACE_END(Collision)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
