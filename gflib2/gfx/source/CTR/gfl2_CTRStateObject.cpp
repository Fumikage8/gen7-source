//=============================================================================
/**
 * @file    gfl2_CTRStateObject.cpp
 * @brief   各種描画ステートオブジェクト設定のＣＴＲ実装が記載されたファイルです
 * @author  takebe_makoto
 * @date    2014
 *
 *  2016.4.8    ishiguro  CTRDepthStencilStateObject::UpdateStateにてステンシルコマンドを積むように修正
 *
 */
//=============================================================================
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRCommandBufferManager.h>
#include <gfx/include/CTR/gfl2_CTRStateObject.h>

namespace gfl2 { namespace gfx { namespace ctr {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  CTRBlendStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
CTRBlendStateObject::CTRBlendStateObject() : BlendStateObject(),
  m_CommandCache(NULL),
  m_CommandCacheResetFlag(false)
{
	m_FixData = m_StateData;
  
  m_CommandCache = CTRCommandBufferManager<16>::Create();
      // アルファテストのコマンドは8バイト(32ビットのヘッダと32ビットのデータ)
      // キックコマンドは8バイト(32ビットのヘッダと32ビットのデータ)
      // 合計16バイト。このギリギリしか確保していないので気を着けて。
}

CTRBlendStateObject::~CTRBlendStateObject()
{
	if(m_CommandCache)
  {
    delete m_CommandCache;
  }
}

void CTRBlendStateObject::UpdateState()
{
	m_FixData = m_StateData;

  //kawa20150527
  // アルファテストの変更
  {
    if(m_CommandCacheResetFlag)
    {
      m_CommandCache->ResetCommandBuffer();
      m_CommandCacheResetFlag = false;
    }
    else
    {
      m_CommandCache->ResetCurrentPtrOfBackBuffer();  // コマンドバッファの先頭に戻すことで、スワップする前にコマンドをつくり直す場合にも対応できる。決まったコマンドしか積まないのでResetしても問題ない。
      bit32* command = m_CommandCache->GetCurrentPtrOfBackBuffer();
      
      nn::gr::CTR::RenderState::AlphaTest   grAlphaTest(*(GFGL::GetRenderState()));
      nn::gr::CTR::RenderState::Blend       grBlend(*(GFGL::GetRenderState()));
      nn::gr::CTR::RenderState::ColorMask   grColorMask;
      this->Bind(&grAlphaTest, &grBlend, &grColorMask);
      
      command = grAlphaTest.MakeCommand(command, false);
          // isUpdateFBAccessの値
          //     void CTRDepthStencilStateObject::UpdateState() を参考にして下さい。  //kawa20150428
     
      m_CommandCache->SetCurrentPtrOfBackBuffer(command);
      
      m_CommandCache->AddSubroutineCommandToBuckBuffer();
    }
  }
}

void CTRBlendStateObject::Bind(nn::gr::RenderState::AlphaTest* pGrAlphaTest, nn::gr::RenderState::Blend* pGrBlend, nn::gr::RenderState::ColorMask* pGrColorMask)
{
  // アルファトゥカバレッジは3DSでは対応できない

  // アルファテスト
  if (pGrAlphaTest != NULL)
  {
    static const PicaDataAlphaTest sc_funcList[] =
    {
      PICA_DATA_ALPHA_TEST_LESS,      // Less,					//!< リファレンス値より小さい場合にのみパスさせる
      PICA_DATA_ALPHA_TEST_LEQUAL,    // LessEqual,		//!< リファレンス値と同じか小さい場合にパスさせる
      PICA_DATA_ALPHA_TEST_EQUAL,     // Equal,				//!< リファレンス値と同じ場合にはパスさせる
      PICA_DATA_ALPHA_TEST_GEQUAL,    // GreaterEqual,	//!< リファレンス値と同じか大きい場合にパスさせる
      PICA_DATA_ALPHA_TEST_GREATER,   // Greater,			//!< リファレンス値よりも大きい場合にパスさせる
      PICA_DATA_ALPHA_TEST_NOTEQUAL,  // NotEqual,			//!< リファレンス値と同じではない場合にパスさせる
      PICA_DATA_ALPHA_TEST_ALWAYS,    // Always,				//!< 常にパスさせる
      PICA_DATA_ALPHA_TEST_NEVER,     // Never,				//!< パスさせない
    };
    pGrAlphaTest->isEnable = m_FixData.m_AlphaTestEnable;
    pGrAlphaTest->func = sc_funcList[m_FixData.m_AlphaCompareFunc];
    pGrAlphaTest->refValue = m_FixData.m_AlphaRef * 255;
  }

  // ブレンド
  if (pGrBlend != NULL)
  {
    static const PicaDataBlendEquation sc_eqList[] =
    {
      PICA_DATA_BLEND_EQUATION_ADD,               // Add = 0,					//!< ( R, G, B, A ) = ( Rs * Rx + Rd * Ry, Gs * Gx + Gd * Gy, Bs * Bx + Bd * By, As * Ax + Ad * Ay )
      PICA_DATA_BLEND_EQUATION_SUBTRACT,          // Subtract,					//!< ( R, G, B, A ) = ( Rs * Rx - Rd * Ry, Gs * Gx - Gd * Gy, Bs * Bx - Bd * By, As * Ax - Ad * Ay )
      PICA_DATA_BLEND_EQUATION_REVERSE_SUBTRACT,  // ReverseSubtract,	//!< ( R, G, B, A ) = ( Rd * Ry - Rs * Rx, Gd * Gy - Gs * Gx, Bd * By - Bs * Bx, Ad * Ay - As * Ax )
      PICA_DATA_BLEND_EQUATION_MIN,               // Min,							//!< ( R, G, B, A ) = ( min( Rs * Rd ), min( Gs * Gd ), min( Bs * Bd ), min( As * Ad ) )
      PICA_DATA_BLEND_EQUATION_MAX,               // Max,							//!< ( R, G, B, A ) = ( MAX( Rs * Rd ), MAX( Gs * Gd ), MAX( Bs * Bd ), MAX( As * Ad ) )
    };

    static const PicaDataBlendFunc sc_funcList[] =
    {
      PICA_DATA_BLEND_FUNC_ZERO,                  // Zero = 0,							//!< ( R, G, B, A ) = ( 0, 0, 0, 0 )
      PICA_DATA_BLEND_FUNC_ONE,                   // One,									//!< ( R, G, B, A ) = ( 1, 1, 1, 1 )
      PICA_DATA_BLEND_FUNC_SRC_COLOR,             // SrcColor,							//!< ( R, G, B, A ) = ( Rs, Gs, Bs, As )
      PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_COLOR,   // InverseSrcColor,			//!< ( R, G, B, A ) = ( 1 - Rs, 1 - Gs, 1 - Bs, 1 - As )
      PICA_DATA_BLEND_FUNC_DST_COLOR,             // DestColor,						//!< ( R, G, B, A ) = ( Rd, Gd, Bd, Ad )
      PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_COLOR,   // InverseDestColor,			//!< ( R, G, B, A ) = ( 1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad )
      PICA_DATA_BLEND_FUNC_SRC_ALPHA,             // SrcAlpha,							//!< ( R, G, B, A ) = ( As, As, As, As )
      PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA,   // InverseSrcAlpha,			//!< ( R, G, B, A ) = ( 1 - As, 1 - As, 1 - As, 1 - As )
      PICA_DATA_BLEND_FUNC_DST_ALPHA,             // DestAlpha,						//!< ( R, G, B, A ) = ( Ad, Ad, Ad, Ad )
      PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA,   // InverseDestAlpha,			//!< ( R, G, B, A ) = ( 1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad )
      PICA_DATA_BLEND_FUNC_SRC_ALPHA_SATURATE,    // SrcAlphaSaturate,			//!< ( R, G, B, A ) = ( min( As, 1 - Ad ), min( As, 1 - Ad ), min( As, 1 - Ad ), 1 )

      // 定数とのブレンドには対応しない
      // 論理ブレンドには対応しない
    };

    pGrBlend->isEnable = m_FixData.m_BlendEnable;

    // RGB
    pGrBlend->eqRgb = sc_eqList[m_FixData.m_ColorBlendOp];
    pGrBlend->srcRgb = sc_funcList[m_FixData.m_SrcColorBlendFunc];
    pGrBlend->dstRgb = sc_funcList[m_FixData.m_DestColorBlendFunc];

    // Alpha
    pGrBlend->eqAlpha = sc_eqList[m_FixData.m_AlphaBlendOp];
    pGrBlend->srcAlpha = sc_funcList[m_FixData.m_SrcAlphaBlendFunc];
    pGrBlend->dstAlpha = sc_funcList[m_FixData.m_DestAlphaBlendFunc];
  }

  // カラーマスク
  if (pGrColorMask != NULL)
  {
    u32 mask = 0;

    // 3DSはMRT対応していないので必ず0番の設定を使用する
    if (m_FixData.m_RenderTargetWriteMask[0] & COLOR_WRITE_ENABLE_RED)
    {
      mask |= nn::gr::RenderState::COLOR_MASK_R;
    }
    if (m_FixData.m_RenderTargetWriteMask[0] & COLOR_WRITE_ENABLE_GREEN)
    {
      mask |= nn::gr::RenderState::COLOR_MASK_G;
    }
    if (m_FixData.m_RenderTargetWriteMask[0] & COLOR_WRITE_ENABLE_BLUE)
    {
      mask |= nn::gr::RenderState::COLOR_MASK_B;
    }
    if (m_FixData.m_RenderTargetWriteMask[0] & COLOR_WRITE_ENABLE_ALPHA)
    {
      mask |= nn::gr::RenderState::COLOR_MASK_A;
    }

    *pGrColorMask = static_cast<nn::gr::RenderState::ColorMask>(mask);
  }
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  CTRRasterizerStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
CTRRasterizerStateObject::CTRRasterizerStateObject() : RasterizerStateObject()
{
	m_FixData = m_StateData;
}

CTRRasterizerStateObject::~CTRRasterizerStateObject()
{
	
}

void CTRRasterizerStateObject::UpdateState()
{
	m_FixData = m_StateData;
}

void CTRRasterizerStateObject::Bind(nn::gr::RenderState::WBuffer* pGrWBuffer, nn::gr::RenderState::Culling* pGrCulling)
{
  // ポリゴンオフセット
  if (pGrWBuffer != NULL)
  {
    pGrWBuffer->isEnablePolygonOffset = (m_FixData.m_DepthBias != 0);
    pGrWBuffer->polygonOffsetUnit = m_FixData.m_DepthBias;
  }

  // カリング
  if (pGrCulling != NULL)
  {
    static nn::gr::RenderState::Culling::CullFace sc_cullFaceList[] =
    {
      nn::gr::RenderState::Culling::CULL_FACE_BACK,   // None = 0,		   //!< 背面カリングしない
      nn::gr::RenderState::Culling::CULL_FACE_FRONT,  // CW,				 //!< 背面を時計回りでカリング
      nn::gr::RenderState::Culling::CULL_FACE_BACK,   // CCW,				//!< 背面を反時計回りでカリング
    };

    pGrCulling->isEnable = (m_FixData.m_CullMode != gfx::CullMode::None);
    pGrCulling->cullFace = sc_cullFaceList[m_FixData.m_CullMode];
  }

  // マルチサンプリングとポリゴンフィルモードは3DSでは対応できない
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  CTRDepthStencilStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
CTRDepthStencilStateObject::CTRDepthStencilStateObject() : DepthStencilStateObject(),
  m_CommandCache(NULL),
  m_CommandCacheResetFlag(false)
{
	m_FixData = m_StateData;
  
  m_CommandCache = CTRCommandBufferManager<32>::Create();
      // デプステストのコマンドはisUpdateFBAccessがfalseのときは8バイト(32ビットのヘッダと32ビットのデータ)
      // キックコマンドは8バイト(32ビットのヘッダと32ビットのデータ)
      // 合計16バイト。このギリギリしか確保していないので気を付けて。
      // 2016/04/08 ステンシルコマンドが詰まれていなかったので16バイト追加しました。石黒
}

CTRDepthStencilStateObject::~CTRDepthStencilStateObject()
{
  if(m_CommandCache)
  {
    delete m_CommandCache;
  }
}

void CTRDepthStencilStateObject::UpdateState()
{
	m_FixData = m_StateData;

  //kawa20150428
  // デプステストの変更
  {
    if(m_CommandCacheResetFlag)
    {
      m_CommandCache->ResetCommandBuffer();
      m_CommandCacheResetFlag = false;
    }
    else
    {
      m_CommandCache->ResetCurrentPtrOfBackBuffer();  // コマンドバッファの先頭に戻すことで、スワップする前にコマンドをつくり直す場合にも対応できる。決まったコマンドしか積まないのでResetしても問題ない。
      bit32* command = m_CommandCache->GetCurrentPtrOfBackBuffer();
      
      nn::gr::CTR::RenderState::DepthTest   grDepthTest(*(GFGL::GetRenderState()));
      nn::gr::CTR::RenderState::StencilTest grStencilTest(*(GFGL::GetRenderState()));
      this->Bind(&grDepthTest, &grStencilTest);
      
      // カラーマスクの設定もコマンドに含まれる
      command = grDepthTest.MakeCommand(command, false);//8バイト詰まれる
          // カラーマスクの設定
          //     カラーマスクの設定はCTR_SDK/sources/libraries/gr/CTR/gr_RenderState.cpp中の
          //     bit32* RenderState::DepthTest::MakeCommand( bit32* command, bool isUpdateFBAccess ) const
          //     によると、初期化に用いたnn::gr::RenderStateから値を引っ張って来ているので、
          //     変更されることはないと思う。
          //     プログラミングマニュアル
          //         グラフィックス応用編
          //             8.8.11. デプステスト設定レジスタ（0x0107, 0x0126）
          // isUpdateFBAccessの値
          //     BeginSceneが呼ばれた時に（つまりフレームバッファが切り替わったタイミングで）一度だけ更新する作りなのでfalseで問題ないらしい。
          //     マテリアル側でＦＢアクセスは別途積んでいるのでfalseで問題ないというお話もあった。
          //     下記のものもfalseになっている。
          //     R:\home\niji\git_program\gflib2\gfx\source\CTR\gfl2_CTRGL.cpp
          //         void CTRGL::AddDepthTestCommand(nn::gr::CommandBufferJumpHelper* pJumpHelper)
     
      command = grStencilTest.MakeCommand(command, false);//16バイト詰まれる

      m_CommandCache->SetCurrentPtrOfBackBuffer(command);
      
      m_CommandCache->AddSubroutineCommandToBuckBuffer();//8バイト詰まれる
    }
  }
}

void CTRDepthStencilStateObject::Bind(nn::gr::RenderState::DepthTest* pGrDepthTest, nn::gr::RenderState::StencilTest* pGrStencilTest)
{
  // デプステスト
  if (pGrDepthTest != NULL)
  {
    static const PicaDataDepthTest sc_funcList[] =
    {
      PICA_DATA_DEPTH_TEST_LESS,      // Less,					//!< リファレンス値より小さい場合にのみパスさせる
      PICA_DATA_DEPTH_TEST_LEQUAL,    // LessEqual,		//!< リファレンス値と同じか小さい場合にパスさせる
      PICA_DATA_DEPTH_TEST_EQUAL,     // Equal,				//!< リファレンス値と同じ場合にはパスさせる
      PICA_DATA_DEPTH_TEST_GEQUAL,    // GreaterEqual,	//!< リファレンス値と同じか大きい場合にパスさせる
      PICA_DATA_DEPTH_TEST_GREATER,   // Greater,			//!< リファレンス値よりも大きい場合にパスさせる
      PICA_DATA_DEPTH_TEST_NOTEQUAL,  // NotEqual,			//!< リファレンス値と同じではない場合にパスさせる
      PICA_DATA_DEPTH_TEST_ALWAYS,    // Always,				//!< 常にパスさせる
      PICA_DATA_DEPTH_TEST_NEVER,     // Never,				//!< パスさせない
    };

    pGrDepthTest->isEnable = m_FixData.m_DepthTestEnable;
    pGrDepthTest->func = sc_funcList[m_FixData.m_DepthFunc];
    pGrDepthTest->isEnableWrite = m_FixData.m_DepthWriteEnable;
  }

  // ステンシルテスト
  if (pGrStencilTest != NULL)
  {
    static const PicaDataStencilTest sc_funcList[] =
    {
      PICA_DATA_STENCIL_TEST_LESS,      // Less,					//!< リファレンス値より小さい場合にのみパスさせる
      PICA_DATA_STENCIL_TEST_LEQUAL,    // LessEqual,		//!< リファレンス値と同じか小さい場合にパスさせる
      PICA_DATA_STENCIL_TEST_EQUAL,     // Equal,				//!< リファレンス値と同じ場合にはパスさせる
      PICA_DATA_STENCIL_TEST_GEQUAL,    // GreaterEqual,	//!< リファレンス値と同じか大きい場合にパスさせる
      PICA_DATA_STENCIL_TEST_GREATER,   // Greater,			//!< リファレンス値よりも大きい場合にパスさせる
      PICA_DATA_STENCIL_TEST_NOTEQUAL,  // NotEqual,			//!< リファレンス値と同じではない場合にパスさせる
      PICA_DATA_STENCIL_TEST_ALWAYS,    // Always,				//!< 常にパスさせる
      PICA_DATA_STENCIL_TEST_NEVER,     // Never,				//!< パスさせない
    };
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

    pGrStencilTest->isEnable = m_FixData.m_StencilTestEnable;
    pGrStencilTest->maskOp = m_FixData.m_SetStencilWriteMask;

    // 3DSは必ず両面同時設定しかできない
    // 表面に設定されている設定を裏面にも使うことにする
    u32 faceNo = 0;
    pGrStencilTest->func = sc_funcList[m_FixData.m_StencilFunc[faceNo]];
    pGrStencilTest->ref = m_FixData.m_StencilRef;
    pGrStencilTest->mask = m_FixData.m_StencilMask;

    pGrStencilTest->opFail = sc_opList[m_FixData.m_StencilFailOp[faceNo]];
    pGrStencilTest->opZFail = sc_opList[m_FixData.m_StencilDepthFailOp[faceNo]];
    pGrStencilTest->opZPass = sc_opList[m_FixData.m_StencilPassOp[faceNo]];
  }
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  CTRDepthStencilStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
CTRSamplerStateObject::CTRSamplerStateObject() : SamplerStateObject()
{
	m_FixData = m_StateData;
}

CTRSamplerStateObject::~CTRSamplerStateObject()
{
	
}

void CTRSamplerStateObject::UpdateState()
{
	m_FixData = m_StateData;
}

void CTRSamplerStateObject::Bind( nn::gr::Texture* pGrTexture,  u32 texNo, const CTRTexture *pTexture ) const
{
  if (pGrTexture == NULL)
  {
    return;
  }

  bool isNullTexture = (pTexture == NULL);

  // 対象のユニット取得
  nn::gr::Texture::UnitBase* pUnit = NULL;
  if (texNo == 0)
  {
    pUnit = &pGrTexture->unit0;
    
    // Unit0固有の初期化
    pGrTexture->unit0.texType = (isNullTexture)
      ? PICA_DATA_TEXTURE0_SAMPLER_TYPE_FALSE
      : PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D;
  }
  else if (texNo == 1)
  {
    pUnit = &pGrTexture->unit1;

    // Unit1固有の初期化
    pGrTexture->unit1.texType = (isNullTexture)
      ? PICA_DATA_TEXTURE1_SAMPLER_TYPE_FALSE
      : PICA_DATA_TEXTURE1_SAMPLER_TYPE_TEXTURE_2D;
  }
  else
  {
    pUnit = &pGrTexture->unit2;

    // Unit2固有の初期化
    pGrTexture->unit2.texType = (isNullTexture)
      ? PICA_DATA_TEXTURE2_SAMPLER_TYPE_FALSE
      : PICA_DATA_TEXTURE2_SAMPLER_TYPE_TEXTURE_2D;
  }
  // プロシージャルテクスチャは対応しない
  //if (texNo == 3)
  //{
  //  pUnit = &pGrTexture->unit3;
  //}

  // テクスチャが空ならこれ以上は不要
  if (isNullTexture)
  {
    return;
  }

  pUnit->physicalAddr = pTexture->GetPhysicalAddr();

  const Texture::TextureDesc& desc = pTexture->GetTextureDesc();
  pUnit->format = static_cast<PicaDataTextureFormat>(s_ctrTextureFormatTable[desc.format]);
  pUnit->width = desc.width;
  pUnit->height = desc.height;

  if (desc.mipLevels > 1)
  {
    pUnit->minFilter = s_ctrTextureMinFilterMipmap[m_FixData.m_MinFilter][m_FixData.m_MipFilter];
  }
  else
  {
    pUnit->minFilter = s_ctrTextureMinFilter[m_FixData.m_MinFilter];
  }
  pUnit->magFilter = s_ctrTextureMagFilter[m_FixData.m_MagFilter];

  pUnit->wrapS = s_ctrTextureWrap[m_FixData.m_WrapS];
  pUnit->wrapT = s_ctrTextureWrap[m_FixData.m_WrapT];

  pUnit->borderColorR = m_FixData.m_BorderColor.x * 255;
  pUnit->borderColorG = m_FixData.m_BorderColor.y * 255;
  pUnit->borderColorB = m_FixData.m_BorderColor.z * 255;
  pUnit->borderColorA = m_FixData.m_BorderColor.w * 255;

  pUnit->lodBias = m_FixData.m_MipmapLodBias;
  pUnit->minLodLevel = m_FixData.m_MinLod;
  pUnit->maxLodLevel = m_FixData.m_MaxLod;
}


}}}
