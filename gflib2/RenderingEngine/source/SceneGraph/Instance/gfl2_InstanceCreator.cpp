#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ArrayResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MeshResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ShaderResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LightSetResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_DrawEnvResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CameraResourceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

ModelInstanceNode* InstanceCreator::CreateModelInstanceNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc)
{
  return CreateModelInstanceNode( pAllocator, pSrc, (ModelInstanceNode*)NULL );
}

ModelInstanceNode* InstanceCreator::CreateModelInstanceNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc, ModelInstanceNode* pJointReferenceModel)
{
	GFL_ASSERT(pSrc);
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

	ModelInstanceNode				*pModelInstanceNode = new ModelInstanceNode( pJointReferenceModel );

	InstanceCreator::SetupModelInstanceNode( pModelInstanceNode, pSrc );

	SceneGraphManager::AddChild( pModelInstanceNode );
  gfx::GLMemory::ClearInstanceAllocator();

	return pModelInstanceNode;
}


class ResourceVisitor : public GLMemory
{
public:
	ResourceVisitor():GLMemory(),
		m_MaterialResourceNum( 0 ),
		m_TextureResourceNum( 0 ),
		m_MeshResourceNum( 0 ),
		m_ShaderResourceNum( 0 ),
		m_pModelResourceNode( NULL )
	{
    // マテリアル, シェーダ, メッシュのリソースの総数は一致しているのでまとめて初期化
    for (u32 i = 0; i < MAX_RESOURCE_NUM; ++i)
    {
      m_pMaterialResourceNodeList[i] = NULL;
      m_pShaderResourceNodeList[i] = NULL;
      m_pMeshResourceNodeList[i] = NULL;
    }

    // テクスチャの総数は別定義なので別に初期化
    for (u32 i = 0; i < MAX_TEX_RESOURCE_NUM; ++i)
    {
      m_pTextureResourceNodeList[i] = NULL;
    }
	}

	virtual ~ResourceVisitor(){}

	void Visit( DagNode* node )
	{
		//::OutputDebugStringA( node->GetNodeName() );
		//::OutputDebugStringA( "\n" );

    if ( node->IsKindOf<resource::GfMdlResourceNode>() )
		{
			GFL_ASSERT( m_pModelResourceNode == NULL );//２つGfMdlResourceNodeがぶら下がっているのは想定外。ずっと後にそういったパターンもサポートするかも？
			m_pModelResourceNode = node->SafeCast<resource::GfMdlResourceNode>();
		}
		else if ( node->IsKindOf<resource::TextureResourceNode>() )
		{
      GFL_ASSERT(m_TextureResourceNum < MAX_TEX_RESOURCE_NUM);
      AddNode(m_pTextureResourceNodeList, node, m_TextureResourceNum);
		}
		else if ( node->IsKindOf<resource::MaterialResourceNode>() )
		{
      GFL_ASSERT(m_MaterialResourceNum < MAX_RESOURCE_NUM);
      m_pMaterialResourceNodeList[m_MaterialResourceNum] = node->SafeCast<resource::MaterialResourceNode>();
      m_MaterialResourceNum++;
		}
		else if ( node->IsKindOf<resource::MeshResourceNode>() )
		{
      GFL_ASSERT(m_MeshResourceNum < MAX_RESOURCE_NUM);
      m_pMeshResourceNodeList[m_MeshResourceNum] = node->SafeCast<resource::MeshResourceNode>();
      m_MeshResourceNum++;
		}
		else if ( node->IsKindOf<resource::ShaderResourceNode>() )
		{
      GFL_ASSERT(m_ShaderResourceNum < MAX_RESOURCE_NUM);
      AddNode(m_pShaderResourceNodeList, node, m_ShaderResourceNum);
		}
  }

  template<typename T>
  void AddNode(T **nodeList, DagNode *node, u32 &resourceNum)
  {
    /*
      ソートしながら追加している
      最後にまとめて適切なアルゴリズムでソートする方が早いかも？
    */
    T *ptr = node->SafeCast<T>();
    u32 hash = ptr->GetNameHash();

    for(u32 i=0;i<resourceNum;i++)
    {
      if(hash < nodeList[i]->GetNameHash())
      {
        for(u32 j=resourceNum;j>i;j--)
        {
          nodeList[j] = nodeList[j-1];
        }
        nodeList[i] = ptr;
        resourceNum++;
        return;
      }
    }

    nodeList[resourceNum] = ptr;
    resourceNum++;
  }

  static const s32 MAX_RESOURCE_NUM = 256;
  static const s32 MAX_TEX_RESOURCE_NUM = 256;

	u32															m_MaterialResourceNum;
	u32															m_TextureResourceNum;
	u32															m_MeshResourceNum;
	u32															m_ShaderResourceNum;

	const resource::GfMdlResourceNode			*m_pModelResourceNode;
	const resource::MaterialResourceNode	*m_pMaterialResourceNodeList[MAX_RESOURCE_NUM];
	const resource::TextureResourceNode		*m_pTextureResourceNodeList[MAX_TEX_RESOURCE_NUM];
	const resource::ShaderResourceNode		*m_pShaderResourceNodeList[MAX_RESOURCE_NUM];
	const resource::MeshResourceNode			*m_pMeshResourceNodeList[MAX_RESOURCE_NUM];
};

void InstanceCreator::SetupModelInstanceNode( ModelInstanceNode *pModelInstanceNode, resource::ResourceNode* pSrc )
{
	Traverser::Visitor< DagNode, ResourceVisitor>					resourceVisitor;
  Traverser::Traverse<DagNode,ResourceVisitor>( pSrc, &resourceVisitor, 0xFFFFFFFF );

	if ( resourceVisitor.m_ShaderResourceNum )
    pModelInstanceNode->SetShaderResourceNode( resourceVisitor.m_pShaderResourceNodeList, resourceVisitor.m_ShaderResourceNum );
	if ( resourceVisitor.m_TextureResourceNum )
		pModelInstanceNode->SetTextureResourceNode( resourceVisitor.m_pTextureResourceNodeList, resourceVisitor.m_TextureResourceNum );
	if ( resourceVisitor.m_MeshResourceNum )
    pModelInstanceNode->SetMeshResourceNode( resourceVisitor.m_pMeshResourceNodeList, resourceVisitor.m_MeshResourceNum );
	if ( resourceVisitor.m_MaterialResourceNum )
    pModelInstanceNode->SetMaterialResourceNode( resourceVisitor.m_pMaterialResourceNodeList, resourceVisitor.m_MaterialResourceNum );
	pModelInstanceNode->SetGfMdlResourceNode( resourceVisitor.m_pModelResourceNode );

	pModelInstanceNode->LinkNode();
}

void InstanceCreator::SetupModelInstanceNode( ModelInstanceNode *pNode, resource::ResourceNode* pSrc, DagNode* pTextureShaderNodeRoot )
{
  resource::ArrayResourceNode *arrayTextureNode = pTextureShaderNodeRoot->SafeCast<resource::ArrayResourceNode>();
  if(arrayTextureNode && arrayTextureNode->IsFreezed())
  {
    {
      u32 nodeCount;
      const resource::TextureResourceNode **nodeList = arrayTextureNode->GetResourceNode<resource::TextureResourceNode>(&nodeCount);
      if(nodeCount > 0)
      {
        pNode->SetTextureResourceNode(nodeList, nodeCount);
      }
    }
    {
      u32 nodeCount;
      const resource::ShaderResourceNode **nodeList = arrayTextureNode->GetResourceNode<resource::ShaderResourceNode>(&nodeCount);
      if(nodeCount > 0)
      {
        pNode->SetShaderResourceNode(nodeList, nodeCount);
      }
    }
  }
  else
  {
    Traverser::Visitor< DagNode, ResourceVisitor>					textureShaderResourceVisitor;

    {//必要なリソースのみを収集
      Traverser::Traverse<DagNode,ResourceVisitor>( pTextureShaderNodeRoot, &textureShaderResourceVisitor, 0xFFFFFFFF );
    }

    
    pNode->SetShaderResourceNode( textureShaderResourceVisitor.m_pShaderResourceNodeList, textureShaderResourceVisitor.m_ShaderResourceNum );
    if( textureShaderResourceVisitor.m_TextureResourceNum )
    {
      pNode->SetTextureResourceNode( textureShaderResourceVisitor.m_pTextureResourceNodeList, textureShaderResourceVisitor.m_TextureResourceNum );
    }
  }

  Traverser::Visitor< DagNode, ResourceVisitor>					resourceVisitor;

  {//必要なリソースのみを収集
    Traverser::Traverse<DagNode,ResourceVisitor>( pSrc, &resourceVisitor, 0xFFFFFFFF );
  }

  pNode->SetMeshResourceNode( resourceVisitor.m_pMeshResourceNodeList, resourceVisitor.m_MeshResourceNum );
  pNode->SetMaterialResourceNode( resourceVisitor.m_pMaterialResourceNodeList, resourceVisitor.m_MaterialResourceNum );
  pNode->SetGfMdlResourceNode( resourceVisitor.m_pModelResourceNode );
  pNode->LinkNode();
}

//LocatorResourceNode用ビジターを作成
class LocatorInstanceNodeNodeVisitor
{
public:
	LocatorInstanceNodeNodeVisitor():m_pLocatorInstanceNodeTop( NULL ){}
	virtual ~LocatorInstanceNodeNodeVisitor() {}

	inline void Visit( gfl2::renderingengine::scenegraph::resource::LocatorResourceNode* pNode )
	{
		LocatorInstanceNode		*pInstanceNode = new LocatorInstanceNode();
		pInstanceNode->SetResourceNode( pNode );

		if ( m_pLocatorInstanceNodeTop )
			m_pLocatorInstanceNodeTop->AddChild( pInstanceNode );
		else
			m_pLocatorInstanceNodeTop = pInstanceNode;
	}

	LocatorInstanceNode					*m_pLocatorInstanceNodeTop;
};

LocatorInstanceNode* InstanceCreator::CreateLocatorInstanceNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc)
{
	GFL_ASSERT(pSrc);
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

	scenegraph::Traverser::Visitor<scenegraph::resource::LocatorResourceNode, LocatorInstanceNodeNodeVisitor>				visitor;
	scenegraph::Traverser::Traverse<scenegraph::resource::LocatorResourceNode, LocatorInstanceNodeNodeVisitor>( pSrc, &visitor );
  gfx::GLMemory::ClearInstanceAllocator();

	return visitor.m_pLocatorInstanceNodeTop;
}

LocatorInstanceNode* InstanceCreator::CreateLocatorInstanceNode( gfx::IGLAllocator* pAllocator )
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
  LocatorInstanceNode* instance = new LocatorInstanceNode();
  gfx::GLMemory::ClearInstanceAllocator();
  return instance;
}

ViewerLocatorInstanceNode* InstanceCreator::CreateViewerLocatorInstanceNode(gfx::IGLAllocator* pAllocator, const c8* nodeName, const c8* parentName)
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
  ViewerLocatorInstanceNode* instance = new ViewerLocatorInstanceNode(nodeName, parentName);
  gfx::GLMemory::ClearInstanceAllocator();
  return instance;
}

LightSetNode* InstanceCreator::CreateLightSetNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc)
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

	if ( pSrc == NULL )
  {
		LightSetNode* instance = new LightSetNode();
    gfx::GLMemory::ClearInstanceAllocator();
    return instance;
  }

	LightSetNode												*pLightSetNode = NULL;
	resource::LightSetResourceNode			*pLightSetResourceNode = pSrc->SafeCast<resource::LightSetResourceNode>();

	GFL_ASSERT( pLightSetResourceNode );
	if ( pLightSetResourceNode == NULL )
  {
    gfx::GLMemory::ClearInstanceAllocator();
		return pLightSetNode;
  }

	pLightSetNode = new LightSetNode();

	pLightSetNode->SetSetNo( pLightSetResourceNode->GetSetNo() );

	for( u32 i = 0; i < pLightSetResourceNode->GetLightNum(); ++i )
	{
		const resource::LightSetResourceNode::LightData			*pLightData = pLightSetResourceNode->GetLightData( i );
		LightNode																						*pLightNode = new LightNode();

		pLightNode->SetNodeName( pLightData->m_Name );
		pLightNode->SetNodeNameHash( pLightData->m_NameHash );
		pLightNode->SetLightType( pLightData->m_Type );
		pLightNode->SetPosition( math::Vector3(pLightData->m_Position.x, pLightData->m_Position.y, pLightData->m_Position.z) );
		pLightNode->SetDirection( math::Vector3(pLightData->m_Direction.x, pLightData->m_Direction.y, pLightData->m_Direction.z) );
		pLightNode->SetColor( math::Vector4(pLightData->m_Color.x, pLightData->m_Color.y, pLightData->m_Color.z, pLightData->m_Color.w) );
		pLightNode->SetIntensity( pLightData->m_Intensity );
		pLightNode->SetConeAngle( pLightData->m_ConeAngle );

		pLightSetNode->AddLight( pLightNode );
	}

  gfx::GLMemory::ClearInstanceAllocator();
	return pLightSetNode;
}

LightNode* InstanceCreator::CreateLightNode( gfx::IGLAllocator* pAllocator )
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
	LightNode* instance = new LightNode();
  gfx::GLMemory::ClearInstanceAllocator(); 
  return instance;
}

CameraNode* InstanceCreator::CreateCameraNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc)
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

	if ( pSrc == NULL )
  {
		CameraNode* instance =  new CameraNode();
    gfx::GLMemory::ClearInstanceAllocator();
    return instance;
  }

	CameraNode												*pCameraNode = NULL;
	resource::CameraResourceNode			*pCameraResourceNode = pSrc->SafeCast<resource::CameraResourceNode>();

	GFL_ASSERT( pCameraResourceNode );
	if ( pCameraResourceNode == NULL )
  {
    gfx::GLMemory::ClearInstanceAllocator();

		return pCameraNode;
  }

	pCameraNode = new CameraNode();
  pCameraNode->SetNodeName(pCameraResourceNode->GetNodeName());

	const resource::CameraResourceNode::CameraData			*pData = pCameraResourceNode->GetCameraData();

  math::Matrix44 tmp = *(math::Matrix44*)&pData->m_LocalMatrix;
  tmp = tmp.Transpose();
  tmp.Normalize();
	math::Matrix34 matrix = *(math::Matrix34*)&tmp;

  pCameraNode->SetLocalScale(matrix.GetElemScaling());
  math::Quaternion rot;
  rot.MatrixToQuaternion(matrix);
  pCameraNode->SetLocalRotation(rot);
  pCameraNode->SetLocalTranslate(matrix.GetElemPosition());

	pCameraNode->SetNear( pData->m_Near );
	pCameraNode->SetFar( pData->m_Far );
	pCameraNode->SetFovy( pData->m_Fovy );
	pCameraNode->SetAspectRatio( pData->m_AspectRatio );

    
  gfx::GLMemory::ClearInstanceAllocator();

	return pCameraNode;
}

//DrawEnvResourceNode用ビジターを作成
class DrawEnvResourceNodeVisitor
{
public:
	DrawEnvResourceNodeVisitor():m_pDrawEnvResourceNode( NULL ){}
	virtual ~DrawEnvResourceNodeVisitor() {}

	inline void Visit( gfl2::renderingengine::scenegraph::resource::DrawEnvResourceNode* pNode )
	{
		GFL_ASSERT( m_pDrawEnvResourceNode == NULL );//複数収集には対応していない。
		m_pDrawEnvResourceNode = pNode;
	}

	scenegraph::resource::DrawEnvResourceNode					*m_pDrawEnvResourceNode;
};

//LightSetResourceNode用ビジターを作成
class LightSetResourceNodeVisitor
{
public:
  LightSetResourceNodeVisitor() : 
    m_pDrawEnvNode(NULL),
    m_pAllocator(NULL)
  {
  }
  virtual ~LightSetResourceNodeVisitor()
  {
  }

	inline void Visit( gfl2::renderingengine::scenegraph::resource::LightSetResourceNode* pNode )
	{
		LightSetNode	*pLightSetNode = InstanceCreator::CreateLightSetNode( m_pAllocator, pNode );
		m_pDrawEnvNode->AddLightSet( pLightSetNode );
	}

	DrawEnvNode			*m_pDrawEnvNode;
  gfx::IGLAllocator   *m_pAllocator;
};

//CameraResourceNode用ビジターを作成
class CameraResourceNodeVisitor
{
public:
	CameraResourceNodeVisitor(){}
	virtual ~CameraResourceNodeVisitor() {}

	inline void Visit( gfl2::renderingengine::scenegraph::resource::CameraResourceNode* pNode )
	{
		CameraNode	*pCameraNode = InstanceCreator::CreateCameraNode( m_pAllocator, pNode );
		m_pDrawEnvNode->AddCamera( pCameraNode );
	}

	DrawEnvNode			*m_pDrawEnvNode;
  gfx::IGLAllocator   *m_pAllocator;
};

DrawEnvNode* InstanceCreator::CreateDrawEnvNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc)
{
	DrawEnvNode					*pDrawEnvNode = NULL;
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

	if ( pSrc == NULL )
	{
		pDrawEnvNode = new DrawEnvNode();
		SceneGraphManager::AddChild( pDrawEnvNode );
    gfx::GLMemory::ClearInstanceAllocator();
		return pDrawEnvNode;
	}

	GFL_ASSERT(pSrc);

	{
		scenegraph::Traverser::Visitor<scenegraph::resource::DrawEnvResourceNode, DrawEnvResourceNodeVisitor>				visitor;
		scenegraph::Traverser::Traverse<scenegraph::resource::DrawEnvResourceNode, DrawEnvResourceNodeVisitor>( pSrc, &visitor );

		GFL_ASSERT( visitor.m_pDrawEnvResourceNode );
		if ( visitor.m_pDrawEnvResourceNode == NULL )
    {
      gfx::GLMemory::ClearInstanceAllocator();
			return pDrawEnvNode;
    }

		pDrawEnvNode = new scenegraph::instance::DrawEnvNode();
		pDrawEnvNode->ReadDrawEnvResourceNode( *(visitor.m_pDrawEnvResourceNode) );
	}

	{
		scenegraph::Traverser::Visitor<scenegraph::resource::LightSetResourceNode, LightSetResourceNodeVisitor>				visitor;
		visitor.m_pDrawEnvNode = pDrawEnvNode;
    visitor.m_pAllocator = pAllocator;
		scenegraph::Traverser::Traverse<scenegraph::resource::LightSetResourceNode, LightSetResourceNodeVisitor>( pSrc, &visitor );
	}

	{
		scenegraph::Traverser::Visitor<scenegraph::resource::CameraResourceNode, CameraResourceNodeVisitor>				visitor;
		visitor.m_pDrawEnvNode = pDrawEnvNode;
    visitor.m_pAllocator = pAllocator;
		scenegraph::Traverser::Traverse<scenegraph::resource::CameraResourceNode, CameraResourceNodeVisitor>( pSrc, &visitor );
	}

	SceneGraphManager::AddChild( pDrawEnvNode );

  gfx::GLMemory::ClearInstanceAllocator();
	return pDrawEnvNode;
}

MotionConfigInstanceNode* InstanceCreator::CreateMotionConfigInstanceNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc)
{
  GFL_ASSERT(pSrc);
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

  MotionConfigInstanceNode* motionConfigNode = new MotionConfigInstanceNode();
  resource::MotionConfigResourceNode* pMotionConfigResourceNode = pSrc->SafeCast<resource::MotionConfigResourceNode>();
  if (pMotionConfigResourceNode == NULL)
  {
    // 子供が持っている可能性
    pMotionConfigResourceNode = pSrc->GetChild()->SafeCast<resource::MotionConfigResourceNode>();
  }
  motionConfigNode->SetMotionConfigData(pMotionConfigResourceNode);
  gfx::GLMemory::ClearInstanceAllocator();

  return motionConfigNode;
}

/**
* @brief ResourceNodeから、ModelInstanceNodeを作成(テクスチャとシェーダーを別に渡せる版)
* @param pModelSrc           モデルリソースノードのポインタ
* @param ppTextureSrcTable   テクスチャリソースノード配列のポインタ 
* @pamra textureDataCount    テクスチャリソース数
* @param ppShaderSrcTable   シェーダーリソースノード配列のポインタ 
* @pamra shaderDataCount    シェーダーリソース数
*
* @return			生成したノードのポインタ。
*/ 
ModelInstanceNode* InstanceCreator::CreateModelInstanceNode(
  gfx::IGLAllocator* pAllocator,
  resource::ResourceNode* pSrc,
  DagNode* pTextureShaderNodeRoot
  )
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

  ModelInstanceNode				*pModelInstanceNode = new ModelInstanceNode();

  SetupModelInstanceNode(pModelInstanceNode, pSrc, pTextureShaderNodeRoot);

  SceneGraphManager::AddChild( pModelInstanceNode );

  gfx::GLMemory::ClearInstanceAllocator(); 

  return pModelInstanceNode;
}
	
/**
* @brief EffectNodeLocatorInstanceNodeを作成
* @param parentName 親の名前
* @return			生成したノードのポインタ。
*/ 
EffectNodeLocatorInstanceNode* InstanceCreator::CreateEffectNodeLocatorInstanceNode(gfx::IGLAllocator* pAllocator, const c8* parentName)
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
  EffectNodeLocatorInstanceNode* instance = new EffectNodeLocatorInstanceNode(parentName);
  gfx::GLMemory::ClearInstanceAllocator();
  return instance;
}

}}}}

