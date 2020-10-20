#ifndef __GFL_DEBUGG3DSTEREOCAMERA_H__
#define __GFL_DEBUGG3DSTEREOCAMERA_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DebugG3dStereoCamera.h
 *	@brief  デバッグ3Dステレオカメラ
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <ui/ctr/gfl_UI_CTR_DeviceManager.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dStereoCamera.h>

namespace gfl {
namespace debug {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	デバッグ3Dステレオカメラクラス
//=====================================
class DebugG3dStereoCamera : public gfl::grp::g3d::StereoCamera
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugG3dStereoCamera);

public:
  //-----------------------------------------------------------------------------
  /*
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  DebugG3dStereoCamera(void)
    : gfl::grp::g3d::StereoCamera(),
      m_initial_rotate(0.0f, 0.0f, 0.0f),
      m_initial_target_pos(0.0f, 0.0f, 0.0f),
      m_initial_target_distance(DEFAULT_DISTANCE),
      m_rotate(0.0f, 0.0f, 0.0f),
      m_target_pos(0.0f, 0.0f, 0.0f),
      m_target_distance(DEFAULT_DISTANCE)
  {
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~DebugG3dStereoCamera()
  {
    Destroy();
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
   *  @param[in]    depth_level        ベースカメラから基準面までの距離
   *  @param[in]    depth_range        視差の補正値
   *  @param[in]    real_switch        現実感を出すために視野角の変更を許すか(trueのとき許す)
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*          allocator,
      const gfl::math::VEC3&           camera_position   = CAMERA_POSITION_DEFAULT,
      const gfl::math::VEC3&           target_position   = TARGET_POSITION_DEFAULT,
      f32                              near_clip         = NEAR_CLIP_DEFAULT,
      f32                              far_clip          = FAR_CLIP_DEFAULT,
      f32                              fovy_radian       = FOVY_RADIAN_DEFAULT,
      f32                              w_scale           = 0.0f,
      f32                              depth_level       = DEPTH_LEVEL_DEFAULT,
      f32                              depth_range       = DEPTH_RANGE_DEFAULT,
      b32                              real_switch       = REAL_SWITCH_DEFAULT
  );
  //-----------------------------------------------------------------------------
  /*
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /*
   *  @brief        カメラの位置を設定する
   *
   *  @param[in]    camera_position    カメラの位置
   */
  //-----------------------------------------------------------------------------
  virtual void SetCameraPosition( const gfl::math::VEC3& camera_position );
  //-----------------------------------------------------------------------------
  /*
   *  @brief        カメラの注視点の位置を設定する
   *
   *  @param[in]    target_position    カメラの注視点の位置
   */
  //-----------------------------------------------------------------------------
  virtual void SetTargetPosition( const gfl::math::VEC3& target_position );
  //-----------------------------------------------------------------------------
  /*
   *  @brief        カメラの位置と注視点の位置を設定する
   *
   *  @param[in]    camera_position    カメラの位置
   *  @param[in]    target_position    カメラの注視点の位置
   */
  //-----------------------------------------------------------------------------
  virtual void SetCameraAndTargetPosition( const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position );

public:
  // Controlフラグ
  enum
  {
    FLAG_CONT_RESET = 1<<0,
    FLAG_CONT_FORWARD_BACK = 1<<1,
    FLAG_CONT_TRANSLATE = 1<<2,
    FLAG_CONT_TRANSLATE_XZ = 1<<3,
    FLAG_CONT_UP_DOWN = 1<<4,
    FLAG_CONT_ROTATE = 1<<5,

    FLAG_CONT_NORMAL = (FLAG_CONT_RESET | FLAG_CONT_FORWARD_BACK | FLAG_CONT_TRANSLATE | FLAG_CONT_UP_DOWN |FLAG_CONT_ROTATE),
  };
  
  //-----------------------------------------------------------------------------
  /*
   *  @brief         操作
   *                 呼んだフレームの操作が反映される。毎フレーム呼べば毎フレーム操作可能。
   *
   *  @param[in]     device_manager    デバイスマネージャー
   */
  //-----------------------------------------------------------------------------
  void Control(const gfl::ui::DeviceManager* device_manager, u32 flag = FLAG_CONT_NORMAL);

private:
  //-----------------------------------------------------------------------------
  /*
   *  @brief        現在設定されているカメラの位置と注視点の位置をデバッグカメラに反映する
   */
  //-----------------------------------------------------------------------------
  void UpdateDebugCamera(void);

private:
  static const f32 DOLLY_SPEED_BASE = 30.0f / 60.0f;
  static const f32 TUMBLE_SPEED     = 128.0f / 60.0f;
  static const f32 MOVE_SPEED_BASE  = 30.0f / 60.0f;
  static const f32 ROTATE_X_LIMIT   = 64.f - TUMBLE_SPEED;
  static const f32 DEFAULT_DISTANCE = 10.0f;
  static const f32 MIN_DISTANCE     = 0.01f;

private:
  gfl::math::VEC3    m_initial_rotate;
  gfl::math::VEC3    m_initial_target_pos;
  f32                m_initial_target_distance;

  gfl::math::VEC3    m_rotate;
  gfl::math::VEC3    m_target_pos;
  f32                m_target_distance;
};

}  // namespace debug
}  // namespace gfl

#endif // __GFL_DEBUGG3DSTEREOCAMERA_H__
