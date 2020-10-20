#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_TEXTURERESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_TEXTURERESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class TextureResourceNodeFactory;

class TextureResourceNode : public NodeTrait<TextureResourceNode, ResourceNode>, public NodeName
{
public:
	friend class TextureResourceNodeFactory;

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::TextureResourceNode") ) );
		return hash;
	}

	virtual ~TextureResourceNode();

	void SetImage( const void* data, int size, gfx::Pool pool, gfx::IGLAllocator* pAllocator = NULL );

	const c8* GetName() const { return this->GetNodeName(); }
	u32 GetNameHash() const { return m_NameHash; }

	const gfx::Texture* GetTexture() const { return m_pTexture; }

protected:

  TextureResourceNode();
  TextureResourceNode(gfx::Texture* pTexture, b32 isOwner);

	gfx::Texture										*m_pTexture;
	u32															m_NameHash;
	b32															m_isOwner;
  void                            *m_pVramTexture;
};

}}}}

#endif

