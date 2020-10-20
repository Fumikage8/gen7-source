
#include <Animation/include/gfl2_InterestController.h>

#include <Debug/include/gfl2_DebugPrint.h>

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
InterestController::InterestController() :
  m_pModelNode(NULL),
  m_pMotionConfigNode(NULL),
  m_pInterestJointFunctionList(NULL),
  m_InterestJointFunctionCount(0),
  m_pInterestPosition(NULL),
  m_isFinish(false),
  m_isAdjustEnabled(false),
  m_TagAdjustAngle(0.0f),
  m_AdjustAngle(0.0f),
  m_TagDecline(0.0f),
  m_Decline(0.0f),
  m_AdjustSpringVal(0.050f),
  m_DeclineSpringVal(0.150f)
{
}

/**
 * @brief デストラクタ
 */
InterestController::~InterestController()
{
  this->Finalize();
}

/**
 * @brief 初期化
 *
 * @param pHeap ヒープ
 * @param pModelNode モデルノード
 * @param pMotionConfigNode モーションコンフィグノード
 */
void InterestController::Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode, gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode)
{
  m_pModelNode = pModelNode;
  m_pMotionConfigNode = pMotionConfigNode;

  // ノードがなければ無視
  if (m_pModelNode == NULL)
  {
    return;
  }
  if (m_pMotionConfigNode == NULL)
  {
    return;
  }
  m_InterestJointFunctionCount = m_pMotionConfigNode->GetInterestCount();
  if (m_InterestJointFunctionCount == 0)
  {
    return;
  }

  // 注目用の関数作成
  m_pInterestJointFunctionList = GFL_NEW_ARRAY(pHeap) InterestJointFunction[m_InterestJointFunctionCount];

  for (u32 i = 0; i < m_InterestJointFunctionCount; ++i)
  {
    InterestJointFunction& jointFunction = m_pInterestJointFunctionList[i];

    resource::MotionConfigResourceNode::InterestNode* pInterestNode = pMotionConfigNode->GetInterest(i);
    math::Quaternion* pOldRotation = m_pMotionConfigNode->GetInterestOldRotation(i);

    jointFunction.Initialize(m_pModelNode);
    jointFunction.SetInterestNode(pInterestNode, pOldRotation);

    instance::JointInstanceNode* pJointNode = GetJointNodeFromName(m_pModelNode, pInterestNode->Name);
    jointFunction.SetJointNode(pJointNode);
  }

  // 親子構造解決
  for (u32 i = 0; i < m_InterestJointFunctionCount; ++i)
  {
    InterestJointFunction& jointFunction = m_pInterestJointFunctionList[i];
    
    // ノードが無いなら無視
    if (jointFunction.GetJointNode() == NULL)
    {
      continue;
    }

    for (u32 j = 0; j < m_InterestJointFunctionCount; ++j)
    {
      InterestJointFunction& parentFunction = m_pInterestJointFunctionList[j];

      // ノードが無いなら無視
      if (parentFunction.GetJointNode() == NULL)
      {
        continue;
      }

      // 自分と同じインデックスは無視
      if (i == j)
      {
        continue;
      }

      // 既に子供がいるなら無視
      if (parentFunction.GetChildFunction() != NULL)
      {
        continue;
      }

      if (::std::strcmp(jointFunction.GetJointNode()->GetParentName(), parentFunction.GetJointNode()->GetNodeName()) == 0)
      {
        jointFunction.SetParentFunction(&parentFunction);
        parentFunction.SetChildFunction(&jointFunction);
        break;
      }
    }
  }
}

/**
 * @brief 終了
 */
void InterestController::Finalize()
{
  GFL_SAFE_DELETE_ARRAY(m_pInterestJointFunctionList);
}

/**
 * @brief 更新
 */
void InterestController::Update()
{
  // 注目用ジョイントがなければ無視
  if (m_InterestJointFunctionCount == 0)
  {
    return;
  }

  // 子供がいない骨をルートに更新
  bool isRotateLimit = true;
  bool isFinish = true;
  for (u32 i = 0; i < m_InterestJointFunctionCount; ++i)
  {
    InterestJointFunction& jointFunction = m_pInterestJointFunctionList[i];

    // @fix GFNMCat[1480] ハングを回避するためにNULLチェック追加
    if (jointFunction.GetJointNode() == NULL)
    {
      continue;
    }
    
    // 子供がいる骨は無視
    if (jointFunction.GetChildFunction() != NULL)
    {
      continue;
    }

    // 更新
    f32 overAngle0 = 0.0f;
    f32 overAngle1 = 0.0f;
    jointFunction.GetFirstAngle(m_pInterestPosition, &overAngle0, &overAngle1);

    // 角度制限
    if (!jointFunction.Update(&overAngle0, &overAngle1))
    {
      isRotateLimit = false;
    }

    if (!jointFunction.IsFinish())
    {
      isFinish = false;
    }

    // 全ての親を更新
    InterestJointFunction* pParent = jointFunction.GetParentFunction();
    while (pParent != NULL)
    {
      if (!pParent->Update(&overAngle0, &overAngle1))
      {
        isRotateLimit = false;
      }

      if (!pParent->IsFinish())
      {
        isFinish = false;
      }

      pParent = pParent->GetParentFunction();
    }
  }

  // 顔向け補正行列を計算
  this->CalculateAdjustMatrix();

  // 終了フラグ
  m_isFinish = (isRotateLimit || isFinish);
}

/**
 * @brief 終了確認
 *
 * @return true:終了 false:更新中
 */
bool InterestController::IsFinish() const
{
  return GFL_BOOL_CAST(m_isFinish);
}

/**
 * 顔向け補正行列を計算
 */
void InterestController::CalculateAdjustMatrix()
{
  static const f32 sc_limitDecline = gfl2::math::ConvDegToRad(1.0f); // 傾き限界

  // 注目用ジョイントがなければ無視
  if (m_InterestJointFunctionCount == 0)
  {
    return;
  }

  // 顔向け傾きを更新
 	m_Decline += (m_TagDecline - m_Decline) * m_DeclineSpringVal; // この値は何？よくわからないのでkujiraからマジックナンバーのまま移植
  if (this->IsAdjustAngleEnabled() || (sc_limitDecline < gfl2::math::FAbs(m_Decline)))
  {
    // 補正角度を更新
    m_AdjustAngle += (m_TagAdjustAngle - m_AdjustAngle) * m_AdjustSpringVal; // この値は何？よくわからないのでkujiraからマジックナンバーのまま移植

    // @fix GFNMCat[2527]
    // ルートのジョイントを補正タイプに合わせて回転させる
    for (u32 i = 0; i < m_InterestJointFunctionCount; ++i)
    {
      // 親がいなければルート
      if (m_pInterestJointFunctionList[i].GetParentFunction() == NULL)
      {
        m_pInterestJointFunctionList[i].CalculateAdjustMatrix(m_AdjustAngle, m_Decline);
      }
    }
  }
}

}}

// eof
