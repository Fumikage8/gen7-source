//======================================================================
/**
 * @file ColliderBase.cpp
 * @date 2016/10/14 16:31:14
 * @author fang_yicheng
 * @brief ベースコライダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ColliderBase.h"
#include "ColliderSphere.h"
#include "../Model/ModelBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Collision)

ColliderBase::ColliderBase() :
  m_type(TYPE_NONE),
  m_pBaseModel(NULL),
  m_jointIndex(-1)
{
}

ColliderBase::~ColliderBase()
{
}

void ColliderBase::Initialize(Model::ModelBase* pBaseModel, s32 jointIndex)
{
  m_pBaseModel = pBaseModel;
  m_jointIndex = jointIndex;
}

ColliderBase::TYPE ColliderBase::GetType()
{
  return m_type;
}

bool ColliderBase::TestCollider(ColliderBase *pTargetCollider)
{
  switch(pTargetCollider->GetType())
  {
  case TYPE_SPHERE:
    return TestSphereCollider(static_cast<ColliderSphere*>(pTargetCollider));
  case TYPE_CYLINDER:
    return TestCylinderCollider(static_cast<ColliderCylinder*>(pTargetCollider));
  default:
    HOU_PRINT("[TestCollider]Not implement shape.(type=%d)\n", pTargetCollider->GetType());
    return false;
  }
}

ColliderBase::CollisionInfo* ColliderBase::GetCollisionInfo()
{
  return &m_collisionInfo;
}

// 衝突検出前の位置の更新
void ColliderBase::PreSetup()
{
  m_prePosition = GetPosition();
}

gfl2::math::Vector3 ColliderBase::GetPosition()
{
  return m_jointIndex >= 0 ? m_pBaseModel->GetJointPosition(m_jointIndex) : m_pBaseModel->GetPosition();
}

Model::ModelBase* ColliderBase::GetModel()
{
  return m_pBaseModel;
}

// 点から線分上の最接近点を求める
gfl2::math::Vector3 ColliderBase::CalcClosestPointOnLine(const gfl2::math::Vector3 &point, const Line &line, f32 *out_pjLength)
{
  f32 length = line.Length();
  if(length <= EPSILON)
  {
    // 線分がほぼ点であるため、線分のEndを返す
    if(out_pjLength != NULL)
    {
      *out_pjLength = 0.0f;
    }
    return line.m_end;
  }

  gfl2::math::Vector3 sp = point - line.m_start;      // 線分のStartとPointのベクトル
  gfl2::math::Vector3 dir = line.Direction();         // 線分の方向(単位ベクトル)
  
  f32 p = sp.Dot(dir);  // spが線分上の射影長を求める
  f32 t = p / length;   // 射影長と線分長の比例

  if(out_pjLength != NULL)
  {
    *out_pjLength = p;
  }

  if(t <= 0.0f)
  {
    // 点が線分Startの外側にあるため、最接近点は線分のStart
    return line.m_start;
  }
  else if(t >= 1.0f)
  {
    // 点が線分Endの外側にあるため、最接近点は線分のEnd
    return line.m_end;
  }
  else
  {
    // 点が線分内側にあるため、最接近を計算する
    return line.m_start + dir * p;
  }
}

GFL_NAMESPACE_END(Collision)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
