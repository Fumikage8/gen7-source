//=============================================================================
/**
 * @file    gfl2_MaterialResourceNode.cpp
 * @brief   MaterialResourceNodeクラス（マテリアルのリソースを持つノード）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.4.8    ishiguro  CopyMaterialDepthStencilStateToStateObject関数追加とm_SamplerStateObjectの削除
 *  2016.4.7    ishiguro  SetUserDataShaderParam関数追加
 *
 */
//=============================================================================
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <debug/include/gfl2_Assert.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

#if defined(GF_PLATFORM_CTR)
struct CommandHeader
{
  u32 addr    : 16;
  u32 be      : 4;
  u32 size    : 8;
  u32 padding : 3;
  u32 seq     : 1;
};

struct SingleCommand
{
  u32             data;
  CommandHeader   header;
};

union StencilTestData
{
  struct
  {
    u32 value;
  };
  struct
  {
    u32 enable    : 1;
    u32 padding0  : 3;
    u32 func      : 3;
    u32 padding1  : 1;
    u32 fb_mask   : 8;
    u32 ref       : 8;
    u32 mask      : 8;
  };
};

union StencilOpData
{
  struct
  {
    u32 value;
  };
  struct
  {
    u32 fail      : 3;
    u32 padding0  : 1;
    u32 zfail     : 3;
    u32 padding1  : 1;
    u32 zpass     : 3;
    u32 padding2  : 21;
  };
};

union DepthTestData
{
  struct
  {
    u32 value;
  };
  struct
  {
    u32 enable      : 1;
    u32 padding0    : 3;
    u32 func        : 3;
    u32 padding1    : 5;
    u32 flag        : 1;
    u32 padding2    : 19;
  };
};
#endif

MaterialResourceNode::MaterialResourceNode() : NodeTrait<MaterialResourceNode, ResourceNode>(),
	m_BlendStateObject( NULL ),
	m_DepthStencilStateObject( NULL ),
  m_RasterizerStateObject( NULL ),
	m_DummySamplerStateObject( NULL )
{
#if defined(GF_PLATFORM_WIN32)
	for( u32 i = 0; i < Constant::TEXTURE_MAX; ++i )
		m_SamplerStateObject[i] = NULL;
#endif

#if defined(GF_PLATFORM_CTR)
	for( u32 i = 0; i < LUT_MAX; ++i )
		m_MaterialResourceData.m_LutCommandNameHash[i] = 0;

	for( u32 i = 0; i < 4; ++i )
		m_MaterialResourceData.m_VertexColorEnable[i] = true;
#endif

	::std::memset( &m_MaterialResourceData.m_Header.shaderNameBuffer, 0, sizeof(m_MaterialResourceData.m_Header.shaderNameBuffer) );
	::std::memset( &m_MaterialResourceData.m_FShaderNameBuffer, 0, sizeof(m_MaterialResourceData.m_FShaderNameBuffer) );

	m_MaterialResourceData.m_AttributeParam.m_BumpMapNo = -1;
}

MaterialResourceNode::~MaterialResourceNode()
{
	if ( m_MaterialResourceData.m_pTextureInfoList )
		this->GetInstanceAllocator()->SystemMemoryFree( m_MaterialResourceData.m_pTextureInfoList );

#if defined(GF_PLATFORM_WIN32)
	for( u32 i = 0; i < Constant::TEXTURE_MAX; ++i)
	{
		GFX_SAFE_DELETE( m_SamplerStateObject[i] );
	}
#endif

	GFX_SAFE_DELETE( m_BlendStateObject );
	GFX_SAFE_DELETE( m_DepthStencilStateObject );
  GFX_SAFE_DELETE( m_RasterizerStateObject );
	GFX_SAFE_DELETE( m_DummySamplerStateObject );
}

void MaterialResourceNode::SetData( IBinaryFileAccessor *pBinaryFileAccessor, const void* data )
{
  pBinaryFileAccessor->SetAttributeParam( this->GetInstanceAllocator(), &this->m_MaterialResourceData, data );
}

void MaterialResourceNode::CreateStateObject()
{
#if defined(GF_PLATFORM_CTR)
  //kawa20150527
  GFX_SAFE_DELETE( m_BlendStateObject );
  m_BlendStateObject = GFGL::CreateBlendStateObject(this->GetInstanceAllocator());

  //kawa20150428
  GFX_SAFE_DELETE( m_DepthStencilStateObject );
  m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject(this->GetInstanceAllocator());

#elif defined(GF_PLATFORM_WIN32)
  {//ここで描画ステートを作成
		for( u32 i = 0; i < m_MaterialResourceData.m_TextureNum; ++i )
		{
			const TextureInfo& textureInfo = m_MaterialResourceData.m_pTextureInfoList[i];
			u32 index = textureInfo.m_SlotNo;
			GFX_SAFE_DELETE( m_SamplerStateObject[index] );

      m_SamplerStateObject[index] = GFGL::CreateSamplerStateObject( this->GetInstanceAllocator() );

			m_SamplerStateObject[index]->SetSamplerAddress( static_cast<TextureAddress>( textureInfo.m_Attribute.m_RepeatU ),
																											static_cast<TextureAddress>( textureInfo.m_Attribute.m_RepeatV )
																									);

			m_SamplerStateObject[index]->SetSamplerFilter(static_cast<TextureFilter>( textureInfo.m_Attribute.m_MagFilter ),
																								static_cast<TextureFilter>( textureInfo.m_Attribute.m_MinFilter % 2 ),
																								static_cast<TextureFilter>( textureInfo.m_Attribute.m_MinFilter >> 1 ),
																								static_cast<f32>( textureInfo.m_Attribute.m_MipBias ) );

			m_SamplerStateObject[index]->UpdateState();
		}
	}

	{// ここでブレンドステートを作成
		GFX_SAFE_DELETE( m_BlendStateObject );
		m_BlendStateObject = GFGL::CreateBlendStateObject( this->GetInstanceAllocator() );

		{// ブレンドの設定
			b32 blendEnable = m_MaterialResourceData.m_BlendTest.m_Blender.m_BlendMode != renderer::BlendMode::None;
			m_BlendStateObject->SetBlendEnable(blendEnable);
		}

		if (m_MaterialResourceData.m_BlendTest.m_Blender.m_BlendMode == renderer::BlendMode::Blend)
		{// ブレンドモードが[Blend]だったら加算合成の設定に強制的に変更
			m_MaterialResourceData.m_BlendTest.m_Blender.m_Alpha.m_SrcElement = gfl2::renderingengine::renderer::BlendElement::One;
			m_MaterialResourceData.m_BlendTest.m_Blender.m_Alpha.m_DstElement = gfl2::renderingengine::renderer::BlendElement::Zero;
			m_MaterialResourceData.m_BlendTest.m_Blender.m_Alpha.m_BlendFunc = BlendOp::Add;
		}

		m_BlendStateObject->SetBlendOp(
			m_MaterialResourceData.m_BlendTest.m_Blender.m_Color.m_BlendFunc,
			m_MaterialResourceData.m_BlendTest.m_Blender.m_Alpha.m_BlendFunc
		);

		m_BlendStateObject->SetBlendFunc(
			(gfx::BlendFunc)m_MaterialResourceData.m_BlendTest.m_Blender.m_Color.m_SrcElement,
			(gfx::BlendFunc)m_MaterialResourceData.m_BlendTest.m_Blender.m_Color.m_DstElement,
			(gfx::BlendFunc)m_MaterialResourceData.m_BlendTest.m_Blender.m_Alpha.m_SrcElement,
			(gfx::BlendFunc)m_MaterialResourceData.m_BlendTest.m_Blender.m_Alpha.m_DstElement
		);

		{// アルファテストの設定
			b32 alphaTestEnable;
			alphaTestEnable = (m_MaterialResourceData.m_BlendTest.m_AlphaTest.m_AlphaTestType != CompareFunc::Always) ? true : false;
			m_BlendStateObject->SetAlphaTestEnable(alphaTestEnable);
			//m_BlendStateObject->SetAlphaToCoverageEnable(false);
			m_BlendStateObject->SetAlphaRef(m_MaterialResourceData.m_BlendTest.m_AlphaTest.m_AlphaTestRefValue);
		}

		m_BlendStateObject->UpdateState();
	}

	{// デプス・ステンシルステートを作成
		GFX_SAFE_DELETE( m_DepthStencilStateObject );
		m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject(this->GetInstanceAllocator());

		// デプスの設定
		m_DepthStencilStateObject->SetDepthTestEnable(true);//m_BlendTest.m_DepthTest.m_UpDateDepth);
		m_DepthStencilStateObject->SetDepthWriteEnable(m_MaterialResourceData.m_BlendTest.m_DepthTest.m_UpDateDepth);
		m_DepthStencilStateObject->SetDepthFunc(m_MaterialResourceData.m_BlendTest.m_DepthTest.m_DepthTestType);

		// ステンシルの設定
		m_DepthStencilStateObject->SetStencilTestEnable(m_MaterialResourceData.m_BlendTest.m_StencilTest.m_StencilTestEnable);
		m_DepthStencilStateObject->SetStencilFunc(
			PolygonFace::CCW,
			m_MaterialResourceData.m_BlendTest.m_StencilTest.m_StencilTestType,
			m_MaterialResourceData.m_BlendTest.m_StencilTest.m_StencilValue,
			m_MaterialResourceData.m_BlendTest.m_StencilTest.m_StencilMask
		);
		m_DepthStencilStateObject->SetStencilOp(
			PolygonFace::CCW,
			m_MaterialResourceData.m_BlendTest.m_StencilTest.m_StencilFailOp,
			m_MaterialResourceData.m_BlendTest.m_StencilTest.m_StencilZFailOp,
			m_MaterialResourceData.m_BlendTest.m_StencilTest.m_StencilZpassOp
		);
		m_DepthStencilStateObject->UpdateState();
	}

  {// ラスタライザ・ステンシルステートを作成
    GFX_SAFE_DELETE( m_RasterizerStateObject );
    m_RasterizerStateObject = GFGL::CreateRasterizerStateObject(this->GetInstanceAllocator());

    m_RasterizerStateObject->SetCullMode( m_MaterialResourceData.m_AttributeParam.m_CullMode );

    m_RasterizerStateObject->UpdateState();
  }
#endif
}

#if defined(GF_PLATFORM_CTR)
/**
  * @brief マテリアルの初期値をステートオブジェクトにコピーする
  * @@attention ＣＴＲのステートオブジェクトはマテリアルの初期値を引き継いでいません。別途コピーの実行が必要です。
  */
void MaterialResourceNode::CopyMaterialDepthStencilStateToStateObject()
{
  u32*                pSeek = const_cast<u32*>( reinterpret_cast<const u32*>( m_MaterialResourceData.m_pMaterialCommandTop ) );
  u32                 commandSize = m_MaterialResourceData.m_CommandLength / sizeof(u32);
  u32                 SingleCommandSize = sizeof(SingleCommand);
  StencilTestData     stencilTestData;
  StencilOpData       stencilOpData;
  DepthTestData       depthTestData;
  
  //gfbmdl内マテリアルコマンドより初期値を収集します。
  for( u32 pos = 0; pos < commandSize; )
  {
    SingleCommand   *pSingleCommand = reinterpret_cast<SingleCommand*>( &pSeek[pos] );

    pos += SingleCommandSize / sizeof(u32);
    pos += pSingleCommand->header.size;//シングルなら０、１以上ならバースト
    pos += pSingleCommand->header.size % 2;//パディング

    switch( pSingleCommand->header.addr ){
    case PICA_REG_STENCIL_TEST:
      stencilTestData.value = pSingleCommand->data;
      break;
    case PICA_REG_STENCIL_OP:
      stencilOpData.value = pSingleCommand->data;
      break;
    case PICA_REG_DEPTH_COLOR_MASK:
      {//gfbmdlではPICA_REG_STENCIL_TEST->PICA_REG_STENCIL_OP->PICA_REG_DEPTH_COLOR_MASKの順で詰まれています。よってここが読み取り最後です。
        depthTestData.value = pSingleCommand->data;
        pos = commandSize;//読み取り終了
      }
      break;
    }
  }

  //ＣＴＲコマンド値をgfxの抽象値に変換するテーブルです。
  static const gfx::CompareFunc sc_funcList[] =
  {
    gfx::CompareFunc::Never,         //PICA_DATA_DEPTH_TEST_NEVER,     //!< パスさせない
    gfx::CompareFunc::Always,        //PICA_DATA_DEPTH_TEST_ALWAYS,    //!< 常にパスさせる
    gfx::CompareFunc::Equal,         //PICA_DATA_DEPTH_TEST_EQUAL,     //!< リファレンス値と同じ場合にはパスさせる
    gfx::CompareFunc::NotEqual,      //PICA_DATA_DEPTH_TEST_NOTEQUAL,  //!< リファレンス値と同じではない場合にパスさせる
    gfx::CompareFunc::Less,          //PICA_DATA_DEPTH_TEST_LESS,      //!< リファレンス値より小さい場合にのみパスさせる
    gfx::CompareFunc::LessEqual,     //PICA_DATA_DEPTH_TEST_LEQUAL,    //!< リファレンス値と同じか小さい場合にパスさせる
    gfx::CompareFunc::Greater,       //PICA_DATA_DEPTH_TEST_GREATER,   //!< リファレンス値よりも大きい場合にパスさせる
    gfx::CompareFunc::GreaterEqual   //PICA_DATA_DEPTH_TEST_GEQUAL,    //!< リファレンス値と同じか大きい場合にパスさせる
  };

  //ＣＴＲコマンド値をgfxの抽象値に変換するテーブルです。
  static const gfx::StencilOp sc_opList[] =
  {
    gfx::StencilOp::Keep,             //PICA_DATA_STENCIL_OP_KEEP,        //!< 更新しない
    gfx::StencilOp::Zero,             //PICA_DATA_STENCIL_OP_ZERO,        /!< ０に設定
    gfx::StencilOp::Replace,          //PICA_DATA_STENCIL_OP_REPLACE,     //!< 参照値で置き換える
    gfx::StencilOp::Increment,        //PICA_DATA_STENCIL_OP_INCR,        //!< 最大値に達するまで増加
    gfx::StencilOp::Decrement,        //PICA_DATA_STENCIL_OP_DECR,        //!< ０になるまで減少
    gfx::StencilOp::Invert,           //PICA_DATA_STENCIL_OP_INVERT,      //!< ビットを反転
    gfx::StencilOp::IncrementWrap,    //PICA_DATA_STENCIL_OP_INCR_WRAP,   //!< 増加させその値が最大値を超えると０に戻します。
    gfx::StencilOp::DecrementWrap,    //PICA_DATA_STENCIL_OP_DECR_WRAP,   //!< 減少させその値が０より小さくなった場合は最大値に戻します。
  };

  // デプスの設定
  m_DepthStencilStateObject->SetDepthTestEnable(depthTestData.enable);
  m_DepthStencilStateObject->SetDepthWriteEnable(depthTestData.flag);
  m_DepthStencilStateObject->SetDepthFunc(sc_funcList[depthTestData.func]);

  // ステンシルの設定
  m_DepthStencilStateObject->SetStencilTestEnable(stencilTestData.enable);

  m_DepthStencilStateObject->SetStencilFunc(
    PolygonFace::CCW,
    sc_funcList[stencilTestData.func],
    stencilTestData.ref,
    stencilTestData.mask
    );

  m_DepthStencilStateObject->SetStencilWriteMask( 0xFF );

  m_DepthStencilStateObject->SetStencilOp(
    PolygonFace::CCW,
    sc_opList[stencilOpData.fail],
    sc_opList[stencilOpData.zfail],
    sc_opList[stencilOpData.zpass]
    );

#if 0
  //正しくコピーできているか確認
  m_DepthStencilStateObject->UpdateState();

  gfx::ctr::CTRDepthStencilStateObject         *pCTRDepthStencilStateObject = static_cast<gfx::ctr::CTRDepthStencilStateObject*>( m_DepthStencilStateObject );
  gfx::ctr::CTRCommandBufferManager<32>        *pCommand = pCTRDepthStencilStateObject->GetCommandCache();

  pCommand->SwapCommandBuffer();
  if( pCommand->IsFrontBufferUsed() )
  {
    pSeek = const_cast<u32*>( reinterpret_cast<const u32*>( pCommand->GetStartPtrOfFrontBuffer() ) );
    commandSize = pCommand->GetSizeOfFrontBuffer();
  }

  StencilTestData     stencilTestDataCheck;
  StencilOpData       stencilOpDataCheck;
  DepthTestData       depthTestDataCheck;

  //gfbmdl内マテリアルコマンドより初期値を収集します。
  for( u32 pos = 0; pos < commandSize; )
  {
    SingleCommand   *pSingleCommand = reinterpret_cast<SingleCommand*>( &pSeek[pos] );

    pos += SingleCommandSize / sizeof(u32);
    pos += pSingleCommand->header.size;//シングルなら０、１以上ならバースト
    pos += pSingleCommand->header.size % 2;//パディング

    switch( pSingleCommand->header.addr ){
    case PICA_REG_STENCIL_TEST:
      stencilTestDataCheck.value = pSingleCommand->data;
      break;
    case PICA_REG_STENCIL_OP:
      stencilOpDataCheck.value = pSingleCommand->data;
      break;
    case PICA_REG_DEPTH_COLOR_MASK:
      depthTestDataCheck.value = pSingleCommand->data;
      break;
    }
  }

  GFL_ASSERT( stencilTestDataCheck.value == stencilTestData.value );
  GFL_ASSERT( stencilOpDataCheck.value == stencilOpData.value );
  GFL_ASSERT( depthTestDataCheck.value == depthTestData.value );
#endif
}
#endif

const c8* MaterialResourceNode::GetShaderName(gfx::ShaderType type) const 
{
	switch( type ){
	case gfx::ShaderType::Vertex:
		return m_MaterialResourceData.m_Header.shaderNameBuffer;		
	case gfx::ShaderType::Geometry:
		break;
	case gfx::ShaderType::Pixel:
		return m_MaterialResourceData.m_FShaderNameBuffer;		
	}
	return m_MaterialResourceData.m_Header.shaderNameBuffer;
}

u32 MaterialResourceNode::GetShaderNameHash(gfx::ShaderType type) const 
{
	switch( type ){
	case gfx::ShaderType::Vertex:
		return m_MaterialResourceData.m_Header.shaderNameHash;		
	case gfx::ShaderType::Geometry:
		break;
	case gfx::ShaderType::Pixel:
		return m_MaterialResourceData.m_FShaderNameHash;		
	}
	return m_MaterialResourceData.m_Header.shaderNameHash;
}

// --------------------------------------------------------------------------
/**
  * @brief  UserDataの汎用シェーダーパラメーター設定
  * @param  no       汎用シェーダーパラメーター番号設定（０～３）
  * @param  value    セットしたい値
  */
// --------------------------------------------------------------------------
void MaterialResourceNode::SetUserDataShaderParam( u32 no, f32 value )
{
  switch( no ){
  case 0:
    m_MaterialResourceData.m_UserData.m_ShaderParam0 = value;
    break;
  case 1:
    m_MaterialResourceData.m_UserData.m_ShaderParam1 = value;
    break;
  case 2:
    m_MaterialResourceData.m_UserData.m_ShaderParam2 = value;
    break;
  case 3:
    m_MaterialResourceData.m_UserData.m_ShaderParam3 = value;
    break;
  default:
    GFL_ASSERT_MSG( 0, "第一引数は0～3を指定してください。" );
    break;
  }
  
}

}}}}
