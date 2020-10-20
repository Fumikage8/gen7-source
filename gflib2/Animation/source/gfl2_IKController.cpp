#include <Animation/include/gfl2_IKController.h>

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
IKController::IKController() :
  m_pModelNode(NULL),
  m_pMotionConfigNode(NULL),
  m_pIKJointFunctionList(NULL),
  m_IKJointFunctionCount(0)
{
}

/**
 * @brief デストラクタ
 */
IKController::~IKController()
{
  if (m_pIKJointFunctionList != NULL)
  {
    delete[] m_pIKJointFunctionList;
  }
}

/**
 * @brief 初期化
 *
 * @param pModelNode モデルノード
 * @param pMotionConfigNode モーションコンフィグノード
 */
void IKController::Initialize(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode, gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode)
{
  m_pModelNode = pModelNode;
  m_pMotionConfigNode = pMotionConfigNode;

  m_IKJointFunctionCount = m_pMotionConfigNode->GetIKCount();
  if (m_IKJointFunctionCount == 0)
  {
    return;
  }

  m_pIKJointFunctionList = new IKJointFunction[m_IKJointFunctionCount];

  for (u32 i = 0; i < m_IKJointFunctionCount; ++i)
  {
    IKJointFunction& jointFunction = m_pIKJointFunctionList[i];

    const resource::MotionConfigResourceNode::IKNode* pIK = m_pMotionConfigNode->GetIK(i);

    jointFunction.Initialize(m_pModelNode);

    instance::JointInstanceNode* pJointNode = GetJointNodeFromName(m_pModelNode, pIK->Joint);
    jointFunction.SetJointNode(pJointNode);

    instance::JointInstanceNode* pChainNode = GetJointNodeFromName(m_pModelNode, pIK->Chain);
    jointFunction.SetChainJoint(pChainNode);

    instance::JointInstanceNode* pEffectorNode = GetJointNodeFromName(m_pModelNode, pIK->Effector);
    jointFunction.SetEffectorJoint(pEffectorNode);

    instance::JointInstanceNode* pTipJoint = GetJointNodeFromName(m_pModelNode, pIK->Tip);
    jointFunction.SetTipJoint(pTipJoint);

    jointFunction.CalcJointLength();
  }
}

/**
 * @brief 終了
 */
void IKController::Finalize()
{
  if (m_pIKJointFunctionList != NULL)
  {
    delete[] m_pIKJointFunctionList;
    m_pIKJointFunctionList = NULL;
  }
}

/**
 * @brief 更新
 */
void IKController::Update()
{
  // IK
  for (u32 i = 0; i < m_IKJointFunctionCount; ++i)
  {
    m_pIKJointFunctionList[i].Update();
  }
}

}}

// eof
