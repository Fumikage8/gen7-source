#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_IRESOURCENODEFAACTRY_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_IRESOURCENODEFAACTRY_H_INCLUDED

//#include <iostream>
#include <memory>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceNodeFactory.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/**
	* @brief リソースノードのファクトリインターフェース
	*/
class IResourceNodeFactory : public gfx::GLMemory
{
public:

	virtual ~IResourceNodeFactory() {}

	/**
		* @brief ResourceNodeのファクトリメソッド　所有権付きのリソースノードを返す
		*/
	//virtual ResourceNode* CreateResourceNode(const void* data, int size) = 0;
  virtual ResourceNode* CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size) = 0;

	virtual b32 IsSupport( IResourceData::Type type ) = 0;

	//! @brief バージョンIDの取得
	virtual void SetDataVersionId( u32 id ){ m_DataVersionId = id; }

	//! @brief バージョンIDの取得
	virtual u32 GetDataVersionId( void ) const { return m_DataVersionId; }

protected:
	IResourceNodeFactory():gfx::GLMemory(),m_DataVersionId(0){}
	u32				m_DataVersionId;
};

}}}}

#endif

