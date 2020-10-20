//=============================================================================
/**
 * @file    gfl2_EdgeMapSceneRenderPath.h
 * @brief   EdgeMapSceneRenderPathクラス（アウトラインを生成するための情報をオフスクリーンバッファにレンダリングする描画パス）
 * @author  ishiguro_masateru
 * @date    2014
 */
//=============================================================================
#if !defined(POKE_3D_LIB_MODEL_EDGEMAPSCENERENDERPATH_H_INCLUDED)
#define POKE_3D_LIB_MODEL_EDGEMAPSCENERENDERPATH_H_INCLUDED
#pragma once

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>

namespace poke_3d { namespace renderer {

class EdgeMapSceneRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	class EdgeMapShaderDriver : public gfl2::gfx::GLMemory, public gfl2::renderingengine::renderer::IShaderDriver
	{
	public:

    class EdgeTarget{
	  public:
		  enum Enum{
        Both = 0,//法線エッジ＋ＩＤエッジ
			  Normal,  //法線エッジのみ
			  ID,      //ＩＤエッジのみ
			  Max
		  };
	  };

		EdgeMapShaderDriver();
		virtual ~EdgeMapShaderDriver();
    void Initialize( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum);

		virtual b32 Apply( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag = NULL ) const;
		virtual void SetTextureOverride( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag ) const;

		gfl2::gfx::Shader						*m_pVertexShader;
		gfl2::gfx::Shader						*m_pPixelShader;
		gfl2::gfx::ConstantBuffer		*m_pConstantBuffer;
    gfl2::gfx::BlendStateObject	*m_pBlendStateObject[EdgeTarget::Max];    // ブレンド・ステート・オブジェクト

		gfl2::renderingengine::scenegraph::resource::ResourceNode												*m_pBinModelRootNode;
		gfl2::renderingengine::scenegraph::instance::ModelInstanceNode									*m_pModelInstanceNode;
		gfl2::renderingengine::scenegraph::resource::TextureResourceNode								*m_pZInvTextureResourceNode;

		class TextureType{
		public:
			enum {
				NormalRGB = 0,//! 法線カラー
				InvNormalRGBA,//! 逆法線カラーとα255
				Max
			};
		};
	};

	class EdgeColor{
	public:
		enum Enum{
      None = -1,
			Normal = 0,
			ID,
			Max
		};
	};

	EdgeMapSceneRenderPath();
  EdgeMapSceneRenderPath(gfl2::gfx::IGLAllocator* pAllocator, int mdlNum);
	virtual ~EdgeMapSceneRenderPath();

  void LoadResources( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum);

	void CreateEdgeMapTexture( gfl2::gfx::IGLAllocator* pAllocator, f32 screanWidth, f32 screanHeight );

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	const gfl2::gfx::Texture* GetEdgeMapTexture( u32 type ){ return m_pEdgeMapTexture[type]; }

  void SetEnable( u32 edgeColor, b32 flag );
  b32 IsEnable( u32 edgeColor ) const;

private:
	virtual const gfl2::renderingengine::renderer::IShaderDriver* ShaderOverride();
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual void SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
	virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;
	EdgeMapShaderDriver																																							m_EdgeMapShaderDriver;
	gfl2::gfx::Texture																																							*m_pEdgeMapTexture[EdgeColor::Max];
  b32                                                                                             m_EdgeColorEnableTable[ EdgeColor::Max ];
};

}}

#endif