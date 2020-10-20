//=============================================================================
/**
 * @file    gfl2_EdgeMapSceneRenderPath.cpp
 * @brief   EdgeMapSceneRenderPathクラス（アウトラインを生成するための情報をオフスクリーンバッファにレンダリングする描画パス）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.5.20   ishiguro  [fix]NMCat[2430]:ステンシルテスト無効設定の重複設定不具合修正 
 *  2016.5.17   ishiguro  [fix]NMCat[2014]:ステンシルテスト無効設定漏れ修正
 *  2016.4.9    ishiguro  エッジタイプがOwnColor設定時にＩＤ書き込みを無効にします。
 *
 */
//=============================================================================
#include <renderer/include/gfl2_EdgeMapSceneRenderPath.h>

#include <math/include/gfl2_MathCommon.h>
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
#include <RenderingEngine/include/renderer/gfl2_CombinerCtrShaderDriver.h>
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
			GFL_ASSERT(0);
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

namespace poke_3d { namespace renderer {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
EdgeMapSceneRenderPath::EdgeMapShaderDriver::EdgeMapShaderDriver() :
  GLMemory(),
  IShaderDriver(),
  m_pVertexShader(NULL),
  m_pPixelShader(NULL),
  m_pConstantBuffer(NULL),
  m_pBinModelRootNode(NULL),
  m_pModelInstanceNode(NULL),
  m_pZInvTextureResourceNode(NULL)
{
  for (u32 i = 0; i < EdgeTarget::Max; ++i)
  {
    m_pBlendStateObject[i] = NULL;
  }
}

void EdgeMapSceneRenderPath::EdgeMapShaderDriver::Initialize(
  gfl2::gfx::IGLAllocator* pAllocator, 
  c8* gfbmdlBinary,
  c8** shaderBinaries,
  s32 shaderNum,
  c8** textureBinaries,
  s32 texNum)
{
	{
		scenegraph::resource::GfBinaryMdlData							resourceData;

		resourceData.SetModelData( gfbmdlBinary );

		m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &resourceData );	
	}

	{
		for( s32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
		{
			scenegraph::resource::GfBinaryShdData	resourceData;
			resourceData.SetShaderData( shaderBinaries[shaderNo] );
			scenegraph::resource::ResourceNode		*pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &resourceData );
			m_pBinModelRootNode->AddChild( pBinShdNode );
		}
	}

	{
		m_pZInvTextureResourceNode = NULL;
		for( s32 texNo = 0; texNo < texNum; ++texNo )
		{
			scenegraph::resource::GfBinaryTexData							texResourceData;
			scenegraph::resource::ResourceNode								*pBinTexRootNode;

			texResourceData.SetTextureData( textureBinaries[texNo] );

			//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
			pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &texResourceData );
			m_pBinModelRootNode->AddChild( pBinTexRootNode );

			if ( texNo == 1 )
			{
				m_pZInvTextureResourceNode = pBinTexRootNode->GetChild(0)->SafeCast<scenegraph::resource::TextureResourceNode>();
			}
		}
	}

	{
		m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( pAllocator, m_pBinModelRootNode );
	}

  scenegraph::instance::DrawableNode::UpdateDrawTagInfo updateDrawTagInfo;
  m_pModelInstanceNode->UpdateDrawTag(updateDrawTagInfo);
  scenegraph::SceneGraphManager::RemoveChild(m_pModelInstanceNode);

#if defined GF_PLATFORM_WIN32

	{
		ConstantBufferElement element[] = {
			{ ConstantType::Vector, 1 },
			{ ConstantType::Vector, 1 },
			{ ConstantType::Vector, 1 },
			ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
		};

		m_pConstantBuffer = GFGL::CreateConstantBuffer( pAllocator, element, Usage::WriteOnly, Pool::Default );
	}
  for( u32 i = 0; i < EdgeTarget::Max; ++i )
	{
		m_pBlendStateObject[i] = GFGL::CreateBlendStateObject(pAllocator);
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
#elif defined(GF_PLATFORM_CTR)
	for( u32 i = 0; i < 3; ++i )
	{
		m_pBlendStateObject[i] = GFGL::CreateBlendStateObject(pAllocator);
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

		m_pConstantBuffer = GFGL::CreateConstantBuffer( pAllocator, element, Usage::WriteOnly, Pool::Default );
	}
#endif
}

EdgeMapSceneRenderPath::EdgeMapShaderDriver::~EdgeMapShaderDriver()
{
  for( u32 i = 0; i < EdgeTarget::Max; ++i )
  {
    GFX_SAFE_DELETE( m_pBlendStateObject[i] );
  }
  GFX_SAFE_DELETE( m_pConstantBuffer );
	GFX_SAFE_DELETE( m_pModelInstanceNode );
	GFX_SAFE_DELETE( m_pBinModelRootNode );
}

b32 EdgeMapSceneRenderPath::EdgeMapShaderDriver::Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
	RenderState::SetUpEnable																			origSetUpEnable = rRenderState.GetSetUpEnable();
	RenderState::SetUpEnable																			setUpEnable = origSetUpEnable;
#if defined(GF_PLATFORM_CTR)
	MeshDrawTag																										*pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );
#endif
  const scenegraph::instance::MaterialInstanceNode							*pMaterialInstanceNode = pMeshDrawTag->GetMaterialInstanceNode();
	const scenegraph::resource::MaterialResourceNode::UserData		&rUserData = pMaterialInstanceNode->GetUserData();

#if defined GF_PLATFORM_WIN32

	if ( rUserData.m_IDEdgeEnable == false && rUserData.m_EdgeType == scenegraph::resource::EdgeType::None )
		return false;

  {
    math::Vector				psVectorConstants;

	  psVectorConstants.x = static_cast<f32>( rUserData.m_EdgeID );
	  psVectorConstants.x /= 255.0f;

	  m_pConstantBuffer->Update(&psVectorConstants, sizeof(psVectorConstants));
	  GFGL::SetPixelShaderConstant( 6, m_pConstantBuffer );
  }

	if ( rUserData.m_EdgeType == scenegraph::resource::EdgeType::OwnColor )
	{
		pMeshDrawTag->GetShaderDriver()->Apply( rRenderState, pMeshDrawTag );

		gfx::BlendStateObject*					pBlendStateObject = pMaterialInstanceNode->GetBlendStateObject();

		if ( pBlendStateObject->GetBlendEnable() && (rUserData.m_IDEdgeEnable == false) )
		{//[fix]メガゲンガー特殊設定対応。（マスク用箱の対応です）ToDo #7910 094メガゲンガ―について、体の下にある立方体に輪郭線が出ている
		 //ブレンド設定も独自設定を使用。ここに入ってくるのは、メガゲンガーのみ。
		 //かつGF_PLATFORM_WIN32でくくられているので製品コードに影響なし 2016/03/13
			GFGL::SetBlendState( pBlendStateObject );
		}
		else
		{
			if ( rUserData.m_IDEdgeEnable )
				GFGL::SetBlendState( m_pBlendStateObject[EdgeTarget::Both] );
			else
				GFGL::SetBlendState( m_pBlendStateObject[EdgeTarget::Normal] );
		}

		return true;
	}

	{
		MeshDrawTag						*pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );
		const IShaderDriver		*pShaderDriver = pTag->GetShaderDriver();

		scenegraph::resource::MaterialResourceNode::AttributeParam			*pAttr = m_pModelInstanceNode->GetMaterialInstanceNode( 0 )->GetAttributeParam();

		switch( rUserData.m_EdgeType ){
		case scenegraph::resource::EdgeType::None:
			break;
		case scenegraph::resource::EdgeType::Erase:
			pAttr->m_ConstantColor[1].a = 0;//法線出力オフ
			pAttr->m_ConstantColor[2].a = 0;//頂点カラー出力オフ
			break;
		case scenegraph::resource::EdgeType::VColor:
			pAttr->m_ConstantColor[1].a = 0;//法線出力オフ
			pAttr->m_ConstantColor[2].a = 255;//頂点カラー出力オン
			break;
		default:
			pAttr->m_ConstantColor[1].a = 255;//法線出力オン
			pAttr->m_ConstantColor[2].a = 255;//頂点カラー出力オン
			break;
		}

		switch( rUserData.m_EdgeMapAlphaMask ){
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::None:
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture0A:
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture1A:
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture2A:
			pAttr->m_ConstantColor[3].a = 255;//抜きにα成分を用いる
			break;
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture0R:
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture1R:
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture2R:
			pAttr->m_ConstantColor[3].a = 255;//抜きにＲ成分を用いる
			break;
		}

		static const s32			textureSlotNos[scenegraph::resource::CombinerAttribute::EdgeMapAlpha::NumberOf] = 
		{
				0,//Texture0 = 0,
				1,//Texture1,
				2,//Texture2,
				0,//Texture0R,
				1,//Texture1R,
				2//Texture2R,
		};

		if ( rUserData.m_EdgeMapAlphaMask != scenegraph::resource::CombinerAttribute::EdgeMapAlpha::None ) 
		{
			u32				slotNo = textureSlotNos[rUserData.m_EdgeMapAlphaMask];

			for( u32 i = 0; i < pMaterialInstanceNode->GetTextureNum(); ++i )
			{
				const scenegraph::resource::MaterialResourceNode::TextureInfo *pInfo = pMaterialInstanceNode->GetTextureInfo( i );

				if ( pInfo->m_SlotNo == slotNo )
				{
					rRenderState.SetTextureInfoOverride( 1, pInfo );
				}
			}
		}
		
		{
			pShaderDriver->Apply( rRenderState, pMeshDrawTag );

      u32					maskType = EdgeTarget::Both;

      if ( rUserData.m_EdgeType == scenegraph::resource::EdgeType::None )			maskType = EdgeTarget::ID;//ＩＤエッジのみ
      if ( rUserData.m_IDEdgeEnable == false)																	maskType = EdgeTarget::Normal;//法泉エッジのみ

			GFGL::SetBlendState( m_pBlendStateObject[maskType] );
		}
	}
	
	GFGL::SetRasterizerState( pMaterialInstanceNode->GetRasterizerStateObject() );
	GFGL::SetDepthStencilState( pMaterialInstanceNode->GetDepthStencilStateObject() );

#elif defined(GF_PLATFORM_CTR)

	CombinerCtrShaderDriver		*pShaderDriver = (CombinerCtrShaderDriver*)( pTag->GetShaderDriver() );

  if ( rUserData.m_EdgeType == scenegraph::resource::EdgeType::None )
  {
    return false;
  }

  if ( rUserData.m_EdgeType == scenegraph::resource::EdgeType::OwnColor )
  {
    scenegraph::instance::MaterialInstanceNode    *pMaterial = const_cast<scenegraph::instance::MaterialInstanceNode*>( pMaterialInstanceNode );
    gfl2::gfx::DepthStencilStateObject            *pDepthStencilStateObject = pMaterial->GetDepthStencilStateObject();
    StencilOp                                     stencilFailOp = pDepthStencilStateObject->GetStencilFailOp(PolygonFace::CCW);
    StencilOp                                     stencilDepthFailOp = pDepthStencilStateObject->GetStencilDepthFailOp(PolygonFace::CCW);

    pMeshDrawTag->GetShaderDriver()->Apply( rRenderState, pMeshDrawTag );

    {
      static const PicaDataStencilOp sc_opList[] =
      {
        PICA_DATA_STENCIL_OP_KEEP,        // Keep = 0,		  //!< 更新しない
        PICA_DATA_STENCIL_OP_ZERO,        // Zero,					//!< ０に設定
        PICA_DATA_STENCIL_OP_REPLACE,     // Replace,			//!< 参照値で置き換える
        PICA_DATA_STENCIL_OP_INCR,        // Increment,		//!< 最大値に達するまで増加
        PICA_DATA_STENCIL_OP_INCR_WRAP,   // IncrementWrap,//!< 増加させその値が最大値を超えると０に戻します。
        PICA_DATA_STENCIL_OP_DECR,        // Decrement,		//!< ０になるまで減少
        PICA_DATA_STENCIL_OP_DECR_WRAP,   // DecrementWrap,//!< 減少させその値が０より小さくなった場合は最大値に戻します。
        PICA_DATA_STENCIL_OP_INVERT,      // Invert,				//!< ビットを反転
      };
      static const u32 BE = 0x3;  // 0011

      //ＩＤ書き込み(ステンシル書き込み)を無効にします。
      //ＩＤ書き込みはここでは行ってはいけません。奥のモデルが先に書き込まれるとＩＤを書き込まないモデルが手前に来た際にアウトラインが透けてしまいます。
      //kujira, sangoの時より、ＩＤ書き込みは、EdgeMap描画にて深度バッファが出来た状態で、それをマスクとして描画していました。
      nn::gr::CommandBufferJumpHelper   *pCommandBuffer = GFGL::GetJumpHelper();
      bit32                             *command = pCommandBuffer->GetCurrentPtr();

      *command++ = PICA_CMD_DATA_STENCIL_OP( sc_opList[stencilFailOp], sc_opList[stencilDepthFailOp], sc_opList[StencilOp::Keep] );
      *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_STENCIL_OP, BE );

      pCommandBuffer->Put(command);
    }
    
    {//ＩＤ書き込み(ステンシル書き込み)を有効にします。
      //メガゲンガーのmegagangar_Maskマテリアルの為の設定です。
      //この処理に入ってくる他のマテリアルは全て、Replace設定になっています。
      //megagangar_MaskのみKeppになっており、kujiraの頃より、この処理で描画実行時にReplaceに置き換わっていました。
      //本来であれば、リソースを修正すべきですが、アッパー修正とさせていただきます。
      pDepthStencilStateObject->SetStencilOp( PolygonFace::CCW, stencilFailOp, stencilDepthFailOp, StencilOp::Replace );
      pDepthStencilStateObject->UpdateState();
    }

    return true;
  }

	{
		setUpEnable.VertexShader = false;
		setUpEnable.GeometryShader = false;
		setUpEnable.FragmentShader = false;
		setUpEnable.UvSetting = false;
		rRenderState.SetSetUpEnable( setUpEnable );

		pMeshDrawTag->GetShaderDriver()->Apply( rRenderState, pMeshDrawTag );
	}

	scenegraph::resource::MaterialResourceNode::AttributeParam			*pAttr = m_pModelInstanceNode->GetMaterialInstanceNode( 0 )->GetAttributeParam();

	switch( rUserData.m_EdgeType ){
	case scenegraph::resource::EdgeType::None:
		break;
	case scenegraph::resource::EdgeType::Erase:
		pAttr->m_ConstantColor[1].a = 0;//法線出力オフ
		pAttr->m_ConstantColor[2].a = 0;//頂点カラー出力オフ
		break;
	case scenegraph::resource::EdgeType::VColor:
    pAttr->m_ConstantColor[1].a = 0;//法線出力オフ
		pAttr->m_ConstantColor[2].a = 255;//頂点カラー出力オン
		//return false;
		break;
	default:
		pAttr->m_ConstantColor[1].a = 255;//法線出力オン
		pAttr->m_ConstantColor[2].a = 255;//頂点カラー出力オン
		break;
	}

	switch( rUserData.m_EdgeMapAlphaMask ){
	case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::None:
	case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture0A:
	case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture1A:
	case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture2A:
		pAttr->m_ConstantColor[3].a = 255;//抜きにα成分を用いる
		break;
	case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture0R:
	case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture1R:
	case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture2R:
		pAttr->m_ConstantColor[3].a = 0;//抜きにＲ成分を用いる
		break;
	}

	static const s32			textureSlotNos[scenegraph::resource::CombinerAttribute::EdgeMapAlpha::NumberOf] = 
	{
			0,//Texture0 = 0,
			1,//Texture1,
			2,//Texture2,
			0,//Texture0R,
			1,//Texture1R,
			2//Texture2R,
	};

	if ( rUserData.m_EdgeMapAlphaMask != scenegraph::resource::CombinerAttribute::EdgeMapAlpha::None ) 
	{
		u32				slotNo = textureSlotNos[rUserData.m_EdgeMapAlphaMask];

		for( u32 i = 0; i < pMaterialInstanceNode->GetTextureNum(); ++i )
		{
			const scenegraph::resource::MaterialResourceNode::TextureInfo *pInfo = pMaterialInstanceNode->GetTextureInfo( i );

			if ( pInfo->m_SlotNo == slotNo )
			{
				rRenderState.SetTextureInfoOverride( 1, pInfo );
			}
		}
	}

	{
		setUpEnable.value = 0;
    setUpEnable.VsIsWorldSpace = origSetUpEnable.VsIsWorldSpace;
		setUpEnable.VertexShader = true;
		setUpEnable.GeometryShader = true;
		setUpEnable.FragmentShader = true;
		setUpEnable.UvSetting = true;
		//setUpEnable.Material = true;
		rRenderState.SetSetUpEnable( setUpEnable );

		pShaderDriver->Apply( rRenderState, pMeshDrawTag );
	}

	{//ブレンド無効
		nn::gr::CommandBufferJumpHelper		*pCommandBuffer = GFGL::GetJumpHelper();
		bit32															*command = pCommandBuffer->GetCurrentPtr();

		*command++ = PICA_CMD_DATA_COLOR_OPERATION( PICA_DATA_FRAGOP_MODE_DMP, PICA_DATA_ENABLE_BLEND );
		*command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_COLOR_OPERATION, 0x3 );

		// 0x101
		*command++ = PICA_CMD_DATA_BLEND_FUNC( PICA_DATA_BLEND_EQUATION_ADD, PICA_DATA_BLEND_FUNC_ONE, PICA_DATA_BLEND_FUNC_ZERO );
		*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_BLEND_FUNC );

		//*command++ = PICA_CMD_DATA_FRAGOP_ALPHA_TEST( true, PICA_DATA_ALPHA_TEST_GREATER, 0 );
    //*command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_FRAGOP_ALPHA_TEST, 0x3 );

		pCommandBuffer->Put(command);
	}

  {//ステンシルテスト無効
		nn::gr::CommandBufferJumpHelper		*pCommandBuffer = GFGL::GetJumpHelper();
		bit32															*command = pCommandBuffer->GetCurrentPtr();

		*command++ = PICA_CMD_DATA_STENCIL_TEST_DISABLE();
		*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_STENCIL_TEST );

		pCommandBuffer->Put(command);
	}

	{//シェーダーアニメ
		nn::gr::CommandBufferJumpHelper																				*pCommandBuffer = GFGL::GetJumpHelper();
		bit32									*cmd = pCommandBuffer->GetCurrentPtr();
		static const u32			regNo[6] = { PICA_REG_TEX_ENV0_COLOR, PICA_REG_TEX_ENV1_COLOR, PICA_REG_TEX_ENV2_COLOR,
																			 PICA_REG_TEX_ENV3_COLOR, PICA_REG_TEX_ENV4_COLOR, PICA_REG_TEX_ENV5_COLOR, };

		for( u32 i = 1; i <= 2; ++i )
		{
			s16										index = pAttr->m_TextureCombinerConstant[i];
      const gfx::ColorU8		&rColor = pAttr->m_ConstantColor[index];

			*cmd++ = PICA_CMD_DATA_TEX_ENV_CONST( rColor.r, rColor.g, rColor.b, rColor.a );
			*cmd++ = PICA_CMD_HEADER_SINGLE( regNo[i] );
		}

		pCommandBuffer->Put(cmd);
	}

#endif

	rRenderState.SetSetUpEnable( origSetUpEnable );

	rRenderState.SetTextureInfoOverride( 1, NULL );//リセット

	return true;
}

void EdgeMapSceneRenderPath::EdgeMapShaderDriver::SetTextureOverride( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
	RenderState::ShaderStateMap																		shaderStateMap = rRenderState.GetShaderStateMap();
	scenegraph::resource::MaterialResourceNode										*pMaterial = const_cast<scenegraph::resource::MaterialResourceNode*>( pMeshDrawTag->GetMaterialInstanceNode()->GetMaterialResourceNode() );
  const scenegraph::resource::MaterialResourceNode::UserData		&rUserData = pMaterial->GetUserData();

	if ( rUserData.m_EdgeType == scenegraph::resource::EdgeType::OwnColor )
		return;

	MeshDrawTag																										*pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );

#if defined(GF_PLATFORM_CTR)
	
	const scenegraph::resource::MaterialResourceNode							*pMaterialResourceNode = m_pModelInstanceNode->GetMaterialInstanceNode( 0 )->GetMaterialResourceNode();

	{
		u32                   textureCommand[2];

		const PicaDataTexture0SamplerType texture0SamplerType = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D;
    const PicaDataTexture1SamplerType texture1SamplerType = PICA_DATA_TEXTURE1_SAMPLER_TYPE_TEXTURE_2D;
    const PicaDataTexture2SamplerType texture2SamplerType = PICA_DATA_TEXTURE2_SAMPLER_TYPE_FALSE;
    const PicaDataTexture2TexCoord    texture2TexCoord    = PICA_DATA_TEXTURE2_TEXCOORD_TEXTURE1;
    const PicaDataTexture3SamplerType texture3SamplerType = PICA_DATA_TEXTURE3_SAMPLER_TYPE_FALSE;
    const PicaDataTexture3TexCoord    texture3TexCoord    = PICA_DATA_TEXTURE3_TEXCOORD_TEXTURE0;
    const bit32                       clearTextureCache   = 1;

    // 0x080
    textureCommand[0] = PICA_CMD_DATA_TEXTURE_FUNC( texture0SamplerType,
                                              texture1SamplerType,
                                              texture2SamplerType,
                                              texture3TexCoord,
                                              texture3SamplerType,
                                              texture2TexCoord,
                                              clearTextureCache );

		textureCommand[1] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE_FUNC );

		GFGL::AddCommandCopy( textureCommand, sizeof(textureCommand) );//@note 直接書き込みたい

		{
			nn::gr::CommandBufferJumpHelper																*pCommandBuffer = GFGL::GetJumpHelper();
			bit32									*cmd = pCommandBuffer->GetCurrentPtr();

			math::Vector4					uvSlotNo( 0, 1, 0, 0 );
			cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 0, *(nn::math::VEC4*)&uvSlotNo );
			pCommandBuffer->Put(cmd);
		}
	}

#endif
	
	{
		const gfx::Texture							*pTexture = pTag->GetTexture( TextureType::NormalRGB );

		if ( rUserData.m_EdgeType == scenegraph::resource::EdgeType::Deco )
			pTexture = m_pZInvTextureResourceNode->GetTexture();

#if defined(GF_PLATFORM_CTR)
		{
			u32                   textureCommand[10];

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

			textureCommand[8] = 0;
			textureCommand[9] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_LOD );
				
			GFGL::AddCommandCopy( textureCommand, sizeof(textureCommand) );//@note 直接書き込みたい
		}
#else
		const gfx::SamplerStateObject		*pSampler =  pTag->GetSampler( TextureType::NormalRGB );

		GFGL::SetTexture( 0, pTexture );
		GFGL::SetSamplerState( 0, pSampler );
#endif

		shaderStateMap.m_Uv0Enable = true;
	}

	{
		const gfx::Texture							*pTexture = NULL;

		switch( rUserData.m_EdgeMapAlphaMask ){
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::None:
			pTexture = pTag->GetTexture( TextureType::InvNormalRGBA );
			break;
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture0A:
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture0R:
			pTexture = pMeshDrawTag->GetTexture( 0 );
			break;
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture1A:
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture1R:
			pTexture = pMeshDrawTag->GetTexture( 1 );
			break;
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture2A:
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture2R:
			pTexture = pMeshDrawTag->GetTexture( 2 );
			break;
    default:
      // いずれにも該当しなければEdgeMapAlphaMaskの設定が間違っているのでASSERT.
      GFL_ASSERT(pTexture && "rUserData.m_EdgeMapAlphaMask is wrong.");
      break;
		}

#if defined(GF_PLATFORM_CTR)
		{
			u32                   textureCommand[10];

			const gfl2::gfx::ctr::CTRTexture					*pCTRTexture = reinterpret_cast<const gfl2::gfx::ctr::CTRTexture*>(pTexture);
			const gfl2::gfx::Texture::TextureDesc			&rTextureDesc = pCTRTexture->GetTextureDesc();

			{
				const scenegraph::resource::MaterialResourceNode::TextureInfo		*pTexInfo = &pMaterialResourceNode->GetTextureInfo( TextureType::InvNormalRGBA );

				static const s32			textureSlotNos[scenegraph::resource::CombinerAttribute::EdgeMapAlpha::NumberOf] = 
				{
						0,//Texture0 = 0,
						1,//Texture1,
						2,//Texture2,
						0,//Texture0R,
						1,//Texture1R,
						2//Texture2R,
				};

				if ( rUserData.m_EdgeMapAlphaMask != scenegraph::resource::CombinerAttribute::EdgeMapAlpha::None ) 
				{
					u32				slotNo = textureSlotNos[rUserData.m_EdgeMapAlphaMask];

					for( u32 i = 0; i < pMaterial->GetTextureNum(); ++i )
					{
						scenegraph::resource::MaterialResourceNode::TextureInfo *pInfo = &pMaterial->GetTextureInfo( i );

						if ( pInfo->m_SlotNo == slotNo )
						{
							pTexInfo = pInfo;
						}
					}
				}

				textureCommand[0] = PICA_CMD_DATA_TEXTURE_WRAP_FILTER(	pTexInfo->m_Attribute.m_MagFilter, pTexInfo->m_Attribute.m_MinFilter,
																																pCTRTexture->GetFormat(),
																																pTexInfo->m_Attribute.m_RepeatV, pTexInfo->m_Attribute.m_RepeatU,
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

			textureCommand[8] = 0;
			textureCommand[9] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_LOD );
				
			GFGL::AddCommandCopy( textureCommand, sizeof(textureCommand) );//@note 直接書き込みたい
		}
#else

		const gfx::SamplerStateObject		*pSampler = NULL;

		switch( rUserData.m_EdgeMapAlphaMask ){
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::None:
			pSampler =  pTag->GetSampler( TextureType::InvNormalRGBA );
			break;
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture0A:
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture0R:
			pSampler =  pMeshDrawTag->GetSampler( 0 );
			break;
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture1A:
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture1R:
			pSampler =  pMeshDrawTag->GetSampler( 1 );
			break;
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture2A:
		case scenegraph::resource::CombinerAttribute::EdgeMapAlpha::Texture2R:
			pSampler =  pMeshDrawTag->GetSampler( 2 );
			break;
		}

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
  m_DrawableNodeContainer.CreateBuffer( 6 );//例えばポケモン６体分

  for( u32 i = 0; i < EdgeColor::Max; i++)
  {
    m_pEdgeMapTexture[i] = NULL;
    m_EdgeColorEnableTable[ i ] = true;
  }
}

EdgeMapSceneRenderPath::EdgeMapSceneRenderPath(gfl2::gfx::IGLAllocator* pAllocator, int mdlNum) : SceneRenderPath()
{
  m_DrawableNodeContainer.CreateBuffer(mdlNum, pAllocator);

  for( u32 i = 0; i < EdgeColor::Max; i++)
  {
    m_pEdgeMapTexture[i] = NULL;
    m_EdgeColorEnableTable[ i ] = true;
  }
}

EdgeMapSceneRenderPath::~EdgeMapSceneRenderPath()
{
  GFX_SAFE_DELETE( m_pEdgeMapTexture[EdgeColor::Normal] );
  GFX_SAFE_DELETE( m_pEdgeMapTexture[EdgeColor::ID] );
}

void EdgeMapSceneRenderPath::LoadResources(
  gfl2::gfx::IGLAllocator* pAllocator, 
  c8* gfbmdlBinary,
  c8** shaderBinaries,
  s32 shaderNum,
  c8** textureBinaries,
  s32 texNum
  )
{
  m_EdgeMapShaderDriver.Initialize(pAllocator, gfbmdlBinary, shaderBinaries, shaderNum, textureBinaries, texNum);
}

void EdgeMapSceneRenderPath::CreateEdgeMapTexture( gfl2::gfx::IGLAllocator* pAllocator, f32 screanWidth, f32 screanHeight )
{
#if defined(GF_PLATFORM_CTR)
	m_pEdgeMapTexture[EdgeColor::Normal] = GFGL::CreateTexture( pAllocator, screanHeight, screanWidth, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
	m_pEdgeMapTexture[EdgeColor::ID] = NULL;
#else
	m_pEdgeMapTexture[EdgeColor::Normal] = GFGL::CreateTexture( pAllocator, screanWidth, screanHeight, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
	m_pEdgeMapTexture[EdgeColor::ID] = GFGL::CreateTexture(  pAllocator,screanWidth, screanHeight, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );
#endif
}

const IShaderDriver* EdgeMapSceneRenderPath::ShaderOverride()
{
	return &m_EdgeMapShaderDriver;
}

const gfl2::gfx::GLFixedSizeContainer<scenegraph::instance::DrawableNode*>* EdgeMapSceneRenderPath::DrawableNodeOverride()
{
	return &m_DrawableNodeContainer;
}

void EdgeMapSceneRenderPath::SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
	//GFGL::SetRenderTarget( 0, pRenderTarget );
	GFGL::SetRenderTarget( EdgeColor::Normal, m_pEdgeMapTexture[EdgeColor::Normal]->GetSurface() );
	if ( m_pEdgeMapTexture[EdgeColor::ID] )
		GFGL::SetRenderTarget( EdgeColor::ID, m_pEdgeMapTexture[EdgeColor::ID]->GetSurface() );
	GFGL::SetDepthStencil( pDepthStencil );
}

void EdgeMapSceneRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
#if defined(GF_PLATFORM_WIN32)
	GFGL::ClearRenderTarget( EdgeColor::Normal, Color( 0.5f, 0.5f, 0.0f) );
	GFGL::ClearDepthStencil( pDepthStencil, 1.0f, 255 );
#elif defined(GF_PLATFORM_CTR)
	GFGL::ClearRenderTargetDepthStencil( m_pEdgeMapTexture[EdgeColor::Normal]->GetSurface(), Color( 0.5f, 0.5f, 0.0f), pDepthStencil, 1.0f, 255 );
	//GFGL::ClearRenderTargetDepthStencil( pRenderTarget, Color( 0.5f, 0.5f, 0.0f), pDepthStencil, 1.0f, 255 );
#endif

	if ( m_pEdgeMapTexture[EdgeColor::ID] )
		GFGL::ClearRenderTarget( EdgeColor::ID, Color( 1.0f, 1.0f, 1.0f) );
}

void EdgeMapSceneRenderPath::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	if ( m_DrawableNodeContainer.free_size() )
		m_DrawableNodeContainer.push_back( pNode );
}

void EdgeMapSceneRenderPath::RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.remove( pNode );
}

void EdgeMapSceneRenderPath::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
}

void EdgeMapSceneRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
#if defined(GF_PLATFORM_CTR)
  if( this->IsEnable( EdgeColor::Normal ) == false )
  {
    return;
  }
#endif
#if defined(GF_PLATFORM_WIN32)
  if( this->IsEnable( EdgeColor::Normal ) == false && this->IsEnable( EdgeColor::ID ) == false )
  {
    return;
  }
#endif

  SetupDraw(rDrawContext);
  DrawManagerDraw(false);

#if defined(GF_PLATFORM_WIN32)
	GFGL::SetRenderTarget( EdgeColor::Normal, NULL );
	GFGL::SetRenderTarget( EdgeColor::ID, NULL );
#endif
}

void EdgeMapSceneRenderPath::CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	//SetCamera(rDrawContext);
 // this->Execute(rDrawContext);
	SetCamera(rDrawContext);
  SetupDraw( rDrawContext );
  ReuseCommand();

#if 0

#if defined(GF_PLATFORM_CTR)
  if( this->IsEnable( EdgeColor::Normal ) == false )
  {
    return;
  }
#endif
#if defined(GF_PLATFORM_WIN32)
  if( this->IsEnable( EdgeColor::Normal ) == false && this->IsEnable( EdgeColor::ID ) == false )
  {
    return;
  }
#endif

	SetCamera(rDrawContext);
  SetupDraw(rDrawContext);
  ReuseCommand();

#if defined(GF_PLATFORM_WIN32)
	GFGL::SetRenderTarget( EdgeColor::Normal, NULL );
	GFGL::SetRenderTarget( EdgeColor::ID, NULL );
#endif

#endif
}

void EdgeMapSceneRenderPath::SetEnable( u32 edgeColor, b32 flag )
{
  if( edgeColor >= EdgeColor::Max )
  {
    return;
  }

  m_EdgeColorEnableTable[ edgeColor ] = flag;
}

b32 EdgeMapSceneRenderPath::IsEnable( u32 edgeColor ) const
{
  if( edgeColor >= EdgeColor::Max )
  {
    return false;
  }

  return m_EdgeColorEnableTable[ edgeColor ];
}

}}
