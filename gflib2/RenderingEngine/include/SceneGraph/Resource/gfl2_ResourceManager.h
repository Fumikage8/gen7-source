#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_RESOURCEMANAGER_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_RESOURCEMANAGER_H_INCLUDED

//#include <iostream>
#include <memory>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class ResourceManager : public gfx::GLMemory
{
public:

	/*inline static void Initialize()
	{
    Initialize( gfx::GLMemory::GetAllocator() );
	}*/

  inline static void Initialize( gfx::IGLAllocator* pAllocator )
	{
    gfx::GLMemory::SetInstanceAllocator( pAllocator );
		if ( s_Gp == NULL )
			s_Gp = new ResourceManager();
    gfx::GLMemory::ClearInstanceAllocator();
	}

	inline static void Terminate()
	{
		if ( s_Gp )
			delete s_Gp;
	}

	/**
	* @brief リソースノードを生成しシーングラフにアタッチ
	* @param resource リソースデータのポインタ
	* @return			生成したノードのポインタ。リファレンスカウントは２になっている。
	*/ 
	/*static ResourceNode* CreateResourceNode(resource::IResourceData* resourceData)
	{
    return CreateResourceNode( gfx::GLMemory::GetAllocator(), resourceData );
	}*/
  static ResourceNode* CreateResourceNode(gfx::IGLAllocator* pAllocator, resource::IResourceData* resourceData)
	{
     gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
		GFL_ASSERT(resourceData);
    ResourceNode* pNode = s_Gp->createResourceNode_(resourceData);
     gfx::GLMemory::ClearInstanceAllocator(); 
    return pNode;
	}
  
	/**
	* @brief リソースノードを生成しシーングラフにアタッチ
	* @param resource リソースデータのポインタ
  * @param pTopNode トップノード。このノードの直下にデータが生成される。
	*/ 
  static void CreateResourceNode(gfx::IGLAllocator* pAllocator, resource::IResourceData* resourceData, DagNode *pTopNode)
  {
     gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
		GFL_ASSERT(resourceData);
    s_Gp->createResourceNode_(resourceData, pTopNode);
     gfx::GLMemory::ClearInstanceAllocator(); 
  }
  
	/**
	* @brief リソースデータのインデックスを指定してリソースノードを生成する
	* @param resource リソースデータのポインタ
  * @param index 生成するリソースデータのインデックス
	* @return	生成したノードのポインタ
	*/ 
  static ResourceNode* CreateResourceNodeIndex(gfx::IGLAllocator* pAllocator, resource::IResourceData* resourceData, u32 index)
  {
     gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
		GFL_ASSERT(resourceData);
    ResourceNode* pNode = s_Gp->createResourceNodeIndex_(resourceData, index);
     gfx::GLMemory::ClearInstanceAllocator(); 

     return pNode;
  }

	/**
	* @brief リソースノードを複製しシーングラフにアタッチ
	* @param resource リソースノードのポインタ
	* @return			生成したノードのポインタ。リファレンスカウントは２になっている。
	*/ 
	static ResourceNode* DuplicateResourceNode(ResourceNode* pSrc)
  {
    (void)pSrc;
  }

	/**
	* @brief シーングラフよりデタッチしリソースノードを削除。
	* @param resource リソースノードのポインタ
	* @return			リファレンスカウンタが２の場合、デッタチ(-1)しデクリメント(-1)して削除まてしてしまいます。そうでなければfalse。
	*/ 
	static b32 DeleteResourceNode(ResourceNode* pSrc)
  {
    (void)pSrc;
  }

	/**
		* @brief リソースノードファクトリを追加\n
		*        一致したtypeのリソースを発見したら登録したfactoryが呼ばれる
		*/
	static void AddResourceNodeFactory( resource::IResourceNodeFactory* factory )
	{
		GFL_ASSERT(factory);
		return s_Gp->addResourceNodeFactory_( factory );
	}

	/**
		* @brief リソースのタイプからリソースノードファクトリを取得
    *
    * @param type リソースタイプ
		*/
  static resource::IResourceNodeFactory* GetResourceNodeFactory(const resource::IResourceData::Type type)
  {
		for (u32 factryIdx = 0; factryIdx < s_Gp->m_FactoryContaider.size(); ++factryIdx)
    {
			if (s_Gp->m_FactoryContaider[factryIdx]->IsSupport(type))
			{
        return s_Gp->m_FactoryContaider[factryIdx];
			}
    }

    return NULL;
  }

	/**
		* @brief 登録したリソースノードファクトリを全てクリア
		*/
	static void ClearResourceNodeFactory()
	{
		s_Gp->clearResourceNodeFactory_();
	}

private:
	ResourceManager();
	~ResourceManager();

	static ResourceManager				*s_Gp;
	static const u32							FactoryContaiderMax = 32;

	util::FixedSizeContainer< IResourceNodeFactory*, FactoryContaiderMax >						m_FactoryContaider;
  
	ResourceNode* createResourceNode_(resource::IResourceData* resourceData);
	ResourceNode* createResourceNodeIndex_(resource::IResourceData* resourceData, u32 index);
	void createResourceNode_(resource::IResourceData* resourceData, DagNode *pTopNode);
	void addResourceNodeFactory_( resource::IResourceNodeFactory* factory );
	void clearResourceNodeFactory_();
};

}}}}

#endif

