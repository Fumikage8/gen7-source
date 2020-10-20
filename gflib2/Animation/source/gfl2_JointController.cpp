#include <Animation/include/gfl2_JointController.h>

using namespace gfl2;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph;

namespace gfl2 { namespace animation { 

#if 0
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
#endif

/**
 * @brief コンストラクタ
 */
JointController::JointController() :
  m_pModelNode(NULL),
  m_pMotionConfigNode(NULL)
{
}

/**
 * @brief デストラクタ
 */
JointController::~JointController()
{
}

/**
 * @brief 初期化
 *
 * @param pModelNode モデルノード
 * @param pMotionConfigNode モーションコンフィグノード
 */
void JointController::Initialize(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode, gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode)
{
  m_pModelNode = pModelNode;
  m_pMotionConfigNode = pMotionConfigNode;

  m_pModelNode->SetMotionConfigInstanceNode(pMotionConfigNode);

  // Interest
  m_InterestController.Initialize(NULL, m_pModelNode, m_pMotionConfigNode);

  // Spring
  m_SpringController.Initialize(NULL, m_pModelNode, m_pMotionConfigNode);

  // IK
  m_IKController.Initialize(m_pModelNode, m_pMotionConfigNode);
}

/**
 * @brief 終了
 */
void JointController::Finalize()
{
  // Interest
  m_InterestController.Finalize();

  // Spring
  m_SpringController.Finalize();

  // IK
  m_IKController.Finalize();
}

/**
 * @brief 更新
 */
void JointController::Update()
{
  // Interest
  m_InterestController.Update();

  // Spring
  //m_SpringController.Update();

  // IK
  //m_IKController.Update();
}

}}

// eof
