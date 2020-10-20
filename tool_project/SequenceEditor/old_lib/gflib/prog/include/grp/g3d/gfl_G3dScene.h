#ifndef __GFL_G3DSCENE_H__
#define __GFL_G3DSCENE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dScene.h
 *	@brief  3Dシーン
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// メモ
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

型一覧
                                             [外から見た型]   [内部で保持している型]
scene_calculate_group_index                  s32              s16
scene_calculate_group_num                    s32              s16
scene_draw_group_index                       s32              s16
scene_draw_group_num                         s32              s16

scene_calculate_group_model_index            s32              u16
scene_calculate_group_model_num              s32              u16
scene_calculate_group_light_index            s32              u16
scene_calculate_group_light_num              s32              u16
scene_calculate_group_camera_index           s32              u8
scene_calculate_group_camera_num             s32              u8
scene_calculate_group_fog_index              s32              u8
scene_calculate_group_fog_num                s32              u8
scene_calculate_group_particle_index         s32              u16
scene_calculate_group_particle_num           s32              u16

scene_own_model_index                        s32              u16
scene_own_model_num                          s32              u16
scene_own_light_index                        s32              u16
scene_own_light_num                          s32              u16
scene_own_camera_index                       s32              u8
scene_own_camera_num                         s32              u8
scene_own_fog_index                          s32              u8
scene_own_fog_num                            s32              u8
scene_own_scene_environment_setting_index    s32              u8
scene_own_scene_environment_setting_num      s32              u8

メモリ確保は[内部で保持している型]で行うが、関数の引数や関数内のローカル変数は[外から見た型]で行う。
つまり、最終的に保持するときは[内部で保持している型]になるが、それ以外は[外から見た型]で行う、ということ。

*/



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// include
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Base.h>

// gflib math
#include <math/gfl_MathAABB.h>

// gflib grp
#include <grp/gfl_GraphicsSystem.h>
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dSceneSystem.h>
#include <grp/g3d/gfl_G3dRenderSystem.h>
#include <grp/g3d/gfl_G3dShadowSystem.h>
#include <grp/g3d/gfl_G3dParticle.h>
#include <grp/g3d/gfl_G3dEdgeMapSystem.h>
#include <grp/g3d/gfl_G3dDepthOfFieldSystem.h>
#include <grp/g3d/gfl_G3dSceneEnvironmentSetting.h>
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dStereoCamera.h>
#include <grp/g3d/gfl_G3dLight.h>
#include <grp/g3d/gfl_G3dLightSet.h>
#include <grp/g3d/gfl_G3dFog.h>
#include <grp/g3d/gfl_G3dTexture.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dPostEffectSystem.h>
#include <grp/g3d/gfl_G3dBloomFilter.h>

#if GFL_GRP_G3D_H3D_USE
#include <grp/g3d/gfl_G3dH3dModel.h>
#endif  // GFL_GRP_G3D_H3D_USE



namespace gfl {
namespace grp {
namespace g3d {



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// extern関数宣言
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/**
 *  @brief         所有者を設定する
 *                 所有者のIDを設定する
 *                 class Objectのフレンド関数
 *
 *  @param[in]     object       オブジェクト 
 *  @param[in]     a_owner      オブジェクトの所有者
 *  @param[in]     a_owner_id   所有者のID
 */
//-----------------------------------------------------------------------------
extern void Object_SetOwnerAndOwnerId(Object* object, ObjectOwner a_owner, s32 a_owner_id);

//-----------------------------------------------------------------------------
/**
 *  @brief         ユーザが任意で付けられるIDを設定する(強制的)
 *                 ユーザが管理していないオブジェクトでも、ユーザが任意で付けられるIDを設定できる。
 *
 *  @param[in]     object     オブジェクト 
 *  @param[in]     a_user_id  ユーザが任意で付けられるID
 */
//-----------------------------------------------------------------------------
extern void Object_SetUserIdForcedly(Object* object, s32 a_user_id);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// クラス宣言
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------
///	3Dシーンクラス
//=====================================
class Scene : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(Scene);


  friend class SceneCalculateGroup;

#if GFL_GRP_G3D_H3D_USE
  friend class H3dModel;
  friend class H3dEnvironment;
#endif  // GFL_GRP_G3D_H3D_USE


  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // 定数宣言
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
public:
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // シーン計算グループ
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //-------------------------------------
  //
  /// シーン計算グループを制御するビットフラグ
  //
  //=====================================
  enum
  {
    //////// なし ////////
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_NONE    = 0,  // 何もしない。


    //////// 単独 ////////
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE                 = 0x1 << 0,  // CalculateにおいてScale,Rotate,Translateを実行し
                                                                                             // ワールドマトリクスを更新する。
                                                                                
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL                 = 0x1 << 1,  // Calculateにおいてスケルタルモデル、ビルボードモデルの骨の
                                                                                             // ワールドマトリクスを更新する。
                                                                                             // 現在の姿勢の値を骨のワールドマトリクスに反映するので
                                                                                             // 見た目が変わります。
                                                                                
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           = 0x1 << 2,  // Calculateにおいてアニメーションフレームを進める。
                                                                                             // アニメーションのフレームの数値を進めるだけです。
                                                                                             // フレームの数値がnだったものがn+mになるだけです。
                                                                                             // 骨の姿勢が動いたり色が変わったりはしません。
                                                                                
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE     = 0x1 << 3,  // Calculateにおいてビジビリティアニメーションを更新する。
                                                                                             // 描画フラグが更新されるので、表示のON/OFFが変わります。
                                                                                
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     = 0x1 << 4,  // Calculateにおいて
                                                                                             // スケルタルアニメーション(ワールドマトリクスは変わらない)と
                                                                                             // マテリアルアニメーションを更新する。
                                                                                             // 骨がアニメーション結果の姿勢になりますが、
                                                                                             // 骨のワールドマトリクスは更新されないので、
                                                                                             // 見た目何も変わりません。
                                                                                             // マテリアルがアニメーション結果の色に変わります。
                                                                                
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE                       = 0x1 << 5,  // Calculateにおいてパーティクルを更新する。

    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE = 0x1 << 6,  // Calculateにおいてg3dオブジェクト固有の更新処理を行う。
                                                                                             // カリングの有無に関わらず必ず必要な更新が行われます。
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING = 0x1 << 7,  // Calculateにおいてg3dオブジェクト固有の更新処理を行う。
                                                                                             // カリングされて見えないときは、
                                                                                             // しなくても問題が起きないない更新が行われます。


    //////// 複合 ////////

    // デフォルト用の通常設定。Calculateにおいて全ての更新を行う。
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_DEFAULT                        =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE                 \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL                 \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE                       \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING ,

    // Calculateにおいてアニメーションを評価する。
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM                  =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     ,

    // Calculateにおいてg3dオブジェクト固有の更新処理を行う。
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT              =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING ,

    // Calculateにおいて画面内に表示されているか否かの判定を行うのに必要となるものを更新する。
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_BEFORE_CULLING          =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE                 ,

    // CalculateにおいてBEFORE_CULLINGで行っていないものを更新する。
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_AFTER_CULLING           =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL                 \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE                       ,

    // Scale,Rotate,Translateは完璧に更新する
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SRT_PERFECTLY           =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE                 ,

    // スケルタルアニメーションを含むモデルはSRT抜きで正常に更新する
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_EXACTLY        =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL                 \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     ,

    // マテリアルアニメーションを含むモデルはSRT抜きで正常に更新する
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_MATERIAL_EXACTLY        =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     ,

    // ビジビリティアニメーションを含むモデルはSRT抜きで正常に更新する
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_VISIBILITY_EXACTLY      =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE     ,

    // カメラアニメーションを含むカメラはSRT抜きで正常に更新する
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_CAMERA_EXACTLY          =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     ,

    // ライトアニメーションを含むライトはSRT抜きで正常に更新する
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_LIGHT_EXACTLY           =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                           \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING     ,

    // パーティクルはSRT抜きで正常に更新する
        // @note パーティクルでマテリアルアニメーションを行うなら、そのフラグを立てる必要がある。
        // 今はg3dのパーティクルがそれに対応していないので立てていない。
    SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE_EXACTLY        =   SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING \
                                                                           | SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE                       ,
  };
/*
  //////// Calculateの実行される順番 ////////

  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE 
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE 
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE             
  
      **** カリングはここに入る ****

  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING 
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING 
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL             
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM                       
  SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE                   
*/



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // シーン描画グループ
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //-------------------------------------
  //
  /// シーン描画グループを制御するビットフラグ
  //
  //=====================================
  enum
  {
    // なし
    SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_NONE      = 0,           // 何もしない。

    // 単独
    SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE   = 0x1  <<  0,  // SubmitViewにおいてレンダーキューに積む。
    SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_SORT      = 0x1  <<  1,  // SubmitViewにおいてレンダーキューをソートする。
    
    // 複合
    SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_DEFAULT   =   SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE \
                                                 | SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_SORT
                                                     // デフォルト用の通常設定。SubmitViewにおいてレンダーキューに積みソートする。
  };

  //-------------------------------------
  //
  /// シーン描画グループのレンダーキューをマージする方法
  //
  //=====================================
  // (A, B, method)  「レンダーキューB」を「レンダーキューA」に「方法method」でマージする。
  // マージ後、【「新A」】は【「旧A」と「B」を合わせたもの】になり、「B」は「B」のまま変更されない。
  enum SceneDrawGroupMergeMethod
  {
    SCENE_DRAW_GROUP_MERGE_METHOD_HEAD,           // BをAの前に置く。(ソートはしないので、A,Bはソートしておくこと)
    SCENE_DRAW_GROUP_MERGE_METHOD_TAIL,           // BをAの後に置く。(ソートはしないので、A,Bはソートしておくこと)
    SCENE_DRAW_GROUP_MERGE_METHOD_SORTED_MIX,     // ソート済みのBをソート済みAに混ぜる。(新Aはソートされたものになる)
                                                  // (A,Bがソートされていない場合、動作は保障されない)
    SCENE_DRAW_GROUP_MERGE_METHOD_MIX_NEED_SORT   // BをAに混ぜ、ソートする。(ソートし直すのでA,Bはソートされていなくてもよい)
  };



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // 構造体宣言
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
public:
  //-------------------------------------
  //
  /// シーンの設定内容
  //
  //=====================================
  struct Description  // maxと付いているのは管理する最大数、付いていないのはその個数生成
  {
    u32                                  own_model_num_max;
    u32                                  own_light_num_max;
    u32                                  own_camera_num_max;
    u32                                  own_fog_num_max;
    u32                                  own_scene_environment_setting_num_max;
    u32                                  user_model_num_max;
    u32                                  user_light_num_max;
    u32                                  user_camera_num_max;
    u32                                  user_fog_num_max;
    u32                                  user_particle_num_max;
    SceneSystem::Description*            scene_system_description;
    RenderSystem::Description*           render_system_description;
    u32                                  scene_calculate_group_num;
    SceneCalculateGroup::Description*    scene_calculate_group_description_array;  // scene_calculate_group_num分の配列もしくはNULLを渡して下さい。
        // 例
        // gfl::grp::g3d::Scene::Description                scene_description;
        // gfl::grp::g3d::SceneCalculateGroup::Description  scene_calculate_group_description_array[3];
        // scene_description.scene_calculate_group_num               = 3;
        // scene_description.scene_calculate_group_description_array = scene_calculate_group_description_array;
    u32                                  scene_draw_group_num;
    SceneDrawGroup::Description*         scene_draw_group_description_array;  // scene_draw_group_num分の配列もしくはNULLを渡して下さい。

    // コンストラクタ
    Description(void)
      : own_model_num_max                          (256),
        own_light_num_max                          (8),
        own_camera_num_max                         (8),
        own_fog_num_max                            (8),
        own_scene_environment_setting_num_max      (4),
        user_model_num_max                         (256),
        user_light_num_max                         (8),
        user_camera_num_max                        (8),
        user_fog_num_max                           (8),
        user_particle_num_max                      (8),
        scene_system_description                   (NULL),
        render_system_description                  (NULL),
        scene_calculate_group_num                  (1),
        scene_calculate_group_description_array    (NULL),
        scene_draw_group_num                       (2),
        scene_draw_group_description_array         (NULL)
    {}
  };



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // 公開static関数
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
public:
  
  //-----------------------------------------------------------------------------
  /**
   * @brief カリング判定スケール
   * @param scale     スケール
   */
  //-----------------------------------------------------------------------------
  static void SetViewCullBoxScale( f32 scale );
  //-----------------------------------------------------------------------------
  /**
   * @brief モデルの視野カリング
   * @param r_model     対象モデル
   * @param r_camera    対象カメラ
   * @return            視野内か
   */
  //-----------------------------------------------------------------------------
  static bool ViewCullingModel( const Model &r_model, const Camera &r_camera );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief AABBの視野カリング
   * @param aabb        対象ＡＡＢＢ
   * @param r_camera    対象カメラ
   * @return            視野内か
   */
  //-----------------------------------------------------------------------------
  static bool ViewCullingAABB( const gfl::math::AABB &aabb, const Camera &r_camera, const nn::math::MTX34 &worldMat );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief スクリーン座標系でのＡＡＢＢ取得
   * @param p_dest_aabb     出力先
   * @param r_src_aabb      対象ＡＡＢＢ
   * @param r_camera        対象カメラ
   */
  //-----------------------------------------------------------------------------
  static void GetScreanCoordinatesAABB( gfl::math::AABB *p_dest_aabb, const gfl::math::AABB &r_src_aabb, const Camera &r_camera, const nn::math::MTX34 &worldMat );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief スクリーン座標系でのモデルＡＡＢＢ取得
   * @param p_dest_aabb       出力先
   * @param r_model           対象モデル
   * @param r_camera          対象カメラ
   *
   * @retval  true  正常なAABB
   * @retval  false 不正なAABB
   */
  //-----------------------------------------------------------------------------
  static bool GetScreanCoordinatesModelAABB( gfl::math::AABB *p_dest_aabb, const Model &r_model, const Camera &r_camera );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief スクリーン座標系でのモデルＡＡＢＢ取得
   * @param p_dest_aabb       出力先
   * @param r_h3d_model           対象モデル
   * @param r_camera          対象カメラ
   *
   * @retval  true  正常なAABB
   * @retval  false 不正なAABB
   */
  //-----------------------------------------------------------------------------
  static bool GetScreanCoordinatesModelAABB( gfl::math::AABB *p_dest_aabb, const H3dModel &r_h3d_model, const Camera &r_camera );


#if GFL_GRP_G3D_H3D_USE
public:
  //-----------------------------------------------------------------------------
  /**
   * @brief モデルの視野カリング
   * @param r_model     対象モデル
   * @param r_camera    対象カメラ
   * @return            視野内か
   */
  //-----------------------------------------------------------------------------
  static bool ViewCullingH3dModel( const H3dModel &r_model, const Camera &r_camera );
#endif  // GFL_GRP_G3D_H3D_USE


  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // 公開関数
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // コンストラクタ / デストラクタ
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  Scene(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createを呼びます。
   *
   *  @param[in]     device_allocator    デバイスアロケータ
   *  @param[in]     description  シーンの設定内容
   *                              (NULLのときはデフォルト値。
   *                              呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   *  @param[in]     heap_allocator      ヒープアロケータ(NULLのときはdevice_allocatorを使用します)
   */
  //-----------------------------------------------------------------------------
  Scene( gfl::heap::NwAllocator* device_allocator, const Description* description = NULL, gfl::heap::NwAllocator* heap_allocator = NULL );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~Scene(); 



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // デバッグ機能
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  void DebugTickSetEnableFlag(b32 flag);        // trueのとき計測あり、falseのとき計測なし
  void DebugTickSetDumpAndResetFlag(b32 flag);  // trueのときダンプ＆リセットあり、falseのときなし
  b32  DebugTickGetDumpAndResetFlag(void);
  
  //----------------------------------------------------------------------------
  /**
   *	@brief    UpdateSceneにかかった時間をログ出力
   */
  //-----------------------------------------------------------------------------
  void DebugUpdateSceneTickOutPut(void);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // 生成 / 破棄
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     device_allocator    デバイスアロケータ
   *  @param[in]     description  シーンの設定内容
   *                              (NULLのときはデフォルト値。
   *                              呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   *  @param[in]     heap_allocator      ヒープアロケータ(NULLのときはdevice_allocatorを使用します)
   */
  //-----------------------------------------------------------------------------
  void Create(gfl::heap::NwAllocator* device_allocator, const Description* description = NULL, gfl::heap::NwAllocator* heap_allocator = NULL );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);



 
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // Initialize
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         初期化処理
   *                 (終了処理はなし)
   */
  //-----------------------------------------------------------------------------
  // 外部から呼ばれる初期化処理
  void Initialize(const SceneInitializeProcess scene_initialize_process = SCENE_INITIALIZE_PROCESS_ALL); 
  void Initialize(const s32 scene_calculate_group_index, const SceneInitializeProcess scene_initialize_process = SCENE_INITIALIZE_PROCESS_ALL);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // 計算処理
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         計算処理
   */
  //-----------------------------------------------------------------------------
  // 計算処理 
  virtual void Calculate(void);  // 全て(パーティクルを含む)の計算処理を行う
  virtual void Calculate(s32 scene_calculate_group_index);  // scene_calculate_group_indexにある全て(パーティクルを含む)の計算処理を行う

  // パーティクルのノードに関するCalculateだけ行う(SceneUpdater部分だけ行って、ParticleSceneUpdater部分は行わない)
  void CalculateParticleNode(Particle* particle = NULL);  // particleがNULLのときSceneに登録されている全てのパーティクルに対して行う  // シーンツリーから外れていても(Particleクラスにユーザが指定して外れている場合も、カリングで外れている場合も)処理を行います。
  void CalculateParticleNode(s32 scene_calculate_group_index, Particle* particle = NULL);  // particleがNULLのときscene_calculate_group_indexに登録されている全てのパーティクルに対して行う  // シーンツリーから外れていても(Particleクラスにユーザが指定して外れている場合も、カリングで外れている場合も)処理を行います。

  // パーティクルだけフレームを進める
  void RepeatCalculateParticle(s32 repeat_count, Particle* particle = NULL);  // パーティクルだけ計算処理を行う  // particleがNULLのときSceneに登録されている全てのパーティクルに対して行う  // シーンツリーから外れていても(Particleクラスにユーザが指定して外れている場合も、カリングで外れている場合も)処理を行います。
  void RepeatCalculateParticle(s32 repeat_count, s32 scene_calculate_group_index, Particle* particle = NULL);  // パーティクルだけ計算処理を行う  // particleがNULLのときscene_calculate_group_indexに登録されている全てのパーティクルに対して行う  // シーンツリーから外れていても(Particleクラスにユーザが指定して外れている場合も、カリングで外れている場合も)処理を行います。

  // カリング
  void CalculateCulling(Camera* camera, bool update_camera_matrix = true);
  void CalculateCulling(s32 scene_calculate_group_index, Camera* camera, bool update_camera_matrix = true);
  void ResetCalculateCullingFlag(void);
  void ResetCalculateCullingFlag(s32 scene_calculate_group_index);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループを制御するビットフラグを設定する
   *
   *  @param[in]  scene_calculate_group_enable_bit_flag_array  シーン計算グループを制御するビットフラグの配列(シーン計算グループ全ての分)
   *
   *      bit32 bit_flag_array[calc_group_num] =
   *      {
   *         gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_NONE,                       // calc_group_0
   *         gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE,      // calc_group_1
   *           gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE \
   *         | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM           \
   *         | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM,            // calc_group_2
   *         gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_DEFAULT                     // calc_group_3
   *      };
   */
  //-----------------------------------------------------------------------------
  void SetSceneCalculateGroupEnableBitFlagArray(bit32* scene_calculate_group_enable_bit_flag_array);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループを制御するビットフラグを設定する
   *
   *  @param[in]  scene_calculate_group_index            シーン計算グループ
   *  @param[in]  scene_calculate_group_enable_bit_flag  シーン計算グループを制御するビットフラグ
   *                  例
   *                  scene_calculate_group_enable_bit_flag = \
   *                        gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE \
   *                      | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM           \
   *                      | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM
   */
  //-----------------------------------------------------------------------------
  void SetSceneCalculateGroupEnableBitFlag(s32 scene_calculate_group_index, bit32 scene_calculate_group_enable_bit_flag);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループを制御するビットフラグを取得する
   *
   *  @param[in]  scene_calculate_group_index            シーン計算グループ
   *
   *  @retval     シーン計算グループを制御するビットフラグ
   */
  //-----------------------------------------------------------------------------
  bit32 GetSceneCalculateGroupEnableBitFlag(s32 scene_calculate_group_index) const;



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // SubmitView
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////    ↓SubmitViewを一度に行うための関数↓    //////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //-----------------------------------------------------------------------------
  /**
   *  @brief         フォグの更新を行う
   *
   *  @param[in]    camera    ここで渡したカメラからの距離でフォグの濃さを調整します
   *
   *  【注意事項】
   *      (1) フォグクラスの基底クラスであるシーンノードの更新に関して
   *          フォグクラスの基底クラスであるシーンノードの更新はCalculateで行われるので、この関数を呼び出す前にCalculateは済ませておいて下さい。
   *      (2) カメラに関して
   *          フォグの更新に使うカメラのカメラマトリクスの更新は、この関数を呼び出す前に済ませておいて下さい。(カメラのマトリクスの更新の前に、Calculateを済ませておく必要があります。)
   */
  //-----------------------------------------------------------------------------
  void UpdateFog(Camera* camera);  // 全てのシーン計算グループのフォグの更新を行う
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループで使用するカメラを全て設定する
   *
   *  @param[in]    scene_draw_group_camera_array    シーン描画グループの配列通りにカメラを設定した配列(シーン描画グループ全ての分)
   *                                                 
   *                                                 例
   *                                                 Camera* camera_a;  // for draw_group_0, draw_group_1
   *                                                 Camera* camera_b;  // for draw_group_2
   *
   *                                                 Camera* camera_array[draw_group_num] =
   *                                                 {
   *                                                   camera_a,  // draw_group_0
   *                                                   camera_a,  // draw_group_1
   *                                                   camera_b   // draw_group_2
   *                                                 };
   *
   *  【注意事項】
   *      (1) ここで設定したカメラが使用されるのは・・・
   *          ここで設定したカメラが使用されるのは、SubmitViewAllでのみです。
   *          設定しておいたカメラが破棄されてもSubmitViewAllを呼ばなければ不具合が生じることはありませんが、
   *          設定しておいたカメラを破棄するときは安全のためNULLを設定するようにして下さい。
   */
  //-----------------------------------------------------------------------------
  void SetSubmitViewCameraArray(const Camera** scene_draw_group_camera_array);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループで使用するカメラを設定する
   *
   *  @param[in]    scene_draw_group_index    scene_draw_group_cameraを設定するシーン描画グループ
   *  @param[in]    scene_draw_group_camera   scene_draw_group_indexに設定するカメラ
   *
   *  【注意事項】
   *      (1) ここで設定したカメラが使用されるのは・・・
   *          ここで設定したカメラが使用されるのは、SubmitViewAllでのみです。
   *          設定しておいたカメラが破棄されてもSubmitViewAllを呼ばなければ不具合が生じることはありませんが、
   *          設定しておいたカメラを破棄するときは安全のためNULLを設定するようにして下さい。
   */
  //-----------------------------------------------------------------------------
  void SetSubmitViewCamera(s32 scene_draw_group_index, const Camera* scene_draw_group_camera);
 
  // シーン描画グループで使用するカメラを取得する
  const Camera* GetSubmitViewCamera(s32 scene_draw_group_index) const;
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループを制御するビットフラグを設定する
   *
   *  @param[in]  scene_draw_group_enable_bit_flag_array  シーン描画グループを制御するビットフラグの配列(シーン描画グループ全ての分)
   *
   *      例
   *      bit32 bit_flag_array[draw_group_num] =
   *      {
   *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_NONE,                                                                   // draw_group_0
   *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE,                                                                // draw_group_1
   *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE | gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_SORT,  // draw_group_2
   *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_DEFAULT                                                                 // draw_group_3
   *      };
   *
   *  【注意事項】
   *      (1) ここで設定したフラグが使用されるのは・・・
   *          ここで設定したフラグが使用されるのは、SubmitViewAllでのみです。
   *              **** 通常のSubmitViewのにも適用してもよいがどうしよう・・・ ****
   */
  //-----------------------------------------------------------------------------
  void SetSceneDrawGroupEnableBitFlagArray(bit32* scene_draw_group_enable_bit_flag_array);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループを制御するビットフラグを設定する
   *
   *  @param[in]  scene_draw_group_index            シーン描画グループ
   *  @param[in]  scene_draw_group_enable_bit_flag  シーン描画グループを制御するビットフラグ
   *                  例
   *                  scene_draw_group_enable_bit_flag = \
   *                      gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE | gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_SORT;
   *
   *  【注意事項】
   *      (1) ここで設定したフラグが使用されるのは・・・
   *          ここで設定したフラグが使用されるのは、SubmitViewAllでのみです。
   *              **** 通常のSubmitViewのにも適用してもよいがどうしよう・・・ ****
   */
  //-----------------------------------------------------------------------------
  void SetSceneDrawGroupEnableBitFlag(s32 scene_draw_group_index, bit32 scene_draw_group_enable_bit_flag);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループを制御するビットフラグを取得する
   *
   *  @param[in]  scene_draw_group_index            シーン描画グループ
   *
   *  @retval     シーン描画グループを制御するビットフラグ
   *
   *  【注意事項】
   *      (1) ここで取得したフラグが使用されているのは・・・
   *          ここで取得したフラグが使用されているのは、SubmitViewAllでのみです。
   *              **** 通常のSubmitViewのにも適用してもよいがどうしよう・・・ ****
   */
  //-----------------------------------------------------------------------------
  bit32 GetSceneDrawGroupEnableBitFlag(s32 scene_draw_group_index) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループ全てのSubmitViewを一度に行う
   *
   *  【注意事項】
   *      (1) カメラに関して
   *          この関数を呼び出す前に、SetSubmitViewCameraArrayやSetSubmitViewCameraで、シーン描画グループ全てに対して使用するカメラを設定しておいて下さい。
   *          上記にて設定しておいたカメラのカメラマトリクスの更新は、この関数を呼び出す前に済ませておいて下さい。(カメラのマトリクスの更新の前に、Calculateを済ませておく必要があります。)
   *      (2) フォグに関して
   *          この関数を呼び出す前に、UpdateFogでフォグの更新を済ませておいて下さい。
   */
  //-----------------------------------------------------------------------------
  void SubmitViewAll(void);
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループのレンダーキューをマージする
   * 
   *  @param[in]  scene_draw_group_index_A  シーン描画グループ。Bが追加されるるので変更されます。
   *  @param[in]  scene_draw_group_index_B  シーン描画グループ。変更されません。
   *  @param[in]  method                    マージ方法
   *
   *  【注意事項】
   *      (1) AとBを同じにしないで下さい。
   *      (2)「レンダーキューB」を「レンダーキューA」に「方法method」でマージする。
   *          マージ後、【「新A」】は【「旧A」と「B」を合わせたもの】になり、「B」は「B」のまま変更されない。
   */
  //-----------------------------------------------------------------------------
  void MergeRenderQueue(s32 scene_draw_group_index_A, const s32 scene_draw_group_index_B, const SceneDrawGroupMergeMethod method);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////    ↑SubmitViewを一度に行うための関数↑    //////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         SubmitView
   *
   *  @param[in]     camera                  カメラ
   *  @param[in]     update_camera_matrix    cameraのマトリクスを更新する場合true
   *  @param[in]     updata_for              フォグを更新する場合true
   *  @param[in]     scene_draw_group_index  シーン描画グループのインデックス
   * 
   *  cameraに gfl::grp::g3d::Camera* camera = GFL_NEW() StereoCamera; というふうに生成したカメラを渡した場合、
   *  Camera* camera を引数に持つこちらのSubmitViewが呼ばれるため、
   *  update_camera_matrixがtrueのときベースカメラだけ更新されて、左目用カメラ、右目用カメラが更新されない現象が起きます。
   *  ↑対処済み、update_camera_matrixがtrueのときベースカメラも左目用カメラも右目用カメラも更新し、falseのとき3つとも更新しない。
   */
  //-----------------------------------------------------------------------------
  void SubmitView(
      Camera* camera,
      bool update_camera_matrix = true,
      bool update_fog = true,
      s32 scene_draw_group_index = 0);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // Draw
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         描画処理
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     display_type           表示面
   *  @param[in]     camera                 このシーンを描画するカメラ
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void Draw(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::DisplayType         display_type,
    Camera*                       camera,
    bool                          command_cache_dump = false
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ステレオ描画処理
   *                 上左目画面(DISPLAY_UPPER)、上右目画面(DISPLAY_UPPER_RIGHT)に描画する
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     camera                 このシーンを描画するステレオカメラ
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void StereoDraw(
    gfl::grp::GraphicsSystem*    graphics_system,
    StereoCamera*                camera,
    bool                         command_cache_dump = false
  );

  // Draw 
  void Draw(
      gfl::grp::GraphicsSystem* graphics_system,
      Camera* camera,
      gfl::grp::RenderTarget* render_target,
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false );

/*
  h3dでサポートしきれないのでコメントアウト。
  // コマンドセーブだけしてDrawしない
  void CommandSaveNotDraw(
      gfl::grp::GraphicsSystem* graphics_system,
      gfl::grp::RenderTarget* render_target,
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false );

  // コマンドセーブしてあるものを使ってDraw
  void ReuseCommandSaveDraw(
      gfl::grp::GraphicsSystem* graphics_system,
      Camera* camera,
      gfl::grp::RenderTarget* render_target);
*/

  // コマンドセーブしないでStereoDraw
  void StereoDraw(
      GraphicsSystem* graphics_system,
      Camera* left_camera, 
      gfl::grp::RenderTarget* left_render_target,
      Camera* right_camera,
      gfl::grp::RenderTarget* right_render_target,
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false );
  void StereoDraw(
      GraphicsSystem* graphics_system,
      StereoCamera* camera,
      gfl::grp::RenderTarget* left_render_target,
      gfl::grp::RenderTarget* right_render_target,
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false );

  // コマンドセーブしてStereoDraw
private:
  void CommandSaveStereoDraw(
      gfl::grp::GraphicsSystem* graphics_system, 
      Camera* left_camera, 
      gfl::grp::RenderTarget* left_render_target,
      Camera* right_camera,
      gfl::grp::RenderTarget* right_render_target,
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      s32 scene_draw_group_index,
      Camera* base_camera,
      bool command_cache_dump = false
#else
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false
#endif
      );
public:
  void CommandSaveStereoDraw(
      gfl::grp::GraphicsSystem* graphics_system, 
      StereoCamera* camera,
      gfl::grp::RenderTarget* left_render_target,
      gfl::grp::RenderTarget* right_render_target,
      s32 scene_draw_group_index = 0,
      bool command_cache_dump = false );

/*
  h3dでサポートしきれないのでコメントアウト。
  // コマンドセーブしてあるものを使ってStereoDraw
  void ReuseCommandSaveStereoDraw(
      gfl::grp::GraphicsSystem* graphics_system, 
      Camera* left_camera, 
      gfl::grp::RenderTarget* left_render_target,
      Camera* right_camera,
      gfl::grp::RenderTarget* right_render_target );
  void ReuseCommandSaveStereoDraw(
      gfl::grp::GraphicsSystem* graphics_system, 
      StereoCamera* camera,
      gfl::grp::RenderTarget* left_render_target,
      gfl::grp::RenderTarget* right_render_target );
*/

#if GFL_GRP_G3D_H3D_USE
private:
  void drawH3dModel(
      gfl::grp::GraphicsSystem* graphics_system, 
      Camera* camera,
      gfl::grp::RenderTarget* render_target,
      s32 scene_draw_group_index
  );
  void commandSaveStereoDrawH3dModel(
      gfl::grp::GraphicsSystem* graphics_system, 
      Camera* left_camera,
      gfl::grp::RenderTarget* left_render_target,
      Camera* right_camera,
      gfl::grp::RenderTarget* right_render_target,
      s32 scene_draw_group_index
  );
#endif



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // シーンノードの 追加 / 削除
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // 
  // お勧め関数
  // 
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // AddNodeToCalcを呼んだ後、AddNodeToDrawを呼ぶ関数 
  b32 AddNodeToCalcAndDraw(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index);
  // AddNodeToCalcを呼んだ後、AddNodeToExDrawを呼ぶ関数 
  b32 AddNodeToCalcAndExDraw(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index);
  // AddNodeToCalcを呼んだ後、AddNodeToAllDrawを呼ぶ関数 
  b32 AddNodeToCalcAndAllDraw(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index);

  // scene_nodeをscene_calculate_group_indexに入れる
  // 既に他のシーン計算グループに入っている場合は移動する。
  // シーン描画グループに対しては何もしない。
  // 新規でも移動でもscene_nodeをscene_calculate_group_indexに入れられなかったらfalseを返す。
  // 移動の際は、元々入っていたシーン計算グループから取り除いてからscene_calculate_group_indexに入れるので、
  // 取り除くのはうまくいきscene_calculate_group_indexに入れるのだけ失敗した場合は、
  // どのシーン計算グループにも入っていない状態になってfalseを返す。このようなどのシーン計算グループにも入っていない状態になったときは、
  // 全てのシーン描画グループを解除する(シーン計算グループに入っていないときはシーン描画グループには登録されない、というルールを守るため)。
  b32 AddNodeToCalc(SceneNode* scene_node, s32 scene_calculate_group_index);

  // scene_nodeをscene_draw_indexに入れる
  // 既に他のシーン描画グループに入っている場合は、scene_draw_group_indexにも入れるので複数に登録されることになる。
  // シーン計算グループに対しては何もしない。
  // scene_nodeをscene_draw_indexに入れられなかったらfalseを返す。
  // 元々入っていたシーン描画グループに対しては何もしない。
  // シーン計算グループに入っていないときは、シーン描画グループには登録されないのでfalseを返す。
  b32 AddNodeToDraw(SceneNode* scene_node, s32 scene_draw_group_index);
  // scene_nodeを排他的にscene_draw_indexに入れる
  // 既に他のシーン描画グループに入っている場合は、他の登録を全て解除しscene_draw_group_indexにだけ入れる。
  // シーン計算グループに対しては何もしない。
  // scene_nodeをscene_draw_indexに入れられなかったらfalseを返す。
  // 元々入っていたシーン描画グループから取り除いてからscene_draw_group_indexに入れるので、
  // 取り除くのはうまくいきscene_draw_group_indexに入れるのだけ失敗した場合は、
  // どのシーン描画グループにも入っていない状態になってfalseを返す。
  // シーン計算グループに入っていないときは、シーン描画グループには登録されないのでfalseを返す。
  b32 AddNodeToExDraw(SceneNode* scene_node, s32 scene_draw_group_index);
  // scene_nodeを全てのシーン描画グループに入れる
  // シーン計算グループに対しては何もしない。
  // 1個でも入れられないシーン描画グループがあったときはfalseを返すが、入れられるところには入れた状態で戻る。
  // シーン計算グループに入っていないときは、シーン描画グループには登録されないのでfalseを返す。
  b32 AddNodeToAllDraw(SceneNode* scene_node);
  
  // scene_nodeをシーン計算グループと全てのシーン描画グループから解除する
  void RemoveNodeFromCalcAndAllDraw(SceneNode* scene_node);
  // scene_nodeをシーン計算グループと全てのシーン描画グループから解除する  // RemoveNodeFromCalcAndAllDrawと中身は同じ。
  void RemoveNodeFromCalc(SceneNode* scene_node);                          // ModelNodePropertyでなければシーン描画グループには入っていないので
                                                                           // この名前の関数もつくっておくことにした。
  // scene_nodeをscene_draw_group_indexから解除する
  // scene_draw_group_index以外のシーン描画グループにも入っている場合は、そのシーン描画グループには残る。
  // シーン計算グループに対しては何もしない。
  void RemoveNodeFromDraw(SceneNode* scene_node, s32 scene_draw_group_index);
  // scene_nodeを全てのシーン描画グループから解除する
  // シーン計算グループに対しては何もしない。
  void RemoveNodeFromAllDraw(SceneNode* scene_node);

  // scene_calculate_group_indexに登録されている全てのシーンノードを、scene_calculate_group_indexと全てのシーン描画グループから解除する
  void RemoveAllNodeFromCalcAndAllDraw(s32 scene_calculate_group_index);
  // scene_draw_group_indexに登録されている全てのシーンノードを、scene_draw_group_indexから解除する
  // scene_draw_group_indexに登録されている各シーンノードがscene_draw_group_index以外のシーン描画グループにも入っている場合は、
  // そのシーン描画グループには残る。
  // シーン計算グループに対しては何もしない。
  void RemoveAllNodeFromDraw(s32 scene_draw_group_index);
  // 全てのシーン計算グループと全てのシーン描画グループから全てのシーンノードを解除する。
  // 即ち、シーンに登録されている全てのシーンノードをシーンから解除する
  void RemoveAllNodeFromAllCalcAndAllDraw(void);

#if GFL_GRP_G3D_H3D_USE
  // scene_nodeをシーン計算グループscene_calculate_group_indexに追加する
  b32  AddH3dModelToCalc(H3dModel* scene_node, s32 scene_calculate_group_index);
  
  // scene_nodeをシーン描画グループscene_draw_group_indexに追加する
  b32  AddH3dModelToDraw(H3dModel* scene_node, s32 scene_draw_group_index);

  //   scene_nodeを登録してあるシーン計算グループから取り除く
  // & scene_nodeを登録してある全てのシーン描画グループから取り除く
  void RemoveH3dModelFromCalcAndAllDraw(H3dModel* scene_node);

  // scene_nodeをシーン描画グループscene_draw_group_indexから取り除く
  void RemoveH3dModelFromDraw(H3dModel* scene_node, s32 scene_draw_group_index);
  
  //   シーン計算グループscene_calculate_group_indexに登録されている全てのH3dModelを取り除く
  // & 取り除いたH3dModelは登録してあった全てのシーン描画グループからも取り除かれる
  void RemoveAllH3dModelFromCalcAndAllDraw(s32 scene_calculate_group_index);

  //   全てのH3dModelを全てのシーン計算グループから取り除く
  // & 全てのH3dModelを全てのシーン描画グループから取り除く
  void RemoveAllH3dModelFromAllCalcAndAllDraw(void);
#endif  // GFL_GRP_G3D_H3D_USE


  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // 
  // 従来通りの関数
  // 
  /////////////////////////////////////////////
  /////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループにシーンノードを登録する。
   *
   *                 [シーン計算グループ]
   *                 シーン計算グループに未登録の場合は、シーン計算グループscene_calculate_group_index番に登録される。
   *                 シーン計算グループに登録済みの場合は、シーン計算グループscene_calculate_group_index番に変更される。
   *                 シーン計算グループは1つにしか登録できない。
   *                 
   *                 [シーン描画グループ]
   *                 シーン計算グループに未登録の場合は、シーン描画グループ0番にだけ自動的に登録される。
   *                 シーン計算グループに未登録の場合は、他のシーン描画グループに登録済みの場合でも、シーン描画グループ0番だけになる。
   *                 シーン計算グループに登録済みの場合は、シーン描画グループは変更されない。
   *  
   *  @param[in]     scene_node                     登録するシーンノード
   *  @param[in]     scene_calculate_group_index    scene_nodeを入れるシーン計算グループ
   *
   *  @retval        b32   登録できたらtrue
   */
  //-----------------------------------------------------------------------------
  b32  AddSceneNode( SceneNode* scene_node, s32 scene_calculate_group_index = 0 );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループからシーンノードの登録を解除する。
   *                 このシーンノードが登録されている全てのシーン描画グループからも解除される。
   *  
   *  @param[in]     scene_node    登録を解除するシーンノード
   *  
   *  @retval        b32   解除できたらtrue
   */
  //-----------------------------------------------------------------------------
  b32  RemoveSceneNode( SceneNode* scene_node );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループから全てのシーンノードの登録を解除する。
   *                 シーン描画グループからも全て解除される。
   *  
   *  @retval        b32   trueしか返さない。(全て解除できたらtrue、解除できないものが1つでもあったらfalse、というふうにはできない)
   */
  //-----------------------------------------------------------------------------
  b32  RemoveAllSceneNodes(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループにシーンノードを登録する。
   *
   *                 [シーン計算グループ]
   *                 シーン計算グループに未登録の場合は、シーン計算グループscene_calculate_group_index番に登録される。
   *                 シーン計算グループに登録済みの場合は、シーン計算グループscene_calculate_group_index番に変更される。
   *                 シーン計算グループは1つにしか登録できない。
   *                 
   *                 [シーン描画グループ]
   *                 シーン計算グループに未登録の場合は、シーン描画グループ0番にだけ自動的に登録される。
   *                 シーン計算グループに未登録の場合は、他のシーン描画グループに登録済みの場合でも、シーン描画グループ0番だけになる。
   *                 シーン計算グループに登録済みの場合は、シーン描画グループは変更されない。
   *  
   *  @param[in]     model, light, camera, fog, particle    登録するシーンノード
   *  @param[in]     scene_calculate_group_index            scene_nodeを入れるシーン計算グループ
   *
   *  @retval        b32   登録できたらtrue
   */
  //-----------------------------------------------------------------------------
  b32 AddModelToSceneCalculateGroup( Model*    model,    s32 scene_calculate_group_index = 0 );
  b32 AddLightToSceneCalculateGroup( Light*    light,    s32 scene_calculate_group_index = 0 );
  b32 AddCameraToSceneCalculateGroup( Camera*   camera,   s32 scene_calculate_group_index = 0 );
  b32 AddFogToSceneCalculateGroup( Fog*      fog,      s32 scene_calculate_group_index = 0 );
  b32 AddParticleToSceneCalculateGroup( Particle* particle, s32 scene_calculate_group_index = 0 );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループからシーンノードの登録を解除する。
   *                 このシーンノードが登録されている全てのシーン描画グループからも解除される。
   *  
   *  @param[in]     model, light, camera, fog, particle   登録を解除するシーンノード
   *  
   *  @retval        b32   解除できたらtrue
   */
  //-----------------------------------------------------------------------------
  b32 RemoveModelFromSceneCalculateGroup( Model*    model    );
  b32 RemoveLightFromSceneCalculateGroup( Light*    light    );
  b32 RemoveCameraFromSceneCalculateGroup( Camera*   camera   );
  b32 RemoveFogFromSceneCalculateGroup( Fog*      fog      );
  b32 RemoveParticleFromSceneCalculateGroup( Particle* particle );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループ、シーン描画グループにシーンノードを登録する。
   *
   *                 [シーン計算グループ]
   *                 シーン計算グループに未登録の場合は、シーン計算グループscene_calculate_group_index番に登録される。
   *                 シーン計算グループに登録済みの場合は、シーン計算グループscene_calculate_group_index番に変更される。
   *                 シーン計算グループは1つにしか登録できない。
   *                 
   *                 [シーン描画グループ]
   *                 シーン計算グループに未登録の場合は、シーン描画グループscene_draw_group_index番にだけ登録される。
   *                 シーン計算グループに未登録の場合は、他のシーン描画グループに登録済みの場合でも、シーン描画グループscene_draw_group_index番だけになる。
   *                 シーン計算グループに登録済みの場合は、シーン描画グループscene_draw_group_index番に登録される。
   *                 シーン計算グループに登録済みの場合は、他のシーン描画グループに登録済みのときは複数のシーン描画グループに登録されることになる。
   *                 シーン描画グループは複数に登録できる。
   *  
   *  @param[in]     model, particle                登録するシーンノード
   *  @param[in]     scene_calculate_group_index    scene_nodeを入れるシーン計算グループ
   *  @param[in]     scene_draw_group_index         scene_nodeを入れるシーン描画グループ
   *
   *  @retval        b32   登録できたらtrue
   */
  //-----------------------------------------------------------------------------
  b32 AddModelToSceneCalculateGroupAndSceneDrawGroup( Model*  model, s32 scene_calculate_group_index = 0, s32 scene_draw_group_index = 0 );
  b32 AddParticleToSceneCalculateGroupAndSceneDrawGroup( Particle* particle, s32 scene_calculate_group_index = 0, s32 scene_draw_group_index = 0 );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループから全てのシーンノードの登録を解除する。
   *                 シーン描画グループからも全て解除される。
   *  
   *  @retval        b32   trueしか返さない。(全て解除できたらtrue、解除できないものが1つでもあったらfalse、というふうにはできない)
   */
  //-----------------------------------------------------------------------------
  b32  RemoveAllSceneNodesFromSceneCalculateGroup(void);
  //b32  RemoveAllSceneNodesFromSceneCalculateGroup(s32 scene_calculate_group_index);  // シーン計算グループscene_calculate_groupに登録されているものだけ解除する、という関数も必要か？

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループにシーンノードを登録する。
   *
   *                 [シーン計算グループ]
   *                 シーン計算グループに未登録の場合は、シーン計算グループ0番に自動的に登録される。
   *                 シーン計算グループに登録済みの場合は、シーン計算グループは変更されない。
   *                 
   *                 [シーン描画グループ]
   *                 シーン描画グループscene_draw_group_index番に登録される。
   *                 他のシーン描画グループに登録済みのときは複数のシーン描画グループに登録されることになる。
   *                 シーン描画グループは複数に登録できる。
   *  
   *  @param[in]     model, particle           登録するシーンノード
   *  @param[in]     scene_draw_group_index    scene_nodeを入れるシーン描画グループ
   *
   *  @retval        b32   登録できたらtrue
   */
  //-----------------------------------------------------------------------------
  b32 AddModelToSceneDrawGroup(Model* model, s32 scene_draw_group_index);
  b32 AddParticleToSceneDrawGroup(Particle* particle, s32 scene_draw_group_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         全てのシーン描画グループにシーンノードを登録する。
   *
   *                 [シーン計算グループ]
   *                 シーン計算グループに未登録の場合は、シーン計算グループ0番に自動的に登録される。
   *                 シーン計算グループに登録済みの場合は、シーン計算グループは変更されない。
   *                 
   *                 [シーン描画グループ]
   *                 全てのシーン描画グループに登録される。
   *                 シーン描画グループは複数に登録できる。
   *  
   *  @param[in]     model, particle           登録するシーンノード
   *  @param[in]     scene_draw_group_index    scene_nodeを入れるシーン描画グループ
   *
   *  @retval        b32   登録できたらtrue
   */
  //-----------------------------------------------------------------------------
  b32 AddModelToAllSceneDrawGroup(Model* model);
  b32 AddParticleToAllSceneDrawGroup(Particle* particle);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループからシーンノードの登録を解除する。
   *
   *                 [シーン計算グループ]
   *                 シーン計算グループは変更しない。
   *                 
   *                 [シーン描画グループ]
   *                 シーン描画グループscene_draw_group_index番の登録を解除する。
   *                 シーン描画グループscene_draw_group_index番以外にも複数登録されているときは、scene_draw_group_index番以外の登録は残る。
   *                 シーン描画グループは複数に登録できる。
   *  
   *  @param[in]     model, particle           登録を解除するシーンノード
   *  @param[in]     scene_draw_group_index    scene_nodeを取り除くシーン描画グループ
   *
   *  @retval        b32   登録を解除できたらtrue
   *                       登録を解除できなかったらfalse
   *                       元々登録されていないシーン描画グループから取り除こうとしたらfalse
   */
  //-----------------------------------------------------------------------------
  b32 RemoveModelFromSceneDrawGroup(Model* model, s32 scene_draw_group_index);
  b32 RemoveParticleFromSceneDrawGroup(Particle* particle, s32 scene_draw_group_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         全てのシーン描画グループからシーンノードの登録を解除する。
   *
   *                 [シーン計算グループ]
   *                 シーン計算グループは変更しない。
   *                 
   *                 [シーン描画グループ]
   *                 全てのシーン描画グループから登録を解除する。
   *                 シーン描画グループは複数に登録できる。
   *  
   *  @param[in]     model, particle           登録を解除するシーンノード
   *
   *  @retval        b32   登録を解除できたらtrue
   *                       登録を解除できなかったらfalse
   *                       元々登録されていなかったらfalse
   */
  //-----------------------------------------------------------------------------
  b32 RemoveModelFromAllSceneDrawGroup(Model* model);
  b32 RemoveParticleFromAllSceneDrawGroup(Particle* particle);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // シーン環境
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境を設定する
   *
   *  @param[in]     scene_environment_setting  設定するシーン環境
   */
  //-----------------------------------------------------------------------------
  void SetSceneEnvironmentSetting( SceneEnvironmentSetting* scene_environment_setting );

/*

[質問][NW4C]SceneEnvironmentのライト反映方法について

開発環境 : RVCT 4.0 for Nintendo (20100728 build 839) + NintendoWare for CTR 1.0.0 グラフィックスパッチ(20100803 版)


みけ 2010-09-15 18:50:59 

お世話になっています。
初歩的な質問で恐縮なのですが、モデルに上手くライトが反映されていない状況です。

SceneEnvironmentSettingsを環境ファイルリソース(.cenv)から作成し、
既に作成されているRenderContextのSceneEnvironmentに反映しているのですが、
モデルの方に上手くライトが反映されない状況です（真っ黒になります）

SceneEnvironmentSetting::ResolveReference → SceneEnvironment::ApplyFrom

を行った後、リソースからLightもインスタンスを生成して、ツリーに登録しています。
この他になにか手続きは必要でしょうか？
サンプルを見ている感じでは他にやっていることはなかったようなのですが…。

よろしくお願いします。


maru 2010-09-15 18:59:32 

横から失礼します。

自分もちょっとはまったのですが、どうやら先にシーンルートに
ライトが追加されていないとだめっぽいですよ。
まちがっていたらごめんなさい。。。

こんな感じ↓

// ライトのインスタンスを生成します。
↓
// フォグのインスタンスを生成します。
↓
// シーンに追加します。
↓
// シーンツリーをトラバースして初期化を行います。
↓
最後にこれ>SceneEnvironmentSetting::ResolveReference → SceneEnvironment::ApplyFrom



田中＠NTSC 2010-09-15 19:11:03 

はい。そうなります。
ApplyFromより後で登録するのであれば自分で、
SceneEnvironmentからLightSetを取得して、
生成したライトを追加するなどしてください。


みけ 2010-09-16 15:24:20 

お世話になっています。

ヘルプの基本的な機能を見てResolveReferrenceやApplyFromが先だと先入観があったようです。
ライトなどを作成した後に環境を設定したら、意図した通りライトが反映されるようになりました。

ありがとうございます。

*/


  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // カメラ設定
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境にカメラを設定する
   *
   *  @param[in]     camera         カメラ(camera_indexに設定する)
   *                                StereoCamera型も可
   *  @param[in]     camera_index   カメラのインデックス
   *
   *  「camera=NULL, camera_index=インデックス」を渡すとcamera_indexに割り当てていたカメラをなしにする
   */
  //-----------------------------------------------------------------------------
  void AssignCameraIndex( Camera* camera, s32 camera_index );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         カメラを設定する
   *
   *  @param[in]     index      設定するインデックス
   *  @param[in]     camera     設定するカメラ
   * 
   *  「camera=NULL, camera_index=インデックス」を渡すとindexに割り当てていたカメラをなしにする
   */
  //-----------------------------------------------------------------------------
  void SetCamera( s32 index, Camera* camera );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境に設定したカメラのインデックスを取得する
   *
   *  @param[in]     camera         カメラ
   *                                StereoCamera型も可
   *
   *  @retval        s32    カメラのインデックス
   *                        カメラにインデックスを割り振っていなかったらCAMERA_INDEX_NONEを返す
   */
  //-----------------------------------------------------------------------------
  s32  GetAssignedCameraIndex( const Camera* camera ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境に設定したカメラを取得する
   *
   *  @param[in]     camera_index         カメラのインデックス
   *
   *  @retval        Camera*    カメラ
   *                            StereoCamera型であってもCamera型で返すので、この関数で取得後に判別する必要がある
   *                            camera_indexにカメラを割り当てていなかったらNULLを返す
   */
  //-----------------------------------------------------------------------------
  Camera* GetAssignedCamera( const s32 camera_index ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループのアクティブカメラのカメラインデックスを設定する
   *
   *  @param[in]     camera_index             カメラのインデックス
   *  @param[in]     scene_draw_group_index   シーン描画グループのインデックス
   */
  //-----------------------------------------------------------------------------
  void SetActiveCameraIndex( s32 camera_index, s32 scene_draw_group_index = 0 );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループのアクティブカメラのカメラインデックスを取得する
   *
   *  @param[in]     scene_draw_group_index   シーン描画グループのインデックス
   */
  //-----------------------------------------------------------------------------
  s32  GetActiveCameraIndex( const s32 scene_draw_group_index = 0 ) const;


  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // ライトセット設定
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境にライトセットを設定する
   *
   *  @param[in]     light_set_index   設定先となるライトセットのインデックス 
   *  @param[in]     light_set         ライトセット。
   *                                   呼び出し元はこのライトセットを保持し続けて下さい。
   *                                   light_set_indexに設定しているライトセットを外す場合はNULLを渡して下さい。
   *                                   このライトセットに追加してあるライトは、このシーンに追加されていなければなりません。
   */
  //-----------------------------------------------------------------------------
  void SetLightSet(s32 light_set_index, LightSet* light_set);


  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // 描画環境
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief       描画環境を初期化する
   */
  //-----------------------------------------------------------------------------
  void ResetRenderState(void)
  {
    m_render_system->ResetState();
  }



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // 個数
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         個数を出力する(Releaseでは何もしない)
   *                 InNwSceneContext関数を使用しているので、Initializeをしてからでないと正確な値が取得できません。
   */
  //-----------------------------------------------------------------------------
  void PrintNum(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループの個数を得る
   */
  //-----------------------------------------------------------------------------
  u32 GetSceneCalculateGroupNum(void) const { return m_scene_calculate_group_num; }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループの個数を得る
   */
  //-----------------------------------------------------------------------------
  u32 GetSceneDrawGroupNum(void) const { return m_scene_draw_group_num; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン計算グループに追加されているシーンノードの個数を取得する
   */
  //-----------------------------------------------------------------------------
  u32 GetModelNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
  u32 GetLightNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
  u32 GetCameraNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
  u32 GetFogNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
  u32 GetParticleNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
#if GFL_GRP_G3D_H3D_USE
  u32 GetH3dModelNumInSceneCalculateGroup(s32 scene_calculate_group_index) const;
#endif 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         全シーン計算グループに追加されているシーンノードの個数を合計して取得する
   */
  //-----------------------------------------------------------------------------
  u32 GetModelNum(void) const;
  u32 GetLightNum(void) const;
  u32 GetCameraNum(void) const;
  u32 GetFogNum(void) const;
  u32 GetParticleNum(void) const;
#if GFL_GRP_G3D_H3D_USE
  u32 GetH3dModelNum(void) const;
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         全シーン計算グループに追加されているモデルのマテリアルの個数を合計して取得する
   */
  //-----------------------------------------------------------------------------
  u32 GetModelMaterialCount(void) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  レンダリングしたメッシュの数をカウント
   */
  //-----------------------------------------------------------------------------
  inline s32 GetRenderMeshCount( void ) const { return m_render_system->GetRenderMeshCount(); }
  

  //-----------------------------------------------------------------------------
  /**
   *  @brief         Nwシーンコンテキストに追加されているNwシーンノードの個数を取得する
   *                 Initializeをしてからでないと正確な値が取得できません。
   */
  //-----------------------------------------------------------------------------
  // 全シーン計算グループを足し合わせた総数
  u32 GetNwSceneNodeNumInAllNwSceneContext(void) const
  {
    u32 num = 0;
    for(u32 i=0; i<m_scene_calculate_group_num; ++i)
    {
      num += GetNwSceneNodeNumInNwSceneContext(i);
    }
    return num;
  }
  
  // 各シーン計算グループにある総数
  u32 GetNwSceneNodeNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwSceneNodeNumInNwSceneContext();
  }
  
  // 個別
  // UserRenderNode
  u32 GetNwUserRenderNodeNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwUserRenderNodeNumInNwSceneContext();
  }
  // モデル(SkeletalModel, ParticleModelの個数を含む)
  u32 GetNwModelNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwModelNumInNwSceneContext();
  }
  // スケルタルモデル
  u32 GetNwSkeletalModelNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwSkeletalModelNumInNwSceneContext();
  }
  // ライト(FragmentLight, VertexLight, HemiSphereLight, AmbientLightの個数を含む)
  u32 GetNwLightNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwLightNumInNwSceneContext();
  }
  // フラグメントライト
  u32 GetNwFragmentLightNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwFragmentLightNumInNwSceneContext();
  }
  // 頂点ライト
  u32 GetNwVertexLightNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwVertexLightNumInNwSceneContext();
  }
  // 半球ライト
  u32 GetNwHemiSphereLightNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwHemiSphereLightNumInNwSceneContext();
  }
  // アンビエントライト
  u32 GetNwAmbientLightNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwAmbientLightNumInNwSceneContext();
  }
  // カメラ
  u32 GetNwCameraNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwCameraNumInNwSceneContext();
  }
  // フォグ
  u32 GetNwFogNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwFogNumInNwSceneContext();
  }
  // パーティクルセット
  u32 GetNwParticleSetNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwParticleSetNumInNwSceneContext();
  }
  // パーティクルエミッタ
  u32 GetNwParticleEmitterNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwParticleEmitterNumInNwSceneContext();
  }
  // パーティクルモデル
  u32 GetNwParticleModelNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwParticleModelNumInNwSceneContext();
  }
  // アニメーション
  u32 GetNwAnimatableNodeNumInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwAnimatableNodeNumInNwSceneContext();
  }

  // モデルのマテリアル(各モデルのマテリアルの個数を合計したもの)(SkeletalModel, ParticleModelのマテリアルの個数を含む)
  u32 GetNwModelMaterialCountInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwModelMaterialCountInNwSceneContext();
  }
  // スケルタルモデルのマテリアル(各スケルタルモデルのマテリアルの個数を合計したもの)
  u32 GetNwSkeletalModelMaterialCountInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwSkeletalModelMaterialCountInNwSceneContext();
  }
  // パーティクルモデルのマテリアル(各パーティクルモデルのマテリアルの個数を合計したもの)
  u32 GetNwParticleModelMaterialCountInNwSceneContext(s32 scene_calculate_group_index) const
  {
    return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group->GetNwParticleModelMaterialCountInNwSceneContext();
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         Sceneが所有し管理するものの個数を取得する(シーン計算グループに登録されているかどうかは見ていない)
   */
  //-----------------------------------------------------------------------------
  u32 GetOwnModelNum(void) const;
  u32 GetOwnLightNum(void) const;
  u32 GetOwnCameraNum(void) const;
  u32 GetOwnFogNum(void) const;
  u32 GetOwnSceneEnvironmentSettingNum(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループに登録されているシーンノードの個数を取得する
   *                 全シーン計算グループを走査するので時間が掛かります。
   */
  //-----------------------------------------------------------------------------
  u32 GetModelNumInSceneDrawGroup(s32 scene_draw_group_index) const;
  u32 GetParticleNumInSceneDrawGroup(s32 scene_draw_group_index) const;


  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // 影
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ShadowSystem
   */
  //-----------------------------------------------------------------------------
  // 影の生成
  // CreateShadowSystemを呼んだ後は必ずScene::Initializeを呼んで下さい。
  void CreateShadowSystem(
      gfl::heap::NwAllocator*                               heap_allocator,
      gfl::heap::NwAllocator*                               device_allocator,
      const gfl::grp::g3d::ShadowSystem::Description* description = NULL );
  // 影の破棄
  void DestroyShadowSystem(void);
  // 影のレシーバーのセットアップ
  void SetupShadowReceiverModel(
      Model*                                 model,
      const s32                              material_index,
      const f32                              shadow_intensity,
      const ShadowSystem::ReceiverType receiver_type,
      const s32                              shadow_group_index = 0);
  // 影のキャスターのOn/Off
  void SwitchShadowCasterModel(Model* model, const b32 cast_on, const s32 shadow_group_index = 0);
/*
  // 影のレシーバーのOn/Off
  void SwitchShadowReceiverModel(
      Model*                                 model,
      const s32                              material_index,
      const ShadowSystem::ReceiverType receiver_type,
      const b32                              receive_on,
      const s32                              shadow_group_index = 0);
  );
*/
  // 影のレシーバーの影の強度を設定する
  void SetShadowReceiverModelShadowIntensity(
      Model*                                 model,
      const s32                              material_index,
      const f32                              shadow_intensity,
      const ShadowSystem::ReceiverType receiver_type,
      const s32                              shadow_group_index = 0);  // @note 使っていない引数があるので、引数を精査したほうがいい。
  // 影のSubmitView
  void SubmitViewOfShadow(const b32 update_camera_matrix = true); 
  // 影のDraw
  void DrawShadow(const b32 command_cache_dump = false);
  // 影の情報を取得
  gfl::grp::g3d::Camera* GetShadowGroupCamera(const s32 shadow_group_index = 0) const;



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // エッジ
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   * @brief エッジマップシステム生成関数
   *
   * @param heap_allocator          初期化時に確保するメモリをここから取ります
   * @param device_allocator        初期化時に確保するメモリをここから取ります
   * @param scene_draw_group_index  表示グループ
   * @param p_desc                  レンダーターゲット設定内容
   * @param f_delete_memory_control １フレ遅れ破棄を任せるかどうか
   */
  //-----------------------------------------------------------------------------
  void CreateEdgeMapSystem( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, s32 scene_draw_group_index, gfl::grp::RenderTarget::Description* p_desc = NULL, gfl::grp::RenderTarget::Description* p_low_desc = NULL, b32 f_delete_memory_control = true );

  //-----------------------------------------------------------------------------
  /**
   * @brief エッジマップシステム破棄
   */
  //-----------------------------------------------------------------------------
  void DestroyEdgeMapSystem(void);
  
  //-----------------------------------------------------------------------------
  /**
   * @brief エッジマップシステム取得
   * @return 
   */
  //-----------------------------------------------------------------------------
  G3DEdgeMapSystem* GetG3DEdgeMapSystem(){ return m_g3d_edge_map_system; }

  //-----------------------------------------------------------------------------
  /**
   * @brief エッジマップ描画設定
   *
   * @param model     モデルオブジェクト
   * @param cast_on   有効無効
   * @param group_index グループ番号(SCENE_DRAW_GROUP_INDEX_NONEでシステムの値を使う)
   */
  //-----------------------------------------------------------------------------
  void SwitchEdgeMapModel( Model* model, const b32 cast_on , const s32 group_index = SCENE_DRAW_GROUP_INDEX_NONE );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief エッジマップ描画設定
   *
   * @param model     H3dモデルオブジェクト
   * @param cast_on   有効無効
   * @param group_index グループ番号(SCENE_DRAW_GROUP_INDEX_NONEでシステムの値を使う)
   */
  //-----------------------------------------------------------------------------
  void SwitchEdgeMapModel( H3dModel* model, const b32 cast_on , const s32 group_index = SCENE_DRAW_GROUP_INDEX_NONE );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         エッジマップ描画処理
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     display_type           表示面
   *  @param[in]     camera                 このシーンを描画するカメラ
   *  @param[in]     group_index            グループ番号(SCENE_DRAW_GROUP_INDEX_NONEでシステムの値を使う)
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void EdgeMapDraw(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::DisplayType         display_type,
    Camera*                       camera,
    const s32                     group_index = SCENE_DRAW_GROUP_INDEX_NONE ,
    bool                          command_cache_dump = false
  );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         エッジマップ描画処理
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     command_target         コマンド先
   *  @param[in]     camera                 このシーンを描画するカメラ
   *  @param[in]     group_index            グループ番号(SCENE_DRAW_GROUP_INDEX_NONEでシステムの値を使う)
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void EdgeMapDraw(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::RenderTarget*       command_target,
    Camera*                       camera,
    const s32                     group_index = SCENE_DRAW_GROUP_INDEX_NONE ,
    bool                          command_cache_dump = false
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         エッジマップステレオ描画処理
   *                 上左目画面(DISPLAY_UPPER)、上右目画面(DISPLAY_UPPER_RIGHT)に描画する
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     camera                 このシーンを描画するステレオカメラ
   *  @param[in]     group_index            グループ番号(SCENE_DRAW_GROUP_INDEX_NONEでシステムの値を使う)
   *  @param[in]     clear_buffer           バッファをクリアするか
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void StereoEdgeMapDraw(
    gfl::grp::GraphicsSystem*    graphics_system,
    StereoCamera*                camera,
    const s32                    group_index = SCENE_DRAW_GROUP_INDEX_NONE,
    bool                         clear_buffer = true,
    bool                         command_cache_dump = false
  );
  
  //! エッジマップ描画情報
  struct EdgeMapDrawInfo{
    gfl::grp::GraphicsSystem*    graphics_system;       //! グラフィックスシステム
    StereoCamera*                camera;                //! このシーンを描画するステレオカメラ
    gfl::grp::RenderTarget*      left_render_target;    //! 左目用レンダーターゲット
    gfl::grp::RenderTarget*      right_render_target;   //! 右目用レンダーターゲット
    s32                          group_index;           //! グループ番号(SCENE_DRAW_GROUP_INDEX_NONEでシステムの値を使う)
    bool                         clear_color;           //! カラーバッファをクリアするか
    bool                         clear_zbuffer;         //! 深度バッファをクリアするか
    bool                         command_cache_dump;    //! コマンドキャッシュをログ出力する
  };
  
  //-----------------------------------------------------------------------------
  /**
   * @brief エッジマップステレオ描画処理
   * @param rInfo       エッジマップ描画情報
   */
  //-----------------------------------------------------------------------------
  void StereoEdgeMapDraw( const EdgeMapDrawInfo &rInfo );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         エッジマップステレオ描画処理
   *                 上左目画面(DISPLAY_UPPER)、上右目画面(DISPLAY_UPPER_RIGHT)に描画する
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     left_camera            左目用カメラ
   *  @param[in]     left_render_target     左目用レンダーターゲット
   *  @param[in]     right_camera           右目用カメラ
   *  @param[in]     right_render_target    右目用レンダーターゲット
   *  @param[in]     group_index            グループ番号(SCENE_DRAW_GROUP_INDEX_NONEでシステムの値を使う)
   *  @param[in]     clear_buffer           バッファをクリアするか
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void StereoEdgeMapDraw(
      GraphicsSystem*              graphics_system,
      Camera*                      left_camera, 
      gfl::grp::RenderTarget*      left_render_target,
      Camera*                      right_camera,
      gfl::grp::RenderTarget*      right_render_target,
      const s32                    group_index = SCENE_DRAW_GROUP_INDEX_NONE,
      bool                         clear_buffer = true,
      bool                         command_cache_dump = false )
  {
  }
  
  //-----------------------------------------------------------------------------
  /**
   * @brief エッジフィルタのセットアップ
   */
  //-----------------------------------------------------------------------------
  void SetUpEdgeFilter( gfl::heap::NwAllocator* heap_allocator, const gfl::grp::GraphicsSystem* graphics_system, gfl::grp::g3d::G3DPostEffectSystem::Description* p_desc );
 

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // ポスト処理
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   * @brief ポスト処理システム生成
   * @param heap_allocator      生成に用いるアロケーター
   * @param device_allocator    生成に用いるアロケーター
   * @param p_desc              初期化設定
   */
  //-----------------------------------------------------------------------------
  void CreatePostEffectSystem(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      gfl::grp::g3d::G3DPostEffectSystem::Description* p_desc,
      const gfl::grp::GraphicsSystem* graphics_system          = NULL,
      const gfl::grp::MemoryArea      frame_buffer_memory_area = gfl::grp::MEMORY_AREA_NONE
  );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief ポスト処理システム破棄
   */
  //-----------------------------------------------------------------------------
  // これは1フレーム遅れ解放ができていないので、注意して使用して下さい。
  void DestroyPostEffectSystem();
  
  //宣言だけ
  void SetPostEffectOrder();
  
  //-----------------------------------------------------------------------------
  /**
   * @brief エッジ有効無効設定
   */
  //-----------------------------------------------------------------------------
  void SetEdgeEnable(b32 f);
  
  //-----------------------------------------------------------------------------
  /**
   * @brief エッジが有効か？
   * @return 有効無効フラグ
   */
  //-----------------------------------------------------------------------------
  b32 IsEdgeEnable();
  
  //-----------------------------------------------------------------------------
  /**
   * @brief エッジタイプ指定
   * @param type     タイプ
   */
  //-----------------------------------------------------------------------------
  void SetEdgeType( G3DEdgeFilter::EdgeType type );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief エッジの太さ調整
   * @param scale     スケール値
   */
  //-----------------------------------------------------------------------------
  void SetEdgeWidthScale( f32 scale = 1.0f );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief エッジの色設定
   * @param scale     スケール値
   */
  //-----------------------------------------------------------------------------
  void SetEdgeColor( f32 r, f32 g, f32 b, f32 a = 0.0f );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief エッジマップ描画グループ用SubmitView関数
   *
   * @param camera カメラ
   * @param update_camera_matrix
   * @param group_index グループ番号(SCENE_DRAW_GROUP_INDEX_NONEでシステムの値を使う)
   */
  //-----------------------------------------------------------------------------
  void SubmitViewOfEdge( StereoCamera* camera, bool update_camera_matrix = true , const s32 group_index = SCENE_DRAW_GROUP_INDEX_NONE );

  //-----------------------------------------------------------------------------
  /**
   * @brief エッジマップ描画グループ用SubmitView関数
   *
   * @param camera カメラ
   * @param update_camera_matrix
   * @param group_index グループ番号(SCENE_DRAW_GROUP_INDEX_NONEでシステムの値を使う)
   */
  //-----------------------------------------------------------------------------
  void SubmitViewOfEdge( Camera* camera, bool update_camera_matrix = true , const s32 group_index = SCENE_DRAW_GROUP_INDEX_NONE );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ポストエフェクト描画処理
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     display_type           表示面
   *  @param[in]     camera                 このシーンを描画するカメラ
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void PostEffectDraw(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::DisplayType         display_type,
    Camera*                       camera,
    bool                          command_cache_dump = false
  );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ポストエフェクト描画処理
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     render_target          表示ターゲット
   *  @param[in]     camera                 このシーンを描画するカメラ
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void DrawPostEdge(
    gfl::grp::GraphicsSystem* graphics_system,
    gfl::grp::RenderTarget* render_target,
    Camera* camera,
    bool command_cache_dump = false
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ポストエフェクト描画処理
   *                 上左目画面(DISPLAY_UPPER)、上右目画面(DISPLAY_UPPER_RIGHT)に描画する
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     camera                 このシーンを描画するステレオカメラ
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void StereoPostEffectDraw(
    gfl::grp::GraphicsSystem*    graphics_system,
    StereoCamera*                camera,
    bool                         command_cache_dump = false
  );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         エッジ描画処理
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     display_type           表示面
   *  @param[in]     camera                 このシーンを描画するカメラ
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void DrawPostEdge(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::DisplayType         display_type,
    Camera*                       camera,
    bool                          command_cache_dump = false
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         エッジステレオ描画処理
   *                 上左目画面(DISPLAY_UPPER)、上右目画面(DISPLAY_UPPER_RIGHT)に描画する
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     camera                 このシーンを描画するステレオカメラ
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void StereoDrawPostEdge(
    gfl::grp::GraphicsSystem*    graphics_system,
    StereoCamera*                camera,
    bool                         command_cache_dump = false
  );


 
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // 被写界深度
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   * @brief 被写界深度システム生成関数
   *
   * @param heap_allocator          初期化時に確保するメモリをここから取ります
   * @param device_allocator        初期化時に確保するメモリをここから取ります
   * @param graphics_system         初期化時に確保するビデオメモリをここから取ります
   */
  //-----------------------------------------------------------------------------
  void CreateDepthOfFieldSystem( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, gfl::grp::GraphicsSystem* graphics_system );

  //-----------------------------------------------------------------------------
  /**
   * @brief 被写界深度システム破棄
   */
  //-----------------------------------------------------------------------------
  void DestroyDepthOfFieldSystem(void);
  
  //-----------------------------------------------------------------------------
  /**
   * @brief 被写界深度タイプ指定
   */
  //-----------------------------------------------------------------------------
  void SetDofType( DofType::Enum type );

  //-----------------------------------------------------------------------------
  /**
   * @brief 被写界深度フォーカス距離指定
   */
  //-----------------------------------------------------------------------------
  void SetFocusLength( f32 focus );
  
  //-----------------------------------------------------------------------------
  /**
   * @brielf    ぼやけレイヤー設定（最大4段階）
   * @param layer             ぼやけレイヤー番号
   * @param rangeDistance     影響範囲
   * @param blurred           ぼやけ具合( 最小0.0f ? 1.0f最大 )
   */
  //-----------------------------------------------------------------------------
  void SetFocusRange( u32 layer, f32 rangeDistance, f32 blurred );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief   ぼやけ設定リセット
   */
  //-----------------------------------------------------------------------------
  void ReSetFocusRange();
  
  //-----------------------------------------------------------------------------
  /**
   * @brief   ぼかしスケール設定(今のところ前方ぼかしのみに対応)
   */
  //-----------------------------------------------------------------------------
  void SetBlurScale( f32 blurScale );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         被写界深度描画処理
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     display_type           表示面
   *  @param[in]     camera                 このシーンを描画するカメラ
   *  @param[in]     command_cache_dump     コマンドキャッシュをログ出力する
   */
  //-----------------------------------------------------------------------------
  void DrawPostDepthOfField(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::DisplayType         display_type,
    Camera*                       camera
  );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         被写界深度描画処理
   *                 上左目画面(DISPLAY_UPPER)、上右目画面(DISPLAY_UPPER_RIGHT)に描画する
   *
   *  @param[in]     graphics_system        グラフィックスシステム
   *  @param[in]     camera                 このシーンを描画するステレオカメラ
   */
  //-----------------------------------------------------------------------------
  void StereoDrawPostDepthOfField(
    gfl::grp::GraphicsSystem*    graphics_system,
    StereoCamera*                camera
  );
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         被写界深度用描画バッファ取得
   *
   *  @param[in]     DisplayType        DisplayType
   */
  //-----------------------------------------------------------------------------
  gfl::grp::RenderTarget* GetDepthOfFieldRenderTarget( DisplayType type = DISPLAY_UPPER );

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // BloomFilter
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         BloomFilter
   */
  //-----------------------------------------------------------------------------
public:
  // createBloomFilterを呼んだ後は必ずScene::Initializeを呼んで下さい。
  void CreateBloomFilter(void); 
  // これは1フレーム遅れ解放ができていないので、注意して使用して下さい。
  void DestroyBloomFilter(void);
  void SubmitViewOfBloom(void); 
  void DrawBloomPlainImageFrameBuffer(
      gfl::grp::GraphicsSystem*   graphics_system,
      const gfl::grp::DisplayType plain_image_display_type,  // plain_image_display_typeとrender_targetのコマンドリストが異なる場合、結果は保証されない。
      gfl::grp::RenderTarget*     render_target,
      const b32 command_cache_dump = false
  );
  void DrawBloomPlainImageOffScreenBuffer(
      gfl::grp::GraphicsSystem*     graphics_system,
      const gfl::grp::RenderTarget* plain_image_off_screen_buffer,  // plain_image_off_screen_bufferとrender_targetのコマンドリストが異なる場合、結果は保証されない。
      gfl::grp::RenderTarget*       render_target,
      const b32 command_cache_dump = false
  );
  void StereoDrawBloomPlainImageFrameBuffer(
      gfl::grp::GraphicsSystem* graphics_system,
      // plain_imageはgfl::grp::DISPLAY_UPPER
      gfl::grp::RenderTarget* left_render_target,   // gfl::grp::DISPLAY_UPPERとleft_render_targetのコマンドリストが異なる場合、結果は保証されない。
      // plain_imageはgfl::grp::DISPLAY_UPPER_RIGHT
      gfl::grp::RenderTarget* right_render_target,  // gfl::grp::DISPLAY_UPPER_RIGHTとright_render_targetのコマンドリストが異なる場合、結果は保証されない。
      const b32 command_cache_dump = false
  );
 
  u32  GetBloomSmallImageNum(void) const;
  u32  GetBloomBloomBoardNum(void) const;
  void SetBloomHighLuminanceColorWeight(const gfl::grp::ColorF32& c);
  void GetBloomHighLuminanceColorWeight(gfl::grp::ColorF32* c) const;
  void SetBloomHighLuminanceMinValue(const f32 v);
  f32  GetBloomHighLuminanceMinValue(void) const;
  void SetBloomBloomBoardExpanse(const f32 bloom_board_expanse);
  f32  GetBloomBloomBoardExpanse(void) const;
  void SetBloomBloomBoardIntensity(const f32 bloom_board_intensity);
  f32  GetBloomBloomBoardIntensity(void) const;
  void SetBloomBloomBoardSmallImageCount(const u32 bloom_board_index, const u32 small_image_count);
  u32  GetBloomBloomBoardSmallImageCount(const u32 bloom_board_index) const;


#if GFL_DEBUG
  Model* GetBloomTextureCheckBoardModel(void) const;
/*
うまく表示されないので、使用禁止。
  // コマンド蓄積(を行う関数を呼ぶ)
  void   SetImageFrameBufferToBloomTextureCheckBoard(const gfl::grp::DisplayType image_display_type);
*/
  // コマンド蓄積(を行う関数を呼ぶ)
  void   SetImageOffScreenBufferToBloomTextureCheckBoard(const gfl::grp::RenderTarget* image_off_screen_buffer);
  // コマンド蓄積(を行う関数を呼ぶ)
  void   SetBloomHighLuminanceImageToBloomTextureCheckBoard(void);
  // コマンド蓄積(を行う関数を呼ぶ)
  void   SetBloomHighLuminanceImageToBloomTextureCheckBoard(const gfl::math::VEC2& uv_scale, const gfl::math::VEC2& uv_translate);
#endif  // #if GFL_DEBUG

  //-----------------------------------------------------------------------------
  /**
   * @brief ブルーム有効無効設定
   */
  //-----------------------------------------------------------------------------
  void SetBloomEnable(b32 f);
  
  //-----------------------------------------------------------------------------
  /**
   * @brief ブルームが有効か？
   * @return 有効無効フラグ
   */
  //-----------------------------------------------------------------------------
  b32 IsBloomEnable();



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // 簡易処理
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         リソースからシーンを構築する
   *                 生成したオブジェクトはSceneが管理する。
   *  
   *  @param[in]     heap_allocator         ヒープアロケータ
   *  @param[in]     device_allocator  デバイスメモリのアロケータ
   *  @param[in]     Resource*         シーン構築に使用するリソース
   *                                   nw::gfx::res::ResSceneNodeの親子階層を反映する。
   *                                   これに含まれる以下のリソースから生成したオブジェクトが
   *                                   シーンに管理され、シーンに登録される。
   *                                   nw::gfx::res::ResSceneNode(nw::gfx::res::ResModel, nw::gfx::res::Light, など)
   *                                   これに含まれる以下のリソースから生成したオブジェクトが
   *                                   シーンに管理される。
   *                                   nw::gfx::res::ResSceneEnvironmentSetting
   *  @param[in]     user_id           生成したオブジェクトに対して、ユーザが任意で付けられるID
   */
  //-----------------------------------------------------------------------------
  void  BuildSceneUsingResource(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      Resource*               resource,
      s32                     user_id         = OBJECT_USER_ID_NOT_USE
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンが管理するオブジェクトを使って、シーン環境を設定する
   */
  //-----------------------------------------------------------------------------
  void  SetSceneEnvironmentUsingOwnObject(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンが管理するオブジェクトを破棄する
   */
  //-----------------------------------------------------------------------------
  void  DestroyOwnObject(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンが管理するオブジェクトを取得する
   *                 オブジェクトを取得はするが、シーンが管理し続けるので、解放してはならない。
   *  
   *  @param[in]     a_resource_id        リソースのID
   *  @param[in]     a_index_in_resource  リソース内におけるこのオブジェクトのインデックス
   *  @param[in]     a_user_id            ユーザが任意で付けたID
   *
   *  @retval        NULLのときなし
   */
  //-----------------------------------------------------------------------------
  Model*                    GetOwnModel( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id = OBJECT_USER_ID_ANYTHING );
  Camera*                   GetOwnCamera( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id = OBJECT_USER_ID_ANYTHING );
  Light*                    GetOwnLight( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id = OBJECT_USER_ID_ANYTHING );
  Fog*                      GetOwnFog( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id = OBJECT_USER_ID_ANYTHING );
  SceneEnvironmentSetting*  GetOwnSceneEnvironmentSetting( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id = OBJECT_USER_ID_ANYTHING );




  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // コールバック
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
public:
  // シーン描画グループに、Drawの前に呼び出すコールバック関数を設定する
  void SetDrawBeginCommandInvokeCallback(SceneDrawGroupCommandInvokeCallback callback = NULL, void* work = NULL, s32 scene_draw_group_index = 0 )
  {
    m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group->SetBeginCommandInvokeCallback(callback, work);
  }
  // シーン描画グループに、Drawの後に呼び出すコールバック関数を設定する
  void SetDrawEndCommandInvokeCallback(SceneDrawGroupCommandInvokeCallback callback = NULL, void* work = NULL, s32 scene_draw_group_index = 0 )
  {
    m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group->SetEndCommandInvokeCallback(callback, work);
  }




  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  //
  // 非公開定数
  //
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
private:
  //-------------------------------------
  //
  /// シーンノードを制御するビットフラグSCENE_NODE_PROPERTY_BIT_FLAG
  //
  //=====================================
  enum
  {
    // 単独
    SCENE_NODE_PROPERTY_BIT_FLAG_NONE             = 0x0,        // なし
    SCENE_NODE_PROPERTY_BIT_FLAG_TEMP_VISIBLE     = 0x1  << 0,  // ModelNodePropertyでのみ使用。モデルの描画フラグをSubmitView前に記憶し、
                                                                // SubmitView後に戻す。
    SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG = 0x1  << 1,  // ModelNodePropertyでのみ使用。CalculateCullingメンバ関数で変更されるシーンツリーフラグ。
                                                                // 1のときSceneにAddされていたらシーンツリーに入れられる、
                                                                // 0のときSceneにAddされていてもシーンツリーから外される。
    // 複合
    SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT          = SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG  // デフォルト
  };
 
  //-------------------------------------
  // 
  /// 越えてはならない上限数 
  //
  //=====================================
  enum
  {
    // シーンが所有し管理するものの個数
    SCENE_OWN_MODEL_NUM_MAX                      = 32000,  // u16の最大値
    SCENE_OWN_LIGHT_NUM_MAX                      = 32000,  // u16の最大値
    SCENE_OWN_CAMERA_NUM_MAX                     = 250,    // u8の最大値
    SCENE_OWN_FOG_NUM_MAX                        = 250,    // u8の最大値
    SCENE_OWN_SCENE_ENVIRONMENT_SETTING_NUM_MAX  = 250,    // u8の最大値

    // シーン計算グループ1個につき次の個数が最大値となる
    SCENE_CALCULATE_GROUP_MODEL_NUM_MAX          = 32000,  // u16の最大値
    SCENE_CALCULATE_GROUP_LIGHT_NUM_MAX          = 32000,  // u16の最大値
    SCENE_CALCULATE_GROUP_CAMERA_NUM_MAX         = 250,    // u8の最大値
    SCENE_CALCULATE_GROUP_FOG_NUM_MAX            = 250,    // u8の最大値
    SCENE_CALCULATE_GROUP_PARTICLE_NUM_MAX       = 32000,  // u16の最大値
                                                 
    // シーン計算グループ、シーン描画グループの個数
    SCENE_CALCULATE_GROUP_NUM_MAX                = 16000,  // s16の正の最大値
    SCENE_DRAW_GROUP_NUM_MAX                     = 16000   // s16の正の最大値
  };



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  //
  // 非公開構造体
  //
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
private:
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // シーンノードのプロパティ
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  
  //-------------------------------------
  // 
  /// シーンノードにシーン特有の情報を付加した構造体
  // 
  //=====================================
  struct SceneNodeProperty
  {
    SceneNode* scene_node;
    bit32      bit_flag;     // SCENE_NODE_PROPERTY_BIT_FLAG
    SceneNodeProperty(void)
        : scene_node(NULL),
          bit_flag(SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT)
    {}
  };

  //-------------------------------------
  // 
  /// シーンノードのモデル、パーティクルにシーン特有の情報を付加した構造体
  // 
  //=====================================
  struct ModelNodeProperty : public SceneNodeProperty
  {
    s16             scene_draw_group_index_one;  // scene_draw_group_index_one>=0のとき1つのシーン描画グループにしか入っていない。
                                                 // SCENE_CALCULATE_GROUP_INDEX_NONEのときどのシーン描画グループにも入っていない。
                                                 // SCENE_DRAW_GROUP_INDEX_MULTIのとき2つ以上のシーン描画グループに入っている。
    s8              padding[2];                  // 親にもpaddingって変数を持たせたら混乱のもとかもしれないが、
                                                 // padding変数なんてアクセスしないから気にしなくていいか。
    gfl::base::Bit* scene_draw_group_bit;        // m_scene_draw_group_numだけビットを用意する。
                                                 // scene_draw_group_index_oneがSCENE_DRAW_GROUP_INDEX_MULTIのとき、
                                                 // このビットのシーン描画グループに入っている。
    ModelNodeProperty(void)
        : SceneNodeProperty(),
          scene_draw_group_index_one(SCENE_DRAW_GROUP_INDEX_NONE),
          scene_draw_group_bit(NULL)
    {}
  };

#if GFL_GRP_G3D_H3D_USE
  //-------------------------------------
  // 
  /// h3dモデルにシーン特有の情報を付加した構造体
  // 
  //=====================================
  struct H3dModelProperty
  {
    H3dModel* scene_node;
    bit32      bit_flag;     // SCENE_NODE_PROPERTY_BIT_FLAG

    s16             scene_draw_group_index_one;
    s8              padding[2];          
    gfl::base::Bit* scene_draw_group_bit;
    
    H3dModelProperty(void)
        : scene_node(NULL),
          bit_flag(SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT),

          scene_draw_group_index_one(SCENE_DRAW_GROUP_INDEX_NONE),
          scene_draw_group_bit(NULL)
    {}
  };
#endif  // GFL_GRP_G3D_H3D_USE

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // おまとめ構造体
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////

  //-------------------------------------
  // 
  // SceneNodePropertyをまとめた構造体
  // 
  //=====================================

  // 型定義 
  typedef SceneNode* SceneNodePointer;
  typedef gfl::base::FixedListElement<SceneNodeProperty> SceneNodePropertyElement;
  typedef gfl::base::FixedListElement<ModelNodeProperty> ModelNodePropertyElement;
  typedef gfl::base::FixedListArraySentinel<SceneNodeProperty, SceneNodePointer> SceneNodePropertyArray;
  typedef gfl::base::FixedListArraySentinel<ModelNodeProperty, SceneNodePointer> ModelNodePropertyArray;
  struct ModelNodePropertyFunctionWork
  {
    gfl::heap::HeapBase* heap_memory;
    s16                  scene_draw_group_num;
    s8                   padding[2];
  };

  // 関数定義
  // SceneNodeProperty
  // CreateDataFunction
  static void CreateSceneNodePropertyFunction(SceneNodeProperty* a, void* work)
  {
    // 何もしない
  }
  // DestroyDataFunction
  static void DestroySceneNodePropertyFunction(SceneNodeProperty* a, void* work)
  {
    // 何もしない
  }
  // InitializeDefaultDataFunction
  static void InitializeDefaultSceneNodePropertyFunction(SceneNodeProperty* a, void* work) 
  {
    a->scene_node = NULL;
    a->bit_flag   = SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT;
  }
  // CompareDataFunction
  static s32  CompareSceneNodePropertyFunction(const SceneNodeProperty* a, const SceneNodePointer b, void* work)
  {
    if( a->scene_node == b )          return  0;
    else if( a->scene_node >  b )     return  1;
    else /*if( a->scene_node <  b )*/ return -1;
  }
  // CopyDataFunction
  static void CopySceneNodePropertyFunction(SceneNodeProperty* dst, const SceneNodeProperty* src, void* work)
  {
    dst->scene_node = src->scene_node;
    dst->bit_flag   = src->bit_flag;
  }

  // ModelNodeProperty
  // CreateDataFunction
  static void CreateModelNodePropertyFunction(ModelNodeProperty* a, void* work)
  {
    // workはModelNodePropertyFunctionWorkにしておく。
    ModelNodePropertyFunctionWork* l_work = reinterpret_cast<ModelNodePropertyFunctionWork*>(work);
    a->scene_draw_group_bit = GFL_NEW(l_work->heap_memory) gfl::base::Bit(l_work->heap_memory, l_work->scene_draw_group_num);
  }
  // DestroyDataFunction
  static void DestroyModelNodePropertyFunction(ModelNodeProperty* a, void* work)
  {
    GFL_DELETE a->scene_draw_group_bit;
  }
  // InitializeDefaultDataFunction
  static void InitializeDefaultModelNodePropertyFunction(ModelNodeProperty* a, void* work) 
  {
    InitializeDefaultSceneNodePropertyFunction(a, work); 
    a->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
    a->scene_draw_group_bit->OffAll();  // メモリ解放はしない
  }
  // CompareDataFunction
  static s32  CompareModelNodePropertyFunction(const ModelNodeProperty* a, const SceneNodePointer b, void* work)
  {
    return CompareSceneNodePropertyFunction(a, b, work);
  }
  // CopyDataFunction
  static void CopyModelNodePropertyFunction(ModelNodeProperty* dst, const ModelNodeProperty* src, void* work)
  {
    CopySceneNodePropertyFunction(dst, src, work);
    dst->scene_draw_group_index_one = src->scene_draw_group_index_one;
    dst->scene_draw_group_bit->Copy(src->scene_draw_group_bit);
  }

#if GFL_GRP_G3D_H3D_USE
  // 型定義 
  typedef H3dModel* H3dModelPointer;
  typedef gfl::base::FixedListElement<H3dModelProperty> H3dModelPropertyElement;
  typedef gfl::base::FixedListArraySentinel<H3dModelProperty, H3dModelPointer> H3dModelPropertyArray;
  struct H3dModelPropertyFunctionWork
  {
    gfl::heap::HeapBase* heap_memory;
    s16                  scene_draw_group_num;
    s8                   padding[2];
  };

  // 関数定義
  // H3dModelProperty
  // CreateDataFunction
  static void CreateH3dModelPropertyFunction(H3dModelProperty* a, void* work)
  {
    // workはH3dModelPropertyFunctionWorkにしておく。
    H3dModelPropertyFunctionWork* l_work = reinterpret_cast<H3dModelPropertyFunctionWork*>(work);
    a->scene_draw_group_bit = GFL_NEW(l_work->heap_memory) gfl::base::Bit(l_work->heap_memory, l_work->scene_draw_group_num);
  }
  // DestroyDataFunction
  static void DestroyH3dModelPropertyFunction(H3dModelProperty* a, void* work)
  {
    GFL_DELETE a->scene_draw_group_bit;
  }
  // InitializeDefaultDataFunction
  static void InitializeDefaultH3dModelPropertyFunction(H3dModelProperty* a, void* work) 
  {
    a->scene_node = NULL;
    a->bit_flag   = SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT;
 
    a->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
    a->scene_draw_group_bit->OffAll();  // メモリ解放はしない
  }
  // CompareDataFunction
  static s32  CompareH3dModelPropertyFunction(const H3dModelProperty* a, const H3dModelPointer b, void* work)
  {
    if( a->scene_node == b )          return  0;
    else if( a->scene_node >  b )     return  1;
    else /*if( a->scene_node <  b )*/ return -1;
  }
  // CopyDataFunction
  static void CopyH3dModelPropertyFunction(H3dModelProperty* dst, const H3dModelProperty* src, void* work)
  {
    dst->scene_node = src->scene_node;
    dst->bit_flag   = src->bit_flag;
    
    dst->scene_draw_group_index_one = src->scene_draw_group_index_one;
    dst->scene_draw_group_bit->Copy(src->scene_draw_group_bit);
  }
#endif  // GFL_GRP_G3D_H3D_USE

  // シーン計算グループごとに管理する
  struct SceneNodePropertyPack
  {
    ModelNodePropertyArray* model_array;
    SceneNodePropertyArray* light_array;
    SceneNodePropertyArray* camera_array;
    SceneNodePropertyArray* fog_array;
    ModelNodePropertyArray* particle_array;
#if GFL_GRP_G3D_H3D_USE
    H3dModelPropertyArray*  h3d_model_array;
#endif  // GFL_GRP_G3D_H3D_USE

    SceneNodePropertyPack(void)
        : model_array(NULL),
          light_array(NULL),
          camera_array(NULL),
          fog_array(NULL),
          particle_array(NULL)
#if GFL_GRP_G3D_H3D_USE
          , h3d_model_array(NULL)
#endif  // GFL_GRP_G3D_H3D_USE
    {}
  };
 
  //-------------------------------------
  // 
  // オブジェクトをまとめた構造体
  // 
  //=====================================
  // Sceneが所有し管理する
  struct ObjectPack
  {
    u16                       model_num_max;
    u16                       model_num;
    u16                       light_num_max;
    u16                       light_num;
    u8                        camera_num_max;
    u8                        camera_num;
    u8                        fog_num_max;
    u8                        fog_num;
    u8                        scene_environment_setting_num_max;
    u8                        scene_environment_setting_num;
    u8                        padding[2];
    Model**                   model_array;   // 常に[0]から隙間なく詰めておき値が入っている最後尾が[model_num-1]となるようにしておく。
    Light**                   light_array;   // 常に[0]から隙間なく詰めておき値が入っている最後尾が[light_num-1]となるようにしておく。
    Camera**                  camera_array;  // 常に[0]から隙間なく詰めておき値が入っている最後尾が[camera_num-1]となるようにしておく。
    Fog**                     fog_array;     // 常に[0]から隙間なく詰めておき値が入っている最後尾が[fog_num-1]となるようにしておく。
    SceneEnvironmentSetting** scene_environment_setting_array;   // 常に[0]から隙間なく詰めておき値が入っている
                                                                 // 最後尾が[scene_environment_setting_num-1]となるようにしておく。

    ObjectPack(void)
        : model_num_max(0),
          model_num(0),
          light_num_max(0),
          light_num(0),
          camera_num_max(0),
          camera_num(0),
          fog_num_max(0),
          fog_num(0),
          scene_environment_setting_num_max(0),
          scene_environment_setting_num(0),
          model_array(NULL),
          light_array(NULL),
          camera_array(NULL),
          fog_array(NULL)
    {}
  };

  //-------------------------------------
  // 
  /// シーン計算グループとその付属情報
  // 
  //=====================================
  struct SceneCalculateGroupPack
  {
    SceneCalculateGroup*     scene_calculate_group;
    SceneNodePropertyPack    scene_node_property_pack;
    bit32                    scene_calculate_group_enable_bit_flag;  // シーン計算グループを制御するビットフラグ

    SceneCalculateGroupPack(void)
      : scene_calculate_group(NULL),
        scene_node_property_pack(),
        scene_calculate_group_enable_bit_flag(SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_DEFAULT)
    {}
  };
 
  //-------------------------------------
  // 
  /// シーン描画グループとその付属情報
  // 
  //=====================================
  struct SceneDrawGroupPack
  {
    SceneDrawGroup*          scene_draw_group;
    const Camera*            scene_draw_group_camera;  // 他のところのカメラのポインタを覚えているだけで、カメラの実体は管理しない
    bit32                    scene_draw_group_enable_bit_flag;  // シーン描画グループを制御するビットフラグ

    SceneDrawGroupPack(void)
      : scene_draw_group(NULL),
        scene_draw_group_camera(NULL),
        scene_draw_group_enable_bit_flag(SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_DEFAULT)
    {}
  };



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  //
  // メンバ変数
  //
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
private:
  // 個数
  s16                       m_scene_calculate_group_num;
  s16                       m_scene_draw_group_num;
 
  // シーン計算グループ
  SceneCalculateGroupPack*       m_scene_calculate_group_pack_array;
  ModelNodePropertyFunctionWork* m_model_node_property_function_work;
#if GFL_GRP_G3D_H3D_USE
  H3dModelPropertyFunctionWork*  m_h3d_model_property_function_work;
#endif  // GFL_GRP_G3D_H3D_USE

  // シーン描画グループ
  SceneDrawGroupPack*       m_scene_draw_group_pack_array;
  nw::gfx::RenderQueue*     m_render_queue_TEMP;

  // Sceneが所有し管理するもの
  ObjectPack*               m_scene_own_object_pack;

  // 必須システム
  SceneSystem*              m_scene_system;
  RenderSystem*             m_render_system;

  // 追加システム
  ShadowSystem*             m_shadow_system;
  G3DEdgeMapSystem*         m_g3d_edge_map_system;
  G3DPostEffectSystem*      m_g3d_post_effect_system;
  G3DDepthOfFieldSystem*    m_G3DDepthOfFieldSystem;
  
  static f32                s_CullBoxScale;

#if GFL_DEBUG
private:
  // デバッグ
  b32  m_debug_tick_enable_flag;
  b32  m_debug_tick_dump_and_reset_flag;
#endif



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // 非公開static関数
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // SceneNodeProperty(ModelNodeProperty)に対する操作
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // 【戻り値注意】SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAGが1だったら【0以外(1とは限らないしいつも違う値になる)】を返す、0だったら【0】を返す。
  static inline u32 isSceneNodePropertyNotCullingFlagOn(const SceneNodeProperty& scene_node_property)
  { return IsBitFlagOnOne(scene_node_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG); }
  // SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAGのビットフラグを設定する
  static inline void setSceneNodePropertyNotCullingFlag(SceneNodeProperty* scene_node_property, b32 is_on)
  { SetBitFlag(&(scene_node_property->bit_flag), SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG, is_on); }
  
  // 【戻り値注意】SCENE_NODE_PROPERTY_BIT_FLAG_TEMP_VISIBLEが1だったら【0以外(1とは限らないしいつも違う値になる)】を返す、0だったら【0】を返す。
  static inline u32 isSceneNodePropertyTempVisibleOn(const SceneNodeProperty& scene_node_property)
  { return IsBitFlagOnOne(scene_node_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_TEMP_VISIBLE); }
  // SCENE_NODE_PROPERTY_BIT_FLAG_TEMP_VISIBLEのビットフラグを設定する
  static inline void setSceneNodePropertyTempVisible(SceneNodeProperty* scene_node_property, b32 visible)
  { SetBitFlag(&(scene_node_property->bit_flag), SCENE_NODE_PROPERTY_BIT_FLAG_TEMP_VISIBLE, visible); }
  
  // 今現在シーンノードがシーンツリーに入っていたらtrueを返す、入っていなかったらfalseを返す。
  // SceneにSceneNodeがAddされていることも確認する、処理を端折っていない版
  static inline b32 isSceneNodePropertyInSceneTree(const SceneNodeProperty& scene_node_property)
  {
    return \
      ( \
           IsBitFlagOnOne(scene_node_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG) \
        && scene_node_property.scene_node \
        && scene_node_property.scene_node->IsAddedToScene() \
        && scene_node_property.scene_node->GetTreeFlag() \
      );
      // SceneにSceneNodeがAddされていることが確実な場合は
      //     && scene_node_property.scene_node \
      //     && scene_node_property.scene_node->IsAddedToScene() \
      // の確認は必要ないので、
      // 処理を端折った版のチェック関数isSceneNodePropertyInSceneTreeEasyを使ってよい。
  }

  // 今現在シーンノードがシーンツリーに入っていたらtrueを返す、入っていなかったらfalseを返す。
  // SceneにSceneNodeがAddされていることが確実な場合にだけ呼んでよい、処理を端折った版
  static inline b32 isSceneNodePropertyInSceneTreeEasy(const SceneNodeProperty& scene_node_property)
  {
    return \
      ( \
           IsBitFlagOnOne(scene_node_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG) \
        && scene_node_property.scene_node->GetTreeFlag() \
      );
  }
#if GFL_GRP_G3D_H3D_USE
  // 【戻り値注意】SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAGが1だったら【0以外(1とは限らないしいつも違う値になる)】を返す、0だったら【0】を返す。
  static inline u32 isH3dModelPropertyNotCullingFlagOn(const H3dModelProperty& h3d_model_property)
  { return IsBitFlagOnOne(h3d_model_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG); }
  // SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAGのビットフラグを設定する
  static inline void setSceneNodePropertyNotCullingFlag(H3dModelProperty* h3d_model_property, b32 is_on)
  { SetBitFlag(&(h3d_model_property->bit_flag), SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG, is_on); }

  static inline b32 isH3dModelPropertyInSceneTreeEasy(const H3dModelProperty& h3d_model_property)
  {
    return \
      ( \
           IsBitFlagOnOne(h3d_model_property.bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_NOT_CULLING_FLAG) \
        && h3d_model_property.scene_node->GetTreeFlag() \
      );
  }
#endif  // GFL_GRP_G3D_H3D_USE

  // シーンノードのプロパティをデフォルト状態になるようにクリアする。確保メモリの解放はしない。
  static inline void clearDefaultSceneNodeProperty(SceneNodeProperty* scene_node_property)
  {
    scene_node_property->scene_node = NULL;
    scene_node_property->bit_flag   = SCENE_NODE_PROPERTY_BIT_FLAG_DEFAULT;
  }
  static inline void clearDefaultModelNodeProperty(ModelNodeProperty* scene_node_property)
  {
    clearDefaultSceneNodeProperty(scene_node_property);
    scene_node_property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
    scene_node_property->scene_draw_group_bit->OffAll();
  }

  // シーンノードのプロパティをコピーする。
  static inline void copySceneNodeProperty(SceneNodeProperty* dst, const SceneNodeProperty* src)
  {
    dst->scene_node = src->scene_node;
    dst->bit_flag   = src->bit_flag;
  }
  static inline void copyModelNodeProperty(ModelNodeProperty* dst, const ModelNodeProperty* src)
  {
    copySceneNodeProperty(dst, src);
    dst->scene_draw_group_index_one = src->scene_draw_group_index_one;
    dst->scene_draw_group_bit->Copy(src->scene_draw_group_bit);
  }

  // シーンノードのプロパティのシーン描画グループの設定を更新する(addModelNodePropertyToDrawかremoveModelNodePropertyFromDrawからしか呼んではならない)
  static inline void updateModelNodePropertyDraw(ModelNodeProperty* scene_node_property)
  {
    // 現在のscene_node_property->scene_draw_group_bitに基づいて、scene_node_property->scene_draw_group_index_oneを更新する
    u32 bit_num = scene_node_property->scene_draw_group_bit->GetOnBitNum();
    if( bit_num == 0 )
    {
      scene_node_property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
    }
    else if( bit_num == 1 )
    {
      u32 index;
      scene_node_property->scene_draw_group_bit->GetOnBitMinIndex(&index);
      scene_node_property->scene_draw_group_index_one = index;
    }
    else
    {
      scene_node_property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_MULTI;
    }
  }
  // シーンノードのプロパティにシーン描画グループを設定する
  static inline b32 addModelNodePropertyToDraw(ModelNodeProperty* scene_node_property, s32 scene_draw_group_index)
  {
    scene_node_property->scene_draw_group_bit->On(scene_draw_group_index);
    updateModelNodePropertyDraw(scene_node_property);
    return true;
  }
  // シーンノードのプロパティに設定されているシーン描画グループを解除する
  static inline void removeModelNodePropertyFromDraw(ModelNodeProperty* scene_node_property, s32 scene_draw_group_index)
  {
    scene_node_property->scene_draw_group_bit->Off(scene_draw_group_index);
    updateModelNodePropertyDraw(scene_node_property);
  }
  // シーンノードのプロパティにシーン描画グループを全て設定する
  static inline b32 addModelNodePropertyToAllDraw(ModelNodeProperty* scene_node_property)
  {
    scene_node_property->scene_draw_group_bit->OnAll();
    if( scene_node_property->scene_draw_group_bit->GetBitNum() == 1 )
    {
      scene_node_property->scene_draw_group_index_one = 0;
    }
    else
    {
      scene_node_property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_MULTI;
    }
    return true;
  }
  // シーンノードのプロパティに設定されているシーン描画グループを全て解除する
  static inline void removeModelNodePropertyFromAllDraw(ModelNodeProperty* scene_node_property)
  {
    scene_node_property->scene_draw_group_bit->OffAll();
    scene_node_property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
  }



  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // 
  // 非公開関数
  // 
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // friendクラスであるSceneCalculateGroupからだけアクセスしてもよい関数
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // 現在のシーンツリーフラグでシーンツリーの状態を更新する
  void RefreshNodeTreeFlag(SceneNode* scene_node);
#if GFL_GRP_G3D_H3D_USE
  //void RefreshNodeTreeFlag(H3dModel* scene_node);
#endif  // GFL_GRP_G3D_H3D_USE


#if GFL_GRP_G3D_H3D_USE
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // friendクラスであるH3dEnvironmentからだけアクセスしてもよい関数
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // シーン環境を得る 
  const nw::gfx::SceneEnvironment& GetSceneEnvironment(s32* max_cameras, s32* max_light_sets, s32* max_fogs, s32* max_vertex_lights) const
  {
    return m_render_system->GetSceneEnvironment(max_cameras, max_light_sets, max_fogs, max_vertex_lights);
  }
#endif  // GFL_GRP_G3D_H3D_USE



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // 生成 / 破棄
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // シーン計算グループの生成
  void createSceneCalculateGroupPack(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description);
  // SceneNodePropertyArrayの生成
  inline SceneNodePropertyArray* createSceneNodePropertyArray(gfl::heap::HeapBase* heap_memory, s32 max_element_num);
  // ModelNodePropertyArrayの生成
  inline ModelNodePropertyArray* createModelNodePropertyArray(gfl::heap::HeapBase* heap_memory, s32 max_element_num);
#if GFL_GRP_G3D_H3D_USE
  // H3dModelPropertyArrayの生成 
  inline H3dModelPropertyArray* createH3dModelPropertyArray(gfl::heap::HeapBase* heap_memory, s32 max_element_num);
#endif  // GFL_GRP_G3D_H3D_USE
  // シーン描画グループの生成
  void createSceneDrawGroupPack(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description);
  // Sceneが所有し管理するものの生成
  void createSceneOwnObjectPack(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description);
  // シーン計算グループの破棄
  void destroySceneCalculateGroupPack(void);
  // シーン描画グループの破棄
  void destroySceneDrawGroupPack(void);
  // Sceneが所有し管理するものの破棄
  void destroySceneOwnObjectPack(void);
  // コンストラクタと同じ初期化
  // コンストラクタの初期化リスト(constructor initialize list)と
  // コンストラクタの中身でやっている初期化と同じ内容
  void constructorInitialize(void);

  // DetachChildでエラーが起きないかのチェック
  void checkErrorDetach(nw::gfx::SceneNode* parent_node, nw::gfx::SceneNode* child_node, const char* msg);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // Initialize
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // 内部で使用する初期化処理
  void initializeInternal(const SceneInitializeProcess scene_initialize_process = SCENE_INITIALIZE_PROCESS_ALL);
  void initializeInternal(SceneCalculateGroup* calc_group, const SceneInitializeProcess scene_initialize_process = SCENE_INITIALIZE_PROCESS_ALL);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  // 計算処理
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // g3dオブジェクト固有の更新処理を行う。
  // カリングの有無に関わらず必ず必要な更新が行われます。
  void updateObjectBeforeCulling(SceneCalculateGroupPack* scene_calculate_group_pack);
  // g3dオブジェクト固有の更新処理を行う。
  // カリングされて見えないときは、
  // しなくても問題が起きないない更新が行われます。
  void updateObjectAfterCulling(SceneCalculateGroupPack* scene_calculate_group_pack);

  // カメラのカメラマトリックスを更新する
  void updateCameraMatrix(const s32 scene_calculate_group_index);

  // カリング
  void resetCalculateCullingFlag(s32 scene_calculate_group_index, ModelNodePropertyArray* property_array);
  void setCalculateCullingFlag(s32 scene_calculate_group_index, SceneNodeProperty* scene_node_property, b32 not_culling_flag);  // falseのときカリングされてシーンツリーから外れる
  void calculateBeforeCulling(s32 scene_calculate_group_index);
  void calculateAfterCulling(s32 scene_calculate_group_index);
  void culling(Camera* camera);
  void culling(s32 scene_calculate_group_index, Camera* camera);

#if GFL_GRP_G3D_H3D_USE
  ///////////////////////////
  // 全部入り
  ///////////////////////////
  void calculateH3dModel(s32 scene_calculate_group_index);
  
  ////////////////////////////////////////////////////////////////////////////////
  // 【注意】全部入りと個別は1フレームにどちらかしか使わないようにして下さい。
  ////////////////////////////////////////////////////////////////////////////////
 
  ///////////////////////////
  // 個別
  ///////////////////////////
  void calculateH3dModelBeforeCulling(s32 scene_calculate_group_index, bit32 scene_calculate_group_enable_bit_flag);
  void calculateH3dModelAfterCulling(s32 scene_calculate_group_index, bit32 scene_calculate_group_enable_bit_flag);

  ///////////////////////////
  // カリング
  ///////////////////////////
  void resetH3dCalculateCullingFlag(s32 scene_calculate_group_index, H3dModelPropertyArray* property_array);
  void setH3dCalculateCullingFlag(s32 scene_calculate_group_index, H3dModelProperty* h3d_model_property, b32 not_culling_flag);  // falseのときカリングされてシーンツリーから外れる
#endif  // GFL_GRP_G3D_H3D_USE



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  // SubmitView
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  void submitViewAllModel(SceneNodePropertyPack* property_pack);
  void submitViewAllParticle(SceneNodePropertyPack* property_pack);
#if GFL_GRP_G3D_H3D_USE
  void submitViewAllH3dModel(SceneNodePropertyPack* property_pack);
#endif  // GFL_GRP_G3D_H3D_USE

  void mergeRenderQueueMethodHead(nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B);
  void mergeRenderQueueMethodTail(nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B);
  void mergeRenderQueueMethodSortedMix(nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B);
  void mergeRenderQueueMethodMixNeedSort(SceneDrawGroup* draw_group_A,
      nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B);

  void storeSceneNodeVisible(ModelNodePropertyArray* property_array, s32 scene_draw_group_index);
  void restoreSceneNodeVisible(ModelNodePropertyArray* property_array);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // シーンノードの 追加 / 削除
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // 
  // お勧め関数
  // 
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // old_scene_calculate_group_indexに登録されているobject_typeのold_elementを、scene_calculate_group_indexに移動する
  // 移動できなかったらold_elementのscene_nodeが登録されているシーン描画グループを全て解除する
  b32 moveNodeFromCalcToCalc(void* old_element, ObjectType object_type, s32 old_scene_calculate_group_index, s32 scene_calculate_group_index);
  // scene_calculate_group_indexにscene_nodeを登録する
  // 登録したSceneNodePropertyのポインタをscene_node_propertyに入れて返します。
  // scene_nodeがまだどのシーン計算グループにも登録されていないときに呼ぶこと(それ以外ではうまく動作しません)
  b32 addNodeToCalc(SceneNode* scene_node, s32 scene_calculate_group_index, void** element);
  // old_scene_calculate_group_indexのold_index番目に登録されているscene_nodeを、old_scene_calculate_group_indexと全てのシーン描画グループから解除する
  void removeNodeFromCalcAndAllDraw(void* old_element, ObjectType object_type, s32 old_scene_calculate_group_index);
  // 全シーンノードを取り除く
  void removeAllSceneNodeFromCalcAndAllDraw(SceneNodePropertyArray* property_array, ObjectType object_type, s32 scene_calculate_group_index)
  {
    const SceneNodePropertyElement* sentinel = property_array->GetSentinelElement();
    SceneNodePropertyElement*       element  = property_array->GetElementHead();
    while(element != sentinel)
    {
      SceneNodePropertyElement* temp = element->next;
      removeNodeFromCalcAndAllDraw(element, object_type, scene_calculate_group_index);
      element = temp;
    }
  }
  // 全モデルノードを取り除く
  void removeAllModelNodeFromCalcAndAllDraw(ModelNodePropertyArray* property_array, ObjectType object_type, s32 scene_calculate_group_index)
  {
    const ModelNodePropertyElement* sentinel = property_array->GetSentinelElement();
    ModelNodePropertyElement*       element  = property_array->GetElementHead();
    while(element != sentinel)
    {
      ModelNodePropertyElement* temp = element->next;
      removeNodeFromCalcAndAllDraw(element, object_type, scene_calculate_group_index);
      element = temp;
    }
  }
  // シーン描画グループに登録されている全モデルノードを取り除く
  void removeAllModelNodeFromDraw(ModelNodePropertyArray* property_array, s32 scene_draw_group_index)
  {
    const ModelNodePropertyElement* sentinel = property_array->GetSentinelElement();
    ModelNodePropertyElement*       element  = property_array->GetElementHead();
    while(element != sentinel)
    {
      removeModelNodePropertyFromDraw(&(element->data), scene_draw_group_index);
      element = element->next;
    }
  }
 

  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // 
  // 従来通りの関数
  // 
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  // scene_nodeをscene_calculate_group_indexに登録する。
  // scene_nodeがModelNodePropertyの場合、まだどのシーン計算グループにも登録されていないときはシーン描画グループ0番にも登録する。
  // scene_nodeが既にどれかの他のシーン計算グループに登録されていたら、そのシーン計算グループから取り除いて、scene_calculate_group_indexに登録する。
  // scene_nodeがModelNodePropertyの場合、既にどれか他のシーン計算グループに登録されていたら、シーン描画グループに対しては何も変更をしない(この場合は、
  // シーン描画グループに登録されていない場合はそのまま登録なしにしておく)。
  // scene_nodeがModelNodePropertyの場合、既にどれか他のシーン計算グループに登録されておりそのシーン計算グループから取り除いた後、
  // scene_calculate_group_indexに登録する際にエラーとなったら、scene_nodeが登録されているシーン描画グループを全て解除する。
  // scen_nodeをscene_calculate_group_indexに登録できなかったとき、falseを返す。
  b32 addSceneNodeToSceneCalculateGroup(SceneNode* scene_node, s32 scene_calculate_group_index);
  // scene_nodeをシーン計算グループから取り除く。
  // scene_nodeがModelNodePropertyの場合、scene_nodeが登録されている全てのシーン描画グループから解除する。
  // scene_nodeが既にシーン計算グループに登録されていないときfalseを返す。
  b32 removeSceneNodeFromSceneCalculateGroup(SceneNode* scene_node);
  // 全てのシーン計算グループに登録されている全てのシーンノードに対して、removeSceneNodeFromSceneCalculateGroupを呼んだのと同じ効果のある関数。
  // trueしか返さない。
  // (内部でremoveSceneNodeFromSceneCalculateGroupを使っていないので、全部trueのときtrueを返す、1つでもfalseのときfalseを返す、というふうにはできない)
  b32 removeAllSceneNodeFromSceneCalculateGroup(void);

  // scene_nodeをscene_calculate_group_index、scene_draw_group_indexに登録する。
  // scene_nodeはModelNodePropertyでなければならない。
  // scene_nodeがまだどのシーン計算グループにも登録されていないときはシーン描画グループはscene_draw_group_indexにだけ登録されることになる。
  // scene_nodeが既にどれかの他のシーン計算グループに登録されていたら、そのシーン計算グループから取り除いて、scene_calculate_group_indexに登録する。
  // scene_nodeが既にどれか他のシーン計算グループに登録されていたら、シーン描画グループはそれまでのシーン描画グループにscene_draw_group_indexを
  // 加えることになる(どのシーン描画グループにも登録されていない場合は勿論scene_draw_group_indexにだけ登録されることになる)。
  // scene_nodeが既にどれか他のシーン計算グループに登録されておりそのシーン計算グループから取り除いた後、
  // scene_calculate_group_indexに登録する際にエラーとなったら、scene_nodeが登録されているシーン描画グループを全て解除する。
  // scen_nodeをscene_calculate_group_indexに登録できなかったとき、falseを返す。
  b32 addModelNodeToSceneCalculateGroupAndSceneDrawGroup(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index);

  // scene_nodeを全てのシーン描画グループに加える。
  // scene_nodeはModelNodePropertyでなければならない。
  // scene_nodeがまだどのシーン計算グループにも登録されていないときはシーン計算グループ0番に登録される。
  // scene_nodeがまだどのシーン計算グループにも登録されていないときに、シーン計算グループ0番に登録できなかったときfalseを返す。
  b32 addModelNodeToAllSceneDrawGroup(SceneNode* scene_node);
  // scene_nodeをこれまで加えられていたシーン描画グループはそのままにscene_draw_group_indexに加える。
  // scene_nodeはModelNodePropertyでなければならない。
  // scene_nodeがまだどのシーン計算グループにも登録されていないときはシーン計算グループ0番に登録される。
  // scene_nodeがまだどのシーン計算グループにも登録されていないときに、シーン計算グループ0番に登録できなかったときfalseを返す。
  b32 addModelNodeToSceneDrawGroup(SceneNode* scene_node, s32 scene_draw_group_index);
  // scene_nodeを全てのシーン描画グループから取り除く。
  // scene_nodeはModelNodePropertyでなければならない。
  // シーン計算グループに対しては何もしない。
  // scene_nodeがどのシーン計算グループにも登録されていないときはfalseを返す。
  // scene_nodeがシーン描画グループに登録されていないだけのときはfalseにならない。
  b32 removeModelNodeFromAllSceneDrawGroup(SceneNode* scene_node);
  // scene_nodeをscene_draw_group_indexから取り除く。
  // 他のシーン描画グループにも登録されている場合、他のものへの登録に対しては何もしない。
  // scene_nodeはModelNodePropertyでなければならない。
  // scene_nodeがどのシーン計算グループにも登録されていないときはfalseを返す。
  // scene_nodeがscene_draw_group_indexに登録されていないだけのときはfalseにならない。
  b32 removeModelNodeFromSceneDrawGroup(SceneNode* scene_node, s32 scene_draw_group_index);


 

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  // カメラ設定
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  void assignNotSetCameraIndex( Camera* camera );
  s32  getNotSetCameraIndex(void);
  s32  getFirstAssignedCameraIndex(void);
  // setupNotSetCameraとcleanupNotSetCameraは必ず対で使用し同じ引数を渡して下さい。
  void setupNotSetCamera(Camera* camera, s32 scene_draw_group_index, b32* is_assign_camera_index, b32* is_set_active_camera_index);
  void cleanupNotSetCamera(Camera* camera, s32 scene_draw_group_index, const b32 is_assign_camera_index, const b32 is_set_active_camera_index);



  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  // 取得
  //
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  // 指定したシーンノードmodel, light, camera, fog, particleがどこに入っているか取得する。
  // 見付けられたらtrueを返す。
  // 見付けられなかったらfalseを返し、引数戻り値に入る値は不定となる。 

  // @param[out] scene_calculate_group_index 全シーン計算グループから探し、見付けたシーン計算グループのインデックスを入れて返す。
  // @param[out] object_type                 ObjectTypeを入れて返す。
  // @retval                                 見付けられなかったらNULLを返し、
  //                                         見付けられたらmodel, light, camera_fog, particleが入っているリストの要素のポインタを返す。 
  //                                         light, camera, fogのときは戻り値を(SceneNodePropertyElement*)にキャストして使用し、
  //                                         model, particleのときは戻り値を(ModelNodePropertyElement*)にキャストして使用する。
  void* getNodeElement(const SceneNode* scene_node, s32* scene_calculate_group_index, ObjectType* object_type); 
  ModelNodePropertyElement* getModelElement(const Model* scene_node, s32* scene_calculate_group_index); 
  SceneNodePropertyElement* getLightElement(const Light* scene_node, s32* scene_calculate_group_index); 
  SceneNodePropertyElement* getCameraElement(const Camera* scene_node, s32* scene_calculate_group_index); 
  SceneNodePropertyElement* getFogElement(const Fog* scene_node, s32* scene_calculate_group_index); 
  ModelNodePropertyElement* getParticleElement(const Particle* scene_node, s32* scene_calculate_group_index); 
#if GFL_GRP_G3D_H3D_USE
  H3dModelPropertyElement* getH3dModelElement(const H3dModel* scene_node, s32* scene_calculate_group_index);
#endif  // GFL_GRP_G3D_H3D_USE


  // @param[in]  scene_node_property_pack    このパックからだけ探す。
  // @param[out] scene_node_property         model, light, camera_fog, particleが入っているシーンノードのプロパティのポインタを入れて返す。
  // @param[out] object_type                 ObjectTypeを入れて返す。
  // @retval                                 見付けられなかったらNULLを返し、
  //                                         見付けられたらmodel, light, camera_fog, particleが入っているリストの要素のポインタを返す。 
  //                                         light, camera, fogのときは戻り値を(SceneNodePropertyElement*)にキャストして使用し、
  //                                         model, particleのときは戻り値を(ModelNodePropertyElement*)にキャストして使用する。
  void* getNodeElement(const SceneNode* scene_node, const SceneNodePropertyPack* scene_node_property_pack, ObjectType* object_type); 
  ModelNodePropertyElement* getModelElement(const Model* scene_node, const SceneNodePropertyPack* scene_node_property_pack); 
  SceneNodePropertyElement* getLightElement(const Light* scene_node, const SceneNodePropertyPack* scene_node_property_pack); 
  SceneNodePropertyElement* getCameraElement(const Camera* scene_node, const SceneNodePropertyPack* scene_node_property_pack); 
  SceneNodePropertyElement* getFogElement(const Fog* scene_node, const SceneNodePropertyPack* scene_node_property_pack); 
  ModelNodePropertyElement* getParticleElement(const Particle* scene_node, const SceneNodePropertyPack* scene_node_property_pack); 
#if GFL_GRP_G3D_H3D_USE
  H3dModelPropertyElement* getH3dModelElement(const H3dModel* scene_node, const SceneNodePropertyPack* scene_node_property_pack);
#endif  // GFL_GRP_G3D_H3D_USE


  // @param[in]  scene_calculate_group_index このシーン計算グループからだけ探す。
  // @param[out] object_type                 ObjectTypeを入れて返す。
  // @retval                                 見付けられなかったらNULLを返し、
  //                                         見付けられたらmodel, light, camera_fog, particleが入っているリストの要素のポインタを返す。 
  //                                         light, camera, fogのときは戻り値を(SceneNodePropertyElement*)にキャストして使用し、
  //                                         model, particleのときは戻り値を(ModelNodePropertyElement*)にキャストして使用する。
  void* getNodeElement(const SceneNode* scene_node, const s32 scene_calculate_group_index, ObjectType* object_type); 
  ModelNodePropertyElement* getModelElement(const Model* scene_node, const s32 scene_calculate_group_index); 
  SceneNodePropertyElement* getLightElement(const Light* scene_node, const s32 scene_calculate_group_index); 
  SceneNodePropertyElement* getCameraElement(const Camera* scene_node, const s32 scene_calculate_group_index); 
  SceneNodePropertyElement* getFogElement(const Fog* scene_node, const s32 scene_calculate_group_index); 
  ModelNodePropertyElement* getParticleElement(const Particle* scene_node, const s32 scene_calculate_group_index); 
#if GFL_GRP_G3D_H3D_USE
  H3dModelPropertyElement* getH3dModelElement(const H3dModel* scene_node, const s32 scene_calculate_group_index); 
#endif  // GFL_GRP_G3D_H3D_USE

  
  // 先頭要素を得る
  inline ModelNodePropertyElement* getModelElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline SceneNodePropertyElement* getLightElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline SceneNodePropertyElement* getCameraElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline SceneNodePropertyElement* getFogElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline ModelNodePropertyElement* getParticleElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
#if GFL_GRP_G3D_H3D_USE
  inline H3dModelPropertyElement* getH3dModelElementHead(const SceneNodePropertyPack* scene_node_property_pack) const;
#endif  // GFL_GRP_G3D_H3D_USE

  inline ModelNodePropertyElement* getModelElementHead(const s32 scene_calculate_group_index) const;
  inline SceneNodePropertyElement* getLightElementHead(const s32 scene_calculate_group_index) const;
  inline SceneNodePropertyElement* getCameraElementHead(const s32 scene_calculate_group_index) const;
  inline SceneNodePropertyElement* getFogElementHead(const s32 scene_calculate_group_index) const;
  inline ModelNodePropertyElement* getParticleElementHead(const s32 scene_calculate_group_index) const;
#if GFL_GRP_G3D_H3D_USE
  inline H3dModelPropertyElement* getH3dModelElementHead(const s32 scene_calculate_group_index) const;
#endif  // GFL_GRP_G3D_H3D_USE
  

  // 番兵要素を得る
  inline const ModelNodePropertyElement* getModelSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline const SceneNodePropertyElement* getLightSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline const SceneNodePropertyElement* getCameraSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline const SceneNodePropertyElement* getFogSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
  inline const ModelNodePropertyElement* getParticleSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
#if GFL_GRP_G3D_H3D_USE
  inline const H3dModelPropertyElement* getH3dModelSentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const;
#endif  // GFL_GRP_G3D_H3D_USE

  inline const ModelNodePropertyElement* getModelSentinelElement(const s32 scene_calculate_group_index) const;
  inline const SceneNodePropertyElement* getLightSentinelElement(const s32 scene_calculate_group_index) const;
  inline const SceneNodePropertyElement* getCameraSentinelElement(const s32 scene_calculate_group_index) const;
  inline const SceneNodePropertyElement* getFogSentinelElement(const s32 scene_calculate_group_index) const;
  inline const ModelNodePropertyElement* getParticleSentinelElement(const s32 scene_calculate_group_index) const;
#if GFL_GRP_G3D_H3D_USE
  inline const H3dModelPropertyElement* getH3dModelSentinelElement(const s32 scene_calculate_group_index) const;
#endif  // GFL_GRP_G3D_H3D_USE
 


  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 
  // デバッグ機能
  // 
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
private:
  void debugTickDumpAndReset(const char* message);  // messageは最後の改行なしでOK
};




}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSCENE_H__
