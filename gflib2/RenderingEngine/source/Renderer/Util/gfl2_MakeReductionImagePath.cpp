#include <RenderingEngine/include/Renderer/Util/gfl2_MakeReductionImagePath.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CombinerAttribute.h>
#include <gfx/include/gfl2_DrawUtil.h>
#include <math/include/gfl2_MathUtil.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

/**
 * @brief コンストラクタ
 */
MakeReductionImagePath::MakeReductionImagePath() :
  m_ScreenWidth(0),
  m_ScreenHeight(0),
  m_FrameBufferWidth(0),
  m_FrameBufferHeight(0),
  m_pSourceTexture(NULL),
  m_pDestinationTexture(NULL)
{
  memset(m_VertexDataArray, 0, sizeof(m_VertexDataArray));
}

/**
 * @brief 初期化
 * @param pAllocator テクスチャを作成するためのアロケータ
 * @param desc 初期化に必要な設定データ
 * @note リサイズ対象となるテクスチャを設定する
 */
void MakeReductionImagePath::Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc)
{
  GFL_ASSERT(!m_pDestinationTexture && "MakeReductionImagePath::Initialize() doesn't work correctly. This is already initialized.");
  m_ScreenWidth = desc.m_ScreenWidth;
  m_ScreenHeight = desc.m_ScreenHeight;
  m_FrameBufferWidth = m_ScreenWidth;
  m_FrameBufferHeight = m_ScreenHeight;
  f32 createBufferWidth = m_FrameBufferWidth;
  f32 createBufferHeight = m_FrameBufferHeight;

#if defined(GF_PLATFORM_CTR)
  // CTR版は2のべき乗の最大の大きさを検索してフレームバッファの大きさを指定
  for (u32 size = 8; size < 1024; size *= 2)
  {
    if (size >= m_FrameBufferWidth)
    {
      m_FrameBufferWidth = size;
      break;
    }
  }
  for (u32 size = 8; size < 1024; size *= 2)
  {
    if (size >= m_FrameBufferHeight)
    {
      m_FrameBufferHeight = size;
      break;
    }
  }

  // フレームバッファ作成時の横幅/縦幅を入れ替える
  createBufferWidth = m_FrameBufferHeight;
  createBufferHeight = m_FrameBufferWidth;
#endif

  // リサイズ対象のテクスチャを設定
  m_pSourceTexture = desc.m_pSourceTexture;

  GFL_ASSERT((m_ScreenWidth > 0) && (m_ScreenHeight > 0) && "MakeReductionImagePath::Initialize() doesn't work correctly. ScreenWidth and ScreenHeight must be larger than 0.");
  // 画面をキャプチャする用のテクスチャを生成
  m_pDestinationTexture = gfl2::gfx::GFGL::CreateTexture(
    pAllocator,
    createBufferWidth,
    createBufferHeight,
    1,
    gfl2::gfx::Usage::RenderTarget,
    gfl2::gfx::Format::A8R8G8B8,
    gfl2::gfx::Pool::Default
  );	
}

/**
 * @brief デストラクタ
 */
MakeReductionImagePath::~MakeReductionImagePath()
{
  // 縮小画像作成用のテクスチャを破棄
  GFL_DELETE(m_pDestinationTexture);

  // 縮小対象のテクスチャSourceTextureは外部から渡されるテクスチャなので解放しない
}

/**
 * @brief 縮小対象のテクスチャを設定
 * @param pSourceTexture 縮小対象のテクスチャ
 */
void MakeReductionImagePath::SetSourceTexture(const gfl2::gfx::Texture* pSourceTexture)
{
  m_pSourceTexture = pSourceTexture;
}

/**
 * @brief 縮小後のテクスチャを取得
 * @return const gfl2::gfx::Texture* 縮小後のテクスチャ
 */
const gfl2::gfx::Texture* MakeReductionImagePath::GetDestinationTexture() const
{
  return m_pDestinationTexture;
}

void MakeReductionImagePath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  GFL_ASSERT((m_pDestinationTexture != NULL) && "MakeReductionImagePath::Execute() doesn't work correctly. MakeReductionImagePath is not initialized.");

  if (!m_pSourceTexture)
  {// 縮小対象のソース画像が設定されていなければ何もしない
    return;
  }

  const gfx::Surface* pTargetSurface = m_pDestinationTexture->GetSurface();

  gfl2::gfx::GFGL::SetRenderTarget(m_pDestinationTexture->GetSurface());
	gfx::GFGL::SetDepthStencil(NULL);

  // DrawUtilに設定されている現在のテクスチャとスクリーンのサイズを記録(処理終了後に元に戻す)
  const gfl2::gfx::Texture* originalDrawUtilTexture = gfl2::gfx::DrawUtil::GetTexture();
  u32 originalDrawUtilScreenWidth = gfl2::gfx::DrawUtil::GetScreenWidth();
  u32 originalDrawUtilScreenHeight = gfl2::gfx::DrawUtil::GetScreenHeight();

  gfl2::gfx::GFGL::BeginScene();

  gfl2::gfx::DrawUtil::BeginRender();

  gfl2::gfx::GFGL::SetViewport(0, 0, m_FrameBufferWidth, m_FrameBufferHeight);
  gfl2::gfx::GFGL::SetScissor(0, 0, m_FrameBufferWidth, m_FrameBufferHeight);

  gfl2::gfx::DrawUtil::SetScreenSize(m_FrameBufferWidth, m_FrameBufferHeight);
  // 描画前に描画対象のレンダーターゲットを黒でクリア
  gfl2::gfx::GFGL::ClearRenderTarget(pTargetSurface, gfl2::gfx::Color(0.0f, 0.0f, 0.0f, 1.0f));

  gfl2::gfx::DepthStencilStateObject::StateData depthStateData;
  depthStateData.m_DepthTestEnable = false;
  depthStateData.m_DepthWriteEnable = false;
  gfl2::gfx::DrawUtil::SetDepthTestState(depthStateData);

  gfl2::gfx::DrawUtil::SetBlendTestState(
    false,
    gfl2::gfx::BlendOp::Add,
    gfl2::gfx::BlendFunc::One,
    gfl2::gfx::BlendFunc::One,
    gfl2::gfx::BlendFunc::Zero,
    gfl2::gfx::BlendFunc::Zero);
  gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
  gfl2::gfx::DrawUtil::SetTexture(m_pSourceTexture);
  gfl2::gfx::DrawUtil::DrawRect(0.0f, 0.0f, m_FrameBufferWidth, m_FrameBufferHeight);

  gfl2::gfx::DrawUtil::EndRender();

  gfl2::gfx::GFGL::EndScene();

  // 描画が終わったので本来のレンダーターゲットに戻す
  gfl2::gfx::GFGL::SetRenderTarget(rDrawContext.m_pRenderTarget);
  gfl2::gfx::GFGL::SetDepthStencil(rDrawContext.m_pDepthStencil);

  // 描画が終わったのでDrawUtilのテクスチャとスクリーンサイズを元に戻す
  gfl2::gfx::DrawUtil::SetTexture(originalDrawUtilTexture);
  gfl2::gfx::DrawUtil::SetScreenSize(originalDrawUtilScreenWidth, originalDrawUtilScreenHeight);
}

}}}}
