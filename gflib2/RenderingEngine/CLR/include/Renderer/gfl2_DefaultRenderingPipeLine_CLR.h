#ifndef GFLIB2_RENDERINGENGINE_CLR_RENDERER_MYRENDERINGPIPELINE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_RENDERER_MYRENDERINGPIPELINE_H_INCLUDED

//#include "RenderingEngine/Sample/DrawEnvSample/DefaultRenderingPipeLine.h"
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <math/CLR/include/gfl2_Matrix_CLR.h>
#include <gfx/CLR/include/gfl2_Surface_CLR.h>
#include <RenderingEngine/CLR/include/SceneGraph/Instance/gfl2_DrawableNode_CLR.h>
#include <RenderingEngine/CLR/include/SceneGraph/Instance/gfl2_DrawEnvNode_CLR.h>

namespace gfl2 { namespace renderingengine { namespace clr { namespace renderer {

public ref class DefaultRenderingPipeLine
{
	DefaultRenderingPipeLine();
	~DefaultRenderingPipeLine()
	{
		this->!DefaultRenderingPipeLine();
	}
	!DefaultRenderingPipeLine();

	gfl2::renderingengine::renderer::DefaultRenderingPipeLine* GetNative()
	{
		return m_pNative;
	}

	// @attention: ネイティブ側のDefaultRenderPathに追加してからラップ
	//void AddEdgeRenderingTarget( gfl2::renderingengine::clr::scenegraph::instance::DrawableNode^ pNode );
	//void AddDrawEnv( gfl2::renderingengine::clr::scenegraph::instance::DrawEnvNode^ pNode );

	void SetProjectionMatrix( gfl2::math::clr::Matrix^ mat );
	void SetViewMatrix( gfl2::math::clr::Matrix^ mat );

	void SetRenderTarget(gfx::clr::Surface^ const pSurface);
	void SetDepthStencil(gfx::clr::Surface^ const pSurface);

	void Execute();

private:
	gfl2::renderingengine::renderer::DefaultRenderingPipeLine* m_pNative;

};

}}}}

#endif