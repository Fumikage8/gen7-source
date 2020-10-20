//======================================================================
/**
 * @file FinderStudioRenderPipeLine.h
 * @date 2016/08/25
 * @author harada_takumi
 * @brief ファインダースタジオ描画パイプライン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_STUDIO_RENDER_PIPELINE_H_INCLUDED__
#define __FINDER_STUDIO_RENDER_PIPELINE_H_INCLUDED__
#pragma once

// 継承元
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include "DemoLib/Object/include/ObjectRenderingPipeLine.h"

// gfl2
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

// System
#include "System/include/GflUse.h"

GFL_NAMESPACE_BEGIN(System)
  class ModelRenderPath;
  class NijiEdgeMapSceneRenderPath;
  class NijiOutLinePostSceneRenderPath;
  class nijiAllocator;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(btl)
  class BgSystem;
GFL_NAMESPACE_END(btl)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(renderer)
  class FrameBufferClearAlphaPath;
GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

class FinderStudioRenderPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine, public DemoLib::Obj::ObjectRenderPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderStudioRenderPipeLine); //コピーコンストラクタ＋代入禁止

public:
  FinderStudioRenderPipeLine(gfl2::heap::HeapBase* pHeap,gfl2::heap::HeapBase* pWorkHeap,gfl2::gfx::IGLAllocator* pAllocator);
  virtual ~FinderStudioRenderPipeLine();

  // エッジマップ・アウトラインパスを読み込み
  void LoadEdgeMapOutLineAsync();
  bool IsLoadEdgeMapOutLine();
  //! @brief エッジマップ・アウトラインパスをセットアップ
  //! @return セットアップが成功したらtrue
  bool SetupEdgeMapOutLine();

  /// @brief クリアバッファ用パスの初期化
  void InitializeBufferClearAlphaPath();

  /// @brief 環境ノードを登録
  void SetDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );

  /// @brief 背景描画パスをセット
  void SetBgRenderPath(btl::BgSystem* pBgSystem);
  /// @brief 背景描画パスをリセット
  void ResetBgRenderpath();

  /// @brief エッジマスクを追加
  void AddEdgeMask(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode);
  void RemoveEdgeMask(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode);

  /// @brief フロアマスク用モデルをセット
  void AddFloorMask(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode);
  void RemoveFloorMask(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode);

  // RenderingPipeLineから継承 ======================================================================
  virtual b32 StartRenderPath();
  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();

  // ObjectRenderPipeLineから継承 ===================================================================
  //モデルを追加
  virtual void AddObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt ){}
  virtual void RemoveObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt ){}
  //ポケモデルを追加
  virtual void AddObject_Poke( PokeTool::PokeModel *pokeModel , int opt );
  virtual void RemoveObject_Poke( PokeTool::PokeModel *pokeModel , int opt );
  //キャラモデルを追加
  virtual void AddObject_Chara( poke_3d::model::CharaModel *charaModel , int opt );
  virtual void RemoveObject_Chara( poke_3d::model::CharaModel *charaModel , int opt );
  //着せ替えキャラモデルを追加
  virtual void AddObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt );
  virtual void RemoveObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt );

  // デバッグ用
#if PM_DEBUG
  /// @brief 天球用の描画パスをリセット
  void ResetSkyBoxRenderPath(){ m_pSkyBoxRenderPath = NULL; }
#endif

private:
  // ヒープ
  gfl2::heap::HeapBase* m_pHeap;
  // 一時ヒープ
  gfl2::heap::HeapBase* m_pWorkHeap;
  // アロケータ
  gfl2::gfx::IGLAllocator* m_pAllocator;

  // 描画パス回し用
  u32 m_RenderPathCnt;

  // シェーダリソース用バッファ
  void* m_pEdgeMapRenderPathRes;
  void* m_pOutLineRenderPathRes;

  // 描画パス
  System::ModelRenderPath* m_pModelRenderPath;
  System::NijiEdgeMapSceneRenderPath* m_pEdgeMapPath;
  System::NijiOutLinePostSceneRenderPath* m_pOutLinePath;
  // フロアマスク
  System::ModelRenderPath* m_pFloorMaskPath;

  // 背景用描画パス（外部から参照）
  System::ModelRenderPath* m_pBgRenderPath;
  System::ModelRenderPath* m_pFarRenderPath;
  System::ModelRenderPath* m_pSkyBoxRenderPath;

  // IDエッジ用
  gfl2::gfx::Texture*  m_pDepthStencilTexture;

  // アルファクリア用パス
  poke_3d::renderer::FrameBufferClearAlphaPath* m_pBufferClearAlphaPath;
  // アルファクリア用バッファ
  void* m_pFrameBufferClearAlphaBuffer;

  // 環境ノード（外部から参照）
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvBase;

  // 通常モデル最大数
  static const u32 NUM_MAX_MODEL;
};

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

#endif // __FINDER_STUDIO_RENDER_PIPELINE_H_INCLUDED__