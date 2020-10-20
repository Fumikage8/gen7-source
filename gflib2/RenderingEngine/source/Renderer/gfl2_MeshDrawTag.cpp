//=============================================================================
/**
 * @file    gfl2_MeshDrawTag.cpp
 * @brief   MeshDrawTagクラス（Mesh用DrawTagクラス。）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.5.26   ishiguro  [fix]NMCat[2881]:同じテクスチャでもMipLevelが異なる場合があったのでMipLevelコマンドは常に発行します。
 *
 */
//=============================================================================
#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <gfx/include/gfl2_DrawUtil.h>
#include <debug/include/gfl2_Assert.h>

#if defined(NN_PLATFORM_CTR)
	#include <nn/gx/CTR/gx_MacroTexture.h>
#endif

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace renderer { 

u32				MeshDrawTag::s_TextureCache[scenegraph::instance::MaterialInstanceNode::MATERIAL_TEXTURE_MAX];

/*MeshDrawTag* MeshDrawTag::Create()
{
  return Create(gfx::GLMemory::GetAllocator());
}*/

MeshDrawTag* MeshDrawTag::Create( gfx::IGLAllocator* pAllocator )
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
	MeshDrawTag* instance = new MeshDrawTag();
  gfx::GLMemory::ClearInstanceAllocator();
  return instance;
}


/*MeshDrawTag* MeshDrawTag::CreateArray( u32 arrayNum )
{
	return new MeshDrawTag[arrayNum];
}*/

MeshDrawTag* MeshDrawTag::CreateArray( gfx::IGLAllocator* pAllocator, u32 arrayNum )
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
	MeshDrawTag* instance = new MeshDrawTag[arrayNum];
  gfx::GLMemory::ClearInstanceAllocator();
  return instance;
}

MeshDrawTag::MeshDrawTag() : 
	gfx::GLMemory(),
	DrawTag(),
	m_JointInstanceNodeList(NULL),
	m_JointNum( 0 )
{
	for( u32 i = 0; i < scenegraph::instance::MaterialInstanceNode::MATERIAL_TEXTURE_MAX; ++i )
	{
		m_pTextureList[ i ] = NULL;
		m_pSamplerList[ i ] = NULL;
	}

	for( u32 i = 0; i < Constant::VERTEX_BUFFER_MAX; ++i )
	{
		m_pVertexBuffer[i] = NULL;
	}

#if defined(GF_PLATFORM_WIN32)
  m_pOcclusionQuery = NULL;
  m_DummySamplerStateObject = GFGL::CreateSamplerStateObject(this->GetInstanceAllocator());
	m_DummySamplerStateObject->UpdateState();
#endif
}

MeshDrawTag::~MeshDrawTag()
{
#if defined(GF_PLATFORM_WIN32)
	GFX_SAFE_DELETE( m_DummySamplerStateObject );
#endif
}

void MeshDrawTag::SetMaterialInstanceNode( const scenegraph::instance::MaterialInstanceNode* pMaterial )
{
	GFL_ASSERT(pMaterial);
	m_pMaterialInstanceNode = pMaterial;

  m_DrawLayerNo = m_pMaterialInstanceNode->GetMaterialResourceNode()->GetBlendTest().m_DrawLayer;
	m_LightSetNo = m_pMaterialInstanceNode->GetMaterialResourceNode()->GetAttributeParam().m_LightSetNo;
}

void MeshDrawTag::Draw( RenderState& rRenderState )
{
#if defined(GF_PLATFORM_WIN32)
	if ( m_pOcclusionQuery )
		GFGL::BeginConditionalRender( m_pOcclusionQuery );
#endif

	rRenderState.SetWorldMatrix( this->GetWorldMatrix() );
	rRenderState.SetJointInstanceNode( m_JointInstanceNodeList );
	rRenderState.SetJointNum( m_JointNum );

	const IShaderDriver		*pShader = rRenderState.GetOverrideShader();
	b32										fDrawEnable = false;

	{
		if ( pShader )
			fDrawEnable = pShader->Apply( rRenderState, this );
		else
			fDrawEnable = m_pShaderDriver->Apply( rRenderState, this );
	}

	if ( fDrawEnable == false )
		return;

#if defined(GF_PLATFORM_CTR)

	{
		static const u32																			sizeTable[] = { PICA_REG_TEXTURE0_SIZE, PICA_REG_TEXTURE1_SIZE, PICA_REG_TEXTURE2_SIZE };
		static const u32																			addrTable[] = { PICA_REG_TEXTURE0_ADDR1, PICA_REG_TEXTURE1_ADDR, PICA_REG_TEXTURE2_ADDR };
		static const u32																			formatTable[] = { PICA_REG_TEXTURE0_FORMAT, PICA_REG_TEXTURE1_FORMAT, PICA_REG_TEXTURE2_FORMAT };
		static const u32																			wrapTable[] = { PICA_REG_TEXTURE0_WRAP_FILTER, PICA_REG_TEXTURE1_WRAP_FILTER, PICA_REG_TEXTURE2_WRAP_FILTER };
		static const u32																			samplerTable[] = { PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D, PICA_DATA_TEXTURE1_SAMPLER_TYPE_TEXTURE_2D, PICA_DATA_TEXTURE2_SAMPLER_TYPE_TEXTURE_2D };
		static const u32																			lodTable[] = { PICA_REG_TEXTURE0_LOD, PICA_REG_TEXTURE1_LOD, PICA_REG_TEXTURE2_LOD };
		const scenegraph::resource::MaterialResourceNode			*pMaterialResourceNode = m_pMaterialInstanceNode->GetMaterialResourceNode();
		u32																										dataVersionId = pMaterialResourceNode->GetVersionId();
		RenderState::ShaderStateMap														shaderStateMap = rRenderState.GetShaderStateMap();
		b32																										fTextureSetUp = false;

    nn::gr::CommandBufferJumpHelper* pJumpHelper = GFGL::GetJumpHelper();
    u32* pCommand = pJumpHelper->GetCurrentPtr();
		u32				textureCnt = 0;

		for( u32 i = 0; i < scenegraph::instance::MaterialInstanceNode::MATERIAL_TEXTURE_MAX; ++i )
		{
			u32				textureAddr = 0;

			if ( m_pTextureList[i] )
			{
				const gfl2::gfx::ctr::CTRTexture					                      *pCTRTexture = reinterpret_cast<const gfl2::gfx::ctr::CTRTexture*>(m_pTextureList[i]);
				const gfl2::gfx::Texture::TextureDesc			                      &rTextureDesc = pCTRTexture->GetTextureDesc();
				u32																				                      mappingType = 0;
        const scenegraph::resource::MaterialResourceNode::TextureInfo		&rTexInfo = pMaterialResourceNode->GetTextureInfo( textureCnt );

				textureAddr = pCTRTexture->GetPhysicalAddr();

        {//同じテクスチャでもフィルタ設定やMipLevelは変えることがあるので外に出す。
          if ( dataVersionId >= scenegraph::resource::GfModelVersion::AddWrapFilerState )
          {//このif分をなくす時に、モデルバイナリよりコマンドを削ってしまおう
            if ( dataVersionId >= scenegraph::resource::GfModelVersion::VertexShaderUpdate15032914 )
            {
              mappingType = rTexInfo.m_Attribute.m_MappingType;
            }

            *pCommand++ = PICA_CMD_DATA_TEXTURE_WRAP_FILTER(  rTexInfo.m_Attribute.m_MagFilter, rTexInfo.m_Attribute.m_MinFilter,
                                                              pCTRTexture->GetFormat(),
                                                              rTexInfo.m_Attribute.m_RepeatV, rTexInfo.m_Attribute.m_RepeatU,
                                                              0, samplerTable[i] );

            *pCommand++ = PICA_CMD_HEADER_SINGLE( wrapTable[i] );
          }

          {// レジスタ0x084への設定。（ミップレベル設定レジスタへの設定）
            if ( rTextureDesc.mipLevels > 1 )
            {//テクスチャがミップマップの場合
              *pCommand++ = PICA_CMD_DATA_TEXTURE_LOD_LEVEL( nn::gr::CTR::Float32ToFix13Fraction8( rTexInfo.m_Attribute.m_MipBias ), rTextureDesc.mipLevels - 1, 0 );
            }
            else
            {//テクスチャがミップでない場合
              *pCommand++ = 0;//ミップレベル、バイアスを０に設定。（ビットを０で埋める）
            }
            *pCommand++ = PICA_CMD_HEADER_SINGLE( lodTable[i] );
          }
        }

				if ( s_TextureCache[i] != textureAddr )
				{
					//デフォルトではTexture0に設定するコマンド
					// 0x082
					*pCommand++ = PICA_CMD_DATA_TEXTURE_SIZE( rTextureDesc.width, rTextureDesc.height );
					*pCommand++ = PICA_CMD_HEADER_SINGLE( sizeTable[i] );

					// 0x085
					*pCommand++ = PICA_CMD_DATA_TEXTURE_ADDR( textureAddr );
					*pCommand++ = PICA_CMD_HEADER_SINGLE( addrTable[i] );

					// 0x08e
					*pCommand++ = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( pCTRTexture->GetFormat() );
					*pCommand++ = PICA_CMD_HEADER_SINGLE( formatTable[i] );

					switch(i){
					case 0:
						shaderStateMap.m_Uv0Enable = (mappingType == 0) ? true : false;
						break;
					case 1:
						shaderStateMap.m_Uv1Enable = (mappingType == 0) ? true : false;
						shaderStateMap.m_Texture1Enable = (mappingType == 2) ? true : false;
						break;
					case 2:
						shaderStateMap.m_Uv2Enable = (mappingType == 0) ? true : false;
						shaderStateMap.m_Texture2Enable = (mappingType == 2) ? true : false;
						break;
					default:
						break;
					}
					fTextureSetUp = true;
				}
				++textureCnt;
			}

			s_TextureCache[i] = textureAddr;
		}

		if ( fTextureSetUp )
		{//テクスチャキャッシュクリア
			*pCommand++ = 1 << 16;
			*pCommand++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_TEXTURE_FUNC, 0x4 );
		}

		rRenderState.SetShaderStateMap( shaderStateMap );
    pJumpHelper->Put(pCommand);

		if ( pShader )
			pShader->SetTextureOverride( rRenderState, this );
	}

	if ( fDrawEnable )
	{
		{//頂点シェーダーBoolレジスタ設定
			nn::gr::CommandBufferJumpHelper* pJumpHelper = GFGL::GetJumpHelper();
			u32* cmd = pJumpHelper->GetCurrentPtr();

			
			RenderState::ShaderStateMap		map = rRenderState.GetShaderStateMap();

			map.m_WeightWEnable = ( m_pFaceData->m_JointWeightMax == 4 ) ? true : false;

			*cmd++ = 0x7fff0000 | map.value;
			*cmd++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_BOOL ); // 0x2b0

			pJumpHelper->Put(cmd);
		}
		
		b32 fDrawOverride = false;

		if ( pShader )
			fDrawOverride = pShader->SetPolygonDrawOverride( rRenderState, this );

		if ( fDrawOverride == false )
		{
			GFGL::AddCommandJump( reinterpret_cast<const u32*>( m_pFaceData->GetCommand( scenegraph::resource::MeshResourceNode::FaceData::CommandType::VertexAttrEnable ) ),
													m_pFaceData->GetCommandSize( scenegraph::resource::MeshResourceNode::FaceData::CommandType::VertexAttrEnable ) );

			GFGL::AddCommandJump( reinterpret_cast<const u32*>( m_pFaceData->GetCommand( scenegraph::resource::MeshResourceNode::FaceData::CommandType::Draw ) ),
														m_pFaceData->GetCommandSize( scenegraph::resource::MeshResourceNode::FaceData::CommandType::Draw ) );

			GFGL::AddCommandJump( reinterpret_cast<const u32*>( m_pFaceData->GetCommand( scenegraph::resource::MeshResourceNode::FaceData::CommandType::VertexAttrDisable ) ),
														m_pFaceData->GetCommandSize( scenegraph::resource::MeshResourceNode::FaceData::CommandType::VertexAttrDisable ) );

      // フレームバッファキャッシュのクリア
      nn::gr::CommandBufferJumpHelper* pJumpHelper = GFGL::GetJumpHelper();
      u32* command = pJumpHelper->GetCurrentPtr();
      command = nn::gr::FrameBuffer::MakeClearCacheCommand(command);
      pJumpHelper->Put(command);
		}
	}
	
#elif defined(GF_PLATFORM_WIN32)

	for( u32 i = 0; i < Constant::VERTEX_BUFFER_MAX; ++i )
	{
		if ( m_pVertexBuffer[i] )
		{
			rRenderState.SetBlendMeshWeight( i, m_VertexBlendWeight[i] );
			GFGL::SetStreamSource( i, m_pVertexBuffer[i] );
		}
		else
		{
			rRenderState.SetBlendMeshWeight( i, 0.0f );
		}
	}
	GFGL::SetIndices( m_pIndexBuffer );

	{
		for( u32 i = 0; i < scenegraph::instance::MaterialInstanceNode::MATERIAL_TEXTURE_MAX; ++i )
		{
			if ( m_pTextureList[i] && m_pSamplerList[i] )
			{// テクスチャとサンプラは対でセットする
				GFGL::SetTexture( i, m_pTextureList[i] );
				GFGL::SetSamplerState( i, m_pSamplerList[i] );
			}
			else
			{
				GFGL::SetTexture( i, NULL );
				GFGL::SetSamplerState( i, m_DummySamplerStateObject );
			}
		}

		if ( pShader )
			pShader->SetTextureOverride( rRenderState, this );
	}

  glDisable( GL_DITHER );

	if ( fDrawEnable )
		GFGL::DrawIndexedPrimitive( PrimitiveType::Triangles, 0, m_pIndexBuffer->GetIndexNum() );

  if ( m_pOcclusionQuery )
		GFGL::EndConditionalRender(m_pOcclusionQuery);
#endif
}

void MeshDrawTag::SetJointInstanceNode( const scenegraph::instance::JointInstanceNode* pNode, u32 size )
{
	m_JointInstanceNodeList = pNode;
	m_JointNum = size;
}

}}}

