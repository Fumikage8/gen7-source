#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ShaderResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MeshResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_BlendShapeAnimationResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LocatorResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MotionConfigResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_DrawEnvResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LightSetResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CameraResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPackFactory.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

ResourceManager				*ResourceManager::s_Gp = NULL;

ResourceManager::ResourceManager() : gfx::GLMemory()
{
#define ADD_FACTORY( instance )																											  \
	{                                                                                   \
		IResourceNodeFactory*		ptr = reinterpret_cast<IResourceNodeFactory*>(instance);	\
		m_FactoryContaider.push_back( ptr );																							\
	}                                                                                   \

	ADD_FACTORY( new ShaderResourceNodeFactory() );
	ADD_FACTORY( new TextureResourceNodeFactory() );
	ADD_FACTORY( new MeshResourceNodeFactory() );
	ADD_FACTORY( new MaterialResourceNodeFactory() );
	ADD_FACTORY( new GfMdlResourceNodeFactory() );
	ADD_FACTORY( new AnimationResourceNodeFactory() );
	ADD_FACTORY( new BlendShapeAnimationResourceNodeFactory() );
	ADD_FACTORY( new LocatorResourceNodeFactory() );
	ADD_FACTORY( new MotionConfigResourceNodeFactory() );
	ADD_FACTORY( new DrawEnvResourceNodeFactory() );
	ADD_FACTORY( new LightSetResourceNodeFactory() );
	ADD_FACTORY( new CameraResourceNodeFactory() );
	ADD_FACTORY( new CollisionMeshResourceNodeFactory() );
	ADD_FACTORY( new GfBinaryMdlDataPackFactory() );
}

ResourceManager::~ResourceManager()
{
	clearResourceNodeFactory_();
}

ResourceNode* ResourceManager::createResourceNode_(resource::IResourceData* resourceData)
{
  u32       versionId = resourceData->GetVersionId();

	ResourceNode*				pTopNode = new ResourceNode();
  pTopNode->SetVersionId( versionId );

	SceneGraphManager::AddChild( pTopNode );
  createResourceNode_(resourceData, pTopNode);

	return pTopNode;
}

void ResourceManager::createResourceNode_(resource::IResourceData* resourceData, DagNode *pTopNode)
{
	u32				dataNum = resourceData->GetSize();
  u32       versionId = resourceData->GetVersionId();

	for( u32 dataIdx = 0; dataIdx < dataNum; ++dataIdx )
	{
		IResourceData::Type			type = resourceData->GetType( dataIdx );

		IResourceNodeFactory		*pIResourceNodeFactory = GetResourceNodeFactory(type);

		if ( pIResourceNodeFactory )
		{
			ResourceNode	*child;
			pIResourceNodeFactory->SetDataVersionId( versionId );
			child = pIResourceNodeFactory->CreateResourceNode( this->GetInstanceAllocator(), resourceData->GetData( dataIdx ), resourceData->GetDataSize( dataIdx ) );
			pTopNode->AddChild( child );
		}
	}
}

ResourceNode* ResourceManager::createResourceNodeIndex_(resource::IResourceData* resourceData, u32 index)
{
	u32				dataNum = resourceData->GetSize();
  u32       versionId = resourceData->GetVersionId();

  // 指定されたインデックスがデータ数より多ければエラー
  if (index >= dataNum)
  {
    GFL_ASSERT(index >= dataNum);
    return NULL;
  }

  IResourceData::Type			type = resourceData->GetType( index );

  IResourceNodeFactory		*pIResourceNodeFactory = GetResourceNodeFactory(type);

  if ( pIResourceNodeFactory )
  {
    pIResourceNodeFactory->SetDataVersionId( versionId );
    return pIResourceNodeFactory->CreateResourceNode( this->GetInstanceAllocator(), resourceData->GetData( index ), resourceData->GetDataSize( index ) );
  }

  return NULL;
}

void ResourceManager::addResourceNodeFactory_( IResourceNodeFactory* factory )
{
	m_FactoryContaider.push_back( factory );
}

void ResourceManager::clearResourceNodeFactory_()
{
	for( u32 i = 0; i < m_FactoryContaider.size(); ++i )
	{
		if ( m_FactoryContaider[i] )
			delete m_FactoryContaider[i];
	}
	m_FactoryContaider.clear();
}

}}}}

