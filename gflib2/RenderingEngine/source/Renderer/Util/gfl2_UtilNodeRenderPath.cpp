#include <Renderingengine/include/Scenegraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_UtilNodeRenderPath.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
NodeRenderPath::NodeRenderPath() :
  IRenderPath(),
  m_pSelectedNode(NULL),
  m_DepthStencilStateObject(NULL)
{
}

NodeRenderPath::~NodeRenderPath()
{
}

void NodeRenderPath::Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc)
{
  // 初期化設定保存
  m_InitDescription = desc;

  // デプステスト無しのステートを作成
	m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject(pAllocator);
	m_DepthStencilStateObject->SetDepthTestEnable(false);
	m_DepthStencilStateObject->UpdateState();
}

void NodeRenderPath::SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  // レンダーターゲットの設定
	GFGL::SetRenderTarget( pRenderTarget );
	GFGL::SetDepthStencil( pDepthStencil );
}

void NodeRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  (void)pRenderTarget;
  (void)pDepthStencil;

  // レンダーターゲットのクリアはしない
}

void NodeRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  // シザー、カリング、フィルの設定
  DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
  DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

  // マテリアル設定
  DrawUtil::SetMaterialAlpha(1.0f);

  // 描画開始
	GFGL::BeginScene();
  DrawUtil::ClearScissor();

  // デプステスト無しに設定する
  GFGL::SetDepthStencilState(m_DepthStencilStateObject);

  // 簡易描画開始
  DrawUtil::BeginRender();

  // プロジェクション行列設定
  DrawUtil::SetProjectionMatrix(rDrawContext.m_Projection);
  // ビュー行列設定
  DrawUtil::SetViewMatrix(rDrawContext.m_View);
  // モデル行列設定
  DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());

  // ゲーム画面にノード位置描画
  //! @note カメラノード、ライトノードなどデバッグに必要そうなノードにも対応する

  // 現在選択中のノード
  this->DrawSelectedNode();

  // 描画終了
  DrawUtil::EndRender();
	GFGL::EndScene();
}

/**
 * @brief 選択中ノードを設定する
 * @param pNode ノード
 */
void NodeRenderPath::SetSelectedNode(gfl2::renderingengine::scenegraph::DagNode* pNode)
{
  m_pSelectedNode = pNode;
}

/**
 * @brief 選択中ノードを描画する
 */
void NodeRenderPath::DrawSelectedNode()
{
  // 何も選択されていなければ無視
  if (m_pSelectedNode == NULL)
  {
    return;
  }

  // トランスフォームノードにキャストできなければ無視
  gfl2::renderingengine::scenegraph::instance::TransformNode* pTransformNode = m_pSelectedNode->SafeCast<gfl2::renderingengine::scenegraph::instance::TransformNode>();
  if (pTransformNode == NULL)
  {
    return;
  }

  const math::Matrix34& worldMatrix = pTransformNode->GetWorldMatrix();
  math::Vector3 pos = worldMatrix.GetElemPosition();

  DrawUtil::VertexFormat vertexList[4];
  for (s32 i = 0; i < sizeof(vertexList) / sizeof(vertexList[0]); ++i)
  {
    vertexList[i].pos.Set(pos.x, pos.y, pos.z);
    vertexList[i].color.Set(1.0f, 0.5f, 0.5f);
  }
  static const f32 rectSize = 1.0f;
  vertexList[0].pos.y += rectSize;
  vertexList[1].pos.x += rectSize;
  vertexList[2].pos.x -= rectSize;
  vertexList[3].pos.y -= rectSize;

  DrawUtil::VertexFormat vertexList_[6];
  vertexList_[0] = vertexList[0];
  vertexList_[1] = vertexList[1];
  vertexList_[2] = vertexList[2];
  vertexList_[3] = vertexList[2];
  vertexList_[4] = vertexList[1];
  vertexList_[5] = vertexList[3];

  DrawUtil::DrawTriangles(vertexList, sizeof(vertexList) / sizeof(vertexList[0]), false);
}

}}}}

