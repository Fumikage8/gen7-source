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
#include <grp/g3d/gfl_G3dScene.h>
#include <grp/g3d/gfl_G3dSceneNode.h>

#include <grp/g3d/gfl_G3dSceneCalculateGroup.h>


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	3Dシーン計算グループクラス
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 */
//-----------------------------------------------------------------------------
SceneCalculateGroup::SceneCalculateGroup(gfl::heap::NwAllocator* allocator, const Description* description)
  : m_state(STATE_NONE),
    m_scene_initialize_process_need(SCENE_INITIALIZE_PROCESS_NONE),
    m_scene_context(NULL),
    m_scene_root(NULL),
    m_particle_context(NULL),
    m_particle_enable_swap_buffer(true),
    m_scene_calculate_group_index_borrow(SCENE_CALCULATE_GROUP_INDEX_NONE),
    m_scene_borrow(NULL)
{
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  m_scene_context = nw::gfx::SceneContext::Builder()
    .MaxSceneNodes        (l_description.max_scene_nodes)
    .MaxModels            (l_description.max_models)
    .MaxSkeletalModels    (l_description.max_skeletal_models)
    .MaxCameras           (l_description.max_cameras)
    .MaxLights            (l_description.max_lights)
    .MaxFragmentLights    (l_description.max_fragment_lights)
    .MaxVertexLights      (l_description.max_vertex_lights)
    .MaxHemiSphereLights  (l_description.max_hemisphere_lights)
    .MaxAmbientLights     (l_description.max_ambient_lights)
    .MaxFogs              (l_description.max_fogs)
    .MaxParticleSets      (l_description.max_particlesets)
    .MaxParticleEmitters  (l_description.max_particleemitters)
    .MaxParticleModels    (l_description.max_particlemodels)
    .MaxAnimatableNodes   (l_description.max_animatable_nodes)
    .IsFixedSizeMemory    (l_description.scene_context_is_fixed_size_memory)
    .Create               (allocator);

  m_scene_root = nw::gfx::TransformNode::DynamicBuilder()
    .MaxCallbacks(l_description.max_callbacks)
    .MaxChildren(l_description.max_children)
    .IsFixedSizeMemory(l_description.scene_root_is_fixed_size_memory)
    .Create(allocator);

  m_particle_context = nw::gfx::ParticleContext::Builder()
    .IsFixedSizeMemory(l_description.particle_context_is_fixed_size_memory)
    .MaxEmission(l_description.particle_context_max_emission)
    .MaxStreamLength(l_description.particle_context_max_stream_length)
    .UseDoubleBuffer(l_description.particle_context_use_double_buffer)
    .Create(allocator);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 */
//-----------------------------------------------------------------------------
SceneCalculateGroup::~SceneCalculateGroup()
{
  nw::gfx::SafeDestroy(m_particle_context);
  nw::gfx::SafeDestroy(m_scene_root);
  nw::gfx::SafeDestroy(m_scene_context);
}


//-----------------------------------------------------------------------------
/*
 *  @brief         privateメンバ関数
 */
//-----------------------------------------------------------------------------
// Sceneを設定する
void SceneCalculateGroup::SetScene(Scene* scene, s32 scene_calculate_group_index )
{
  m_scene_borrow = scene;
  m_scene_calculate_group_index_borrow = scene_calculate_group_index;
}

// シーンノードを追加する
void SceneCalculateGroup::AddSceneNode(SceneNode* scene_node)
{
  m_scene_borrow->AddNodeToCalc(scene_node, m_scene_calculate_group_index_borrow);
}
// シーンノードを削除する
void SceneCalculateGroup::RemoveSceneNode(SceneNode* scene_node)
{
  m_scene_borrow->RemoveNodeFromCalcAndAllDraw(scene_node);
}

// 現在のシーンツリーフラグでシーンツリーの状態を更新する
void SceneCalculateGroup::RefreshSceneNodeTreeFlag(SceneNode* scene_node)
{
  m_scene_borrow->RefreshNodeTreeFlag(scene_node);  // @note m_scene_calculate_group_index_borrowが一致しているか確認したほうがいいかも。
                                                    // @note Refresh関数1つではなく、フラグOn関数とフラグOff関数の2つを用意したほうがいいかも。
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl
