#if !defined(GFL2_RENDERINGENGINE_RENDERER_UTIL_COLORSHADERTEXTURERENDERPATH_H_INCLUDED)
#define GFL2_RENDERINGENGINE_RENDERER_UTIL_COLORSHADERTEXTURERENDERPATH_H_INCLUDED
#pragma once

#include <renderingengine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

//-----------------------------------------------------------
//! @brief カラーシェーダテクスチャ描画パスクラス
//-----------------------------------------------------------
class ColorShaderTextureRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
  ColorShaderTextureRenderPath();
  virtual ~ColorShaderTextureRenderPath();

  void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

  /**
   * @brief カラーシェーダの色の設定
   * @param color カラーシェーダで適用する色
   * @param executeFrame 実行するフレーム
   * @note executeFrameは基本1. 処理落ち発生の際に, 確実に実行してほしいフレーム数を指定する
   */
  void SetColorShaderTextureColor(const math::Vector4& color, s32 executeFrame = 1);

  void SetOriginalColorTexture(const gfx::Texture* pTexture)
  {
    m_pOriginalColorTexture = pTexture;
  }

  void SetColorShaderTexture(const gfx::Texture* pTexture)
  {
    m_pColorShaderTexture = pTexture;
  }

  void SetColorShaderRenderTargetPlane(scenegraph::instance::ModelInstanceNode* pNode)
  {
    m_pRenderTargetModelInstanceNode = pNode;
  }

private:
  s32 m_ExecuteFrame;                           //!<@brief 実行するフレーム数
  math::Vector4 m_Color;                        //!<@brief xyzがRGB, wが補間係数
  const gfx::Texture* m_pOriginalColorTexture;  //!<@brief 元画像テクスチャ
  const gfx::Texture* m_pColorShaderTexture;    //!<@brief カラーシェーダ適用テクスチャ
  scenegraph::instance::ModelInstanceNode* m_pRenderTargetModelInstanceNode;  //!<@brief 板ポリゴンを持つだけのノード
};

}}}}

#endif
