#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_JointInstanceNODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_JointInstanceNODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_TransformNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;
class ModelInstanceNode;

class JointInstanceNode : public NodeTrait<JointInstanceNode, TransformNode>
{
public:

	friend class InstanceCreator;
	friend class ModelInstanceNode;

	virtual ~JointInstanceNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::JointInstanceNode") ) );
		return hash;
	}

  const c8* GetNodeName() const { return m_pResourceData->m_Name; }
	const c8* GetName() const { return GetNodeName(); }
	const c8* GetParentName() const { return m_pResourceData->m_ParentName; }

#ifdef GF_PLATFORM_CTR
  s16 GetParentIndex() const { return m_pResourceData->m_ParentIndex; }
#endif

	b32 IsNeedRendering() const { return m_NeededRendering; }
	void SetNeedRendering() { m_NeededRendering = true; }
  const math::SRT& GetDefaultSRT() const { return m_DefaultSRT; }
  const math::Vector3& GetDefaultScale() const { return m_DefaultSRT.scale; }
  const math::Quaternion& GetDefaultRotation() const { return m_DefaultSRT.rotation; }
  const math::Vector3& GetDefaultTranslate() const { return m_DefaultSRT.translate; }

  void SetJointData( const resource::GfMdlResourceNode::JointData *pData );

  const math::Matrix34& GetBindPoseMatrix() const { return m_BindPoseMatrix; }

  const math::Matrix34& GetSkinningMatrix() const { return m_SkinningMatrix; }
  const math::Matrix34* GetSkinningMatrixPtr() const { return &m_SkinningMatrix; }

protected:
	JointInstanceNode();

private:

  math::SRT                                       m_DefaultSRT;
	b32																							m_NeededRendering;

	math::Matrix34																	m_BindPoseMatrix;
	math::Matrix34																	m_SkinningMatrix;
  const resource::GfMdlResourceNode::JointData    *m_pResourceData;
};

}}}}

#endif

