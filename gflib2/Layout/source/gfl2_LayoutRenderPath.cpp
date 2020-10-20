//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl2_LayoutRenderPath.cpp
 *  @brief  レイアウト描画パス
 *  @author kawada_koji@gamefreak.co.jp
 *  @date   2015.01.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <layout/include/gfl2_LytSys.h>
#include <layout/include/gfl2_LytWkBase.h>
#include <layout/include/gfl2_LayoutRenderPath.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GLMemory.h>



GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)


//-----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
LayoutRenderPath::LayoutRenderPath( gfl2::gfx::IGLAllocator* pGLAllocator )
: IRenderPath()
, m_pRasterizerStateObject(NULL)
, m_pBlendStateObject(NULL)
, m_pRenderTarget(NULL)
, m_pDepthStencil(NULL)
//, m_pLytWkList( NULL )
//, m_eDispType( NULL )
//, m_eCameraType( NULL )
, m_lytWkNum(0)
, m_lytWkMaxNum(0)
{
  if(pGLAllocator)
  {
    m_pRasterizerStateObject = gfx::GFGL::CreateRasterizerStateObject(pGLAllocator);
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
  m_pRasterizerStateObject->SetCullMode( gfx::CullMode::None );
  m_pRasterizerStateObject->SetFillMode( gfx::FillMode::Fill );
  m_pRasterizerStateObject->UpdateState();

  if(pGLAllocator)
  {
    m_pBlendStateObject = gfx::GFGL::CreateBlendStateObject(pGLAllocator);
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
  m_pBlendStateObject->SetBlendEnable(true);
  m_pBlendStateObject->SetBlendOp(gfx::BlendOp::Add);
  m_pBlendStateObject->SetBlendFunc(gfx::BlendFunc::SrcAlpha, gfx::BlendFunc::InverseSrcAlpha);
  m_pBlendStateObject->UpdateState();

  for( int i=0; i<LYT_WK_NUM_MAX; ++i )
  {
    m_pLytWkList[i] = NULL;
    m_eDispType[i] = lyt::DISPLAY_NONE;
    m_eCameraType[i] = Env::CAMERA_NORMAL;
    m_eOrderList[i] = ORDER_NORMAL;
  }

  m_lytWkMaxNum = LYT_WK_NUM_MAX;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
 */
//-----------------------------------------------------------------------------
LayoutRenderPath::~LayoutRenderPath(void)
{
  //GFL_SAFE_DELETE_ARRAY(m_eCameraType);
  //GFL_SAFE_DELETE_ARRAY(m_eDispType);
  //GFL_SAFE_DELETE_ARRAY(m_pLytWkList);

  GFL_SAFE_DELETE( m_pRasterizerStateObject );
  GFL_SAFE_DELETE( m_pBlendStateObject );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  初期化
 */
//-----------------------------------------------------------------------------
void LayoutRenderPath::Init( gfl2::heap::HeapBase* heap, u32 lytWkMax )
{
  //m_pLytWkList  = GFL_NEW_ARRAY(heap) lyt::LytWk*[lytWkMax];
  //m_eDispType   = GFL_NEW_ARRAY(heap) lyt::DisplayType[lytWkMax];
  //m_eCameraType = GFL_NEW_ARRAY(heap) Env::CameraType[lytWkMax];

  //m_lytWkMaxNum = lytWkMax;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  レンダーターゲットの設定
 */
//-----------------------------------------------------------------------------
void LayoutRenderPath::SetRenderTarget(const gfx::Surface* pRenderTarget, const gfx::Surface* pDepthStencil)
{
  m_pRenderTarget = pRenderTarget;
  m_pDepthStencil = pDepthStencil;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  描画したレイアウトワークのクリア
 */
//-----------------------------------------------------------------------------
void LayoutRenderPath::ClearDrawableLytWk(void)
{
  for(u32 i=0; i<m_lytWkNum; ++i)
  {
    m_pLytWkList[i] = NULL;
    m_eDispType[i] = lyt::DISPLAY_NONE;
    m_eCameraType[i] = Env::CAMERA_NORMAL;
    m_eOrderList[i] = ORDER_NORMAL;
  }

  m_lytWkNum = 0;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  描画するレイアウトワークの登録
 */
//-----------------------------------------------------------------------------
void LayoutRenderPath::AddDrawableLytWk( lyt::LytWk* pLytWk, gfl2::lyt::DisplayType lod, u8 order, Env::CameraType cameraType )
{
  if(m_lytWkNum < m_lytWkMaxNum-1)
  {
    u32 index = GetAddIndex( order );

    m_pLytWkList[index]  = pLytWk;
    m_eDispType[index]   = lod;  // nijiでは左目、右目の描画呼び出しが分かれており別々のlodで呼び出されるが、gflibレイアウトはどちらにも上画面の設定を用いる。その違いの吸収はLytWkBaseで行っている。
    m_eCameraType[index] = cameraType;
    m_eOrderList[index]  = order;

    ++m_lytWkNum;
  }
  else
  {
    // 登録数オーバー
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  実行
 */
//-----------------------------------------------------------------------------
void LayoutRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  // レンダーターゲットの変更
  if(m_pRenderTarget) gfx::GFGL::SetRenderTarget( m_pRenderTarget );
  else                gfx::GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );

  if(m_pDepthStencil) gfx::GFGL::SetDepthStencil( m_pDepthStencil );
  else                gfx::GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );

  BeginDraw( rDrawContext );

  // Drawが呼ばれた順にLytWkを描画していく
  for(u32 i=0; i<m_lytWkNum; ++i)
  {
    m_pLytWkList[i]->Draw( m_eDispType[i], m_eCameraType[i] );
  }

  EndDraw( rDrawContext );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  描画開始時に呼び出す
 */
//-----------------------------------------------------------------------------
void LayoutRenderPath::BeginDraw( const RenderingPipeLine::DrawContext& rDrawContext )
{
  u32 view_x, view_y;
  view_x = view_y = 0;

  // フレームバッファに描画結果を反映
  gfx::GFGL::BeginScene();

  // レイアウトの座標系とOpenGLでの座標系はビューポートの基点が左下で異なるためレイアウトの座標系にあわせてビューポートを設定
  u32 width = rDrawContext.m_DisplayWidth;
  u32 height = rDrawContext.m_DisplayHeight;
  u32 frameBufferHeight = height;

  gfx::GFGL::SetViewport( view_x, frameBufferHeight - view_y - height, width, height );
  gfx::GFGL::SetScissor( view_x, frameBufferHeight - view_y - height, width, height );

  // 各描画設定を有効に
  gfx::GFGL::SetBlendState( m_pBlendStateObject );
//  gfx::GFGL::SetDepthStencil( m_pDepthStencil );
  gfx::GFGL::SetRasterizerState( m_pRasterizerStateObject );

#if defined(GF_PLATFORM_CTR)
  // FragmentOperation::Add3DCommandと同様の処理をしているはず・・・
  nn::gr::CommandBufferJumpHelper* jh = gfx::GFGL::GetJumpHelper();

  nn::gr::Viewport viewport;
  viewport.Set( view_x, frameBufferHeight - view_y - height, height, width );
  jh->Put( viewport.MakeCommand( jh->GetCurrentPtr() ) );

  nn::gr::Scissor scissor;
  scissor.Set( view_x, frameBufferHeight - view_y - height, height, width );
  jh->Put( scissor.MakeCommand( jh->GetCurrentPtr() ) );

  nn::gr::RenderState render_state;
  {
    render_state.cullingTest.isEnable = false;
    render_state.wBuffer.wScale = 0.0f;
    render_state.wBuffer.depthRangeNear = 0.0f;
    render_state.wBuffer.depthRangeFar = 1.0f;
    render_state.wBuffer.polygonOffsetUnit = 0;
    render_state.stencilTest.isEnable = false;
    render_state.depthTest.isEnable = false;
    render_state.colorMask = nn::gr::CTR::RenderState::COLOR_MASK_RGBA;

    jh->Put( render_state.MakeCommand(jh->GetCurrentPtr()) );
  }
#endif
}

//-----------------------------------------------------------------------------
/**
 *  @brief  描画終了時に呼び出す
 */
//-----------------------------------------------------------------------------
void LayoutRenderPath::EndDraw( const RenderingPipeLine::DrawContext& rDrawContext )
{
  u32 width = rDrawContext.m_pRenderTarget->GetSurfaceDesc().width;
  u32 height = rDrawContext.m_pRenderTarget->GetSurfaceDesc().height;

  gfx::GFGL::SetViewport( 0, 0, width, height );
  gfx::GFGL::SetScissor( 0, 0, width, height );

  gfx::GFGL::EndScene();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  登録位置の取得
 *  @param  order   描画順
 *  @return 登録位置
 */
//-----------------------------------------------------------------------------
u32 LayoutRenderPath::GetAddIndex( u8 order )
{
  GFL_ASSERT( m_lytWkNum < m_lytWkMaxNum - 1 );

  s32 lytNum = static_cast<s32>(m_lytWkNum);
  s32 index = lytNum;

  for( s32 i=0; i<lytNum; ++i )
  {
    // 登録済みのレイアウトよりも描画順が小さい時
    if( m_eOrderList[i] > order )
    {
      index = i;
      break;
    }
  }

  // 登録済みよりも大きいので最後尾に追加
  if( index == lytNum ) return static_cast<u32>(lytNum);

  // 登録済みよりも小さいのでindexの場所に挿入するためindex以降を後ろにずらす
  for( s32 i=lytNum-1; i>=index; i-- )
  {
    m_pLytWkList[i+1]  = m_pLytWkList[i];
    m_eDispType[i+1]   = m_eDispType[i];
    m_eCameraType[i+1] = m_eCameraType[i];
    m_eOrderList[i+1]  = m_eOrderList[i];
  }

  return static_cast<u32>(index);
}


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)
