#include <RenderingEngine/include/Renderer/Util/gfl2_ColorShaderTextureRenderPath.h>
#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

/**
 * @brief コンストラクタ
 */
ColorShaderTextureRenderPath::ColorShaderTextureRenderPath() :
  m_ExecuteFrame(0),  
  m_Color(1.0f, 1.0f, 1.0f, 1.0f),
  m_pOriginalColorTexture(NULL),
  m_pColorShaderTexture(NULL),  
  m_pRenderTargetModelInstanceNode(NULL)
{

}

/**
 * @brief デストラクタ
 */
ColorShaderTextureRenderPath::~ColorShaderTextureRenderPath()
{
  // 外側から渡されたリソースなのでm_pModelInstanceNodeは解放しない
}

void ColorShaderTextureRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  if (m_ExecuteFrame <= 0)
  {// 実行フレーム数が0なら実行しない
    return;
  }

  if (!m_pRenderTargetModelInstanceNode)
  {
    return;
  }

  if (!m_pColorShaderTexture)
  {
    return;
  }

  if (!m_pOriginalColorTexture)
  {
    return;
  }

  const gfx::Surface* pColorShaderSurface = m_pColorShaderTexture->GetSurface();
  const gfx::Surface::SurfaceDesc			&rDesc = pColorShaderSurface->GetSurfaceDesc();
  gfx::GFGL::SetRenderTarget(pColorShaderSurface);
#if defined(GF_PLATFORM_CTR)
	gfx::GFGL::SetDepthStencil(rDrawContext.m_pDepthStencil);
#else
	gfx::GFGL::SetDepthStencil(NULL);
#endif
  GFGL::BeginScene();

#if defined(GF_PLATFORM_CTR)
  GFGL::SetViewport(0, 0, rDesc.height, rDesc.width);
  GFGL::SetScissor(0, 0, rDesc.height, rDesc.width);
#else
  GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
  GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
#endif
  gfx::GFGL::ClearRenderTarget(pColorShaderSurface, Color(1.0f, 0.0f, 0.0f, 1.0f));

  renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* pParam =
    m_pRenderTargetModelInstanceNode->GetMaterialInstanceNode(0)->GetAttributeParam();

  // ConstantColor[3].xyzがRGB, ConstantColor[4].wが係数
  pParam->m_ConstantColor[3].r = static_cast<u8>( m_Color.x * 255.0f );
  pParam->m_ConstantColor[3].g = static_cast<u8>( m_Color.y * 255.0f );
  pParam->m_ConstantColor[3].b = static_cast<u8>( m_Color.z * 255.0f );
  pParam->m_ConstantColor[4].a = static_cast<u8>( m_Color.w * 255.0f );

  u32							tagNum = m_pRenderTargetModelInstanceNode->GetDrawTagNum();
  RenderState			renderState;
  math::Matrix44		dummyMat44 = math::Matrix44::GetIdentity();
  math::Matrix34		dummyMat34 = math::Matrix34::GetIdentity();

  renderState.SetProjectionMatrix( &dummyMat44 );
  renderState.SetViewMatrix( &dummyMat34 );
  renderState.SetWorldMatrix( &dummyMat34 );

  renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo updateDrawTagInfo;
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
    pMeshDrawTag->SetTexture(0, m_pOriginalColorTexture);
    pMeshDrawTag->Draw( renderState );
  }

  GFGL::EndScene();

  // 実行フレームをデクリメント
  --m_ExecuteFrame;
}

/**
 * @brief カラーシェーダの色の設定
 * @param color カラーシェーダで適用する色
 * @param executeFrame 実行するフレーム
 * @note executeFrameは基本1. 処理落ち発生の際に, 確実に実行してほしいフレーム数を指定する
 */
void ColorShaderTextureRenderPath::SetColorShaderTextureColor(const math::Vector4& color, s32 executeFrame)
{
  m_Color = color;
  m_ExecuteFrame = executeFrame;  // 色の変更があったのでパス実行フラグを立てる
}

}}}}
