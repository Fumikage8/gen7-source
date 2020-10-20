//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DebugG3dCamera.cpp
 *	@brief  3Dカメラ
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include "gfl_Heap.h"
#include "gfl_Macros.h"
#include "gfl_Math.h"

#include "ui/gfl_UI_DeviceManager.h"
#include "ui/gfl_UI_VectorDevice.h"
#include "ui/gfl_UI_Button.h"
#include "ui/ctr/gfl_UI_CTR_DeviceManager.h"

#include "debug/gfl_Assert.h"
#include "debug/gfl_DebugPrint.h"

#include "debug/gfl_DebugG3dCamera.h"

// gflib grp g3d
#include "grp/g3d/gfl_G3dResource.h"
#include "grp/g3d/gfl_G3dCamera.h"

namespace gfl {
namespace debug {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dカメラクラス
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         生成
 *
 *  @param[in]     allocator          アロケータ
 *  @param[in]     device_allocator   デバイスメモリのアロケータ
 *  @param[in]     resource           リソース
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 */
//-----------------------------------------------------------------------------
void DebugG3dCamera::Create(
    gfl::heap::NwAllocator*           allocator,
    gfl::heap::NwAllocator*           device_allocator,
    gfl::grp::g3d::Resource*          resource,
    s32                               index_in_resource
)
{
  GFL_UNUSED(allocator);

  if(    allocator         == NULL )
  {
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResCamera res = nw_res_graphics_file->GetCameras(index_in_resource);
    
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      false,
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      0
  );
  GFL_ASSERT_STOP(scene_object);

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // 独自処理 ここから
  UpdateDebugCamera();
  {
    GFL_PRINT("[camera controller usage]\n");
    GFL_PRINT("stick     : rotation\n");
    GFL_PRINT("stick + a : forward / backward\n");
    GFL_PRINT("stick + b : translation\n");
    GFL_PRINT("stick + y : upward / downward\n");
    GFL_PRINT("x         : reset\n");
  }
  // 独自処理 ここまで
}

//-----------------------------------------------------------------------------
/*
 *  @brief        生成
 * 
 *  @param[in]    allocator          生成に使用するアロケーター
 *  @param[in]    camera_position    カメラの位置座標
 *  @param[in]    target_position    カメラターゲットの座標
 *  @param[in]    near_clip          ニアクリップの値
 *  @param[in]    far_clip           ファークリップの値
 *  @param[in]    fovy_radian        FOVのY方向の値
 *  @param[in]    pivot_direction    画面の上方向
 *  @param[in]    w_scale            WScaleの値
 *
 *  @retval       DebugG3dCamera*  生成したカメラクラス
 */
//-----------------------------------------------------------------------------
void DebugG3dCamera::Create(
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3&           camera_position,
    const gfl::math::VEC3&           target_position,
    f32                              near_clip,
    f32                              far_clip,
    f32                              fovy_radian,
    gfl::math::PivotDirection        pivot_direction,
    f32                              w_scale
)
{
  nw::gfx::Camera* nw_camera = Camera::CreateCamera(
    allocator,
    camera_position,
    target_position,
    near_clip,
    far_clip,
    fovy_radian,
    pivot_direction,
    w_scale
  );

  SetNwSceneNode(nw_camera);

  // 独自処理 ここから
  UpdateDebugCamera();
  {
    GFL_PRINT("[camera controller usage]\n");
    GFL_PRINT("stick     : rotation\n");
    GFL_PRINT("stick + a : forward / backward\n");
    GFL_PRINT("stick + b : translation\n");
    GFL_PRINT("stick + y : upward / downward\n");
    GFL_PRINT("x         : reset\n");
  }
  // 独自処理 ここまで
}

//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
 */
//-----------------------------------------------------------------------------
void DebugG3dCamera::Destroy(void)
{
  gfl::grp::g3d::Camera::Destroy();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         操作
 *                 呼んだフレームの操作が反映される。毎フレーム呼べば毎フレーム操作可能。
 *
 *  @param[in]     device_manager    デバイスマネージャー
 */
//-----------------------------------------------------------------------------
void DebugG3dCamera::Control(const gfl::ui::DeviceManager* device_manager)
{
  const gfl::ui::VectorDevice* stick  = device_manager->GetStick(0);   // アナログスティック
  const gfl::ui::Button*       button = device_manager->GetButton(0);  // ボタン

  if( button->IsHold( gfl::ui::BUTTON_X ) )
  {
    m_rotate            = m_initial_rotate;
    m_target_pos        = m_initial_target_pos;
    m_target_distance   = m_initial_target_distance;
  }
  else if( button->IsHold( gfl::ui::BUTTON_A ) )
  {
    f32 dolly_speed = DOLLY_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_distance -= stick->GetY() * dolly_speed;
    if( m_target_distance < MIN_DISTANCE ) m_target_distance = MIN_DISTANCE; 
  }
  else if( button->IsHold( gfl::ui::BUTTON_B ) )
  {
    f32 move_speed = MOVE_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));

    /*
    gfl::math::VEC3 d_screen(stick->GetX() * move_speed, 0.0f, -stick->GetY() * move_speed);
    gfl::math::VEC3 d_world;
    gfl::math::MTX34 screen_to_world;

    gfl::math::MTX34RotXYZFIdx(&screen_to_world, GFL_MATH_DEG_TO_FIDX(0.0f), GFL_MATH_DEG_TO_FIDX(m_rotate.y), GFL_MATH_DEG_TO_FIDX(m_rotate.z));
    nw::math::VEC3TransformNormal(&d_world, &screen_to_world, &d_screen);
    
    m_target_pos.x += d_world.x;
    m_target_pos.z += d_world.z;
    */

    gfl::math::VEC3 camera_pos;
    GetCameraPosition(camera_pos);
    gfl::math::VEC3 dir;
    gfl::math::VEC3Sub(&dir, &m_target_pos, &camera_pos);
    gfl::math::VEC3Normalize(&dir, &dir);

    gfl::math::VEC3 up(0.0f, 1.0f, 0.0f);
    gfl::math::VEC3 cross;
    gfl::math::VEC3Cross(&cross, &up, &dir);
    gfl::math::VEC3Normalize(&cross, &cross);

    gfl::math::VEC3 front_back;
    gfl::math::VEC3 left_right;
    gfl::math::VEC3Scale(&front_back, &dir, stick->GetY() * move_speed);
    gfl::math::VEC3Scale(&left_right, &cross, -stick->GetX() * move_speed);

    gfl::math::VEC3Add(&m_target_pos, &m_target_pos, &front_back);
    gfl::math::VEC3Add(&m_target_pos, &m_target_pos, &left_right);
  }
  else if( button->IsHold( gfl::ui::BUTTON_Y ) )
  {
    f32 move_speed = MOVE_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_pos.y += stick->GetY() * move_speed;
  }
  else
  {
    m_rotate.x -= stick->GetY() * TUMBLE_SPEED;
    m_rotate.y -= stick->GetX() * TUMBLE_SPEED;
    m_rotate.x = gfl::math::Clamp(m_rotate.x, -ROTATE_X_LIMIT, ROTATE_X_LIMIT);
  } 
  
  {
    gfl::grp::g3d::Camera::SetTargetPosition(m_target_pos);

    f32 xs = gfl::math::SinFIdx(m_rotate.x);
    f32 xc = gfl::math::CosFIdx(m_rotate.x);
    f32 ys = gfl::math::SinFIdx(m_rotate.y);
    f32 yc = gfl::math::CosFIdx(m_rotate.y);
    gfl::math::VEC3 camera_pos(
      m_target_pos.x - m_target_distance * xc * ys,
      m_target_pos.y - m_target_distance * xs,
      m_target_pos.z + m_target_distance * xc * yc
    );
    gfl::grp::g3d::Camera::SetCameraPosition(camera_pos);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief        カメラの位置を設定する
 *
 *  @param[in]    camera_position    カメラの位置
 */
//-----------------------------------------------------------------------------
void DebugG3dCamera::SetCameraPosition( const gfl::math::VEC3& camera_position )
{
  gfl::grp::g3d::Camera::SetCameraPosition( camera_position );

  UpdateDebugCamera();
}

//-----------------------------------------------------------------------------
/*
 *  @brief        カメラの注視点の位置を設定する
 *
 *  @param[in]    target_position    カメラの注視点の位置
 */
//-----------------------------------------------------------------------------
void DebugG3dCamera::SetTargetPosition( const gfl::math::VEC3& target_position )
{
  gfl::grp::g3d::Camera::SetTargetPosition( target_position );
  
  UpdateDebugCamera();
}

//-----------------------------------------------------------------------------
/*
 *  @brief        カメラの位置と注視点の位置を設定する
 *
 *  @param[in]    camera_position    カメラの位置
 *  @param[in]    target_position    カメラの注視点の位置
 */
//-----------------------------------------------------------------------------
void DebugG3dCamera::SetCameraAndTargetPosition( const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position )
{
  gfl::grp::g3d::Camera::SetCameraAndTargetPosition( camera_position, target_position );
  
  UpdateDebugCamera();
}

//-----------------------------------------------------------------------------
/*
 *  @brief        現在設定されているカメラの位置と注視点の位置をデバッグカメラに反映する
 */
//-----------------------------------------------------------------------------
void DebugG3dCamera::UpdateDebugCamera(void)
{
  gfl::math::VEC3 camera_pos;
  gfl::math::VEC3 rotate;
  gfl::math::VEC3 target_pos;
  f32             target_distance;
  
  GetCameraPosition(camera_pos);
  GetTargetPosition(target_pos);

  target_distance = gfl::math::VEC3Dist(&target_pos, &camera_pos);
  GFL_ASSERT( target_distance >= MIN_DISTANCE );  // @note 除数が0に近いときを考慮
  if( target_distance < MIN_DISTANCE ) target_distance = MIN_DISTANCE; 

  gfl::math::VEC3 look_reverse = (camera_pos - target_pos) / target_distance;
  rotate.x = gfl::math::AsinFIdx(-look_reverse.y);
  rotate.y = gfl::math::Atan2FIdx(-look_reverse.x, look_reverse.z);
  rotate.z = 0.0f;

  m_initial_rotate          = m_rotate           = rotate;
  m_initial_target_pos      = m_target_pos       = target_pos;
  m_initial_target_distance = m_target_distance  = target_distance;
}

}  // namespace debug
}  // namespace gfl

