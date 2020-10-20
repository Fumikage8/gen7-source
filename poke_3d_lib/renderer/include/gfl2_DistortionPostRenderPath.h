#if !defined(POKE_3D_LIB_MODEL_DistortionPOSTRENDERPATH_H_INCLUDED)
#define POKE_3D_LIB_MODEL_DistortionPOSTRENDERPATH_H_INCLUDED
#pragma once

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>

namespace poke_3d { namespace renderer {

class DistortionPostRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:

	class DistortionPostShaderDriver : public gfl2::gfx::GLMemory, public gfl2::renderingengine::renderer::IShaderDriver
	{
	public:
		DistortionPostShaderDriver();
		~DistortionPostShaderDriver();

		virtual b32 Apply( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag = NULL ) const;
		virtual void SetTextureOverride( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag ) const;
    void Initialize( gfl2::gfx::IGLAllocator* pAllocator );

		void SetOverrideTexture( gfl2::gfx::Texture *pTexture ){ m_pFrameBufferTexture = pTexture; }

		gfl2::gfx::Texture					*m_pFrameBufferTexture;
    gfl2::gfx::ConstantBuffer		*m_pConstantBuffer;
	};

	DistortionPostRenderPath();
  DistortionPostRenderPath(gfl2::gfx::IGLAllocator* pAllocator, int mdlNum);
	virtual ~DistortionPostRenderPath();

  void CreateBuffer(gfl2::gfx::IGLAllocator* pAllocator, int mdlNum);
	void AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );
	void RemoveDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );
	void RemoveAllDrawEnv();

	void AddDistortionRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveDistortionRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

	void SetOverrideTexture( gfl2::gfx::Texture *pTexture ){ m_DistortionPostShaderDriver.SetOverrideTexture( pTexture ); }

private:

	virtual const gfl2::renderingengine::renderer::IShaderDriver* ShaderOverride();
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();

	virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil ){}

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;
	DistortionPostShaderDriver																																			m_DistortionPostShaderDriver;
	
};

}}

#endif