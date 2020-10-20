
#include <Renderingengine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_UtilCollisionRenderPath.h>


using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
CollisionRenderPath::CollisionRenderPath() :
  IRenderPath(),
  m_pVertexFormatList(NULL),
  m_pCurrentTerrain(NULL),
  m_VertexFormatLimit(0)
{
}

CollisionRenderPath::~CollisionRenderPath()
{
  if (m_pVertexFormatList != NULL)
  {
    delete[] m_pVertexFormatList;
  }
}

void CollisionRenderPath::Initialize(const InitDescription& desc)
{
  // 初期化設定保存
  m_InitDescription = desc;

  // 頂点フォーマットリスト作成
  m_VertexFormatLimit = desc.maxTriangleCount * 3;
  m_pVertexFormatList = new DrawUtil::VertexFormat[m_VertexFormatLimit];
}

void CollisionRenderPath::SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  // レンダーターゲットの設定
	GFGL::SetRenderTarget( pRenderTarget );
	GFGL::SetDepthStencil( pDepthStencil );
}

void CollisionRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  (void)pRenderTarget;
  (void)pDepthStencil;

  // レンダーターゲットのクリアはしない
}

// JointInstanceNodeに対してのVisitor
class CollisionMeshNodeVisitor
{
public:
  CollisionMeshNodeVisitor() :
    desc(NULL),
    vertexList(NULL),
    vertexLimit(0),
    vertexCount(0),
    pCurrentTerrain(NULL)
  {
  }

  virtual ~CollisionMeshNodeVisitor()
  {
  }

  void Visit( scenegraph::resource::CollisionMeshResourceNode* pNode )
  {
    u32 triangleCount = pNode->GetCollisionTriangleCount();

    for (u32 i = 0; i < triangleCount; ++i)
    {
      const scenegraph::resource::CollisionTriangle* pTriangle = pNode->GetCollisionTriangle(i);

      math::Vector4 color = desc->terrainColor;
      if (pCurrentTerrain == pTriangle)
      {
        color = desc->currentTerrainColor;
      }

      // 少し小さくする
      math::Vector4 center = pTriangle->m_Positions[0] + pTriangle->m_Positions[1] + pTriangle->m_Positions[2];
      center /= 3.0f;

      DrawUtil::VertexFormat& vertex0 = vertexList[vertexCount++];
      vertex0.color = color;
      vertex0.pos = pTriangle->m_Positions[0];
      vertex0.pos += (center - vertex0.pos) * 0.01f;
        
      DrawUtil::VertexFormat& vertex1 = vertexList[vertexCount++];
      vertex1.color = color;
      vertex1.pos = pTriangle->m_Positions[1];
      vertex1.pos += (center - vertex1.pos) * 0.01f;
        
      DrawUtil::VertexFormat& vertex2 = vertexList[vertexCount++];
      vertex2.color = color;
      vertex2.pos = pTriangle->m_Positions[2];
      vertex2.pos += (center - vertex2.pos) * 0.01f;
    }
  }

  CollisionRenderPath::InitDescription* desc;                // 初期化設定
  DrawUtil::VertexFormat* vertexList;   // 頂点リスト
  u32 vertexLimit;                      // 頂点最大数

  u32 vertexCount;                      // 頂点数
  const scenegraph::resource::CollisionTriangle* pCurrentTerrain;
};

void CollisionRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  // シザー、カリング、フィルの設定
  DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
  DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

  // マテリアル設定
  DrawUtil::SetMaterialAlpha(0.3f);
  
  // 簡易描画のデプステストをありに設定する
  gfl2::gfx::DepthStencilStateObject::StateData stateData;
  stateData.m_DepthTestEnable = true;
  DrawUtil::SetDepthTestState(stateData);

  // 描画開始
	GFGL::BeginScene();
  DrawUtil::ClearScissor();

  // 簡易描画開始
  DrawUtil::BeginRender();

  // プロジェクション行列設定
  DrawUtil::SetProjectionMatrix(rDrawContext.m_Projection);
  // ビュー行列設定
  DrawUtil::SetViewMatrix(rDrawContext.m_View);
  // モデル行列設定
  DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());
  
  // ノードを巡回して頂点リストを作成する
  scenegraph::Traverser::Visitor<scenegraph::resource::CollisionMeshResourceNode, CollisionMeshNodeVisitor>				visitor;
  visitor.desc = &m_InitDescription;
  visitor.vertexList = m_pVertexFormatList;
  visitor.vertexLimit = m_VertexFormatLimit;
  visitor.vertexCount = 0;
  visitor.pCurrentTerrain = m_pCurrentTerrain;
  scenegraph::SceneGraphManager::Traverse<scenegraph::resource::CollisionMeshResourceNode, CollisionMeshNodeVisitor>( &visitor );

  // 頂点リスト描画
  DrawUtil::DrawTriangles(m_pVertexFormatList, visitor.vertexCount, false);

  // 描画終了
  DrawUtil::EndRender();
	GFGL::EndScene();
}

}}}}
