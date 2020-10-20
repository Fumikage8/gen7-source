//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DebugG3dCameraChase.cpp
 *	@brief  3Dカメラ追跡型
 *	@author	Koji Kawada
 *	@date		2011.02.21
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

#include "debug/gfl_DebugG3dCameraChase.h"

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
void DebugG3dCameraChase::Create(
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
    GFL_PRINT("Cross Key : rotate\n");
    GFL_PRINT("        L : near\n");
    GFL_PRINT("        R : far\n");
    GFL_PRINT("        X : up\n");
    GFL_PRINT("        Y : down\n");
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
void DebugG3dCameraChase::Create(
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
    GFL_PRINT("Cross Key : rotate\n");
    GFL_PRINT("        L : near\n");
    GFL_PRINT("        R : far\n");
    GFL_PRINT("        X : up\n");
    GFL_PRINT("        Y : down\n");
  }
  // 独自処理 ここまで
}

//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
 */
//-----------------------------------------------------------------------------
void DebugG3dCameraChase::Destroy(void)
{
  gfl::grp::g3d::Camera::Destroy();

  m_initial_rotate           = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_initial_target_pos       = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_initial_target_distance  = DEFAULT_DISTANCE;
  m_initial_target_offset_y  = 0.0f;
  m_rotate                   = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_target_pos               = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_target_distance          = DEFAULT_DISTANCE;
  m_target_offset_y          = 0.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         操作
 *                 呼んだフレームの操作が反映される。毎フレーム呼べば毎フレーム操作可能。
 *
 *  @param[in]     device_manager    デバイスマネージャー
 *  @param[in]     target_position   カメラの注視点の位置
 */
//-----------------------------------------------------------------------------
void DebugG3dCameraChase::Control(const gfl::ui::DeviceManager* device_manager, const gfl::math::VEC3& target_position )
{
  const gfl::ui::VectorDevice* stick     = device_manager->GetStick(0);     // アナログスティック
  const gfl::ui::VectorDevice* cross_key = device_manager->GetCrossKey(0);  // 十字キー
  const gfl::ui::Button*       button    = device_manager->GetButton(0);    // ボタン

  if( button->IsHold( gfl::ui::BUTTON_X ) )
  {
    f32 move_speed = MOVE_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_offset_y += move_speed;
  }
  else if( button->IsHold( gfl::ui::BUTTON_Y ) )
  {
    f32 move_speed = MOVE_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_offset_y -= move_speed;
  }

  else if( button->IsHold( gfl::ui::BUTTON_L ) )
  {
    f32 dolly_speed = DOLLY_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_distance -= dolly_speed;
    if( m_target_distance < MIN_DISTANCE ) m_target_distance = MIN_DISTANCE; 
  }
  else if( button->IsHold( gfl::ui::BUTTON_R ) )
  {
    f32 dolly_speed = DOLLY_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_distance += dolly_speed;
  }

  else if( cross_key->IsHold( gfl::ui::DIR_UP ) )
  {
    m_rotate.x -= 1.0f * TUMBLE_SPEED;
    m_rotate.x = gfl::math::Clamp(m_rotate.x, -ROTATE_X_LIMIT, ROTATE_X_LIMIT);
  }
  else if( cross_key->IsHold( gfl::ui::DIR_DOWN ) )
  {
    m_rotate.x += 1.0f * TUMBLE_SPEED;
    m_rotate.x = gfl::math::Clamp(m_rotate.x, -ROTATE_X_LIMIT, ROTATE_X_LIMIT);
  }
  else if( cross_key->IsHold( gfl::ui::DIR_LEFT ) )
  {
    m_rotate.y += 1.0f * TUMBLE_SPEED;
  }
  else if( cross_key->IsHold( gfl::ui::DIR_RIGHT ) )
  {
    m_rotate.y -= 1.0f * TUMBLE_SPEED;
  }
  
  Control(target_position);
}

void DebugG3dCameraChase::Control( const gfl::math::VEC3& target_position )
{
  m_initial_target_pos = m_target_pos = target_position;
  
  UpdateDebugCameraFromParameter(); 
}

//-----------------------------------------------------------------------------
/*
 *  @brief        カメラの位置を設定する
 *
 *  @param[in]    camera_position    カメラの位置
 */
//-----------------------------------------------------------------------------
void DebugG3dCameraChase::SetCameraPosition( const gfl::math::VEC3& camera_position )
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
void DebugG3dCameraChase::SetTargetPosition( const gfl::math::VEC3& target_position )
{
  gfl::grp::g3d::Camera::SetTargetPosition( target_position );
  
  UpdateDebugCamera();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         パラメータ出力
 */
//-----------------------------------------------------------------------------
void DebugG3dCameraChase::PrintParameter(void) const
{
//  GFL_PRINT("m_initial_rotate=(%f, %f, %f)\n", m_initial_rotate.x, m_initial_rotate.y, m_initial_rotate.z);
//  GFL_PRINT("m_initial_target_pos=(%f, %f, %f)\n", m_initial_target_pos.x, m_initial_target_pos.y, m_initial_target_pos.z);
//  GFL_PRINT("m_initial_target_distance=%f\n", m_initial_target_distance);
//  GFL_PRINT("m_initial_target_offset_y=%f\n", m_initial_target_offset_y);
  GFL_PRINT("m_rotate=(%f, %f, %f)\n", m_rotate.x, m_rotate.y, m_rotate.z);
//  GFL_PRINT("m_target_pos=(%f, %f, %f)\n", m_target_pos.x, m_target_pos.y, m_target_pos.z);
  GFL_PRINT("m_target_distance=%f\n", m_target_distance);
  GFL_PRINT("m_target_offset_y=%f\n", m_target_offset_y);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         パラメータの取得/設定
 */
//-----------------------------------------------------------------------------
void DebugG3dCameraChase::SetParameterRotate(const gfl::math::VEC3& rotate)
{
  m_initial_rotate = m_rotate = rotate;
  UpdateDebugCameraFromParameter();
}
void DebugG3dCameraChase::GetParameterRotate(gfl::math::VEC3* rotate) const
{
  *rotate = m_rotate;
}

void DebugG3dCameraChase::SetParameterTargetPos(const gfl::math::VEC3& pos)
{
  m_initial_target_pos = m_target_pos = pos;
  UpdateDebugCameraFromParameter();
}
void DebugG3dCameraChase::GetParameterTargetPos(gfl::math::VEC3* pos) const
{
  *pos = m_target_pos;
}

void DebugG3dCameraChase::SetParameterTargetDistance(const f32 distance)
{
  m_initial_target_distance = m_target_distance = distance;
  UpdateDebugCameraFromParameter();
}
f32  DebugG3dCameraChase::GetParameterTargetDistance(void) const
{
  return m_target_distance;
}

void DebugG3dCameraChase::SetParameterTargetOffsetY(const f32 y)
{
  m_initial_target_offset_y = m_target_offset_y = y;
  UpdateDebugCameraFromParameter();
}
f32  DebugG3dCameraChase::GetParameterTargetOffsetY(void) const
{
  return m_target_offset_y;
}

//-----------------------------------------------------------------------------
/*
 *  @brief        カメラの位置と注視点の位置を設定する
 *
 *  @param[in]    camera_position    カメラの位置
 *  @param[in]    target_position    カメラの注視点の位置
 */
//-----------------------------------------------------------------------------
void DebugG3dCameraChase::SetCameraAndTargetPosition( const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position )
{
  gfl::grp::g3d::Camera::SetCameraAndTargetPosition( camera_position, target_position );
  
  UpdateDebugCamera();
}

//-----------------------------------------------------------------------------
/*
 *  @brief        現在設定されているカメラの位置と注視点の位置をデバッグカメラに反映する
 */
//-----------------------------------------------------------------------------
void DebugG3dCameraChase::UpdateDebugCamera(void)
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
  m_initial_target_offset_y = m_target_offset_y  = 0.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief        現在設定されているパラメータでデバッグカメラを更新する
 */
//-----------------------------------------------------------------------------
void DebugG3dCameraChase::UpdateDebugCameraFromParameter(void)
{
  {
    gfl::math::VEC3 target_pos( m_target_pos.x, m_target_pos.y + m_target_offset_y, m_target_pos.z );
    gfl::grp::g3d::Camera::SetTargetPosition(target_pos);

    f32 xs = gfl::math::SinFIdx(m_rotate.x);
    f32 xc = gfl::math::CosFIdx(m_rotate.x);
    f32 ys = gfl::math::SinFIdx(m_rotate.y);
    f32 yc = gfl::math::CosFIdx(m_rotate.y);
    gfl::math::VEC3 camera_pos(
      target_pos.x - m_target_distance * xc * ys,
      target_pos.y - m_target_distance * xs,
      target_pos.z + m_target_distance * xc * yc
    );
    gfl::grp::g3d::Camera::SetCameraPosition(camera_pos);
  }
}

}  // namespace debug
}  // namespace gfl

