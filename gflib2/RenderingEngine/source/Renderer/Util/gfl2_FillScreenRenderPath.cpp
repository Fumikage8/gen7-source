#include <RenderingEngine/include/Renderer/Util/gfl2_FillScreenRenderPath.h>
#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CombinerAttribute.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

/**
 * @brief コンストラクタ
 */
FillScreenRenderPath::FillScreenRenderPath() :
  m_pFillScreenTexture(NULL),  
  m_pRenderTargetModelInstanceNode(NULL)
{

}

/**
 * @brief デストラクタ
 */
FillScreenRenderPath::~FillScreenRenderPath()
{
  // 外側から渡されたリソースなのでm_pModelInstanceNodeは解放しない
}

void FillScreenRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  if (!m_pRenderTargetModelInstanceNode)
  {
    return;
  }

  if (!m_pFillScreenTexture)
  {
    return;
  }

  GFGL::BeginScene();

  const gfl2::gfx::Surface::SurfaceDesc			&rDesc = gfl2::gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
  gfl2::gfx::GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
  gfl2::gfx::GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);

#if defined(GF_PLATFORM_CTR)
  GFGL::SetViewport(0, 0, rDesc.height, rDesc.width);
  GFGL::SetScissor(0, 0, rDesc.height, rDesc.width);
#else
  GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
  GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
#endif
  gfx::GFGL::ClearRenderTarget(rDrawContext.m_pRenderTarget, Color(1.0f, 0.0f, 0.0f, 1.0f));

  u32							tagNum = m_pRenderTargetModelInstanceNode->GetDrawTagNum();
  RenderState			renderState;
  renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo updateDrawTagInfo;
  math::Matrix44		dummyMat44 = math::Matrix44::GetIdentity();
  math::Matrix34		dummyMat34 = math::Matrix34::GetIdentity();

  renderState.SetProjectionMatrix( &dummyMat44 );
  renderState.SetViewMatrix( &dummyMat34 );
  renderState.SetWorldMatrix( &dummyMat34 );

  m_pRenderTargetModelInstanceNode->UpdateDrawTag(updateDrawTagInfo);

  for( u32 tagNo = 0; tagNo < tagNum; ++tagNo )
  {
    MeshDrawTag			*pMeshDrawTag = m_pRenderTargetModelInstanceNode->GetMeshDrawTag( tagNo );

    // 非表示ならタグを積まない
    if (!pMeshDrawTag->IsVisible())
    {
      continue;
    }

    // 色を変更するテクスチャに差し替える
    pMeshDrawTag->SetTexture(0, m_pFillScreenTexture);
    pMeshDrawTag->Draw( renderState );
  }

  GFGL::EndScene();
}

void FillScreenRenderPath::SetConstantColorToRenderTargetPlane(u32 constantNo, const gfl2::math::Vector4& color)
{
  GFL_ASSERT(m_pRenderTargetModelInstanceNode != NULL);
  GFL_ASSERT(constantNo < scenegraph::resource::CombinerAttribute::ConstantMax);

  u32 materialNum = m_pRenderTargetModelInstanceNode->GetMaterialNum();
  for (u32 i = 0; i < materialNum; ++i)
  {
    scenegraph::resource::MaterialResourceNode::AttributeParam* pParam =
      m_pRenderTargetModelInstanceNode->GetMaterialInstanceNode(i)->GetAttributeParam();
    pParam->m_ConstantColor[constantNo] = color;
  }
}

}}}}
