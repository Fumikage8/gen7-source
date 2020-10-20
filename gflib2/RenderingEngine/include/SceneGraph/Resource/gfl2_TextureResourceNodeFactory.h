#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_TEXTURERESOURCENODEFAACTRY_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_TEXTURERESOURCENODEFAACTRY_H_INCLUDED

//#include <iostream>
#include <memory>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_Texture.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceNodeFactory.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/**
	* @brief テクスチャリソースノードのファクトリインターフェース
	*/
class TextureResourceNodeFactory : public IResourceNodeFactory
{
public:

	static const c8* GetFactoryTypeName();

	TextureResourceNodeFactory():IResourceNodeFactory()
	{
	#if defined(GF_PLATFORM_WIN32)
		m_pTemporaryAllocator = NULL;
	#endif
    m_Pool = gfx::Pool::Default;
	}
	virtual ~TextureResourceNodeFactory() {}

#if defined(GF_PLATFORM_WIN32)
	void SetTemporaryAllocator( gfx::IGLAllocator* pAllocator ){ m_pTemporaryAllocator = pAllocator; }
#endif

	/**
		* @brief ResourceNodeのファクトリメソッド　所有権付きのリソースノードを返す
		*/
	//virtual ResourceNode* CreateResourceNode(const void* data, int size);
  virtual ResourceNode* CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size);
  /**
   * @brief ResourceNodeのファクトリメソッド　名前, テクスチャを直接渡してリソースノード作成
   */
  //virtual ResourceNode* CreateResourceNode(const c8* name, gfx::Texture* pTexture, b32 isOwner = true );
  virtual ResourceNode* CreateResourceNode(gfx::IGLAllocator* pAllocator, const c8* name, gfx::Texture* pTexture, b32 isOwner = true);

	virtual b32 IsSupport( IResourceData::Type type );

  void SetPool(gfx::Pool pool)
  {
    m_Pool = pool;
  }

  gfx::Pool GetPool() const
  {
    return m_Pool;
  }

private:
#if defined(GF_PLATFORM_WIN32)
	gfx::IGLAllocator		*m_pTemporaryAllocator;
#endif
  gfx::Pool m_Pool;
};

}}}}

#endif

