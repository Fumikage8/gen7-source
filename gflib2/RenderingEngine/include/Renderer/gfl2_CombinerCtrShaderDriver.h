//=============================================================================
/**
 * @file    gfl2_CombinerCtrShaderDriver.h
 * @brief   CombinerCtrShaderDriverクラス（ＣＴＲに対してシェーダーとそのパラメーターを設定するクラス）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.5.09    ishiguro  未使用変数の警告対処
 */
//=============================================================================
#ifndef GFLIB2_RENDERINGENGINE_RENDERER_CombinerCtrShaderDriver_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_CombinerCtrShaderDriver_H_INCLUDED
#pragma once

#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/renderer/gfl2_IShaderDriver.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ShaderResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <debug/include/gfl2_Assert.h>

//kawa20150507
#if defined(GF_PLATFORM_CTR)
#include <gfx/include/CTR/gfl2_CTRCommandBufferManager.h>
#endif  // GF_PLATFORM_CTR


namespace gfl2 { namespace renderingengine { namespace renderer { 

//==============================================================================
/**
 * @class CombinerCtrShaderDriver
 * @brief ＣＴＲ用シェーダーパラメーター設定クラス
 * @date  2014
 */
//==============================================================================
class CombinerCtrShaderDriver : public gfx::GLMemory, public IShaderDriver
{
public:

	//static CombinerCtrShaderDriver* Create();
	static CombinerCtrShaderDriver* Create( gfx::IGLAllocator* pAllocator );
	static void CacheClear()
	{
		s_MaterialCacheHash = 0;
		s_VertexShaderCacheHash = 0;
		s_PixelShaderCacheHash = 0;
		s_LightParamCheckCache.Init();
	}

  // 立体視用の左目と右目のビュースペースでの差分行列を設定
  static void SetViewSpaceDiffMatrix(const gfl2::math::Matrix34& viewSpaceDiffMatrix)
  {
    s_ViewSpaceDiffMatrix = viewSpaceDiffMatrix;
  }
  // 立体視用の左目と右目のビュースペースでの差分行列を取得
  static const gfl2::math::Matrix34& GetViewSpaceDiffMatrix()
  {
    return s_ViewSpaceDiffMatrix;
  }

	virtual ~CombinerCtrShaderDriver();

	void SetMaterialInstanceNode( const scenegraph::instance::MaterialInstanceNode* pMaterial ){ GFL_ASSERT( pMaterial );m_pMaterial = pMaterial; }
	void SetVertexShaderResourceNode( const scenegraph::resource::ShaderResourceNode* pShader ){ GFL_ASSERT(pShader);m_pVertexShader = pShader; }
	void SetGeometryShaderResourceNode( const scenegraph::resource::ShaderResourceNode* pShader ){ GFL_ASSERT(pShader);m_pGeometryShader = pShader; }
	void SetPixelShaderResourceNode( const scenegraph::resource::ShaderResourceNode* pShader ){ m_pPixelShader = pShader; }

	const scenegraph::resource::ShaderResourceNode			*GetVertexShader() const { return m_pVertexShader; }
	const scenegraph::resource::ShaderResourceNode			*GetGeometryShader() const { return m_pGeometryShader; }
	const scenegraph::resource::ShaderResourceNode			*GetPixelShader() const { return m_pPixelShader; }

	virtual b32 Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const;

protected:

	static const int UvMax = 3;

	struct LightParamCheck
	{
		static const s32												s_LightMax = 8;

		b32																			fPsLightingEnable;
		b32																			isEnableLutRefl;
		u32																			layerConfig;
		s32																			bumpMapNo;
    gfx::ColorU8                            MaterialAmbient;
    gfx::ColorU8                            MaterialEmission;
		const scenegraph::instance::LightNode		*lightNodeList[s_LightMax];

		LightParamCheck()
		{
			Init();
		}

		void Init()
		{
		#if defined(GF_PLATFORM_CTR)
			fPsLightingEnable = false;
			isEnableLutRefl = false;
			layerConfig = PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG0;
			bumpMapNo = -1;
			for( s32 i = 0; i < s_LightMax; ++i )
				lightNodeList[i] = NULL;
		#endif
		}

		bool operator == ( const LightParamCheck& lhs )
		{
			static const u32		dwordSize = sizeof(LightParamCheck) / sizeof(u32);
			u32									*pLhs = (u32*)&lhs;
			u32									*pRhs = (u32*)this;

			for( u32 i = 0; i < dwordSize; ++i )
			{
				if( pLhs[i] != pRhs[i] )
					return false;
			}

			return true;
		}

		bool operator != ( const LightParamCheck& lhs )
		{
			return !(*this == lhs);
		}
	};

	CombinerCtrShaderDriver();

#if defined(NN_PLATFORM_CTR)
	void ApplyLight( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag, RenderState::ShaderStateMap& shaderStateMap, nn::gr::CommandBufferJumpHelper* pCommandBuffer, u32 materialVertion, const scenegraph::resource::MaterialResourceNode* pMaterial, const scenegraph::resource::MaterialResourceNode::AttributeParam* pAttributeParam, const scenegraph::resource::MaterialResourceNode::UserData& rUserData) const;
	void ApplyFragmentLight( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag, u32 materialVertion ) const;
	void ApplyFog( RenderState& rRenderState, const scenegraph::resource::MaterialResourceNode::AttributeParam* pAttributeParam ) const;
	void ApplyDepthStencil() const;
	void ApplyBlendState() const;
  void ApplyUVMatrix( nn::gr::CommandBufferJumpHelper* pCommandBuffer, RenderState& rRenderState, u32 materialVertion ) const;
  void ApplyShader( b32 fMaterialSetUp, b32 fVsSetUp, b32 fPsSetUp, RenderState::SetUpEnable setUpState, const scenegraph::resource::MaterialResourceNode* pMaterial ) const;
  void ApplyShaderAnim( nn::gr::CommandBufferJumpHelper* pCommandBuffer, u32 materialVertion, RenderState::SetUpEnable setUpState, const scenegraph::resource::MaterialResourceNode* pMaterial, const scenegraph::resource::MaterialResourceNode::AttributeParam* pAttributeParam, const MeshDrawTag* pMeshDrawTag ) const;
  void ApplyJointMatrix( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag, RenderState::ShaderStateMap& shaderStateMap, nn::gr::CommandBufferJumpHelper* pCommandBuffer, u32 materialVertion ) const;
  void ApplyViewSpaceJointMatrix( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag, RenderState::ShaderStateMap& shaderStateMap, nn::gr::CommandBufferJumpHelper* pCommandBuffer, u32 materialVertion ) const;
#endif

private:
  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  void createFogFixCommand(void);
  void destroyFogFixCommand(void);

#if GFL_DEBUG_PRINT
  /**
    @brief  モデル情報のデバッグ表示
    @param  rRenderState        描画ステート
    @param  pMeshDrawTag        描画対象タグ
    @param  shaderStateMap      現在のシェーダーステート
    @param  pCommandBuffer      コマンドバッファ情報
    @param  materialVertion     マテリアルのバイナリバージョン
  */
  void DumpModelData( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag, RenderState::ShaderStateMap& shaderStateMap, nn::gr::CommandBufferJumpHelper* pCommandBuffer, u32 materialVertion ) const;
#endif  // GFL_DEBUG_PRINT

#endif  // GF_PLATFORM_CTR

protected:

	const scenegraph::instance::MaterialInstanceNode		*m_pMaterial;
	const scenegraph::resource::ShaderResourceNode			*m_pVertexShader;
	const scenegraph::resource::ShaderResourceNode			*m_pGeometryShader;
	const scenegraph::resource::ShaderResourceNode			*m_pPixelShader;

private:
  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  // フォグの固定コマンド
  gfx::ctr::CTRCommandBuffer<16> *m_pFogEnableCommand;
  gfx::ctr::CTRCommandBuffer<16> *m_pFogDisableCommand;
#endif  // GF_PLATFORM_CTR

protected:

	static u32																					s_MaterialCacheHash;
	static u32																					s_VertexShaderCacheHash;
	static u32																					s_PixelShaderCacheHash;
	static LightParamCheck															s_LightParamCheckCache;
	static const f32																		s_PokeShadowOffSet;
  static gfl2::math::Matrix34                         s_ViewSpaceDiffMatrix;  // 立体視用の左目と右目のビュースペースでの差分行列
  
};

}}}

#endif

