//======================================================================
/**
 * @file ColliderSphere.cpp
 * @date 2016/10/14 16:31:14
 * @author fang_yicheng
 * @brief 球体コライダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ColliderSphere.h"
#include "ColliderCylinder.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Collision)

ColliderSphere::ColliderSphere()
{
  m_type    = TYPE_SPHERE;
  m_radius  = 0.0f;
}

ColliderSphere::~ColliderSphere()
{
}

f32 ColliderSphere::GetRadius()
{
  return m_radius;
}
 
void ColliderSphere::SetRadius(f32 radius)
{
  m_radius = radius;
}

// 球体コライダーと衝突検出
bool ColliderSphere::TestSphereCollider(ColliderSphere* pTargetCollider)
{
  m_collisionInfo.Initialize();
  f32 ra = this->GetRadius() + pTargetCollider->GetRadius();

  // 自機移動でできた線分
  Line moveLine(m_prePosition, GetPosition());
  if(moveLine.Length() > 0.0f)
  {
    // 自機が移動した場合
    f32 pjLength;  // 自機移動前の位置と対象位置のベクトルが、自機移動線分上の射影長
    gfl2::math::Vector3 closestPoint = CalcClosestPointOnLine(pTargetCollider->GetPosition(), moveLine, &pjLength); // 対象位置が線分上の最接近点

    // 対象位置と最接近点の距離
    f32 length = (closestPoint - pTargetCollider->GetPosition()).Length();
    if(length >= ra)
    {
      // 距離が合計半径以上の場合は衝突していない
      return false;
    }

    // 衝突時の自機位置を求める
    gfl2::math::Vector3 vPoint = moveLine.m_start + moveLine.Direction() * pjLength;  // 対象位置が移動線分上の垂直点
    f32 h = (vPoint - pTargetCollider->GetPosition()).Length();                       // 垂直線の長さ
    f32 a = gfl2::math::FSqrt(gfl2::math::FAbs(length * length - h * h));
    f32 b = gfl2::math::FSqrt(gfl2::math::FAbs(ra * ra - h * h));
    gfl2::math::Vector3 collisionPos = closestPoint - moveLine.Direction() * (b - a);

    // 衝突情報作成
    m_collisionInfo.m_isCollided = true;
    m_collisionInfo.m_pTargetModel = pTargetCollider->GetModel();
    // 法線
    gfl2::math::Vector3 normal = (collisionPos - pTargetCollider->GetPosition());
    if(normal.Length() > 0.0f)
    {
      m_collisionInfo.m_normal = normal.Normalize();
    }
    // 押し返し量
    m_collisionInfo.m_solveVector = collisionPos - GetPosition();
    m_collisionInfo.m_solveVector += moveLine.LineVector() - m_collisionInfo.m_normal * moveLine.LineVector().Dot(m_collisionInfo.m_normal);
  }
  else
  {
    // 自機が静止している場合
    gfl2::math::Vector3 distance = this->GetPosition() - pTargetCollider->GetPosition();  // 対象との距離
    f32 length = distance.Length();
    // 距離と合計半径の差分
    f32 diff = ra - length;
    if(diff > 0.001f)
    {
      // 距離が半径より小さい場合は衝突する
      m_collisionInfo.m_isCollided = true;
      m_collisionInfo.m_pTargetModel = pTargetCollider->GetModel();
      // 法線
      m_collisionInfo.m_normal = gfl2::math::Vector3(0.0f, 0.0f, -1.0f);
      if(distance.Length() > 0.0f)
      {
        m_collisionInfo.m_normal = distance.Normalize();
      }
      // 押し返し量
      m_collisionInfo.m_solveVector = m_collisionInfo.m_normal * diff;
    }
  }
  return m_collisionInfo.m_isCollided;
}

// シリンダーコライダーとの衝突検出
bool ColliderSphere::TestCylinderCollider(ColliderCylinder *pTargetCollider)
{
  m_collisionInfo.Initialize();

  gfl2::math::Vector3 position = GetPosition();
  Line centerLine = pTargetCollider->GetCenterLine();
  f32 length = 0.0f;            // 対象との距離
  f32 pjLength = 0.0f;          // 自機位置と対象中心線分のベクトルが、対象中心線分上の射影長
  gfl2::math::Vector3 normal;   // 衝突法線
  gfl2::math::Vector3 c1 = CalcClosestPointOnLine(position, centerLine, &pjLength);
  if(pjLength > pTargetCollider->GetHeight() || pjLength < 0.0f)
  {
    // 自機位置が対象中心線分の外側に居る
    gfl2::math::Vector3 center = pjLength < 0.0f ? centerLine.m_start : centerLine.m_end;
    gfl2::math::Vector3 p = centerLine.m_start + centerLine.Direction() * pjLength;
    // 直径線分を取得
    Line diameterLine = pTargetCollider->GetDiameterLine(center, (position - p).Normalize());
    gfl2::math::Vector3 c2 = CalcClosestPointOnLine(position, diameterLine);
    length = (position - c2).Length();
    normal = (position - c2).Normalize();
  }
  else
  {
    // 自機位置が対象中心線分の内側に居る
    length = (position - c1).Length() - pTargetCollider->GetRadius();
    normal = (position - c1).Normalize();
  }

  // 距離と自機半径の差分
  f32 diff = GetRadius() - length;
  if(diff > 0.001f)
  {
    // 距離が半径より小さい場合は衝突する
    m_collisionInfo.m_isCollided = true;
    m_collisionInfo.m_pTargetModel = pTargetCollider->GetModel();
    // 法線
    m_collisionInfo.m_normal = normal;
    // 押し返し量
    m_collisionInfo.m_solveVector = m_collisionInfo.m_normal * diff;
  }
  return m_collisionInfo.m_isCollided;
}

GFL_NAMESPACE_END(Collision)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
