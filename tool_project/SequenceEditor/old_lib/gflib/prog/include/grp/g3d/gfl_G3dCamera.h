#ifndef __GFL_G3DCAMERA_H__
#define __GFL_G3DCAMERA_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dCamera.h
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
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Math.h>
#include <grp/gfl_Camera.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dAnimationManager.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dCameraViewClip.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3Dカメラクラス
//=====================================
class Camera : public SceneNode
{
  GFL_FORBID_COPY_AND_ASSIGN(Camera);


  friend class AnimationManager;
  friend class StereoCamera;


public:

  // ビューモード
  enum ViewMode
  {
    VIEW_MODE_AIM,      // Aimカメラ
    VIEW_MODE_LOOK_AT,  // LookAtカメラ
    VIEW_MODE_ROTATE    // Rotateカメラ
  };

  // 射影モード
  enum ProjectionMode
  {
    PROJECTION_MODE_PERSPECTIVE,
    PROJECTION_MODE_FRUSTUM,
    PROJECTION_MODE_ORTHO,
  };

  // リソースからカメラを生成するときの設定内容
  struct ResourceDescription
  {
    b32  animation_enable;          // アニメーションを行う場合はtrue(アニメーションを割り当てるスロットを1個つくります)
    u32  animation_blend_unit_num;  // スロット内でアニメーションのブレンドを行う場合2以上、行わない場合0か1(0でも1でもスロット内に1つユニットをつくります)
    u32  animation_num_in_unit;     // ユニット内で補間ブレンドしながらアニメーションを変更する場合2以上、行わない場合0か1(0でも1でもユニット内に1つつくります)
    ResourceDescription(void)
      : animation_enable(false),
        animation_blend_unit_num(0),
        animation_num_in_unit(0)
    {}
  };
  // ResourceDescriptionの初期化関数
  // デフォルト値を設定する
  static void InitializeResourceDescriptionForDefault(
      ResourceDescription*  description
  );

  // 設定内容
  struct Description
  {
    ViewMode                   view_mode;
    gfl::math::VEC3            camera_position;          //  AIM  LOOK_AT  ROTATE  のとき使用
    gfl::math::VEC3            target_position;          //  AIM  LOOK_AT          のとき使用
    gfl::math::VEC3            up_vector;                //       LOOK_AT          のとき使用
    f32                        twist;  // [radian]       //  AIM                   のとき使用
    gfl::math::VEC3            rotate_xyz;  // [radian]  //                ROTATE  のとき使用
    f32                        w_scale;                  // Wスケールの値が0.0f以外のときWバッファ(クリップ座標系で用いられるWの値を格納する為の記憶領域)を使用
    ProjectionMode             projection_mode;
    f32                        fovy;  // [radian]        //  PROJECTION_MODE_PERSPECTIVE                                                  のとき使用
    f32                        aspect;                   //  PROJECTION_MODE_PERSPECTIVE  PROJECTION_MODE_FRUSTUM  PROJECTION_MODE_ORTHO  のとき使用
    f32                        height;                   //                               PROJECTION_MODE_FRUSTUM  PROJECTION_MODE_ORTHO  のとき使用
    gfl::math::VEC2            center;                   //                               PROJECTION_MODE_FRUSTUM  PROJECTION_MODE_ORTHO  のとき使用
    f32                        near;
    f32                        far;
    gfl::math::PivotDirection  pivot;
    s32                        max_callbacks;            // 管理できるコールバックの最大数
    s32                        max_children;             // 子の最大数
    b32                        is_fixed_size_memory;     // 生成時以外にもメモリを確保するかどうかのフラグ
    b32                        animation_enable;          // アニメーションを行う場合はtrue(アニメーションを割り当てるスロットを1個つくります)
    u32                        animation_blend_unit_num;  // スロット内でアニメーションのブレンドを行う場合2以上、行わない場合0か1(0でも1でもスロット内に1つユニットをつくります)
    u32                        animation_num_in_unit;     // ユニット内で補間ブレンドしながらアニメーションを変更する場合2以上、行わない場合0か1(0でも1でもユニット内に1つつくります)
    Description(void)
      : view_mode(VIEW_MODE_LOOK_AT),
        camera_position(0.0f,0.0f,100.0f),
        target_position(0.0f,0.0f,0.0f),
        up_vector(0.0f,1.0f,0.0f),
        twist(GFL_MATH_DEG_TO_RAD(0.0f)),
        rotate_xyz(GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f),GFL_MATH_DEG_TO_RAD(0.0f)),
        w_scale(0.0f),
        projection_mode(PROJECTION_MODE_PERSPECTIVE),
        fovy(GFL_MATH_DEG_TO_RAD(37.8f)),
        aspect(400.0f/240.0f),
        height(240.0f),
        center(0.0f,0.0f),
        near(1.0f),
        far(10000.0f),
        pivot(gfl::math::PIVOT_NONE),
        max_callbacks(0),
        max_children(0),
        is_fixed_size_memory(false),
        animation_enable(false),
        animation_blend_unit_num(0),
        animation_num_in_unit(0)
    {}
  };

  // Descriptionの初期化関数
  // デフォルト値を設定する
  static void InitializeDescriptionForDefault(
      Description*  description
  );

  // Descriptionの初期化関数
  // AIM     PERSPECTIVE
  static void InitializeDescriptionForAimPerspective(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const f32                         twist,  // [radian]
    const f32                         w_scale,
    const f32                         fovy,  // [radian]
    const f32                         aspect,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // AIM     FRUSTUM
  static void InitializeDescriptionForAimFrustum(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const f32                         twist,  // [radian]
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // AIM     ORTHO
  static void InitializeDescriptionForAimOrtho(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const f32                         twist,  // [radian]
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // LOOK_AT PERSPECTIVE
  static void InitializeDescriptionForLookAtPerspective(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const gfl::math::VEC3&            up_vector,
    const f32                         w_scale,
    const f32                         fovy,  // [radian]
    const f32                         aspect,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // LOOK_AT FRUSTUM
  static void InitializeDescriptionForLookAtFrustum(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const gfl::math::VEC3&            up_vector,
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // LOOK_AT ORTHO
  static void InitializeDescriptionForLookAtOrtho(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            target_position,
    const gfl::math::VEC3&            up_vector,
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // ROTATE  PERSPECTIVE
  static void InitializeDescriptionForRotatePerspective(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            rotate_xyz,  // [radian]
    const f32                         w_scale,
    const f32                         fovy,  // [radian]
    const f32                         aspect,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // ROTATE  FRUSTUM
  static void InitializeDescriptionForRotateFrustum(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            rotate_xyz,  // [radian]
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );
  // ROTATE  ORTHO
  static void InitializeDescriptionForRotateOrtho(
    Description*                      description,
    const gfl::math::VEC3&            camera_position,
    const gfl::math::VEC3&            rotate_xyz,  // [radian]
    const f32                         w_scale,
    const f32                         aspect,
    const f32                         height,
    const gfl::math::VEC2&            center,
    const f32                         near,
    const f32                         far,
    const gfl::math::PivotDirection   pivot
  );


public:
  //
  // コンストラクタ / デストラクタ
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  Camera(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~Camera();




  //
  // 生成 / 破棄
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     allocator          アロケータ
   *  @param[in]     device_allocator   デバイスメモリのアロケータ
   *  @param[in]     resource           リソース
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @param[in]     description        リソースからカメラを生成するときの設定内容
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      Resource*                         resource,
      s32                               index_in_resource,
      ResourceDescription*              description = NULL
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     description    
   *
   * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*    allocator,
      Description*               description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         計算処理
   */
  //-----------------------------------------------------------------------------
  virtual void Calculate(void);




  //
  // トランスフォーム
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの位置を得る
   *
   *  @param[out]   camera_position    カメラの位置を返す
   */
  //-----------------------------------------------------------------------------
  void GetCameraPosition( gfl::math::VEC3& camera_position ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの注視点の位置を得る
   *
   *  @param[out]   target_position    カメラの注視点の位置を返す
   */
  //-----------------------------------------------------------------------------
  void GetTargetPosition( gfl::math::VEC3& target_position ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの位置と注視点の位置を得る
   *
   *  @param[out]   camera_position    カメラの位置を返す
   *  @param[out]   target_position    カメラの注視点の位置を返す
   */
  //-----------------------------------------------------------------------------
  void GetCameraAndTargetPosition( gfl::math::VEC3& camera_position, gfl::math::VEC3& target_position );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの位置を設定する
   *
   *  @param[in]    camera_position    カメラの位置
   */
  //-----------------------------------------------------------------------------
  virtual void SetCameraPosition( const gfl::math::VEC3& camera_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの注視点の位置を設定する
   *
   *  @param[in]    target_position    カメラの注視点の位置
   */
  //-----------------------------------------------------------------------------
  virtual void SetTargetPosition( const gfl::math::VEC3& target_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの位置と注視点の位置を設定する
   *
   *  @param[in]    camera_position    カメラの位置
   *  @param[in]    target_position    カメラの注視点の位置
   */
  //-----------------------------------------------------------------------------
  virtual void SetCameraAndTargetPosition( const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        回転カメラの角度
   *                単位はラジアン。
   */
  //-----------------------------------------------------------------------------
  void GetRotateXYZ(gfl::math::VEC3* r) const;
  virtual void SetRotateXYZ(const gfl::math::VEC3& r);
  virtual void SetRotateXYZ(f32 x, f32 y, f32 z);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスを取得する
   *
   *  @param[out]    matrix          取得したマトリクスを格納して返す
   *
   *  @retval        bool            正常に取得できたらtrue
   */
  //-----------------------------------------------------------------------------
  bool GetWorldMatrix(gfl::math::MTX34* matrix) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスを設定する
   *                 SceneのCalculateの後でないと反映されません。
   *
   *  @param[in]     matrix          設定するマトリクス
   *
   *  @retval        bool            正常に設定できたらtrue
   */
  //-----------------------------------------------------------------------------
  bool SetWorldMatrix(const gfl::math::MTX34& matrix);
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  マトリックスのニア、ファー値を変更
   *	@note   使用後、 SetMatrixNearFarDefault で必ず元に戻すこと。
   *
   *	@param	f32 near ニア値
   *	@param	f32 far  ファー値
   */
  //-----------------------------------------------------------------------------
  void SetMatrixNearFar( const f32 near, const f32 far );
    
  //-----------------------------------------------------------------------------
  /**
   *	@brief  マトリックスのニア、ファーを内部で保持している値に戻す
   */
  //-----------------------------------------------------------------------------
  void SetMatrixNearFarDefault( void );




  // 
  // 変換
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief         grpカメラの内容をg3dカメラに反映する
   *
   *  @param[in]     grp_camera    grpカメラ
   */
  //-----------------------------------------------------------------------------
  void ConvertGrpCameraToG3dCamera( const gfl::grp::Camera& grp_camera );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         g3dカメラの内容をgrpカメラに反映する
   *
   *  @param[out]    grp_camera    grpカメラ
   */
  //-----------------------------------------------------------------------------
  void ConvertG3dCameraToGrpCamera( gfl::grp::Camera& grp_camera );




  //
  // カメラマトリクス
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ビューマトリクスとプロジェクションマトリクスを更新する
   */
  //-----------------------------------------------------------------------------
  virtual void UpdateCameraMatrix(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ビューマトリクスを得る
   *
   *  @param[out]    view_matrix    ビューマトリクス
   */
  //-----------------------------------------------------------------------------
  void GetViewMatrix( gfl::math::MTX34* view_matrix ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ビューマトリクスを設定する
   *                【注意】この後にUpdateCameraMatrixを行うと、UpdateCameraMatrixで計算した値で上書きされてしまいます。
   *
   *  @param[in]    view_matrix     ビューマトリクス
   *  @param[in]    update_matrix   ビューマトリクスの逆行列を計算し直すときtrue
   */
  //-----------------------------------------------------------------------------
  void SetViewMatrix( const gfl::math::MTX34& view_matrix, const b32 update_matrix = true );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         プロジェクョンマトリクスを得る
   *
   *  @param[out]    projection_matrix    プロジェクションマトリクス
   */
  //-----------------------------------------------------------------------------
  void GetProjectionMatrix( gfl::math::MTX44* projection_matrix ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         プロジェクョンマトリクスを設定する
   *                【注意】この後にUpdateCameraMatrixを行うと、UpdateCameraMatrixで計算した値で上書きされてしまいます。
   *
   *  @param[in]    projection_matrix   プロジェクションマトリクス
   *  @param[in]    update_matrix       プロジェクションマトリクスの逆行列を計算し直すときtrue
   */
  //-----------------------------------------------------------------------------
  void SetProjectionMatrix( const gfl::math::MTX44& projection_matrix, const b32 update_matrix = true );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ビューマトリクスとプロジェクョンマトリクスを得る
   *
   *  @param[out]    view_matrix          ビューマトリクス
   *  @param[out]    projection_matrix    プロジェクションマトリクス
   */
  //-----------------------------------------------------------------------------
  void GetViewAndProjectionMatrix( gfl::math::MTX34* view_matrix, gfl::math::MTX44* projection_matrix ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ビューマトリクスとプロジェクョンマトリクスを設定する
   *                【注意】この後にUpdateCameraMatrixを行うと、UpdateCameraMatrixで計算した値で上書きされてしまいます。
   *
   *  @param[in]    view_matrix         ビューマトリクス
   *  @param[in]    projection_matrix   プロジェクションマトリクス
   *  @param[in]    update_matrix       ビューマトリクスとプロジェクションマトリクスの逆行列を計算し直すときtrue
   */
  //-----------------------------------------------------------------------------
  void SetViewAndProjectionMatrix( const gfl::math::MTX34& view_matrix, const gfl::math::MTX44& projection_matrix, const b32 update_matrix = true );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        スクリーン座標系の点をワールド座標系の半直線(カメラの位置からスクリーンの点に飛ばしたレイ)に変換する
   *
   *  @param[out]   world_ray_direction   ワールド座標系の半直線の方向ベクトル(大きさ1)
   *  @param[out]   world_ray_origin      ワールド座標系の半直線の始点座標(カメラの位置GetCameraPositionと同じ値が得られます)
   *  @param[in]    screen_point_x        スクリーン座標系の点の位置座標(左上が(0.0f,0.0f)、右下が(1.0f,1.0f))
   *  @param[in]    screen_point_y
   *  @param[in]    render_target         レンダーターゲット
   */
  //-----------------------------------------------------------------------------
  void ConvertScreenPointToWorldRay(
      gfl::math::VEC3*              world_ray_direction,
      gfl::math::VEC3*              world_ray_origin,
      const f32                     screen_point_x,
      const f32                     screen_point_y,
      const gfl::grp::RenderTarget& render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        上画面スクリーン座標系の点をワールド座標系の半直線(カメラの位置からスクリーンの点に飛ばしたレイ)に変換する
   *
   *  @param[out]   world_ray_direction   ワールド座標系の半直線の方向ベクトル(大きさ1)
   *  @param[out]   world_ray_origin      ワールド座標系の半直線の始点座標(カメラの位置GetCameraPositionと同じ値が得られます)
   *  @param[in]    upper_screen_point_x  上画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(400-1,240-1))
   *  @param[in]    upper_screen_point_y  画面範囲外も表せるようにsignedにしました。 
   *  @param[in]    upper_render_target   レンダーターゲット(これがベースカメラのときは左でも右でもどちらでもいい)
   */
  //-----------------------------------------------------------------------------
  void ConvertUpperScreenPointToWorldRay(
      gfl::math::VEC3*              world_ray_direction,
      gfl::math::VEC3*              world_ray_origin,
      const s32                     upper_screen_point_x,
      const s32                     upper_screen_point_y,
      const gfl::grp::RenderTarget& upper_render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        下画面スクリーン座標系の点をワールド座標系の半直線(カメラの位置からスクリーンの点に飛ばしたレイ)に変換する
   *
   *  @param[out]   world_ray_direction   ワールド座標系の半直線の方向ベクトル(大きさ1)
   *  @param[out]   world_ray_origin      ワールド座標系の半直線の始点座標(カメラの位置GetCameraPositionと同じ値が得られます)
   *  @param[in]    lower_screen_point_x  下画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(320-1,240-1))
   *  @param[in]    lower_screen_point_y  画面範囲外も表せるようにsignedにしました。 
   *  @param[in]    lower_render_target   レンダーターゲット
   */
  //-----------------------------------------------------------------------------
  void ConvertLowerScreenPointToWorldRay(
      gfl::math::VEC3*              world_ray_direction,
      gfl::math::VEC3*              world_ray_origin,
      const s32                     lower_screen_point_x,
      const s32                     lower_screen_point_y,
      const gfl::grp::RenderTarget& lower_render_target
  ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief        スクリーン座標系の点をワールド座標系の点(ニア面上orファー面上)に変換する
   *
   *  @param[out]   world_point           ワールド座標系の点の位置座標
   *  @param[in]    is_near               trueのときニア面上、falseのときファー面上
   *  @param[in]    screen_point_x        スクリーン座標系の点の位置座標(左上が(0.0f,0.0f)、右下が(1.0f,1.0f))
   *  @param[in]    screen_point_y
   *  @param[in]    render_target         レンダーターゲット
   */
  //-----------------------------------------------------------------------------
  void ConvertScreenPointToWorldPoint(
      gfl::math::VEC3*              world_point,
      const b32                     is_near,
      const f32                     screen_point_x,
      const f32                     screen_point_y,
      const gfl::grp::RenderTarget& render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        上画面スクリーン座標系の点をワールド座標系の点(ニア面上orファー面上)に変換する
   *
   *  @param[out]   world_point           ワールド座標系の点の位置座標
   *  @param[in]    is_near               trueのときニア面上、falseのときファー面上
   *  @param[in]    upper_screen_point_x  上画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(400-1,240-1))
   *  @param[in]    upper_screen_point_y  画面範囲外も表せるようにsignedにしました。 
   *  @param[in]    render_target         レンダーターゲット
   */
  //-----------------------------------------------------------------------------
  void ConvertUpperScreenPointToWorldPoint(
      gfl::math::VEC3*              world_ray_direction,
      const b32                     is_near,
      const s32                     upper_screen_point_x,
      const s32                     upper_screen_point_y,
      const gfl::grp::RenderTarget& upper_render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        下画面スクリーン座標系の点をワールド座標系の点(ニア面上orファー面上)に変換する
   *
   *  @param[out]   world_point           ワールド座標系の点の位置座標
   *  @param[in]    is_near               trueのときニア面上、falseのときファー面上
   *  @param[in]    lower_screen_point_x  下画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(320-1,240-1))
   *  @param[in]    lower_screen_point_y  画面範囲外も表せるようにsignedにしました。 
   *  @param[in]    render_target         レンダーターゲット
   */
  //-----------------------------------------------------------------------------
  void ConvertLowerScreenPointToWorldPoint(
      gfl::math::VEC3*              world_point,
      const b32                     is_near,
      const s32                     lower_screen_point_x,
      const s32                     lower_screen_point_y,
      const gfl::grp::RenderTarget& lower_render_target
  ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ワールド座標系の点をスクリーン座標系の点に変換する
   *
   *  @param[out]    screen_point_x        スクリーン座標系の点の位置座標(左上が(0.0f,0.0f)、右下が(1.0f,1.0f))
   *  @param[out]    screen_point_y        
   *  @param[in]     world_point           ワールド座標系の点の位置座標
   *  @param[in]     render_target         レンダーターゲット
   */
  //-----------------------------------------------------------------------------
  void ConvertWorldPointToScreenPoint(
      f32*                          screen_point_x,
      f32*                          screen_point_y,
      const gfl::math::VEC3&        world_point,
      const gfl::grp::RenderTarget& render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ワールド座標系の点を上画面スクリーン座標系の点に変換する
   *
   *  @param[out]    upper_screen_point_x  上画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(400-1,240-1))
   *  @param[out]    upper_screen_point_y  画面範囲外も表せるようにsignedにしました。     
   *  @param[in]     world_point           ワールド座標系の点の位置座標
   *  @param[in]     upper_render_target   レンダーターゲット(これがベースカメラのときは左でも右でもどちらでもいい)
   */
  //-----------------------------------------------------------------------------
  void ConvertWorldPointToUpperScreenPoint(
      s32*                          upper_screen_point_x,
      s32*                          upper_screen_point_y,
      const gfl::math::VEC3&        world_point,
      const gfl::grp::RenderTarget& upper_render_target
  ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ワールド座標系の点を下画面スクリーン座標系の点に変換する
   *
   *  @param[out]    lower_screen_point_x  下画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(320-1,240-1))
   *  @param[out]    lower_screen_point_y  画面範囲外も表せるようにsignedにしました。
   *  @param[in]     world_point           ワールド座標系の点の位置座標
   *  @param[in]     lower_render_target   レンダーターゲット
   */
  //-----------------------------------------------------------------------------
  void ConvertWorldPointToLowerScreenPoint(
      s32*                          lower_screen_point_x,
      s32*                          lower_screen_point_y,
      const gfl::math::VEC3&        world_point,
      const gfl::grp::RenderTarget& lower_render_target
  ) const;




  //
  // アニメーション
  //
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションを変更する
   *                 初めてこの関数を呼んだときは、Animationを生成する。
   *
   *  @param[in]     allocator               アロケータ
   *  @param[in]     res_anim                アニメーションリソース
   *  @param[in]     max_anim_members        アニメーションするメンバの最大数。
   *                                         初めてこの関数を呼んだときは、使用される。
   *                                         Resource::GetCameraMemberAnimSetCountで得た値を渡すとよい。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのGetMemberAnimSetCountを使用します。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのアニメーショングループのメンバ数に、
   *                                         即ち、そのアニメーションタイプにおける最大数にします。
   */
  //-----------------------------------------------------------------------------
  void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    ResAnim*                res_anim,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションを変更する
   *                 初めてこの関数を呼んだときは、Animationを生成する。
   *
   *  @param[in]     allocator               アロケータ
   *  @param[in]     resource                リソース
   *  @param[in]     index_in_resource       リソース内におけるインデックス
   *  @param[in]     max_anim_members        アニメーションするメンバの最大数。
   *                                         初めてこの関数を呼んだときは、使用される。
   *                                         Resource::GetCameraMemberAnimSetCountで得た値を渡すとよい。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのGetMemberAnimSetCountを使用します。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのアニメーショングループのメンバ数に、
   *                                         即ち、そのアニメーションタイプにおける最大数にします。
   */
  //-----------------------------------------------------------------------------
  void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    Resource*               resource,
    s32                     index_in_resource,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );


  void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members
  );
  void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    Resource*               resource,
    s32                     index_in_resource,
    u32                     change_frame,
    s32                     max_anim_members
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_indexのunit_indexに割り当てるアニメーションを変更する
   *
   *  @param[in]     heap_allocator          ヒープアロケータ
   *  @param[in]     device_allocator        デバイスアロケータ
   *  @param[in]     unit_index              スロット内につくったユニットのインデックス
   *  @param[in]     res_anim                アニメーションリソース(NULLを渡すと全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)
   *  @param[in]     resource                リソース(NULLを渡すと全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)
   *  @param[in]     animation_resource_type アニメーションリソースのタイプ(resourceがNULLのときは使用されないのでてきとーな値でOK)
   *  @param[in]     index_in_resource       リソース内におけるインデックス(resourceがNULLのときは使用されないのでてきとーな値でOK)
   *  @param[in]     change_frame            何フレームで変更するか(この間ブレンドされる)
   *  @param[in]     max_anim_members        アニメーションするメンバの最大数。(res_animがNULLのときは使用されないのでてきとーな値でOK)
   *                                         Resource::Get???MemberAnimSetCountで得た値を渡すとよい。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのGetMemberAnimSetCountを使用します。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのアニメーショングループのメンバ数に、
   *                                         即ち、そのアニメーションタイプにおける最大数にします。
   */
  //-----------------------------------------------------------------------------
  void ChangeUnitAnimation(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    u32                     unit_index,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );
  void ChangeUnitAnimation(
    gfl::heap::NwAllocator*          heap_allocator,
    gfl::heap::NwAllocator*          device_allocator,
    u32                              unit_index,
    Resource*                        resource,
    s32                              index_in_resource,
    u32                              change_frame,
    s32                              max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのフレームを設定する
   *
   *  @param[in]     frame           フレーム
   */
  //-----------------------------------------------------------------------------
  void SetAnimationFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのフレームを取得する
   *
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimationFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの更新フレームを設定する
   *                 0.0fで停止、1.0fで等倍、2.0fで2倍速、-1.0fで逆等倍再生
   *
   *  @param[in]     step_frame      更新フレーム
   */
  //-----------------------------------------------------------------------------
  void SetAnimationStepFrame(f32 step_frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの更新フレームを取得する
   *
   *  @retval        更新フレーム
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimationStepFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームを設定する
   *
   *  @param[in]     frame           フレーム
   */
  //-----------------------------------------------------------------------------
  void SetAnimationStartFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームを取得する
   *
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimationStartFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの終了フレームを設定する
   *
   *  @param[in]     frame           フレーム
   */
  //-----------------------------------------------------------------------------
  void SetAnimationEndFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの終了フレームを取得する
   *
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimationEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームと終了フレームを設定する
   *
   *  @param[in]     start_frame     開始フレーム
   *  @param[in]     end_frame       終了フレーム
   */
  //-----------------------------------------------------------------------------
  void SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの現在のフレームが終了フレームか否か判定する
   *
   *  @retval        bool    現在のフレームが終了フレームの場合tureを返す
   */
  //-----------------------------------------------------------------------------
  bool IsAnimationFrameEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションがループ再生か否か取得する
   *
   *  @retval        bool            ループのときtrue、ループでないときfalseを返す
   */
  //-----------------------------------------------------------------------------
  bool IsAnimationLoop(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションにループ再生を設定する
   *
   *  @param[in]     loop            ループのときtrue、ループでないときfalseを設定する
   */
  //-----------------------------------------------------------------------------
  void SetAnimationLoop(bool loop);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         フレーム操作
   */
  //-----------------------------------------------------------------------------
  void SetUnitFrame(u32 unit_index, f32 frame);
  f32  GetUnitFrame(u32 unit_index) const;
  void SetUnitStepFrame(u32 unit_index, f32 step_frame);
  f32  GetUnitStepFrame(u32 unit_index) const;
  void SetUnitStartFrame(u32 unit_index, f32 frame);
  f32  GetUnitStartFrame(u32 unit_index) const;
  void SetUnitEndFrame(u32 unit_index, f32 frame);
  f32  GetUnitEndFrame(u32 unit_index) const;
  void SetUnitStartAndEndFrame(u32 unit_index, f32 start_frame, f32 end_frame);
  b32  IsUnitFrameEndFrame(u32 unit_index) const;
  b32  IsUnitLoop(u32 unit_index) const;
  void SetUnitLoop(u32 unit_index, b32 loop);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_index内につくったユニットのアニメーションをブレンドする
   *
   *  @param[in]     heap_allocator          ヒープアロケータ
   *  @param[in]     device_allocator        デバイスアロケータ
   *  @param[in]     mode                    AnimationSlot::MODE_NONE                slot_indexのアニメーションをなしにする
   *                                         AnimationSlot::MODE_ANIMATION           ブレンドをやめてanimation_unit_indexで指定したアニメーションを割り当てる
   *                                         AnimationSlot::MODE_BLEND_INTERPOLATOR  補間ブレンド
   *                                         AnimationSlot::MODE_BLEND_ADDER         加算ブレンド
   *  @param[in]     weight0                 AnimationSlot::MODE_BLEND_INTERPOLATORかAnimationSlot::MODE_BLEND_ADDERのときのみ使用。
   *                                         ユニット0のブレンド重み
   *  @param[in]     weight1                 AnimationSlot::MODE_BLEND_INTERPOLATORかAnimationSlot::MODE_BLEND_ADDERのときのみ使用。
   *                                         ユニット1のブレンド重み
   *  @param[in]     interpolator_ignore_no_anim_member   AnimationSlot::MODE_BLEND_INTERPOLATORのときのみ使用。
   *                                                      falseにすると、アニメーションが存在しないメンバはバインド時の値がブレンドされます。
   *                                                      trueにすると、アニメーションが存在しないメンバは重み 0 としてブレンドされます。
   *  @param[in]     interpolator_normalization_enable    AnimationSlot::MODE_BLEND_INTERPOLATORのときのみ使用。
   *                                                      trueにすると、ブレンド重みが合計で 1 になるよう正規化してからブレンドを行います。
   *                                                      falseにすると、設定した重みがそのままブレンドに使用される。
   *  @param[in]     animation_unit_index                 AnimationSlot::MODE_ANIMATIONのときのみ使用。
   *                                                      スロット内のどのユニットにあるアニメーションを割り当てるかを指定する。
   */
  //-----------------------------------------------------------------------------
  void BlendUnitAnimation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      AnimationSlot::Mode        mode,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );
  void BlendUnit2Animation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      AnimationSlot::Mode        mode,
      f32                        weight0,
      f32                        weight1,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         ブレンド重み
   */
  //-----------------------------------------------------------------------------
  void SetUnitBlendWeight(u32 unit_index, f32 weight);
  f32  GetUnitBlendWeight(u32 unit_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResAnim::Typeのアニメーションで変更したポーズをリセットする
   *
   *  アニメーションが割り当てられているときは、そのアニメーションが動かすメンバはリセットされない。
   *  アニメーションが割り当てられていないときは、全てのメンバがリセットされる。
   */
  //-----------------------------------------------------------------------------
  void ResetAnimationPose(void);




  //
  // ユーティリティ関数
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ビューアップデータ ユーティリティ関数
   */
  //-----------------------------------------------------------------------------
  void GetUpVector(gfl::math::VEC3* up_vector) const;
  void SetUpVector(const gfl::math::VEC3& up_vector);
  f32  GetTwist(void) const;  // [radian]
  void SetTwist(f32 twist);   // [radian]

  //-----------------------------------------------------------------------------
  /**
   *  @brief        プロジェクションアップデータ ユーティリティ関数
   */
  //-----------------------------------------------------------------------------
  f32  GetFovy(void) const;  // [radian]
  void SetFovy(f32 fovy);    // [radian]
  f32  GetAspect(void) const;
  void SetAspect(f32 aspect);
  f32  GetHeight(void) const;
  void SetHeight(f32 height);  // aspectを保持するため、heightを変更すると横幅も変更されます。
  void GetCenter(gfl::math::VEC2* center) const;
  void SetCenter(const gfl::math::VEC2& center);

  //-----------------------------------------------------------------------------
  /**
   *  @brief        画面の上方向を設定する
   *
   *  @param[in]    pivot_direction    画面の上方向
   */
  //-----------------------------------------------------------------------------
  void SetPivotDirection( gfl::math::PivotDirection pivot_direction );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        画面の上方向を取得する
   *
   *  @retval    gfl::math::PivotDirection    画面の上方向
   */
  //-----------------------------------------------------------------------------
  gfl::math::PivotDirection  GetPivotDirection(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ニアクリップ / ファークリップ
   */
  //-----------------------------------------------------------------------------
  void SetNearClip( f32 near_clip );
  f32  GetNearClip( void ) const;
  void SetFarClip( f32 far_clip );
  f32  GetFarClip( void ) const;




  //
  // WScale
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        WScale
   */
  //-----------------------------------------------------------------------------
  f32  GetWScale(void) const;
  void SetWScale(f32 w_scale);




  //
  // 取得 / 設定
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ビューモードを取得する
   */
  //-----------------------------------------------------------------------------
  ViewMode GetViewMode(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         射影モードを取得する
   */
  //-----------------------------------------------------------------------------
  ProjectionMode GetProjectionMode(void) const;

  ///< ビュークリップ制御クラスを取得する(実体は派生先が持つ)
  virtual const CameraViewClip* GetViewClip() const { return NULL; }

public:
  //---------------------------------------------------------------------------
  /**
   *           定数宣言
   */
  //---------------------------------------------------------------------------
  static const gfl::math::VEC3  CAMERA_POSITION_DEFAULT;    // カメラ位置のデフォルト値
  static const gfl::math::VEC3  TARGET_POSITION_DEFAULT;    // カメラターゲットのデフォルト値
  static const f32              NEAR_CLIP_DEFAULT;          // ニアクリップのデフォルト値
  static const f32              FAR_CLIP_DEFAULT;           // ファークリップのデフォルト値
  static const f32              FOVY_RADIAN_DEFAULT;        // FOVのY方向のデフォルト値
  static const gfl::math::VEC2  PROJECTION_CENTER_DEFAULT;  // カメラの投影面の中心位置のデフォルト値
  static const f32              PROJECTION_HEIGHT_DEFAULT;  // カメラの投影面の高さのデフォルト値
    
public:
  //
  // NW4C関連 static 生成 / 破棄
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラを生成する
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
   *  @retval       nw::gfx::Camera*  生成したカメラを返す
   *
   * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
   */
  //-----------------------------------------------------------------------------
  static nw::gfx::Camera* CreateCamera(
      gfl::heap::NwAllocator*          allocator,
      const gfl::math::VEC3&           camera_position   = CAMERA_POSITION_DEFAULT,
      const gfl::math::VEC3&           target_position   = TARGET_POSITION_DEFAULT,
      f32                              near_clip         = NEAR_CLIP_DEFAULT,
      f32                              far_clip          = FAR_CLIP_DEFAULT,
      f32                              fovy_radian       = FOVY_RADIAN_DEFAULT,
      gfl::math::PivotDirection        pivot_direction   = gfl::math::PIVOT_NONE,//gfl::math::PIVOT_UPSIDE_TO_TOP,
      f32                              w_scale           = 0.0f
  );

  static nw::gfx::Camera*
  CreateFrustumCamera(
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3& cameraPosition,
    const gfl::math::VEC3& targetPosition,
    f32 nearClip,
    f32 farClip,
    const gfl::math::VEC2& projectionCenter,
    f32 projectionHeight,
    gfl::math::PivotDirection pivotDirection,
    f32 wScale
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        立体視に使用するベースカメラと左右カメラを生成する
   *
   *                実際は右目用カメラと左目用カメラの区別はありません。
   *
   *  @param[out]   pp_base_camera     生成したベースカメラを返すためのポインタ
   *  @param[out]   pp_left_camera     生成した右目用カメラを返すためのポインタ
   *  @param[out]   pp_right_camera    生成した左目用カメラを返すためのポインタ
   *  @param[in]    allocator          生成に使用するアロケーター
   *  @param[in]    camera_position    カメラの位置座標
   *  @param[in]    target_position    カメラターゲットの座標
   *  @param[in]    near_clip          ニアクリップの値
   *  @param[in]    far_clip           ファークリップの値
   *  @param[in]    fovy_radian        FOVのY方向の値
   *  @param[in]    w_scale            WScaleの値
   *
   * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
   */
  //-----------------------------------------------------------------------------
  static void CreateStereoCameras(
      nw::gfx::Camera**           pp_base_camera,
      nw::gfx::Camera**           pp_left_camera,
      nw::gfx::Camera**           pp_right_camera,
      gfl::heap::NwAllocator*     allocator,
      const gfl::math::VEC3&      camera_position   = CAMERA_POSITION_DEFAULT,
      const gfl::math::VEC3&      target_position   = TARGET_POSITION_DEFAULT,
      f32                         near_clip         = NEAR_CLIP_DEFAULT,
      f32                         far_clip          = FAR_CLIP_DEFAULT,
      f32                         fovy_radian       = FOVY_RADIAN_DEFAULT,
      f32                         w_scale           = 0.0f
  );

  static void
  CreateFrustumStereoCameras(
    nw::gfx::Camera**           pp_base_camera,
    nw::gfx::Camera**           pp_left_camera,
    nw::gfx::Camera**           pp_right_camera,
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3& cameraPosition,
    const gfl::math::VEC3& targetPosition,
    f32 nearClip,
    f32 farClip,
    const gfl::math::VEC2& projectionCenter,
    f32 projectionHeight,
    gfl::math::PivotDirection pivotDirection,
    f32 w_scale
  );

  //
  // NW4C関連 static 設定 / 取得
  //
  static void SetFrustum(
     nw::gfx::Camera* camera,
     f32 left,
     f32 right,
     f32 bottom,
     f32 top,
     f32 nearClip,
     f32 farClip
  );
  static void SetFrustum(
     nw::gfx::Camera* camera,
     const nw::gfx::Viewport & viewport
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラのアスペクト比を設定する
   * 
   *                指定したレンダーターゲットに合うように設定されます。
   *                カメラがアタッチされているシーンを更新してから実行する必要があります。
   * 
   *  @param[in]    camera          設定するカメラ
   *  @param[in]    render_target   アスペクト比を合わせるレンダーターゲット
   */
  //-----------------------------------------------------------------------------
  static void SetCameraAspectRatio(
      nw::gfx::Camera*                camera,
      const nw::gfx::IRenderTarget*   render_target
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラのアスペクト比を設定する
   * 
   *                カメラがアタッチされているシーンを更新してから実行する必要があります。
   * 
   *  @param[in]    camera        設定するカメラ
   *  @param[in]    aspect_ratio   アスペクト比
   */
  //-----------------------------------------------------------------------------
  static void SetCameraAspectRatio(
      nw::gfx::Camera*  camera,
      f32               aspect_ratio
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの位置を得る
   *
   *  @param[in]    nw_camera          カメラ
   *  @param[out]   camera_position    カメラの位置を返す
   */
  //-----------------------------------------------------------------------------
  static void GetCameraPosition( const nw::gfx::Camera& nw_camera, gfl::math::VEC3& camera_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの注視点の位置を得る
   *
   *  @param[in]    nw_camera          カメラ
   *  @param[out]   target_position    カメラの注視点の位置を返す
   */
  //-----------------------------------------------------------------------------
  static void GetTargetPosition( const nw::gfx::Camera& nw_camera, gfl::math::VEC3& target_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの位置と注視点の位置を得る
   *
   *  @param[in]    nw_camera          カメラ
   *  @param[out]   camera_position    カメラの位置を返す
   *  @param[out]   target_position    カメラの注視点の位置を返す
   */
  //-----------------------------------------------------------------------------
  static void GetCameraAndTargetPosition( const nw::gfx::Camera& nw_camera, gfl::math::VEC3& camera_position, gfl::math::VEC3& target_position );

  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの位置を設定する
   *
   *  @param[in]    nw_camera          カメラ
   *  @param[in]    camera_position    カメラの位置
   */
  //-----------------------------------------------------------------------------
  static void SetCameraPosition( nw::gfx::Camera& nw_camera, const gfl::math::VEC3& camera_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの注視点の位置を設定する
   *
   *  @param[in]    nw_camera          カメラ
   *  @param[in]    target_position    カメラの注視点の位置
   */
  //-----------------------------------------------------------------------------
  static void SetTargetPosition( nw::gfx::Camera& nw_camera, const gfl::math::VEC3& target_position );
  //-----------------------------------------------------------------------------
  /**
   *  @brief        カメラの位置と注視点の位置を設定する
   *
   *  @param[in]    nw_camera          カメラ
   *  @param[in]    camera_position    カメラの位置
   *  @param[in]    target_position    カメラの注視点の位置
   */
  //-----------------------------------------------------------------------------
  static void SetCameraAndTargetPosition( nw::gfx::Camera& nw_camera, const gfl::math::VEC3& camera_position, const gfl::math::VEC3& target_position );




public:
  //
  // static 生成 / 破棄
  //

  //-----------------------------------------------------------------------------
  /**
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
   *  @retval       Camera*  生成したカメラクラス
   *
   * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
   */
  //-----------------------------------------------------------------------------
  static Camera* Create(
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
  /**
   *  @brief        立体視に使用するベースカメラと左右カメラを生成する
   *
   *                実際は右目用カメラと左目用カメラの区別はありません。
   *
   *  @param[out]   pp_base_camera     生成したベースカメラを返すためのポインタ
   *  @param[out]   pp_left_camera     生成した右目用カメラを返すためのポインタ
   *  @param[out]   pp_right_camera    生成した左目用カメラを返すためのポインタ
   *  @param[in]    allocator          生成に使用するアロケーター
   *  @param[in]    camera_position    カメラの位置座標
   *  @param[in]    target_position    カメラターゲットの座標
   *  @param[in]    near_clip          ニアクリップの値
   *  @param[in]    far_clip           ファークリップの値
   *  @param[in]    fovy_radian        FOVのY方向の値
   *  @param[in]    w_scale            WScaleの値
   *
   * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
   */
  //-----------------------------------------------------------------------------
  static void CreateStereoCameras(
      Camera**                    pp_base_camera,
      Camera**                    pp_left_camera,
      Camera**                    pp_right_camera,
      gfl::heap::NwAllocator*     allocator,
      const gfl::math::VEC3&      camera_position   = CAMERA_POSITION_DEFAULT,
      const gfl::math::VEC3&      target_position   = TARGET_POSITION_DEFAULT,
      f32                         near_clip         = NEAR_CLIP_DEFAULT,
      f32                         far_clip          = FAR_CLIP_DEFAULT,
      f32                         fovy_radian       = FOVY_RADIAN_DEFAULT,
      f32                         w_scale           = 0.0f
  );




public:
  //
  // NW4C関連 生成 / 破棄
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     nw_camera      カメラnw::gfx::Camera
   *                                引数で渡したこのnw_cameraは、今後このクラスで管理されるので解放しないように。
   *
   * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      nw::gfx::Camera*      nw_camera
  )
  {
    SetNwSceneNode(nw_camera);
  }


  //
  // NW4C関連 取得 / 設定
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief         このクラスが保持しているnw::gfx::Camera*を取得
   *
   *  @retval        nw::gfx::Camera*    このクラスが保持しているnw::gfx::Camera
   */
  //-----------------------------------------------------------------------------
  virtual nw::gfx::Camera*  GetNwCamera(void) const  {  return  nw::ut::DynamicCast<nw::gfx::Camera*>( GetNwSceneNode() );  } 


protected:
  virtual nw::gfx::SceneNode* GetNwSceneNode(void) const
  {
    return SceneNode::GetNwSceneNode();
  }


private:
  //
  // private関数
  //
  
  nw::gfx::CameraViewUpdater* CreateAimViewUpdater(
    gfl::heap::NwAllocator*    allocator,
    const gfl::math::VEC3&     target_position,
    f32                        twist  // [radian]
  );
  nw::gfx::CameraViewUpdater* CreateLookAtViewUpdater(
    gfl::heap::NwAllocator*    allocator,
    const gfl::math::VEC3&     target_position,
    const gfl::math::VEC3&     up_vector
  );
  nw::gfx::CameraViewUpdater* CreateRotateViewUpdater(
    gfl::heap::NwAllocator*    allocator,
    const gfl::math::VEC3&     rotate_xyz  // [radian]
  );
  nw::gfx::CameraProjectionUpdater* CreatePerspectiveProjectionUpdater(
    gfl::heap::NwAllocator*    allocator,
    f32                        fovy,  // [radian]
    f32                        aspect
  );
  nw::gfx::CameraProjectionUpdater* CreateFrustumProjectionUpdater(
    gfl::heap::NwAllocator*    allocator,
    f32                        aspect,
    f32                        height,
    const gfl::math::VEC2&     center
  );
  nw::gfx::CameraProjectionUpdater* CreateOrthoProjectionUpdater(
    gfl::heap::NwAllocator*    allocator,
    f32                        aspect,
    f32                        height,
    const gfl::math::VEC2&     center
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     allocator          アロケータ
   *  @param[in]     nw_scene_node      シーンノード
   *
   *  @retval        Camera*  生成したカメラクラス
   *
   * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
   */
  //-----------------------------------------------------------------------------
  static Camera* Create(
      gfl::heap::NwAllocator*           allocator,
      nw::gfx::SceneNode*               nw_scene_node
  );


  
  //----------------------------------------------------------------------------
  /**
   *	@brief  LookAtとAimモード時のアングル取得関数
   *
   *	@param	r   アングル格納先
   *	@param  view_mode ビューモード
   */
  //-----------------------------------------------------------------------------
  void GetAimAndTwistRotateXYZ( gfl::math::VEC3* r, ViewMode view_mode ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  LookAtとAimモード時のアングル設定関数
   *
   *	@param	x   X軸回転角度
   *	@param	y   Y軸回転角度
   *	@param	z   Z軸回転角度
   *	@param  view_mode ビューモード
   */
  //-----------------------------------------------------------------------------
  void SetAimAndTwistRotateXYZ( f32 x, f32 y, f32 z, ViewMode view_mode );
  


protected:
  //Animation*  m_animation;
  u32                m_animation_slot_num;  // 0か1  // 0のときアニメーションを付けられない、1のときアニメーションを付けられる(実際に付けているかどうかはm_animation_managerで確認して下さい)。
  u32                m_animation_blend_unit_num;
  u32                m_animation_num_in_unit;
  AnimationManager*  m_animation_manager;

};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DCAMERA_H__
