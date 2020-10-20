#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_CameraRESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_CameraRESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class CameraResourceNodeFactory;

class CameraResourceNode : public NodeTrait<CameraResourceNode, ResourceNode>, public NodeName
{
public:
	friend class CameraResourceNodeFactory;

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::CameraResourceNode") ) );
		return hash;
	}

	struct CameraData{
		math::Matrix44						m_LocalMatrix;
		math::Vector4						m_LocalScale;
		math::Vector4						m_LocalTransform;
		math::Vector4						m_LocalRotation;

		f32		                  m_Near;
		f32			                m_Far;
		f32				              m_Fovy;
		f32                     m_AspectRatio;
	};

	virtual ~CameraResourceNode();

	void SetData( const void* data, int size );
	const CameraData* GetCameraData() const { return &m_CameraData; }

	const c8* GetName() const { return this->GetNodeName(); }
	u32 GetNameHash() const { return m_NameHash; }

protected:
	CameraResourceNode();

	u32						m_NameHash;
	CameraData		m_CameraData;
};

}}}}

#endif

