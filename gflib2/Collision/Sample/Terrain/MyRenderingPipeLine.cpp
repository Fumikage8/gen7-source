#include "MyRenderingPipeLine.h"

#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>
#include <math/include/gfl2_MathUtil.h>
#include <gfx/include/gfl2_GFGLUtil.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

void PointRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  // シザー、カリング、フィルの設定
  DrawUtil::ClearScissor();
  DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
  DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

  // マテリアル設定
  DrawUtil::SetMaterialAlpha(1.0f);

  // 簡易描画のデプステストを無しに設定する
  DrawUtil::SetDepthTestEnable(false);

  // 描画開始
  GFGL::BeginScene();

  // 簡易描画開始
  DrawUtil::BeginRender();

  // プロジェクション行列設定
  DrawUtil::SetProjectionMatrix(rDrawContext.m_Projection);
  // ビュー行列設定
  DrawUtil::SetViewMatrix(rDrawContext.m_View);
  // モデル行列設定
  DrawUtil::SetModelMatrix(gfl2::math::Matrix::GetIdentity());

#if 0
  // JointInstanceNodeに対してのVisitor
  class CollisionMeshNodeVisitor
  {
  public:
    CollisionMeshNodeVisitor() {}
    virtual ~CollisionMeshNodeVisitor() {}

    void Visit( scenegraph::resource::CollisionMeshResourceNode* pNode )
    {
      u32 triangleCount = pNode->GetCollisionTriangleCount();

      for (u32 i = 0; i < triangleCount; ++i)
      {
        const scenegraph::resource::CollisionTriangle* pTriangle = pNode->GetCollisionTriangle(i);

        math::Vector color = desc->terrainColor;
        if (pCurrentTerrain == pTriangle)
        {
          color = desc->currentTerrainColor;
        }

        // 少し小さくする
        math::Vector center = pTriangle->m_Positions[0] + pTriangle->m_Positions[1] + pTriangle->m_Positions[2];
        center /= 3.0f;

        DrawUtil::VertexFormat& vertex0 = vertexList[vertexCount++];
        vertex0.color = color;
        vertex0.pos = pTriangle->m_Positions[0];
        vertex0.pos += (center - vertex0.pos) * 0.01f;
        //vertex0.pos += pTriangle->m_Normal;

        DrawUtil::VertexFormat& vertex1 = vertexList[vertexCount++];
        vertex1.color = color;
        vertex1.pos = pTriangle->m_Positions[1];
        vertex1.pos += (center - vertex1.pos) * 0.01f;
        //vertex1.pos += pTriangle->m_Normal;

        DrawUtil::VertexFormat& vertex2 = vertexList[vertexCount++];
        vertex2.color = color;
        vertex2.pos = pTriangle->m_Positions[2];
        vertex2.pos += (center - vertex2.pos) * 0.01f;
        //vertex2.pos += pTriangle->m_Normal;
      }
    }

    InitDescription* desc;                // 初期化設定
    DrawUtil::VertexFormat* vertexList;   // 頂点リスト
    u32 vertexLimit;                      // 頂点最大数

    u32 vertexCount;                      // 頂点数
    const scenegraph::resource::CollisionTriangle* pCurrentTerrain;
  };

  // ノードを巡回して頂点リストを作成する
  scenegraph::Traverser::Visitor<scenegraph::resource::CollisionMeshResourceNode, CollisionMeshNodeVisitor>				visitor;
  visitor.desc = &m_InitDescription;
  visitor.vertexList = m_pVertexFormatList;
  visitor.vertexLimit = m_VertexFormatLimit;
  visitor.vertexCount = 0;
  visitor.pCurrentTerrain = m_pCurrentTerrain;
  scenegraph::SceneGraphManager::Traverse<scenegraph::resource::CollisionMeshResourceNode, CollisionMeshNodeVisitor>( &visitor );

  // 頂点リスト描画
  DrawUtil::DrawVertices(PrimitiveType::Triangles, m_pVertexFormatList, visitor.vertexCount, false);
#endif

  gfl2::math::Vector color(0,0,1);
  gfl2::math::Vector uv;
  gfl2::math::Vector vL(-10, 0, 0);
  gfl2::math::Vector vR(10, 0, 0);
  gfl2::math::Vector vF(0, 0, 10);
  gfl2::math::Vector vB(0, 0, -10);
  gfl2::math::Vector vU(0, 10, 0);
  gfl2::math::Vector vD(0, -10, 0);

  gfl2::gfx::DrawUtil::VertexFormat vertexList[] =
  {
    gfl2::gfx::DrawUtil::VertexFormat(m_CharaPos + vL, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_CharaPos + vR, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_CharaPos + vF, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_CharaPos + vB, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_CharaPos + vU, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_CharaPos + vD, color, uv),

    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosL + vL, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosL + vR, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosL + vF, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosL + vB, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosL + vU, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosL + vD, color, uv),

    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosR + vL, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosR + vR, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosR + vF, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosR + vB, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosR + vU, color, uv),
    gfl2::gfx::DrawUtil::VertexFormat(m_FootPosR + vD, color, uv),
  };

  DrawUtil::DrawVertices(PrimitiveType::Lines, vertexList, sizeof(vertexList) / sizeof(vertexList[0]), false);

  // 描画終了
  DrawUtil::EndRender();
  GFGL::EndScene();
}


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MyRenderingPipeLine::MyRenderingPipeLine( f32 screanWidth, f32 screanHeight )
{
  // シーン描画パス初期化
  {
    SceneRenderPath::InitDescription desc;
    m_SceneRenderPath.Initialize(desc);
  }

  // コリジョン描画パス初期化
  {
    renderer::util::CollisionRenderPath::InitDescription desc;
    m_CollisionRenderPath.Initialize(desc);
  }
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
		return &m_SceneRenderPath;
  case 1:
    return &m_CollisionRenderPath;
    //return NULL;
  case 2:
    return &m_PointRenderPath;
    //return NULL;
	case 3:
		return &m_PresentPath;
	}
	return NULL;
}

b32 MyRenderingPipeLine::NextRenderPath()
{
	m_RenderPathCnt++;
	if ( m_RenderPathCnt == 4 )
		return false;
	return true;
}


