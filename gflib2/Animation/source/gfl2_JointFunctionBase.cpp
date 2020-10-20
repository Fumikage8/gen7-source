#include <Animation/include/gfl2_JointFunctionBase.h>

using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
JointFunctionBase::JointFunctionBase() :
  m_pModelNode(NULL),
  m_pJointNode(NULL),
  m_Weight(0.0f)
{
}

/**
 * @brief デストラクタ
 */
JointFunctionBase::~JointFunctionBase()
{
}

/**
 * @brief 初期化
 */
void JointFunctionBase::Initialize(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode)
{
  m_pModelNode = pModelNode;
}

/**
 * @brief 終了
 */
void JointFunctionBase::Finalize()
{
}

#if 0
/**
 * @brief 何もしない設定
 */
void JointFunctionBase::SetFuncNone()
{
  m_FuncType = FuncTypeNone;
}

/**
 * @brief 注目設定
 */
void JointFunctionBase::SetFuncAim(const gfl2::math::Vector& targetPosition)
{
  m_FuncType = FuncTypeAim;
  m_UnionParam.aimParam.targetPositionX = targetPosition.x;
  m_UnionParam.aimParam.targetPositionY = targetPosition.y;
  m_UnionParam.aimParam.targetPositionZ = targetPosition.z;
}

/**
 * @brief 更新：何もしない
 */
void JointFunctionBase::UpdateFuncNone()
{
  // 何もしない
}

static void UpdateJoint(gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode)
{
  const math::Matrix& worldMatrix = pJointNode->GetParent()->SafeCast<gfl2::renderingengine::scenegraph::instance::JointInstanceNode>()->GetWorldMatrix();
  pJointNode->SetWorldMatrix(worldMatrix * pJointNode->GetLocalMatrix());
  for (u32 i = 0; i < pJointNode->GetChildCount(); ++i)
  {
    UpdateJoint(pJointNode->GetChild(i)->SafeCast<gfl2::renderingengine::scenegraph::instance::JointInstanceNode>());
  }
}

/**
 * @brief 更新：注目
 */
void JointFunctionBase::UpdateFuncAim()
{
  const AimParam& aimParam = m_UnionParam.aimParam;
  const math::Matrix& localMatrix = m_pJointNode->GetLocalMatrix();

  const math::Matrix& parentInverseWorldMatrix = (m_pJointNode->GetParent() != NULL)
    //? m_pJointNode->GetParent()->SafeCast<gfl2::renderingengine::scenegraph::instance::JointInstanceNode>()->GetWorldMatrix().Inverse()
    ? m_pJointNode->GetWorldMatrix().Inverse()
    : math::Matrix::GetIdentity();

  // 目標位置をローカル座標に変換
  math::Vector targetPosition(aimParam.targetPositionX, aimParam.targetPositionY, aimParam.targetPositionZ);
  targetPosition = parentInverseWorldMatrix.TransformCoord(targetPosition);

  // 目標までの角度と回転軸を求める
  math::Vector targetDirection = targetPosition.Normalize3();
  math::Vector myDirection = localMatrix[2].Normalize3();

  // 角度
  f32 dot = myDirection.Dot3(targetDirection);
  f32 angle = acos(dot);
  if (dot >= 1.0f)
  {
    // 角度に差分がなければ変更する必要が無いので終了
    return;
  }

  // 回転軸
  math::Vector axis = myDirection.Cross(targetDirection).Normalize3();

  // ローカル行列を回転させる
  f32 cosA = cos(angle * 0.5f);
  f32 sinA = sin(angle * 0.5f);
  math::Quaternion q(axis.x * sinA, axis.y * sinA, axis.z * sinA, cosA);

  animation::JointTransform local(localMatrix);
  math::Quaternion q2 = local.rotation * q;

  local.rotation.Slerp(q2, m_Weight);
  //local.rotation *= q;

  m_pJointNode->SetLocalMatrix(local.ToMatrix());

  //m_pJointNode->SetLocalMatrix(math::Matrix::GetLookAt(math::Vector(0, 0, 0), targetPosition, math::Vector::GetZUnit()));

  // ワールド行列更新
  UpdateJoint(m_pJointNode);
}

#endif

}}

// eof
