#ifndef __GFL_G3DSHADOWGROUP_H__
#define __GFL_G3DSHADOWGROUP_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dShadowGroup.h
 *	@brief  3D影グループ
 *	@author	Koji Kawada
 *	@date		2011.03.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp
#include <grp/gfl_RenderTarget.h>

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
///	3D影グループクラス
//=====================================
class ShadowGroup
{
  GFL_FORBID_COPY_AND_ASSIGN(ShadowGroup);

  
  // 下記のfriendからはprivateメンバ関数にアクセスしてよい
  friend class ShadowSystem;




public:
  //---------------------------------------------------------------------------
  /**
   *           定数宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	影のタイプ
  //=====================================
  enum Type
  {
    TYPE_NONE,                 // なし
    TYPE_PROJECTION_SHADOW,    // プロジェクションシャドウ
    TYPE_SHADOW_MAP            // シャドウマップ
  };




  //---------------------------------------------------------------------------
  /**
   *           構造体宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	設定内容
  //=====================================
  struct Description
  {
    Type                                   type;                       // 影のタイプ
    s32                                    scene_draw_group_index;     // シーン描画グループ
    s32                                    camera_index;               // カメラに割り振るインデックス
    gfl::grp::g3d::Camera::Description*    camera_description;         // カメラの設定内容
    gfl::grp::RenderTarget::Description*   render_target_description;  // レンダーターゲットの設定内容

    //-----------------------------------------------------------------------------
    /**
     *  @brief         コンストラクタ
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : type                          (TYPE_NONE),
        scene_draw_group_index        (SCENE_DRAW_GROUP_INDEX_NONE),
        camera_index                  (CAMERA_INDEX_NONE),
        camera_description            (NULL), 
        render_target_description     (NULL)
    {}
  };




  // 以下のメンバ関数はthis及びfriendからしかアクセスしません
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  ShadowGroup(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createを呼びます。
   */
  //-----------------------------------------------------------------------------
  ShadowGroup(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const Description*      description = NULL);
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~ShadowGroup();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   */
  //-----------------------------------------------------------------------------
  void Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const Description*      description = NULL);
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         取得
   */
  //-----------------------------------------------------------------------------
  Type                          GetType(void) const                { return m_type; }
  s32                           GetSceneDrawGroupIndex(void) const { return m_scene_draw_group_index; }
  s32                           GetCameraIndex(void) const         { return m_camera_index; }
  gfl::grp::g3d::Camera*        GetCamera(void) const              { return m_camera; }
  gfl::grp::RenderTarget*       GetRenderTarget(void) const        { return m_render_target; }



  
  // 以下のメンバ変数はthisからしかアクセスしません
private:
  Type                          m_type;
  s32                           m_scene_draw_group_index;
  s32                           m_camera_index;
  gfl::grp::g3d::Camera*        m_camera;
  gfl::grp::RenderTarget*       m_render_target;
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSHADOWGROUP_H__
