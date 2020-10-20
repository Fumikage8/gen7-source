#if !defined(NIJI_PROJECT_APPLIB_UTIL_APPRENDERINGMANAGERPOKEMODELRENDERER_H_INCLUDED)
#define NIJI_PROJECT_APPLIB_UTIL_APPRENDERINGMANAGERPOKEMODELRENDERER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    AppRenderingManagerPokeModelRenderer.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.02
 @brief   PokeModelRenderer
 */
//==============================================================================
// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// poke_3d_libのインクルード
#include <model/include/gfl2_BaseCamera.h>

// nijiのインクルード
#include <PokeTool/include/PokeModelSystem.h>
#include <PokeTool/include/PokeModel.h>

#include <System/include/nijiAllocator.h>
#include <System/include/Camera/InFrameCamera.h>

#include <System/include/RenderPath/EdgeMapRenderPath.h>
#include <System/include/RenderPath/OutLinePostRenderPath.h>
#include <System/include/RenderPath/ModelRenderPath.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppCamera.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(renderer)
GFL_NAMESPACE_BEGIN(util)
  class StretchBltFrameBufferPath;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(gfx)
  class Texture;
GFL_NAMESPACE_END(gfx)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(renderer)
  class DistortionPostRenderPath;
GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(poke_3d)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
GFL_NAMESPACE_BEGIN(app_rendering_manager_internal) 




//! @brief  AppRenderingManagerで使うポケモンモデルレンダリングパイプラインの型定義
class PokeModelRendererType
{
public:
  //! @brief  モード
  class Mode
  {
  public:
    enum Tag
    {
      COMMON,       //!< 共通の描画パスを使用する(通常はこちら)
      APP_ORIGINAL, //!< Appオリジナルの描画パスを使用する(オフスクリーンはこちら(画面サイズ以外のテクスチャにも対応した描画パス))
    };
  };
};




//! @brief  AppRenderingManagerで使うポケモンモデルレンダリングパイプライン
class PokeModelRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeModelRenderingPipeLine);

  friend class PokeModelRendererBuilder;

private:
  // 生成/破棄はfriendからのみ可能
  PokeModelRenderingPipeLine(
      PokeModelRendererType::Mode::Tag  mode,
      app::util::Heap*                  pHeap,                            // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
      System::nijiAllocator*            pNijiAllocatorForEdgeMapTexture,  // VRAM限定。エッジマップテクスチャ用のVRAM確保を行う。useDistortionPathがtrueのときは屈折テクスチャ用のVRAM確保も行う。
      void*                             pEdgeMapResBuf,
      void*                             pOutLineResBuf,
      u32                               pokeModelNumMax,                  // Addできるポケモンモデルの最大数(通常モデル、キャラモデル、着せ替えモデルは含まない)
      u32                               modelNumMax,                      // Addできる通常モデルの最大数(ポケモンモデル、キャラモデル、着せ替えモデルは含まない)
      u32                               charaModelNumMax,                 // Addできるキャラモデルの最大数(ポケモンモデル、通常モデル、着せ替えモデルは含まない)
      u32                               dressUpModelNumMax,               // Addできる着せ替えモデルの最大数(ポケモンモデル、通常モデル、キャラモデルは含まない)
      bool                              useDistortionPath                 // ゆがみシェーダーを使用するときtrue
  );
  virtual ~PokeModelRenderingPipeLine();

public:
  virtual b32 StartRenderPath();
  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
  virtual b32 NextRenderPath();

public:
  System::NijiEdgeMapSceneRenderPath* GetEdgeMapPath(void) const;
  System::ModelRenderPath*            GetModelRenderPath(void) const;
  poke_3d::renderer::DistortionPostRenderPath* GetDistortionPath(void) const;  // ゆがみシェーダーを使用しないときはNULL

  void SetOutLinePathOuterEdgeMapTexture(gfl2::gfx::Texture* pOuterDepthStencilTexture);
  void UnsetOutLinePathOuterEdgeMapTexture(void);
  gfl2::gfx::Texture* GetOutLinePathOuterEdgeMapTexture(void) const;  // m_pOuterDepthStencilTextureを返す

  const System::Camera::InFrameCamera* GetInFrameCamera(void) const;
  void SetInFrameCamera(const System::Camera::InFrameCamera* pCamera);
  void UnsetInFrameCamera(void);

  const app::util::AppCamera* GetAppCamera(void) const;
  void SetAppCamera(const app::util::AppCamera* pCamera);
  void UnsetAppCamera(void);

  const poke_3d::model::BaseCamera* GetCamera(void) const;
  void SetCamera(const poke_3d::model::BaseCamera* pCamera);
  void UnsetCamera(void);

  void SetPokeModelSystem(PokeTool::PokeModelSystem* pokeModelSys) { m_pokeModelSys = pokeModelSys; }
  void UnsetPokeModelSystem(void) { m_pokeModelSys = NULL; }

private:
  PokeModelRendererType::Mode::Tag          m_mode;

  u32                                       m_RenderPathCnt;

  System::NijiEdgeMapSceneRenderPath*       m_pEdgeMapPath;
  System::NijiOutLinePostSceneRenderPath*   m_pOutLinePath;
  System::ModelRenderPath*                  m_pModelRenderPath;

  // ゆがみシェーダー用
  gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath* m_pStretchBltFrameBufferPath;
  poke_3d::renderer::DistortionPostRenderPath*                      m_pDistortionPostRenderPath;

  // @fix GFNMCat[3636] アプリに出るポケモンモデル：CTR版でＩＤエッジが表示されていない
  // エッジ用
  gfl2::gfx::Texture*  m_pDepthStencilTexture;  // 内部で生成したテクスチャ
  gfl2::gfx::Texture*  m_pOuterDepthStencilTexture;  // 外部から渡してもらったテクスチャ

  System::nijiAllocator*                    m_pNijiAllocatorForEdgeMapTexture;  // VRAM限定。
  const System::Camera::InFrameCamera*      m_pInFrameCamera;
  const app::util::AppCamera*               m_pAppCamera;
  const poke_3d::model::BaseCamera*         m_pCamera;

  PokeTool::PokeModelSystem*                m_pokeModelSys;
};




//! @brief  AppRenderingManagerで使うポケモンモデルレンダラー生成/破棄クラス
class PokeModelRendererBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeModelRendererBuilder);

public:
  PokeModelRendererBuilder(void);
  virtual ~PokeModelRendererBuilder();

public:
  //! @brief  非同期生成　開始
  //! @note  Updateを毎フレーム呼んでいないと非同期生成処理は進みません。
  void StartAsyncCreate(
      app::util::Heap*            pHeap,             // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
      gfl2::fs::AsyncFileManager* pAsyncFileManager  // ファイルマネージャ
  );
  //! @brief  同期生成 
  void SyncCreate(
      app::util::Heap*            pHeap,             // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
      gfl2::fs::AsyncFileManager* pAsyncFileManager  // ファイルマネージャ
  );
  //! @brief  生成が完了しているか
  //! @return  生成が完了したらtrueを返す。
  //!          生成を開始していないときはfalseを返す。生成中はfalseを返す。
  //! @note  Updateを毎フレーム呼んでいないと非同期生成処理は進みません。
  bool IsCreated(void) const;

  //! @brief  破棄できるか
  //! @return  破棄できるときtrueを返す。
  //!          生成中は破棄できないのでfalseを返す。
  //! @caution  描画をオフにしてから1フレーム後に破棄しないと安全ではありません。
  //!             この関数は「破棄できるか否か」だけを返しており、「オフにして1フレーム空けたか否か」は判定していません。
  //!             「オフにして1フレーム空けたか否か」は呼び出し元で判定して下さい。
  bool CanDestroy(void) const;

  //! @brief  ポケモンモデルレンダリングパイプラインを生成する
  PokeModelRenderingPipeLine* CreateRenderingPipeLine(
      PokeModelRendererType::Mode::Tag  mode,
      app::util::Heap*                  pHeap,                            // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
      System::nijiAllocator*            pNijiAllocatorForEdgeMapTexture,  // VRAM限定。エッジマップテクスチャ用のVRAM確保を行う。useDistortionPathがtrueのときは屈折テクスチャ用のVRAM確保も行う。
      u32                               pokeModelNumMax,                  // Addできるポケモンモデルの最大数(通常モデル、キャラモデル、着せ替えモデルは含まない)
      u32                               modelNumMax,                      // Addできる通常モデルの最大数(ポケモンモデル、キャラモデル、着せ替えモデルは含まない)
      u32                               charaModelNumMax,                 // Addできるキャラモデルの最大数(ポケモンモデル、通常モデル、着せ替えモデルは含まない)
      u32                               dressUpModelNumMax,               // Addできる着せ替えモデルの最大数(ポケモンモデル、通常モデル、キャラモデルは含まない)
      bool                              useDistortionPath                 // ゆがみシェーダーを使用するときtrue
  );
  void DestroyRenderingPipeLine(PokeModelRenderingPipeLine* pPokeModelRenderingPipeLine);

  void Update(void);

private:
  //! @brief  生成状態
  enum CreateState
  {
    CREATE_STATE_NONE,      //!< 生成していない
    CREATE_STATE_CREATING,  //!< 生成中
    CREATE_STATE_CREATED,   //!< 生成済み
  };

private:
  void* m_pEdgeMapResBuf;
  void* m_pOutLineResBuf;

  CreateState                 m_createState;
  gfl2::fs::AsyncFileManager* m_pAsyncFileManager;  // ファイルマネージャ
};




GFL_NAMESPACE_END(app_rendering_manager_internal) 
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APPLIB_UTIL_APPRENDERINGMANAGERPOKEMODELRENDERER_H_INCLUDED

