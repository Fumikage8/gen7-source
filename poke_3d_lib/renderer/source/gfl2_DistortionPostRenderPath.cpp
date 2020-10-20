//=============================================================================
/**
 * @file    gfl2_DistortionPostRenderPath.cpp
 * @brief   DistortionPostRenderPathクラス（ゆがみ屈折シェーダー用描画パス）
 * @author  ishiguro_masateru
 * @date    2015/09/29
 */
//=============================================================================
#include <renderer/include/gfl2_DistortionPostRenderPath.h>

#include <gfx/include/gfl2_GFGLUtil.h>
#include <debug/include/gfl2_Assert.h>

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
#else
	#include <gfx/include/WinOpenGL/gfl2_WinOpenGLStateObject.h>
#endif

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;


namespace poke_3d { namespace renderer {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
DistortionPostRenderPath::DistortionPostShaderDriver::DistortionPostShaderDriver():
  m_pConstantBuffer( NULL )
{
  
}

DistortionPostRenderPath::DistortionPostShaderDriver::~DistortionPostShaderDriver()
{
  GFX_SAFE_DELETE( m_pConstantBuffer );
}

void DistortionPostRenderPath::DistortionPostShaderDriver::Initialize( gfl2::gfx::IGLAllocator* pAllocator )
{
  {
		ConstantBufferElement element[] = {
			{ ConstantType::Vector, 1 },
			ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
		};

		m_pConstantBuffer = GFGL::CreateConstantBuffer( pAllocator, element, Usage::WriteOnly, Pool::Default );
	}
}

b32 DistortionPostRenderPath::DistortionPostShaderDriver::Apply( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag ) const
{
	pMeshDrawTag->GetShaderDriver()->Apply( rRenderState, pMeshDrawTag );

  const scenegraph::instance::MaterialInstanceNode						*pMaterialInstanceNode = pMeshDrawTag->GetMaterialInstanceNode();
	const scenegraph::resource::MaterialResourceNode						*pMaterialResourceNode = pMaterialInstanceNode->GetMaterialResourceNode();
	const scenegraph::resource::MaterialResourceNode::UserData	&rUserData = pMaterialResourceNode->GetUserData();

	if ( (rUserData.m_VertexShaderType != scenegraph::resource::VertexShaderType::MegaYamirami) &&
		(rUserData.m_VertexShaderType != scenegraph::resource::VertexShaderType::Reflect) )
	{
    return true;
	}

  const gfx::Surface::SurfaceDesc			&rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();

  f32   width = m_pFrameBufferTexture->GetTextureDesc().width;
  f32   height = m_pFrameBufferTexture->GetTextureDesc().height;
  f32   widthMax = (static_cast<f32>(rDesc.width-1)) / width;
  f32   heightMax = (static_cast<f32>(rDesc.height-1)) / height;

#if defined(GF_PLATFORM_CTR)
  {
    static const	u32			s_DirLightColorReg = 84;
		static const	u32			s_AmbLightColorReg = 85;

    nn::gr::CommandBufferJumpHelper					*pCommandBuffer = GFGL::GetJumpHelper();
		bit32									                  *cmd = pCommandBuffer->GetCurrentPtr();
		math::Vector4					                  shaderParam0( widthMax, heightMax, 0.0f, 0.0f );
    //math::Vector4					                  shaderParam1( rUserData.m_ShaderParam0, scaleWidth, scaleHeight, 0.0f );
		math::Vector4					                  shaderParam1( rUserData.m_ShaderParam0, 1.0f, 1.0f, 0.0f );

    cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, s_DirLightColorReg, *(nn::math::VEC4*)&shaderParam0 );
		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, s_AmbLightColorReg, *(nn::math::VEC4*)&shaderParam1 );

		pCommandBuffer->Put(cmd);
  }

#elif defined (GF_PLATFORM_WIN32)
  if ( m_pConstantBuffer )
  {
    math::Vector				vsVectorConstants;
    f32   scaleWidth = (static_cast<f32>(rDesc.width)) / width;
    f32   scaleHeight = (static_cast<f32>(rDesc.height)) / height;

    vsVectorConstants.Set( scaleWidth, scaleHeight, widthMax, heightMax );
	  
	  m_pConstantBuffer->Update(&vsVectorConstants, sizeof(vsVectorConstants));
	  GFGL::SetVertexShaderConstant( 5, m_pConstantBuffer );
  }

#endif

  return true;
}

void DistortionPostRenderPath::DistortionPostShaderDriver::SetTextureOverride( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag ) const
{
	const scenegraph::instance::MaterialInstanceNode						*pMaterialInstanceNode = pMeshDrawTag->GetMaterialInstanceNode();
	const scenegraph::resource::MaterialResourceNode						*pMaterialResourceNode = pMaterialInstanceNode->GetMaterialResourceNode();
	const scenegraph::resource::MaterialResourceNode::UserData	&rUserData = pMaterialResourceNode->GetUserData();

	if ( rUserData.m_VertexShaderType != scenegraph::resource::VertexShaderType::MegaYamirami &&
		rUserData.m_VertexShaderType != scenegraph::resource::VertexShaderType::Reflect )
	{
    return;
	}

#if defined(GF_PLATFORM_CTR)
	{
		u32                   textureCommand[10];

		const gfl2::gfx::ctr::CTRTexture					*pCTRTexture = reinterpret_cast<const gfl2::gfx::ctr::CTRTexture*>(m_pFrameBufferTexture);
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
		textureCommand[2] = PICA_CMD_DATA_TEXTURE_SIZE( rTextureDesc.height, rTextureDesc.width );
		textureCommand[3] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_SIZE );

		// 0x085
		textureCommand[4] = PICA_CMD_DATA_TEXTURE_ADDR( pCTRTexture->GetPhysicalAddr() );
		textureCommand[5] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR1 );

		// 0x08e
		textureCommand[6] = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( pCTRTexture->GetFormat() );
		textureCommand[7] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_FORMAT );

		textureCommand[8] = 0;
		textureCommand[9] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_LOD );
				
		GFGL::AddCommandCopy( textureCommand, sizeof(textureCommand) );//@note 直接書き込みたい
	}
#else
  GFGL::SetTexture( 0, m_pFrameBufferTexture );
#endif
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
DistortionPostRenderPath::DistortionPostRenderPath() : RenderingPipeLine::SceneRenderPath()
{
}

DistortionPostRenderPath::DistortionPostRenderPath( gfl2::gfx::IGLAllocator* pAllocator, int mdlNum ) : RenderingPipeLine::SceneRenderPath()
{
  m_DrawableNodeContainer.CreateBuffer( mdlNum, pAllocator);
}

void DistortionPostRenderPath::CreateBuffer(gfl2::gfx::IGLAllocator* pAllocator, int mdlNum)
{
  m_DrawableNodeContainer.CreateBuffer( mdlNum, pAllocator);
  m_DistortionPostShaderDriver.Initialize( pAllocator );
}

DistortionPostRenderPath::~DistortionPostRenderPath()
{
}

void DistortionPostRenderPath::AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{
	m_pDrawManager->AddDrawEnv( pNode );
}

void DistortionPostRenderPath::RemoveDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{
	m_pDrawManager->RemoveDrawEnv( pNode );
}

void DistortionPostRenderPath::RemoveAllDrawEnv()
{
	m_pDrawManager->RemoveAllDrawEnv();
}

const gfl2::renderingengine::renderer::IShaderDriver* DistortionPostRenderPath::ShaderOverride()
{
	return &m_DistortionPostShaderDriver;
}

const gfl2::gfx::GLFixedSizeContainer<scenegraph::instance::DrawableNode*>* DistortionPostRenderPath::DrawableNodeOverride()
{
	return &m_DrawableNodeContainer;
}

void DistortionPostRenderPath::AddDistortionRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  if ( m_DrawableNodeContainer.free_size() )
  {
    m_DrawableNodeContainer.push_back( pNode );
  }
}

void DistortionPostRenderPath::RemoveDistortionRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.remove( pNode );
}

void DistortionPostRenderPath::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  if (m_DrawableNodeContainer.size() == 0)
  {
    return;
  }

	m_pDrawManager->DrawLayerDisable( scenegraph::resource::DrawLayer::Layer0 );
	m_pDrawManager->DrawLayerDisable( scenegraph::resource::DrawLayer::Layer1 );
	m_pDrawManager->DrawLayerDisable( scenegraph::resource::DrawLayer::Layer2 );
	m_pDrawManager->DrawLayerDisable( scenegraph::resource::DrawLayer::Layer3 );
	m_pDrawManager->DrawLayerEnable( scenegraph::resource::DrawLayer::Layer4 );
	m_pDrawManager->DrawLayerEnable( scenegraph::resource::DrawLayer::Layer5 );
	m_pDrawManager->DrawLayerEnable( scenegraph::resource::DrawLayer::Layer6 );
	m_pDrawManager->DrawLayerEnable( scenegraph::resource::DrawLayer::Layer7 );

	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
}

void DistortionPostRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  if (m_DrawableNodeContainer.size() == 0)
  {
    return;
  }

  SetupDraw(rDrawContext);
  DrawManagerDraw(false);
}

void DistortionPostRenderPath::CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
  this->Execute(rDrawContext);
}

}}
