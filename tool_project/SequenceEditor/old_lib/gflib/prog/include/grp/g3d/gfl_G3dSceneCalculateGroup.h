#ifndef __GFL_G3DSCENECALCULATEGROUP_H__
#define __GFL_G3DSCENECALCULATEGROUP_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneCalculateGroup.h
 *	@brief  3Dシーン計算グループ
 *	@author	Koji Kawada
 *	@date		2011.02.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------


//-------------------------------------
///	クラスの前方宣言
//=====================================
class Scene;
class SceneNode;


//-------------------------------------
///	3Dシーン計算グループクラス
//=====================================
class SceneCalculateGroup
{
  GFL_FORBID_COPY_AND_ASSIGN(SceneCalculateGroup);

  
  // 下記クラスからしかアクセスしない
  friend class Scene;
  friend class RenderSystem;
  friend class SceneSystem;
  friend class SceneNode;


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
    // m_scene_context用の設定
    s32 max_scene_nodes;                        // シーンノードの上限数         //@未使用削除予定
    s32 max_models;                             // モデルの上限数               //@未使用削除予定
    s32 max_not_skeletal_models;                // スケルタルモデル以外のモデルの上限数
    s32 max_skeletal_models;                    // スケルタルモデルの上限数
    s32 max_cameras;                            // カメラの上限数
    s32 max_lights;                             // ライトの上限数               //@未使用削除予定
    s32 max_fragment_lights;                    // フラグメントライトの上限数
    s32 max_vertex_lights;                      // 頂点ライトの上限数
    s32 max_hemisphere_lights;                  // 半球ライトの上限数
    s32 max_ambient_lights;                     // アンビエントライトの上限数
    s32 max_fogs;                               // フォグの上限数
    s32 max_particles;                          // パーティクルの上限数
    s32 max_particlesets;                       // パーティクルセットの上限数   //@未使用削除予定
    s32 max_particleemitters;                   // パーティクルエミッタの上限数 //@未使用削除予定
    s32 max_particlemodels;                     // パーティクルモデルの上限数   //@未使用削除予定
    s32 max_animatable_nodes;                   // アニメーションを行うノードの上限数  //@未使用削除予定
    bool scene_context_is_fixed_size_memory;    // 生成時以外にもメモリを確保するかどうかのフラグ  //@未使用削除予定

    // m_scene_root用の設定
    s32  max_callbacks;         // 管理できるコールバックの最大数  //@未使用削除予定
    s32  max_children;          // 子の最大数  //@未使用削除予定
    bool scene_root_is_fixed_size_memory;  // 生成時以外にもメモリを確保するかどうかのフラグ  //@未使用削除予定

    // m_particle_context用の設定
    s32  particle_context_max_emission;       // 放出量の最大数  //@未使用削除予定
    s32  particle_context_max_stream_length;  // ストリーム長の最大数  //@未使用削除予定
    b32  particle_context_use_double_buffer;  // ダブルバッファ。trueのときダブルバッファ有効。  //@未使用削除予定
    b32  particle_context_is_fixed_size_memory;  // 生成時以外にもメモリを確保するかどうかのフラグを設定します。  //@未使用削除予定
                                                 // true を指定すると、生成時のみ固定サイズのメモリ確保を行います。
                                                 // false を指定すると、生成時以外にも必要に応じて動的にメモリ確保が行われます。

#if GFL_GRP_G3D_H3D_USE
    // h3d設定
    s32 max_h3d_models;    // H3dModelの上限数
#endif  // GFL_GRP_G3D_H3D_USE

    //-----------------------------------------------------------------------------
    /**
     *  @brief         コンストラクタ
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : max_scene_nodes                    (nw::gfx::SceneContext::DEFAULT_MAX_SCENE_NODES + 128),
        max_models                         (nw::gfx::SceneContext::DEFAULT_MAX_MODELS + 128),
        max_not_skeletal_models            (nw::gfx::SceneContext::DEFAULT_MAX_MODELS + 128),
        max_skeletal_models                (nw::gfx::SceneContext::DEFAULT_MAX_SKELETAL_MODELS + 128),
        max_cameras                        (8),//(nw::gfx::SceneContext::DEFAULT_MAX_CAMERAS),
        max_lights                         (nw::gfx::SceneContext::DEFAULT_MAX_LIGHTS),
        max_fragment_lights                (nw::gfx::SceneContext::DEFAULT_MAX_FRAGMENT_LIGHTS),
        max_vertex_lights                  (nw::gfx::SceneContext::DEFAULT_MAX_VERTEX_LIGHTS),
        max_hemisphere_lights              (nw::gfx::SceneContext::DEFAULT_MAX_HEMISPHERE_LIGHTS),
        max_ambient_lights                 (nw::gfx::SceneContext::DEFAULT_MAX_AMBIENT_LIGHTS),
        max_fogs                           (nw::gfx::SceneContext::DEFAULT_MAX_FOGS),
        max_particles                      (nw::gfx::SceneContext::DEFAULT_MAX_PARTICLESETS),
        max_particlesets                   (nw::gfx::SceneContext::DEFAULT_MAX_PARTICLESETS),
        max_particleemitters               (nw::gfx::SceneContext::DEFAULT_MAX_PARTICLEEMITTERS),
        max_particlemodels                 (nw::gfx::SceneContext::DEFAULT_MAX_PARTICLEMODELS),
        max_animatable_nodes               (nw::gfx::SceneContext::DEFAULT_MAX_ANIMATABLE_NODES),
        scene_context_is_fixed_size_memory (true),
        max_callbacks                      (nw::gfx::SceneObject::DEFAULT_MAX_CALLBACKS),
        max_children                       (512),//(nw::gfx::SceneObject::DEFAULT_MAX_CHILDREN),
        scene_root_is_fixed_size_memory    (true),
        particle_context_max_emission        (1000),
        particle_context_max_stream_length   (1000),
        particle_context_use_double_buffer   (false),
        particle_context_is_fixed_size_memory(true)
#if GFL_GRP_G3D_H3D_USE
        , max_h3d_models(8)  // @note デフォルトは0にするか
#endif  // GFL_GRP_G3D_H3D_USE
    {}
  };


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   */
  //-----------------------------------------------------------------------------
  SceneCalculateGroup(gfl::heap::NwAllocator* allocator, const Description* description = NULL);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~SceneCalculateGroup();


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンコンテキストに追加されているシーンノードの個数を取得する
   */
  //-----------------------------------------------------------------------------

#define GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(Name,NwArrayName,NwFunctionName) \
  u32 GetNw##Name##NumInNwSceneContext(void) const \
  { \
    u32 num = 0; \
    nw::gfx::NwArrayName##Array::const_iterator end = m_scene_context->Get##NwFunctionName##End(); \
    for(nw::gfx::NwArrayName##Array::const_iterator i = m_scene_context->Get##NwFunctionName##Begin(); i != end; ++i) ++num; \
    return num; \
  }

  // 総数
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(SceneNode,SceneNode,SceneNodes)
  
  // 個別
  // UserRenderNode
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(UserRenderNode,UserRenderNode,UserRenderNodes)
  // モデル(SkeletalModel, ParticleModelの個数を含む)
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(Model,Model,Models)
  // スケルタルモデル
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(SkeletalModel,SkeletalModel,SkeletalModels)
  // ライト(FragmentLight, VertexLight, HemiSphereLight, AmbientLightの個数を含む)
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(Light,Light,Lights)
  // フラグメントライト
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(FragmentLight,FragmentLight,FragmentLights)
  // 頂点ライト
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(VertexLight,VertexLight,VertexLights)
  // 半球ライト
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(HemiSphereLight,HemiSphereLight,HemiSphereLights)
  // アンビエントライト
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(AmbientLight,AmbientLight,AmbientLights)
  // カメラ
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(Camera,Camera,Camera)
  // フォグ
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(Fog,Fog,Fog)
  // パーティクルセット
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(ParticleSet,ParticleSet,ParticleSet)
  // パーティクルエミッタ
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(ParticleEmitter,ParticleEmitter,ParticleEmitter)
  // パーティクルモデル
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(ParticleModel,ParticleModel,ParticleModel)
  // アニメーション
  GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT(AnimatableNode,AnimatableNode,AnimatableNodes)
  
  // モデルのマテリアル(各モデルのマテリアルの個数を合計したもの)(SkeletalModel, ParticleModelのマテリアルの個数を含む)
  u32 GetNwModelMaterialCountInNwSceneContext(void) const
  {
    u32 num = 0;
    nw::gfx::ModelArray::const_iterator end = m_scene_context->GetModelsEnd();
    for(nw::gfx::ModelArray::const_iterator i = m_scene_context->GetModelsBegin(); i != end; ++i)
    {
      const nw::gfx::Model* model = static_cast<const nw::gfx::Model*>(*i);
      num += model->GetMaterialCount();
    }
    return num;
  }
  // スケルタルモデルのマテリアル(各スケルタルモデルのマテリアルの個数を合計したもの)
  u32 GetNwSkeletalModelMaterialCountInNwSceneContext(void) const
  {
    u32 num = 0;
    nw::gfx::SkeletalModelArray::const_iterator end = m_scene_context->GetSkeletalModelsEnd();
    for(nw::gfx::SkeletalModelArray::const_iterator i = m_scene_context->GetSkeletalModelsBegin(); i != end; ++i)
    {
      const nw::gfx::SkeletalModel* model = static_cast<const nw::gfx::SkeletalModel*>(*i);
      num += model->GetMaterialCount();
    }
    return num;
  }
  // パーティクルモデルのマテリアル(各パーティクルモデルのマテリアルの個数を合計したもの)
  u32 GetNwParticleModelMaterialCountInNwSceneContext(void) const
  {
    u32 num = 0;
    nw::gfx::ParticleModelArray::const_iterator end = m_scene_context->GetParticleModelEnd();
    for(nw::gfx::ParticleModelArray::const_iterator i = m_scene_context->GetParticleModelBegin(); i != end; ++i)
    {
      const nw::gfx::ParticleModel* model = static_cast<const nw::gfx::ParticleModel*>(*i);
      num += model->GetMaterialCount();
      ++num;
    }
    return num;
  }

#undef GFL_G3D_SCENE_CALCULATE_GROUP_GET_NW_SCENE_NODE_NUM_IN_NW_SCENE_CONTEXT
  
private:
  enum State
  {
    STATE_NONE,                 // グループ生成直後
    STATE_AFTER_ADD_OR_REMOVE,  // シーンノードを追加もしくは削除した直後
    STATE_AFTER_INITIALIZE,     // Initialize直後
    STATE_AFTER_CALCULATE,      // Calculate直後
    
    STATE_ENUMS32MAX = ENUMS32MAX
  };

private:
  void SetState(const State state)
  {
    m_state = state;
    switch(m_state)
    {
    case STATE_AFTER_ADD_OR_REMOVE: m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_ALL;  break;
    case STATE_AFTER_INITIALIZE:    m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_NONE; break;
    }
  }
  State GetState(void) { return m_state; }
  void SetSceneInitializeProcessNeedOnByTreeFlag(b32 is_add)  // trueのときAdd、falseのときRemove
  {
    if( is_add )
    {
      m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_ALL;
    }
    else
    {
      m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_ALL;
      /* 
      if(m_scene_initialize_process_need == SCENE_INITIALIZE_PROCESS_NONE)
      {
        m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_TRAVERSE;
      }
      */
    }
  }
  void SetSceneInitializeProcessNeedOnByNotCullingFlag(b32 is_add)  // trueのときAdd、falseのときRemove
  {
    if( is_add )
    {
      m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_ALL;
      /*
      if(m_scene_initialize_process_need == SCENE_INITIALIZE_PROCESS_NONE)
      {
        m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_TRAVERSE;  // @note 下の例2の場合はALLにすべきなのだが、その判断がここではできない。
                                                                              //       だから、最適なマテリアルIDになっていない可能性がある。
                                                                              //       最適なマテリアルIDになっていないだけで何かしら
                                                                              //       マテリアルIDは入っているので、TRAVERSEだけでも動作に問題はない。
      }
      */
    }
    else
    {
      m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_ALL;
      /*
      if(m_scene_initialize_process_need == SCENE_INITIALIZE_PROCESS_NONE)
      {
        m_scene_initialize_process_need = SCENE_INITIALIZE_PROCESS_TRAVERSE;
      }
      */
    }

    // 例1
    // (1) モデルその1をNotCullingFlagによってシーンツリーから外す→シーンツリーから外しただけなのでTRAVERSEだけでよい
    // (2) TRAVERSE
    // (3) モデルその1をNotCullingFlagによってシーンツリーに戻す→シーンツリーはこのモデルその1以外は変更していないのでTRAVERSEだけでよい
    // (4) TRAVERSE
   
    // 例2
    // (1) モデルその1をNotCullingFlagによってシーンツリーから外す→シーンツリーから外しただけなのでTRAVERSEだけでよい
    // (2) TRAVERSE 
    // (3) モデルその2をNotCullingFlagによってシーンツリーから外す→シーンツリーから外しただけなのでTRAVERSEだけでよい
    // (4) TRAVERSE 
    // (5) この間に他のモデルその3がシーンツリーに入る→シーンツリー内の順番が変わるかもしれないのでTRAVERSEだけでは足りない
    // (6) INITIALIZE TRAVERSE 
    // (7) モデルその1をNotCullingFlagによってシーンツリーに戻す→モデルその1が外れている間に他のモデルその3が入ってきたので、モデルその1をそのまま戻すだけでは順番がよくないかもしれないので、TRAVERSEだけでは足りない
    // (8) INITIALIZE TRAVERSE  // @note この判断がこのクラスではできないので、m_state, m_scene_initialize_process_needはSceneに持つべきだった
    // (9) 何フレームも経過
    // (10) モデルその2をNotCullingFlagによってシーンツリーに戻す→モデルその2が外れている間に他のモデルその3が入ってきたので、モデルその1をそのまま戻すだけでは順番がよくないかもしれないので、TRAVERSEだけでは足りない
    // (11) INITIALIZE TRAVERSE  // @note この判断がこのクラスではできないので、m_state, m_scene_initialize_process_needはSceneに持つべきだった
  }
  SceneInitializeProcess GetSceneInitializeProcessNeed(void) { return m_scene_initialize_process_need; }

  nw::gfx::SceneContext*  GetSceneContext(void) const  {  return m_scene_context;  }
  nw::gfx::TransformNode* GetSceneRoot(void)    const  {  return m_scene_root;  }

  nw::gfx::ParticleContext* GetParticleContext(void) const  {  return m_particle_context;  }
  b32                       GetParticleEnableSwapBuffer(void) const  {  return m_particle_enable_swap_buffer;  }
  void                      SetParticleEnableSwapBuffer(b32 particle_enable_swap_buffer)  {  m_particle_enable_swap_buffer = particle_enable_swap_buffer;  }


private:
  // Sceneを設定する
  void SetScene(Scene* scene, s32 scene_calculate_group_index );
  
  // シーンノードを追加する
  void AddSceneNode(SceneNode* scene_node);     // シーンへ登録する。
                                                // scene_nodeのシーンツリーフラグが0のときはシーンへの登録は行われるが、シーンツリーへの登録は行われない。
  // シーンノードを削除する
  void RemoveSceneNode(SceneNode* scene_node);  // シーンへの登録を解除する。

  // 現在のシーンツリーフラグでシーンツリーの状態を更新する
  void RefreshSceneNodeTreeFlag(SceneNode* scene_node);  // シーンへの登録は保持したままで、シーンツリーへの登録だけ変更したいときに使う。
                                                         // シーンへ登録したい、シーンへの登録を解除したいときはAddSceneNode, RemoveSceneNodeを使う。


private:
  State                       m_state;
  SceneInitializeProcess      m_scene_initialize_process_need;

  nw::gfx::SceneContext*      m_scene_context;
  nw::gfx::TransformNode*     m_scene_root;

  nw::gfx::ParticleContext*   m_particle_context;
  b8                          m_particle_enable_swap_buffer;
  s8                          m_padding[1];

  s16                         m_scene_calculate_group_index_borrow;  // 他のところで生成したものを借りて覚えておく
  Scene*                      m_scene_borrow;  // 他のところで生成したものを借りて覚えておく
};
 
}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSCENECALCULATEGROUP_H__
