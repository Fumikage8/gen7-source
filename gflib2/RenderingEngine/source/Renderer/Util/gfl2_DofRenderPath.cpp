#include <RenderingEngine/include/Renderer/Util/gfl2_DofRenderPath.h>
#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <gfx/include/gfl2_DrawUtil.h>
#include <math/include/gfl2_MathUtil.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

//!< @brief 前面/背面を見分けるメッシュ名
const c8* DofRenderPath::s_FrontBackMeshName[DofType::NumberOf] = 
{
  "CombFront_OptMesh",    //!< @brief 前面のメッシュ名
  "CombBack_OptMesh"      //!< @brief 背面のメッシュ名
};

/**
 * @brief コンストラクタ
 */
DofRenderPath::DofRenderPath() :
  m_FocusLength(0.0f),
  m_DofType(DofType::Front),
  m_pSourceTexture(NULL),
  m_pDofRenderTargetModelInstanceNode(NULL),
  m_BlurScale(0.0f)
{

}

/**
 * @brief デストラクタ
 */
DofRenderPath::~DofRenderPath()
{
  // 外側から渡されたリソースなのでm_pModelInstanceNodeは解放しない
}

/**
 * @brief 描画パスの初期化
 * @param pAllocator 
 */
void DofRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  if (!m_pDofRenderTargetModelInstanceNode)
  {
    return;
  }

  if (!m_pSourceTexture)
  {
    return;
  }

  const gfx::Surface::SurfaceDesc &rDesc = rDrawContext.m_pRenderTarget->GetSurfaceDesc();
  gfx::GFGL::SetRenderTarget(rDrawContext.m_pRenderTarget);
	gfx::GFGL::SetDepthStencil(rDrawContext.m_pDepthStencil);
  
  GFGL::BeginScene();

  GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
  GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);

#if defined(GF_PLATFORM_CTR)
  GFGL::FlushAllDirtyCommand();
#endif
  
  u32 tagNum = m_pDofRenderTargetModelInstanceNode->GetDrawTagNum();
  RenderState renderState;
  renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo updateDrawTagInfo;

  static gfl2::math::Matrix34 s_viewMatrix = gfl2::math::Matrix34::GetIdentity();
  static gfl2::math::Matrix44 s_projMatrix = gfl2::math::Matrix44::GetIdentity();

  renderState.SetProjectionMatrix(&s_projMatrix);
  renderState.SetViewMatrix(&s_viewMatrix);

#if defined(GF_PLATFORM_CTR)

	nn::gr::CommandBufferJumpHelper		*pCommandBuffer = gfl2::gfx::GFGL::GetJumpHelper();
	bit32		*cmd = pCommandBuffer->GetCurrentPtr();

	{//proj
		nn::math::MTX44						*pMtx44;
    pMtx44 = (nn::math::MTX44*)&(s_projMatrix);
		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 86, *pMtx44 );
	}

	{//view
		nn::math::MTX34						*pMtx34;
    pMtx34 = (nn::math::MTX34*)&(s_viewMatrix);
		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 90, *pMtx34 );
	}

	pCommandBuffer->Put(cmd);
#endif

  gfl2::math::Vector4 tmpWorldPos(0.0f, 0.0f, -m_FocusLength, 1.0f);
  gfl2::math::Vector4 tmpScreenPos = m_ProjMatrix.TransformVec(tmpWorldPos);

  // テクスチャサイズとスクリーンサイズの比率を計算
#if defined(GF_PLATFORM_CTR)
  gfl2::math::Matrix34 modelMatrix = gfl2::math::Matrix34::GetRotationZ(gfl2::math::ConvDegToRad(-90));
#else
  gfl2::math::Matrix34 modelMatrix = gfl2::math::Matrix34::GetIdentity();
#endif
  modelMatrix[2][3] = tmpScreenPos.z;

  m_pDofRenderTargetModelInstanceNode->SetWorldMatrix(modelMatrix);
  m_pDofRenderTargetModelInstanceNode->UpdateDrawTag(updateDrawTagInfo);

  // テクスチャサイズとスクリーンサイズの比率を計算
#if defined(GF_PLATFORM_CTR)
  static f32 screenSizeU = 240.0f;
  static f32 screenSizeV = 400.0f;
  static f32 textureSizeU = 256.0f;
  static f32 textureSizeV = 512.0f;
#else
  static f32 screenSizeU = 512.0f;
  static f32 screenSizeV = 256.0f;
  static f32 textureSizeU = 512.0f;
  static f32 textureSizeV = 256.0f;
#endif

  f32 scaleU = screenSizeU / textureSizeU;
  f32 scaleV = screenSizeV / textureSizeV;

  // ブラー用のUVオフセット計算
  f32 offsetU = (scaleU / screenSizeU) * 2.0f;
  f32 offsetV = (scaleV / screenSizeV) * 2.0f;
  offsetU *= m_BlurScale;
  offsetV *= m_BlurScale;

  // ぼかし用に3枚のテクスチャを少しずつずらしてサンプリングする
  u32 materialNum = m_pDofRenderTargetModelInstanceNode->GetMaterialNum();
  for( u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    scenegraph::instance::MaterialInstanceNode* pMaterial = m_pDofRenderTargetModelInstanceNode->GetMaterialInstanceNode(materialNo);

    scenegraph::resource::MaterialResourceNode::TextureInfo::Attribute* pTexAttribute0 = &pMaterial->GetTextureInfo(0)->m_Attribute;
    scenegraph::resource::MaterialResourceNode::TextureInfo::Attribute* pTexAttribute1 = &pMaterial->GetTextureInfo(1)->m_Attribute;
    scenegraph::resource::MaterialResourceNode::TextureInfo::Attribute* pTexAttribute2 = &pMaterial->GetTextureInfo(2)->m_Attribute;

    // テクスチャサイズとスクリーンサイズの比率をUVスケールとして設定
    pTexAttribute0->m_ScaleU = scaleU;
    pTexAttribute0->m_ScaleV = scaleV;

    pTexAttribute1->m_ScaleU = scaleU;
    pTexAttribute1->m_ScaleV = scaleV;

    pTexAttribute2->m_ScaleU = scaleU;
    pTexAttribute2->m_ScaleV = scaleV;

    // 3テクスチャのサンプリング位置をずらす
    pTexAttribute0->m_TranslateU = 0.0f;
    pTexAttribute0->m_TranslateV = offsetV;
    
    pTexAttribute1->m_TranslateU = -offsetU;
    pTexAttribute1->m_TranslateV = -offsetV;

    pTexAttribute2->m_TranslateU = offsetU;
    pTexAttribute2->m_TranslateV = -offsetV;
  }

  for( u32 tagNo = 0; tagNo < tagNum; ++tagNo )
  {
    MeshDrawTag* pMeshDrawTag = m_pDofRenderTargetModelInstanceNode->GetMeshDrawTag( tagNo );

    // 非表示ならタグを積まない
    if (!pMeshDrawTag->IsVisible())
    {
      continue;
    }

    // ぼかすテクスチャを全スロットにセット
    pMeshDrawTag->SetTexture(0, m_pSourceTexture);
    pMeshDrawTag->SetTexture(1, m_pSourceTexture);
    pMeshDrawTag->SetTexture(2, m_pSourceTexture);

    pMeshDrawTag->Draw( renderState );
  }

  GFGL::EndScene();
}

/**
 * @brief レンダーターゲット(ぼかしの設定がなされているモデルデータ)の設定
 * @param pNode レンダーターゲットとなるモデルインスタンスノード
 */
void DofRenderPath::Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc)
{
  m_pDofRenderTargetModelInstanceNode = desc.m_pTargetModelInstanceNode;
  m_pSourceTexture = desc.m_pSourceTexture;

  // 描画面を前面に設定
  ChangeDofType(DofType::Front);

#if defined(GF_PLATFORM_WIN32)
  // 全てのマテリアルのUV座標を1.0fに修正
  u32 materialNum = m_pDofRenderTargetModelInstanceNode->GetMaterialNum();
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* materialResouceNode = m_pDofRenderTargetModelInstanceNode->GetMaterialInstanceNode(materialNo);
    u32 textureNum = materialResouceNode->GetTextureNum();
    for (u32 texNo = 0; texNo < textureNum; ++texNo)
    {
      gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo* textureInfo = materialResouceNode->GetTextureInfo(texNo);
      textureInfo->m_Attribute.m_ScaleU = 1.0f;
      textureInfo->m_Attribute.m_ScaleV = 1.0f;
    }
  }
#endif
}

/**
 * pbrief 焦点距離の設定
 * @param focusLength 焦点距離
 */
void DofRenderPath::SetFocusLength(const f32 focusLength)
{
  GFL_ASSERT((focusLength > 0.0f) && "DofRenderPath::SetFocusLength() doesn't work correctly. focusLength must be larger than 0.");
  m_FocusLength = focusLength;
}

/**
 * @brief 描画対象面の切り替え
 * @param dofType 前面/背面
 */
void DofRenderPath::ChangeDofType(DofType dofType)
{
  GFL_ASSERT((m_pDofRenderTargetModelInstanceNode != NULL) && "DofRenderPath::ChangeDofType() doesn't work correctly. m_pDofRenderTargetModelInstanceNode is not initialized.");
  m_DofType = dofType;

  u32 visibleMeshNum = 0;
  u32 meshNum = m_pDofRenderTargetModelInstanceNode->GetMehsNum();
  for (u32 meshNo = 0; meshNo < meshNum; ++meshNo)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLinkData = m_pDofRenderTargetModelInstanceNode->GetMeshLinkData(meshNo);
    const c8* meshLinkDataName = pMeshLinkData->GetName();
    bool visible = (strcmp(meshLinkDataName, s_FrontBackMeshName[m_DofType]) == 0);
    pMeshLinkData->SetVisible(visible);
    visibleMeshNum += visible ? 1: 0;
  }

  // Front/Backの2種類のメッシュだけなので, 1になっていなければメッシュの設定が間違っている
  GFL_ASSERT((visibleMeshNum == 1) && "DofRenderPath::ChangeDofType() doesn't work correctly. m_pDofRenderTargetModelInstanceNode has a wrong mesh.");
}

/**
 * @brief ブラーのぼかし率取得
 */
f32 DofRenderPath::GetBlurScale() const
{
  return m_BlurScale;
}

/**
 * @brief ブラーのぼかし率設定
 * @param blurScale ぼかし率
 */
void DofRenderPath::SetBlurScale(f32 blurScale)
{
  m_BlurScale = blurScale;
}

}}}}
