#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_BlendShapeAnimationRESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_BlendShapeAnimationRESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <math/include/gfl2_Matrix.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class BlendShapeAnimationResourceNodeFactory;

class BlendShapeAnimationResourceNode : public NodeTrait<BlendShapeAnimationResourceNode, ResourceNode>, public NodeName
{
public:
	friend class BlendShapeAnimationResourceNodeFactory;

	struct FrameNodeData{
		FrameNodeData()
		{
			::std::memset( m_BlendMeshName, 0, sizeof(m_BlendMeshName) );
		}
		c8							m_BlendMeshName[Constant::BLEND_MESH_MAX][Constant::MESH_NAME_MAX];
		u32							m_BlendMeshNameHash[Constant::BLEND_MESH_MAX];
		f32							m_BlendWeight[Constant::BLEND_MESH_MAX];
	};

	virtual ~BlendShapeAnimationResourceNode();

	void SetData( const void* data, u32 size, u32 frameSize, b32 isLoop );

	u32 GetFrameSize() const { return m_FrameSize; }
  bool IsLoop() const { return m_IsLoop != 0; }
	const c8* GetTargetName() const { return m_Name; }
	u32 GetTargetNameHash() const { return m_NameHash; }
	const FrameNodeData* GetFrameNodeData( u32 frame ) const { return &m_pFrameNodeDataList[frame]; }

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash			hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::BlendShapeAnimationResourceNode") ) );
		return hash;
	}

  void GetBlendShape(f32 frame, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMesh, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode) const;

protected:
	BlendShapeAnimationResourceNode();

private:

	u32							m_FrameSize;
  b32             m_IsLoop;
	c8							m_Name[Constant::MESH_NAME_MAX];
	u32							m_NameHash;
	FrameNodeData		*m_pFrameNodeDataList;

};

}}}}

#endif

