#ifndef __GFL_DEBUGG3DCAMERACHASE_H__
#define __GFL_DEBUGG3DCAMERACHASE_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DebugG3dCameraChase.h
 *	@brief  3Dカメラ追跡型
 *	@author	Koji Kawada
 *	@date		2011.02.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <ui/ctr/gfl_UI_CTR_DeviceManager.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dCamera.h>

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
class DebugG3dCameraChase : public gfl::grp::g3d::Camera
{
public:
  //-----------------------------------------------------------------------------
  /*
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  DebugG3dCameraChase(void)
    : gfl::grp::g3d::Camera(),
      m_initial_rotate(0.0f, 0.0f, 0.0f),
      m_initial_target_pos(0.0f, 0.0f, 0.0f),
      m_initial_target_distance(DEFAULT_DISTANCE),
      m_initial_target_offset_y(0.0f),
      m_rotate(0.0f, 0.0f, 0.0f),
      m_target_pos(0.0f, 0.0f, 0.0f),
      m_target_distance(DEFAULT_DISTANCE),
      m_target_offset_y(0.0f)
  {
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~DebugG3dCameraChase()
  {
    //gfl::grp::g3d::Camera::~Camera();  // @note 親のデストラクタは書かなくても呼ばれるので、書いたら2回呼び出しになってしまうので、ダメ。
  }

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
  virtual void Create(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      gfl::grp::g3d::Resource*          resource,
      s32                               index_in_resource
  );
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
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*          allocator,
      const gfl::math::VEC3&           camera_position   = CAMERA_POSITION_DEFAULT,
      const gfl::math::VEC3&           target_position   = TARGET_POSITION_DEFAULT,
      f32                              near_clip         = NEAR_CLIP_DEFAULT,
      f32                              far_clip          = FAR_CLIP_DEFAULT,
      f32                              fovy_radian       = FOVY_RADIAN_DEFAULT,
      gfl::math::PivotDirection        pivot_direction   = gfl::math::PIVOT_NONE,//gfl::math::PIVOT_UPSIDE_TO_TOP,
      f32                              w_scale           = 0.0f
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
  //-----------------------------------------------------------------------------
  /*
   *  @brief         操作
   *                 呼んだフレームの操作が反映される。毎フレーム呼べば毎フレーム操作可能。
   *
   *  @param[in]     device_manager    デバイスマネージャー
   *  @param[in]     target_position   カメラの注視点の位置
   */
  //-----------------------------------------------------------------------------
  void Control(const gfl::ui::DeviceManager* device_manager, const gfl::math::VEC3& target_position );
  void Control(const gfl::math::VEC3& target_position );
  
  //-----------------------------------------------------------------------------
  /*
   *  @brief         パラメータ出力
   */
  //-----------------------------------------------------------------------------
  void PrintParameter(void) const;

  //-----------------------------------------------------------------------------
  /*
   *  @brief         パラメータの取得/設定
   */
  //-----------------------------------------------------------------------------
  void SetParameterRotate(const gfl::math::VEC3& rotate);
  void GetParameterRotate(gfl::math::VEC3* rotate) const;
  void SetParameterTargetPos(const gfl::math::VEC3& pos);
  void GetParameterTargetPos(gfl::math::VEC3* pos) const;
  void SetParameterTargetDistance(const f32 distance);
  f32  GetParameterTargetDistance(void) const;
  void SetParameterTargetOffsetY(const f32 y);
  f32  GetParameterTargetOffsetY(void) const;

private:
  //-----------------------------------------------------------------------------
  /*
   *  @brief        現在設定されているカメラの位置と注視点の位置をデバッグカメラに反映する
   */
  //-----------------------------------------------------------------------------
  void UpdateDebugCamera(void);

  //-----------------------------------------------------------------------------
  /*
   *  @brief        現在設定されているパラメータでデバッグカメラを更新する
   */
  //-----------------------------------------------------------------------------
  void UpdateDebugCameraFromParameter(void);

private:
  static const f32 DOLLY_SPEED_BASE = 30.0f / 60.0f;
  static const f32 TUMBLE_SPEED     = 128.0f / 60.0f;
  static const f32 MOVE_SPEED_BASE  = 5.0f / 60.0f;
  static const f32 ROTATE_X_LIMIT   = 64.f - TUMBLE_SPEED;
  static const f32 DEFAULT_DISTANCE = 10.0f;
  static const f32 MIN_DISTANCE     = 0.01f;

private:
  gfl::math::VEC3    m_initial_rotate;
  gfl::math::VEC3    m_initial_target_pos;
  f32                m_initial_target_distance;
  f32                m_initial_target_offset_y;

  gfl::math::VEC3    m_rotate;
  gfl::math::VEC3    m_target_pos;
  f32                m_target_distance;
  f32                m_target_offset_y;
};

}  // namespace debug
}  // namespace gfl

#endif // __GFL_DEBUGG3DCAMERACHASE_H__
