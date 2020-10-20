//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dStereoCamera.cpp
 *	@brief  3Dステレオカメラ
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK
#include <nn/ulcd.h>
#include <nn/math.h>

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Math.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dCamera.h>

#include <grp/g3d/gfl_G3dStereoCamera.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dステレオカメラクラス
//=====================================

//
// 定数
//

//-----------------------------------------------------------------------------
/*
 *    デフォルト値
 */
//-----------------------------------------------------------------------------
const f32 StereoCamera::DEPTH_LEVEL_DEFAULT = Camera::NEAR_CLIP_DEFAULT + 5.0f;
const f32 StereoCamera::DEPTH_RANGE_DEFAULT = 0.0f;
const b32 StereoCamera::REAL_SWITCH_DEFAULT = false;




//
// コンストラクタ / デストラクタ
//

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 Createは呼ばれません。
 */
//-----------------------------------------------------------------------------
StereoCamera::StereoCamera(void)
  : Camera(),
    m_base_camera_owner_this(false),
    m_base_camera(NULL),
    m_left_camera(NULL),
    m_right_camera(NULL),
    m_nn_stereo_camera(NULL),
    m_depth_level(DEPTH_LEVEL_DEFAULT),
    m_depth_range(DEPTH_RANGE_DEFAULT),
    m_real_switch(REAL_SWITCH_DEFAULT)
{
  // 基底クラスであるCameraのコンストラクタで登録したものを削除する
  GFL_SINGLETON_INSTANCE(System)->GetCameraManager()->RemoveObject(this);
  // このコンストラクタで登録し直す
  GFL_SINGLETON_INSTANCE(System)->GetStereoCameraManager()->AddObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 *                 Destroyを呼びます。
 */
//-----------------------------------------------------------------------------
StereoCamera::~StereoCamera()
{
  Destroy();

  // このコンストラクタで登録したものを削除する
  GFL_SINGLETON_INSTANCE(System)->GetStereoCameraManager()->RemoveObject(this);
  // 基底クラスであるCameraのマネージャに登録し直す
  GFL_SINGLETON_INSTANCE(System)->GetCameraManager()->AddObject(this);
}




//
// 生成 / 破棄
//

//-----------------------------------------------------------------------------
/*
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
void StereoCamera::Create(
    gfl::heap::NwAllocator*           allocator,
    gfl::heap::NwAllocator*           device_allocator,
    Resource*                         resource,
    s32                               index_in_resource,
    ResourceDescription*              description
)
{
  this->Create(
      allocator,
      device_allocator,
      resource,
      index_in_resource,
      DEPTH_LEVEL_DEFAULT,
      DEPTH_RANGE_DEFAULT,
      REAL_SWITCH_DEFAULT,
      description
  );
}

//-----------------------------------------------------------------------------
/*
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
void StereoCamera::Create(
    gfl::heap::NwAllocator*          allocator,
    gfl::heap::NwAllocator*          device_allocator,
    Resource*                        resource,
    s32                              index_in_resource,
    f32                              depth_level,
    f32                              depth_range,
    b32                              real_switch,
    ResourceDescription*             description
)
{
  ResourceDescription l_description;
  if( description )
  {
    l_description = *description;
  }

  // ベースカメラ
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "リソースが不正のためシーンオブジェクトが生成できませんでした。\n" );
    return;
  }

  m_animation_slot_num = (l_description.animation_enable)?(1):(0);
  m_animation_blend_unit_num = l_description.animation_blend_unit_num;
  m_animation_num_in_unit = l_description.animation_num_in_unit;

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResCamera res = nw_res_graphics_file->GetCameras(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      (m_animation_slot_num>0)?(true):(false),  // ここがfalseだとnw::gfx::AnimGroupが取得できない  // これを外部から設定できるようにするか、アニメーションなしカメラクラスをつくるか
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      m_animation_slot_num
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%dのシーンオブジェクトが生成できませんでした。\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  //m_animation_manager = GFL_NEW(allocator->GetHeapBase()) AnimationManager;
  //m_animation_manager->NoSlot_Create(
  //    allocator,
  //    allocator,
  //    this
  //);

  // 左目用カメラ
  nw::gfx::Camera* nw_left_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
  GFL_ASSERT_STOP(nw_left_camera);
  // SetWScaleするならここ
  m_left_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_left_camera->Create(nw_left_camera);
    
  // 右目用カメラ
  nw::gfx::Camera* nw_right_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
  GFL_ASSERT_STOP(nw_right_camera);
  // SetWScaleするならここ
  m_right_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_right_camera->Create(nw_right_camera);

  m_depth_level = depth_level;
  m_depth_range = depth_range;
  m_real_switch = real_switch;

  // ステレオカメラを生成する
  m_nn_stereo_camera = GFL_NEW(allocator->GetHeapBase()) nn::ulcd::CTR::StereoCamera;
      // 初期化を行わないため、別途、Initialize を呼ぶ必要があります。
  m_nn_stereo_camera->Initialize();
}

//-----------------------------------------------------------------------------
/*
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
void StereoCamera::Create(
    gfl::heap::NwAllocator*    allocator,
    Description*               description
)
{
  this->Create(
      allocator,
      description,
      DEPTH_LEVEL_DEFAULT,
      DEPTH_RANGE_DEFAULT,
      REAL_SWITCH_DEFAULT
  );
}
//-----------------------------------------------------------------------------
/*
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
void StereoCamera::Create(
    gfl::heap::NwAllocator*    allocator,
    Description*               description,
    f32                        depth_level,
    f32                        depth_range,
    b32                        real_switch
)
{
  Camera* base_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  base_camera->Create(
      allocator,
      description
  );

  this->Create(
    allocator,
    base_camera,
    true,
    depth_level,
    depth_range,
    real_switch
  );
}

//-----------------------------------------------------------------------------
/*
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
void StereoCamera::Create(
    gfl::heap::NwAllocator*          allocator,
    Camera*                          base_camera,
    b32                              base_camera_owner_this,
    f32                              depth_level,
    f32                              depth_range,
    b32                              real_switch
)
{
  // ベースカメラ
  m_base_camera_owner_this = base_camera_owner_this;
  m_base_camera = base_camera; 

  // 左目用カメラ
  nw::gfx::Camera* nw_left_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
  GFL_ASSERT_STOP(nw_left_camera);
  // SetWScaleするならここ。
  m_left_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_left_camera->Create(nw_left_camera);
    
  // 右目用カメラ
  nw::gfx::Camera* nw_right_camera = 
      nw::gfx::Camera::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(allocator);
  GFL_ASSERT_STOP(nw_right_camera);
  // SetWScaleするならここ。
  m_right_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_right_camera->Create(nw_right_camera);

  m_depth_level = depth_level;
  m_depth_range = depth_range;
  m_real_switch = real_switch;

  // ステレオカメラを生成する
  m_nn_stereo_camera = GFL_NEW(allocator->GetHeapBase()) nn::ulcd::CTR::StereoCamera;
      // 初期化を行わないため、別途、Initialize を呼ぶ必要があります。
  m_nn_stereo_camera->Initialize();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
 */
//-----------------------------------------------------------------------------
void StereoCamera::Destroy(void)
{
  if( m_nn_stereo_camera )
  {
    GFL_DELETE m_nn_stereo_camera;
        // 内部で Finalize() を呼び出します。
  }

  if( m_right_camera )
  {
    GFL_DELETE m_right_camera;
  }
  if( m_left_camera )
  {
    GFL_DELETE m_left_camera;
  }

  if( m_base_camera )
  {
    if( m_base_camera_owner_this )
    {
      GFL_DELETE m_base_camera;
    }
  }

  Camera::Destroy();

  m_base_camera_owner_this = false;
  m_base_camera            = NULL;

  m_left_camera         = NULL;
  m_right_camera        = NULL;
 
  m_nn_stereo_camera    = NULL;

  m_depth_level         = DEPTH_LEVEL_DEFAULT;
  m_depth_range         = DEPTH_RANGE_DEFAULT;
  m_real_switch         = REAL_SWITCH_DEFAULT;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         計算処理
 */
//-----------------------------------------------------------------------------
void StereoCamera::Calculate(void)
{
  Camera::Calculate();
}




//
// カメラマトリクス
//

//-----------------------------------------------------------------------------
/*
 *  @brief        ビューマトリクスとプロジェクションマトリクスを更新する
 *                ベースカメラ、左目用カメラ、右目用カメラのビューマトリクスとプロジェクションマトリクスを更新する。
 *
 *  @param[in]    update_base_camera  trueのとき、ベースカメラ、左目用カメラ、右目用カメラのビューマトリクスとプロジェクションマトリクスを更新する。
 *                                    falseのとき、ベースカメラのビューマトリクスとプロジェクションマトリクスは更新せず、
 *                                                 左目用カメラ、右目用カメラのビューマトリクスとプロジェクションマトリクスを更新する。
 *                                                 その際、ベースカメラに現在設定してあるビューマトリクスとプロジェクションマトリクスを使用します。
 */
//-----------------------------------------------------------------------------
void StereoCamera::UpdateCameraMatrix(void)
{
  UpdateCameraMatrices(true);
}

//-----------------------------------------------------------------------------
/*
 *  @brief        ビューマトリクスとプロジェクションマトリクスを更新する
 *                (ベースカメラ、)左目用カメラ、右目用カメラのビューマトリクスとプロジェクションマトリクスを更新する。
 *
 *  @param[in]    update_base_camera  trueのとき、ベースカメラ、左目用カメラ、右目用カメラのビューマトリクスとプロジェクションマトリクスを更新する。
 *                                    falseのとき、ベースカメラのビューマトリクスとプロジェクションマトリクスは更新せず、
 *                                                 左目用カメラ、右目用カメラのビューマトリクスとプロジェクションマトリクスを更新する。
 *                                                 その際、ベースカメラに現在設定してあるビューマトリクスとプロジェクションマトリクスを使用します。
 */
//-----------------------------------------------------------------------------
void StereoCamera::UpdateCameraMatrices(b32 update_base_camera)
{
  // ベースカメラ
  if( update_base_camera )
  {
    GetBaseCamera()->Camera::UpdateCameraMatrix();
  }

  // 左目用カメラ、右目用カメラ
  gfl::math::MTX44& base_proj  = GetBaseCamera()->GetNwCamera()->ProjectionMatrix();
  gfl::math::MTX34& base_view  = GetBaseCamera()->GetNwCamera()->ViewMatrix();
  gfl::math::MTX44& left_proj  = m_left_camera->GetNwCamera()->ProjectionMatrix();
  gfl::math::MTX34& left_view  = m_left_camera->GetNwCamera()->ViewMatrix();
  gfl::math::MTX44& right_proj = m_right_camera->GetNwCamera()->ProjectionMatrix();
  gfl::math::MTX34& right_view = m_right_camera->GetNwCamera()->ViewMatrix();

  // Orthoカメラをステレオ表示する場合は自分で絵を用意しなければいけません。
  if( GetBaseCamera()->GetNwCamera()->GetResCamera().GetProjectionType() == nw::gfx::ResCamera::PROJTYPE_ORTHO )
  {
    // @note 現在はOrthoカメラのステレオ表示には未対応です。
    gfl::math::MTX44Copy(&left_proj, &base_proj);
    gfl::math::MTX34Copy(&left_view, &base_view);
    gfl::math::MTX44Copy(&right_proj, &base_proj);
    gfl::math::MTX34Copy(&right_view, &base_view);

    // nn::ulcd::CTR::StereoCamera::CalculateMatrices(....)で画面にあわせての回転を行っているので、
    // Orthoの場合はpivotを設定します。
    GetBaseCamera()->GetNwCamera()->GetProjectionUpdater()->SetPivotDirection(static_cast<nn::math::PivotDirection>(gfl::math::PIVOT_UPSIDE_TO_TOP));
    m_left_camera->GetNwCamera()->GetProjectionUpdater()->SetPivotDirection(static_cast<nn::math::PivotDirection>(gfl::math::PIVOT_UPSIDE_TO_TOP));
    m_right_camera->GetNwCamera()->GetProjectionUpdater()->SetPivotDirection(static_cast<nn::math::PivotDirection>(gfl::math::PIVOT_UPSIDE_TO_TOP));
  }
  else
  {
    // ゲームとしてステレオ描画を禁止している際の対処
    f32 depth_range = 0.0f;
    if( GraphicsSystem::IsGameStereoFlag() ){
      if( nn::gx::IsStereoVisionAllowed()  ){
        depth_range = m_depth_range;
      }
    }

    m_nn_stereo_camera->CalculateMatrices(
        &left_proj,
        &left_view,
        &right_proj,
        &right_view,
        &base_proj,
        &base_view,
        m_depth_level,
        depth_range,
        m_real_switch
    );
  }

  // 逆行列を更新する
  GetLeftCamera()->SetProjectionMatrix(left_proj);
  GetLeftCamera()->SetViewMatrix(left_view);
  GetRightCamera()->SetProjectionMatrix(right_proj);
  GetRightCamera()->SetViewMatrix(right_view);
}




//
// 生成 / 破棄
//

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
 *
 * 【注意】プログラムでカメラをつくったときはアニメーションを設定できない。
 */
//-----------------------------------------------------------------------------
void StereoCamera::Create(
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
  nw::gfx::Camera* nw_base_camera;
  nw::gfx::Camera* nw_left_camera;
  nw::gfx::Camera* nw_right_camera;

  CreateStereoCameras(
    &nw_base_camera,
    &nw_left_camera,
    &nw_right_camera,
    allocator,
    camera_position,
    target_position,
    near_clip,
    far_clip,
    fovy_radian,
    w_scale
  );

  SetNwSceneNode(nw_base_camera);
 
  m_left_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_left_camera->Create(nw_left_camera);
  
  m_right_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_right_camera->Create(nw_right_camera);

  m_depth_level = depth_level;
  m_depth_range = depth_range;
  m_real_switch = real_switch;

  // ステレオカメラを生成する
  m_nn_stereo_camera = GFL_NEW(allocator->GetHeapBase()) nn::ulcd::CTR::StereoCamera;
      // 初期化を行わないため、別途、Initialize を呼ぶ必要があります。
  m_nn_stereo_camera->Initialize();
}

void StereoCamera::CreateFrustum(
  gfl::heap::NwAllocator*          allocator,
  const gfl::math::VEC3& cameraPosition,
  const gfl::math::VEC3& targetPosition,
  f32 nearClip,
  f32 farClip,
  const gfl::math::VEC2& projectionCenter,
  f32 projectionHeight,
  gfl::math::PivotDirection pivotDirection,
  f32 w_scale,

    f32                              depth_level,
    f32                              depth_range,
    b32                              real_switch
    )
{
  nw::gfx::Camera* nw_base_camera;
  nw::gfx::Camera* nw_left_camera;
  nw::gfx::Camera* nw_right_camera;

  CreateFrustumStereoCameras(
    &nw_base_camera,
    &nw_left_camera,
    &nw_right_camera,
    allocator,
    cameraPosition,
    targetPosition,
    nearClip,
    farClip,
    projectionCenter,
    projectionHeight,
    pivotDirection,
    w_scale
  );

  SetNwSceneNode(nw_base_camera);
 
  m_left_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_left_camera->Create(nw_left_camera);
  
  m_right_camera = GFL_NEW(allocator->GetHeapBase()) Camera;
  m_right_camera->Create(nw_right_camera);

  m_depth_level = depth_level;
  m_depth_range = depth_range;
  m_real_switch = real_switch;

  // ステレオカメラを生成する
  m_nn_stereo_camera = GFL_NEW(allocator->GetHeapBase()) nn::ulcd::CTR::StereoCamera;
      // 初期化を行わないため、別途、Initialize を呼ぶ必要があります。
  m_nn_stereo_camera->Initialize();
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

