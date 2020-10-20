#include <RenderingEngine/include/Renderer/Util/gfl2_MakeBlurImagePath.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CombinerAttribute.h>
#include <gfx/include/gfl2_DrawUtil.h>
#include <math/include/gfl2_MathUtil.h>
#if defined(GF_PLATFORM_CTR)
#include <gfx/include/CTR/gfl2_CTRSurface.h>
#endif

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

//!< @brief 被写界深度描画時の深度の比較設定
const gfl2::gfx::CompareFunc MakeBlurImagePath::s_DofCompareFunc[DofType::NumberOf] = 
{
  gfl2::gfx::CompareFunc::Greater,
  gfl2::gfx::CompareFunc::Less
};

/**
 * @brief コンストラクタ
 */
MakeBlurImagePath::MakeBlurImagePath() :
  m_ScreenWidth(0),
  m_ScreenHeight(0),
  m_FrameBufferWidth(0),
  m_FrameBufferHeight(0),
  m_DofType(DofType::Front),
  m_AvailableLayerNum(0),
  m_pDepthWrittenTexture(NULL),
  m_pCopyColorBufferTexture(NULL),
  m_pScreenCapturedTexture(NULL),
  m_FocusLength(0.0f)
{
  memset(m_VertexDataArray, 0, sizeof(m_VertexDataArray));

  for (u32 i = 0; i < LAYER_MAX; ++i)
  {
    m_BlurLayerInfoArray[i].m_RangeDistance = 0.0f;
    m_BlurLayerInfoArray[i].m_BlurredCoef = 0.0f;
  }
}

/**
 * @brief 初期化
 * @param pAllocator テクスチャを作成するためのアロケータ
 * @param desc 初期化に必要な設定データ
 * @note 描画対象となるSurfaceを設定する
 */
void MakeBlurImagePath::Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc)
{
  GFL_ASSERT(!m_pDepthWrittenTexture && "MakeBlurImagePath::Initialize() doesn't work correctly. This is already initialized.");
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

  m_pCopyColorBufferTexture = gfl2::gfx::GFGL::CreateTexture(
    pAllocator,
    createBufferWidth,
    createBufferHeight,
    1,
    gfl2::gfx::Usage::RenderTarget,
    gfl2::gfx::Format::A8R8G8B8,
    gfl2::gfx::Pool::Default);

  // 描画パスの実行結果を書き込むサーフェスとテクスチャを生成
  m_pDepthWrittenTexture = gfl2::gfx::GFGL::CreateTexture(
    pAllocator,
    createBufferWidth,
    createBufferHeight,
    1,
    gfl2::gfx::Usage::RenderTarget,
    gfl2::gfx::Format::A8R8G8B8,
    gfl2::gfx::Pool::Default);
}

/**
 * @brief デストラクタ
 */
MakeBlurImagePath::~MakeBlurImagePath()
{
  // 各種テクスチャを破棄
  GFL_DELETE(m_pDepthWrittenTexture);
  GFL_DELETE(m_pCopyColorBufferTexture);
}

void MakeBlurImagePath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  if (!m_pDepthWrittenTexture)
  {
    return;
  }

  // カラーバッファの内容をテクスチャにコピー
  gfl2::gfx::GFGL::SetRenderTarget(rDrawContext.m_pRenderTarget);
#if defined(GF_PLATFORM_CTR)
  const gfx::ctr::CTRTexture* pCtrCopyColorBufferTexture = reinterpret_cast<const gfx::ctr::CTRTexture*>(m_pCopyColorBufferTexture);
  uptr destinationAddress = pCtrCopyColorBufferTexture->GetVirtualAddr();
  gfl2::gfx::ctr::CTRGL::AddTransferColorBufferCommand(reinterpret_cast<void*>(destinationAddress));
#else
  gfl2::gfx::GFGL::BlitSurfaceToTexture(rDrawContext.m_pRenderTarget, m_pCopyColorBufferTexture);
#endif
  // DrawUtilに設定されている現在のテクスチャとスクリーンのサイズを記録(処理終了後に元に戻す)
  const gfl2::gfx::Texture* originalDrawUtilTexture = gfl2::gfx::DrawUtil::GetTexture();
  u32 originalDrawUtilScreenWidth = gfl2::gfx::DrawUtil::GetScreenWidth();
  u32 originalDrawUtilScreenHeight = gfl2::gfx::DrawUtil::GetScreenHeight();

  const gfx::Surface* pTargetSurface = m_pDepthWrittenTexture->GetSurface();

  gfl2::gfx::GFGL::SetRenderTarget(pTargetSurface);
  gfl2::gfx::GFGL::SetDepthStencil(rDrawContext.m_pDepthStencil);

  gfl2::gfx::GFGL::BeginScene();
  gfl2::gfx::DrawUtil::BeginRender();

  gfl2::gfx::GFGL::SetViewport(0, 0, m_FrameBufferWidth, m_FrameBufferHeight);
  gfl2::gfx::GFGL::SetScissor(0, 0, m_FrameBufferWidth, m_FrameBufferHeight);
  gfl2::gfx::DrawUtil::SetScreenSize(m_FrameBufferWidth, m_FrameBufferHeight);

  // 描画前に描画対象のレンダーターゲットを黒でクリア
  gfl2::gfx::GFGL::ClearRenderTarget(pTargetSurface, gfl2::gfx::Color(0.0f, 0.0f, 0.0f, 0.0f));

  gfl2::gfx::DrawUtil::SetProjectionMatrix(gfl2::math::Matrix44::GetIdentity());
  gfl2::gfx::DrawUtil::SetViewMatrix(gfl2::math::Matrix34::GetIdentity());
  gfl2::gfx::DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());

  // 描画する頂点の座標を設定
  static const f32 scale = 1.0f;
  static const gfl2::math::Vector2 uvArray[VERTEX_DATA_ARRAY_LENGTH] =
  {
    gfl2::math::Vector2(-scale, +scale),  // [0]
    gfl2::math::Vector2(+scale, +scale),  // [1]
    gfl2::math::Vector2(-scale, -scale),  // [2]
    gfl2::math::Vector2(-scale, -scale),  // [2]
    gfl2::math::Vector2(+scale, +scale),  // [1]
    gfl2::math::Vector2(+scale, -scale),  // [3]
  };

  // 深度テストに合格したところのみαの値が入るようにする.
  gfl2::gfx::DepthStencilStateObject::StateData depthStateData;
  for (u32 layerNo = 0; layerNo < m_AvailableLayerNum; ++layerNo)
  {
    gfl2::math::Vector4 tmpWorldPos(0.0f, 0.0f, -(m_FocusLength + m_BlurLayerInfoArray[layerNo].m_RangeDistance), 1.0f);
    gfl2::math::Vector4 tmpScreenPos = m_ProjMatrix.TransformVec(tmpWorldPos);
    for (u32 vertexIndex = 0; vertexIndex < VERTEX_DATA_ARRAY_LENGTH; ++vertexIndex)
    {
      m_VertexDataArray[vertexIndex].pos = gfl2::math::Vector4(uvArray[vertexIndex].x, uvArray[vertexIndex].y, tmpScreenPos.z, 1.0f);
      m_VertexDataArray[vertexIndex].color.x = 1.0f;
      m_VertexDataArray[vertexIndex].color.y = 1.0f;
      m_VertexDataArray[vertexIndex].color.z = 1.0f;
      m_VertexDataArray[vertexIndex].color.w = 1.0f;

      gfl2::math::Vector4 screenPosition = m_VertexDataArray[vertexIndex].pos;
    }

    depthStateData.m_DepthFunc = s_DofCompareFunc[m_DofType];
    depthStateData.m_DepthWriteEnable = false;
    gfl2::gfx::DrawUtil::SetDepthTestState(depthStateData);
    gfl2::gfx::DrawUtil::SetBlendTestState(
      false,
      gfl2::gfx::BlendOp::Add,
      gfl2::gfx::BlendFunc::One,
      gfl2::gfx::BlendFunc::Zero,
      gfl2::gfx::BlendFunc::One,
      gfl2::gfx::BlendFunc::Zero);

    // アルファの設定
    gfl2::gfx::DrawUtil::SetMaterialAlpha(m_BlurLayerInfoArray[layerNo].m_BlurredCoef);

    // テクスチャを外した状態でデプステストを反映させてプレーンを描画
    gfl2::gfx::DrawUtil::DrawTriangles(m_VertexDataArray, 6, false);
  }

  gfl2::gfx::DrawUtil::EndRender();
  gfl2::gfx::GFGL::EndScene();

  gfl2::gfx::GFGL::BeginScene();
  gfl2::gfx::DrawUtil::BeginRender();

  // カラーバッファのアルファ値には書き込まず, 現在の色情報のみを描画
  depthStateData.m_DepthFunc = gfl2::gfx::CompareFunc::Always;
  depthStateData.m_DepthTestEnable = false;
  depthStateData.m_DepthWriteEnable = false;
  depthStateData.m_StencilTestEnable = false;
  gfl2::gfx::DrawUtil::SetDepthTestState(depthStateData);
  gfl2::gfx::DrawUtil::SetBlendTestState(
    true,
    gfl2::gfx::BlendOp::Add,
    gfl2::gfx::BlendFunc::One,
    gfl2::gfx::BlendFunc::Zero,
    gfl2::gfx::BlendFunc::Zero,
    gfl2::gfx::BlendFunc::One,
    gfl2::gfx::BlendStateObject::COLOR_WRITE_ENABLE_ALL);
  
  gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
  // テクスチャを外した状態でデプステストを反映させてプレーンを描画
  gfl2::gfx::DrawUtil::SetTexture(m_pCopyColorBufferTexture);
  gfl2::gfx::DrawUtil::DrawRect(0, 0, m_FrameBufferWidth, m_FrameBufferHeight);

  gfl2::gfx::DrawUtil::EndRender();
  gfl2::gfx::GFGL::EndScene();

  // 描画が終わったので本来のレンダーターゲットに戻す
  gfl2::gfx::GFGL::SetRenderTarget(rDrawContext.m_pRenderTarget);
  gfl2::gfx::GFGL::SetDepthStencil(rDrawContext.m_pDepthStencil);

  // 描画が終わったのでDrawUtilのテクスチャとスクリーンサイズを元に戻す
  gfl2::gfx::DrawUtil::SetTexture(originalDrawUtilTexture);
  gfl2::gfx::DrawUtil::SetScreenSize(originalDrawUtilScreenWidth, originalDrawUtilScreenHeight);
}

/**
 * @brief 被写界深度の種類の設定(Front/Backの設定)
 * @param dofType 前面/背面の設定
 */
void MakeBlurImagePath::ChangeDofType(DofType dofType)
{
  GFL_ASSERT((dofType < DofType::NumberOf) && "MakeBlurImagePath::SetDofType() doesn't work correctly. dofType must be Front or Back.");
  m_DofType = dofType;
}

/**
 * @brief 使用するレイヤーの枚数の設定
 * @param layerNum 使用するレイヤーの枚数
 */
void MakeBlurImagePath::SetAvailableLayerNum(const u32 layerNum)
{
  GFL_ASSERT((layerNum <= LAYER_MAX) && "MakeBlurImagePath::SetAvailableLayerNum() doesn't work correctly. m_AvailableLayerNum must be less equal than LAYER_MAX.");
  m_AvailableLayerNum = layerNum;
}

/**
 * @brief ぼやけ具合の設定
 * @param layerNo 描画するレイヤー番号
 * @param rangeDistance 影響を受ける範囲の距離
 * @param blurredCoef ぼやけ具合(0.0f～1.0f)
 */
void MakeBlurImagePath::SetFocusRange(u32 layerNo, f32 rangeDistance, f32 blurredCoef)
{
  GFL_ASSERT((layerNo < m_AvailableLayerNum) && "MakeBlurImagePath::SetFocusRange() doesn't work correctly. layerNo must be less than m_AvailableLayerNum.");
  m_BlurLayerInfoArray[layerNo].m_RangeDistance = rangeDistance;
  m_BlurLayerInfoArray[layerNo].m_BlurredCoef = blurredCoef;
}

/**
 * @brief 深度情報をアルファに書き込んだテクスチャ取得
 * @return const gfl2::gfx::Texture* 深度情報を書き込んだテクスチャ
 */
const gfl2::gfx::Texture* MakeBlurImagePath::GetDepthWrittenTexture() const
{
  return m_pDepthWrittenTexture;
}

}}}}
