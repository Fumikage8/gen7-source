#if defined(GF_PLATFORM_WIN32)

#include <Renderingengine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_UtilEffectLocatorRenderPath.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ViewerLocatorInstanceNode.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;
using namespace gfl2::renderingengine::scenegraph::instance;

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

const gfl2::math::AABB EffectLocatorRenderPath::s_PokedecoBoundingBox[gfl2::renderingengine::PokedecoType::NumberOf] =
{
  gfl2::math::AABB(gfl2::math::Vector3(-10.0f, -14.0f, 0.0f), gfl2::math::Vector3(10.0f, 10.0f, 22.0f)),
  gfl2::math::AABB(gfl2::math::Vector3(-19.0f, -14.0f, 0.0f), gfl2::math::Vector3(19.0f, 14.0f, 10.0f)),
  gfl2::math::AABB(gfl2::math::Vector3(-19.0f, -8.0f, 0.0f), gfl2::math::Vector3(19.0f, 8.0f, 5.0f)),
  gfl2::math::AABB(gfl2::math::Vector3(-11.0f, -22.0f, 0.0f), gfl2::math::Vector3(11.0f, 11.0f, 10.0f)),
};

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
EffectLocatorRenderPath::EffectLocatorRenderPath() :
  IRenderPath(),
  m_pVertexFormatList(NULL)
{
}

EffectLocatorRenderPath::~EffectLocatorRenderPath()
{
  if (m_pVertexFormatList != NULL)
  {
    delete[] m_pVertexFormatList;
    m_pVertexFormatList = NULL;
  }
}

void EffectLocatorRenderPath::Initialize(gfl2::gfx::IGLAllocator * pGLAllocator, const InitDescription& desc)
{
  gfl2::gfx::GLMemory::SetInstanceAllocator( pGLAllocator ); 
  // 初期化設定保存
  m_InitDescription = desc;
  m_VertexFormatLimit = 6 * 256;    // XYZ軸 x 256個
  m_pVertexFormatList = new DrawUtil::VertexFormat[m_VertexFormatLimit];
  gfl2::gfx::GLMemory::ClearInstanceAllocator();
}

void EffectLocatorRenderPath::SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  // レンダーターゲットの設定
	GFGL::SetRenderTarget( pRenderTarget );
	GFGL::SetDepthStencil( pDepthStencil );
}

void EffectLocatorRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  // レンダーターゲットのクリアはしない
}

// ViewerLocatorInstanceNodeに対してのVisitor
class ViewerLocatorInstanceNodeVisitor
{
public:
  ViewerLocatorInstanceNodeVisitor() {}
  virtual ~ViewerLocatorInstanceNodeVisitor() {}

  void Visit( scenegraph::instance::ViewerLocatorInstanceNode* pNode )
  {
    const c8* name = pNode->GetName();

    if (std::strncmp(name, "Eff", 3) == 0)
    {// エフェクトのロケータを表示
      if (pNode->CheckState(ViewerLocatorInstanceNode::State::Visible)
        && pNode->CheckState(ViewerLocatorInstanceNode::State::Focused)
        )
      {
        gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
        // ノードの行列、親の行列を取得
        const math::Matrix34& nodeMatrix = pNode->GetWorldMatrix();
        gfl2::math::Vector4 nodePos(nodeMatrix.GetElem(0, 3), nodeMatrix.GetElem(1, 3), nodeMatrix.GetElem(2, 3));
        math::Vector4 diff(1.0f, 1.0f, 1.0f);
        math::Vector4 color(1.0f, 0.25f, 0.25f);
        DrawUtil::DrawAABB(nodePos - diff, nodePos + diff, color);
      }
    }
    else if (std::strncmp(name, "Pd", 2) == 0)
    {// ポケデコ用のロケータを表示
      if (pNode->CheckState(ViewerLocatorInstanceNode::State::Focused))
      {
        const math::Matrix34& nodeMatrix = pNode->GetWorldMatrix();
        if (pNode->CheckState(ViewerLocatorInstanceNode::State::Visible))
        {
          // ノードの行列、親の行列を取得
          gfl2::math::Vector3 nodePos(nodeMatrix.GetElem(0, 3), nodeMatrix.GetElem(1, 3), nodeMatrix.GetElem(2, 3));
          // 子のXYZ軸
          const f32 axisLength = 10.0f;
          gfl2::math::Vector3 childAxisX = gfl2::math::Vector3::GetXUnit() * axisLength;
          gfl2::math::Vector3 childAxisY = gfl2::math::Vector3::GetYUnit() * axisLength;
          gfl2::math::Vector3 childAxisZ = gfl2::math::Vector3::GetZUnit() * axisLength;

          gfl2::math::Vector3 axisXPos = nodeMatrix * childAxisX;
          gfl2::math::Vector3 axisYPos = nodeMatrix * childAxisY;
          gfl2::math::Vector3 axisZPos = nodeMatrix * childAxisZ;

          // X軸
          DrawUtil::VertexFormat& vertexXBegin = vertexList[vertexCount++];
          DrawUtil::VertexFormat& vertexXEnd = vertexList[vertexCount++];
          vertexXBegin.pos.Set(nodePos.x, nodePos.y, nodePos.z);
          vertexXBegin.color.Set(1, 0, 0);
          vertexXEnd.pos.Set(axisXPos.x, axisXPos.y, axisXPos.z);
          vertexXEnd.color.Set(1, 0, 0);

          // Y軸
          DrawUtil::VertexFormat& vertexYBegin = vertexList[vertexCount++];
          DrawUtil::VertexFormat& vertexYEnd = vertexList[vertexCount++];
          vertexYBegin.pos.Set(nodePos.x, nodePos.y, nodePos.z);
          vertexYBegin.color.Set(0, 1, 0);
          vertexYEnd.pos.Set(axisYPos.x, axisYPos.y, axisYPos.z);
          vertexYEnd.color.Set(0, 1, 0);

          // Z軸
          DrawUtil::VertexFormat& vertexZBegin = vertexList[vertexCount++];
          DrawUtil::VertexFormat& vertexZEnd = vertexList[vertexCount++];
          vertexZBegin.pos.Set(nodePos.x, nodePos.y, nodePos.z);
          vertexZBegin.color.Set(0, 0, 1);
          vertexZEnd.pos.Set(axisZPos.x, axisZPos.y, axisZPos.z);
          vertexZEnd.color.Set(0, 0, 1);
        }

        for (s32 i = 0; i < gfl2::renderingengine::PokedecoType::NumberOf; ++i)
        {
          if (!pNode->IsPokedecoAABBVisible(static_cast<gfl2::renderingengine::PokedecoType>(i)))
          {
            continue;
          }

          // マテリアル設定
          gfl2::gfx::DrawUtil::SetMaterialAlpha(0.1f);
          // ワールドマトリックスを四角形に設定
          gfl2::math::Matrix44 nodeMatrix44;
          nodeMatrix44[0] = nodeMatrix[0];
          nodeMatrix44[1] = nodeMatrix[1];
          nodeMatrix44[2] = nodeMatrix[2];
          nodeMatrix44[3] = gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
          DrawUtil::SetModelMatrix(nodeMatrix44);
          DrawUtil::DrawAABB(
            gfl2::renderingengine::renderer::util::EffectLocatorRenderPath::s_PokedecoBoundingBox[i],
            gfl2::math::Vector4(0.25f, 1.0f, 1.0f, 0.1f));
          DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());
        }
      }
    }
  }

  EffectLocatorRenderPath::InitDescription* desc;   //!< @brief 初期化設定
  DrawUtil::VertexFormat* vertexList;               //!< @brief 頂点リスト
  u32 vertexLimit;                                  //!< @brief 頂点最大数
  u32 vertexCount;                                  //!< @brief 頂点数
};

void EffectLocatorRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  // カリング、フィルの設定
  DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
  DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

  // マテリアル設定
  DrawUtil::SetMaterialAlpha(1.0f);
  
  // 簡易描画のデプステストを無しに設定する
  gfl2::gfx::DepthStencilStateObject::StateData stateData;
  stateData.m_DepthTestEnable = false;
  DrawUtil::SetDepthTestState(stateData);

  gfl2::gfx::GFGL::SetRenderTarget(rDrawContext.m_pRenderTarget);
  gfl2::gfx::GFGL::SetDepthStencil(rDrawContext.m_pDepthStencil);

  // 描画開始
	GFGL::BeginScene();
  DrawUtil::ClearScissor();

  // 簡易描画開始
  DrawUtil::BeginRender();

  // ノードを巡回して頂点リストを作成する
  scenegraph::Traverser::Visitor<scenegraph::instance::ViewerLocatorInstanceNode, ViewerLocatorInstanceNodeVisitor>	visitor;
  visitor.desc = &m_InitDescription;
  visitor.vertexList = m_pVertexFormatList;
  visitor.vertexLimit = m_VertexFormatLimit;
  visitor.vertexCount = 0;
  scenegraph::SceneGraphManager::Traverse<scenegraph::instance::ViewerLocatorInstanceNode, ViewerLocatorInstanceNodeVisitor>( &visitor );

  if (visitor.vertexCount > 0)
  {
    // プロジェクション行列設定
    DrawUtil::SetProjectionMatrix(rDrawContext.m_Projection);
    // ビュー行列設定
    DrawUtil::SetViewMatrix(rDrawContext.m_View);
    // モデル行列設定
    DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());
  
    // 頂点リスト描画
    gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
    DrawUtil::DrawLines(m_pVertexFormatList, visitor.vertexCount, false);
  }

  // 描画終了
  DrawUtil::EndRender();
	GFGL::EndScene();
}

}}}}

#endif
