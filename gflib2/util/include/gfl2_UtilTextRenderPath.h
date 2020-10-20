#ifndef GFLIB2_UTIL_UTILTEXTRENDERPATH_H_INCLUDED
#define GFLIB2_UTIL_UTILTEXTRENDERPATH_H_INCLUDED
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file    gfl2_UtilTextRenderPath.h
 *  @brief  テキスト描画パス
 *  @author  fukushima_yuya
 *  @date    2015.02.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <renderingengine/include/renderer/gfl2_RenderingPipeLine.h>

#if defined(GF_PLATFORM_WIN32)
#include <nw/font.h>
#include <nw/font/font_RectDrawer.h>
#elif defined(GF_PLATFORM_CTR)
#include <nw/font2.h>
#include <nw/font2/font_RectDrawer.h>
#endif
#include <util/include/gfl2_DrawUtilText.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(gfx)
class IGLAllocator;
GFL_NAMESPACE_END(gfx)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)

using gfl2::renderingengine::renderer::RenderingPipeLine;

class UtilTextRenderPath : public RenderingPipeLine::IRenderPath
{

public:
  /// コンストラクタ
  UtilTextRenderPath( gfl2::gfx::IGLAllocator* allocator );
  /// デストラクタ
  virtual ~UtilTextRenderPath( void );

  /**
   * @brief    レンダーターゲットの設定
   * @none    NULLのときはExecuteに渡されたDrawContextから取得する
   */
  void SetRenderTarget( const gfx::Surface* pRenderTarget, const gfx::Surface* pDepthStencil );

  /**
   * @brief    描画するTextのリストをクリアする
   * @none    NULLのときはExecuteに渡されたDrawContextから取得する
   */
  void ClearDrawableText( void );

public:
  /**
   * @brief    描画するTextを追加する
   */
  void AddDrawableText( DrawUtilText::UtilTextInfo& info,gfl2::util::DrawUtilText* pDrawUtilText );
  /**
   * @brief    描画テキスト数を取得
   * @retval  m_textNum    描画テキスト登録数
   */
  u32 GetAddTextCount( void ) const { return m_textNum; };

public:
  /**
   * @brief    IRenderPathのメンバ関数のオーバーライド
   * @param    rDrawContext
   */
  virtual void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

private:
  const gfx::Surface*    m_pRenderTarget;
  const gfx::Surface*    m_pDepthStencil;

  DrawUtilText::UtilTextInfo  m_info[32];
  u32                          m_textNum;
  gfl2::util::DrawUtilText*   m_pDrawUtilText;

  gfx::BlendStateObject*      m_pBlendStateObject;
};

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif  // GFLIB2_UTIL_UTILTEXTRENDERPATH_H_INCLUDED
