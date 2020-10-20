#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_DrawEnvRESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_DrawEnvRESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <math/include/gfl2_MathAABB.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class DrawEnvResourceNodeFactory;

class DrawEnvResourceNode : public NodeTrait<DrawEnvResourceNode, ResourceNode>, public NodeName
{
public:
  //kawa20150513
  struct Fog
  {
    u32          enable;      // 1 or 0
    f32          nearLength;
    f32          farLength;
    f32          strength;
    math::Vector4 color;
  };
  struct GlareFilter
  {
    u16          enable;     // 1 or 0
    u16          reduceNum;
    f32          strength;
    f32          range;
    f32          intensity;
    math::Vector4 weight;
  };
  static const u32        FOG_NUM  =  4;


public:
	friend class DrawEnvResourceNodeFactory;

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::DrawEnvResourceNode") ) );
		return hash;
	}

	struct EnvData{  // ←gfl2_GfEnvData.cppのDrawEnvと同じ内容
		c8											          m_EnvTextureFilePath[Constant::FLAT_RESOURCE_NAME_MAX];
		math::Vector4                      m_WorldPos;
		math::Vector4                      m_FrontDOF;
		math::Vector4                      m_BackDOF;
		math::AABB							          m_AABB;

    //kawa20150513
    DrawEnvResourceNode::Fog          m_Fog[DrawEnvResourceNode::FOG_NUM];
    DrawEnvResourceNode::GlareFilter  m_GlareFilter;

		int                               m_Priority;
		u32                               m_IsLinkEnv;
	};

	virtual ~DrawEnvResourceNode();

	void SetData( const void* data, int size );
	const EnvData* GetData() const { return &m_EnvData; };

	const c8* GetName() const { return this->GetNodeName(); }
	u32 GetNameHash() const { return m_NameHash; }

protected:
	DrawEnvResourceNode();

	u32						m_NameHash;
	EnvData				m_EnvData;
};

}}}}

#endif

