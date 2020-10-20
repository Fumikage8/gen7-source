#include <Animation/include/gfl2_SpringJointFunction.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

using namespace gfl2;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
SpringJointFunction::SpringJointFunction() :
  m_Damping(0.0f),
  m_Stiffness(0.0f),
  m_Mass(0.0f),
  m_Gravity(0.0f),
  m_RotOnlyY(false),
  m_ExternalForce(),
  m_DownForce(0.0f),
  m_ResetFlag(true),
  m_Position(),
  m_Velocity()
{
}

/**
 * @brief デストラクタ
 */
SpringJointFunction::~SpringJointFunction()
{
}

/**
 * @brief 更新
 *
 * @param springEnable バネのシミュレーション有効フラグ
 */
bool SpringJointFunction::Update(bool springEnable)
{
  instance::JointInstanceNode* pJointNode = this->GetJointNode();
  instance::JointInstanceNode* pParentNode = pJointNode->GetParent()->SafeCast<instance::JointInstanceNode>();
  math::Matrix34 worldMatrix = pJointNode->GetWorldMatrix();

  // パラメータリセット
  if (m_ResetFlag || !springEnable)
  {
    m_ResetFlag = false;

    m_Velocity = math::Vector3::GetZero();
    m_Position = worldMatrix.GetElemPosition();
  }

  // 外力計算
  math::Vector3 force = worldMatrix.GetElemPosition() - m_Position;

  // ずれが一定値以下なら終了にする
  f32 forceLength = force.Length();
  bool isEnd = (forceLength < 0.01f);

  force += m_ExternalForce;
  force.y += m_DownForce;
  force *= m_Stiffness;
  force.y -= m_Gravity;

  // 加速度計算
  math::Vector3 accel = force / m_Mass;

  // バネシミュレーション
  if (springEnable)
  {
    // 速度計算
    m_Velocity += accel;
    m_Velocity *= m_Damping;

    // 位置計算
    m_Position += m_Velocity;
  }

  math::Vector3 localSpringPosition = pParentNode->GetWorldMatrix().Inverse().TransformCoord(m_Position);
  math::Vector3 n = localSpringPosition.Normalize();

  // 目標までの角度、回転軸を求める
  math::Vector3 axisX(1, 0, 0);
  f32 dot = axisX.Dot(n);
  f32 angle = acos(dot);
  if (angle < 0.00001f)
  {
    return isEnd;
  }

  math::Vector3 axis = axisX.Cross(n);

  // ０割りチェック
  f32 axis_length = axis.Length();
  if (axis_length < 0.00001f)
  {
    return isEnd;
  }

  // 正規化
  axis /= axis_length;

  // Ｙ軸回転だけの設定
  if (m_RotOnlyY)
  {
    if (math::IsAlmostZero(axis.y))
    {
      return isEnd;
    }
    axis.x = 0.0f;
    axis.z = 0.0f;
    axis = axis.Normalize();
  }

  f32 cosAngle = cos(angle * 0.5f);
  f32 sinAngle = sin(angle * 0.5f);
  math::Quaternion q(sinAngle * axis.x, sinAngle * axis.y, sinAngle * axis.z, cosAngle);

  pParentNode->SetLocalRotation(pParentNode->GetLocalRotation() * q);

  // ワールド行列更新
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast(pParentNode);

  m_Position = pJointNode->GetWorldMatrix().GetElemPosition();

  return isEnd;
}

}}

// eof
