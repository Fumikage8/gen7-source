//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DebugG3dStereoCamera.cpp
 *	@brief  デバッグ3Dステレオカメラ
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
#include <gfl_Math.h>

#include <ui/gfl_UI_DeviceManager.h>
#include <ui/gfl_UI_VectorDevice.h>
#include <ui/gfl_UI_Button.h>
#include <ui/ctr/gfl_UI_CTR_DeviceManager.h>

#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

#include <debug/gfl_DebugG3dStereoCamera.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dStereoCamera.h>

namespace gfl {
namespace debug {

//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	デバッグ3Dステレオカメラクラス
//=====================================

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
 *  @param[in]    depth_level        ベースカメラから基準面までの距離
 *  @param[in]    depth_range        視差の補正値
 *  @param[in]    real_switch        現実感を出すために視野角の変更を許すか(trueのとき許す)
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCamera::Create(
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3&           camera_position,
    const gfl::math::VEC3&           target_position,
    f32                              near_clip,
    f32                              far_clip,
    f32                              fovy_radian,
    f32                              w_scale,
    f32                              depth_level,
    f32                              depth_range,
    b32                              real_switch
)
{
  gfl::grp::g3d::StereoCamera::Create(
    allocator,
    camera_position,
    target_position,
    near_clip,
    far_clip,
    fovy_radian,
    w_scale,
    depth_level,
    depth_range,
    real_switch
  );

  // 独自処理 ここから
  UpdateDebugCamera();
  {
    GFL_PRINT("[camera controller usage]\n");
    GFL_PRINT("stick     : rotation\n");
    GFL_PRINT("stick + a : forward / backward\n");
    GFL_PRINT("stick + b : translation\n");
    GFL_PRINT("stick + y : upward / downward\n");
    GFL_PRINT("x         : reset\n");
    GFL_PRINT("start     : print log\n");
  }
  // 独自処理 ここまで
}

//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCamera::Destroy(void)
{
  gfl::grp::g3d::StereoCamera::Destroy();

  m_initial_rotate           = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_initial_target_pos       = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_initial_target_distance  = DEFAULT_DISTANCE;
  m_rotate                   = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_target_pos               = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
  m_target_distance          = DEFAULT_DISTANCE;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         操作
 *                 呼んだフレームの操作が反映される。毎フレーム呼べば毎フレーム操作可能。
 *
 *  @param[in]     device_manager    デバイスマネージャー
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCamera::Control(const gfl::ui::DeviceManager* device_manager, u32 flag)
{
  const gfl::ui::VectorDevice* stick  = device_manager->GetStick(0);   // アナログスティック
  const gfl::ui::Button*       button = device_manager->GetButton(0);  // ボタン

  if( button->IsHold( gfl::ui::BUTTON_X ) && (flag & FLAG_CONT_RESET) )
  {
    m_rotate            = m_initial_rotate;
    m_target_pos        = m_initial_target_pos;
    m_target_distance   = m_initial_target_distance;
  }
  else if( button->IsHold( gfl::ui::BUTTON_A ) && (flag & FLAG_CONT_FORWARD_BACK) )
  {
    f32 dolly_speed = DOLLY_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_distance -= stick->GetY() * dolly_speed;
    if( m_target_distance < MIN_DISTANCE ) m_target_distance = MIN_DISTANCE; 
  }
  else if( button->IsHold( gfl::ui::BUTTON_B ) && (flag & FLAG_CONT_TRANSLATE) )
  {
    f32 move_speed = MOVE_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));

    /*
    gfl::math::VEC3 d_screen(stick->GetX() * move_speed, 0.0f, -stick->GetY() * move_speed);
    gfl::math::VEC3 d_world;
    gfl::math::MTX34 screen_to_world;

    gfl::math::MTX34RotXYZFIdx(&screen_to_world, GFL_MATH_DEG_TO_FIDX(0.0f), GFL_MATH_DEG_TO_FIDX(m_rotate.y), GFL_MATH_DEG_TO_FIDX(m_rotate.z));
    nw::math::VEC3TransformNormal(&d_world, &screen_to_world, &d_screen);  //
    
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
  else if( button->IsHold( gfl::ui::BUTTON_B ) && (flag & FLAG_CONT_TRANSLATE_XZ) )
  {
    f32 move_speed = MOVE_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));

    gfl::math::VEC3 camera_pos;
    GetCameraPosition(camera_pos);
    gfl::math::VEC3 dir;
    gfl::math::VEC3Sub(&dir, &m_target_pos, &camera_pos);
    dir.y = 0.0f;
    if( dir.LengthSquare() > 0.0f ){
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
  }
  else if( button->IsHold( gfl::ui::BUTTON_Y ) && (flag & FLAG_CONT_UP_DOWN) )
  {
    f32 move_speed = MOVE_SPEED_BASE * gfl::math::FSqrt(gfl::math::FAbs(m_target_distance));
    m_target_pos.y += stick->GetY() * move_speed;
  }
  else if( (flag & FLAG_CONT_ROTATE) )
  {
    m_rotate.x -= stick->GetY() * TUMBLE_SPEED;
    m_rotate.y -= stick->GetX() * TUMBLE_SPEED;
    m_rotate.x = gfl::math::Clamp(m_rotate.x, -ROTATE_X_LIMIT, ROTATE_X_LIMIT);
  } 
 
  
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

  if( button->IsTrigger( gfl::ui::BUTTON_START ) )
  {
    GFL_PRINT("[camera controller infomation]\n");
    GFL_PRINT("camera_pos X=%f Y=%f Z%f\n", camera_pos.x, camera_pos.y, camera_pos.z);
    GFL_PRINT("target_pos X=%f Y=%f Z%f\n", m_target_pos.x, m_target_pos.y, m_target_pos.z);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief        カメラの位置を設定する
 *
 *  @param[in]    camera_position    カメラの位置
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCamera::SetCameraPosition( const gfl::math::VEC3& camera_position )
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
void DebugG3dStereoCamera::SetTargetPosition( const gfl::math::VEC3& target_position )
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
void DebugG3dStereoCamera::SetCameraAndTargetPosition( const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position )
{
  // 基底のSetCameraAndTargetPosition()を呼ぶと内部でSetCameraPosition()とSetTargetPosition()が呼ばれていて、
  // 結局このクラスのSetCameraPosition()とSetTargetPosition()が呼ばれる。
  // 処理速度という観点で既に問題だが、設定順の関係で正しい値を設定していてもGFL_ASSERT(target_distance >= MIN_DISTANCE);で落ちることがある。論外である。
  // gfl::grp::g3d::Camera::SetCameraAndTargetPosition( camera_position, target_position );
  gfl::grp::g3d::Camera::SetCameraPosition(camera_position);
  gfl::grp::g3d::Camera::SetTargetPosition(target_position);

  UpdateDebugCamera();
}

//-----------------------------------------------------------------------------
/*
 *  @brief        現在設定されているカメラの位置と注視点の位置をデバッグカメラに反映する
 */
//-----------------------------------------------------------------------------
void DebugG3dStereoCamera::UpdateDebugCamera(void)
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

