#ifndef __GFL_G3DRENDERSYSTEM_H__
#define __GFL_G3DRENDERSYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dRenderSystem.h
 *	@brief  3D描画システム
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/gfx/gfx_SceneUpdateHelper.h>  // #include <nw/gfx.h> ではgfx_SceneUpdateHelper.hはインクルードされないようだ。

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp
#include <grp/gfl_GraphicsSystem.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSceneSystem.h>
#include <grp/g3d/gfl_G3dSceneCalculateGroup.h>
#include <grp/g3d/gfl_G3dSceneDrawGroup.h>
#include <grp/g3d/gfl_G3dModel.h>

#if GFL_GRP_G3D_H3D_USE
#include <grp/g3d/gfl_G3dH3dModel.h>
#endif  // GFL_GRP_G3D_H3D_USE


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D描画システムクラス
//=====================================
class RenderSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(RenderSystem);


  // 下記クラスからしかアクセスしない
  friend class Scene;


public:
  //---------------------------------------------------------------------------
  /**
   *           構造体宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	シーンシステムの設定内容
  //=====================================
  struct Description
  {
    // m_render_context用の設定
    s32  max_cameras;        // カメラの最大数
    s32  max_light_sets;     // ライトセットの最大数
    s32  max_fogs;           // フォグの最大数
    s32  max_vertex_lights;  // 頂点ライトの最大数
 
    //-----------------------------------------------------------------------------
    /**
     *  @brief         コンストラクタ
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : max_cameras        (nw::gfx::RenderContext::DEFAULT_MAX_CAMERAS),
        max_light_sets     (nw::gfx::RenderContext::DEFAULT_MAX_LIGHT_SETS),
        max_fogs           (nw::gfx::RenderContext::DEFAULT_MAX_FOGS),
        max_vertex_lights  (nw::gfx::LightSet::DEFAULT_MAX_VERTEX_LIGHTS)
    {}
  };



  //-----------------------------------------------------------------------------
  /**
   *    ボックステストの設定,パラメータ取得
   */
  //-----------------------------------------------------------------------------
  static void SetBoxTest( bool flag );
  static bool IsBoxTest( void );
  static s32 GetBoxTestCutCount( void );
  static s32 GetBoxTestThroughCount( void );


  // レンダリングしたメッシュ数をカウントアップ
  inline void ClearRenderMeshCount(void){ m_render_mesh_count = 0; }
  inline void AddRenderMeshCount(void){ ++m_render_mesh_count; }
  inline s32 GetRenderMeshCount( void ) const { return m_render_mesh_count; }
private:
  //-----------------------------------------------------------------------------
  /**
   *					クラス宣言
   */
  //-----------------------------------------------------------------------------
  //-------------------------------------
  ///	モデルのユーザーパラメータをチェックして、描画要素に加えるか否かを判定する関数オブジェクト
  //=====================================
  class ModelUserParameterCheckFunctor : public nw::gfx::ISceneUpdater::IsVisibleModelFunctor
  {
    GFL_FORBID_COPY_AND_ASSIGN(ModelUserParameterCheckFunctor);
  public:
    ModelUserParameterCheckFunctor(void)
      : m_render_queue_index(0)
    {
      // nw::gfx::ISceneUpdater::IsVisibleModelFunctor にはコンストラクタもデストラクタもない
      // 何もしない
    }
    ModelUserParameterCheckFunctor(s32 render_queue_index)
      : m_render_queue_index(render_queue_index)
    {
      // nw::gfx::ISceneUpdater::IsVisibleModelFunctor にはコンストラクタもデストラクタもない
      // 何もしない
    }
    virtual ~ModelUserParameterCheckFunctor()
    {
      // nw::gfx::ISceneUpdater::IsVisibleModelFunctor にはコンストラクタもデストラクタもない
      // 何もしない
    }
    void SetRenderQueueIndex(u32 render_queue_index)
    {
      m_render_queue_index = render_queue_index;
    }
    u32 GetRenderQueueIndex(void) const
    {
      return m_render_queue_index;
    }
  public:
    virtual bool IsVisible(const nw::gfx::Model* model)
    {
      return gfl::grp::g3d::Model::IsUserParameterRenderQueue(model, m_render_queue_index);
    }

  private:
    s32 m_render_queue_index;
  };

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *
   *  @param[in]     allocator  アロケータ
   */
  //-----------------------------------------------------------------------------
  RenderSystem(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, Description* description = NULL);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~RenderSystem(); 

  void InitializeSceneDrawGroup(SceneDrawGroup* scene_draw_group);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境設定からシーン環境を適用する
   *
   *  @param[in]     scene_environment_setting  適用するシーン環境
   *                                            (nw::gfx::SceneEnvironmentSetting::ResolveReferenceが済んでいること)
   */
  //-----------------------------------------------------------------------------
  void SetSceneEnvironmentSetting(const nw::gfx::SceneEnvironmentSetting& scene_environment_setting);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         レンダーコンテキストに設定されたシーン環境を取得する
   */
  //-----------------------------------------------------------------------------
  const nw::gfx::SceneEnvironment& GetSceneEnvironment(s32* max_cameras, s32* max_light_sets, s32* max_fogs, s32* max_vertex_lights) const
  {
    *max_cameras       = m_max_cameras_plus_system_camera;
    *max_light_sets    = m_max_light_sets;
    *max_fogs          = m_max_fogs;
    *max_vertex_lights = m_max_vertex_lights;

    return m_render_context->GetSceneEnvironment();
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境にカメラを設定する
   *
   *  @param[in]     camera_index   カメラのインデックス
   *  @param[in]     camera         カメラ(camera_indexに設定する)
   * 
   *  「camera=NULL, camera_index=インデックス」を渡すとcamera_indexに割り当てていたカメラをなしにする
   */
  //-----------------------------------------------------------------------------
  void SetCamera(s32 camera_index, nw::gfx::Camera* camera);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境のカメラ配列からカメラを取得する
   *
   *  @param[in]     camera_index   カメラのインデックス
   *
   *  @retval        配列のこのインデックスに渡してあったカメラのポインタを得る。
   *                 NULLのときこのインデックスにはカメラを渡していなかった。
   */
  //-----------------------------------------------------------------------------
  nw::gfx::Camera* GetCamera(const s32 camera_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境のカメラ配列からカメラのインデックスを取得する
   *
   *  @param[in]     camera   カメラ
   *
   *  @retval        配列にこのカメラがある場合そのインデックスを得る。
   *                 ない場合CAMERA_INDEX_NONEを返す。
   */
  //-----------------------------------------------------------------------------
  s32              GetCameraIndex(const nw::gfx::Camera* camera) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境のカメラ配列から一番最初にあるカメラのインデックスを取得する
   *
   *  @retval        配列に一つもカメラがない場合CAMERA_INDEX_NONEを返す。
   */
  //-----------------------------------------------------------------------------
  s32              GetFirstCameraIndex(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境にカメラをカメラ配列に設定していないカメラを設定する
   *
   *  @param[in]     camera         カメラ(GetNotSetCameraIndexに設定する)
   * 
   *  「camera=NULL」を渡すとGetNotSetCameraIndexに割り当てていたカメラをなしにする
   */
  //-----------------------------------------------------------------------------
  void SetNotSetCamera(nw::gfx::Camera* camera);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         カメラをカメラ配列に設定していないときに使うカメラのインデックスを取得する
   *
   *  @retval        m_max_cameras+SYSTEM_CAMERA_NOT_SETを返す。
   */
  //-----------------------------------------------------------------------------
  s32 GetNotSetCameraIndex(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境にライトセットを設定する
   *
   *  @param[in]     light_set_index   設定先となるライトセットのインデックス 
   *  @param[in]     light_set         ライトセット。
   *                                   呼び出し元はこのライトセットを保持し続けて下さい。
   *                                   light_set_indexに設定しているライトセットを外す場合はNULLを渡して下さい。
   */
  //-----------------------------------------------------------------------------
  void SetLightSet(s32 light_set_index, nw::gfx::LightSet* light_set);

#if GFL_GRP_G3D_H3D_USE
  //-----------------------------------------------------------------------------
  /**
   *  @brief       h3d環境をgfx環境からつくる
   */
  //-----------------------------------------------------------------------------
  void SetH3dEnvironmentFromSceneEnvironment(const nw::gfx::Camera& camera);
#endif  // GFL_GRP_G3D_H3D_USE

  //-----------------------------------------------------------------------------
  /**
   *  @brief       描画環境を初期化する
   */
  //-----------------------------------------------------------------------------
  void ResetState(void)
  {
    m_render_context->ResetState();

    // コンテキストを有効化します
    m_render_context->ActivateContext();
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief        レンダーキューにレンダーエレメントを追加
   *
   *  @param[in]    camera         レンダーキュー生成に使用するカメラ
   *  @param[in]    scene_system   描画するシーンコンテンツを含むシーンシステム
   */
  //-----------------------------------------------------------------------------
  void BeginSubmitView(SceneDrawGroup* scene_draw_group);
  void SubmitViewModel(
      nw::gfx::Camera*       camera,
      SceneSystem*           scene_system,
      nw::gfx::Model*        model,
      SceneDrawGroup*        scene_draw_group );
#if GFL_GRP_G3D_H3D_USE
  void SubmitViewH3dModel(
      nw::gfx::Camera*       camera,
      SceneSystem*           scene_system,
      H3dModel*              model,
      SceneDrawGroup*        scene_draw_group );
#endif  // GFL_GRP_G3D_H3D_USE
  void SubmitView(
      nw::gfx::Camera*       camera,
      SceneSystem*           scene_system,
      SceneCalculateGroup*   scene_calculate_group,
      SceneDrawGroup*        scene_draw_group );
  void EndSubmitView(SceneDrawGroup* scene_draw_group);

  //-----------------------------------------------------------------------------
  /**
   *  @brief        シーンを描画
   *
   *  @param[in]    camera              このシーンを描画するカメラ
   *  @param[in]    command_cache_dump  コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void RenderScene(
    gfl::grp::GraphicsSystem* graphics,
    gfl::grp::RenderTarget*  render_target,
    nw::gfx::Camera*         camera,
    SceneDrawGroup*          scene_draw_group,
    bool                     command_cache_dump = false);

  void RenderScene(
    //nw::gfx::IRenderTarget*  render_target,
    gfl::grp::RenderTarget*  render_target,
    nw::gfx::Camera*         camera,
    SceneDrawGroup*          scene_draw_group,
    bool                     command_cache_dump = false);

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ステレオディスプレイに対応したシーンの描画
   *                上画面にのみ対応
   * 
   *  @param[in]   
   *  @param[in]   
   *  @param[in]  
   */
  //-----------------------------------------------------------------------------
  void RenderStereoScene(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::RenderTarget*       render_target_left,
    nw::gfx::Camera*              camera_left,
    gfl::grp::RenderTarget*       render_target_right,
    nw::gfx::Camera*              camera_right,
    SceneDrawGroup*               scene_draw_group,
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
    nw::gfx::Camera*              camera_base,
#endif
    bool                          command_cache_dump = false);

#if 0
  //h3dでサポートしきれないのでコメントアウト。
  //-----------------------------------------------------------------------------
  /**
   *  @brief        コマンドセーブだけしてシーンの描画はしない
   */
  //-----------------------------------------------------------------------------
  void CommandSaveNotRenderScene(
    gfl::grp::GraphicsSystem*     graphics_system,
    nw::gfx::IRenderTarget*       render_target,
    SceneDrawGroup*               scene_draw_group,
    bool                          command_cache_dump = false);
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief        コマンドセーブしてあるものを使ってシーンの描画をする
   */
  //-----------------------------------------------------------------------------
  void ReuseCommandSaveRenderScene(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::RenderTarget*       render_target,
    nw::gfx::Camera*              camera);
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ボックステスト　効果カウンタの値クリア
   */
  //-----------------------------------------------------------------------------
  void ClearBoxTestCount( void );


#if GFL_GRP_G3D_H3D_USE
public:  // privateにしたほうがいいか
  inline H3dCommand* GetH3dCommand(void) const { return m_h3d_command; }
  inline H3dEnvironment* GetH3dEnvironment(void) const { return m_h3d_environment; }
  
  b32 IsFragmentAndWBufferDirty() const { return m_fragmentAndWBufferDirty; }

private:
  void createH3dSystem(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void destroyH3dSystem(void);
#endif  // GFL_GRP_G3D_H3D_USE

private:

    // gfx_SceneUpdater.h
    // gfx_SceneUpdater.cpp
    //     class SceneUpdater 
    //         void SubmitView
    // gfx_SceneUpdateHelper.h
    //     class SceneUpdateHelper
    //         class SubmitViewFunctor
    //             void operator() (Model* model)
    // を参考にした。

    //! @brief        カメラの視界に基づいてシーンを更新し、描画キューを構築します。
    //!
    //! @tparam       IsVisibleModelFunctor 表示するモデルを識別するための関数オブジェクトの型です。
    //! @param[in]    skeletonUpdater スケルトンアップデータです。
    //! @param[in]    billboardUpdater ビルボードアップデータです。
    //! @param[in]    renderQueue 描画対象となる要素を集めたキューです。
    //! @param[in]    model モデルです。
    //! @param[in]    camera カメラです。
    //! @param[in]    layerId 描画要素のソート時に最優先に区分される ID です。レイヤーやビューポートの描画を制御するのに用います。
    //! @param[in]    particleLayerId パーティクル描画用の layerId です。
    //! @param[in]    isVisibleModel 表示するモデルを識別するための関数オブジェクトです。
    //! @param[in]    renderSortMode 描画ソートモードです。
    //!
    template<typename IsVisibleModelFunctor>
    static void SceneUpdateHelperSubmitViewModel(
        nw::gfx::SkeletonUpdater* skeletonUpdater,
        nw::gfx::BillboardUpdater* billboardUpdater,
        nw::gfx::RenderQueue* renderQueue,
        nw::gfx::Model* model,
        const nw::gfx::Camera& camera,
        u8 layerId,
        u8 particleLayerId,
        nw::gfx::ISceneUpdater::RenderSortMode renderSortMode,
        nw::gfx::ISceneUpdater::DepthSortMode depthSortMode,
        IsVisibleModelFunctor* isVisibleModel)
    {
        if( model == NULL ) return;
        
        if (depthSortMode == nw::gfx::ISceneUpdater::SORT_DEPTH_OF_TRANSLUCENT_MESH)
        {
            // 半透明のメッシュのみ深度計算を行います。
            // 不透明などのメッシュは深度によるソートがされなくなりますが、
            // 深度計算分のCPUの処理負荷を削減します。
            
            typedef nw::gfx::SceneUpdateHelper::SubmitViewFunctor<
                nw::gfx::RenderQueue::FastEnqueueModelFunctor,
                nw::gfx::RenderQueue::FastEnqueueSkeletalModelFunctor,
                IsVisibleModelFunctor> FastMeshBaseSubmitViewFunctor;
            
            typedef nw::gfx::SceneUpdateHelper::SubmitViewFunctor<
                nw::gfx::RenderQueue::EnqueueModelTranslucentModelBaseFunctor,
                nw::gfx::RenderQueue::EnqueueSkeletalModelTranslucentModelBaseFunctor,
                IsVisibleModelFunctor> FastTranslucentModelBaseSubmitViewFunctor;
            
            if (renderSortMode == nw::gfx::ISceneUpdater::OPAQUE_MESH_BASE_AND_TRANSLUCENT_MODEL_BASE_SORT)
            {
                    FastTranslucentModelBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel)(model);
            }
            else
            {
                    FastMeshBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel)(model);
            }
        }
        else
        {
            // すべてのメッシュの深度計算を行います。
            
            typedef nw::gfx::SceneUpdateHelper::SubmitViewFunctor<
                nw::gfx::RenderQueue::EnqueueModelFunctor,
                nw::gfx::RenderQueue::EnqueueSkeletalModelFunctor,
                IsVisibleModelFunctor> MeshBaseSubmitViewFunctor;
            
            typedef nw::gfx::SceneUpdateHelper::SubmitViewFunctor<
                nw::gfx::RenderQueue::EnqueueModelTranslucentModelBaseFunctor,
                nw::gfx::RenderQueue::EnqueueSkeletalModelTranslucentModelBaseFunctor,
                IsVisibleModelFunctor> TranslucentModelBaseSubmitViewFunctor;
            
            if (renderSortMode == nw::gfx::ISceneUpdater::OPAQUE_MESH_BASE_AND_TRANSLUCENT_MODEL_BASE_SORT)
            {
                    TranslucentModelBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel)(model);
            }
            else
            {
                    MeshBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel)(model);
            }
        }
    }


private:
  // 定数
  // システムで使うカメラ
  enum
  {
    // カメラ配列に対して割り当てる場合は、m_max_camerasからのオフセットとして扱う
    SYSTEM_CAMERA_NOT_SET,  // カメラをカメラ配列に設定していないときに使う
    SYSTEM_CAMERA_MAX
  };

private:
  nw::gfx::RenderContext*                 m_render_context;
  s32                                     m_max_cameras;                     // カメラの最大数
  s32                                     m_max_cameras_plus_system_camera;  // m_max_camerasにシステムで使うカメラ分を加えた個数
  s32                                     m_max_light_sets;
  s32                                     m_max_fogs;
  s32                                     m_max_vertex_lights;
  nw::gfx::RenderKeyFactory*              m_prior_material_render_key_factory;
  nw::gfx::RenderKeyFactory*              m_prior_depth_render_key_factory;
  nw::gfx::MeshRenderer*                  m_mesh_renderer;
  ModelUserParameterCheckFunctor*         m_model_user_parameter_check_functor;
  nw::gfx::ISceneUpdater::RenderSortMode  m_render_sort_mode;
  nw::gfx::ISceneUpdater::DepthSortMode   m_depth_sort_mode;  // SORT_DEPTH_OF_TRANSLUCENT_MESHのとき、半透明以外の深度ソートを省略する。
  s32                                     m_render_mesh_count;  // 最後のレンダリングのメッシュ数
  b32                                     m_fragmentAndWBufferDirty;

#if GFL_GRP_G3D_H3D_USE
  H3dCommand*               m_h3d_command;
  H3dEnvironment*           m_h3d_environment;
#endif  // GFL_GRP_G3D_H3D_USE

#if GFL_GRP_G3D_H3D_USE
public:
  struct RenderSceneParam
  {
    enum ElementType
    {
      ELEMENT_TYPE_NONE,
      ELEMENT_TYPE_GFX,  // gfxのcommandとmesh
      ELEMENT_TYPE_H3D,
      ELEMENT_TYPE_MAX
    };
 
    gfl::grp::GraphicsSystem*               graphics_system;  // Stereo描画でしか使わない
    RenderSystem*                           render_system;
    gfl::grp::RenderTarget*                 render_target;
    nw::gfx::Camera*                        nw_camera;
    gfl::grp::RenderTarget*                 render_target_extension;  // Stereo描画でしか使わない
    nw::gfx::Camera*                        nw_camera_extension;  // Stereo描画でしか使わない
    SceneDrawGroup*                         scene_draw_group;
    nw::gfx::internal::CommandCacheBuilder* command_cache_builder;
    ElementType                             element_type;         
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
    gfl::math::MTX34                        view_mtx;
    gfl::math::MTX34                        inv_view_mtx;
    u32                                     inv_exist;
#endif

    RenderSceneParam(void)
    {
      graphics_system         = NULL;
      render_system           = NULL;
      render_target           = NULL;
      nw_camera               = NULL;
      render_target_extension = NULL;
      nw_camera_extension     = NULL;
      scene_draw_group        = NULL;
      command_cache_builder   = NULL;
      element_type            = ELEMENT_TYPE_NONE;
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      inv_exist               = 0;
#endif
    }
  };

  static void BeginRenderScene( nw::gfx::RenderContext* render_context, RenderSceneParam* param );
  static void BeginGfxRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void EndGfxRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void BeginH3dRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void EndH3dRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);

  static void BeginGfxRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void EndGfxRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void BeginH3dRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void EndH3dRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void endRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
#endif  // GFL_GRP_G3D_H3D_USE
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DRENDERSYSTEM_H__
