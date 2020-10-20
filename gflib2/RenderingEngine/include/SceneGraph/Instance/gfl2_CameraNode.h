#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_CameraNODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_CameraNODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_TransformNode.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <math/include/gfl2_MathAABB.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;

class CameraNode : public NodeTrait<CameraNode, TransformNode>, public NodeName
{
public:

	friend class InstanceCreator;

	virtual ~CameraNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::CameraNode") ) );
		return hash;
	}

	inline math::Matrix44 GetProjectionMatrix() const { return math::Matrix44::GetPerspective( m_Fovy, m_AspectRatio, m_Near, m_Far ); }
  inline math::Matrix34 GetViewMatrix() const { return this->GetWorldMatrix().Inverse(); } 

	inline void SetNear( f32 value ){ m_Near = value; }
	inline void SetFar( f32 value ){ m_Far = value; }
	inline void SetFovy( f32 value ){ m_Fovy = value; }
	inline void SetAspectRatio( f32 value ){ m_AspectRatio = value; }

	inline f32 GetNear() const { return m_Near; }
	inline f32 GetFar() const { return m_Far; }
	inline f32 GetFovy() const { return m_Fovy; }
	inline f32 GetAspectRatio() const { return m_AspectRatio; }

protected:
	CameraNode();

private:

	f32		                  m_Near;
	f32			                m_Far;
	f32				              m_Fovy;
	f32                     m_AspectRatio;

};

}}}}

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif

