#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>
#include <RenderingEngine/include/renderer/gfl2_CombinerShaderDriver.h>
#include <RenderingEngine/include/renderer/gfl2_CombinerCtrShaderDriver.h>
#include <debug/include/gfl2_Assert.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

/**
  @brief  ResourceNodeの検索
  @param  nodeList  ノードの配列（nameHash順にソート済みであること）
  @param  nodeNum   ノード数
  @param  nameHash  検索対象のハッシュ値
  @return 見つかったノード
*/
template<typename T>
static inline s32 FindResourceNode(T **nodeList, s32 nodeNum, u32 nameHash)
{
  s32 begin = 0;
  s32 end = nodeNum - 1;
  s32 index = (begin + end) / 2;

  while(begin < end)
  {
    u32 hash = nodeList[index]->GetNameHash();

    if(nameHash < hash)
    {
      end = index - 1;
    }
    else if(nameHash > hash)
    {
      begin = index + 1;
    }
    else
    {
      while(--index >= 0)
      {
        if(nodeList[index]->GetNameHash() != nameHash)
        {
          return index + 1;
        }
      }
      return 0;
    }

    index = (begin + end) / 2;
  }

  if(0 <= index && index < nodeNum
    && nodeList[index]->GetNameHash() == nameHash)
  {
    while(--index >= 0)
    {
      if(nodeList[index]->GetNameHash() != nameHash)
      {
        return index + 1;
      }
    }
    return 0;
  }
  return -1;
}

/**
  @brief  TextureResourceNodeの検索
  @param  nodeList  ノードの配列（nameHash順にソート済みであること）
  @param  nodeNum   ノード数
  @param  type      シェーダータイプ
  @param  nameHash  検索対象のハッシュ値
  @param  name      検索対象の名前
  @return 見つかったノード
*/
template<typename T>
static inline T *FindResourceNode(T **nodeList, u32 nodeNum, u32 nameHash, const c8 *name)
{
  s32 index = FindResourceNode(nodeList, nodeNum, nameHash);

  if(index < 0)
  {
    return NULL;
  }

  do
  {
    if(std::strcmp(nodeList[index]->GetName(), name) == 0)
    {
      return nodeList[index];
    }
    index++;

  } while(index < nodeNum && nodeList[index]->GetNameHash() == nameHash);

  return NULL;
}

/**
  @brief  ShaderResourceNodeの検索
  @param  nodeList  ノードの配列（nameHash順にソート済みであること）
  @param  nodeNum   ノード数
  @param  type      シェーダータイプ
  @param  nameHash  検索対象のハッシュ値
  @param  name      検索対象の名前
  @return 見つかったノード
*/
static inline const resource::ShaderResourceNode *FindResourceNode(const resource::ShaderResourceNode **nodeList, u32 nodeNum, gfx::ShaderType type, u32 nameHash, const c8 *name)
{
  s32 index = FindResourceNode(nodeList, nodeNum, nameHash);

  if(index < 0)
  {
    return NULL;
  }

  do
  {
    const void *shader = NULL;
    switch(type)
    {
    case gfx::ShaderType::Vertex:
      shader = nodeList[index]->GetVertexShader();
      break;
    case gfx::ShaderType::Pixel:
      shader = nodeList[index]->GetFragmentShader();
      break;
    case gfx::ShaderType::Geometry:
      shader = nodeList[index]->GetGeometryShader();
      break;
    }

    if(shader && std::strcmp(nodeList[index]->GetName(), name) == 0)
    {
      return nodeList[index];
    }
    index++;

  } while(index < nodeNum && nodeList[index]->GetNameHash() == nameHash);

  return NULL;
}


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  ModelInstanceNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MaterialInstanceNode::MaterialInstanceNode() : 
  NodeTrait<MaterialInstanceNode, InstanceNode>(),
  m_pTextureResourceNodeListOriginal(NULL),
  m_TextureResourceNodeListNumOriginal(0),
  m_pMaterialResourceNode(NULL),
  m_pVertexShaderResourceNode(NULL),
	m_pGeometryShaderResourceNode(NULL),
	m_pPixelShaderResourceNode(NULL),
  m_TextureNum(0),
  m_pShaderDriver(NULL),
  m_PsFogType(0)
{
  memset(m_pTextureResourceNodeList, 0, sizeof(const resource::TextureResourceNode*) * MATERIAL_TEXTURE_MAX);
  memset(m_TextureInfoList, 0, sizeof(m_TextureInfoList));

#if defined(GF_PLATFORM_CTR)
	for( u32 i = 0; i < resource::MaterialResourceNode::LUT_MAX; ++i )
		m_pLutCommand[i] = NULL;
#endif
}

MaterialInstanceNode::~MaterialInstanceNode()
{
	if ( m_pShaderDriver )
  {
		delete m_pShaderDriver;
  }
}

void MaterialInstanceNode::LinkData(const resource::MaterialResourceNode* pMaterial,
                                    const resource::TextureResourceNode** pTextures, u32 textureNum,
                                    const resource::ShaderResourceNode** pShaders, u32 shaderNum )
{
	m_pMaterialResourceNode = pMaterial;

  m_pTextureResourceNodeListOriginal = pTextures;
  m_TextureResourceNodeListNumOriginal = textureNum;

	m_TextureNum = 0;

  for (u32 texNo = 0; texNo < MATERIAL_TEXTURE_MAX; ++texNo)
  {
    m_pTextureResourceNodeList[texNo] = NULL;
  }

  u32 materialTextureNum = m_pMaterialResourceNode->GetTextureNum();
	for( u32 texNo = 0; texNo < materialTextureNum; ++texNo )
	{
		const resource::MaterialResourceNode::TextureInfo		&rInfo = m_pMaterialResourceNode->GetTextureInfo( texNo );
    GFL_ASSERT( rInfo.m_SlotNo < MATERIAL_TEXTURE_MAX );

    const resource::TextureResourceNode *node = FindResourceNode(pTextures, textureNum, rInfo.m_TextureNameHash, rInfo.m_TextureName);
    GFL_ASSERT_MSG( node, "%s not found", rInfo.m_TextureName  );

    m_pTextureResourceNodeList[rInfo.m_SlotNo] = node;
    m_TextureNum++;
	}

	if ( m_pMaterialResourceNode->GetVersionId() >= resource::GfModelVersion::OptimizeAndAddUserData )
	{
    u32 nameHash = m_pMaterialResourceNode->GetShaderNameHash(gfx::ShaderType::Vertex);
    const c8 *name =  m_pMaterialResourceNode->GetShaderName(gfx::ShaderType::Vertex);
    if(nameHash)
    {
      m_pVertexShaderResourceNode = FindResourceNode(pShaders, shaderNum, gfx::ShaderType::Vertex, nameHash, name);
    }
    
    nameHash = m_pMaterialResourceNode->GetShaderNameHash(gfx::ShaderType::Pixel);
    name =  m_pMaterialResourceNode->GetShaderName(gfx::ShaderType::Pixel);
    if(nameHash)
    {
      m_pPixelShaderResourceNode = FindResourceNode(pShaders, shaderNum, gfx::ShaderType::Pixel, nameHash, name);
    }
    
    nameHash = m_pMaterialResourceNode->GetShaderNameHash(gfx::ShaderType::Geometry);
    name =  m_pMaterialResourceNode->GetShaderName(gfx::ShaderType::Geometry);
    if(nameHash)
    {
      m_pGeometryShaderResourceNode = FindResourceNode(pShaders, shaderNum, gfx::ShaderType::Geometry, nameHash, name);
    }
	}
	else
	{
		u32 nameHash = m_pMaterialResourceNode->GetShaderNameHash();
		const c8 *name = m_pMaterialResourceNode->GetShaderName();

    if(nameHash)
    {
      m_pVertexShaderResourceNode = FindResourceNode(pShaders, shaderNum, gfx::ShaderType::Vertex, nameHash, name);
      m_pPixelShaderResourceNode = FindResourceNode(pShaders, shaderNum, gfx::ShaderType::Pixel, nameHash, name);
      m_pGeometryShaderResourceNode = FindResourceNode(pShaders, shaderNum, gfx::ShaderType::Geometry, nameHash, name);
    }

#if defined(GF_PLATFORM_CTR)
		GFL_ASSERT( m_pVertexShaderResourceNode );
#elif defined(GF_PLATFORM_WIN32)
		GFL_ASSERT( m_pVertexShaderResourceNode );
		GFL_ASSERT( m_pPixelShaderResourceNode );
#endif
	}

	{//@note 将来はシェーダーマネージャがドライバを管理し、受け渡す。
		
		const c8*			pShaderTypeName = m_pMaterialResourceNode->GetShaderTypeName();

		if ( std::strcmp( pShaderTypeName, "Comb" ) == 0 )
		{
    #if defined(GF_PLATFORM_WIN32)
      renderer::CombinerShaderDriver		*pShaderDriver = renderer::CombinerShaderDriver::Create( this->GetInstanceAllocator() );
			pShaderDriver->SetMaterialInstanceNode( this );
			pShaderDriver->SetVertexShaderResourceNode( m_pVertexShaderResourceNode );
			if ( m_pGeometryShaderResourceNode )
				pShaderDriver->SetGeometryShaderResourceNode( m_pGeometryShaderResourceNode );
			pShaderDriver->SetPixelShaderResourceNode( m_pPixelShaderResourceNode );
			m_pShaderDriver = pShaderDriver;
    #endif
		}
		else if ( std::strcmp( pShaderTypeName, "CombCtr" ) == 0 )
		{
			renderer::CombinerCtrShaderDriver		*pShaderDriver = renderer::CombinerCtrShaderDriver::Create( this->GetInstanceAllocator() );
			pShaderDriver->SetMaterialInstanceNode( this );
			pShaderDriver->SetVertexShaderResourceNode( m_pVertexShaderResourceNode );
			if ( m_pGeometryShaderResourceNode )
				pShaderDriver->SetGeometryShaderResourceNode( m_pGeometryShaderResourceNode );
			pShaderDriver->SetPixelShaderResourceNode( m_pPixelShaderResourceNode );
			m_pShaderDriver = pShaderDriver;
		}
	}

  GFL_ASSERT( m_pShaderDriver );
	GFL_ASSERT( m_TextureNum == m_pMaterialResourceNode->GetTextureNum() );


  // マテリアルリソースからコピー

#if !MATERIAL_INSTANCE_COPY_TEXTURE_INFO_ONLY
  // サンプラーステートオブジェクト
  for (u32 i = 0; i < m_pMaterialResourceNode->GetTextureNum(); ++i)
  {
    m_pSamplerStateObjectList[i] = GFGL::CreateSamplerStateObject();
    *m_pSamplerStateObjectList[i] = *m_pMaterialResourceNode->GetSamplerStateObject(i);
    m_pSamplerStateObjectList[i]->UpdateState();
  }

  // ブレンドステート
  m_pBlendStateObject = GFGL::CreateBlendStateObject();
  *m_pBlendStateObject = *m_pMaterialResourceNode->GetBlendStateObject();
  m_pBlendStateObject->UpdateState();

  // デプスステンシルステート
  m_pDepthStencilStateObject = GFGL::CreateDepthStencilStateObject();
  *m_pDepthStencilStateObject = *m_pMaterialResourceNode->GetDepthStencilStateObject();
  m_pDepthStencilStateObject->UpdateState();
#endif

  // テクスチャインフォ
  for (u32 i = 0; i < m_pMaterialResourceNode->GetTextureNum(); ++i)
  {
    GFL_ASSERT( i < MATERIAL_TEXTURE_MAX );
    m_TextureInfoList[i] = m_pMaterialResourceNode->GetTextureInfo(i);
  }

  // アトリビュートパラム
  m_AtrributeParam = m_pMaterialResourceNode->GetAttributeParam();
}


#if 0
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  MaterialLinkDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
ModelInstanceNode::MaterialLinkData::MaterialLinkData() :
	gfx::GLMemory(),
	m_pShaderResourceNode( NULL ),
	m_pShaderDriver( NULL )
{

}

ModelInstanceNode::MaterialLinkData::~MaterialLinkData()
{
}

#endif
}}}}

