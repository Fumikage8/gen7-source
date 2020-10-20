﻿#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_LocatorRESOURCENODEFAACTRY_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_LocatorRESOURCENODEFAACTRY_H_INCLUDED

//#include <iostream>
#include <memory>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceNodeFactory.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/**
	* @brief ジョイントアニメーションリソースノードのファクトリインターフェース
	*/
class LocatorResourceNodeFactory : public IResourceNodeFactory
{
public:

  struct LocatorDataHeader{
		u32				locatorNum;
		u32				padding[3];
	};

	LocatorResourceNodeFactory():IResourceNodeFactory(){}
	virtual ~LocatorResourceNodeFactory() {}

	/**
		* @brief ResourceNodeのファクトリメソッド　所有権付きのリソースノードを返す
		*/
	//virtual ResourceNode* CreateResourceNode(const void* data, int size);
	virtual ResourceNode* CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size);

	virtual b32 IsSupport( IResourceData::Type type );
};

}}}}

#endif

