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
    void* pShaderBinary = pAllocator->SystemMemoryMalloc(fileSize, 128);
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
#if 0
	{
		scenegraph::resource::GfBinaryMdlData							resourceData;

		s32		size;
		c8		*pBinData = (c8*)LocalFunc::ReadFile( "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/EdgeMapVColA.gfbmdl.wingl" );
			
		resourceData.SetModelData( pBinData );

		m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );	
	}

	{
		static const char* pShaderFileList[] = {
			"R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Shader/Comb2_Manual.wingl.gfvsh",
			"R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Shader/Comb2_Manual.wingl.gffsh",
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

			delete pBinData;
		}
	}

	{
		static const c8*			pTextureFileList[] = {
			"R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Texture/NormalTexture.wingl.tga",
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

			delete pTexBinData;
		}
	}
#endif

	{
		scenegraph::resource::GfMdlData							resourceData;
		resourceData.ReadFile( "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/EdgeMapVColA.gfmdl" );

		m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
	}

	{
		m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pBinModelRootNode );
	}

	{
		scenegraph::instance::DrawableNode::UpdateDrawTagInfo				info;

		m_pModelInstanceNode->UpdateDrawTag(info);

		scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode );
	}

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

#ifdef WIN32
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
#if 1
	//GFX_SAFE_DELETE( m_pVertexShader );
	//GFX_SAFE_DELETE( m_pPixelShader );
#endif

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

#ifdef WIN32
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

		GFGL::SetTexture( 0, pTexture );
		GFGL::SetSamplerState( 0, pSampler );
	}

	scenegraph::resource::MaterialResourceNode										*pMaterial = const_cast<scenegraph::resource::MaterialResourceNode*>( pMeshDrawTag->GetMaterialInstanceNode()->GetMaterialResourceNode() );
	scenegraph::resource::MaterialResourceNode::AttributeParam		*pAttributeParam = pMaterial->GetAttributeParam();
	const scenegraph::resource::MaterialResourceNode::UserData		&rUserData = pMaterial->GetUserData();

	if( rUserData.m_EdgeMapAlphaMask == scenegraph::resource::CombinerAttribute::UvTextures::None )
		return;

	{
		const gfx::Texture							*pTexture = pMeshDrawTag->GetTexture( rUserData.m_EdgeMapAlphaMask );
		const gfx::SamplerStateObject		*pSampler =  pMeshDrawTag->GetSampler( rUserData.m_EdgeMapAlphaMask );

		GFGL::SetTexture( 1, pTexture );
		GFGL::SetSamplerState( 1, pSampler );
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
	m_pEdgeMapTexture[EdgeColor::Normal] = GFGL::CreateTexture( screanWidth, screanHeight, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
	m_pEdgeMapTexture[EdgeColor::ID] = GFGL::CreateTexture( screanWidth, screanHeight, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
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
	GFGL::SetRenderTarget( EdgeColor::ID, m_pEdgeMapTexture[EdgeColor::ID]->GetSurface() );
	//GFGL::SetRenderTarget( 0, pRenderTarget );
	//GFGL::SetDepthStencil( pDepthStencil );
}

void EdgeMapSceneRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
	GFGL::ClearRenderTarget( m_pEdgeMapTexture[EdgeColor::Normal]->GetSurface(), Color( 0.5f, 0.5f, 0.0f) );
	GFGL::ClearRenderTarget( m_pEdgeMapTexture[EdgeColor::ID]->GetSurface(), Color( 1.0f, 1.0f, 1.0f) );
	//GFGL::ClearRenderTarget( pRenderTarget, Color( 0.5f, 0.5f, 0.0f) );
	//GFGL::ClearDepthStencil( pDepthStencil, 1.0f, 255 );
}

void EdgeMapSceneRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	RenderingPipeLine::SceneRenderPath::Execute( rDrawContext );
	GFGL::SetRenderTarget( EdgeColor::Normal, NULL );
	GFGL::SetRenderTarget( EdgeColor::ID, NULL );
}

void EdgeMapSceneRenderPath::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	if ( m_DrawableNodeContainer.free_size() )
		m_DrawableNodeContainer.push_back( pNode );
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
OutLinePostRenderPath::OutLinePostRenderPath( float screanSizeX, float screanSizeY ) : IRenderPath()
{
	//@note 外らかリソースを渡してもらう
	static const char* pFilePath[ EdgeMapSceneRenderPath::EdgeColor::Max ] = 
	{
		"R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/EdgeMapShader.gfmdl",
		"R:/home/niji/git_program/gflib2/RenderingEngine/resource/IDEdgeMap/exports/IDEdgeMap.gfmdl"
	};

	for( u32 i = 0; i < EdgeMapSceneRenderPath::EdgeColor::Max; ++i )
	{
		scenegraph::resource::GfMdlData							resourceData;
		resourceData.ReadFile( pFilePath[i] );

		m_pBinModelRootNode[i] = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
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

void OutLinePostRenderPath::SetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor type, const gfl2::gfx::Texture* pTexture)
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

//----------------------------------------------------------------------------------------
//【Bloom : 1パス目】輝度抽出用パス
//----------------------------------------------------------------------------------------
BloomRenderPathForLuminance::BloomRenderPathForLuminance(u32 screanWidth, u32 screanHeight) : IRenderPath()
{
	// 頂点フォーマットとシェーダコンパイル
	{
		VertexElement element[] = 
		{
			VertexElement::VERTEX_ELEMENT_END()
		};

		m_pVertexShader			= GFGL::CreateVertexShaderFromFile( "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/BloomLuminance.gfvsh" , "VS", element, NULL, FileFormat::ShaderCode );
		m_pPixelShader			= GFGL::CreatePixelShaderFromFile ( "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/BloomLuminance.gffsh" , "PS", NULL, FileFormat::ShaderCode );
	}

	// インデックスバッファの作成
	{
		u16 idxVertexID[] = { 0, 1, 2, 3 };
		m_pIndexBuffer = GFGL::CreateIndexBuffer( sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index16, idxVertexID );
	}

	// ブレンドステート
	m_pBlendStateObject = GFGL::CreateBlendStateObject();
	m_pBlendStateObject->SetAlphaTestEnable(true);
	m_pBlendStateObject->SetAlphaRef(0.01f);
	m_pBlendStateObject->UpdateState();

	// ラスタライズ
	m_RasterizerStateObject = GFGL::CreateRasterizerStateObject();
	m_RasterizerStateObject->SetCullMode( CullMode::None );
	m_RasterizerStateObject->UpdateState();

	// 深度ステート
	m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject();
	m_DepthStencilStateObject->SetDepthTestEnable(false);
	m_DepthStencilStateObject->SetDepthWriteEnable(false);
	m_DepthStencilStateObject->SetStencilTestEnable(false);
	m_DepthStencilStateObject->SetStencilWriteMask(false);
	m_DepthStencilStateObject->UpdateState();

	// サンプラ
	m_SamplerStateObject = GFGL::CreateSamplerStateObject();
	m_SamplerStateObject->SetSamplerFilter( TextureFilter::Linear, TextureFilter::Linear );
	m_SamplerStateObject->SetSamplerAddress( TextureAddress::Clamp, TextureAddress::Clamp );
	m_SamplerStateObject->UpdateState();

	// フラグメント用定数バッファ
	{
		ConstantBufferElement element[] = 
		{
			{ ConstantType::Vector, 1 },
			{ ConstantType::Vector, 1 },
			ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
		};
		m_pConstantBufferFs = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
	}

#if USE_SCALE_BUFFER
	// 輝度抜き出し用のレンダーバッファを作成
	m_pTextureForLuminance = GFGL::CreateTexture( screanWidth/START_SCALE_CNT , screanHeight/START_SCALE_CNT , 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
#else
	int rate = (BloomRenderPathForScale::SCALE_CNT) * 2 + START_SCALE_CNT;
	// 輝度抜き出し用のレンダーバッファを作成
	m_pTextureForLuminance = GFGL::CreateTexture( screanWidth/rate , screanHeight/rate , 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
#endif

	// グレア対象のテクスチャを作成
	m_pTargetTexture = GFGL::CreateTexture( screanWidth ,screanHeight , 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );	

	// αマスクテクスチャ
	{		
		ImageData image;
		image.ReadFromFile( "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/BloomAlphaMask.tga" );
		m_pAlphaMasTexture = GFGL::CreateTextureFromImage( image, Pool::Default );
	}

	m_weight	= gfl2::math::Vector(0.298912f, 0.586611f , 0.1144780f, 0.50f);
	m_mulColor	= gfl2::math::Vector(1.0f ,1.0f ,1.0f ,0.0f);	
}

void BloomRenderPathForLuminance::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	// ブロックコピー(グレア対象となる画像を取得)
	GFGL::BlitSurfaceToTexture( rDrawContext.m_pRenderTarget , m_pTargetTexture  );

	//------------------------------------------------
	// 輝度テクスチャの作成
	//------------------------------------------------

	// 輝度用サーフェスをセットして描画開始
	GFGL::SetRenderTarget( m_pTextureForLuminance->GetSurface() );
	GFGL::SetDepthStencil( NULL );
	GFGL::BeginScene();

	// サーフェスを初期化
	GFGL::ClearRenderTarget( m_pTextureForLuminance->GetSurface(),gfl2::gfx::Color::GetBlack() );

	const gfx::Surface::SurfaceDesc &rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
	GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);

	// フラグメント用定数バッファの更新
	{
		PixelShaderVectorConstants psVectorConstants;
		psVectorConstants.cConstantWeight   = m_weight;
		psVectorConstants.cConstantMulColor = m_mulColor;
		m_pConstantBufferFs->Update(&psVectorConstants, sizeof(psVectorConstants));
	}
	
	// シェーダと定数バッファのセット
	GFGL::SetVertexShader( m_pVertexShader );
	GFGL::SetPixelShader( m_pPixelShader );
	GFGL::SetPixelShaderConstant ( 8, m_pConstantBufferFs );

	// ポリゴンとレンダーステートのセット
	GFGL::SetIndices( m_pIndexBuffer );	
	GFGL::SetBlendState( m_pBlendStateObject );
	GFGL::SetRasterizerState( m_RasterizerStateObject );
	GFGL::SetDepthStencilState( m_DepthStencilStateObject );
	GFGL::SetTexture( 0 , m_pTargetTexture );
	GFGL::SetTexture( 1 , m_pAlphaMasTexture );
	GFGL::SetSamplerState( 0 , m_SamplerStateObject );
	GFGL::SetSamplerState( 1 , m_SamplerStateObject );

	// ポリゴン描画
	GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);

	GFGL::EndScene();
}

//----------------------------------------------------------------------------------------
//【Bloom : 2パス目】縮小用パス
//----------------------------------------------------------------------------------------
BloomRenderPathForScale::BloomRenderPathForScale( gfl2::gfx::Texture *m_pOriginTexture ) : IRenderPath()
{
	m_pTargetTexture = m_pOriginTexture;

	// 頂点フォーマットとシェーダコンパイル
	{
		VertexElement element[] = 
		{
			VertexElement::VERTEX_ELEMENT_END()
		};

		m_pVertexShader = GFGL::CreateVertexShaderFromFile( "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/BloomScale.gfvsh", "VS", element, NULL, FileFormat::ShaderCode );
		m_pPixelShader  = GFGL::CreatePixelShaderFromFile ( "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/BloomScale.gffsh", "PS", NULL, FileFormat::ShaderCode );
	}

	// インデックスバッファの作成
	{
		u16 idxVertexID[] = { 0, 1, 2, 3 };
		m_pIndexBuffer = GFGL::CreateIndexBuffer( sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index16, idxVertexID );
	}

	// ブレンドステート
	m_pBlendStateObject = GFGL::CreateBlendStateObject();
	m_pBlendStateObject->SetBlendEnable(false);
	m_pBlendStateObject->UpdateState();

	// ラスタライズ
	m_RasterizerStateObject = GFGL::CreateRasterizerStateObject();
	m_RasterizerStateObject->SetCullMode( CullMode::None );
	m_RasterizerStateObject->UpdateState();

	// 深度ステート
	m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject();
	m_DepthStencilStateObject->SetDepthTestEnable(false);
	m_DepthStencilStateObject->SetDepthWriteEnable(false);
	m_DepthStencilStateObject->SetStencilTestEnable(false);
	m_DepthStencilStateObject->SetStencilWriteMask(false);
	m_DepthStencilStateObject->UpdateState();

	// サンプラ
	m_SamplerStateObject = GFGL::CreateSamplerStateObject();
	m_SamplerStateObject->SetSamplerFilter( TextureFilter::Linear, TextureFilter::Linear );
	m_SamplerStateObject->SetSamplerAddress( TextureAddress::Clamp, TextureAddress::Clamp );
	m_SamplerStateObject->UpdateState();

	for( int i=0 ; i<SCALE_CNT ; i++ )
	{
		// 縮小用のレンダーバッファを作成
		m_pTextureForScale[i] = GFGL::CreateTexture( m_pOriginTexture->GetTextureDesc().width / ((i*2)+2) , m_pOriginTexture->GetTextureDesc().height / ((i*2)+2) , 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
	}
}

void BloomRenderPathForScale::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	for( int i=0; i<SCALE_CNT ; i++ )
	{
		// 輝度用サーフェスをセットして描画開始
		GFGL::SetRenderTarget( m_pTextureForScale[i]->GetSurface() );
		GFGL::SetDepthStencil( NULL );				
		GFGL::BeginScene();

		const gfx::Surface::SurfaceDesc &rDesc1 = GFGL::GetRenderTarget()->GetSurfaceDesc();
		GFGL::SetViewport(0, 0, rDesc1.width, rDesc1.height);

		// シェーダのセット
		GFGL::SetVertexShader( m_pVertexShader );
		GFGL::SetPixelShader( m_pPixelShader );

		// ポリゴンとレンダーステートのセット
		GFGL::SetIndices( m_pIndexBuffer );		
		GFGL::SetBlendState( m_pBlendStateObject );
		GFGL::SetRasterizerState( m_RasterizerStateObject );
		GFGL::SetDepthStencilState( m_DepthStencilStateObject );
		if( i== 0 ){ GFGL::SetTexture( 0 , m_pTargetTexture );        }
		else       { GFGL::SetTexture( 0 , m_pTextureForScale[i-1] ); }
		GFGL::SetSamplerState( 0 , m_SamplerStateObject );

		// ポリゴン描画
		GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);

		GFGL::EndScene();
	}
}

//----------------------------------------------------------------------------------------
//【Bloom : 3パス目】ブラー用パス
//----------------------------------------------------------------------------------------
BloomRenderPathForBlur::BloomRenderPathForBlur( gfl2::gfx::Texture *m_pOriginTexture ) : IRenderPath()
{
	m_pTargetTexture = m_pOriginTexture;

	// 頂点フォーマットとシェーダコンパイル
	{
		VertexElement element[] = 
		{
			VertexElement::VERTEX_ELEMENT_END()
		};

		m_pVertexShader = GFGL::CreateVertexShaderFromFile( "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/BloomBlur.gfvsh", "VS", element, NULL, FileFormat::ShaderCode );
		m_pPixelShader  = GFGL::CreatePixelShaderFromFile ( "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/BloomBlur.gffsh", "PS", NULL, FileFormat::ShaderCode );
	}

	// インデックスバッファの作成
	{
		u16 idxVertexID[] = { 0, 1, 2, 3 };
		m_pIndexBuffer = GFGL::CreateIndexBuffer( sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index16, idxVertexID );
	}

	// ブレンドステート
	m_pBlendStateObject = GFGL::CreateBlendStateObject();
	m_pBlendStateObject->SetBlendEnable(true);
	m_pBlendStateObject->SetBlendOp( BlendOp::Add );
	m_pBlendStateObject->SetBlendFunc( BlendFunc::SrcAlpha, BlendFunc::InverseSrcAlpha );
	m_pBlendStateObject->UpdateState();

	// ラスタライズ
	m_RasterizerStateObject = GFGL::CreateRasterizerStateObject();
	m_RasterizerStateObject->SetCullMode( CullMode::None );
	m_RasterizerStateObject->UpdateState();

	// 深度ステート
	m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject();
	m_DepthStencilStateObject->SetDepthTestEnable(false);
	m_DepthStencilStateObject->SetDepthWriteEnable(false);
	m_DepthStencilStateObject->SetStencilTestEnable(false);
	m_DepthStencilStateObject->SetStencilWriteMask(false);
	m_DepthStencilStateObject->UpdateState();

	// サンプラ
	m_SamplerStateObject = GFGL::CreateSamplerStateObject();
	m_SamplerStateObject->SetSamplerFilter( TextureFilter::Linear, TextureFilter::Linear );
	m_SamplerStateObject->SetSamplerAddress( TextureAddress::Clamp, TextureAddress::Clamp );
	m_SamplerStateObject->UpdateState();

	// 頂点用定数バッファ
	{
		ConstantBufferElement element[] = 
		{
			{ ConstantType::Vector, 1 },
			{ ConstantType::Vector, 1 },
			{ ConstantType::Vector, 1 },
			ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
		};
		m_pConstantBufferVs = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
	}

	// 重ね用のレンダーバッファを作成
	m_pTextureForBlur = GFGL::CreateTexture( m_pOriginTexture->GetTextureDesc().width , m_pOriginTexture->GetTextureDesc().height , 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );

	m_uvOffset[0] = gfl2::math::Vector( 0.000f, 0.000f, 0.0f);
	m_uvOffset[1] = gfl2::math::Vector( 0.000f,-0.009f, 0.0f);
	m_uvOffset[2] = gfl2::math::Vector( 0.009f,-0.009f, 0.0f);
	m_uvOffset[3] = gfl2::math::Vector( 0.009f, 0.000f, 0.0f);
	m_uvOffset[4] = gfl2::math::Vector(-0.009f, 0.000f, 0.0f);
	m_uvOffset[5] = gfl2::math::Vector(-0.009f,-0.009f, 0.0f);
	m_blur_cnt    = MAX_BLUR_CNT;
}

void BloomRenderPathForBlur::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	// サーフェスを初期化
	GFGL::SetRenderTarget( m_pTextureForBlur->GetSurface() );
	GFGL::SetDepthStencil( NULL );
	GFGL::BeginScene();

	GFGL::ClearRenderTarget( m_pTextureForBlur->GetSurface(),gfl2::gfx::Color::GetBlack() );

	// 輝度用サーフェスをセットして描画開始
	const gfx::Surface::SurfaceDesc &rDesc1 = GFGL::GetRenderTarget()->GetSurfaceDesc();
	GFGL::SetViewport(0, 0, rDesc1.width, rDesc1.height);

	for( int i=0; i<m_blur_cnt ; i+=3 )
	{
		// 頂点用定数バッファの更新
		{
			VertexShaderVectorConstants vsVectorConstants;
			vsVectorConstants.cOffset0.x = m_uvOffset[i+0].x;
			vsVectorConstants.cOffset0.y = m_uvOffset[i+0].y;
			vsVectorConstants.cOffset1.x = m_uvOffset[i+1].x;
			vsVectorConstants.cOffset1.y = m_uvOffset[i+1].y;
			vsVectorConstants.cOffset2.x = m_uvOffset[i+2].x;
			vsVectorConstants.cOffset2.y = m_uvOffset[i+2].y;
			m_pConstantBufferVs->Update(&vsVectorConstants, sizeof(vsVectorConstants));
		}

		// シェーダのセット
		GFGL::SetVertexShader( m_pVertexShader );
		GFGL::SetVertexShaderConstant( 0, m_pConstantBufferVs );
		GFGL::SetPixelShader( m_pPixelShader );

		// ポリゴンとレンダーステートのセット
		GFGL::SetIndices( m_pIndexBuffer );		
		GFGL::SetBlendState( m_pBlendStateObject );
		GFGL::SetRasterizerState( m_RasterizerStateObject );
		GFGL::SetDepthStencilState( m_DepthStencilStateObject );
		GFGL::SetTexture( 0 , m_pTargetTexture );
		GFGL::SetSamplerState( 0 , m_SamplerStateObject );

		// ポリゴン描画
		GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);
	}

	GFGL::EndScene();
}

//----------------------------------------------------------------------------------------
//【Bloom : 4パス目】加算合成用パス
//----------------------------------------------------------------------------------------
BloomRenderPathForAdd::BloomRenderPathForAdd(gfl2::gfx::Texture* pTargetTexture,gfl2::gfx::Texture* pTextureForAdd) : IRenderPath()
{
	m_pTargetTexture = pTargetTexture;
	m_pTextureForAdd = pTextureForAdd;

	// 頂点フォーマットとシェーダコンパイル
	{
		VertexElement element[] = 
		{
			VertexElement::VERTEX_ELEMENT_END()
		};

		m_pVertexShader = GFGL::CreateVertexShaderFromFile( "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/BloomAdd.gfvsh", "VS", element, NULL, FileFormat::ShaderCode );
		m_pPixelShader  = GFGL::CreatePixelShaderFromFile ( "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/BloomAdd.gffsh", "PS", NULL, FileFormat::ShaderCode );
	}

	// インデックスバッファの作成
	{
		u16 idxVertexID[] = { 0, 1, 2, 3 };
		m_pIndexBuffer = GFGL::CreateIndexBuffer( sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index16, idxVertexID );
	}

	// ブレンドステート
	m_pBlendStateObject = GFGL::CreateBlendStateObject();
	m_pBlendStateObject->SetBlendEnable(false);
	m_pBlendStateObject->UpdateState();

	// ラスタライズ
	m_RasterizerStateObject = GFGL::CreateRasterizerStateObject();
	m_RasterizerStateObject->SetCullMode( CullMode::None );
	m_RasterizerStateObject->UpdateState();

	// 深度ステート
	m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject();
	m_DepthStencilStateObject->SetDepthTestEnable(false);
	m_DepthStencilStateObject->SetDepthWriteEnable(false);
	m_DepthStencilStateObject->SetStencilTestEnable(false);
	m_DepthStencilStateObject->SetStencilWriteMask(false);
	m_DepthStencilStateObject->UpdateState();

	// サンプラ
	m_SamplerStateObject = GFGL::CreateSamplerStateObject();
	m_SamplerStateObject->SetSamplerFilter( TextureFilter::Linear, TextureFilter::Linear );
	m_SamplerStateObject->SetSamplerAddress( TextureAddress::Clamp, TextureAddress::Clamp );
	m_SamplerStateObject->UpdateState();
}

void BloomRenderPathForAdd::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	// デフォルトのサーフェスをセットして描画開始
	GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
	GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );
	GFGL::BeginScene();

	const gfx::Surface::SurfaceDesc &rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
	GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);

	// シェーダのセット
	GFGL::SetVertexShader( m_pVertexShader );
	GFGL::SetPixelShader( m_pPixelShader );

	// ポリゴンとレンダーステートのセット
	GFGL::SetIndices( m_pIndexBuffer );		
	GFGL::SetBlendState( m_pBlendStateObject );
	GFGL::SetRasterizerState( m_RasterizerStateObject );
	GFGL::SetDepthStencilState( m_DepthStencilStateObject );
	GFGL::SetTexture( 0 , m_pTargetTexture );
	GFGL::SetTexture( 1 , m_pTextureForAdd );
	GFGL::SetSamplerState( 0 , m_SamplerStateObject );
	GFGL::SetSamplerState( 1 , m_SamplerStateObject );

	// ポリゴン描画
	GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);

	GFGL::EndScene();
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MyRenderingPipeLine::MyRenderingPipeLine( f32 screanWidth, f32 screanHeight ) : 
	m_OutLinePostRenderPath( screanWidth, screanHeight )
, m_BloomRenderPathForLuminace	 ( (u32)screanWidth  , (u32)screanHeight )
, m_BloomRenderPathForScale		 ( m_BloomRenderPathForLuminace.GetLuminanceTexture() )
#if USE_SCALE_BUFFER
, m_BloomRenderPathForBlur		 ( m_BloomRenderPathForScale.GetMinScaleTexture()     )
#else
, m_BloomRenderPathForBlur		 ( m_BloomRenderPathForLuminace.GetLuminanceTexture() )
#endif
, m_BloomRenderPathForAdd		 ( m_BloomRenderPathForLuminace.GetTargetTexture() , m_BloomRenderPathForBlur.GetBlurTexture() )
{
	SceneRenderPath::InitDescription			desc;
	m_SceneRenderPath.Initialize( desc );

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
		return &m_SceneRenderPath;
		break;
	case 2:
		//return &m_OutLinePostRenderPath;
		break;
	case 3:
		return &m_BloomRenderPathForLuminace;
		break;
	case 4:
#if USE_SCALE_BUFFER
		return &m_BloomRenderPathForScale;
#else 
		return NULL;
#endif
		break;
	case 5:
		return &m_BloomRenderPathForBlur;
		break;
	case 6:
		return &m_BloomRenderPathForAdd;
		break;
	case 7:
		return &m_PresentPath;
		break;
	}
	return NULL;
}

b32 MyRenderingPipeLine::NextRenderPath()
{
	m_RenderPathCnt++;
	if ( m_RenderPathCnt == 8 )
		return false;
	return true;
}

void MyRenderingPipeLine::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_EdgeMapSceneRenderPath.AddEdgeRenderingTarget( pNode );
}

