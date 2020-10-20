//======================================================================
/**
 * @file ModelBaseChara.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief ベースプレイヤーモデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ModelBaseChara.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

ModelBaseChara::ModelBaseChara()
{
  m_surfMotionIdFrom = -1;
  m_surfMotionIdTo = -1;
  m_surfMotionInter = 0.0f;
  m_rotationOrder = gfl2::math::EulerRotation::RotationOrderZYX;
}

ModelBaseChara::~ModelBaseChara()
{
}

void ModelBaseChara::Update()
{
  ApplyRotation();
  ModelBase::Update();
}

void ModelBaseChara::UpdateAnimation()
{
  ModelBase::UpdateAnimation();
  if(m_surfMotionIdFrom != m_surfMotionIdTo)
  {
    m_pBaseModel->UpdateAnimationManual(m_surfMotionInter);
  }
}

void ModelBaseChara::ChangeSurfMotion(s32 animeId, float t)
{
  m_surfMotionIdFrom = GetSurfStraightMotionId();
  m_surfMotionIdTo = animeId;
  m_surfMotionInter = t;

  if(m_surfMotionIdFrom == m_surfMotionIdTo)
  {
    return;
  }

  if(m_ppAnimeResourceNodes[m_surfMotionIdFrom] == NULL || m_ppAnimeResourceNodes[m_surfMotionIdTo] == NULL)
  {
    return;
  }
  m_pBaseModel->ChangeAnimationSmoothManualByResourceNode(m_ppAnimeResourceNodes[m_surfMotionIdFrom], m_ppAnimeResourceNodes[m_surfMotionIdTo]);
}

// 現姿勢の向きベクトルを取得
gfl2::math::Vector3 ModelBaseChara::GetDirection(DIRECTION dir)
{
  gfl2::math::Vector3 rhs(0.0f, 0.0f, 1.0f);
  gfl2::math::Vector3 res;
  f32 z;
  switch(dir)
  {
  case DIR_RIGHT:
    res = QuatTransformNormal(m_localRotation[LRA_Z], rhs);
    z = res.z;
    res.z = res.x;
    res.x = -z;
    break;
  case DIR_UP:
    res = QuatTransformNormal(m_localRotation[LRA_X], rhs);
    z = res.z;
    res.z = res.x;
    res.y = z;
    res.x = 0;
    break;
  case DIR_FRONT:
    res = QuatTransformNormal(m_localRotation[LRA_Y], rhs);
    break;
  }
  return res;
}

gfl2::math::Vector3 ModelBaseChara::QuatTransformNormal(const gfl2::math::Quaternion &quat, const gfl2::math::Vector3 &rhs)
{
  if(quat.IsAlmostZero())
  {
    return rhs;
  }
  gfl2::math::Matrix34 mat;
  quat.QuaternionToMatrix(mat);
  gfl2::math::Vector3 vec = mat.TransformNormal(rhs);
  return vec;
}

f32 ModelBaseChara::GetRotationX()
{
  return GetRotation(LRA_X);
}

f32 ModelBaseChara::GetRotationY()
{
  return GetRotation(LRA_Y);
}

f32 ModelBaseChara::GetRotationZ()
{
  return GetRotation(LRA_Z);
}

f32 ModelBaseChara::GetRotation(LocalRotationAxis axis)
{
  gfl2::math::Vector3 rhs(0.0f, 0.0f, 1.0f);
  gfl2::math::Vector3 res = QuatTransformNormal(m_localRotation[axis], rhs);
  return gfl2::math::NormalizeRadianAngle(0.5f * PI - atan2(res.z, res.x));
}

// X軸で指定角度まで回転
void ModelBaseChara::ChangeRotationX(f32 angle, f32 changeRate)
{
  ChangeRotation(LRA_X, angle, changeRate);
}

// Y軸で指定角度まで回転
void ModelBaseChara::ChangeRotationY(f32 angle, f32 changeRate)
{
  ChangeRotation(LRA_Y, angle, changeRate);
}

// Z軸で指定角度まで回転
void ModelBaseChara::ChangeRotationZ(f32 angle, f32 changeRate)
{
  ChangeRotation(LRA_Z, angle, changeRate);
}

void ModelBaseChara::ChangeRoation(f32 angleX, f32 angleY, f32 angleZ, f32 changeRate)
{
  ChangeRotationX(angleX, changeRate);
  ChangeRotationY(angleY, changeRate);
  ChangeRotationZ(angleZ, changeRate);
}

void ModelBaseChara::ChangeLookDirection(const gfl2::math::Vector3 &vecZX, f32 changeRate, f32 maxAngle)
{
  ChangeDirection(LRA_Y, vecZX.z, vecZX.x, changeRate, maxAngle);
}

void ModelBaseChara::ChangeUpDirection(const gfl2::math::Vector3 &vecYZ, f32 changeRate, f32 maxAngle)
{
  ChangeDirection(LRA_X, vecYZ.y, vecYZ.z, changeRate, maxAngle);
}

void ModelBaseChara::ChangeInclination(const gfl2::math::Vector3 &vecYX, f32 changeRate, f32 maxAngle)
{
  ChangeDirection(LRA_Z, vecYX.y, -vecYX.x, changeRate, maxAngle);
}

void ModelBaseChara::ChangeDirection(LocalRotationAxis axis, f32 a, f32 b, f32 changeRate, f32 maxAngle)
{
  f32 angle = gfl2::math::NormalizeRadianAngle(0.5f * PI - atan2(a, b));
  if(gfl2::math::FAbs(angle) > maxAngle)
  {
    angle = angle > 0.0f ? maxAngle : -maxAngle;
  }
  ChangeRotation(axis, angle, changeRate);
}

void ModelBaseChara::ChangeRotation(LocalRotationAxis axis, f32 angle, f32 changeRate)
{
  angle = gfl2::math::NormalizeRadianAngle(angle);
  gfl2::math::Quaternion targetQua;
  targetQua.RadianYToQuaternion(angle);
  // 回転する
  gfl2::math::Quaternion* pSrcQuat = NULL;
  switch(axis)
  {
  case LRA_X:
    pSrcQuat = &m_localRotation[LRA_X];
    break;
  case LRA_Y:
    pSrcQuat = &m_localRotation[LRA_Y];
    break;
  case LRA_Z:
    pSrcQuat = &m_localRotation[LRA_Z];
    break;
  default:
    GFL_ASSERT(0);
  }
  pSrcQuat->Slerp(targetQua, changeRate);
}

void ModelBaseChara::ApplyRotation()
{
  // 実際の回転角度
  f32 resultAngles[LRA_MAX];

  // XYZ軸それぞれの回転角度を計算
  gfl2::math::Vector3 vec;
  gfl2::math::Vector3 rhs(0.0f, 0.0f, 1.0f);
  for(s32 i = LRA_MIN; i < LRA_MAX; i++)
  {
    vec = QuatTransformNormal(m_localRotation[i], rhs);
    resultAngles[i] = gfl2::math::NormalizeRadianAngle(0.5f * PI - atan2(vec.z, vec.x));
  }

  // 回転を作成
  SetRotation(gfl2::math::Vector3(resultAngles[LRA_X], resultAngles[LRA_Y], resultAngles[LRA_Z]), m_rotationOrder);
}

// 転倒角度を変更
void ModelBaseChara::ChangeTurnOverAngle(f32 angle)
{
  m_localRotation[LRA_X].RadianYToQuaternion(angle);
  m_pBaseModel->SetRotationQuat(m_localRotation[LRA_X]);
}

// 指定軸の回転行列を取得
gfl2::math::Matrix34 ModelBaseChara::GetRotationMatrix(float rad, gfl2::math::Vector3 &axis)
{
  f32 cos_ = cosf(rad);
  f32 cos_1 = 1 - cos_;
  f32 sin_ = sinf(rad);
  f32 x = axis.x;
  f32 y = axis.y;
  f32 z = axis.z;

  return gfl2::math::Matrix34(
    gfl2::math::Vector4(x * x * cos_1 + cos_     , x * y * cos_1 - z * sin_ , z * x * cos_1 + y * sin_ , 0.0f),
    gfl2::math::Vector4(x * y * cos_1 + z * sin_ , y * y * cos_1 + cos_     , y * z * cos_1 - x * sin_ , 0.0f),
    gfl2::math::Vector4(z * x * cos_1 - y * sin_ , y * z * cos_1 + x * sin_ , z * z * cos_1 + cos_     , 0.0f));
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
