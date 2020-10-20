#ifndef MyRenderingPipeLine_H_INCLUDED
#define MyRenderingPipeLine_H_INCLUDED

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

namespace gfl2 { namespace renderingengine { namespace renderer { 


class AABBRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
	AABBRenderPath();
	virtual ~AABBRenderPath(){}

private:
	struct VertexFormat{
		gfl2::math::Vector				pos;
		gfl2::math::Vector				uv;
		gfl2::math::Vector				color;
		VertexFormat(){}
		VertexFormat( gfl2::math::Vector pos_, gfl2::math::Vector uv_, gfl2::math::Vector color_ )
		{
			pos = pos_;
			uv = uv_;
			color = color_;
		}
	};

	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	void DrawAABB( const gfl2::math::AABB &rAABB );
	void UpdateVertexBuffer( const gfl2::math::AABB &rAABB );

	gfl2::gfx::IndexBuffer								*m_pIndexBuffer;
	gfl2::gfx::VertexBuffer								*m_pVertexBuffer;
	gfl2::gfx::Shader											*m_pVertexShader;   // 頂点シェーダ
	gfl2::gfx::Shader											*m_pPixelShader;    // ピクセル・シェーダ

	gfl2::gfx::BlendStateObject						*m_pBlendStateObject;    // ブレンド・ステート・オブジェクト
	gfl2::gfx::RasterizerStateObject			*m_RasterizerStateObject;// ラスタライザ・ステート・オブジェクト
	gfl2::gfx::DepthStencilStateObject		*m_DepthStencilStateObject;	// 深度/ステンシル・ステート・オブジェクト
	gfl2::gfx::SamplerStateObject					*m_SamplerStateObject;				// サンプラー・ステート・オブジェクト
	gfl2::gfx::ConstantBuffer							*m_pConstantBuffer[3];
	gfl2::gfx::Texture										*m_pTexture;    // テクスチャ
};

class MyRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
	MyRenderingPipeLine();
	virtual ~MyRenderingPipeLine();

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );

private:
	virtual b32 StartRenderPath();
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();

	u32																																		m_RenderPathCnt;
	
	gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath		m_SceneRenderPath;
	AABBRenderPath																												m_AABBRenderPath;
	gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath				m_PresentPath;

};

}}}
#endif
