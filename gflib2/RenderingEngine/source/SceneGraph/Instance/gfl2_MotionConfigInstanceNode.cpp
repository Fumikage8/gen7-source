#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionConfigInstanceNode.h>
#include <RenderingEngine/include/renderer/gfl2_CombinerShaderDriver.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  ModelInstanceNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MotionConfigInstanceNode::MotionConfigInstanceNode() : NodeTrait<MotionConfigInstanceNode, InstanceNode>(),
  m_BlendEnable(true),
  m_InterestNodeCount(0),
  m_SpringNodeCount(0),
  m_IKNodeCount(0),
  m_ExpressionNodeCount(0),
  m_pInterestNodeList(NULL),
  m_pSpringNodeList(NULL),
  m_pIKNodeList(NULL),
  m_pExpressionNodeList(NULL),
  m_pBlendList(NULL),
  m_pInterestOldRotationList(NULL)
{
}

MotionConfigInstanceNode::~MotionConfigInstanceNode()
{
  if (m_pInterestNodeList != NULL)
  {
    this->GetInstanceAllocator()->SystemMemoryFree(m_pInterestNodeList);
    m_pInterestNodeList = NULL;
  }
  if (m_pSpringNodeList != NULL)
  {
    this->GetInstanceAllocator()->SystemMemoryFree(m_pSpringNodeList);
    m_pSpringNodeList = NULL;
  }
  if (m_pIKNodeList != NULL)
  {
    this->GetInstanceAllocator()->SystemMemoryFree(m_pIKNodeList);
    m_pIKNodeList = NULL;
  }
  if (m_pExpressionNodeList != NULL)
  {
    this->GetInstanceAllocator()->SystemMemoryFree(m_pExpressionNodeList);
    m_pExpressionNodeList = NULL;
  }

  if (m_pInterestOldRotationList != NULL)
  {
    this->GetInstanceAllocator()->SystemMemoryFree(m_pInterestOldRotationList);
    m_pInterestOldRotationList = NULL;
  }
}

void MotionConfigInstanceNode::SetMotionConfigData( const resource::MotionConfigResourceNode* pResourceNode )
{
	//SetNodeName( rData.m_Name );

  m_InterestNodeCount = pResourceNode->GetInterestCount();
  m_SpringNodeCount = pResourceNode->GetSpringCount();
  m_IKNodeCount = pResourceNode->GetIKCount();
  m_ExpressionNodeCount = pResourceNode->GetExpressionCount();

  // 注目
  if (m_InterestNodeCount > 0)
  {
    u32 interestMemSize = sizeof(resource::MotionConfigResourceNode::InterestNode) * m_InterestNodeCount;
    m_pInterestNodeList = (resource::MotionConfigResourceNode::InterestNode*)this->GetInstanceAllocator()->SystemMemoryMalloc(interestMemSize);
    std::memcpy(m_pInterestNodeList, pResourceNode->GetInterest(0), interestMemSize);
  }

  // バネ
  if (m_SpringNodeCount > 0)
  {
    u32 springMemSize = sizeof(resource::MotionConfigResourceNode::SpringNode) * m_SpringNodeCount;
    m_pSpringNodeList = (resource::MotionConfigResourceNode::SpringNode*)this->GetInstanceAllocator()->SystemMemoryMalloc(springMemSize);
    std::memcpy(m_pSpringNodeList, pResourceNode->GetSpring(0), springMemSize);
  }

  // IK
  if (m_IKNodeCount > 0)
  {
    u32 ikMemSize = sizeof(resource::MotionConfigResourceNode::IKNode) * m_IKNodeCount;
    m_pIKNodeList = (resource::MotionConfigResourceNode::IKNode*)this->GetInstanceAllocator()->SystemMemoryMalloc(ikMemSize);
    std::memcpy(m_pIKNodeList, pResourceNode->GetIK(0), ikMemSize);
  }

  // エクスプレッション
  if (m_ExpressionNodeCount > 0)
  {
    u32 expressionMemSize = sizeof(resource::MotionConfigResourceNode::ExpressionNode) * m_ExpressionNodeCount;
    m_pExpressionNodeList = (resource::MotionConfigResourceNode::ExpressionNode*)this->GetInstanceAllocator()->SystemMemoryMalloc(expressionMemSize);
    std::memcpy(m_pExpressionNodeList, pResourceNode->GetExpression(0), expressionMemSize);
  }

  // ブレンドリスト取得
  m_pBlendList = pResourceNode->GetBlendList();

  // 注目：作業用回転値
  if (m_InterestNodeCount > 0)
  {
    m_pInterestOldRotationList = (math::Quaternion*)this->GetInstanceAllocator()->SystemMemoryMalloc(sizeof(math::Quaternion) * m_InterestNodeCount);
    for (u32 i = 0; i < m_InterestNodeCount; ++i)
    {
      m_pInterestOldRotationList[i].Set(0.0f, 0.0f, 0.0f, 1.0f);
    }
  }
}

u32 MotionConfigInstanceNode::GetBlendFrame(u32 fromHash, u32 toHash) const
{
  if (m_pBlendList == NULL)
  {
    return 0;
  }

  // ブレンド無効
  if (!m_BlendEnable)
  {
    return 0;
  }

  // ハッシュ値0は無効
  if (fromHash == 0)
  {
    return 0;
  }
  if (toHash == 0)
  {
    return 0;
  }

  const u32* pFromHashList = m_pBlendList->body;
  const u32* pToHashList = pFromHashList + m_pBlendList->fromCount;
  const u8* pBlendTable = reinterpret_cast<const u8*>(pToHashList + m_pBlendList->toCount);

  // to検索
  s32 toIndex = -1;
  for (u32 i = 0; i < m_pBlendList->toCount; ++i)
  {
    if (toHash == pToHashList[i])
    {
      toIndex = i;
      break;
    }
  }
  if (toIndex < 0)
  {
    return 0;
  }

  // from検索
  s32 fromIndex = 0; // fromは先頭がallなので見つからなければこれ
  for (u32 i = 0; i < m_pBlendList->fromCount; ++i)
  {
    if (fromHash == pFromHashList[i])
    {
      fromIndex = i;
      break;
    }
  }

  u8 blendFrame = pBlendTable[fromIndex * m_pBlendList->toCount + toIndex];

  return blendFrame;
}

}}}}

