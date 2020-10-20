#ifndef __GFL_G3DSCENESYSTEM_H__
#define __GFL_G3DSCENESYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneSystem.h
 *	@brief  3Dシーンシステム
 *	@author	Koji Kawada
 *	@date		2010.10.12
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
///	3Dシーンシステムクラス
//=====================================
class SceneSystem 
{
  GFL_FORBID_COPY_AND_ASSIGN(SceneSystem);


  // 下記クラスからしかアクセスしない
  friend class RenderSystem;
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
    // m_scene_initializerのIMaterialIdGenerator用の設定
    s32  max_materials;           // マテリアルの最大数
    bool is_fixed_size_memory;    // 生成時以外にもメモリを確保するかどうかのフラグ

    //-----------------------------------------------------------------------------
    /**
     *  @brief         コンストラクタ
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : max_materials         (128),
        is_fixed_size_memory  (true)
    {}
  };

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *
   *  @param[in]     allocator    アロケータ
   *  @param[in]     description  シーンシステムの設定内容
   *                              (NULLのときはデフォルト値。
   *                              呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   */
  //-----------------------------------------------------------------------------
  SceneSystem(gfl::heap::NwAllocator* allocator, const Description* description = NULL);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~SceneSystem();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンの初期化
   *
   *  @param[in]     scene_root                初期化するシーンのルートノード
   *  @param[in]     scene_initialize_process  初期化処理の内容
   */
  //-----------------------------------------------------------------------------
  void InitializeScene(SceneCalculateGroup* scene_calculate_group, const SceneInitializeProcess scene_itialize_process);

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ルートノードのトラバース
   *
   *  @param[in]     scene_root   対象のルートノード
   */
  //-----------------------------------------------------------------------------
  void TraverseScene(SceneCalculateGroup* scene_calculate_group);

public:

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // シーンの更新は
  // UpdateSceneを呼ぶか、
  // UpdateSceneBeforeCullingとUpdateSceneAfterCullingをこの順番で呼ぶか
  // のどちらかにして下さい。
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンの更新
   */
  //-----------------------------------------------------------------------------
  void UpdateScene(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンの更新（カリング前）
   */
  //-----------------------------------------------------------------------------
  void UpdateSceneBeforeCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンの更新（カリング後）
   */
  //-----------------------------------------------------------------------------
  void UpdateSceneAfterCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         パーティクルのノードに関するCalculateだけ行う(SceneUpdater部分だけ行って、ParticleSceneUpdater部分は行わない)
   */
  //-----------------------------------------------------------------------------
  void UpdateParticleNode(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag, Particle* particle);
  void UpdateParticleNodeBeforeCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag, Particle* particle);
  void UpdateParticleNodeAfterCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag, Particle* particle);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         パーティクルの更新(ParticleSceneUpdater部分)
   */
  //-----------------------------------------------------------------------------
  void UpdateParticle(SceneCalculateGroup* scene_calculate_group);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンアップデータの取得
   *
   *  @retval        nw::gfx::ISceneUpdater*    シーンアップデータ
   */
  //-----------------------------------------------------------------------------
  nw::gfx::ISceneUpdater*  GetSceneUpdater(void) const  {  return m_scene_updater;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         パーティクルシーンアップデータの取得
   *
   *  @retval        nw::gfx::ParticleSceneUpdater*    シーンアップデータ
   */
  //-----------------------------------------------------------------------------
  nw::gfx::ParticleSceneUpdater*  GetParticleSceneUpdater(void) const  {  return m_particle_scene_updater;  }

  //----------------------------------------------------------------------------
  /**
   *	@brief    UpdateSceneにかかった時間をログ出力
   */
  //-----------------------------------------------------------------------------
  void DebugUpdateSceneTickOutPut(void);

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         SkeletonUpdaterを得る
   *
   *  @retval        SkeletonUpdater
   */
  //-----------------------------------------------------------------------------
  nw::gfx::SkeletonUpdater*   GetSkeletonUpdater(void)  const { return m_skeleton_updater; }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         BillboardUpdaterを得る
   *
   *  @retval        BillboardUpdater
   */
  //-----------------------------------------------------------------------------
  nw::gfx::BillboardUpdater*  GetBillboardUpdater(void) const { return m_billboard_updater; }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         DepthSortModeを得る
   *
   *  @retval        DepthSortMode
   */
  //-----------------------------------------------------------------------------
  nw::gfx::ISceneUpdater::DepthSortMode  GetDepthSortMode(void) const
  {
    if(m_scene_updater){ return m_scene_updater->GetDepthSortMode(); }
    else{ GFL_ASSERT_MSG(0, "m_scene_updater=NULL\n"); return nw::gfx::ISceneUpdater::SORT_DEPTH_OF_ALL_MESH; }
  }

private:
  nw::gfx::SceneTraverser*     m_scene_traverser;
  nw::gfx::WorldMatrixUpdater* m_world_matrix_updater;  // 覚えておくだけ、破棄は他に任せる
  nw::gfx::SkeletonUpdater*    m_skeleton_updater;      // 覚えておくだけ、破棄は他に任せる
  nw::gfx::BillboardUpdater*   m_billboard_updater;     // 覚えておくだけ、破棄は他に任せる
  nw::gfx::SceneUpdater*       m_scene_updater;
  nw::gfx::SceneInitializer*   m_scene_initializer;

  nw::gfx::ParticleSceneUpdater*  m_particle_scene_updater;

private:
  void SetParticleEnableSwapBuffer(SceneCalculateGroup* scene_calculate_group, b32 particle_enable_swap_buffer)
  {
    scene_calculate_group->SetParticleEnableSwapBuffer(particle_enable_swap_buffer);
  }
};
 
}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSCENESYSTEM_H__
