#include <Animation/include/gfl2_InterestJointFunction.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

using namespace gfl2;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
InterestJointFunction::InterestJointFunction() : 
  m_pInterestNode(NULL),
  m_AfterFrame(0),
  m_AfterFrameMax(8),
  m_pParentFunction(NULL),
  m_pChildFunction(NULL)
{
  m_OldAngles[0] = 0.0f;
  m_OldAngles[1] = 0.0f;
}

/**
 * @brief デストラクタ
 */
InterestJointFunction::~InterestJointFunction()
{
}

/**
 * @brief 注目ノードの設定
 */
void InterestJointFunction::SetInterestNode(gfl2::renderingengine::scenegraph::resource::MotionConfigResourceNode::InterestNode* pInterestNode, math::Quaternion* pOldRotation)
{
  m_pInterestNode = pInterestNode;
  m_pOldRotation = pOldRotation;
}

/**
 * @brief 更新
 *
 * @param inout_pOverAngle0 角度限界を超えた角度0
 * @param inout_pOverAngle1 角度限界を超えた角度1
 *
 * @return true:角度制限に引っかかった false:角度制限以内
 */
bool InterestJointFunction::Update(f32* inout_pOverAngle0, f32* inout_pOverAngle1)
{
#if 0
  if (this->GetWeight() <= 0.0f)
  {
    m_OldAngles[0] = 0.0f;
    m_OldAngles[1] = 0.0f;
    return;
  }
#endif

  bool isRotateLimit = false;

  // 更新前の回転値を保存
  *m_pOldRotation = this->GetJointNode()->GetLocalRotation();

  f32 oldOldAngle0 = m_OldAngles[0];
  f32 oldOldAngle1 = m_OldAngles[1];

  f32 angle0 = *inout_pOverAngle0;
  f32 angle1 = *inout_pOverAngle1;

  // 角度制限
  switch (this->GetType())
  {
  case renderingengine::InterestType::X:
  case renderingengine::InterestType::Y:
  case renderingengine::InterestType::Z:
  case renderingengine::InterestType::X_M90:
  case renderingengine::InterestType::Z_180:
  case renderingengine::InterestType::Y_90:
    isRotateLimit = this->Clamp(&angle0, inout_pOverAngle0);
    break;

  case renderingengine::InterestType::YZ:
  case renderingengine::InterestType::XZ:
  case renderingengine::InterestType::XY:
  case renderingengine::InterestType::ZY:
    isRotateLimit = this->Clamp2(&angle0, &angle1, inout_pOverAngle0, inout_pOverAngle1);
    break;
  }

  switch (this->GetType())
  {
  case gfl2::renderingengine::InterestType::X:
    this->UpdateX(angle0, 0);
    break;

  case gfl2::renderingengine::InterestType::YZ:
    this->UpdateY(angle0, 0);
    this->UpdateZ(angle1, 1);
    break;

  case gfl2::renderingengine::InterestType::XZ:
    this->UpdateX(angle0, 0);
    this->UpdateZ(angle1, 1);
    break;

  case gfl2::renderingengine::InterestType::XY:
    this->UpdateX(angle0, 0);
    this->UpdateY(angle1, 1);
    break;

  case gfl2::renderingengine::InterestType::ZY:
    this->UpdateZ(angle0, 0);
    this->UpdateY(angle1, 1);
    break;

  case gfl2::renderingengine::InterestType::Y:
    this->UpdateY(angle0, 0);
    break;

  case gfl2::renderingengine::InterestType::Z:
    this->UpdateZ(angle0, 0);
    break;

  case gfl2::renderingengine::InterestType::X_M90:
    this->UpdateX(angle0, 0);
    break;

  case gfl2::renderingengine::InterestType::Z_180:
    this->UpdateZ(angle0, 0);
    break;

  case gfl2::renderingengine::InterestType::Y_90:
    this->UpdateY(angle0, 0);
    break;
  }

  // ワールド行列更新
  SceneGraphManager::TraverseModelFast(this->GetJointNode());

  // 角度制限チェック
  if (isRotateLimit)
  {
    f32 subAngle0 = m_OldAngles[0] - oldOldAngle0;
    f32 subAngle1 = m_OldAngles[1] - oldOldAngle1;
    return math::IsAlmostZero(subAngle0) && math::IsAlmostZero(subAngle1);
  }

  return false;
}

/**
 * @brief 最初の回転角度を取得
 *
 * @param pInterestPosition 注視位置
 * @param out_pAngle0 角度0
 * @param out_pAngle1 角度1
 */
void InterestJointFunction::GetFirstAngle(const math::Vector3* pInterestPosition, f32* out_pAngle0, f32* out_pAngle1)
{
  // NULLなら終了させる
  // @attention ここに一緒に解除処理が入ってるのは気持ち悪い
  if (pInterestPosition == NULL)
  {
    if (m_AfterFrame == 0)
    {
      m_AfterAngles[0] = m_OldAngles[0];
      m_AfterAngles[1] = m_OldAngles[1];
    }

    ++m_AfterFrame;
    f32 rate = static_cast<f32>(m_AfterFrame) / m_AfterFrameMax;
    if (rate > 1.0f) rate = 1.0f;
    rate = 1.0f - rate;

    *out_pAngle0 = m_AfterAngles[0] * rate;
    *out_pAngle1 = m_AfterAngles[1] * rate;
    return;
  }

  m_AfterFrame = 0;

  const math::Matrix34& jointMatrix = this->GetJointNode()->GetWorldMatrix();

  switch (this->GetType())
  {
  case gfl2::renderingengine::InterestType::XY:
    GetAngleXY(jointMatrix, *pInterestPosition, false, out_pAngle0, out_pAngle1);
    break;

  case gfl2::renderingengine::InterestType::Y:
  case gfl2::renderingengine::InterestType::YZ:
    GetAngleYZ(jointMatrix, *pInterestPosition, false, out_pAngle0, out_pAngle1);
    break;

  case gfl2::renderingengine::InterestType::ZY:
  case gfl2::renderingengine::InterestType::Z_180:
    GetAngleZY(jointMatrix, *pInterestPosition, false, out_pAngle0, out_pAngle1);
    break;

  case gfl2::renderingengine::InterestType::X_M90:
    {
      math::Matrix34 mat = jointMatrix * math::Matrix34::GetRotationX(-PI * 0.5f);
      GetAngleXZ(mat, *pInterestPosition, true, out_pAngle0, out_pAngle1);
    }
    break;

  case gfl2::renderingengine::InterestType::Y_90:
    {
      math::Matrix34 mat = jointMatrix * math::Matrix34::GetRotationY(PI * 0.5f);
      GetAngleYZ(mat, *pInterestPosition, true, out_pAngle0, out_pAngle1);
    }
    break;

  default:
    GetAngleXZ(jointMatrix, *pInterestPosition, true, out_pAngle0, out_pAngle1);
    break;
  }
}

void InterestJointFunction::GetAngleXY(const math::Matrix34& jointMatrix, const math::Vector3& interestPosition, bool reverseFlag, f32* out_pAngle0, f32* out_pAngle1)
{
  math::Vector3 localTargetPosition = jointMatrix.Inverse().TransformCoord(interestPosition);

  if (reverseFlag)
  {
    localTargetPosition = -localTargetPosition;
  }

  f32 tmp0 = localTargetPosition.y * localTargetPosition.y + localTargetPosition.z * localTargetPosition.z;
  if (math::IsAlmostZero(tmp0))
  {
    return;
  }

  f32 tmp1 = tmp0 + localTargetPosition.x * localTargetPosition.x;
  tmp0 = math::FSqrt(tmp0);
  f32 tmp2 = 1.0f / tmp0;
  tmp1 = math::FSqrt(tmp1);
  tmp1 = 1.0f / tmp1;

  f32 ysin = -localTargetPosition.y * tmp2;
  f32 ycos = localTargetPosition.z * tmp2;
  f32 zsin = localTargetPosition.x * tmp1;
  f32 zcos = tmp0 * tmp1;

  *out_pAngle0 = atan2f(ysin, ycos);
  *out_pAngle1 = atan2f(zsin, zcos);

}
void InterestJointFunction::GetAngleYZ(const math::Matrix34& jointMatrix, const math::Vector3& interestPosition, bool reverseFlag, f32* out_pAngle0, f32* out_pAngle1)
{
  math::Vector3 localTargetPosition = jointMatrix.Inverse() * interestPosition;

  if (reverseFlag)
  {
    localTargetPosition = -localTargetPosition;
  }

  f32 tmp0 = localTargetPosition.x * localTargetPosition.x + localTargetPosition.z * localTargetPosition.z;
  if (math::IsAlmostZero(tmp0))
  {
    return;
  }

  f32 tmp1 = tmp0 + localTargetPosition.y * localTargetPosition.y;
  tmp0 = math::FSqrt(tmp0);
  f32 tmp2 = 1.0f / tmp0;
  tmp1 = math::FSqrt(tmp1);
  tmp1 = 1.0f / tmp1;

  f32 ysin = -localTargetPosition.z * tmp2;
  f32 ycos = localTargetPosition.x * tmp2;
  f32 zsin = localTargetPosition.y * tmp1;
  f32 zcos = tmp0 * tmp1;

  *out_pAngle0 = atan2f(ysin, ycos);
  *out_pAngle1 = atan2f(zsin, zcos);
}
void InterestJointFunction::GetAngleZY(const math::Matrix34& jointMatrix, const math::Vector3& interestPosition, bool reverseFlag, f32* out_pAngle0, f32* out_pAngle1)
{
  math::Vector3 localTargetPosition = jointMatrix.Inverse() * interestPosition;

  if (reverseFlag)
  {
    localTargetPosition = -localTargetPosition;
  }

  f32 tmp0 = localTargetPosition.x * localTargetPosition.x + localTargetPosition.y * localTargetPosition.y;
  if (math::IsAlmostZero(tmp0))
  {
    return;
  }

  f32 tmp1 = tmp0 + localTargetPosition.z * localTargetPosition.z;
  tmp0 = math::FSqrt(tmp0);
  f32 tmp2 = 1.0f / tmp0;
  tmp1 = math::FSqrt(tmp1);
  tmp1 = 1.0f / tmp1;

  f32 ysin = localTargetPosition.y * tmp2;
  f32 ycos = localTargetPosition.x * tmp2;
  f32 zsin = -localTargetPosition.z * tmp1;
  f32 zcos = tmp0 * tmp1;

  *out_pAngle0 = atan2f(ysin, ycos);
  *out_pAngle1 = atan2f(zsin, zcos);
}
void InterestJointFunction::GetAngleXZ(const math::Matrix34& jointMatrix, const math::Vector3& interestPosition, bool reverseFlag, f32* out_pAngle0, f32* out_pAngle1)
{
  math::Vector3 localTargetPosition = jointMatrix.Inverse() * interestPosition;

  if (reverseFlag)
  {
    localTargetPosition = -localTargetPosition;
  }

  f32 tmp0 = localTargetPosition.y * localTargetPosition.y + localTargetPosition.z * localTargetPosition.z;
  if (math::IsAlmostZero(tmp0))
  {
    return;
  }

  f32 tmp1 = tmp0 + localTargetPosition.x * localTargetPosition.x;
  tmp0 = math::FSqrt(tmp0);
  f32 tmp2 = 1.0f / tmp0;
  tmp1 = math::FSqrt(tmp1);
  tmp1 = 1.0f / tmp1;

  f32 ysin = localTargetPosition.z * tmp2;
  f32 ycos = localTargetPosition.y * tmp2;
  f32 zsin = -localTargetPosition.x * tmp1;
  f32 zcos = tmp0 * tmp1;

  *out_pAngle0 = atan2f(ysin, ycos);
  *out_pAngle1 = atan2f(zsin, zcos);
}

bool InterestJointFunction::Clamp(f32* inout_pAngle, f32* out_pOverAngle)
{
  // 通常の角度制限
  if (*inout_pAngle < this->GetLow())
  {
    *out_pOverAngle = *inout_pAngle - this->GetLow();
    *inout_pAngle = this->GetLow();
    return true;
  }

  if (*inout_pAngle > this->GetHigh())
  {
    *out_pOverAngle = *inout_pAngle - this->GetHigh();
    *inout_pAngle = this->GetHigh();
    return true;
  }

  return false;
}

bool InterestJointFunction::Clamp2(f32* inout_pAngle0, f32* inout_pAngle1, f32* out_pOverAngle0, f32* out_pOverAngle1)
{
  // 楕円の角度制限
  m_Upper.SetFocus(this->GetHigh(), this->GetHigh2());
  m_Lower.SetFocus(this->GetHigh(), (this->GetLow2() < 0.0f) ? -this->GetLow2() : this->GetLow2());

  if (0.0f <= *inout_pAngle1)
  {
    return m_Upper.Clamp(inout_pAngle0, inout_pAngle1, out_pOverAngle0, out_pOverAngle1);
  }
  else
  {
    return m_Lower.Clamp(inout_pAngle0, inout_pAngle1, out_pOverAngle0, out_pOverAngle1);
  }
}

void InterestJointFunction::UpdateX(f32 angle, u32 angleIndex)
{
  // 行列更新
  return this->UpdateMatrix(angle, 1.0f, 0.0f, 0.0f, angleIndex);
}

void InterestJointFunction::UpdateY(f32 angle, u32 angleIndex)
{
  // 行列更新
  return this->UpdateMatrix(angle, 0.0f, 1.0f, 0.0f, angleIndex);
}

void InterestJointFunction::UpdateZ(f32 angle, u32 angleIndex)
{
  // 行列更新
  return this->UpdateMatrix(angle, 0.0f, 0.0f, 1.0f, angleIndex);
}

void InterestJointFunction::UpdateMatrix(f32 angle, f32 ax, f32 ay, f32 az, u32 angleIndex)
{
  f32 diff = angle - m_OldAngles[angleIndex];

  // 解除中は速度制限しない
  if (m_AfterFrame == 0)
  {
    // 速度制限
    if (this->GetSpeed() * this->GetSpeed() < diff * diff)
    {
      if (diff > 0.0f)
      {
        angle = m_OldAngles[angleIndex] + this->GetSpeed();
      }
      else
      {
        angle = m_OldAngles[angleIndex] - this->GetSpeed();
      }
    }
  }

  m_OldAngles[angleIndex] = angle;
  this->RotateMatrix(angle, ax, ay, az);

#if 0
  char buf[512];
  sprintf_s(buf, "%s: angle %f, speed %f, diff %f\n", pJointNode->GetNodeName(), angle, this->GetSpeed(), diff);
  OutputDebugStringA(buf);
#endif
}

void InterestJointFunction::RotateMatrix(f32 angle, f32 ax, f32 ay, f32 az)
{
  // ローカル行列を回転させる
  f32 cosAngle = cos(angle * 0.5f);
  f32 sinAngle = sin(angle * 0.5f);
  math::Quaternion q(sinAngle * ax, sinAngle * ay, sinAngle * az, cosAngle);

  instance::JointInstanceNode* pJointNode = this->GetJointNode();
  pJointNode->SetLocalRotation(pJointNode->GetLocalRotation() * q);

  instance::JointInstanceNode* pParentNode = pJointNode->GetParent()->SafeCast<instance::JointInstanceNode>();

  math::Matrix34 localMatrix = pJointNode->GetLocalSRT().ToMatrixNonInheritanceScale(pParentNode->GetLocalScale());

  // ワールド行列を計算
  const math::Matrix34& parentMatrix = pParentNode->GetWorldMatrix();
  pJointNode->SetWorldMatrix(parentMatrix * localMatrix);
}

void InterestJointFunction::RotateX(f32 angle)
{
  this->RotateMatrix(angle, 1.0f, 0.0f, 0.0f);
}

void InterestJointFunction::RotateY(f32 angle)
{
  this->RotateMatrix(angle, 0.0f, 1.0f, 0.0f);
}

void InterestJointFunction::RotateZ(f32 angle)
{
  this->RotateMatrix(angle, 0.0f, 0.0f, 1.0f);
}

/**
 * 顔向け補正行列を計算
 */
void InterestJointFunction::CalculateAdjustMatrix(f32 adjustAngle, f32 decline)
{
  if (this->GetAdjustUpdateType() == ADJUST_UPDATE_TYPE_YZ)
  {
    this->RotateY(adjustAngle);
    this->RotateZ(decline);
  }
  else
  {
    this->RotateZ(adjustAngle);
    this->RotateX(decline);
  }

  // ワールド行列更新
  SceneGraphManager::TraverseModelFast(this->GetJointNode());
}

InterestJointFunction::AdjustUpdateType InterestJointFunction::GetAdjustUpdateType() const
{
  switch (this->GetType())
  {
  case renderingengine::InterestType::X:
  case renderingengine::InterestType::Y:
  case renderingengine::InterestType::XY:
  case renderingengine::InterestType::ZY:
  case renderingengine::InterestType::X_M90:
  case renderingengine::InterestType::Y_90:
    return ADJUST_UPDATE_TYPE_YZ;

  case renderingengine::InterestType::Z:
  case renderingengine::InterestType::YZ:
  case renderingengine::InterestType::XZ:
  case renderingengine::InterestType::Z_180:
    return ADJUST_UPDATE_TYPE_ZX;
  }

  // enumに含まれていないおかしなデータ
  GFL_ASSERT(0);
  return ADJUST_UPDATE_TYPE_YZ;
}

}}

// eof
