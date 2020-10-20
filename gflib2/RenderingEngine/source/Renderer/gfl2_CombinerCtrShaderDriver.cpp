//=============================================================================
/**
 * @file    gfl2_CombinerCtrShaderDriver.cpp
 * @brief   CombinerCtrShaderDriverクラス（ＣＴＲに対してシェーダーとそのパラメーターを設定するクラス）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.5.12    ishiguro  [fix]GFNMCat[3985]:ファイヤーシェーダービルボードのスケール実装間違い修正
 *  2016.5.09    ishiguro  未使用変数の警告対処
 *  2016.4.10    ishiguro  MegaGangarシェーダーの使い方を間違えていたので修正（高さ設定が間違っていました。）
 *
 */
//=============================================================================
#include <gfx/include/gfl2_GFGLUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <RenderingEngine/include/renderer/gfl2_CombinerCtrShaderDriver.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CombinerAttribute.h>
#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>
#include <debug/include/gfl2_Assert.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <gfx/include/CTR/gfl2_CTRCommandBufferUtil.h>

#if defined(NN_PLATFORM_CTR)
  #include <gfx/include/CTR/gfl2_CTRGL.h>
	#include <nn/gx/CTR/gx_MacroReg.h>
	#include <nn/gr/CTR/gr_Utility.h>
	#include <nn/math.h>
	#include <nn.h>
#elif defined(_WIN64)
	#include <gfx/include/WinOpenGL/gfl2_WinOpenGLPixelShader.h>
#endif

#include <debug/include/gfl2_DebugPrint.h>

using namespace gfl2::gfx;
using namespace gfl2::renderingengine::scenegraph::resource;

namespace gfl2 { namespace renderingengine { namespace renderer { 

u32																					CombinerCtrShaderDriver::s_MaterialCacheHash = 0;
u32																					CombinerCtrShaderDriver::s_VertexShaderCacheHash = 0;
u32																					CombinerCtrShaderDriver::s_PixelShaderCacheHash = 0;
CombinerCtrShaderDriver::LightParamCheck		CombinerCtrShaderDriver::s_LightParamCheckCache;
const f32																		CombinerCtrShaderDriver::s_PokeShadowOffSet = 0.5f;

gfl2::math::Matrix34 CombinerCtrShaderDriver::s_ViewSpaceDiffMatrix;

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  CombinerCtrShaderDriverクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

/*CombinerCtrShaderDriver* CombinerCtrShaderDriver::Create()
{
  return Create(GLMemory::GetAllocator());
}*/

CombinerCtrShaderDriver* CombinerCtrShaderDriver::Create( gfx::IGLAllocator * pAllocator )
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
	CombinerCtrShaderDriver* instance = new CombinerCtrShaderDriver();
  gfx::GLMemory::ClearInstanceAllocator();
  return instance;
}
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
CombinerCtrShaderDriver::CombinerCtrShaderDriver() :
	gfx::GLMemory(),
	IShaderDriver(),
	m_pVertexShader( NULL ),
	m_pGeometryShader(NULL),
	m_pPixelShader( NULL )
  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  , m_pFogEnableCommand(NULL),
  m_pFogDisableCommand(NULL)
#endif  // GF_PLATFORM_CTR
{
  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  createFogFixCommand(); 
#endif  // GF_PLATFORM_CTR
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
CombinerCtrShaderDriver::~CombinerCtrShaderDriver()
{
  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  destroyFogFixCommand();
#endif  // GF_PLATFORM_CTR
}

#if defined(NN_PLATFORM_CTR)
void CombinerCtrShaderDriver::ApplyLight( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag, RenderState::ShaderStateMap& shaderStateMap, nn::gr::CommandBufferJumpHelper* pCommandBuffer, u32 materialVertion, const scenegraph::resource::MaterialResourceNode* pMaterial, const scenegraph::resource::MaterialResourceNode::AttributeParam* pAttributeParam, const scenegraph::resource::MaterialResourceNode::UserData& rUserData) const
{
  shaderStateMap.m_InverseLightingEnable = pMaterial->GetVertexColorEnable( 1 );
  shaderStateMap.m_PhongLightingEnable = false;
  shaderStateMap.m_RimLightingEnable = false;

  if ( rUserData.m_RimScale > 0.0f || rUserData.m_PhongScale > 0.0f )
  {
    static const	u32			s_LightPowScaleReg = 82;
    bit32									*cmd = pCommandBuffer->GetCurrentPtr();

    math::Vector4					lightPowScale( rUserData.m_RimPow, rUserData.m_RimScale, rUserData.m_PhongPow, rUserData.m_PhongScale );
    cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, s_LightPowScaleReg, *(nn::math::VEC4*)&lightPowScale );

    shaderStateMap.m_PhongLightingEnable = pMaterial->GetVertexColorEnable( 2 );
    shaderStateMap.m_RimLightingEnable = pMaterial->GetVertexColorEnable( 3 );
    pCommandBuffer->Put(cmd);
  }

  b32		fPokeShadowShaer = false;

  if ( rUserData.m_VertexShaderType == VertexShaderType::PressShadow ||
      rUserData.m_VertexShaderType == VertexShaderType::PressShadowTextureAlpha ||
			rUserData.m_VertexShaderType == VertexShaderType::PokeFireShadow ||
      rUserData.m_VertexShaderType == VertexShaderType::PressShadowYAlpha )
  {
    fPokeShadowShaer = true;
  }

  shaderStateMap.m_LightingEnable = false;
  if ( pAttributeParam->m_VsLightingEnable || rUserData.m_VertexShaderType.IsPokemonShader() || 
    fPokeShadowShaer //古いデータはVsLightingEnableが有効になっていないことが判明。よってシェーダータイプで判別する。
    )
  {
		if ( rUserData.m_VertexShaderType != VertexShaderType::PokeFire )
		{
			static const	u32			s_DirLightVecReg = 83;
			static const	u32			s_DirLightColorReg = 84;
			static const	u32			s_AmbLightColorReg = 85;

			u32						lightNum = rRenderState.GetLightNum();
			bit32					*cmd = pCommandBuffer->GetCurrentPtr();

			if ( lightNum )
				shaderStateMap.m_LightingEnable = true;

			for( u32 i = 0; i < lightNum; ++i )
			{
				const scenegraph::instance::LightNode			*lightNode = rRenderState.GetLightNode(i);			
				math::Vector4															col = lightNode->GetColor();
              
				switch (lightNode->GetLightType())
				{
				case LightType::Directional:
					{
						math::Vector4			dir = -lightNode->GetWorldDirection();
            
            if ( !rRenderState.GetSetUpEnable().VsIsWorldSpace )
            {
              math::Matrix44      viewMatrix = (*rRenderState.GetViewMatrix());
              dir.w = 0.0f;//平行移動をころす
              dir = viewMatrix * dir;
            }

						if( fPokeShadowShaer )
            {
							dir.w = s_PokeShadowOffSet;

              // 影描画でのGPUハング防止
              if (dir.y == 0.0f)
              {
                dir.y = 0.1f;
              }
            }

						cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, s_DirLightVecReg, *(nn::math::VEC4*)&dir );
						cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, s_DirLightColorReg, *(nn::math::VEC4*)&col );
					}
					break;
				case LightType::Ambient:
					{
						cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, s_AmbLightColorReg, *(nn::math::VEC4*)&col );
					}
					break;
				}

				if ( rUserData.m_VertexShaderType.IsPokemonShader() )
				{
					if ( shaderStateMap.m_InverseLightingEnable == false && shaderStateMap.m_PhongLightingEnable == false && shaderStateMap.m_RimLightingEnable == false )
						shaderStateMap.m_LightingEnable = false;
				}
			}

			pCommandBuffer->Put(cmd);
		}
  }

  ApplyFragmentLight( rRenderState, pMeshDrawTag, materialVertion );
}

void CombinerCtrShaderDriver::ApplyFragmentLight( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag, u32 materialVertion ) const
{
#if defined(GF_PLATFORM_CTR)
	nn::gr::CommandBufferJumpHelper																				*pCommandBuffer = GFGL::GetJumpHelper();
	const scenegraph::resource::MaterialResourceNode::AttributeParam			*pAttributeParam = m_pMaterial->GetAttributeParam();
	const scenegraph::resource::MaterialResourceNode											*pMaterial = m_pMaterial->GetMaterialResourceNode();
	const scenegraph::resource::MaterialResourceNode::UserData						&rUserData = pMaterial->GetUserData();
	LightParamCheck																												lightParamCheck;

	{
		lightParamCheck.fPsLightingEnable = pAttributeParam->m_PsLightingEnable;
		lightParamCheck.isEnableLutRefl = false;
		lightParamCheck.layerConfig = PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG0;
		lightParamCheck.bumpMapNo = pAttributeParam->m_BumpMapNo;
    lightParamCheck.MaterialAmbient = pAttributeParam->m_MaterialColor.m_Ambient;
    lightParamCheck.MaterialEmission = pAttributeParam->m_MaterialColor.m_Emission;

		if ( m_pMaterial->GetLutCommand( 0 ) || m_pMaterial->GetLutCommand( 1 ) || m_pMaterial->GetLutCommand( 2 ) )
		{
			lightParamCheck.isEnableLutRefl = true;

			if ( m_pMaterial->GetLutCommand( 1 ) || m_pMaterial->GetLutCommand( 2 ) )
			{
				lightParamCheck.layerConfig = ( PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG4 );
			}
		}

		u32 lightNum = rRenderState.GetLightNum();

		for( u32 i = 0; i < lightNum; ++i )
		{
			lightParamCheck.lightNodeList[i] = rRenderState.GetLightNode(i);
		}
	}

	if ( s_LightParamCheckCache == lightParamCheck )
		return;

	s_LightParamCheckCache = lightParamCheck;

	nn::gr::FragmentLight																									fragLight;
	const math::Matrix34																									*pViewMatrix = rRenderState.GetViewMatrix();
	math::Matrix34																												viewRotMatrix( *pViewMatrix );

	viewRotMatrix[0][3] = 0;
	viewRotMatrix[1][3] = 0;
	viewRotMatrix[2][3] = 0;

	for( u32 i = 0; i < nn::gr::FragmentLight::LIGHT_SOURCE_MAX; ++i )
		fragLight.isEnable[i] = false;
	
	if ( lightParamCheck.fPsLightingEnable )
	{
		fragLight.isEnableLutRefl = lightParamCheck.isEnableLutRefl;
		fragLight.layerConfig = (PicaDataFragLightEnvLayerConfig)lightParamCheck.layerConfig;

		fragLight.source[0].specular0R = 0;
		fragLight.source[0].specular0G = 0;
		fragLight.source[0].specular0B = 0;
		fragLight.source[0].specular1R = 255;
		fragLight.source[0].specular1G = 255;
		fragLight.source[0].specular1B = 255;
		
		fragLight.fresnelSelector = ( PICA_DATA_FRAG_LIGHT_ENV_NO_FRESNEL );
		fragLight.shadowSelector = ( PICA_DATA_FRAG_LIGHT_ENV_TEXTURE0 );
		fragLight.bumpMode = ( PICA_DATA_FRAG_LIGHT_ENV_BUMP_NOT_USED_DMP );		

		if ( lightParamCheck.bumpMapNo >= 0 )
		{
			fragLight.bumpMode = ( PICA_DATA_FRAG_LIGHT_ENV_BUMP_AS_BUMP_DMP );
			fragLight.bumpSelector = ( PICA_DATA_FRAG_LIGHT_ENV_TEXTURE2 );
		}
			
		fragLight.isEnableClampHighLights = false;

		//-------- ライト情報 --------------	
		u32 lightNum = rRenderState.GetLightNum();

		for( u32 i = 0; i < lightNum; ++i )
		{
			const scenegraph::instance::LightNode* lightNode = lightParamCheck.lightNodeList[i];
				
			switch (lightNode->GetLightType())
			{
			case LightType::Directional:
				{
					math::Vector3			pos = -lightNode->GetWorldDirection();
					pos = viewRotMatrix * pos;
					pos = pos.Normalize();

					fragLight.source[i].SetPosition( pos.x, pos.y, pos.z, true );
					fragLight.source[i].diffuseR = static_cast<u8>( lightNode->GetColor().x * 255.0f );
					fragLight.source[i].diffuseG = static_cast<u8>( lightNode->GetColor().y * 255.0f );
					fragLight.source[i].diffuseB = static_cast<u8>( lightNode->GetColor().z * 255.0f );
					fragLight.isEnable[i] = true;
				}
				break;
			case LightType::Ambient:
				if ( rUserData.m_VertexShaderType.IsPokemonShader() == false )
				{
          if ( materialVertion >= GfModelVersion::SupportMaterialColor15101716 )
          {
            math::Vector4         ambientColor = lightNode->GetColor();
            math::Vector4         materialAmbient = pAttributeParam->m_MaterialColor.m_Ambient.GetColor();

            ambientColor.x *= materialAmbient.x;
            ambientColor.y *= materialAmbient.y;
            ambientColor.z *= materialAmbient.z;

            ambientColor += pAttributeParam->m_MaterialColor.m_Emission.GetColor();

            for( u32 elm = 0; elm < 3; ++elm )
            {
              if ( ambientColor[elm] > 1.0f )
                ambientColor[elm] = 1.0f;
            }

            fragLight.globalAmbientR = static_cast<u8>( ambientColor.x * 255.0f );
					  fragLight.globalAmbientG = static_cast<u8>( ambientColor.y * 255.0f );
					  fragLight.globalAmbientB = static_cast<u8>( ambientColor.z * 255.0f );
          }
          else
          {
					  fragLight.globalAmbientR = static_cast<u8>( lightNode->GetColor().x * 255.0f );
					  fragLight.globalAmbientG = static_cast<u8>( lightNode->GetColor().y * 255.0f );
					  fragLight.globalAmbientB = static_cast<u8>( lightNode->GetColor().z * 255.0f );
          }
				}
				break;
			}
		}
	}

  bit32* cmd = pCommandBuffer->GetCurrentPtr();

	cmd = fragLight.MakeLightEnvCommand(cmd);
  cmd = fragLight.MakeLightSourceCommand(cmd);

  pCommandBuffer->Put(cmd);
#endif
}

void CombinerCtrShaderDriver::ApplyFog( RenderState& rRenderState, const scenegraph::resource::MaterialResourceNode::AttributeParam* pAttributeParam ) const
{
  scenegraph::instance::DrawEnvNode* pDrawEnvNode = rRenderState.GetCurrentFogNode();
  if(pDrawEnvNode)
  {
    if(pDrawEnvNode->GetFogEnable())
    {
      if(pAttributeParam->m_FogEnable)
      {
        if(rRenderState.GetCurrentFogCommandPutFlag())
        {
          GFGL::AddCommandJump( reinterpret_cast<const u32*>( m_pFogEnableCommand->GetStartPtr() ), m_pFogEnableCommand->GetSize() );
        }
        else
        {
          GFGL::AddCommandJump( reinterpret_cast<const u32*>( m_pFogEnableCommand->GetStartPtr() ), m_pFogEnableCommand->GetSize() );
          gfx::ctr::CTRCommandBufferUtil::SwapCommandBufferAndJumpToFrontBuffer( pDrawEnvNode->GetFogColorCommandCache() );
          gfx::ctr::CTRCommandBufferUtil::SwapCommandBufferAndJumpToFrontBuffer( pDrawEnvNode->GetFogTableCommandCache() );
          rRenderState.SetCurrentFogCommandPutFlag(true);
        }
      }
      else
      {
        GFGL::AddCommandJump( reinterpret_cast<const u32*>( m_pFogDisableCommand->GetStartPtr() ), m_pFogDisableCommand->GetSize() );
      }
    }
    else
    {
      GFGL::AddCommandJump( reinterpret_cast<const u32*>( m_pFogDisableCommand->GetStartPtr() ), m_pFogDisableCommand->GetSize() );
    }
  }
  else
  {
    GFGL::AddCommandJump( reinterpret_cast<const u32*>( m_pFogDisableCommand->GetStartPtr() ), m_pFogDisableCommand->GetSize() );
  }
}

void CombinerCtrShaderDriver::ApplyDepthStencil() const
{
  gfx::ctr::CTRDepthStencilStateObject* pCTRDepthStencilStateObject = static_cast<gfx::ctr::CTRDepthStencilStateObject*>( m_pMaterial->GetDepthStencilStateObject() );
  gfx::ctr::CTRCommandBufferUtil::SwapCommandBufferAndJumpToFrontBuffer( pCTRDepthStencilStateObject->GetCommandCache() );
}

void CombinerCtrShaderDriver::ApplyBlendState() const
{
  gfx::ctr::CTRBlendStateObject* pCTRBlendStateObject = static_cast<gfx::ctr::CTRBlendStateObject*>( m_pMaterial->GetBlendStateObject() );
  gfx::ctr::CTRCommandBufferUtil::SwapCommandBufferAndJumpToFrontBuffer( pCTRBlendStateObject->GetCommandCache() );
}

void CombinerCtrShaderDriver::ApplyUVMatrix( nn::gr::CommandBufferJumpHelper* pCommandBuffer, RenderState& rRenderState, u32 materialVertion ) const
{
  u32			texNum = m_pMaterial->GetTextureNum();
  bit32		*cmd = pCommandBuffer->GetCurrentPtr();

  for( u32 texNo = 0; texNo < texNum; ++texNo )
  {
    const MaterialResourceNode::TextureInfo *pInfo = m_pMaterial->GetTextureInfo( texNo );
		const MaterialResourceNode::TextureInfo *pInfoOverride = rRenderState.GetTextureInfoOverride( texNo );

    if (pInfo->m_SlotNo >= CombinerCtrShaderDriver::UvMax)
    {// スロット番号が3以上だったらカラーテクスチャ以外の用途なので除外
      continue;
    }

    const MaterialResourceNode::TextureInfo::Attribute			*pAttr = &pInfo->m_Attribute;
    u32																											uvRegStart = 11;

		if ( pInfoOverride )
			pAttr = &pInfoOverride->m_Attribute;

    if ( materialVertion >= GfModelVersion::VertexShaderUpdate15031412 )
      uvRegStart = 1;

    if (pAttr->m_Rotate != 0.0f)
    {
      // 回転ある場合
      // @attention あとで最適化
      math::Matrix44				uvMat;
      math::Matrix44				centerMat = math::Matrix44::GetTranslation( math::Vector3( -0.5f, -0.5f, 0.0f) );
      math::Matrix44				rotMat = math::Matrix44::GetRotationZ( pAttr->m_Rotate );
      math::Matrix44				offSetMat = math::Matrix44::GetTranslation( math::Vector3( 0.5f, 0.5f, 0.0f) );
      math::Matrix44				scaleMat = math::Matrix44::GetScaling( math::Vector3(pAttr->m_ScaleU, pAttr->m_ScaleV, 1.0f) );
      math::Matrix44				transMat = math::Matrix44::GetTranslation( math::Vector3( -pAttr->m_TranslateU, -pAttr->m_TranslateV, 0.0f) );//Mayaはテクスチャが移動するので逆スクロール
    
      uvMat = transMat * scaleMat * offSetMat * rotMat * centerMat;

      {
        u32			regNo = uvRegStart + (pInfo->m_SlotNo * 3);
        nn::math::MTX34		*pMtx34 = reinterpret_cast<nn::math::MTX34*>(&uvMat);
        cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, regNo, *pMtx34 );
      }
    }
    else
    {
      f32 uvMat[3][4] =
      {
        pAttr->m_ScaleU, 0.0f,           0.0f, -pAttr->m_TranslateU * pAttr->m_ScaleU,
        0.0f,           pAttr->m_ScaleV, 0.0f, -pAttr->m_TranslateV * pAttr->m_ScaleV,
        0.0f,           0.0f,           1.0f, 0.0f
      };
      u32			regNo = uvRegStart + (pInfo->m_SlotNo * 3);
      cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, regNo, *(nn::math::MTX34*)&uvMat[0][0] );
    }
  }
  pCommandBuffer->Put(cmd);
}

void CombinerCtrShaderDriver::ApplyShader( b32 fMaterialSetUp, b32 fVsSetUp, b32 fPsSetUp, RenderState::SetUpEnable setUpState, const scenegraph::resource::MaterialResourceNode* pMaterial ) const
{
  if ( fVsSetUp && setUpState.VertexShader )
    GFGL::AddCommandJump( reinterpret_cast<const u32*>( m_pVertexShader->GetVertexShader() ), m_pVertexShader->GetVertexShaderSize() );

  if ( fMaterialSetUp && setUpState.Material )
  {
    GFGL::AddCommandJump( reinterpret_cast<const u32*>( pMaterial->GetMaterialCommand() ), pMaterial->GetMaterialCommandSize() );
  }

  if ( fPsSetUp && setUpState.FragmentShader )
    GFGL::AddCommandJump( reinterpret_cast<const u32*>( m_pPixelShader->GetFragmentShader() ), m_pPixelShader->GetFragmentShaderSize() );
}

void CombinerCtrShaderDriver::ApplyShaderAnim( nn::gr::CommandBufferJumpHelper* pCommandBuffer, u32 materialVertion, RenderState::SetUpEnable setUpState, const scenegraph::resource::MaterialResourceNode* pMaterial, const scenegraph::resource::MaterialResourceNode::AttributeParam* pAttributeParam, const MeshDrawTag* pMeshDrawTag ) const
{
  const scenegraph::resource::MaterialResourceNode::UserData						&rUserData = pMaterial->GetUserData();

	switch( rUserData.m_VertexShaderType )
	{
	case VertexShaderType::PokeFire:
	case VertexShaderType::PokeFireShadow:		
		{
			static const	u32			s_DirLightColorReg = 84;
			static const	u32			s_AmbLightColorReg = 85;

			bit32									*cmd = pCommandBuffer->GetCurrentPtr();

			math::Vector4					meshScale( 0.0f, 0.0f, 0.0f, 0.0f );
			math::Vector4					uvOffSet( 0.0f, 0.0f, 0.125f, 1.0f );
			math::Vector3					scale = pMeshDrawTag->GetWorldMatrix()->GetElemScaling();
      
      //meshScale.x = rUserData.m_ShaderParam0 * scale.MinElem();とするとgfl2_RenderingEngine.aが1272Byte増える。
      {//この書き方なら248Byte増加で収まります。
        f32                   min_scale = scale.x;

        if( min_scale > scale.y )
        {
          min_scale = scale.y;
        }

        if( min_scale > scale.z )
        {
          min_scale = scale.z;
        }

        meshScale.x = rUserData.m_ShaderParam0 * min_scale;//ビルボードの大きさ(スケール)
      }

			uvOffSet.z = (1.0f / rUserData.m_ShaderParam2);//rUserData.m_ShaderParam2はパターン数

			cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, s_AmbLightColorReg, *(nn::math::VEC4*)&meshScale );
			cmd = nn::gr::CTR::MakeUniformCommandGS( cmd, s_DirLightColorReg, *(nn::math::VEC4*)&uvOffSet );

			pCommandBuffer->Put(cmd);
		}
		break;
	case VertexShaderType::MegaGangar:
		{
			static const	u32			s_MegaGangarReg = 84;

			bit32									*cmd = pCommandBuffer->GetCurrentPtr();

			math::Vector4					effectParam( 0.0f, 0.0f, 0.0f, 0.0f );
			math::Vector3					scale = pMeshDrawTag->GetWorldMatrix()->GetElemScaling();

      /*
      シェーダーコードは以下の様になっています。

      (頂点Ｙ-effectParam.z) / effectParam.y
      です。

      #define cMegaGangarShadingHieght      	c84.y
      #define cMegaGangarShadingStartY      	c84.z

      sub     rTemp.y,        WRLD_POSI.y,      cMegaGangarShadingStartY    //rTemp.y = 開始地点からの高さ(c84.z)
      rcp     rTemp.x,        cMegaGangarShadingHieght                      //rTemp.x = 1 / cMegaGangarShadingHieght(c84.y)
      mul     rLightingCol.x, rTemp.y,          rTemp.x                     //rLightingCol.x = 開始地点からの高さ / cMegaGangarShadingHieght(c84.y)
      */
			effectParam.y = (rUserData.m_ShaderParam3 * scale.x);//エフェクトの高さ
			effectParam.z = rUserData.m_ShaderParam1;//エフェクト開始位置(地面の位置)

			cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, s_MegaGangarReg, *(nn::math::VEC4*)&effectParam );

			pCommandBuffer->Put(cmd);
		}
		break;
	}

  if ( setUpState.Material )
  {
    for( u32 no = 0; no < MaterialResourceNode::LUT_MAX; ++no )
    {
      const u32				*pLutCommand = m_pMaterial->GetLutCommand( no );
      if ( pLutCommand )
        GFGL::AddCommandJump( pLutCommand, m_pMaterial->GetLutCommandSize( no ) );
    }
  }

  if ( materialVertion >= GfModelVersion::SupportConstantColorAnim )
  {//シェーダーアニメ
    bit32									*cmd = pCommandBuffer->GetCurrentPtr();
    static const u32			regNo[CombinerAttribute::CombinerMax] = { PICA_REG_TEX_ENV0_COLOR, PICA_REG_TEX_ENV1_COLOR, PICA_REG_TEX_ENV2_COLOR,
                                                      PICA_REG_TEX_ENV3_COLOR, PICA_REG_TEX_ENV4_COLOR, PICA_REG_TEX_ENV5_COLOR, };

    for( u32 i = 0; i < CombinerAttribute::CombinerMax; ++i )
    {
      s16										index = pAttributeParam->m_TextureCombinerConstant[i];
      const gfx::ColorU8		&rColor = pAttributeParam->m_ConstantColor[index];

      *cmd++ = PICA_CMD_DATA_TEX_ENV_CONST( rColor.r, rColor.g, rColor.b, rColor.a );
      *cmd++ = PICA_CMD_HEADER_SINGLE( regNo[i] );
    }

    pCommandBuffer->Put(cmd);
  }
}

#if GFL_DEBUG_PRINT
/**
  @brief  モデル情報のデバッグ表示
  @param  rRenderState        描画ステート
  @param  pMeshDrawTag        描画対象タグ
  @param  shaderStateMap      現在のシェーダーステート
  @param  pCommandBuffer      コマンドバッファ情報
  @param  materialVertion     マテリアルのバイナリバージョン
*/
void CombinerCtrShaderDriver::DumpModelData( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag, RenderState::ShaderStateMap& shaderStateMap, nn::gr::CommandBufferJumpHelper* pCommandBuffer, u32 materialVertion ) const
{
  const scenegraph::instance::JointInstanceNode				*pJointNode = rRenderState.GetJointInstanceNode();
  const scenegraph::resource::MeshResourceNode::FaceData			*pFace = pMeshDrawTag->GetFaceData();
  const scenegraph::resource::MaterialResourceNode*   pMaterual = pMeshDrawTag->GetMaterialResourceNode();
  const c8*                                           matName = pMaterual->GetName();
  const c8*                                           pVsName = pMaterual->GetShaderName( gfx::ShaderType::Vertex );
  const c8*                                           pFsName = pMaterual->GetShaderName( gfx::ShaderType::Pixel );

  {
    GFL_PRINT( "Data Error!!!!!\n" );

    GFL_PRINT( "Material Name = %s : %s : %s\n", matName, pVsName, pFsName );

    for( u32 jointNo = 0; jointNo < pFace->m_JointNum; ++jointNo )
    {
      scenegraph::DagNode*                        pParentNode = (scenegraph::DagNode*)( pJointNode[jointNo].GetParent() );
      scenegraph::instance::ModelInstanceNode     *pModelInstance = pParentNode->SafeCast<scenegraph::instance::ModelInstanceNode>();
          
      if ( pModelInstance )
      {
        const scenegraph::resource::GfMdlResourceNode*    pModelResource = pModelInstance->GetModelResourceNode();
        for( s32 meshNo = 0; meshNo < pModelResource->GetMeshNum(); ++meshNo )
        {
          const c8*   pMeshName = pModelResource->GetMeshName( meshNo );

          GFL_PRINT( "mesh[%d]%s\n", meshNo, pMeshName );
        }
        break;
      }
    }

    for( u32 jointNo = 0; jointNo < pFace->m_JointNum; ++jointNo )
    {
      const c8*                                   pParentJointName = pJointNode[jointNo].GetParentName();
      const c8*                                   pJointName = pJointNode[jointNo].GetName();

      GFL_PRINT( "Joint %s -> %s\n", pParentJointName, pJointName );
    }
  }
}
#endif  // GFL_DEBUG_PRINT

void CombinerCtrShaderDriver::ApplyJointMatrix( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag, RenderState::ShaderStateMap& shaderStateMap, nn::gr::CommandBufferJumpHelper* pCommandBuffer, u32 materialVertion ) const
{
  const scenegraph::instance::JointInstanceNode								*pJointNode = rRenderState.GetJointInstanceNode();
  
  if ( pJointNode )
  {
    bit32																												*cmd = pCommandBuffer->GetCurrentPtr();
    const scenegraph::resource::MeshResourceNode::FaceData			*pFace = pMeshDrawTag->GetFaceData();
    u32																													regNo = 25;

    if ( materialVertion >= GfModelVersion::VertexShaderUpdate15031412 )
      regNo = 10;

    for( u32 i = 0; i < pFace->m_JointNum; ++i )
    {
      u32			jointNo = pFace->m_pJointList[i];
      b32     isNeedRendering = pJointNode[jointNo].IsNeedRendering();

      #if GFL_DEBUG_PRINT
      if ( isNeedRendering == false )
      {
        DumpModelData( rRenderState, pMeshDrawTag, shaderStateMap, pCommandBuffer, materialVertion );
      }
      #endif  // GFL_DEBUG_PRINT

      GFL_ASSERT( isNeedRendering );

      const math::Matrix34* pSkiningMatrix = pJointNode[jointNo].GetSkinningMatrixPtr();
      cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, regNo, *(nn::math::MTX34*)pSkiningMatrix );

      regNo += 3;
    }

    pCommandBuffer->Put(cmd);
    shaderStateMap.m_IsSkining = true;
  }
  else
  {//world
    bit32														*cmd = pCommandBuffer->GetCurrentPtr();
    const math::Matrix34*						worldMat = rRenderState.GetWorldMatrix();
    u32															regNo = 0;

    if ( materialVertion >= GfModelVersion::VertexShaderUpdate15031412 )
      regNo = 10;

    cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, regNo, *(nn::math::MTX34*)worldMat);

    pCommandBuffer->Put(cmd);
    shaderStateMap.m_IsSkining = false;
  }
}

void CombinerCtrShaderDriver::ApplyViewSpaceJointMatrix( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag, RenderState::ShaderStateMap& shaderStateMap, nn::gr::CommandBufferJumpHelper* pCommandBuffer, u32 materialVertion ) const
{
  const scenegraph::instance::JointInstanceNode								*pJointNode = rRenderState.GetJointInstanceNode();
  math::Matrix44                                              viewInvMatrix = (*rRenderState.GetViewMatrix());

  if ( pJointNode )
  {
    bit32																												*cmd = pCommandBuffer->GetCurrentPtr();
    const scenegraph::resource::MeshResourceNode::FaceData			*pFace = pMeshDrawTag->GetFaceData();
    u32																													regNo = 25;

    if ( materialVertion >= GfModelVersion::VertexShaderUpdate15031412 )
      regNo = 10;

    for( u32 i = 0; i < pFace->m_JointNum; ++i )
    {
      u32			jointNo = pFace->m_pJointList[i];
      const math::Matrix34* pSkiningMatrix = pJointNode[jointNo].GetSkinningMatrixPtr();

      b32     isNeedRendering = pJointNode[jointNo].IsNeedRendering();

      #if GFL_DEBUG_PRINT
      if ( isNeedRendering == false )
      {
        DumpModelData( rRenderState, pMeshDrawTag, shaderStateMap, pCommandBuffer, materialVertion );
      }
      #endif  // GFL_DEBUG_PRINT

      GFL_ASSERT( isNeedRendering );

      math::Matrix44        viewSpaceMat = viewInvMatrix * math::Matrix44(*pSkiningMatrix);
      cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, regNo, *(nn::math::MTX34*)&viewSpaceMat );

      regNo += 3;
    }

    pCommandBuffer->Put(cmd);
    shaderStateMap.m_IsSkining = true;
  }
  else
  {//world
    bit32														*cmd = pCommandBuffer->GetCurrentPtr();
    const math::Matrix34*						worldMat = rRenderState.GetWorldMatrix();
    u32															regNo = 0;

    if ( materialVertion >= GfModelVersion::VertexShaderUpdate15031412 )
      regNo = 10;

    math::Matrix44        viewSpaceMat = viewInvMatrix * math::Matrix44(*worldMat);
    cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, regNo, *(nn::math::MTX34*)&viewSpaceMat);

    pCommandBuffer->Put(cmd);
    shaderStateMap.m_IsSkining = false;
  }
}
#endif

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
b32 CombinerCtrShaderDriver::Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
  if (pMeshDrawTag == NULL)
  {
    GFL_ASSERT(0);
    return false;
  }
#if defined(GF_PLATFORM_CTR)
	RenderState::ShaderStateMap																						shaderStateMap = rRenderState.GetShaderStateMap();
	RenderState::SetUpEnable																				rSetUpState = rRenderState.GetSetUpEnable();
	nn::gr::CommandBufferJumpHelper																				*pCommandBuffer = GFGL::GetJumpHelper();
	const scenegraph::resource::MaterialResourceNode											*pMaterial = m_pMaterial->GetMaterialResourceNode();
	
	u32																																		materialVertion = pMaterial->GetVersionId();
	const scenegraph::resource::MaterialResourceNode::AttributeParam			*pAttributeParam = m_pMaterial->GetAttributeParam();
  u32																																		materialHash = pAttributeParam->m_MaterialHash;
	const scenegraph::resource::MaterialResourceNode::UserData						&rUserData = pMaterial->GetUserData();

  b32				fMaterialSetUp = true;
	b32				fVsSetUp = true;
	b32				fPsSetUp = true;

	if ( materialVertion >= GfModelVersion::VertexShaderUpdate15031412 )
	{
		fMaterialSetUp = materialHash != s_MaterialCacheHash;
		s_MaterialCacheHash = materialHash;
	}

	fMaterialSetUp = true;

	if ( m_pVertexShader->GetVersionId() >= GfModelVersion::VertexShaderUpdate15031412 )
	{
		if ( rSetUpState.VertexShader )
		{
			u32					vsHash = m_pVertexShader->GetShaderHash(ShaderResourceNode::ShaderKind::Vertex);
			fVsSetUp = vsHash != s_VertexShaderCacheHash;
			s_VertexShaderCacheHash = vsHash;
			shaderStateMap.m_NormalMapgEnable = ( pAttributeParam->m_BumpMapNo >= 0 ) ? true : false;
		}
	}

	if ( m_pPixelShader )
	{
		if ( m_pPixelShader->GetVersionId() >= GfModelVersion::VertexShaderUpdate15031412 )
		{
			if ( rSetUpState.FragmentShader )
			{
				u32					psHash = m_pPixelShader->GetShaderHash(ShaderResourceNode::ShaderKind::Fragment);
				fPsSetUp = psHash != s_PixelShaderCacheHash;
				s_PixelShaderCacheHash = psHash;
			}
		}
	}
	else
	{
		fPsSetUp = false;
	}

  this->ApplyShader( fMaterialSetUp, fVsSetUp, fPsSetUp, rSetUpState, pMaterial );

	if ( fMaterialSetUp && rSetUpState.Lighting )
	{
    this->ApplyLight( rRenderState, pMeshDrawTag, shaderStateMap, pCommandBuffer, materialVertion, pMaterial, pAttributeParam, rUserData );
	}

	if ( rSetUpState.VertexShader )
	{
    if ( rSetUpState.VsIsWorldSpace )
    {
      this->ApplyJointMatrix( rRenderState, pMeshDrawTag, shaderStateMap, pCommandBuffer, materialVertion );
    }
    else
    {
      this->ApplyViewSpaceJointMatrix( rRenderState, pMeshDrawTag, shaderStateMap, pCommandBuffer, materialVertion );
    }
	}

  this->ApplyShaderAnim( pCommandBuffer, materialVertion, rSetUpState, pMaterial, pAttributeParam, pMeshDrawTag );

	if ( rSetUpState.UvSetting )
	{
    this->ApplyUVMatrix( pCommandBuffer, rRenderState, materialVertion );
	}

	rRenderState.SetShaderStateMap( shaderStateMap );

  //kawa20150527
  // ブレンドステートのアルファテスト
  this->ApplyBlendState();

  //kawa20150428
  // 深度ステンシルステートのデプステスト
  this->ApplyDepthStencil();
  
  //kawa20150507
  // フォグ
  this->ApplyFog(rRenderState, pAttributeParam);

#endif

	return true;
}


//kawa20150507
#if defined(GF_PLATFORM_CTR)
void CombinerCtrShaderDriver::createFogFixCommand(void)
{
  GFL_ASSERT( this->GetInstanceAllocator() );
  
  m_pFogEnableCommand  = gfx::ctr::CTRCommandBuffer<16>::Create();
  m_pFogDisableCommand = gfx::ctr::CTRCommandBuffer<16>::Create();

  // Enable
  {
    bit32* command = m_pFogEnableCommand->GetCurrentPtr();
    bool isEnableZFlip = false;
    *command++ = ( PICA_DATA_FOG ) | ( ( isEnableZFlip ? 1 : 0 ) << 16 );
    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_GAS_FOG_MODE, 0x5 );
    m_pFogEnableCommand->SetCurrentPtr(command);
    m_pFogEnableCommand->AddSubroutineCommand();
  }
  // Disable
  {
    bit32* command = m_pFogDisableCommand->GetCurrentPtr();
    command = nn::gr::Fog::MakeDisableCommand(command);
    m_pFogDisableCommand->SetCurrentPtr(command);
    m_pFogDisableCommand->AddSubroutineCommand();
  }
}
void CombinerCtrShaderDriver::destroyFogFixCommand(void)
{
  if(m_pFogDisableCommand)
  {
    delete m_pFogDisableCommand;
    m_pFogDisableCommand = NULL;
  }
  if(m_pFogEnableCommand)
  {
    delete m_pFogEnableCommand;
    m_pFogEnableCommand = NULL;
  }
}
#endif  // GF_PLATFORM_CTR


}}}//namespace gfl2::renderingengine::renderer


