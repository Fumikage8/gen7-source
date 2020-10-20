#include "MyRenderingPipeLine.h"

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_GFGLUtil.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

namespace gfl2 { namespace renderingengine { namespace renderer { 

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
AABBRenderPath::AABBRenderPath() : IRenderPath()
{
	return;
	{
		VertexElement element[] = {
			{ 0, StreamUsage::Position,	0, VertexType::Float, 4 },
			{ 0, StreamUsage::TexCoord,	0, VertexType::Float, 4 },
			{ 0, StreamUsage::Color,		0, VertexType::Float, 4 },
			VertexElement::VERTEX_ELEMENT_END()
		};

		m_pVertexShader = GFGL::CreateVertexShaderFromFile( "C:/home/niji/git_gflib2/gfx/Sample/data/SimpleDraw.sh", "VS", element, NULL, FileFormat::ShaderCode );
		m_pPixelShader = GFGL::CreatePixelShaderFromFile( "C:/home/niji/git_gflib2/gfx/Sample/data/SimpleDraw.sh", "PS", NULL, FileFormat::ShaderCode );
	}

	{
		ConstantBufferElement element[] = {
			{ ConstantType::Matrix, 1 },
			ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
		};

		m_pConstantBuffer[0] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
		m_pConstantBuffer[1] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
		m_pConstantBuffer[2] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
	}

	{
		float									vscale = 0.9f;
		static const u32			vertexNum = 6 * 6;

		u16 idxVertexID[vertexNum];

		for( u32 i = 0; i < vertexNum; ++i )
			idxVertexID[i] = i;

		VertexFormat vertexDataArray[vertexNum];
		m_pVertexBuffer = GFGL::CreateVertexBuffer( sizeof(VertexFormat), vertexNum, Usage::WriteOnly, Pool::Default, vertexDataArray );
		m_pVertexBuffer->Update( vertexDataArray, sizeof(vertexDataArray) );

		m_pIndexBuffer = GFGL::CreateIndexBuffer( sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index16, idxVertexID );
	}

	m_pBlendStateObject = GFGL::CreateBlendStateObject();
	m_pBlendStateObject->UpdateState();

	m_RasterizerStateObject = GFGL::CreateRasterizerStateObject();
	m_RasterizerStateObject->SetCullMode( CullMode::None );
	m_RasterizerStateObject->SetFillMode( FillMode::Line );
	m_RasterizerStateObject->UpdateState();

	m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject();
	m_DepthStencilStateObject->SetDepthTestEnable(false);
	m_DepthStencilStateObject->UpdateState();

	m_SamplerStateObject = GFGL::CreateSamplerStateObject();
	m_SamplerStateObject->UpdateState();

	// **********************************************************

	ImageData				image;
	//image.ReadFromFile( "C:/home/niji/git_gflib2/gfx/Sample/data/dxt1_mip.dds" );
	image.ReadFromFile( "C:/home/niji/git_gflib2/gfx/Sample/data/black.tga" );

	m_pTexture = GFGL::CreateTextureFromImage( image, Pool::Default );
}

void AABBRenderPath::DrawAABB( const gfl2::math::AABB &rAABB )
{
	return;
	UpdateVertexBuffer( rAABB );

	// ***************************************
	GFGL::SetVertexShader( m_pVertexShader );
	GFGL::SetVertexShaderConstant( 0, m_pConstantBuffer[0] );
	GFGL::SetVertexShaderConstant( 1, m_pConstantBuffer[1] );
	GFGL::SetVertexShaderConstant( 2, m_pConstantBuffer[2] );

	GFGL::SetPixelShader( m_pPixelShader );
	GFGL::SetPixelShaderConstant( 0, m_pConstantBuffer[0] );
	GFGL::SetPixelShaderConstant( 1, m_pConstantBuffer[1] );
	GFGL::SetPixelShaderConstant( 2, m_pConstantBuffer[2] );

	GFGL::SetIndices( m_pIndexBuffer );
	GFGL::SetStreamSource( 0, m_pVertexBuffer );
		
	GFGL::SetBlendState( m_pBlendStateObject );
	GFGL::SetRasterizerState( m_RasterizerStateObject );
	GFGL::SetDepthStencilState( m_DepthStencilStateObject );
	GFGL::SetSamplerState( 0, m_SamplerStateObject );
	GFGL::SetTexture( 0, m_pTexture );

	GFGL::DrawIndexedPrimitive( PrimitiveType::Triangles, 0, 6 * 6 );
}

void AABBRenderPath::UpdateVertexBuffer( const math::AABB &rAABB )
{
	return;
	static const u32			vertexNum = 6 * 6;

	VertexFormat					vertexDataArray[vertexNum];

	//前
	vertexDataArray[0].pos.Set( rAABB.GetMin().x, rAABB.GetMax().y, rAABB.GetMin().z );
	vertexDataArray[1].pos.Set( rAABB.GetMax().x, rAABB.GetMax().y, rAABB.GetMin().z );
	vertexDataArray[2].pos.Set( rAABB.GetMin().x, rAABB.GetMin().y, rAABB.GetMin().z );

	vertexDataArray[3].pos.Set( rAABB.GetMax().x, rAABB.GetMax().y, rAABB.GetMin().z );
	vertexDataArray[4].pos.Set( rAABB.GetMin().x, rAABB.GetMin().y, rAABB.GetMin().z );
	vertexDataArray[5].pos.Set( rAABB.GetMax().x, rAABB.GetMin().y, rAABB.GetMin().z );

	//後ろ
	vertexDataArray[6].pos.Set( rAABB.GetMin().x, rAABB.GetMax().y, rAABB.GetMax().z );
	vertexDataArray[7].pos.Set( rAABB.GetMax().x, rAABB.GetMax().y, rAABB.GetMax().z );
	vertexDataArray[8].pos.Set( rAABB.GetMin().x, rAABB.GetMin().y, rAABB.GetMax().z );

	vertexDataArray[9].pos.Set( rAABB.GetMax().x, rAABB.GetMax().y, rAABB.GetMax().z );
	vertexDataArray[10].pos.Set( rAABB.GetMin().x, rAABB.GetMin().y, rAABB.GetMax().z );
	vertexDataArray[11].pos.Set( rAABB.GetMax().x, rAABB.GetMin().y, rAABB.GetMax().z );

	//右
	vertexDataArray[12].pos.Set( rAABB.GetMax().x, rAABB.GetMax().y, rAABB.GetMin().z );
	vertexDataArray[13].pos.Set( rAABB.GetMax().x, rAABB.GetMax().y, rAABB.GetMax().z );
	vertexDataArray[14].pos.Set( rAABB.GetMax().x, rAABB.GetMin().y, rAABB.GetMin().z );

	vertexDataArray[15].pos.Set( rAABB.GetMax().x, rAABB.GetMax().y, rAABB.GetMax().z );
	vertexDataArray[16].pos.Set( rAABB.GetMax().x, rAABB.GetMin().y, rAABB.GetMin().z );
	vertexDataArray[17].pos.Set( rAABB.GetMax().x, rAABB.GetMin().y, rAABB.GetMax().z );

	//左
	vertexDataArray[18].pos.Set( rAABB.GetMin().x, rAABB.GetMax().y, rAABB.GetMin().z );
	vertexDataArray[19].pos.Set( rAABB.GetMin().x, rAABB.GetMax().y, rAABB.GetMax().z );
	vertexDataArray[20].pos.Set( rAABB.GetMin().x, rAABB.GetMin().y, rAABB.GetMin().z );

	vertexDataArray[21].pos.Set( rAABB.GetMin().x, rAABB.GetMax().y, rAABB.GetMax().z );
	vertexDataArray[22].pos.Set( rAABB.GetMin().x, rAABB.GetMin().y, rAABB.GetMin().z );
	vertexDataArray[23].pos.Set( rAABB.GetMin().x, rAABB.GetMin().y, rAABB.GetMax().z );

	//上
	vertexDataArray[24].pos.Set( rAABB.GetMin().x, rAABB.GetMax().y, rAABB.GetMax().z );
	vertexDataArray[25].pos.Set( rAABB.GetMax().x, rAABB.GetMax().y, rAABB.GetMax().z );
	vertexDataArray[26].pos.Set( rAABB.GetMin().x, rAABB.GetMax().y, rAABB.GetMin().z );

	vertexDataArray[27].pos.Set( rAABB.GetMax().x, rAABB.GetMax().y, rAABB.GetMax().z );
	vertexDataArray[28].pos.Set( rAABB.GetMin().x, rAABB.GetMax().y, rAABB.GetMin().z );
	vertexDataArray[29].pos.Set( rAABB.GetMax().x, rAABB.GetMax().y, rAABB.GetMin().z );

	//下
	vertexDataArray[30].pos.Set( rAABB.GetMin().x, rAABB.GetMin().y, rAABB.GetMax().z );
	vertexDataArray[31].pos.Set( rAABB.GetMax().x, rAABB.GetMin().y, rAABB.GetMax().z );
	vertexDataArray[32].pos.Set( rAABB.GetMin().x, rAABB.GetMin().y, rAABB.GetMin().z );

	vertexDataArray[33].pos.Set( rAABB.GetMax().x, rAABB.GetMin().y, rAABB.GetMax().z );
	vertexDataArray[34].pos.Set( rAABB.GetMin().x, rAABB.GetMin().y, rAABB.GetMin().z );
	vertexDataArray[35].pos.Set( rAABB.GetMax().x, rAABB.GetMin().y, rAABB.GetMin().z );

	m_pVertexBuffer->Update( vertexDataArray, sizeof(vertexDataArray) );
}

void AABBRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	return;
	math::Matrix44		projMat = rDrawContext.m_Projection.Transpose();
	math::Matrix44		viewMat = rDrawContext.m_View.Transpose();
	math::Matrix		worldMat = math::Matrix::GetIdentity();

	m_pConstantBuffer[0]->Update(&projMat, sizeof(projMat));
	m_pConstantBuffer[1]->Update(&viewMat, sizeof(viewMat));
	m_pConstantBuffer[2]->Update(&worldMat, sizeof(worldMat));

	GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
	GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );

	GFGL::BeginScene();

	const gfx::Surface::SurfaceDesc			&rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
	GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);

	class LocalFunc{
	public:
		static void DrawAABB( const scenegraph::AABBTree* pTree, AABBRenderPath *pThis )
		{
			const scenegraph::AABBTree	*pChild = pTree->GetChildTree();

			if ( pChild )
			{
				for( u32 i = 0; i < scenegraph::AABBTree::ChildBoxNum; ++i )
				{
					LocalFunc::DrawAABB( &pChild[i], pThis );
				}
			}
			else
			{
				//if ( pTree->GetCullingObjectNodeTop() )
					pThis->DrawAABB( pTree->GetDrawableVolumeAABB() );
			}
		}

	};

	const scenegraph::AABBTree&		rAABBTree = scenegraph::SceneGraphManager::GetAABBTree();
	const scenegraph::AABBTree		*pSeek = rAABBTree.GetChildTree();

	for( u32 i = 0; i < scenegraph::AABBTree::ChildBoxNum; ++i )
	{
		LocalFunc::DrawAABB( &pSeek[i], this );
	}

	GFGL::EndScene();
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MyRenderingPipeLine::MyRenderingPipeLine()
{
	SceneRenderPath::InitDescription			desc;
	m_SceneRenderPath.Initialize( desc );
}

MyRenderingPipeLine::~MyRenderingPipeLine()
{

}

b32 MyRenderingPipeLine::StartRenderPath()
{
	m_RenderPathCnt = 0;
	return true;
}

RenderingPipeLine::IRenderPath* MyRenderingPipeLine::GetRenderPath()
{
	//前のパスの結果を踏まえて、パスをスキップすることも可能。

	switch( m_RenderPathCnt ){
	case 0:
		break;
	case 1:
		return &m_SceneRenderPath;
		break;
	case 2:
		return &m_AABBRenderPath;
		break;
	case 3:
		break;
	case 4:
		return &m_PresentPath;
		break;
	}
	return NULL;
}

b32 MyRenderingPipeLine::NextRenderPath()
{
	m_RenderPathCnt++;
	if ( m_RenderPathCnt == 5 )
		return false;
	return true;
}

void MyRenderingPipeLine::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{

}

void MyRenderingPipeLine::AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{
	m_SceneRenderPath.AddDrawEnv( pNode );
}

}}}
