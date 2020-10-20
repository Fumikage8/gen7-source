#include "MyRenderingPipeLine.h"

#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>

#if defined(GF_PLATFORM_CTR)
#include <nn/fs.h>
#include <nn/hio.h>
#endif

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

struct LocalFunc{
	static void *ReadFile( const char* pFilePath )
  {//自前データロード
#if defined(GF_PLATFORM_CTR)
    nn::fs::FileReader file( pFilePath );
    u32 fileSize = file.GetSize();
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
	{
		scenegraph::resource::GfBinaryMdlData							resourceData;

		s32		size;
#if defined(GF_PLATFORM_CTR)
		c8		*pBinData = (c8*)LocalFunc::ReadFile( "rom:/EdgeMapVColAShader/EdgeMapVColA.ctr.gfbmdl" );
#else
		c8		*pBinData = (c8*)LocalFunc::ReadFile( "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/EdgeMapVColA.wingl.gfbmdl" );
#endif
			
		resourceData.SetModelData( pBinData );

		m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );	
	}

	{
		static const char* pShaderFileList[] = {
#if defined(GF_PLATFORM_CTR)
			"rom:/EdgeMapVColAShader/Shader/Comb2_Manual.ctr.gfbvsh",
#else
			"R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Shader/Comb2_Manual.wingl.gfvsh",
			"R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Shader/Comb2_Manual.wingl.gffsh",
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
		}
	}

	{
		static const c8*			pTextureFileList[] = {
#if defined(GF_PLATFORM_CTR)
			"rom:/EdgeMapVColAShader/Texture/NormalTexture.3ds.tga",
#else
			"R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Texture/NormalTexture.wingl.tga",
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
}

b32 EdgeMapSceneRenderPath::EdgeMapShaderDriver::Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
	MeshDrawTag						*pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );
	const IShaderDriver		*pShaderDriver = pTag->GetShaderDriver();

	scenegraph::resource::MaterialResourceNode										*pMaterial = const_cast<scenegraph::resource::MaterialResourceNode*>( pMeshDrawTag->GetMaterialInstanceNode()->GetMaterialResourceNode() );
	scenegraph::resource::MaterialResourceNode::AttributeParam		*pAttributeParam = pMaterial->GetAttributeParam();
	const scenegraph::resource::MaterialResourceNode::UserData		&rUserData = pMaterial->GetUserData();

	if ( rUserData.m_IDEdgeEnable == false && rUserData.m_EdgeType == scenegraph::resource::EdgeType::None )
		return false;

	switch( rUserData.m_EdgeType ){
	case scenegraph::resource::EdgeType::None:
		break;
	case scenegraph::resource::EdgeType::Erase:
		pAttributeParam->m_ConstantColor[1].w = 0.0f;//法線出力オフ
		pAttributeParam->m_ConstantColor[2].w = 0.0f;//頂点カラー出力オフ
		break;
	case scenegraph::resource::EdgeType::VColor:
		pAttributeParam->m_ConstantColor[1].w = 0.0f;//法線出力オフ
		pAttributeParam->m_ConstantColor[2].w = 1.0f;//頂点カラー出力オン
		//return false;
		break;
	default:
		pAttributeParam->m_ConstantColor[1].w = 1.0f;//法線出力オン
		pAttributeParam->m_ConstantColor[2].w = 1.0f;//頂点カラー出力オン
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
#endif

	return true;
}

void EdgeMapSceneRenderPath::EdgeMapShaderDriver::SetTextureOverride( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
	MeshDrawTag											*pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );

	{
		const gfx::Texture							*pTexture = pTag->GetTexture( 0 );
		const gfx::SamplerStateObject		*pSampler =  pTag->GetSampler( 0 );

#if defined(GF_PLATFORM_CTR)
		{
			u32                   textureCommand[6];

			const gfl2::gfx::ctr::CTRTexture					*pCTRTexture = reinterpret_cast<const gfl2::gfx::ctr::CTRTexture*>(pTexture);
			const gfl2::gfx::Texture::TextureDesc			&rTextureDesc = pCTRTexture->GetTextureDesc();
				
			//デフォルトではTexture0に設定するコマンド
			// 0x082
			textureCommand[0] = PICA_CMD_DATA_TEXTURE_SIZE( rTextureDesc.width, rTextureDesc.height );
			textureCommand[1] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_SIZE );

			// 0x085
			textureCommand[2] = PICA_CMD_DATA_TEXTURE_ADDR( pCTRTexture->GetPhysicalAddr() );
			textureCommand[3] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR1 );

			// 0x08e
			textureCommand[4] = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( pCTRTexture->GetFormat() );
			textureCommand[5] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_FORMAT );
				
			GFGL::AddCommandCopy( textureCommand, sizeof(textureCommand) );//@note 直接書き込みたい
		}
#else
		GFGL::SetTexture( 0, pTexture );
		GFGL::SetSamplerState( 0, pSampler );
#endif
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
			u32                   textureCommand[6];

			const gfl2::gfx::ctr::CTRTexture					*pCTRTexture = reinterpret_cast<const gfl2::gfx::ctr::CTRTexture*>(pTexture);
			const gfl2::gfx::Texture::TextureDesc			&rTextureDesc = pCTRTexture->GetTextureDesc();
				
			//デフォルトではTexture0に設定するコマンド
			// 0x082
			textureCommand[0] = PICA_CMD_DATA_TEXTURE_SIZE( rTextureDesc.width, rTextureDesc.height );
			textureCommand[1] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_SIZE );

			// 0x085
			textureCommand[2] = PICA_CMD_DATA_TEXTURE_ADDR( pCTRTexture->GetPhysicalAddr() );
			textureCommand[3] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_ADDR );

			// 0x08e
			textureCommand[4] = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( pCTRTexture->GetFormat() );
			textureCommand[5] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_FORMAT );
				
			GFGL::AddCommandCopy( textureCommand, sizeof(textureCommand) );//@note 直接書き込みたい
		}
#else
		GFGL::SetTexture( 1, pTexture );
		GFGL::SetSamplerState( 1, pSampler );
#endif
	}
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
EdgeMapSceneRenderPath::EdgeMapSceneRenderPath() : SceneRenderPath()
{
	SceneRenderPath::InitDescription			desc;

	//this->SceneRenderPath::Initialize( desc );
	m_DrawableNodeContainer.CreateBuffer( 6 );//例えばポケモン６体分
}

EdgeMapSceneRenderPath::~EdgeMapSceneRenderPath()
{
	GFX_SAFE_DELETE( m_pEdgeMapTexture[EdgeColor::Normal] );
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

const gfx::GLFixedSizeContainer<scenegraph::instance::DrawableNode*>* EdgeMapSceneRenderPath::DrawableNodeOverride()
{
	return &m_DrawableNodeContainer;
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
	GFGL::ClearRenderTargetDepthStencil( m_pEdgeMapTexture[EdgeColor::Normal]->GetSurface(), Color( 0.5f, 0.5f, 0.0f), pDepthStencil, 1.0f, 255 );

	if ( m_pEdgeMapTexture[EdgeColor::ID] )
		GFGL::ClearRenderTarget( m_pEdgeMapTexture[EdgeColor::ID]->GetSurface(), Color( 1.0f, 1.0f, 1.0f) );
}

void EdgeMapSceneRenderPath::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	if ( m_DrawableNodeContainer.free_size() )
		m_DrawableNodeContainer.push_back( pNode );
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
	m_DrawableNodeContainer.CreateBuffer( 6 );//例えばポケモン６体分
}

MainRenderPath::~MainRenderPath()
{

}

void MainRenderPath::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.push_back( pNode );
}

const gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* MainRenderPath::DrawableNodeOverride()
{
	return &m_DrawableNodeContainer;
}

void MainRenderPath::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
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

		GFGL::ClearRenderTarget( rDrawContext.m_pRenderTarget, Color( 0.4f, 0.4f, 0.4f) );
		GFGL::ClearDepthStencil( rDrawContext.m_pDepthStencil, 1.0f, 0xFF );
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

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
OutLinePostRenderPath::OutLinePostRenderPath( float screanSizeX, float screanSizeY ) : IRenderPath()
{
	{
		{
			scenegraph::resource::GfBinaryMdlData							resourceData;

			s32		size;
#if defined(GF_PLATFORM_CTR)
			c8		*pBinData = (c8*)LocalFunc::ReadFile( "rom:/EdgeMap/exports/EdgeMapShader.ctr.gfbmdl" );
#else
			c8		*pBinData = (c8*)LocalFunc::ReadFile( "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/EdgeMapShader.wingl.gfbmdl" );
#endif
				
			resourceData.SetModelData( pBinData );

			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::Normal] = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );	
		}

		{
			static const char* pShaderFileList[] = {
#if defined(GF_PLATFORM_CTR)
					"rom:/EdgeMap/exports/Shader/Comb3_Manual.ctr.gfvsh",
#else
					"R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/Shader/Comb3_Manual.wingl.gfvsh",
					"R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/Shader/Comb3_Manual.wingl.gffsh",
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

				//GLMemory::Free( pBinData );
			}
		}

		{
			scenegraph::resource::GfBinaryTexData							texResourceData;
			scenegraph::resource::ResourceNode								*pBinTexRootNode;

			s32		texsize;
			s32		size;
#if defined(GF_PLATFORM_CTR)
			c8		*pTexBinData = (c8*)LocalFunc::ReadFile( "rom:/EdgeMap/exports/Texture/EdgeMapImage.3ds.tga" );
#else
			c8		*pTexBinData = (c8*)LocalFunc::ReadFile( "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/Texture/EdgeMapImage.wingl.tga" );
#endif
			
			texResourceData.SetTextureData( pTexBinData );

			//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
			pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::Normal]->AddChild( pBinTexRootNode );

			//GLMemory::Free( pTexBinData );
		}
	}

	{
		{
			scenegraph::resource::GfBinaryMdlData							resourceData;

			s32		size;
#if defined(GF_PLATFORM_CTR)
			c8		*pBinData = (c8*)LocalFunc::ReadFile( "rom:/IDEdgeMap/exports/IDEdgeMap.ctr.gfbmdl" );
#else
			c8		*pBinData = (c8*)LocalFunc::ReadFile( "R:/home/niji/git_program/gflib2/RenderingEngine/resource/IDEdgeMap/exports/IDEdgeMap.wingl.gfbmdl" );
#endif
				
			resourceData.SetModelData( pBinData );

			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::ID] = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );	
		}

		{
			static const char* pShaderFileList[] = {
#if defined(GF_PLATFORM_CTR)
					"rom:/IDEdgeMap/exports/Shader/Comb2_Manual.ctr.gfvsh",
					"rom:/IDEdgeMap/exports/Shader/Comb3_Manual.ctr.gfvsh",
#else
					"R:/home/niji/git_program/gflib2/RenderingEngine/resource/IDEdgeMap/exports/Shader/Comb2_Manual.wingl.gfvsh",
					"R:/home/niji/git_program/gflib2/RenderingEngine/resource/IDEdgeMap/exports/Shader/Comb2_Manual.wingl.gffsh",
					"R:/home/niji/git_program/gflib2/RenderingEngine/resource/IDEdgeMap/exports/Shader/Comb3_Manual.wingl.gfvsh",
					"R:/home/niji/git_program/gflib2/RenderingEngine/resource/IDEdgeMap/exports/Shader/Comb3_Manual.wingl.gffsh",
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
			}
		}

		{
			scenegraph::resource::GfBinaryTexData							texResourceData;
			scenegraph::resource::ResourceNode								*pBinTexRootNode;

			s32		texsize;
			s32		size;
#if defined(GF_PLATFORM_CTR)
			c8		*pTexBinData = (c8*)LocalFunc::ReadFile( "rom:/IDEdgeMap/exports/Texture/MapImage.3ds.tga" );
#else
			c8		*pTexBinData = (c8*)LocalFunc::ReadFile( "R:/home/niji/git_program/gflib2/RenderingEngine/resource/IDEdgeMap/exports/Texture/MapImage.wingl.tga" );
#endif
			
			texResourceData.SetTextureData( pTexBinData );

			//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
			pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::ID]->AddChild( pBinTexRootNode );

			//GLMemory::Free( pTexBinData );
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

	GFGL::ClearDepthStencil( rDrawContext.m_pDepthStencil, 1.0f, 255 );

	{
		RenderState			renderState;
		math::Matrix44		dummyMat44 = math::Matrix44::GetIdentity();
		math::Matrix34		dummyMat34 = math::Matrix34::GetIdentity();
			
		renderState.SetProjectionMatrix( &dummyMat44 );
		renderState.SetViewMatrix( &dummyMat34 );
		renderState.SetWorldMatrix( &dummyMat34 );

		{
			MeshDrawTag			*pTag = m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::Normal]->GetMeshDrawTag( 0 );

			for( u32 texNo = 0; texNo < 3; ++texNo )
				pTag->SetTexture( texNo, m_pTexture[EdgeMapSceneRenderPath::EdgeColor::Normal] );

			pTag->Draw( renderState );
		}

		{//IDエッジ
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

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MyRenderingPipeLine::MyRenderingPipeLine( f32 screanWidth, f32 screanHeight ) : 
	m_OutLinePostRenderPath( screanWidth, screanHeight )
{
	SceneRenderPath::InitDescription			desc;
	m_MainRenderPath.Initialize( desc );

	desc.m_DrawManagerDesc.m_NodeStackSize = 6;
	desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;
	m_EdgeMapSceneRenderPath.Initialize( desc );

	m_EdgeMapSceneRenderPath.CreateEdgeMapTexture( screanWidth, screanHeight );

	const gfx::Texture		*pTexture;

	pTexture = m_EdgeMapSceneRenderPath.GetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::Normal );
	m_OutLinePostRenderPath.SetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::Normal, pTexture );

	pTexture = m_EdgeMapSceneRenderPath.GetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::ID );
	m_OutLinePostRenderPath.SetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::ID, pTexture );	
}

MyRenderingPipeLine::~MyRenderingPipeLine()
{

}

b32 MyRenderingPipeLine::StartRenderPath()
{
	m_RenderPathCnt = 0;
	return true;
}

RenderingPipeLine::IRenderPath* MyRenderingPipeLine::GetRenderPath()
{
	//前のパスの結果を踏まえて、パスをスキップすることも可能。

	switch( m_RenderPathCnt ){
	case 0:
		//return &m_EdgeMapSceneRenderPath;
		break;
	case 1:
		return &m_MainRenderPath;
		break;
	case 2:
		//return &m_OutLinePostRenderPath;
		break;
	}
	return NULL;
}

b32 MyRenderingPipeLine::NextRenderPath()
{
	m_RenderPathCnt++;
	if ( m_RenderPathCnt == 3 )
		return false;
	return true;
}

void MyRenderingPipeLine::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_EdgeMapSceneRenderPath.AddEdgeRenderingTarget( pNode );
	m_MainRenderPath.AddEdgeRenderingTarget( pNode );
}

void MyRenderingPipeLine::SetDepthTexture( gfl2::gfx::Texture* pTexture )
{
#if defined(GF_PLATFORM_CTR)
	m_OutLinePostRenderPath.SetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::ID, pTexture );	
#endif
}

