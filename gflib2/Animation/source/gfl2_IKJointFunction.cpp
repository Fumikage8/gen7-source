#include <Animation/include/gfl2_IKJointFunction.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

using namespace gfl2;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
IKJointFunction::IKJointFunction() :
  m_pChainJoint(NULL),
  m_pEffectorJoint(NULL),
  m_pTipJoint(NULL),
  m_pCallback(NULL),
  m_pUserData(NULL),
  m_TargetPosition(),
  m_TargetNormal(0, 1, 0)
{
}

/**
 * @brief デストラクタ
 */
IKJointFunction::~IKJointFunction()
{
}

void CalkIk(instance::JointInstanceNode* pJointNode, instance::JointInstanceNode* pEffectorNode, const math::Vector3& targetPosition)
{
  instance::JointInstanceNode* pParentNode = pJointNode->GetParent()->SafeCast<instance::JointInstanceNode>();
  math::Vector3 localTargetPosition = pParentNode->GetWorldMatrix().Inverse().TransformCoord(targetPosition);
  math::Vector3 tv = localTargetPosition.Normalize();

  math::Vector3 effectorPosition = pEffectorNode->GetWorldMatrix().GetElemPosition();
  math::Vector3 localEffectorPosition = pParentNode->GetWorldMatrix().Inverse().TransformCoord(effectorPosition);
  math::Vector3 ev = localEffectorPosition.Normalize();

  // 目標までの角度、回転軸を求める
  f32 dot = ev.Dot(tv);
  if (dot >= 1.0f - 0.000001f)
  {
    return;
  }
  f32 angle = acos(dot);

  math::Vector3 axis = ev.Cross(tv).Normalize();

  f32 cosAngle = cos(angle * 0.5f);
  f32 sinAngle = sin(angle * 0.5f);
  math::Quaternion q(sinAngle * axis.x, sinAngle * axis.y, sinAngle * axis.z, cosAngle);
  pParentNode->SetLocalRotation(pParentNode->GetLocalRotation() * q);

  // ワールド行列更新
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast(pParentNode);
}

static void CalkIk2(instance::JointInstanceNode* pJointNode, const math::Vector3& targetPosition, const math::Vector3& originalPosition, f32 jointLength0, f32 jointLength1)
{
  // Analytic IK ジョイントを三角形に見立てて位置を補正する
  instance::JointInstanceNode* pParentNode = pJointNode->GetParent()->SafeCast<instance::JointInstanceNode>();
  math::Matrix34 invMatrix = pParentNode->GetWorldMatrix().Inverse();
  math::Vector3 localTargetPosition = invMatrix.TransformCoord(targetPosition);

  f32 x = localTargetPosition.x;
  f32 y = sqrtf((localTargetPosition.y * localTargetPosition.y) + (localTargetPosition.z * localTargetPosition.z));
  f32 d1 = jointLength0;
  f32 d2 = jointLength1;

  f32 x_2 = x * x;
  f32 y_2 = y * y;
  f32 d1_2 = d1 * d1;
  f32 d2_2 = d2 * d2;

  // angle2を求める
  f32 cosAngle2 = (x_2 + y_2 - d1_2 - d2_2) / (2.0f * d1 * d2);
  if (cosAngle2 < -1.0f) cosAngle2 = -1.0f;
  if (cosAngle2 > 1.0f) cosAngle2 = 1.0f;
  f32 angle2 = acosf(cosAngle2);

  f32 sinAngle2 = sinf(angle2);

  // angle1を求める
  f32 tanAngle3 = (d2 * sinAngle2) / (d1 + d2 * cosAngle2);
  f32 tanAngle4 = y / x;

  f32 tanAngle1 = (tanAngle4 - tanAngle3) / (1.0f + tanAngle4 * tanAngle3);
  f32 angle1 = atanf(tanAngle1);

  // ジョイント１回転
  {
    math::Vector3 localOriginalPosition = invMatrix.TransformCoord(originalPosition);
    math::Vector3 localJointPosition = invMatrix.TransformCoord(pJointNode->GetWorldMatrix().GetElemPosition());

    math::Vector3 tmp0 = localOriginalPosition.Normalize();
    math::Vector3 tmp1 = localJointPosition.Normalize();

    f32 dot = tmp0.Dot(tmp1);
    if (dot >= 1.0f - 0.000001f)
    {
      // オリジナルが並んでしまっているので回転軸が計算できない、ターゲットで代用する
      tmp0 = localTargetPosition.Normalize();
    }

    math::Vector3 axis = tmp0.Cross(tmp1).Normalize();

    f32 cosAngle = cosf(-angle1 * 0.5f);
    f32 sinAngle = sinf(-angle1 * 0.5f);
    math::Quaternion q(sinAngle * axis.x, sinAngle * axis.y, sinAngle * axis.z, cosAngle);
    pParentNode->SetLocalRotation(pParentNode->GetLocalRotation() * q);
  }

  // ワールド行列更新
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast(pParentNode);

  // ジョイント２回転
  {
    math::Matrix34 invMatrix = pJointNode->GetWorldMatrix().Inverse();

    math::Vector3 localTargetPosition = invMatrix.TransformCoord(targetPosition);
    math::Vector3 localOriginalPosition = invMatrix.TransformCoord(pJointNode->GetChild()->SafeCast<instance::JointInstanceNode>()->GetWorldMatrix().GetElemPosition());
    math::Vector3 axis = localOriginalPosition.Cross(localTargetPosition).Normalize();

    f32 dot = localTargetPosition.Normalize().Dot(localOriginalPosition.Normalize());
    if (dot >= 1.0f - 0.000001f)
    {
      return;
    }
    f32 angle = acosf(dot);

    f32 cosAngle = cosf(angle * 0.5f);
    f32 sinAngle = sinf(angle * 0.5f);
    math::Quaternion q(sinAngle * axis.x, sinAngle * axis.y, sinAngle * axis.z, cosAngle);
    pJointNode->SetLocalRotation(pJointNode->GetLocalRotation() * q);
  }

  // ワールド行列更新
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast(pJointNode);
}

static void AdjustNormal(instance::JointInstanceNode* pJointNode, const math::Vector3& normal)
{
  instance::JointInstanceNode* pParentNode = pJointNode->GetParent()->SafeCast<instance::JointInstanceNode>();
  math::Matrix34 mat = pParentNode->GetWorldMatrix();
  mat[0][3] = 0.0f;
  mat[1][3] = 0.0f;
  mat[2][3] = 0.0f;
  math::Vector3 localNormal = mat.Inverse().TransformNormal(normal).Normalize();
  //math::Vector3 localZ = pParentNode->GetLocalMatrix()[0].Normalize();
  math::Vector3 localZ = mat.Inverse().TransformNormal(math::Vector3(0, 1, 0)).Normalize();

  math::Vector3 vecY(0, 1, 0);

  // 目標までの角度、回転軸を求める
  //f32 dot = vecY.Dot(normal);
  f32 dot = localZ.Dot(localNormal);
  if (dot >= 1.0f - 0.000001f)
  {
    return;
  }
  f32 angle = acos(dot);
  //angle = math::ConvDegToRad(-45);

  math::Vector3 axis = localZ.Cross(localNormal).Normalize();
  //math::Vector3 axis(0, -1, 0);

  //math::Vector3 n0 = axis.Cross(localNormal).Normalize();
  //math::Vector3 n1 = localNormal.Cross(axis).Normalize();

  //math::Vector3 n2 = localNormal.Cross(n0);
  //math::Vector3 n3 = localNormal.Cross(n1);
  //math::Vector3 axis = vecY.Cross(normal).Normalize();

  f32 cosAngle = cos(angle * 0.5f);
  f32 sinAngle = sin(angle * 0.5f);
  math::Quaternion q(sinAngle * axis.x, sinAngle * axis.y, sinAngle * axis.z, cosAngle);
  pParentNode->SetLocalRotation(pParentNode->GetLocalRotation() * q);

  // ワールド行列更新
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast(pParentNode);
}

/**
 * @brief 更新
 */
void IKJointFunction::Update()
{
  instance::JointInstanceNode* pTipJoint = this->GetTipJoint();

  // コールバックが無ければ無視
  if (m_pCallback == NULL)
  {
    return;
  }

  // 位置座標や法線が取得できなければ無視
  math::Vector3 oldNormal = m_TargetNormal;
  bool found = m_pCallback(&m_TargetPosition, &m_TargetNormal, this->GetEffectorJoint(), m_pUserData);

  // 法線の線形補間、法線をぬるんとさせる
  // @attention 補間速度を設定できるようにする
  oldNormal.Lerp(m_TargetNormal, 0.2f);
  m_TargetNormal = oldNormal.Normalize();

  // つま先を法線に合わせる
  AdjustNormal(pTipJoint, m_TargetNormal);

  if (found)
  {
    CalkIk2(this->GetJointNode(), m_TargetPosition, this->GetEffectorJoint()->GetWorldMatrix().GetElemPosition(), m_JointLength0, m_JointLength1);
  }

#if 0
  // @attention とりあえず実装が簡単なCCD、あとでもっと軽い処理に
  if (found)
  {
    static const u32 CALK_COUNT = 10;
    for (u32 i = 0; i < CALK_COUNT; ++i)
    {
      instance::JointInstanceNode* pTmp = m_pChainJoint;
      do {
        pTmp = pTmp->GetChild()->SafeCast<instance::JointInstanceNode>();
        CalkIk(pTmp, this->GetEffectorJoint(), m_TargetPosition);
      } while (pTmp != this->GetEffectorJoint());
    }
  }
#endif

  // @attention つま先の位置からかかとの位置を逆算した方がキレイかも

  // つま先がめり込んでいたら補正する
  math::Vector3 tmp;
  found = m_pCallback(&m_TargetPosition, &tmp, pTipJoint, m_pUserData);

  if (found)
  {
    math::Matrix34 invMatrix = this->GetEffectorJoint()->GetWorldMatrix().Inverse();

    math::Vector3 localTargetPosition = invMatrix.TransformCoord(m_TargetPosition);
    math::Vector3 localOriginalPosition = invMatrix.TransformCoord(pTipJoint->GetWorldMatrix().GetElemPosition());
    math::Vector3 axis = localOriginalPosition.Cross(localTargetPosition).Normalize();

    f32 dot = localTargetPosition.Normalize().Dot(localOriginalPosition.Normalize());
    if (dot >= 1.0f - 0.000001f)
    {
      return;
    }
    f32 angle = acosf(dot);

    f32 cosAngle = cosf(angle * 0.5f);
    f32 sinAngle = sinf(angle * 0.5f);
    math::Quaternion q(sinAngle * axis.x, sinAngle * axis.y, sinAngle * axis.z, cosAngle);
    instance::JointInstanceNode* pEffector = this->GetEffectorJoint();
    pEffector->SetLocalRotation(pEffector->GetLocalRotation() * q);

    // ワールド行列更新
    gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast(this->GetEffectorJoint());
  }
}

}}

// eof
