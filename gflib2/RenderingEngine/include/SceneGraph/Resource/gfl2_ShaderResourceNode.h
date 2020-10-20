#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_SHADERRESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_SHADERRESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class ShaderResourceNodeFactory;

class ShaderResourceNode : public NodeTrait<ShaderResourceNode, ResourceNode>, public NodeName
{
public:
	friend class ShaderResourceNodeFactory;

	class ShaderKind{
	public:
		enum Enum{
			Vertex,
			Fragment,
			Geometry,
			Max
		};
	};

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::ShaderResourceNode") ) );
		return hash;
	}

	virtual ~ShaderResourceNode();

	void SetData( const void* data, int size );
	const c8* GetName() const { return this->GetNodeName(); }
	u32 GetNameHash() const { return m_NameHash; }

#if defined(GF_PLATFORM_CTR)
	const u8* GetVertexShader() const { return m_pShader[ShaderKind::Vertex]; }
	const u8* GetFragmentShader() const { return m_pShader[ShaderKind::Fragment]; }
	const u8* GetGeometryShader() const { return m_pShader[ShaderKind::Geometry]; }

	u32 GetVertexShaderSize() const { return m_ShaderSize[ShaderKind::Vertex]; }
	u32 GetFragmentShaderSize() const { return m_ShaderSize[ShaderKind::Fragment]; }
	u32 GetGeometryShaderSize() const { return m_ShaderSize[ShaderKind::Geometry]; }

	u32 GetShaderHash( ShaderKind::Enum type ) const { return m_DataHash[type]; }

#elif defined(GF_PLATFORM_WIN32)
	const gfx::Shader* GetVertexShader() const { return m_pShader[ShaderKind::Vertex]; }
	const gfx::Shader* GetFragmentShader() const { return m_pShader[ShaderKind::Fragment]; }
	const gfx::Shader* GetGeometryShader() const { return m_pShader[ShaderKind::Geometry]; }
#endif

	

protected:

	ShaderResourceNode();

#if defined(GF_PLATFORM_CTR)
	const u8*												m_pShader[ShaderKind::Max];
	u32															m_ShaderSize[ShaderKind::Max];
	u32															m_DataHash[ShaderKind::Max];
#elif defined(GF_PLATFORM_WIN32)
	gfx::Shader*										m_pShader[ShaderKind::Max];
#endif

	u32															m_NameHash;
};

}}}}

#endif

