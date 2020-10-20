#include <RenderingEngine/CLR/include/Renderer/gfl2_DefaultRenderingPipeLine_CLR.h>

namespace gfl2 { namespace renderingengine { namespace clr { namespace renderer {

DefaultRenderingPipeLine::DefaultRenderingPipeLine()
{
	m_pNative = new gfl2::renderingengine::renderer::DefaultRenderingPipeLine();
}

DefaultRenderingPipeLine::!DefaultRenderingPipeLine()
{
	if (m_pNative)
	{
		delete m_pNative;
		m_pNative = 0;
	}
}

// @node: ネイティブ側のDefaultRenderPathに追加してからラップ
/*
void DefaultRenderingPipeLine::AddEdgeRenderingTarget( gfl2::renderingengine::clr::scenegraph::instance::DrawableNode^ pNode )
{
	m_pNative->AddEdgeRenderingTarget(pNode->GetNative());
}

void DefaultRenderingPipeLine::AddDrawEnv( gfl2::renderingengine::clr::scenegraph::instance::DrawEnvNode^ pNode )
{
	m_pNative->AddDrawEnv(pNode->GetNative());
}
*/

void DefaultRenderingPipeLine::SetProjectionMatrix( gfl2::math::clr::Matrix^ mat )
{
	gfl2::math::Matrix				nativeMat;
	for(s32 col = 0; col < 4; ++col)
	{
		for(s32 row = 0; row < 4; ++row)
		{
			float elem = mat->GetElem(col, row);
			nativeMat.SetElem(col, row, elem);
		}
	}
	this->m_pNative->SetProjectionMatrix( nativeMat );
}

void DefaultRenderingPipeLine::SetViewMatrix( gfl2::math::clr::Matrix^ mat )
{
	math::Matrix				nativeMat;
	for(s32 col = 0; col < 4; ++col)
	{
		for(s32 row = 0; row < 4; ++row)
		{
			float elem = mat->GetElem(col, row);
			nativeMat.SetElem(col, row, elem);
		}
	}
	this->m_pNative->SetViewMatrix( nativeMat );
}

void DefaultRenderingPipeLine::SetRenderTarget(gfx::clr::Surface^ const pSurface)
{
	m_pNative->SetRenderTarget(pSurface->GetNative());
}

void DefaultRenderingPipeLine::SetDepthStencil(gfx::clr::Surface^ const pSurface)
{
	m_pNative->SetDepthStencil(pSurface->GetNative());
}

void DefaultRenderingPipeLine::Execute()
{
	m_pNative->Execute();
}

}}}}