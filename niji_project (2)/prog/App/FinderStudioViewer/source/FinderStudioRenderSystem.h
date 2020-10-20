//======================================================================
/**
 * @file FinderStudioRenderSystem.h
 * @date 2016/08/25
 * @author harada_takumi
 * @brief ファインダースタジオ描画システム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_STUDIO_RENDER_SYSTEM_H_INCLUDED__
#define __FINDER_STUDIO_RENDER_SYSTEM_H_INCLUDED__
#pragma once

// gfl2
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

// System
#include "System/include/GflUse.h"
#include "System/include/nijiAllocator.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)
  class FinderStudioRenderPipeLine;
  struct CaptureTexture;

class FinderStudioRenderSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderStudioRenderSystem); //コピーコンストラクタ＋代入禁止

public:
  FinderStudioRenderSystem(gfl2::heap::HeapBase* pHeap,gfl2::heap::HeapBase* pWorkHeap,System::nijiAllocator* pNijiAllocator);
  virtual ~FinderStudioRenderSystem();

  // 初期化処理
  bool InitFunc();
  // 終了処理
  bool Terminate();
  // 更新
  void Update();
  // 描画
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  // ビュー行列を登録
  void RegistViewMtx(const gfl2::math::Matrix34* pViewMtx){ m_pViewMtx = pViewMtx; }
  // プロジェクション行列を登録
  void RegistProjMtx(const gfl2::math::Matrix44* pProjMtx){ m_pProjMtx = pProjMtx; }

  // 描画パイプラインを取得
  FinderStudioRenderPipeLine* GetRenderPipeLine() const { return m_pRenderPipeLine; }

  /// @brief ライト方向変更
  /// @attention ポケモンモデルには反映されない
  void SetLightDirection(const gfl2::math::Vector3& direction){ m_pDirectionalLightNode->SetDirection(direction); }

  /// @brief カラーバッファ取得 リクエスト
  /// @param pOutBuffer 格納先バッファ
  void RequestTransferColorBuffer(CaptureTexture* pOutTexture);

private:
  // カラーバッファをテクスチャへ転送する
  void transferColorBuffer(gfl2::gfx::CtrDisplayNo no);

  // ヒープ
  gfl2::heap::HeapBase* m_pHeap;
  // 一時ヒープ
  gfl2::heap::HeapBase* m_pWorkHeap;
  // アロケータ
  System::nijiAllocator* m_pNijiAllocator;

  // 初期化用
  u32 m_InitSeq;

  // 描画パイプライン
  FinderStudioRenderPipeLine* m_pRenderPipeLine;

  // 環境ノード
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;
  // ディレクショナルライトノード
  gfl2::renderingengine::scenegraph::instance::LightNode* m_pDirectionalLightNode;

  // プロジェクション行列
  // カメラシステムから受け取る
  const gfl2::math::Matrix44* m_pProjMtx;
  // ビュー行列
  // カメラシステムから受け取る
  const gfl2::math::Matrix34* m_pViewMtx;

  // カメラシステムが無い時用（固定カメラ）
  gfl2::math::Matrix34 m_ProvViewMtx;
  gfl2::math::Matrix44 m_ProvProjMtx;

  // 撮影用テクスチャ
  CaptureTexture* m_pCaptureTexture;
};

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

#endif // __FINDER_STUDIO_RENDER_SYSTEM_H_INCLUDED__