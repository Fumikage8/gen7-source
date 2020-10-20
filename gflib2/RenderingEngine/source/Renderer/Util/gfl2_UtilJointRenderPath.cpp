#if defined(GF_PLATFORM_WIN32)

#include <Renderingengine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_UtilJointRenderPath.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
JointRenderPath::JointRenderPath() :
  IRenderPath(),
  m_pVertexFormatList(NULL)
{
}

JointRenderPath::~JointRenderPath()
{
  if (m_pVertexFormatList != NULL)
  {
    delete[] m_pVertexFormatList;
    m_pVertexFormatList = NULL;
  }
}

void JointRenderPath::Initialize(const InitDescription& desc)
{
  // 初期化設定保存
  m_InitDescription = desc;

  // 頂点フォーマットリスト作成
  m_VertexFormatLimit = desc.maxJointCount * 22;
  m_pVertexFormatList = new DrawUtil::VertexFormat[m_VertexFormatLimit];
}

void JointRenderPath::SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  // レンダーターゲットの設定
	GFGL::SetRenderTarget( pRenderTarget );
	GFGL::SetDepthStencil( pDepthStencil );
}

void JointRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  (void)pRenderTarget;
  (void)pDepthStencil;

  // レンダーターゲットのクリアはしない
}

// JointInstanceNodeに対してのVisitor
class JointInstanceNodeVisitor
{
public:
  JointInstanceNodeVisitor() {}
  virtual ~JointInstanceNodeVisitor() {}

  void Visit( scenegraph::instance::JointInstanceNode* pNode )
  {
    scenegraph::instance::JointInstanceNode* pParent = pNode->GetParent()->SafeCast<scenegraph::instance::JointInstanceNode>();
    if (pParent == NULL)
    {
      return;
    }

    // @note このままだと全モデルのジョイントを描画してしまうのでカリング処理を行う
      
    // ノードの行列、親の行列を取得
    const math::Matrix34& nodeMatrix = pNode->GetWorldMatrix();
    const math::Matrix34& parentMatrix = pParent->GetWorldMatrix();

    gfl2::math::Vector3 nodePos = nodeMatrix.GetElemPosition();
    gfl2::math::Vector3 parentPos = parentMatrix.GetElemPosition();

    // 頂点数オーバー確認
    GFL_ASSERT(vertexCount + 22 < vertexLimit);

    // 四角錘

    // 親のXYZ軸
    gfl2::math::Vector3 axisX = parentMatrix.GetElemAxisX();
    gfl2::math::Vector3 axisY = parentMatrix.GetElemAxisY();
    gfl2::math::Vector3 axisZ = parentMatrix.GetElemAxisZ();

    axisX *= desc->boneScale;
    axisY *= desc->boneScale;
    axisZ *= desc->boneScale;

    gfl2::math::Vector3 quadPos0 = parentPos + axisY;
    gfl2::math::Vector3 quadPos1 = parentPos + axisZ;
    gfl2::math::Vector3 quadPos2 = parentPos - axisY;
    gfl2::math::Vector3 quadPos3 = parentPos - axisZ;

    // 底辺の四角の部分
    DrawUtil::VertexFormat& vertexQuad0 = vertexList[vertexCount++];
    vertexQuad0.pos.Set(quadPos0.x, quadPos0.y, quadPos0.z);
    vertexQuad0.color = desc->jointColor;

    DrawUtil::VertexFormat& vertexQuad1 = vertexList[vertexCount++];
    vertexQuad1.pos.Set(quadPos1.x, quadPos1.y, quadPos1.z);
    vertexQuad1.color = desc->jointColor;

    vertexList[vertexCount++] = vertexQuad1;

    DrawUtil::VertexFormat& vertexQuad2 = vertexList[vertexCount++];
    vertexQuad2.pos.Set(quadPos2.x, quadPos2.y, quadPos2.z);
    vertexQuad2.color = desc->jointColor;

    vertexList[vertexCount++] = vertexQuad2;

    DrawUtil::VertexFormat& vertexQuad3 = vertexList[vertexCount++];
    vertexQuad3.pos.Set(quadPos3.x, quadPos3.y, quadPos3.z);
    vertexQuad3.color = desc->jointColor;

    vertexList[vertexCount++] = vertexQuad3;

    vertexList[vertexCount++] = vertexQuad0;

    // 子ノードへ繋ぐ
    DrawUtil::VertexFormat& vertexNode = vertexList[vertexCount++];
    vertexNode.pos.Set(nodePos.x, nodePos.y, nodePos.z);
    vertexNode.color = desc->jointColor;

    vertexList[vertexCount++] = vertexQuad0;

    vertexList[vertexCount++] = vertexNode;
    vertexList[vertexCount++] = vertexQuad1;

    vertexList[vertexCount++] = vertexNode;
    vertexList[vertexCount++] = vertexQuad2;

    vertexList[vertexCount++] = vertexNode;
    vertexList[vertexCount++] = vertexQuad3;

    // ここから軸

    // 子のXYZ軸
    gfl2::math::Vector3 childAxisX = nodeMatrix.GetElemAxisX();
    gfl2::math::Vector3 childAxisY = nodeMatrix.GetElemAxisY();
    gfl2::math::Vector3 childAxisZ = nodeMatrix.GetElemAxisZ();

    childAxisX *= desc->xyzScale;
    childAxisY *= desc->xyzScale;
    childAxisZ *= desc->xyzScale;

    // X軸
    DrawUtil::VertexFormat& vertexXBegin = vertexList[vertexCount++];
    DrawUtil::VertexFormat& vertexXEnd = vertexList[vertexCount++];
    vertexXBegin.pos.Set(nodePos.x, nodePos.y, nodePos.z);
    vertexXBegin.color.Set(1, 0, 0);
    math::Vector3 axisXPos = nodePos + childAxisX;
    vertexXEnd.pos.Set(axisXPos.x, axisXPos.y, axisXPos.z);
    vertexXEnd.color.Set(1, 0, 0);

    // Y軸
    DrawUtil::VertexFormat& vertexYBegin = vertexList[vertexCount++];
    DrawUtil::VertexFormat& vertexYEnd = vertexList[vertexCount++];
    vertexYBegin.pos.Set(nodePos.x, nodePos.y, nodePos.z);
    vertexYBegin.color.Set(0, 1, 0);
    math::Vector3 axisYPos = nodePos + childAxisY;
    vertexYEnd.pos.Set(axisYPos.x, axisYPos.y, axisYPos.z);
    vertexYEnd.color.Set(0, 1, 0);

    // Z軸
    DrawUtil::VertexFormat& vertexZBegin = vertexList[vertexCount++];
    DrawUtil::VertexFormat& vertexZEnd = vertexList[vertexCount++];
    vertexZBegin.pos.Set(nodePos.x, nodePos.y, nodePos.z);
    vertexZBegin.color.Set(0, 0, 1);
    math::Vector3 axisZPos = nodePos + childAxisZ;
    vertexZEnd.pos.Set(axisZPos.x, axisZPos.y, axisZPos.z);
    vertexZEnd.color.Set(0, 0, 1);
  }

  JointRenderPath::InitDescription* desc;                // 初期化設定
  DrawUtil::VertexFormat* vertexList;   // 頂点リスト
  u32 vertexLimit;                      // 頂点最大数

  u32 vertexCount;                      // 頂点数
};

void JointRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  // シザー、カリング、フィルの設定
  DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
  DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

  // マテリアル設定
  DrawUtil::SetMaterialAlpha(1.0f);
  
  // 簡易描画のデプステストを無しに設定する
  gfl2::gfx::DepthStencilStateObject::StateData stateData;
  stateData.m_DepthTestEnable = false;
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
  scenegraph::Traverser::Visitor<scenegraph::instance::JointInstanceNode, JointInstanceNodeVisitor>				visitor;
  visitor.desc = &m_InitDescription;
  visitor.vertexList = m_pVertexFormatList;
  visitor.vertexLimit = m_VertexFormatLimit;
  visitor.vertexCount = 0;
  scenegraph::SceneGraphManager::Traverse<scenegraph::instance::JointInstanceNode, JointInstanceNodeVisitor>( &visitor );

  // 頂点リスト描画
  DrawUtil::DrawLines(m_pVertexFormatList, visitor.vertexCount, false);

  // 描画終了
  DrawUtil::EndRender();
	GFGL::EndScene();
}

}}}}

#endif
