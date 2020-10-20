#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_RESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_RESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

public ref class ResourceNode
{
public:
	ResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pNative);
	~ResourceNode(){ this->!ResourceNode(); }
	!ResourceNode();

	gfl2::renderingengine::scenegraph::resource::ResourceNode* GetNative()
	{
		return m_pNative;
	}

  void AddShaderResources(array<System::String^>^ paths);
  void AddTextureResources(array<System::String^>^ paths);

protected:
//private:
	gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pNative;
  c8** m_pShaderDataBuffers;
  s32 m_ShaderDataNum;
  c8** m_pTextureDataBuffers;
  s32 m_TextureDataNum;
};

}}}}}

#endif