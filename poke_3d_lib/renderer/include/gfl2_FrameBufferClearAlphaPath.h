#if !defined(POKE_3D_LIB_MODEL_FrameBufferClearAlphaPath_H_INCLUDED)
#define POKE_3D_LIB_MODEL_FrameBufferClearAlphaPath_H_INCLUDED
#pragma once

#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <renderer/include/gfl2_EdgeMapSceneRenderPath.h>

namespace poke_3d { namespace renderer {

class FrameBufferClearAlphaPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:

	FrameBufferClearAlphaPath();
	virtual ~FrameBufferClearAlphaPath();

  void Initialize( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8* vertexShaderBinary, c8* fragmentShaderBinary );

private:
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

  gfl2::renderingengine::scenegraph::resource::ResourceNode												*m_pBinModelRootNode;
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode									*m_pModelInstanceNode;
};

}}

#endif