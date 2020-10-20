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

#include <RenderingEngine/include/renderer/gfl2_CombinerCtrShaderDriver.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <model/include/gfl2_PokemonModel.h>

#if defined(GF_PLATFORM_CTR)
#include <nn/fs.h>
#include <nn/hio.h>
#endif

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;
using namespace poke_3d::renderer;

struct LocalFunc{
	static void *ReadFile( IGLAllocator* pObj, const char* pFilePath )
  {//自前データロード
#if defined(GF_PLATFORM_CTR)
    nn::fs::FileReader file( pFilePath );
    u32 fileSize = file.GetSize();
    GLMemory::SetInstanceAllocator( pObj );
    void* pShaderBinary = GLMemory::Malloc(fileSize, 128);
    GLMemory::ClearInstanceAllocator();
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
MainRenderPath::MainRenderPath(gfl2::gfx::IGLAllocator* allocator)
{
	m_DrawableNodeContainer.CreateBuffer( 6, allocator );//例えばポケモン６体分
}

MainRenderPath::~MainRenderPath()
{

}

void MainRenderPath::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.push_back( pNode );
}

void MainRenderPath::RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.remove( pNode );
}

const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* MainRenderPath::DrawableNodeOverride()
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

		//GFGL::ClearRenderTarget( rDrawContext.m_pRenderTarget, Color( 0.5f, 0.5f, 0.5f) );
		//GFGL::ClearDepthStencil( rDrawContext.m_pDepthStencil, 1.0f, 0xFF );
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

	m_pDrawManager->SetViewMatrix( rDrawContext.m_View );
	m_pDrawManager->Draw( false );

	GFGL::EndScene();
}


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MyRenderingPipeLine::MyRenderingPipeLine( gfl2::gfx::IGLAllocator* allocator, f32 screanWidth, f32 screanHeight, const gfl2::gfx::Surface *pFrameBuffer ):
  m_MainRenderPath(allocator),
  m_EdgeMapSceneRenderPath(allocator, 6),
  m_DistortionPostRenderPath(allocator, 6),
  m_DofType(gfl2::renderingengine::DofType::Front)
{
  m_IsBackgroundVisible = false;
	SceneRenderPath::InitDescription			desc;
	m_MainRenderPath.Initialize( allocator, desc );

	desc.m_DrawManagerDesc.m_NodeStackSize = 6;
	desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;
	m_EdgeMapSceneRenderPath.Initialize( allocator, desc );
  m_DistortionPostRenderPath.Initialize( allocator, desc );

	//screanWidth = 512, screanHeight = 256
  m_StretchBltFrameBufferPath.CreateFrameBufferTexture( allocator, pFrameBuffer );

  m_DistortionPostRenderPath.SetOverrideTexture( m_StretchBltFrameBufferPath.GetFrameBufferTexture() );

	m_EdgeMapSceneRenderPath.CreateEdgeMapTexture( allocator, screanWidth, screanHeight );

	const gfx::Texture		*pTexture;

	pTexture = m_EdgeMapSceneRenderPath.GetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::Normal );
	m_OutLinePostRenderPath.SetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::Normal, pTexture );

	pTexture = m_EdgeMapSceneRenderPath.GetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::ID );
	m_OutLinePostRenderPath.SetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::ID, pTexture );	

	{
		gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData							resourceData;
		m_pColorShaderData = (c8*)LocalFunc::ReadFile( allocator, "rom:/ColorShaderGfmdl/colorshaderplane.ctr.gfbmdl" );

		resourceData.SetModelData( m_pColorShaderData );

		m_ColorShaderRenderTargetPlaneResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator, &resourceData );
	}

	{
		static const char* pShaderFileList[] = {
			"rom:/ColorShaderGfmdl/Shaders/Comb1_Manual.ctr.gfbfsh",
			"rom:/ColorShaderGfmdl/Shaders/FillScrean.ctr.gfbvsh",
		};

		u32				shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);

		for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
		{
			scenegraph::resource::GfBinaryShdData							resourceData;

			s32		size;
			c8		*pBinData = (c8*)LocalFunc::ReadFile( allocator, pShaderFileList[shaderNo] );

			resourceData.SetShaderData( pBinData );

			scenegraph::resource::ResourceNode		*pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( allocator, &resourceData );
			m_ColorShaderRenderTargetPlaneResourceNode->AddChild( pBinShdNode );

			//GLMemory::Free( pBinData );
		}
	}

	{
		static const c8*			pTextureFileList[] = {
			"rom:/ColorShaderGfmdl/Textures/colorshadertexture.ctr.btex",
		};

		u32				texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);

		for( u32 texNo = 0; texNo < texNum; ++texNo )
		{
			scenegraph::resource::GfBinaryTexData							texResourceData;
			scenegraph::resource::ResourceNode								*pBinTexRootNode;

			s32		texsize;
			s32		size;
			c8		*pTexBinData = (c8*)LocalFunc::ReadFile( allocator, pTextureFileList[texNo] );
			
			texResourceData.SetTextureData( pTexBinData );

			pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( allocator, &texResourceData );
			m_ColorShaderRenderTargetPlaneResourceNode->AddChild( pBinTexRootNode );
		}
	}

	{
		m_ColorShaderRenderTargetPlaneInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(allocator, m_ColorShaderRenderTargetPlaneResourceNode);

		// カラーシェーダ用モデルをシーングラフから外す
		gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild(m_ColorShaderRenderTargetPlaneInstanceNode);

		m_ColorShaderTextureRenderPaths.CreateBuffer( poke_3d::model::PokemonModel::COLORTEXTURE_MAX,allocator );
	}

	{
		const char* pFileList[s_EdgeMapFileNum] = {
			"rom:/EdgeMapVColAShader/EdgeMapVColA.ctr.gfbmdl",
			"rom:/EdgeMapVColAShader/Shaders/Default.ctr.gfbvsh",
			"rom:/EdgeMapVColAShader/Shaders/NormalToUv.ctr.gfbvsh",
			"rom:/EdgeMapVColAShader/Shaders/Comb2_Manual.ctr.gfbfsh",
			"rom:/EdgeMapVColAShader/Textures/NormalTexture.ctr.btex",
			"rom:/EdgeMapVColAShader/Textures/ZInvNormalTexture.ctr.btex",
		};

		for( u32 i = 0; i < s_EdgeMapFileNum; ++i )
		{
			m_pEdgeMapBinData[i] = (c8*)LocalFunc::ReadFile( allocator, pFileList[i] );
		}

		m_EdgeMapSceneRenderPath.LoadResources( 
      allocator,
			m_pEdgeMapBinData[0],
			&m_pEdgeMapBinData[1], 3,
			&m_pEdgeMapBinData[4], 2 );
	}

	{
		const char* pFileList[s_OutLineFileNum] = {
			"rom:/EdgeMap/exports/EdgeMapShader.ctr.gfbmdl",
			"rom:/EdgeMap/exports/Shader/Comb3_Manual.ctr.gfbvsh",
			"rom:/EdgeMap/exports/Texture/EdgeMapImage.ctr.btex",

			"rom:/IDEdgeMap/exports/IDEdgeMap.ctr.gfbmdl",
			"rom:/IDEdgeMap/exports/Shaders/FillScrean.ctr.gfbvsh",
			"rom:/IDEdgeMap/exports/Shaders/Comb2_Manual.ctr.gfbfsh",
			"rom:/IDEdgeMap/exports/Shaders/Comb3_Manual.ctr.gfbfsh",
			"rom:/IDEdgeMap/exports/Textures/MapImage.ctr.btex"
		};

		for( u32 i = 0; i < s_OutLineFileNum; ++i )
		{
			m_pOutLineBinData[i] = (c8*)LocalFunc::ReadFile( allocator, pFileList[i] );
		}

		m_OutLinePostRenderPath.Initialize(
      allocator,
			m_pOutLineBinData[0],
			&m_pOutLineBinData[1], 1,
			m_pOutLineBinData[2],

			m_pOutLineBinData[3],
			&m_pOutLineBinData[4], 3,
			m_pOutLineBinData[7] );
	}

  {// 背景のモデルデータのリソース
    memset(m_BackgroundResourceSet, 0, sizeof(BackgroundResourceSet) * s_BackgroundFileNum);

    strcpy(m_BackgroundResourceSet[0].m_GfbmdlFilePath, "rom:/test_skybox/sky_sunny/converted_data/sky_sunny.ctr.gfbmdl");
    m_BackgroundResourceSet[0].m_ShaderNum = 2;
    strcpy(m_BackgroundResourceSet[0].m_ShaderFilePaths[0], "rom:/test_skybox/sky_sunny/converted_data/Shaders/Map04_sky01.ctr.gfbfsh");
    strcpy(m_BackgroundResourceSet[0].m_ShaderFilePaths[1], "rom:/test_skybox/sky_sunny/converted_data/Shaders/Default.ctr.gfbvsh");
    m_BackgroundResourceSet[0].m_TextureNum = 2;
    strcpy(m_BackgroundResourceSet[0].m_TextureFilePaths[0], "rom:/test_skybox/sky_sunny/converted_data/Textures/sky_colorbar01.ctr.btex");
    strcpy(m_BackgroundResourceSet[0].m_TextureFilePaths[1], "rom:/test_skybox/sky_sunny/converted_data/Textures/sky_star01.ctr.btex");

    strcpy(m_BackgroundResourceSet[1].m_GfbmdlFilePath, "rom:/kusa/btl_F/converted_data/btl_F_kusa.ctr.gfbmdl");
    m_BackgroundResourceSet[1].m_ShaderNum = 3;
    strcpy(m_BackgroundResourceSet[1].m_ShaderFilePaths[0], "rom:/kusa/btl_F/converted_data/Shaders/BATTLE_bg_default01GRE.ctr.gfbfsh");
    strcpy(m_BackgroundResourceSet[1].m_ShaderFilePaths[1], "rom:/kusa/btl_F/converted_data/Shaders/btl_A_sea_iro01_Manual.ctr.gfbfsh");
    strcpy(m_BackgroundResourceSet[1].m_ShaderFilePaths[2], "rom:/kusa/btl_F/converted_data/Shaders/BattleFieldShader.ctr.gfbvsh");
    m_BackgroundResourceSet[1].m_TextureNum = 4;
    strcpy(m_BackgroundResourceSet[1].m_TextureFilePaths[0], "rom:/kusa/btl_F/converted_data/Textures/btl_A_sea_iro01.ctr.btex");
    strcpy(m_BackgroundResourceSet[1].m_TextureFilePaths[1], "rom:/kusa/btl_F/converted_data/Textures/btl_A_sea_iro02.ctr.btex");
    strcpy(m_BackgroundResourceSet[1].m_TextureFilePaths[2], "rom:/kusa/btl_F/converted_data/Textures/btl_F_kusa_jime01.ctr.btex");
    strcpy(m_BackgroundResourceSet[1].m_TextureFilePaths[3], "rom:/kusa/btl_F/converted_data/Textures/btl_F_kusa_yama01.ctr.btex");

    strcpy(m_BackgroundResourceSet[2].m_GfbmdlFilePath, "rom:/kusa/btl_N/converted_data/btl_N_kusa.ctr.gfbmdl");
    m_BackgroundResourceSet[2].m_ShaderNum = 2;
    strcpy(m_BackgroundResourceSet[2].m_ShaderFilePaths[0], "rom:/kusa/btl_N/converted_data/Shaders/BATTLE_bg_default01GRE.ctr.gfbfsh");
    strcpy(m_BackgroundResourceSet[2].m_ShaderFilePaths[1], "rom:/kusa/btl_N/converted_data/Shaders/BattleFieldShader.ctr.gfbvsh");
    m_BackgroundResourceSet[2].m_TextureNum = 4;
    strcpy(m_BackgroundResourceSet[2].m_TextureFilePaths[0], "rom:/kusa/btl_N/converted_data/Textures/btl_N_kusa_jime01.ctr.btex");
    strcpy(m_BackgroundResourceSet[2].m_TextureFilePaths[1], "rom:/kusa/btl_N/converted_data/Textures/btl_N_kusa_mori01.ctr.btex");
    strcpy(m_BackgroundResourceSet[2].m_TextureFilePaths[2], "rom:/kusa/btl_N/converted_data/Textures/btl_N_kusa_oka01.ctr.btex");
    strcpy(m_BackgroundResourceSet[2].m_TextureFilePaths[3], "rom:/kusa/btl_N/converted_data/Textures/btl_N_kusa_yasi01.ctr.btex");

    strcpy(m_BackgroundResourceSet[3].m_GfbmdlFilePath, "rom:/kusa/btl_G/converted_data/btl_G_kusa.ctr.gfbmdl");
    m_BackgroundResourceSet[3].m_ShaderNum = 4;
    strcpy(m_BackgroundResourceSet[3].m_ShaderFilePaths[0], "rom:/kusa/btl_G/converted_data/Shaders/BATTLE_bg_blend01GRE.ctr.gfbfsh");
    strcpy(m_BackgroundResourceSet[3].m_ShaderFilePaths[1], "rom:/kusa/btl_G/converted_data/Shaders/BATTLE_bg_default01GRE.ctr.gfbfsh");
    strcpy(m_BackgroundResourceSet[3].m_ShaderFilePaths[2], "rom:/kusa/btl_G/converted_data/Shaders/btl_G_kusa_kusa01_Manual.ctr.gfbfsh");
    strcpy(m_BackgroundResourceSet[3].m_ShaderFilePaths[3], "rom:/kusa/btl_G/converted_data/Shaders/BattleFieldShader.ctr.gfbvsh");
    m_BackgroundResourceSet[3].m_TextureNum = 10;
    strcpy(m_BackgroundResourceSet[3].m_TextureFilePaths[0], "rom:/kusa/btl_G/converted_data/Textures/btl_G_all_rain.ctr.btex");
    strcpy(m_BackgroundResourceSet[3].m_TextureFilePaths[1], "rom:/kusa/btl_G/converted_data/Textures/btl_G_all_rain0m.ctr.btex");
    strcpy(m_BackgroundResourceSet[3].m_TextureFilePaths[2], "rom:/kusa/btl_G/converted_data/Textures/btl_G_all_snow0m.ctr.btex");
    strcpy(m_BackgroundResourceSet[3].m_TextureFilePaths[3], "rom:/kusa/btl_G/converted_data/Textures/btl_G_kusa_iwa01.ctr.btex");
    strcpy(m_BackgroundResourceSet[3].m_TextureFilePaths[4], "rom:/kusa/btl_G/converted_data/Textures/btl_G_kusa_jime01.ctr.btex");
    strcpy(m_BackgroundResourceSet[3].m_TextureFilePaths[5], "rom:/kusa/btl_G/converted_data/Textures/btl_G_kusa_jime02.ctr.btex");
    strcpy(m_BackgroundResourceSet[3].m_TextureFilePaths[6], "rom:/kusa/btl_G/converted_data/Textures/btl_G_kusa_kusa01.ctr.btex");
    strcpy(m_BackgroundResourceSet[3].m_TextureFilePaths[7], "rom:/kusa/btl_G/converted_data/Textures/btl_G_kusa_mori01.ctr.btex");
    strcpy(m_BackgroundResourceSet[3].m_TextureFilePaths[8], "rom:/kusa/btl_G/converted_data/Textures/btl_G_kusa_ueki01.ctr.btex");
    strcpy(m_BackgroundResourceSet[3].m_TextureFilePaths[9], "rom:/kusa/btl_G/converted_data/Textures/btl_G_kusa_yasi01.ctr.btex");

    for (u32 fileNum = 0; fileNum < s_BackgroundFileNum; ++fileNum)
    {
      m_BackgroundResourceSet[fileNum].m_pGfbmdlBinData = static_cast<c8*>(LocalFunc::ReadFile(allocator, m_BackgroundResourceSet[fileNum].m_GfbmdlFilePath));
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData gfBinaryMdlData;
      gfBinaryMdlData.SetModelData(m_BackgroundResourceSet[fileNum].m_pGfbmdlBinData);
      m_BackgroundResourceSet[fileNum].m_pModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator, &gfBinaryMdlData);
      for (s32 i = 0; i < m_BackgroundResourceSet[fileNum].m_ShaderNum; ++i)
      {
        m_BackgroundResourceSet[fileNum].m_pShaderBinData[i] = static_cast<c8*>(LocalFunc::ReadFile(allocator, m_BackgroundResourceSet[fileNum].m_ShaderFilePaths[i]));
        gfl2::renderingengine::scenegraph::resource::GfBinaryShdData gfBinaryShdData;
        gfBinaryShdData.SetShaderData(m_BackgroundResourceSet[fileNum].m_pShaderBinData[i]);
        m_BackgroundResourceSet[fileNum].m_pShaderResourceNodes[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator, &gfBinaryShdData);
        m_BackgroundResourceSet[fileNum].m_pModelResourceNode->AddChild(m_BackgroundResourceSet[fileNum].m_pShaderResourceNodes[i]);
      }
      for (u32 i = 0; i < m_BackgroundResourceSet[fileNum].m_TextureNum; ++i)
      {
        m_BackgroundResourceSet[fileNum].m_pTextureBinData[i] = static_cast<c8*>(LocalFunc::ReadFile(allocator, m_BackgroundResourceSet[fileNum].m_TextureFilePaths[i]));
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData gfBinaryTexData;
        gfBinaryTexData.SetTextureData(m_BackgroundResourceSet[fileNum].m_pTextureBinData[i]);
        m_BackgroundResourceSet[fileNum].m_pTextureResourceNodes[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator, &gfBinaryTexData);
        m_BackgroundResourceSet[fileNum].m_pModelResourceNode->AddChild(m_BackgroundResourceSet[fileNum].m_pTextureResourceNodes[i]);
      }

      m_BackgroundResourceSet[fileNum].m_pModelInstanceNode = 
        gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(allocator, m_BackgroundResourceSet[fileNum].m_pModelResourceNode);

      // 各種カラーテクスチャを検索して保存
      u32 materialNum = m_BackgroundResourceSet[fileNum].m_pModelInstanceNode->GetMaterialNum();
      // 元のマテリアル情報を保持
      for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
      {
        gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
        pMaterialInstanceNode = m_BackgroundResourceSet[fileNum].m_pModelInstanceNode->GetMaterialInstanceNode(materialNo);
      }

      s16 lightSetNo = m_BackgroundResourceSet[fileNum].m_pModelInstanceNode->GetMaterialInstanceNode(0)->GetAttributeParam()->m_LightSetNo;
      for (u32 i = 1; i < materialNum; ++i)
      {// ライトセット番号が一致しているかを調べる.
        GFL_ASSERT(lightSetNo == m_BackgroundResourceSet[fileNum].m_pModelInstanceNode->GetMaterialInstanceNode(0)->GetAttributeParam()->m_LightSetNo);
      }

      // DrawEnvNodeの作成
      m_BackgroundResourceSet[fileNum].m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(allocator);

      // AABBの設定
      m_BackgroundResourceSet[fileNum].m_pDrawEnvNode->SetAABB(
        gfl2::math::AABB(
          gfl2::math::Vector4(-50000.0f, -50000.0f, -50000.0f),
          gfl2::math::Vector4(50000.0f, 50000.0f, 50000.0f)
        )
      );

      // ライトとライトセットのノードを作成
      m_BackgroundResourceSet[fileNum].m_pLightSetNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(allocator);
      m_BackgroundResourceSet[fileNum].m_pLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(allocator);
      m_BackgroundResourceSet[fileNum].m_pLightNode->SetLightType(gfl2::renderingengine::LightType::Directional);
      gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 0.0f, 10000000.0f);
      m_BackgroundResourceSet[fileNum].m_pLightNode->SetPosition(position);
      
      //GfModelビューアのコードr:\home\niji\git_program\tool_project\ModelViewer\project\ApplicationCore.csの２７５行目では、
      //Y軸-1のベクトルを設定しているので合わせます。
      gfl2::math::Vector3 direction( 0.0f, -1.0f, 0.0f );
      m_BackgroundResourceSet[fileNum].m_pLightNode->SetDirection(direction);
      m_BackgroundResourceSet[fileNum].m_pLightNode->SetColor(gfl2::math::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
      m_BackgroundResourceSet[fileNum].m_pLightSetNode->SetSetNo(lightSetNo);        // いったん0を設定.
      m_BackgroundResourceSet[fileNum].m_pLightSetNode->AddLight(m_BackgroundResourceSet[fileNum].m_pLightNode);

      if (lightSetNo == 2)
      {// 2はフィールド用モデルなのでAmbientを追加
        //GfModelビューアのコードr:\home\niji\git_program\tool_project\ModelViewer\project\ApplicationCore.csの２７４行目では、
        //ライトセット番号を０としていますし、実際ここでのリソースも０でした。
        //よってアンビエントライトがセットされていません。このコードはもう不要かもしれません。
        gfl2::renderingengine::scenegraph::instance::LightNode* pAmbientLightNode;
        pAmbientLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(allocator);
        pAmbientLightNode->SetLightType(gfl2::renderingengine::LightType::Ambient);
        pAmbientLightNode->SetColor(gfl2::math::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        m_BackgroundResourceSet[fileNum].m_pLightSetNode->AddLight(pAmbientLightNode);
      }

      m_BackgroundResourceSet[fileNum].m_pDrawEnvNode->AddLightSet(m_BackgroundResourceSet[fileNum].m_pLightSetNode); //DrawEnvNodeの子となる

      gfl2::renderingengine::renderer::util::ModelRenderPath::InitDescription desc;
      m_BackgroundRenderPath[fileNum].Initialize(allocator, desc);
      m_BackgroundRenderPath[fileNum].SetDrawEnvNode(m_BackgroundResourceSet[fileNum].m_pDrawEnvNode);
      m_BackgroundRenderPath[fileNum].SetModelInstanceNode(m_BackgroundResourceSet[fileNum].m_pModelInstanceNode);
    }
  }

  // 被写界深度のパスの初期化
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode;
  m_pDofModelData = new gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData();
  m_pDofModelData->SetModelData((c8*)LocalFunc::ReadFile(allocator, "rom:/Dof/DofShader_ctr.ctr.gfbmdl"));

  m_DofRenderTargetPlaneResourceNode = 
    gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator, m_pDofModelData);

  m_pDofVertexShaderData = new gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
  m_pDofVertexShaderData->SetShaderData((c8*)LocalFunc::ReadFile(allocator, "rom:/Dof/Default.ctr.gfbvsh"));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(allocator, m_pDofVertexShaderData);
  m_DofRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_pDofFrontPixelShaderData = new gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
  m_pDofFrontPixelShaderData->SetShaderData((c8*)LocalFunc::ReadFile(allocator, "rom:/Dof/CombBack_Manual.ctr.gfbfsh"));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(allocator, m_pDofFrontPixelShaderData);
  m_DofRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_pDofBackPixelShaderData = new gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
  m_pDofBackPixelShaderData->SetShaderData((c8*)LocalFunc::ReadFile(allocator, "rom:/Dof/CombFront_Manual.ctr.gfbfsh"));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(allocator, m_pDofBackPixelShaderData);
  m_DofRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_pDofTextureData = new gfl2::renderingengine::scenegraph::resource::GfBinaryTexData();
  m_pDofTextureData->SetTextureData((c8*)LocalFunc::ReadFile(allocator, "rom:/Dof/EdgeMapImage.ctr.btex"));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(allocator, m_pDofTextureData);
  m_DofRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_DofRenderTargetPlaneInstanceNode = 
    gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(allocator, m_DofRenderTargetPlaneResourceNode);
  
  // 被写界深度用モデルをシーングラフから外す
  gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild(m_DofRenderTargetPlaneInstanceNode);

  {
    // ブラー用描画パス初期化
    {
      gfl2::renderingengine::renderer::util::MakeBlurImagePath::InitDescription desc;
      desc.m_ScreenWidth = 400;
      desc.m_ScreenHeight = 240;
      m_MakeBlurImagePath.Initialize(allocator, desc);
    }

    {
      gfl2::renderingengine::renderer::util::MakeReductionImagePath::InitDescription desc;
      desc.m_ScreenWidth = 400 / 2;
      desc.m_ScreenHeight = 240 / 2;
      desc.m_pSourceTexture = m_MakeBlurImagePath.GetDepthWrittenTexture();
      m_MakeReductionImagePath.Initialize(allocator, desc);
    }

    {
      gfl2::renderingengine::renderer::util::DofRenderPath::InitDescription desc;
      desc.m_pTargetModelInstanceNode = m_DofRenderTargetPlaneInstanceNode;
      desc.m_pSourceTexture = m_MakeReductionImagePath.GetDestinationTexture();
      m_DofRenderPath.Initialize(allocator, desc);
    }

    // ブラー用の焦点距離を設定
    f32 focusLength = 200.0f;
    f32 blurredCoef = 0.75f;
    m_MakeBlurImagePath.SetAvailableLayerNum(1);
    m_MakeBlurImagePath.SetFocusRange(0, focusLength, blurredCoef);
    m_DofRenderPath.SetFocusLength(focusLength);
    m_MakeBlurImagePath.ChangeDofType(m_DofType);
    m_DofRenderPath.ChangeDofType(m_DofType);
  }
}

void MyRenderingPipeLine::AddColorShaderTexture(
  const gfl2::gfx::Texture* pOriginalColorTexture,
  const gfl2::gfx::Texture* pColorShaderTexture)
{
  gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath* path = new gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath();

  path->SetColorShaderRenderTargetPlane(m_ColorShaderRenderTargetPlaneInstanceNode);
  path->SetOriginalColorTexture(pOriginalColorTexture);
  path->SetColorShaderTexture(pColorShaderTexture);

	path->SetColorShaderTextureColor( math::Vector4( 0, 0, 1, 0.0 ) );

  m_ColorShaderTextureRenderPaths.push_back(path);
}

void MyRenderingPipeLine::SetColorShaderTextureColor(const gfl2::math::Vector4& color)
{
  for (u32 i = 0; i < m_ColorShaderTextureRenderPaths.size(); ++i)
  {
    m_ColorShaderTextureRenderPaths[i]->SetColorShaderTextureColor(color);
  }
}

void MyRenderingPipeLine::ClearColorShaderPath()
{
	for( u32 i = 0; i < m_ColorShaderTextureRenderPaths.size(); ++i )
	{
		delete m_ColorShaderTextureRenderPaths[i];
	}

	m_ColorShaderTextureRenderPaths.clear();
}

void MyRenderingPipeLine::SetCameraDataToBackgroundPath(
  b32 useCameraData,
  gfl2::math::Matrix44 projection,
  gfl2::math::Matrix34 view)
{
  gfl2::renderingengine::renderer::util::ModelRenderPath::Camera camera;
  camera.m_UseCamera = useCameraData;
  camera.m_Projection = projection;
  camera.m_View = view;
  for (u32 i = 0; i < s_BackgroundFileNum; ++i)
  {
    m_BackgroundRenderPath[i].SetCameraData(camera);
  }
}

MyRenderingPipeLine::~MyRenderingPipeLine()
{
	for( u32 i = 0; i < s_EdgeMapFileNum; ++i )
	{
		delete m_pEdgeMapBinData[i];
	}

	for( u32 i = 0; i < s_OutLineFileNum; ++i )
	{
		delete m_pOutLineBinData[i];
	}

	delete [] m_pColorShaderData;

  if (m_DofRenderTargetPlaneInstanceNode)
  {
    delete m_DofRenderTargetPlaneInstanceNode;
    m_DofRenderTargetPlaneInstanceNode = NULL;
  }

  if (m_pDofModelData)
  {
    delete m_pDofModelData;
    m_pDofModelData = NULL;
  }

  if (m_pDofVertexShaderData)
  {
    delete m_pDofVertexShaderData;
    m_pDofVertexShaderData = NULL;
  }

  if (m_pDofFrontPixelShaderData)
  {
    delete m_pDofFrontPixelShaderData;
    m_pDofFrontPixelShaderData = NULL;
  }

  if (m_pDofBackPixelShaderData)
  {
    delete m_pDofBackPixelShaderData;
    m_pDofBackPixelShaderData = NULL;
  }

  if (m_pDofTextureData)
  {
    delete m_pDofTextureData;
    m_pDofTextureData = NULL;
  }
}

b32 MyRenderingPipeLine::StartRenderPath()
{
	m_RenderPathCnt = 0;
	return true;
}

RenderingPipeLine::IRenderPath* MyRenderingPipeLine::GetRenderPath()
{
	u32			pathNo = m_RenderPathCnt;
	if ( pathNo < m_ColorShaderTextureRenderPaths.size() )
	{
		return m_ColorShaderTextureRenderPaths[pathNo];
	}

  pathNo -= m_ColorShaderTextureRenderPaths.size();

  switch( pathNo )
  {
  case 0:
    return &m_ClearRenderPath;
    break;
  case 1:
    return (m_IsBackgroundVisible) ? &m_BackgroundRenderPath[0] : NULL;
    break;
  case 2:
    return (m_IsBackgroundVisible) ? &m_BackgroundRenderPath[1] : NULL;
    break;
  case 3:
    return (m_IsBackgroundVisible) ? &m_BackgroundRenderPath[2] : NULL;
    break;
  case 4:
    return (m_IsBackgroundVisible) ? &m_BackgroundRenderPath[3] : NULL;
    break;
  case 5:
    return &m_EdgeMapSceneRenderPath;
    break;
  case 6:
    return &m_MainRenderPath;
    break;
  case 7:
    return &m_OutLinePostRenderPath;
    break;
  case 8:
    return &m_StretchBltFrameBufferPath;
    break;
  case 9:
    return &m_DistortionPostRenderPath;
    break;
#if defined(DOF_ENABLE)
  case 10:
    return &m_MakeBlurImagePath;
    break;
  case 11:
    return &m_MakeReductionImagePath;
    break;
  case 12:
    return &m_DofRenderPath;
    break;
#endif
  }
  return NULL;
}

b32 MyRenderingPipeLine::NextRenderPath()
{
	m_RenderPathCnt++;
	if ( m_RenderPathCnt == (13 + m_ColorShaderTextureRenderPaths.size() ) )
		return false;
	return true;
}

void MyRenderingPipeLine::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_EdgeMapSceneRenderPath.AddEdgeRenderingTarget( pNode );
	m_MainRenderPath.AddEdgeRenderingTarget( pNode );
  m_DistortionPostRenderPath.AddDistortionRenderingTarget( pNode );
}

void MyRenderingPipeLine::RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_EdgeMapSceneRenderPath.RemoveEdgeRenderingTarget( pNode );
	m_MainRenderPath.RemoveEdgeRenderingTarget( pNode );
  m_DistortionPostRenderPath.RemoveDistortionRenderingTarget( pNode );
}

void MyRenderingPipeLine::SetDepthTexture( gfl2::gfx::Texture* pTexture )
{
#if defined(GF_PLATFORM_CTR)
	m_OutLinePostRenderPath.SetEdgeMapTexture( EdgeMapSceneRenderPath::EdgeColor::ID, pTexture );	
#endif
}

