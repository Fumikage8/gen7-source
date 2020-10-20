#if !defined(GFL2_RENDERINGENGINE_RENDERER_UTIL_DOFRENDERPATH_H_INCLUDED)
#define GFL2_RENDERINGENGINE_RENDERER_UTIL_DOFRENDERPATH_H_INCLUDED
#pragma once

#include <renderingengine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

//-----------------------------------------------------------
//! @brief 被写界深度描画パスクラス
//-----------------------------------------------------------
class DofRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
  /**
   * @brief パスの初期化情報
   */
  struct InitDescription
  {
    //!< @brief 描画用のモデルインスタンスノード
    scenegraph::instance::ModelInstanceNode* m_pTargetModelInstanceNode;
    //!< @brief 描画元テクスチャ
    const gfx::Texture* m_pSourceTexture;
    InitDescription() : m_pTargetModelInstanceNode(NULL), m_pSourceTexture(NULL){}
  };

  DofRenderPath();
  virtual ~DofRenderPath();

  void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

  /**
   * @brief 初期化処理
   * @param pAllocator アロケータ
   * @param desc 初期化データ
   * @note ぼかしの設定がなされているモデルデータと対象となるテクスチャの設定
   */
  void Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc);

  /**
   * pbrief 焦点距離の設定
   * @param focusLength 焦点距離
   */
  void SetFocusLength(const f32 focusLength);

  /**
   * @brief 描画対象面の切り替え
   * @param dofType 前面/背面
   */
  void ChangeDofType(DofType dofType);

  /**
   * @brief ブラーのぼかし率取得
   */
  f32 GetBlurScale() const;

  /**
   * @brief ブラーのぼかし率設定
   * @param blurScale ぼかし率
   */
  void SetBlurScale(f32 blurScale);

  /**
   * @brief ビュー行列を設定
   */
  void SetViewMatrix(const gfl2::math::Matrix34& viewMatrix)
  {
    m_ViewMatrix = viewMatrix;
  }

  /**
   * @brief 射影行列を設定
   */
  void SetProjMatrix(const gfl2::math::Matrix44& projMatrix)
  {
    m_ProjMatrix = projMatrix;
  }

private:
  //!< @brief 前面/背面を見分けるメッシュ名
  static const c8* s_FrontBackMeshName[DofType::NumberOf];

  //!< @brief 焦点距離
  f32 m_FocusLength;
  //!< @brief ブラーのぼかし率
  f32 m_BlurScale;
  //!< @brief 現在描画対象になっている面(前面/背面)
  DofType m_DofType;
  //!< @brief 元画像テクスチャ
  const gfx::Texture* m_pSourceTexture;
  //!< @brief ぼかしのコンバイナ設定が組まれている板ポリゴンを持ったModelInstanceNode
  scenegraph::instance::ModelInstanceNode* m_pDofRenderTargetModelInstanceNode;

  gfl2::math::Matrix34 m_ViewMatrix;                  //!< @brief ビュー行列
  gfl2::math::Matrix44 m_ProjMatrix;                  //!< @brief 射影行列
};

}}}}

#endif
