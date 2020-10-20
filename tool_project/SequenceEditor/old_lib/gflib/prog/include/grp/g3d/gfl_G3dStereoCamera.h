#ifndef __GFL_G3DSTEREOCAMERA_H__
#define __GFL_G3DSTEREOCAMERA_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dStereoCamera.h
 *	@brief  3Dステレオカメラ
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK
#include <nn/ulcd.h>

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dCamera.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3Dステレオカメラクラス
//=====================================
class StereoCamera : public Camera
{
  GFL_FORBID_COPY_AND_ASSIGN(StereoCamera);


  friend class AnimationManager;


public:
  //
  // 定数
  //

  //-----------------------------------------------------------------------------
  /*
   *    デフォルト値
   */
  //-----------------------------------------------------------------------------
  static const f32 DEPTH_LEVEL_DEFAULT;
  static const f32 DEPTH_RANGE_DEFAULT;
  static const b32 REAL_SWITCH_DEFAULT;




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
  StereoCamera(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~StereoCamera();


#if 0
  /*C++ Tips!
  継承、デフォルト引数で注意すること！
  class A
  {
  virtual void Create(s32 a);
  };
  class B : public A
  {
  virtual void Create(s32 a, s32 b=1);
  };
  A* p = new B;
  p->Create(0);
  とするとA::Createが呼ばれるので、注意！
  ↓
  基底クラスでCameraのCreateと本クラスStereoCameraのCreateを注意してつくった。*/
#endif

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
   *
   *  depth_level は DEPTH_LEVEL_DEFAULT
   *  depth_range は DEPTH_RANGE_DEFAULT 
   *  real_switch は REAL_SWITCH_DEFAULT
   *  が設定される。
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
   *  @param[in]    allocator          アロケータ
   *  @param[in]    device_allocator   デバイスメモリのアロケータ
   *  @param[in]    resource           リソース
   *  @param[in]    index_in_resource  リソース内におけるインデックス
   *  @param[in]    depth_level        ベースカメラから基準面までの距離
   *  @param[in]    depth_range        視差の補正値
   *  @param[in]    real_switch        現実感を出すために視野角の変更を許すか(trueのとき許す)
   *  @param[in]    description        リソースからカメラを生成するときの設定内容
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*          allocator,
      gfl::heap::NwAllocator*          device_allocator,
      Resource*                        resource,
      s32                              index_in_resource,
      f32                              depth_level,
      f32                              depth_range       = DEPTH_RANGE_DEFAULT,
      b32                              real_switch       = REAL_SWITCH_DEFAULT,
      ResourceDescription*             description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     description    
   *
   *  depth_level は DEPTH_LEVEL_DEFAULT
   *  depth_range は DEPTH_RANGE_DEFAULT 
   *  real_switch は REAL_SWITCH_DEFAULT
   *  が設定される。
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
   *  @brief         生成
   *
   *  @param[in]    description    
   *  @param[in]    depth_level        ベースカメラから基準面までの距離
   *  @param[in]    depth_range        視差の補正値
   *  @param[in]    real_switch        現実感を出すために視野角の変更を許すか(trueのとき許す)
   *
   * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*    allocator,
      Description*               description,
      f32                        depth_level,
      f32                        depth_range       = DEPTH_RANGE_DEFAULT,
      b32                        real_switch       = REAL_SWITCH_DEFAULT
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]    allocator          アロケータ
   *  @param[in]    base_camera        ベースカメラ
   *  @param[in]    base_camera_owner_this  これにbase_cameraを管理してもらう場合はtrue(trueにしたときはこれがbase_cameraを解放します)
   *  @param[in]    depth_level        ベースカメラから基準面までの距離
   *  @param[in]    depth_range        視差の補正値
   *  @param[in]    real_switch        現実感を出すために視野角の変更を許すか(trueのとき許す)
   *
   * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*          allocator,
      Camera*                          base_camera,
      b32                              base_camera_owner_this = false,
      f32                              depth_level       = DEPTH_LEVEL_DEFAULT,
      f32                              depth_range       = DEPTH_RANGE_DEFAULT,
      b32                              real_switch       = REAL_SWITCH_DEFAULT
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
  // カメラマトリクス
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ビューマトリクスとプロジェクションマトリクスを更新する
   *                ベースカメラ、左目用カメラ、右目用カメラのビューマトリクスとプロジェクションマトリクスを更新する。
   */
  //-----------------------------------------------------------------------------
  virtual void UpdateCameraMatrix(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ビューマトリクスとプロジェクションマトリクスを更新する
   *                (ベースカメラ、)左目用カメラ、右目用カメラのビューマトリクスとプロジェクションマトリクスを更新する。
   *
   *  @param[in]    update_base_camera  trueのとき、ベースカメラ、左目用カメラ、右目用カメラのビューマトリクスとプロジェクションマトリクスを更新する。
   *                                    falseのとき、ベースカメラのビューマトリクスとプロジェクションマトリクスは更新せず、
   *                                                 左目用カメラ、右目用カメラのビューマトリクスとプロジェクションマトリクスを更新する。
   *                                                 その際、ベースカメラに現在設定してあるビューマトリクスとプロジェクションマトリクスを使用します。
   */
  //-----------------------------------------------------------------------------
  void UpdateCameraMatrices(b32 update_base_camera = true);

  // 以下は全てベースカメラに対しての関数となります。
#if 0
  void GetViewMatrix( gfl::math::MTX34* view_matrix ) const;
  void SetViewMatrix( const gfl::math::MTX34& view_matrix );
  void GetProjectionMatrix( gfl::math::MTX44* projection_matrix ) const;
  void SetProjectionMatrix( const gfl::math::MTX44& projection_matrix );
  void GetViewAndProjectionMatrix( gfl::math::MTX34* view_matrix, gfl::math::MTX44* projection_matrix ) const;
  void SetViewAndProjectionMatrix( const gfl::math::MTX34& view_matrix, const gfl::math::MTX44& projection_matrix );
#endif

  // 以下は昔はベースカメラに対しての関数だったのですが、今はベース、左目用、右目用に対しての関数となります。
#if 0
  void UpdateCameraMatrix(void);
#endif




  //
  // 取得 / 設定
  //

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ベースカメラから基準面までの距離を設定する
   *
   *  @param[in]    depth_level     ベースカメラから基準面までの距離
   */
  //-----------------------------------------------------------------------------
  void SetDepthLevel(f32 depth_level) {  m_depth_level = depth_level;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        視差の補正値を設定する
   *
   *  @param[in]    depth_range     視差の補正値
   */
  //-----------------------------------------------------------------------------
  void SetDepthRange(f32 depth_range)
  {
    // Panic: ulcd_StereoCamera.cpp:685
    // factor must be [0,1].

    if(depth_range<0.0f)      m_depth_range = 0.0f;
    else if(depth_range>1.0f) m_depth_range = 1.0f;
    else                      m_depth_range = depth_range;
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        現実感を出すために視野角の変更を許すかのフラグを設定する
   *
   *  @param[in]    real_switch     現実感を出すために視野角の変更を許すか(trueのとき許す)
   */
  //-----------------------------------------------------------------------------
  void SetRealSwitch(b32 real_switch) {  m_real_switch = real_switch;  }

  //-----------------------------------------------------------------------------
  /**
   * @brief 奥行き方向の限界視差を設定します。
   *        limitには、0以上かつガイドラインで定められている奥行き方向の視差の上限値以下の値を設定できます。ただし、現在は上限値以上の値を指定すると警告を出力するのみで、値は反映されます。
   *        limitはCalculateMatrices関数の結果に反映されます。具体的には、ファークリップ面の位置のオブジェクトが、LCD表面上において本関数で指定した視差をつけられるように計算が行われます。
   *        本関数で設定する限界視差は、「アプリケーション優先の方法」かつ「奥行き方向」に対してのみ有効です。
   *        本クラスでいえば、SetRealSwitch(false)の時のみ有効です。
   *
   * @param fLimit 設定する限界視差(mm)
   */
  //-----------------------------------------------------------------------------
  void SetLimitParallax(const float fLimit) { m_nn_stereo_camera->SetLimitParallax(fLimit); }

  //-----------------------------------------------------------------------------
  /**
   * @brief 設定されている奥行き方向の限界視差を取得します。
   *        一度も SetLimitParallax 関数を使用していない場合は、デフォルトとして ガイドラインで定められている最大の限界視差（奥行き方向）が返されます。 
   *
   *        ガイドラインに変更がなければ10.0fが最大の限界視差。
   *
   * @retval 設定中の限界視差(mm)
   */
  //-----------------------------------------------------------------------------
  f32 GetLimitParallax(void) const { return m_nn_stereo_camera->GetLimitParallax(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief        ベースカメラから基準面までの距離を取得する
   *
   *  @retval       f32    ベースカメラから基準面までの距離
   */
  //-----------------------------------------------------------------------------
  f32 GetDepthLevel(void) const {  return m_depth_level;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        視差の補正値を取得する
   *
   *  @retval       f32    視差の補正値
   */
  //-----------------------------------------------------------------------------
  f32 GetDepthRange(void) const {  return m_depth_range;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        現実感を出すために視野角の変更を許すかのフラグを取得する
   *
   *  @retval       b32    現実感を出すために視野角の変更を許すか(trueのとき許す)
   */
  //-----------------------------------------------------------------------------
  b32 GetRealSwitch(void) const {  return m_real_switch;  }
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief        ベースカメラを取得する
   *
   *  @retval       Camera*    ベースカメラ
   */
  //-----------------------------------------------------------------------------
  Camera* GetBaseCamera(void) const
  {
    if( m_base_camera )
    {
      return m_base_camera;
    }
    else
    {
      const Camera* base_camera = this;         // thisを使っているせいかconst関数にできなかったので、
      return const_cast<Camera*>(base_camera);  // こんな風にしています。
    }
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        左目用カメラを取得する
   *
   *  @retval       Camera*    左目用カメラ
   */
  //-----------------------------------------------------------------------------
  Camera* GetLeftCamera(void) const {  return m_left_camera;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief        右目用カメラを取得する
   *
   *  @retval       Camera*    右目用カメラ
   */
  //-----------------------------------------------------------------------------
  Camera* GetRightCamera(void) const {  return m_right_camera;  }




  //
  // 生成 / 破棄
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
   *  @param[in]    depth_level        ベースカメラから基準面までの距離
   *  @param[in]    depth_range        視差の補正値
   *  @param[in]    real_switch        現実感を出すために視野角の変更を許すか(trueのとき許す)
   *
   * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
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

  virtual void CreateFrustum(
    gfl::heap::NwAllocator*          allocator,
    const gfl::math::VEC3& cameraPosition,
    const gfl::math::VEC3& targetPosition,
    f32 nearClip,
    f32 farClip,
    const gfl::math::VEC2& projectionCenter,
    f32 projectionHeight,
    gfl::math::PivotDirection pivotDirection,
    f32 w_scale,
    f32                              depth_level       = DEPTH_LEVEL_DEFAULT,
    f32                              depth_range       = DEPTH_RANGE_DEFAULT,
    b32                              real_switch       = REAL_SWITCH_DEFAULT
  );



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
  virtual nw::gfx::Camera* GetNwCamera(void) const
  {
  #if 1
    if( m_base_camera )
    {
      return m_base_camera->GetNwCamera();
    }
    else
    {
      return Camera::GetNwCamera();
    }
  #else
    return GetBaseCamera()->Camera::GetNwCamera();  // virtualにしてあるので、Camera::を付けておかないと、StereoCameraのGetNwCamera関数が何度も呼び出されてしまう。
  #endif
  } 
  // 直接GetNwCameraやGetNwSceneNodeを呼べないようにし、代わりにGetNwCameraOfBaseとかを用意すべき。
  // しかしこれが使えないと基底クラスCameraのアニメーションで困ったことになる。


protected:
  virtual nw::gfx::SceneNode* GetNwSceneNode(void) const
  {
    if( m_base_camera )
    {
      return m_base_camera->GetNwSceneNode();  // 親クラスのprotected関数をインスタンスから呼ぶことはできない。→子クラスを親クラスのfriendにした。
    }
    else
    {
      return Camera::GetNwSceneNode();  // 親クラスのprotected関数を直接呼ぶことはできる。
    }
  }



private:
  b32                           m_base_camera_owner_this;  // m_base_camera!=NULLのときのみ有効
                                                           // これがm_base_cameraを管理する場合はtrue(trueのときはこれがm_base_cameraを解放します)
  Camera*                       m_base_camera;             // ベースカメラ  // 基底クラスのm_nw_scene_nodeを使う場合はNULL
  
  Camera*                       m_left_camera;    // 左目用カメラ
  Camera*                       m_right_camera;   // 右目用カメラ

  nn::ulcd::CTR::StereoCamera*  m_nn_stereo_camera;
  
  f32                           m_depth_level;
  f32                           m_depth_range;
  b32                           m_real_switch;
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSTEREOCAMERA_H__
