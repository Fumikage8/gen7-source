#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_MOTIONCONFIGRESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_MOTIONCONFIGRESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_MathAABB.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class MotionConfigResourceNodeFactory;

class MotionConfigResourceNode : public NodeTrait<MotionConfigResourceNode, ResourceNode>, public NodeName
{
public:

	friend class MotionConfigResourceNodeFactory;

  struct ConfigType
  {
    enum
    {
      Top,
      Interest,
      Spring,
      IK,
      Expression,
      BlendList,
    };
  };

  struct DataHeader
  {
    u32 type;
    u32 size;
    u32 offset;
  };

  struct ConfigData
  {
    u32 version;
    u32 dataCount;
    DataHeader dataHeaderList[1];
  };

  struct ConfigTop
  {
    u32 frameSize;
    b32 loopFlag;
    math::AABB aabb;
  };

  struct InterestNode
  {
    c8 Name[Constant::JOINT_NAME_MAX];
    u32 Type;
    f32 Speed;
    f32 Low;
    f32 High;
    f32 Low2;
    f32 High2;
  };

  struct InterestHeader
  {
    u32 nodeCount;
    InterestNode nodeList[1];
  };

  struct SpringNode
  {
    c8 Name[Constant::JOINT_NAME_MAX];
    f32 Damping;
    f32 Stiffness;
    f32 Mass;
    f32 Gravity;
  };

  struct SpringHeader
  {
    u32 nodeCount;
    SpringNode nodeList[1];
  };

  struct IKNode
  {
    enum TYPE
    {
      TYPE_NORMAL,
      TYPE_REVERSE,
      TYPE_EFFECTOR_CHILD,
    };

    c8 Chain[Constant::JOINT_NAME_MAX];
    c8 Joint[Constant::JOINT_NAME_MAX];
    c8 Effector[Constant::JOINT_NAME_MAX];
    c8 EffectorChild[Constant::JOINT_NAME_MAX];
    c8 Tip[Constant::JOINT_NAME_MAX];

    u32 Type;
    b32 Hold;
    b32 Land;
  };

  struct IKHeader
  {
    u32 nodeCount;
    IKNode nodeList[1];
  };

  struct ExpressionNode
  {
    enum TYPE
    {
      TYPE_AIM,
      TYPE_ROOT_AIM,
    };

    c8 Dst[Constant::JOINT_NAME_MAX];
    c8 Src0[Constant::JOINT_NAME_MAX];
    u32 Type;
  };

  struct ExpressionHeader
  {
    u32 nodeCount;
    ExpressionNode nodeList[1];
  };

  struct BlendListHeader
  {
    u32 fromCount;
    u32 toCount;

    u32 body[1];
  };

public:

	virtual ~MotionConfigResourceNode();

	inline const c8* GetName() const { return GetNodeName(); }

	void SetData(const u8* data);

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::MotionConfigResourceNode") ) );
		return hash;
	}

  u32 GetInterestCount() const
  {
    return (m_pInterestHeader != NULL)
      ? m_pInterestHeader->nodeCount
      : 0;
  }

  const InterestNode* GetInterest(u32 no) const
  {
    return &m_pInterestHeader->nodeList[no];
  }

  u32 GetSpringCount() const
  {
    return (m_pSpringHeader != NULL)
      ? m_pSpringHeader->nodeCount
      : 0;
  }

  const SpringNode* GetSpring(u32 no) const
  {
    return &m_pSpringHeader->nodeList[no];
  }

  u32 GetIKCount() const
  {
    return (m_pIKHeader != NULL)
      ? m_pIKHeader->nodeCount
      : 0;
  }

  const IKNode* GetIK(u32 no) const
  {
    return &m_pIKHeader->nodeList[no];
  }

  u32 GetExpressionCount() const
  {
    return (m_pExpressionHeader != NULL)
      ? m_pExpressionHeader->nodeCount
      : 0;
  }

  const ExpressionNode* GetExpression(u32 no) const
  {
    return &m_pExpressionHeader->nodeList[no];
  }

  const BlendListHeader* GetBlendList() const
  {
    return m_pBlendListHeader;
  }

  static bool IsBmcnfSupportedVersion(u32 version);

protected:
	MotionConfigResourceNode();

public:


private:

  void SetDataInterest(const u8*& pData);
  void SetDataSpring(const u8*& pData);
  void SetDataIK(const u8*& pData);
  void SetDataExpression(const u8*& pData);

private:

  const ConfigData* m_pConfigData;

  // Top
  const ConfigTop* m_pConfigTop;

  // Interest
  const InterestHeader* m_pInterestHeader;

  // Spring
  const SpringHeader* m_pSpringHeader;

  // IK
  const IKHeader* m_pIKHeader;

  // Expression
  const ExpressionHeader* m_pExpressionHeader;

  // BlendList
  const BlendListHeader* m_pBlendListHeader;
};

}}}}

#endif

