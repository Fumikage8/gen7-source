#include <Animation/include/gfl2_SpringController.h>

using namespace gfl2;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph;

namespace gfl2 { namespace animation { 

static instance::JointInstanceNode* GetJointNodeFromName(instance::ModelInstanceNode* pModelNode, const c8* pName)
{
  for (u32 j = 0; j < pModelNode->GetJointNum(); ++j)
  {
    instance::JointInstanceNode* pJointNode = pModelNode->GetJointInstanceNode(j);
    if (std::strcmp(pJointNode->GetNodeName(), pName) == 0)
    {
      return pJointNode;
    }
  }

  return NULL;
}

/**
 * @brief コンストラクタ
 */
SpringController::SpringController() :
  m_pModelNode(NULL),
  m_pMotionConfigNode(NULL),
  m_pSpringJointFunctionList(NULL),
  m_SpringJointFunctionCount(0),
  m_SpringEnable(false)
{
  for (u32 i = 0; i < SIDE_COUNT; ++i)
  {
    m_pSpringJointFunctionLR[i] = NULL;
  }
}

/**
 * @brief デストラクタ
 */
SpringController::~SpringController()
{
  if (m_pSpringJointFunctionList != NULL)
  {
    delete[] m_pSpringJointFunctionList;
  }
}

/**
 * @brief 初期化
 *
 * @param pHeap ヒープ
 * @param pModelNode モデルノード
 * @param pMotionConfigNode モーションコンフィグノード
 */
void SpringController::Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode, const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode)
{
  m_pModelNode = pModelNode;
  m_pMotionConfigNode = pMotionConfigNode;

  static const f32 DEFAULT_GRAVITY = (980.665f / (60.0f * 60.0f)); ///< デフォルトの重力加速度

  m_SpringJointFunctionCount = m_pMotionConfigNode->GetSpringCount();
  if (m_SpringJointFunctionCount == 0)
  {
    return;
  }

  for (u32 i = 0; i < SIDE_COUNT; ++i)
  {
    m_pSpringJointFunctionLR[i] = NULL;
  }

  m_pSpringJointFunctionList = GFL_NEW_ARRAY(pHeap) SpringJointFunction[m_SpringJointFunctionCount];

  for (u32 i = 0; i < m_SpringJointFunctionCount; ++i)
  {
    SpringJointFunction& jointFunction = m_pSpringJointFunctionList[i];

    const resource::MotionConfigResourceNode::SpringNode* pSpring = m_pMotionConfigNode->GetSpring(i);

    jointFunction.Initialize(m_pModelNode);

    instance::JointInstanceNode* pJointNode = GetJointNodeFromName(m_pModelNode, pSpring->Name);
    jointFunction.SetJointNode(pJointNode);

    jointFunction.SetDamping(pSpring->Damping);
    jointFunction.SetStiffness(pSpring->Stiffness);
    jointFunction.SetMass(pSpring->Mass);
    jointFunction.SetGravity(pSpring->Gravity * DEFAULT_GRAVITY);
    for (u32 j = 0; j < m_pMotionConfigNode->GetExpressionCount(); ++j)
    {
      const resource::MotionConfigResourceNode::ExpressionNode* pExpression = m_pMotionConfigNode->GetExpression(j);
      if (std::strcmp(pExpression->Src0, pSpring->Name) == 0)
      {
        jointFunction.SetRotOnlyY(pExpression->Type == resource::MotionConfigResourceNode::ExpressionNode::TYPE_ROOT_AIM);
        break;
      }
    }

    // 左、右セット
    if (pSpring->Name[0] == 'L')
    {
      // 既になにか入っているのはおかしい
      GFL_ASSERT(m_pSpringJointFunctionLR[SIDE_L] == NULL);

      m_pSpringJointFunctionLR[SIDE_L] = &jointFunction;
    }
    else if (pSpring->Name[0] == 'R')
    {
      // 既になにか入っているのはおかしい
      GFL_ASSERT(m_pSpringJointFunctionLR[SIDE_R] == NULL);

      m_pSpringJointFunctionLR[SIDE_R] = &jointFunction;
    }
  }
}

/**
 * @brief 終了
 */
void SpringController::Finalize()
{
  GFL_SAFE_DELETE_ARRAY(m_pSpringJointFunctionList);
}

/**
 * @brief 更新
 */
void SpringController::Update()
{
  // 更新
  bool isEnd = true;
  for (u32 i = 0; i < m_SpringJointFunctionCount; ++i)
  {
    if (!m_pSpringJointFunctionList[i].Update(m_SpringEnable))
    {
      isEnd = false;
    }
  }

  // 終了判定
  if (!m_SpringEnableReq && isEnd)
  {
    m_SpringEnable = false;
  }
}

/**
 * @brief バネ制御有効無効を設定
 *
 * @param enable true:バネ制御有効 false:バネ制御無効
 */
void SpringController::SetEnable(bool enable)
{
  // 切り替えリクエスト
  m_SpringEnableReq = enable;

  // 有効にする場合はすぐに反映
  if (enable)
  {
    m_SpringEnable = enable;
  }
  // 無効にするなら外力を戻す
  else
  {
    for (u32 i = 0; i < SIDE_COUNT; ++i)
    {
      this->SetExternalForce(static_cast<Side>(i), gfl2::math::Vector3(0.0f, 0.0f, 0.0f));
      this->SetDownForce(static_cast<Side>(i), 0.0f);
    }
  }
}

/**
 * @brief バネ制御有効無効を取得
 *
 * @return true:バネ制御有効 false:バネ制御無効
 */
bool SpringController::IsEnable() const
{
  return GFL_BOOL_CAST(m_SpringEnable);
}

/**
 * @brief 減衰設定
 *
 * @param lr SIDE_L or SIDE_R
 * @param damping 減衰
 */
void SpringController::SetDamping(Side lr, f32 damping)
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return;
  }

  m_pSpringJointFunctionLR[lr]->SetDamping(damping);
}

/**
 * @brief 減衰取得
 *
 * @param lr SIDE_L or SIDE_R
 * @return 減衰を返す
 */
f32 SpringController::GetDamping(Side lr) const
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return 0.0f;
  }

  return m_pSpringJointFunctionLR[lr]->GetDamping();
}

/**
 * @brief バネ定数設定
 *
 * @param lr SIDE_L or SIDE_R
 * @param stiffness バネ定数
 */
void SpringController::SetStiffness(Side lr, f32 stiffness)
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return;
  }

  m_pSpringJointFunctionLR[lr]->SetStiffness(stiffness);
}

/**
 * @brief バネ定数取得
 *
 * @param lr SIDE_L or SIDE_R
 * @return バネ定数を返す
 */
f32 SpringController::GetStiffness(Side lr) const
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return 0.0f;
  }

  return m_pSpringJointFunctionLR[lr]->GetStiffness();
}

/**
 * @brief 質量設定
 *
 * @param lr SIDE_L or SIDE_R
 * @param mass 質量
 */
void SpringController::SetMass(Side lr, f32 mass)
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return;
  }

  m_pSpringJointFunctionLR[lr]->SetMass(mass);
}

/**
 * @brief 質量取得
 *
 * @param lr SIDE_L or SIDE_R
 * @return 質量を返す
 */
f32 SpringController::GetMass(Side lr) const
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return 0.0f;
  }

  return m_pSpringJointFunctionLR[lr]->GetMass();
}

/**
 * @brief 重力設定
 *
 * @param lr SIDE_L or SIDE_R
 * @param gravity 重力
 */
void SpringController::SetGravity(Side lr, f32 gravity)
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return;
  }

  m_pSpringJointFunctionLR[lr]->SetGravity(gravity);
}

/**
 * @brief 重力取得
 *
 * @param lr SIDE_L or SIDE_R
 * @return 重力を返す
 */
f32 SpringController::GetGravity(Side lr) const
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return 0.0f;
  }

  return m_pSpringJointFunctionLR[lr]->GetGravity();
}

/**
 * @brief Ｙ軸回転だけ行う設定
 *
 * @param lr SIDE_L or SIDE_R
 * @param flag Ｙ軸回転だけならtrue, 全て回転させるならfalse
 */
void SpringController::SetRotOnlyY(Side lr, b32 flag)
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return;
  }

  m_pSpringJointFunctionLR[lr]->SetRotOnlyY(flag);
}

/**
 * @brief Ｙ軸回転だけ行うか？
 *
 * @param lr SIDE_L or SIDE_R
 * @return Ｙ軸回転だけならtrue, 全て回転させるならfalse
 */
b32 SpringController::IsRotOnlyY(Side lr) const
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return false;
  }

  return m_pSpringJointFunctionLR[lr]->IsRotOnlyY();
}

/**
 * @brief 外力設定
 *
 * @param lr SIDE_L or SIDE_R
 * @param externalForce 外力
 */
void SpringController::SetExternalForce(Side lr, const math::Vector3& externalForce)
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return;
  }

  m_pSpringJointFunctionLR[lr]->SetExternalForce(externalForce);
}

/**
 * @brief 外力取得
 *
 * @param lr SIDE_L or SIDE_R
 * @return 外力を返す
 */
const math::Vector3 SpringController::GetExternalForce(Side lr) const
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return math::Vector3();
  }

  return m_pSpringJointFunctionLR[lr]->GetExternalForce();
}

/**
 * @brief ダウンフォース設定
 *
 * @param lr SIDE_L or SIDE_R
 * @param downForce ダウンフォース
 */
void SpringController::SetDownForce(Side lr, f32 downForce)
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return;
  }

  m_pSpringJointFunctionLR[lr]->SetDownForce(downForce);
}

/**
 * @brief ダウンフォース取得
 *
 * @param lr SIDE_L or SIDE_R
 *
 * @return ダウンフォースを返す
 */
f32 SpringController::GetDownForce(Side lr) const
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return 0.0f;
  }

  return m_pSpringJointFunctionLR[lr]->GetDownForce();
}

/**
 * @brief パラメータのリセットリクエスト
 *
 * @param lr SIDE_L or SIDE_R
 */
void SpringController::RequestParameterReset(Side lr)
{
  if (m_pSpringJointFunctionLR[lr] == NULL)
  {
    return;
  }

  m_pSpringJointFunctionLR[lr]->RequestParameterReset();
}


}}

// eof
