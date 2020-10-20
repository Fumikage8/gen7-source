#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_MotionConfigInstanceNODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_MotionConfigInstanceNODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MotionConfigResourceNode.h>

#include <math/include/gfl2_Quaternion.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;
class ModelInstanceNode;

class MotionConfigInstanceNode : public NodeTrait<MotionConfigInstanceNode, InstanceNode>
{
public:

	friend class InstanceCreator;
	friend class ModelInstanceNode;

	virtual ~MotionConfigInstanceNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::MotionConfigInstanceNode") ) );
		return hash;
	}

  void SetMotionConfigData(const resource::MotionConfigResourceNode* pResourceNode);

  u32 GetInterestCount() const
  {
    return m_InterestNodeCount;
  }

  resource::MotionConfigResourceNode::InterestNode* GetInterest(u32 no)
  {
    return &m_pInterestNodeList[no];
  }
  const resource::MotionConfigResourceNode::InterestNode* GetInterest(u32 no) const
  {
    return &m_pInterestNodeList[no];
  }

  math::Quaternion* GetInterestOldRotation(u32 no)
  {
    return &m_pInterestOldRotationList[no];
  }
  const math::Quaternion* GetInterestOldRotation(u32 no) const
  {
    return &m_pInterestOldRotationList[no];
  }

  u32 GetSpringCount() const
  {
    return m_SpringNodeCount;
  }

  resource::MotionConfigResourceNode::SpringNode* GetSpring(u32 no)
  {
    return &m_pSpringNodeList[no];
  }
  const resource::MotionConfigResourceNode::SpringNode* GetSpring(u32 no) const
  {
    return &m_pSpringNodeList[no];
  }

  u32 GetIKCount() const
  {
    return m_IKNodeCount;
  }

  resource::MotionConfigResourceNode::IKNode* GetIK(u32 no)
  {
    return &m_pIKNodeList[no];
  }

  u32 GetExpressionCount() const
  {
    return m_ExpressionNodeCount;
  }

  resource::MotionConfigResourceNode::ExpressionNode* GetExpression(u32 no)
  {
    return &m_pExpressionNodeList[no];
  }
  const resource::MotionConfigResourceNode::ExpressionNode* GetExpression(u32 no) const
  {
    return &m_pExpressionNodeList[no];
  }

  void SetBlendEnable(bool enable)
  {
    m_BlendEnable = enable;
  }

  bool IsBlendEnable() const
  {
    return GFL_BOOL_CAST(m_BlendEnable);
  }

  u32 GetBlendFrame(u32 fromHash, u32 toHash) const;

protected:

	MotionConfigInstanceNode();

public:

  b8  m_BlendEnable;
  u32 : 24; // pad

  u32 m_InterestNodeCount;
  u32 m_SpringNodeCount;
  u32 m_IKNodeCount;
  u32 m_ExpressionNodeCount;

  resource::MotionConfigResourceNode::InterestNode*    m_pInterestNodeList;
  resource::MotionConfigResourceNode::SpringNode*     m_pSpringNodeList;
  resource::MotionConfigResourceNode::IKNode*         m_pIKNodeList;
  resource::MotionConfigResourceNode::ExpressionNode* m_pExpressionNodeList;
  const resource::MotionConfigResourceNode::BlendListHeader* m_pBlendList;

  // 作業用
  math::Quaternion*                               m_pInterestOldRotationList;
};

}}}}

#endif

