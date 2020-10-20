#include "../include/FieldTestMyRenderingPipeLine.h"
#include <RenderingEngine/include/renderer/gfl2_CombinerCtrShaderDriver.h>
#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>

#include "test/FieldTest/include/debug/FieldDebugTypes.h"

// RTTIデバッグシステム
#include "../include/RttiDebug/RttiEditor.h"
#include "../include/RttiDebug/RttiManager.h"

#if defined(GF_PLATFORM_CTR)
#include <nn/fs.h>
#include <nn/hio.h>
#endif

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )

struct LocalFunc{
	static void *ReadFile( const char* pFilePath )
  {//自前データロード
#if defined(GF_PLATFORM_CTR)
    nn::fs::FileReader file( pFilePath );
    u32 fileSize = file.GetSize();

    if( fileSize == 0 )
    {
      GFL_PRINT("Load Error : %s \n", pFilePath );
    }


    void* pShaderBinary = GLMemory::Malloc(fileSize, 128);
    file.Read(pShaderBinary, fileSize);
		return pShaderBinary;
#else
		FILE		*hFile;
		int			file_size;
		char		*read_buff;

		fopen_s( &hFile, pFilePath, "rb" );

		if ( hFile == NULL ){
			assert(0);
			return NULL;
		}

		fseek( hFile, 0, SEEK_END );
		file_size = ftell( hFile );
		fseek( hFile, 0, SEEK_SET );

		u32		align = 16;
		file_size += align;//必ずNULLが入るようにしたい。
		u32 buf_size = file_size + ((-file_size) & (align-1));

		read_buff = reinterpret_cast<c8*>( GLMemory::Malloc(buf_size) );
		memset( read_buff, 0, buf_size );
		fread( read_buff, file_size, 1, hFile );
		fclose( hFile );

		return read_buff;
#endif
  }
};

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
EdgeMapSceneRenderPath::EdgeMapShaderDriver::EdgeMapShaderDriver() : GLMemory(), IShaderDriver()
{
#if REMOVAL_OF_MEMORY_LEAK
  for( u32 i=0 ;i<BIN_DATA_CNT ; i++ )
  {
    m_pBinaryData[i] = NULL;
  }
  s32 binaryDataIndex = 0;
#endif

	{
		scenegraph::resource::GfBinaryMdlData							resourceData;

		s32		size;
#if defined(GF_PLATFORM_CTR)
		c8		*pBinData = (c8*)LocalFunc::ReadFile( "rom:/data/test/field_test/EdgeMapVColAShader/EdgeMapVColA.ctr.gfbmdl" );
#else
		c8		*pBinData = (c8*)LocalFunc::ReadFile( "R:/home/niji/exec/data_win32/data/test/field_test/EdgeMapVColAShader/exports/EdgeMapVColA.wingl.gfbmdl" );
#endif
			
		resourceData.SetModelData( pBinData );

		m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );	

#if REMOVAL_OF_MEMORY_LEAK
      m_pBinaryData[binaryDataIndex] = pBinData;
      binaryDataIndex++;
#endif

	}

	{
		static const char* pShaderFileList[] = {
#if defined(GF_PLATFORM_CTR)
			"rom:/data/test/field_test/EdgeMapVColAShader/Shaders/Default.ctr.gfbvsh",
			"rom:/data/test/field_test/EdgeMapVColAShader/Shaders/NormalToUv.ctr.gfbvsh",
			"rom:/data/test/field_test/EdgeMapVColAShader/Shaders/Comb2_Manual.ctr.gfbfsh",
#else
			"R:/home/niji/exec/data_win32/data/test/field_test/EdgeMapVColAShader/exports/Shader/Comb2_Manual.wingl.gfvsh",
			"R:/home/niji/exec/data_win32/data/test/field_test/EdgeMapVColAShader/exports/Shader/Comb2_Manual.wingl.gffsh",
#endif
		};

		u32				shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);

		for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
		{
			scenegraph::resource::GfBinaryShdData							resourceData;

			s32		size;
			c8		*pBinData = (c8*)LocalFunc::ReadFile( pShaderFileList[shaderNo] );

			resourceData.SetShaderData( pBinData );

			scenegraph::resource::ResourceNode		*pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
			m_pBinModelRootNode->AddChild( pBinShdNode );

			//GLMemory::Free( pBinData );

#if REMOVAL_OF_MEMORY_LEAK
      m_pBinaryData[binaryDataIndex] = pBinData;
      binaryDataIndex++;
#endif

		}
	}

	{
		static const c8*			pTextureFileList[] = {
#if defined(GF_PLATFORM_CTR)
			"rom:/data/test/field_test/EdgeMapVColAShader/Textures/NormalTexture.ctr.btex",
#else
			"R:/home/niji/exec/data_win32/data/test/field_test/EdgeMapVColAShader/exports/Texture/NormalTexture.wingl.tga",
#endif
		};

		u32				texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);

		for( u32 texNo = 0; texNo < texNum; ++texNo )
		{
			scenegraph::resource::GfBinaryTexData							texResourceData;
			scenegraph::resource::ResourceNode								*pBinTexRootNode;

			s32		texsize;
			s32		size;
			c8		*pTexBinData = (c8*)LocalFunc::ReadFile( pTextureFileList[texNo] );
			
			texResourceData.SetTextureData( pTexBinData );

			//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
			pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
			m_pBinModelRootNode->AddChild( pBinTexRootNode );

			//GLMemory::Free( pTexBinData );

#if REMOVAL_OF_MEMORY_LEAK
      m_pBinaryData[binaryDataIndex] = pTexBinData;
      binaryDataIndex++;
#endif

		}
	}

	{
		m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pBinModelRootNode );
	}

	{
		scenegraph::instance::DrawableNode::UpdateDrawTagInfo				info;

		m_pModelInstanceNode->UpdateDrawTag( info );
		scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode );
	}

#if defined GF_PLATFORM_WIN32

#if !REMOVAL_OF_MEMORY_LEAK

	{
		ConstantBufferElement element[] = {
			{ ConstantType::Vector, 1 },
			{ ConstantType::Vector, 1 },
			{ ConstantType::Vector, 1 },
			ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
		};

		m_pConstantBuffer = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
	}

	for( u32 i = 0; i < 3; ++i )
	{
		m_pBlendStateObject[i] = GFGL::CreateBlendStateObject();
		m_pBlendStateObject[i]->SetBlendEnable(false);

		switch( i ){
		case 0:
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 0, BlendStateObject::COLOR_WRITE_ENABLE_ALL );
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 1, BlendStateObject::COLOR_WRITE_ENABLE_ALL );
			break;
		case 1://法泉エッジのみ
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 0, BlendStateObject::COLOR_WRITE_ENABLE_ALL );
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 1, 0 );
			break;
		case 2://ＩＤエッジのみ
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 0, 0 );
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 1, BlendStateObject::COLOR_WRITE_ENABLE_ALL );
			break;
		}

		m_pBlendStateObject[i]->UpdateState();
	}

#endif

	for( u32 i = 0; i < 3; ++i )
	{
		m_pBlendStateObject[i] = GFGL::CreateBlendStateObject();
		m_pBlendStateObject[i]->SetBlendEnable(false);

		switch( i ){
		case 0:
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 0, BlendStateObject::COLOR_WRITE_ENABLE_ALL );
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 1, BlendStateObject::COLOR_WRITE_ENABLE_ALL );
			break;
		case 1://法泉エッジのみ
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 0, BlendStateObject::COLOR_WRITE_ENABLE_ALL );
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 1, 0 );
			break;
		case 2://ＩＤエッジのみ
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 0, 0 );
			m_pBlendStateObject[i]->SetRenderTargetWriteMask( 1, BlendStateObject::COLOR_WRITE_ENABLE_ALL );
			break;
		}

		m_pBlendStateObject[i]->UpdateState();
	}

	{
		ConstantBufferElement element[] = {
			{ ConstantType::Vector, 1 },
			ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
		};

		m_pConstantBuffer = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
	}
#endif
}

EdgeMapSceneRenderPath::EdgeMapShaderDriver::~EdgeMapShaderDriver()
{
	GFX_SAFE_DELETE( m_pModelInstanceNode );
	GFX_SAFE_DELETE( m_pBinModelRootNode );

#if REMOVAL_OF_MEMORY_LEAK

#if defined GF_PLATFORM_WIN32
  GFL_SAFE_DELETE( m_pConstantBuffer );
#endif

  for( u32 i = 0; i < 3; ++i )
	{
    GFL_SAFE_DELETE( m_pBlendStateObject[i] );
  }
  for( u32 i=0 ;i<BIN_DATA_CNT ; i++ )
  {
    GLMemory::Free( m_pBinaryData[i] );
  }
#endif

}

b32 EdgeMapSceneRenderPath::EdgeMapShaderDriver::Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
	MeshDrawTag						*pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );
	const IShaderDriver		*pShaderDriver = pTag->GetShaderDriver();

	scenegraph::resource::MaterialResourceNode										*pMaterial = const_cast<scenegraph::resource::MaterialResourceNode*>( pMeshDrawTag->GetMaterialInstanceNode()->GetMaterialResourceNode() );
	//scenegraph::resource::MaterialResourceNode::AttributeParam		*pAttributeParam = pMaterial->GetAttributeParam();
	const scenegraph::resource::MaterialResourceNode::UserData		&rUserData = pMaterial->GetUserData();

#if defined GF_PLATFORM_WIN32
	if ( rUserData.m_IDEdgeEnable == false && rUserData.m_EdgeType == scenegraph::resource::EdgeType::None )
		return false;
#else
	if ( rUserData.m_EdgeType == scenegraph::resource::EdgeType::None )
		return false;
#endif

	scenegraph::resource::MaterialResourceNode::AttributeParam			*pAttr = m_pModelInstanceNode->GetMaterialInstanceNode( 0 )->GetAttributeParam();

	switch( rUserData.m_EdgeType ){
	case scenegraph::resource::EdgeType::None:
		break;
	case scenegraph::resource::EdgeType::Erase:
		pAttr->m_ConstantColor[1].w = 0.0f;//法線出力オフ
		pAttr->m_ConstantColor[2].w = 0.0f;//頂点カラー出力オフ
		break;
	case scenegraph::resource::EdgeType::VColor:
		pAttr->m_ConstantColor[1].w = 0.0f;//法線出力オフ
		pAttr->m_ConstantColor[2].w = 1.0f;//頂点カラー出力オン
		//return false;
		break;
	default:
		pAttr->m_ConstantColor[1].w = 1.0f;//法線出力オン
		pAttr->m_ConstantColor[2].w = 1.0f;//頂点カラー出力オン
		break;
	}

	pShaderDriver->Apply( rRenderState, pMeshDrawTag );

#if defined GF_PLATFORM_WIN32
	u32					maskType = 0;

	if ( rUserData.m_EdgeType == scenegraph::resource::EdgeType::None )			maskType = 2;//ＩＤエッジのみ
	if ( rUserData.m_IDEdgeEnable == false)																	maskType = 1;//法泉エッジのみ

	math::Vector				psVectorConstants;

	psVectorConstants.x = static_cast<f32>( rUserData.m_EdgeID );
	psVectorConstants.x /= 255.0f;

	m_pConstantBuffer->Update(&psVectorConstants, sizeof(psVectorConstants));
	GFGL::SetPixelShaderConstant( 6, m_pConstantBuffer );
	GFGL::SetBlendState( m_pBlendStateObject[maskType] );
#else

	{//シェーダーアニメ
		nn::gr::CommandBufferJumpHelper																				*pCommandBuffer = GFGL::GetJumpHelper();
		bit32									*cmd = pCommandBuffer->GetCurrentPtr();
		u8										constColorR, constColorG, constColorB, constColorA;
		static const u32			regNo[6] = { PICA_REG_TEX_ENV0_COLOR, PICA_REG_TEX_ENV1_COLOR, PICA_REG_TEX_ENV2_COLOR,
																			 PICA_REG_TEX_ENV3_COLOR, PICA_REG_TEX_ENV4_COLOR, PICA_REG_TEX_ENV5_COLOR, };

		for( u32 i = 1; i <= 2; ++i )
		{
			s16										index = pAttr->m_TextureCombinerConstant[i];
			const math::Vector4		&rColor = pAttr->m_ConstantColor[index];

			constColorR = static_cast<u8>(rColor.x * 255.0f);
			constColorG = static_cast<u8>(rColor.y * 255.0f);
			constColorB = static_cast<u8>(rColor.z * 255.0f);
			constColorA = static_cast<u8>(rColor.w * 255.0f);

			*cmd++ = PICA_CMD_DATA_TEX_ENV_CONST( constColorR, constColorG, constColorB, constColorA );
			*cmd++ = PICA_CMD_HEADER_SINGLE( regNo[i] );
		}

		pCommandBuffer->Put(cmd);
	}

#endif

	return true;
}

void EdgeMapSceneRenderPath::EdgeMapShaderDriver::SetTextureOverride( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
	MeshDrawTag											*pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );
	const scenegraph::resource::MaterialResourceNode			*pMaterialResourceNode = m_pModelInstanceNode->GetMaterialInstanceNode( 0 )->GetMaterialResourceNode();
	u32																										dataVersionId = pMaterialResourceNode->GetVersionId();
	RenderState::ShaderStateMap														shaderStateMap = rRenderState.GetShaderStateMap();

	{
		const gfx::Texture							*pTexture = pTag->GetTexture( 0 );
		const gfx::SamplerStateObject		*pSampler =  pTag->GetSampler( 0 );

#if defined(GF_PLATFORM_CTR)
		{
			u32                   textureCommand[8];

			const gfl2::gfx::ctr::CTRTexture					*pCTRTexture = reinterpret_cast<const gfl2::gfx::ctr::CTRTexture*>(pTexture);
			const gfl2::gfx::Texture::TextureDesc			&rTextureDesc = pCTRTexture->GetTextureDesc();

			{
				const scenegraph::resource::MaterialResourceNode::TextureInfo		&rTexInfo = pMaterialResourceNode->GetTextureInfo( 0 );

				textureCommand[0] = PICA_CMD_DATA_TEXTURE_WRAP_FILTER(	rTexInfo.m_Attribute.m_MagFilter, rTexInfo.m_Attribute.m_MinFilter,
																																pCTRTexture->GetFormat(),
																																rTexInfo.m_Attribute.m_RepeatV, rTexInfo.m_Attribute.m_RepeatU,
																																0, PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D );
					
        textureCommand[1] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_WRAP_FILTER );
			}
				
			//デフォルトではTexture0に設定するコマンド
			// 0x082
			textureCommand[2] = PICA_CMD_DATA_TEXTURE_SIZE( rTextureDesc.width, rTextureDesc.height );
			textureCommand[3] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_SIZE );

			// 0x085
			textureCommand[4] = PICA_CMD_DATA_TEXTURE_ADDR( pCTRTexture->GetPhysicalAddr() );
			textureCommand[5] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR1 );

			// 0x08e
			textureCommand[6] = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( pCTRTexture->GetFormat() );
			textureCommand[7] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_FORMAT );
				
			GFGL::AddCommandCopy( textureCommand, sizeof(textureCommand) );//@todo 直接書き込みたい
		}
#else
		GFGL::SetTexture( 0, pTexture );
		GFGL::SetSamplerState( 0, pSampler );
#endif

		shaderStateMap.m_Uv0Enable = true;
	}

	scenegraph::resource::MaterialResourceNode										*pMaterial = const_cast<scenegraph::resource::MaterialResourceNode*>( pMeshDrawTag->GetMaterialInstanceNode()->GetMaterialResourceNode() );
	scenegraph::resource::MaterialResourceNode::AttributeParam		*pAttributeParam = pMaterial->GetAttributeParam();
	const scenegraph::resource::MaterialResourceNode::UserData		&rUserData = pMaterial->GetUserData();

	if( rUserData.m_EdgeMapAlphaMask == scenegraph::resource::CombinerAttribute::UvTextures::None )
		return;

	{
		const gfx::Texture							*pTexture = pMeshDrawTag->GetTexture( rUserData.m_EdgeMapAlphaMask );
		const gfx::SamplerStateObject		*pSampler =  pMeshDrawTag->GetSampler( rUserData.m_EdgeMapAlphaMask );

#if defined(GF_PLATFORM_CTR)
		{
			u32                   textureCommand[8];

			const gfl2::gfx::ctr::CTRTexture					*pCTRTexture = reinterpret_cast<const gfl2::gfx::ctr::CTRTexture*>(pTexture);
			const gfl2::gfx::Texture::TextureDesc			&rTextureDesc = pCTRTexture->GetTextureDesc();

			{
				const scenegraph::resource::MaterialResourceNode::TextureInfo		&rTexInfo = pMaterial->GetTextureInfo( 0 );

				textureCommand[0] = PICA_CMD_DATA_TEXTURE_WRAP_FILTER(	rTexInfo.m_Attribute.m_MagFilter, rTexInfo.m_Attribute.m_MinFilter,
																																pCTRTexture->GetFormat(),
																																rTexInfo.m_Attribute.m_RepeatV, rTexInfo.m_Attribute.m_RepeatU,
																																0, PICA_DATA_TEXTURE1_SAMPLER_TYPE_TEXTURE_2D );
					
        textureCommand[1] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_WRAP_FILTER );
			}
				
			//デフォルトではTexture1に設定するコマンド
			// 0x082
			textureCommand[2] = PICA_CMD_DATA_TEXTURE_SIZE( rTextureDesc.width, rTextureDesc.height );
			textureCommand[3] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_SIZE );

			// 0x085
			textureCommand[4] = PICA_CMD_DATA_TEXTURE_ADDR( pCTRTexture->GetPhysicalAddr() );
			textureCommand[5] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_ADDR );

			// 0x08e
			textureCommand[6] = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( pCTRTexture->GetFormat() );
			textureCommand[7] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_FORMAT );
				
			GFGL::AddCommandCopy( textureCommand, sizeof(textureCommand) );//@todo 直接書き込みたい
		}
#else
		GFGL::SetTexture( 1, pTexture );
		GFGL::SetSamplerState( 1, pSampler );
#endif

		shaderStateMap.m_Uv1Enable = true;
	}

	rRenderState.SetShaderStateMap( shaderStateMap );
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
EdgeMapSceneRenderPath::EdgeMapSceneRenderPath() : SceneRenderPath()
{
	SceneRenderPath::InitDescription			desc;

	//this->SceneRenderPath::Initialize( desc );
#if USE_CUSTOM_CONTAINER
	m_DrawableNodeContainer.CreateBuffer( 32 );//例えばポケモン６体分
#endif
}

EdgeMapSceneRenderPath::~EdgeMapSceneRenderPath()
{
	GFX_SAFE_DELETE( m_pEdgeMapTexture[EdgeColor::Normal] );

#if REMOVAL_OF_MEMORY_LEAK
#if defined GF_PLATFORM_WIN32
  GFL_SAFE_DELETE( m_pEdgeMapTexture[EdgeColor::ID] );
#endif
#endif
}

void EdgeMapSceneRenderPath::CreateEdgeMapTexture( f32 screanWidth, f32 screanHeight )
{
#if defined(GF_PLATFORM_CTR)
	m_pEdgeMapTexture[EdgeColor::Normal] = GFGL::CreateTexture( screanHeight, screanWidth, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
	m_pEdgeMapTexture[EdgeColor::ID] = NULL;
#else
	m_pEdgeMapTexture[EdgeColor::Normal] = GFGL::CreateTexture( screanWidth, screanHeight, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
	m_pEdgeMapTexture[EdgeColor::ID] = GFGL::CreateTexture( screanWidth, screanHeight, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
#endif
}

const IShaderDriver* EdgeMapSceneRenderPath::ShaderOverride()
{
	return &m_EdgeMapShaderDriver;
}

const util::FixedSizeContainer<scenegraph::instance::DrawableNode*>* EdgeMapSceneRenderPath::DrawableNodeOverride()
{
#if USE_CUSTOM_CONTAINER
	return &m_DrawableNodeContainer;
#else
	return NULL;
#endif
}

void EdgeMapSceneRenderPath::SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
	GFGL::SetRenderTarget( EdgeColor::Normal, m_pEdgeMapTexture[EdgeColor::Normal]->GetSurface() );
	if ( m_pEdgeMapTexture[EdgeColor::ID] )
		GFGL::SetRenderTarget( EdgeColor::ID, m_pEdgeMapTexture[EdgeColor::ID]->GetSurface() );
	GFGL::SetDepthStencil( pDepthStencil );
}

void EdgeMapSceneRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
#if SAMPLE_DIFF
  GFGL::ClearRenderTarget( Color( 0.5f, 0.5f, 0.0f) );
  // @note 深度バッファはフレームバッファの物を使う&nijiではGameManager層がバッファクリアを行っているのでカラー部分だけクリアすればよい 150317 ikuta_junya
  // @todo はずだが、無いとWIN32版のIDエッジで画面が乱れるのでクリアする
  GFGL::ClearDepthStencil( pDepthStencil, 1.0f, 255 );
#else
	GFGL::ClearRenderTargetDepthStencil( Color( 0.5f, 0.5f, 0.0f), 1.0f, 255 );
#endif

  // @note WIN32版ではマルチレンダリングターゲットでIDエッジを再現している
  if ( m_pEdgeMapTexture[EdgeColor::ID] )
		GFGL::ClearRenderTarget( EdgeColor::ID, Color( 1.0f, 1.0f, 1.0f) );

}

void EdgeMapSceneRenderPath::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
#if USE_CUSTOM_CONTAINER
	if ( m_DrawableNodeContainer.free_size() )
		m_DrawableNodeContainer.push_back( pNode );
#endif
}

void EdgeMapSceneRenderPath::RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
#if USE_CUSTOM_CONTAINER
	m_DrawableNodeContainer.remove( pNode );
#endif
}

void EdgeMapSceneRenderPath::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
}

void EdgeMapSceneRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );

	GFGL::BeginScene();

	if ( GFGL::GetRenderTarget() )
	{
		if ( rDrawContext.m_DisplayWidth && rDrawContext.m_DisplayHeight )
		{
			GFGL::SetViewport(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
			GFGL::SetScissor(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
		}
		else
		{
			const gfx::Surface::SurfaceDesc			&rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
			GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
			GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
		}

		ClearRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );
	}
	
	const IShaderDriver		*pShader = ShaderOverride();
	m_pDrawManager->SetOverrideShader( pShader );

#if defined(GF_PLATFORM_CTR)

	nn::gr::CommandBufferJumpHelper		*pCommandBuffer = GFGL::GetJumpHelper();
	bit32		*cmd = pCommandBuffer->GetCurrentPtr();

	{//proj
		nn::math::MTX44						*pMtx44;
		u32												tempBuffer[1024];
		u32												*pSeek = tempBuffer;

		pMtx44 = (nn::math::MTX44*)&rDrawContext.m_Projection;
		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 86, *pMtx44 );
	}

	{//view
		nn::math::MTX34						*pMtx34;
		u32												tempBuffer[1024];
		u32												*pSeek = tempBuffer;

		pMtx34 = (nn::math::MTX34*)&rDrawContext.m_View;

		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 90, *pMtx34 );
	}

	pCommandBuffer->Put(cmd);
#endif

	m_pDrawManager->Draw( false );

#if  SAMPLE_DIFF
#if !USE_SKYBOX_BUFFER_CLEAR
  if ( GFGL::GetRenderTarget() )
  {
    // MainRenderPathの為に深度バッファをクリア @todo MainRenderPath側でも良い 150317 ikuta_junya
    GFGL::ClearDepthStencil( 1.0f, 0xFF );
  }
#endif
#endif

	GFGL::EndScene();
#if defined(GF_PLATFORM_WIN32)
	GFGL::SetRenderTarget( EdgeColor::Normal, NULL );
	GFGL::SetRenderTarget( EdgeColor::ID, NULL );
#endif
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MainRenderPath::MainRenderPath()
{
#if USE_CUSTOM_CONTAINER
	m_DrawableNodeContainer.CreateBuffer( 120 );//例えばポケモン６体分
#endif
}

MainRenderPath::~MainRenderPath()
{

}

void MainRenderPath::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
#if USE_CUSTOM_CONTAINER
	if ( m_DrawableNodeContainer.free_size() )
	  m_DrawableNodeContainer.push_back( pNode );
#endif
}

void MainRenderPath::RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
#if USE_CUSTOM_CONTAINER
  m_DrawableNodeContainer.remove( pNode );
#endif
}

const gfl2::util::FixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* MainRenderPath::DrawableNodeOverride()
{
#if USE_CUSTOM_CONTAINER
	return &m_DrawableNodeContainer;
#else
  return NULL;
#endif
}

void MainRenderPath::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);

  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE_MAIN_SET_DRAWABLE );
	SetDrawableNode();
  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE_MAIN_SET_DRAWABLE );
  FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE_MAIN_DRAW_UPDATE );
	m_pDrawManager->Update();
  FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE_MAIN_DRAW_UPDATE );

#if USE_LOCAL
  Field::debug::FieldDebugTypes::s_DrawInfo.visibleTagNum = m_pDrawManager->GetVisibleTagNum();
#endif // #if USE_LOCAL

}

void MainRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );

	GFGL::BeginScene();

	if ( GFGL::GetRenderTarget() )
	{
		if ( rDrawContext.m_DisplayWidth && rDrawContext.m_DisplayHeight )
		{
			GFGL::SetViewport(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
			GFGL::SetScissor(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
		}
		else
		{
			const gfx::Surface::SurfaceDesc			&rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
			GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
			GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
		}

#if SAMPLE_DIFF
     // @note nijiの現状のパイプラインでは不要、カラーはGameManager層、深度はEdgeMapSceneRenderPathの最後にクリアしている ikuta_junya 150317
     // @note EdgeMapSceneRenderPathを行わない場合、深度はGameManager層でクリアされている
#else // #if SAMPLE_DIFF
     GFGL::ClearRenderTarget( Color( 1.0f, 0.1f, 0.1f) );
     GFGL::ClearDepthStencil( 1.0f, 0xFF );
#endif
	}
	
	const IShaderDriver		*pShader = ShaderOverride();
	m_pDrawManager->SetOverrideShader( pShader );

#if defined(GF_PLATFORM_CTR)

	nn::gr::CommandBufferJumpHelper		*pCommandBuffer = GFGL::GetJumpHelper();
	bit32		*cmd = pCommandBuffer->GetCurrentPtr();

	{//proj
		nn::math::MTX44						*pMtx44;
		u32												tempBuffer[1024];
		u32												*pSeek = tempBuffer;

		pMtx44 = (nn::math::MTX44*)&rDrawContext.m_Projection;
		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 86, *pMtx44 );
	}

	{//view
		nn::math::MTX34						*pMtx34;
		u32												tempBuffer[1024];
		u32												*pSeek = tempBuffer;

		pMtx34 = (nn::math::MTX34*)&rDrawContext.m_View;

		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 90, *pMtx34 );
	}

	pCommandBuffer->Put(cmd);
#endif

	m_pDrawManager->Draw( false );

	GFGL::EndScene();
}

// @todo 
void MainRenderPath::SetDrawableNode()
{
	m_pDrawManager->RemoveAllObject();
  Field::debug::FieldDebugTypes::s_DrawInfo.drawbleNodeNum = 0;
  Field::debug::FieldDebugTypes::s_DrawInfo.visibleNodeNum = 0;

	const util::FixedSizeContainer<scenegraph::instance::DrawableNode*>		*pDrawableNodeContainer = DrawableNodeOverride();
	if ( pDrawableNodeContainer )
	{
		const util::FixedSizeContainer<scenegraph::instance::DrawableNode*>		&rDrawableNodeContainer = *pDrawableNodeContainer;

		for( u32 i = 0; i < rDrawableNodeContainer.size(); ++i )
		{
      Field::debug::FieldDebugTypes::s_DrawInfo.drawbleNodeNum ++;
      if( rDrawableNodeContainer[i]->IsVisible() )
      {
        Field::debug::FieldDebugTypes::s_DrawInfo.visibleNodeNum++;
      }

			m_pDrawManager->AddObject( rDrawableNodeContainer[i] );
		}
	}
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
OutLinePostRenderPath::OutLinePostRenderPath( float screanSizeX, float screanSizeY ) : IRenderPath()
{
#if SAMPLE_DIFF
  for( u32 i = 0; i < EdgeMapSceneRenderPath::EdgeColor::Max; i++ )
  {
    m_EnabledTable[i] = false;
  }
#endif

#if REMOVAL_OF_MEMORY_LEAK
  for( u32 i=0 ;i<BIN_DATA_CNT ; i++ )
  {
    m_pBinaryData[i] = NULL;
  }
  s32 binaryDataIndex = 0;
#endif

	{
		{
			scenegraph::resource::GfBinaryMdlData							resourceData;

			s32		size;
#if defined(GF_PLATFORM_CTR)
			c8		*pBinData = (c8*)LocalFunc::ReadFile( "rom:/data/test/field_test/EdgeMap/exports/EdgeMapShader.ctr.gfbmdl" );
#else
			c8		*pBinData = (c8*)LocalFunc::ReadFile( "R:/home/niji/exec/data_win32/data/test/field_test/EdgeMap/exports/EdgeMapShader.wingl.gfbmdl" );
#endif
				
			resourceData.SetModelData( pBinData );

			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::Normal] = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );	

#if REMOVAL_OF_MEMORY_LEAK
      m_pBinaryData[binaryDataIndex] = pBinData;
      binaryDataIndex++;
#endif
		}

		{
			static const char* pShaderFileList[] = {
#if defined(GF_PLATFORM_CTR)
          			"rom:/data/test/field_test/EdgeMap/exports/Shader/Comb3_Manual.ctr.gfvsh",
#else
					"R:/home/niji/exec/data_win32/data/test/field_test/EdgeMap/exports/Shaders/Comb3_Manual.wingl.gfbvsh",
					"R:/home/niji/exec/data_win32/data/test/field_test/EdgeMap/exports/Shaders/Comb3_Manual.wingl.gfbfsh",
#endif
				};

			u32				shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);

			for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
			{
				scenegraph::resource::GfBinaryShdData							resourceData;

				s32		size;
				c8		*pBinData = (c8*)LocalFunc::ReadFile( pShaderFileList[shaderNo] );

				resourceData.SetShaderData( pBinData );

				scenegraph::resource::ResourceNode		*pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
				m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::Normal]->AddChild( pBinShdNode );

#if REMOVAL_OF_MEMORY_LEAK
        m_pBinaryData[binaryDataIndex] = pBinData;
        binaryDataIndex++;
#endif

				//GLMemory::Free( pBinData );
			}
		}

		{
			scenegraph::resource::GfBinaryTexData							texResourceData;
			scenegraph::resource::ResourceNode								*pBinTexRootNode;

			s32		texsize;
			s32		size;
#if defined(GF_PLATFORM_CTR)
			c8		*pTexBinData = (c8*)LocalFunc::ReadFile( "rom:/data/test/field_test/EdgeMap/exports/Texture/EdgeMapImage.3ds.tga" );
#else
			c8		*pTexBinData = (c8*)LocalFunc::ReadFile( "R:/home/niji/exec/data_win32/data/test/field_test/EdgeMap/exports/Texture/EdgeMapImage.wingl.tga" );
#endif
			
			texResourceData.SetTextureData( pTexBinData );

			//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
			pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::Normal]->AddChild( pBinTexRootNode );

#if REMOVAL_OF_MEMORY_LEAK
      m_pBinaryData[binaryDataIndex] = pTexBinData;
      binaryDataIndex++;
#endif

			//GLMemory::Free( pTexBinData );
		}
	}

	{
		{
			scenegraph::resource::GfBinaryMdlData							resourceData;

			s32		size;
#if defined(GF_PLATFORM_CTR)
			c8		*pBinData = (c8*)LocalFunc::ReadFile( "rom:/data/test/field_test/IDEdgeMap//exports/IDEdgeMap.ctr.gfbmdl" );
#else
			c8		*pBinData = (c8*)LocalFunc::ReadFile( "R:/home/niji/exec/data_win32/data/test/field_test/IDEdgeMap/exports/IDEdgeMap.wingl.gfbmdl" );
#endif
				
			resourceData.SetModelData( pBinData );

			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::ID] = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );	

#if REMOVAL_OF_MEMORY_LEAK
      m_pBinaryData[binaryDataIndex] = pBinData;
      binaryDataIndex++;
#endif

		}

		{
			static const char* pShaderFileList[] = {
#if defined(GF_PLATFORM_CTR)
          "rom:/data/test/field_test/IDEdgeMap/exports/Shaders/FillScrean.ctr.gfbvsh",
          "rom:/data/test/field_test/IDEdgeMap/exports/Shaders/Comb2_Manual.ctr.gfbfsh",
          "rom:/data/test/field_test/IDEdgeMap/exports/Shaders/Comb3_Manual.ctr.gfbfsh",
#else
					"R:/home/niji/exec/data_win32/data/test/field_test/IDEdgeMap/exports/Shader/Comb2_Manual.wingl.gfvsh",
					"R:/home/niji/exec/data_win32/data/test/field_test/IDEdgeMap/exports/Shader/Comb2_Manual.wingl.gffsh",
					"R:/home/niji/exec/data_win32/data/test/field_test/IDEdgeMap/exports/Shader/Comb3_Manual.wingl.gfvsh",
					"R:/home/niji/exec/data_win32/data/test/field_test/IDEdgeMap/exports/Shader/Comb3_Manual.wingl.gffsh",
#endif
				};

			u32				shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);

			for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
			{
				scenegraph::resource::GfBinaryShdData							resourceData;

				s32		size;
				c8		*pBinData = (c8*)LocalFunc::ReadFile( pShaderFileList[shaderNo] );

				resourceData.SetShaderData( pBinData );

				scenegraph::resource::ResourceNode		*pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
				m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::ID]->AddChild( pBinShdNode );

				//GLMemory::Free( pBinData );

#if REMOVAL_OF_MEMORY_LEAK
        m_pBinaryData[binaryDataIndex] = pBinData;
        binaryDataIndex++;
#endif

			}
		}

		{
			scenegraph::resource::GfBinaryTexData							texResourceData;
			scenegraph::resource::ResourceNode								*pBinTexRootNode;

			s32		texsize;
			s32		size;
#if defined(GF_PLATFORM_CTR)
			c8		*pTexBinData = (c8*)LocalFunc::ReadFile( "rom:/data/test/field_test/IDEdgeMap//exports/Textures/MapImage.3ds.tga" );
#else
			c8		*pTexBinData = (c8*)LocalFunc::ReadFile( "R:/home/niji/exec/data_win32/data/test/field_test/IDEdgeMap/exports/Texture/MapImage.wingl.tga" );
#endif
			
			texResourceData.SetTextureData( pTexBinData );

			//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
			pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::ID]->AddChild( pBinTexRootNode );

			//GLMemory::Free( pTexBinData );

#if REMOVAL_OF_MEMORY_LEAK
      m_pBinaryData[binaryDataIndex] = pTexBinData;
      binaryDataIndex++;
#endif

		}
	}

	for( u32 i = 0; i < EdgeMapSceneRenderPath::EdgeColor::Max; ++i )
	{
		m_pModelInstanceNode[i] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pBinModelRootNode[i] );

		scenegraph::instance::DrawableNode::UpdateDrawTagInfo				info;
		m_pModelInstanceNode[i]->UpdateDrawTag(info);

		scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode[i] );
	}
}

OutLinePostRenderPath::~OutLinePostRenderPath()
{
	for( u32 i = 0; i < EdgeMapSceneRenderPath::EdgeColor::Max; ++i )
	{
		delete m_pModelInstanceNode[i];
		delete m_pBinModelRootNode[i];
	}

#if REMOVAL_OF_MEMORY_LEAK
  for( u32 i=0 ;i<BIN_DATA_CNT ; i++ )
  {
    GLMemory::Free( m_pBinaryData[i] );
  }
#endif
}

void OutLinePostRenderPath::SetEdgeMapTexture( u32 type, const gfl2::gfx::Texture* pTexture)
{
	m_pTexture[type] = pTexture;
}

void OutLinePostRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
	GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );

	GFGL::BeginScene();

	const gfx::Surface::SurfaceDesc			&rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
	GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);

	// GFGL::ClearDepthStencil( rDrawContext.m_pDepthStencil, 1.0f, 255 );

	{
		RenderState			renderState;
		math::Matrix34	dummyMat34 = math::Matrix34::GetIdentity();
		math::Matrix44	dummyMat = math::Matrix44::GetIdentity();
			
		renderState.SetProjectionMatrix( &dummyMat );
		renderState.SetViewMatrix( &dummyMat34 );
		renderState.SetWorldMatrix( &dummyMat34 );

    // 法線エッジ
#if SAMPLE_DIFF
    if( m_EnabledTable[EdgeMapSceneRenderPath::EdgeColor::Normal ] )
#endif
		{
			MeshDrawTag			*pTag = m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::Normal]->GetMeshDrawTag( 0 );

			for( u32 texNo = 0; texNo < 3; ++texNo )
				pTag->SetTexture( texNo, m_pTexture[EdgeMapSceneRenderPath::EdgeColor::Normal] );

			pTag->Draw( renderState );
		}

    // IDエッジ
#if SAMPLE_DIFF
    if( m_EnabledTable[EdgeMapSceneRenderPath::EdgeColor::ID ] )
#endif
		{
			{//レイヤー０
				MeshDrawTag			*pTag = m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::ID]->GetMeshDrawTag( 0 );

				for( u32 texNo = 0; texNo < 3; ++texNo )
					pTag->SetTexture( texNo, m_pTexture[EdgeMapSceneRenderPath::EdgeColor::ID] );

				pTag->Draw( renderState );
			}

			{//レイヤー１
				MeshDrawTag			*pTag = m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::ID]->GetMeshDrawTag( 1 );

				for( u32 texNo = 0; texNo < 3; ++texNo )
					pTag->SetTexture( texNo, m_pTexture[EdgeMapSceneRenderPath::EdgeColor::ID] );

				pTag->Draw( renderState );
			}
		}
	}

	GFGL::SetTexture( 0, NULL );//次のフレームでは最初にRTとして使用するので、バインドをはずしておく。
	GFGL::SetTexture( 1, NULL );//次のフレームでは最初にRTとして使用するので、バインドをはずしておく。
	GFGL::SetTexture( 2, NULL );//次のフレームでは最初にRTとして使用するので、バインドをはずしておく。

	GFGL::EndScene();
}

//----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
MyRenderingPipeLine::MyRenderingPipeLine( f32 screanWidth, f32 screanHeight )
  : m_RenderPathCnt(0)
  , m_OutLinePostRenderPath( screanWidth, screanHeight )
  , m_LayoutRenderPath()
  , m_EffectRenderPath()
{
	// メインとなるシーン描画パスを初期化
	SceneRenderPath::InitDescription desc;
	desc.m_DrawManagerDesc.m_NodeStackSize	  = 120;
	desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;
	
  m_MainRenderPath.Initialize( desc );

	// エッジマップシーンを初期化
	m_EdgeMapSceneRenderPath.Initialize( desc );
	m_EdgeMapSceneRenderPath.CreateEdgeMapTexture( screanWidth, screanHeight );

	// アウトラインシーンを初期化
	const gfx::Texture *pTexture;
	pTexture = m_EdgeMapSceneRenderPath.GetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::Normal );
	m_OutLinePostRenderPath.SetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::Normal, pTexture );
	pTexture = m_EdgeMapSceneRenderPath.GetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::ID );
	m_OutLinePostRenderPath.SetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::ID, pTexture );

#if SAMPLE_DIFF
    m_EdgeMode = EDGE_MODE_OFF;
    SetEdgeMode( EDGE_MODE_FULL );
#endif

	// エッジマップシーンの内部リストを初期をする(場合によってはこれをしないとDrawでストップします)
	Update();
}

//----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
MyRenderingPipeLine::~MyRenderingPipeLine(void)
{
}

//----------------------------------------------------------------------------
/**
*  @brief  最初のパスから描画できる状態にする
*/
//-----------------------------------------------------------------------------
b32 MyRenderingPipeLine::StartRenderPath(void)
{
	m_RenderPathCnt = 0;
	return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  現状のパスにあったシーンを返す
*  @return シーン
*/
//-----------------------------------------------------------------------------
RenderingPipeLine::IRenderPath* MyRenderingPipeLine::GetRenderPath(void)
{
	switch( m_RenderPathCnt )
	{
	case 0:
#if SAMPLE_DIFF
      {
        if( m_EdgeMode == EDGE_MODE_OFF )
        {
          return NULL;
        }

#if defined(GF_PLATFORM_CTR)

        // @note CTR版ではIDエッジの書き込みはMainRenderPathで行う
        if( m_EdgeMode == EDGE_MODE_ID )
        {
          return NULL;
        }

#endif // defined(GF_PLATFORM_CTR)
      }
#endif // #if SAMPLE_DIFF

		return &m_EdgeMapSceneRenderPath;
		break;
	case 1:
		return &m_MainRenderPath;
		break;
	case 2:
#if SAMPLE_DIFF
      {
        if( m_EdgeMode == EDGE_MODE_OFF )
        {
          return NULL;
        }
      }
#endif // #if SAMPLE_DIFF
		return &m_OutLinePostRenderPath;
		break;
  case 3:
    return &m_EffectRenderPath;
    break;
	case 4:
		return &m_LayoutRenderPath;
		break;
  case 5:
    return &m_UtilTextRenderPath;
    break;
	}
	return NULL;
}

//----------------------------------------------------------------------------
/**
*  @brief  次のパスへ遷移
*  @return trueなら次のパス、falseなら描画終了
*/
//-----------------------------------------------------------------------------
b32 MyRenderingPipeLine::NextRenderPath(void)
{
	m_RenderPathCnt++;
	if ( m_RenderPathCnt == 6 )
	{
		return false;
	}
	return true;
}


void MyRenderingPipeLine::UpdateTest( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContex )
{
  b32			isEnable = this->StartRenderPath();
  if ( !isEnable )		return;

  do
  {
    IRenderPath			*pRenderPath = this->GetRenderPath();

    if ( pRenderPath )
    {
      #if FIELD_DEBUG_TICK_ENABLE
      Field::debug::FieldDebugTypes::TickDefine::Index tickDefineTable[] = { 
        Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE_EDEGE_MAP,
        Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE_MAIN,
        Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE_OUTLINE_POST,
        Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE_EFFECT,
        Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE_LAYOUT,
        Field::debug::FieldDebugTypes::TickDefine::PIPELINE_UPDATE_UTILTEXT
      };
      #endif
      FIELD_DEBUG_TICK_START( tickDefineTable[ m_RenderPathCnt ] );
      pRenderPath->Update( rDrawContex );
      FIELD_DEBUG_TICK_END( tickDefineTable[ m_RenderPathCnt ] );
    }

  }while( this->NextRenderPath() );
}

void MyRenderingPipeLine::ExecuteTest( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContex )
{
  b32			isEnable = this->StartRenderPath();
  if ( !isEnable )		return;

  do
  {
    IRenderPath			*pRenderPath = this->GetRenderPath();

    if ( pRenderPath )
    {
#if FIELD_DEBUG_TICK_ENABLE
      Field::debug::FieldDebugTypes::TickDefine::Index tickDefineTable[] = { 
        Field::debug::FieldDebugTypes::TickDefine::EXE_EDEGE_MAP,
        Field::debug::FieldDebugTypes::TickDefine::EXE_MAIN,
        Field::debug::FieldDebugTypes::TickDefine::EXE_OUTLINE_POST,
        Field::debug::FieldDebugTypes::TickDefine::EXE_EFFECT,
        Field::debug::FieldDebugTypes::TickDefine::EXE_LAYOUT,
        Field::debug::FieldDebugTypes::TickDefine::EXE_UTILTEXT
      };
#endif
      FIELD_DEBUG_TICK_START( tickDefineTable[ m_RenderPathCnt ] );
      gfl2::renderingengine::renderer::CombinerCtrShaderDriver::CacheClear();
      MeshDrawTag::CacheClear();
      pRenderPath->Execute( rDrawContex );
      FIELD_DEBUG_TICK_END( tickDefineTable[ m_RenderPathCnt ] );
    }

  }while( this->NextRenderPath() );
}



#if SAMPLE_DIFF

void MyRenderingPipeLine::SetEdgeMode( EdgeMode edgeMode )
{
  m_EdgeMode = edgeMode;

  switch ( edgeMode )
  {
  case EDGE_MODE_OFF:    // エッジ無し
    {
      m_OutLinePostRenderPath.SetEnable( EdgeMapSceneRenderPath::EdgeColor::ID, false );
      m_OutLinePostRenderPath.SetEnable( EdgeMapSceneRenderPath::EdgeColor::Normal, false );
      break;
    }
  case EDGE_MODE_ID:     // IDのみ
    {
      m_OutLinePostRenderPath.SetEnable( EdgeMapSceneRenderPath::EdgeColor::ID, true );
      m_OutLinePostRenderPath.SetEnable( EdgeMapSceneRenderPath::EdgeColor::Normal, false );
      break;
    }
  case EDGE_MODE_NORMAL:     // 法線のみ
    {
      m_OutLinePostRenderPath.SetEnable( EdgeMapSceneRenderPath::EdgeColor::ID, false );
      m_OutLinePostRenderPath.SetEnable( EdgeMapSceneRenderPath::EdgeColor::Normal, true );
      break;
    }
  case EDGE_MODE_FULL:       // 法線+ID
    {
      m_OutLinePostRenderPath.SetEnable( EdgeMapSceneRenderPath::EdgeColor::ID, true );
      m_OutLinePostRenderPath.SetEnable( EdgeMapSceneRenderPath::EdgeColor::Normal, true );
      break;
    }
  default:
    break;
  }
}

#endif // #if SAMPLE_DIFF

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )