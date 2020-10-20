//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file    gfl2_UtilTextRenderPath.cpp
 *  @brief  テキスト描画パス
 *  @author  fukushima_yuya
 *  @date    2015.02.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// gfl2
#include <util/include/gfl2_UtilTextRenderPath.h>
#include <util/include/gfl2_DrawUtilText.h>

#include <heap/include/gfl2_Heap.h>
#include <gfx/include/gfl2_DrawUtil.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)


//--------------------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 */
//--------------------------------------------------------------------------------
UtilTextRenderPath::UtilTextRenderPath( gfl2::gfx::IGLAllocator* allocator )
: IRenderPath()
, m_pRenderTarget( NULL )
, m_pDepthStencil( NULL )
, m_info()
, m_textNum(0)
, m_pBlendStateObject(NULL)
{
  m_pBlendStateObject = gfx::GFGL::CreateBlendStateObject( allocator );
  m_pBlendStateObject->SetBlendEnable(true);
  m_pBlendStateObject->SetBlendOp(gfx::BlendOp::Add);
  m_pBlendStateObject->SetBlendFunc(gfx::BlendFunc::SrcAlpha, gfx::BlendFunc::InverseSrcAlpha);
  m_pBlendStateObject->UpdateState();
}

//--------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
//--------------------------------------------------------------------------------
UtilTextRenderPath::~UtilTextRenderPath( void )
{
  ClearDrawableText();
  GFX_SAFE_DELETE(m_pBlendStateObject);
}

//--------------------------------------------------------------------------------
/**
 * @brief    レンダーターゲットの設定
 * @param    pRenderTarget    レンダーターゲット
 * @param    pDepthStencil    デプスステンシル
 */
//--------------------------------------------------------------------------------
void UtilTextRenderPath::SetRenderTarget( const gfx::Surface* pRenderTarget, const gfx::Surface* pDepthStencil )
{
  m_pRenderTarget = pRenderTarget;
  m_pDepthStencil = pDepthStencil;
}

//--------------------------------------------------------------------------------
/**
 * @brief    描画するTextのリストをクリアする
 * @none    NULLのときはExecuteに渡されたDrawContextから取得する
 */
//--------------------------------------------------------------------------------
void UtilTextRenderPath::ClearDrawableText( void )
{
  for( u32 i=0; i<32; i++ )
  {
    m_info[i] = DrawUtilText::UtilTextInfo();
  }

  m_textNum = 0;
}

//--------------------------------------------------------------------------------
/**
 * @brief    描画するTextを追加する
 */
//--------------------------------------------------------------------------------
void UtilTextRenderPath::AddDrawableText( DrawUtilText::UtilTextInfo& info, gfl2::util::DrawUtilText* pDrawUtilText )
{
  m_info[m_textNum] = info;
  m_textNum++;
  m_pDrawUtilText = pDrawUtilText;
}

//--------------------------------------------------------------------------------
/**
 * @brief    IRenderPathのメンバ関数のオーバーライド
 */
//--------------------------------------------------------------------------------
void UtilTextRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  // レンダーターゲットの変更
  if(m_pRenderTarget) gfx::GFGL::SetRenderTarget( m_pRenderTarget );
  else                gfx::GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );

  if(m_pDepthStencil) gfx::GFGL::SetDepthStencil( m_pDepthStencil );
  else                gfx::GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );

  // 以下が描画
  u32 width = rDrawContext.m_DisplayWidth;
  u32 height = rDrawContext.m_DisplayHeight;
  u32 frameBufferHeight = height;

  // 描画開始
  gfx::GFGL::BeginScene();

  // 各描画設定を有効に
  gfx::GFGL::SetViewport( 0, frameBufferHeight - 0 - height, width, height );
  gfx::GFGL::SetScissor( 0, frameBufferHeight - 0 - height, width, height );

  gfx::GFGL::SetBlendState( m_pBlendStateObject );

  if( m_pDrawUtilText != NULL )
  {
    m_pDrawUtilText->BeginDraw( width, height );

    for( u32 i=0; i<m_textNum; ++i )
    {
      m_pDrawUtilText->Draw( m_info[i] );
    }

    m_pDrawUtilText->EndDraw();
  }

  gfx::GFGL::SetViewport( 0, 0, width, height );
  gfx::GFGL::SetScissor( 0, 0, width, height );

  // DrawUtil::End
  gfx::GFGL::EndScene();
}

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)
