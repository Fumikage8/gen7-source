//======================================================================
/**
 * @file FieldCameraControllerTypePokeFinderForPlacement.cpp
 * @date 2015/10/09 12:58:13
 * @author miyachi_soichi
 * @brief カメラ：ポケファインダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <debug/include/gfl2_Assert.h>
// GameSys
#include "GameSys/include/GameManager.h"
// camera
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypePokeFinderForPlacement.h"
// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Camera)
GFL_NAMESPACE_BEGIN(Controller)

/**
 *  @class  ControllerTypePokeFinderForPlacement
 *  @brief  カメラ：ポケファインダー
 */
/**
 *  @brief  コンストラクタ
 */
ControllerTypePokeFinderForPlacement::ControllerTypePokeFinderForPlacement( const SetupParam &data )
{
  m_Position = data.position;
  m_Rotate_Base = data.rotate;
  m_Rotate_Add = gfl2::math::Vector3::GetZero();
  m_Limit_min = data.limit_min;
  m_Limit_max = data.limit_max;
  m_Zoom = 1.0f;
  m_Zoom_Mini = 1.0f;
  m_Zoom_Maxi = data.zoom_maxi;
  m_Stick_Mini = 0.0f;
  m_Stick_Maxi = 0.8f;
  m_Stick_Ratio = 2.0f;
  m_Stick_Counter = 0.25f;
  m_Gyro_Ratio = 0.8f;
  m_IsInputEnable = false;
  m_IsGyroEnable = false;
  m_IsZoomInput = false;
#if PM_DEBUG
  debug_LockZoom = false;
#endif
}

/**
 *  @brief  デストラクタ
 */
ControllerTypePokeFinderForPlacement::~ControllerTypePokeFinderForPlacement( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化処理
 */
void ControllerTypePokeFinderForPlacement::SetUp( poke_3d::model::BaseCamera *pCamera )
{
}

/**
 *  @brief  更新処理
 */
void ControllerTypePokeFinderForPlacement::Update( poke_3d::model::BaseCamera *pCamera )
{
  if( GetInputEnable() )
  {
    // 操作による更新
    updatePad( pCamera );
    // ジャイロの更新
    updateGyro( pCamera );
  }
  // カメラの更新
  updateCamera( pCamera );
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  パッドの更新
 */
void ControllerTypePokeFinderForPlacement::updatePad( poke_3d::model::BaseCamera *pCamera )
{
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager *pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice *pVecDevice = pDeviceManager->GetStick( gfl2::ui::DeviceManager::STICK_STANDARD );
  gfl2::ui::Button *pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  // 距離
#if PM_DEBUG
  if( !debug_LockZoom )
#endif
  {
    f32 add = 0.1f;
    m_IsZoomInput = false;
    if( pButton->IsHold( gfl2::ui::BUTTON_UP   ) ){ m_Zoom += add; m_IsZoomInput = true; }
    if( pButton->IsHold( gfl2::ui::BUTTON_DOWN ) ){ m_Zoom -= add; m_IsZoomInput = true; }
    m_Zoom = gfl2::math::Clamp( m_Zoom, m_Zoom_Mini, m_Zoom_Maxi );
  }
  // 回転
  {
    f32 stickValueX = processStickValue( pVecDevice->GetY() );
    f32 stickValueY = processStickValue( pVecDevice->GetX() );
    f32 ratio_x = gfl2::math::ConvDegToRad( m_Stick_Ratio );
    f32 ratio_y = gfl2::math::ConvDegToRad( m_Stick_Ratio );
    f32 counter = calcCounterZoom();
    stickValueX *= ( ratio_x * counter );
    stickValueY *= ( ratio_y * counter );

    m_Rotate_Add.x = gfl2::math::NormalizeRadianAngle( m_Rotate_Add.x + stickValueX );
    m_Rotate_Add.y = gfl2::math::NormalizeRadianAngle( m_Rotate_Add.y + stickValueY );

    m_Rotate_Add.x = gfl2::math::Clamp( m_Rotate_Add.x, m_Limit_min.x, m_Limit_max.x );
    m_Rotate_Add.y = gfl2::math::Clamp( m_Rotate_Add.y, m_Limit_min.y, m_Limit_max.y );
  }
}

/**
 *  @brief  ジャイロの更新
 */
void ControllerTypePokeFinderForPlacement::updateGyro( poke_3d::model::BaseCamera *pCamera )
{
  // ジャイロチェック
  if( !GetGyroEnable() )
  {
    return;
  }
#if defined(GF_PLATFORM_CTR)
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager *pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Gyroscope *pGyro = pDeviceManager->GetGyroscope( gfl2::ui::DeviceManager::GYROSCOPE_STANDARD );
  gfl2::ui::Accelerometer *pAccel = pDeviceManager->GetAccelerometer( gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD );

  f32 conv = 360.0f / 30.0f;
  gfl2::math::VEC3 spd;
  spd = pGyro->GetSpeed() * conv * m_Gyro_Ratio;
  spd.x = gfl2::math::ConvDegToRad( spd.x );
  spd.y = gfl2::math::ConvDegToRad( spd.y );
  spd.z = gfl2::math::ConvDegToRad( spd.z );

  f32 dstX = spd.x;
  f32 dstY = spd.z;

  m_Rotate_Add.x = gfl2::math::NormalizeRadianAngle( m_Rotate_Add.x + dstX );
  m_Rotate_Add.y = gfl2::math::NormalizeRadianAngle( m_Rotate_Add.y + dstY );

  m_Rotate_Add.x = gfl2::math::Clamp( m_Rotate_Add.x, m_Limit_min.x, m_Limit_max.x );
  m_Rotate_Add.y = gfl2::math::Clamp( m_Rotate_Add.y, m_Limit_min.y, m_Limit_max.y );

#endif
}

/**
 *  @brief  カメラの更新
 */
void ControllerTypePokeFinderForPlacement::updateCamera( poke_3d::model::BaseCamera *pCamera )
{
  gfl2::math::Vector3 targetPos;
  gfl2::math::Vector3 cameraPos = m_Position;
  gfl2::math::Vector3 targetOffset( 0.0f, 0.0f, 1.0f );

  gfl2::math::Vector3 rotate = m_Rotate_Base + m_Rotate_Add;
  gfl2::math::Matrix34 mtx = gfl2::math::Matrix34::GetRotationZYX( rotate.z, rotate.y, rotate.x );
  targetOffset = mtx.TransformCoord( targetOffset );

  targetPos = targetOffset + cameraPos;

  pCamera->SetupCameraLookAt(
    cameraPos, targetPos,
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f )
    );
  pCamera->SetFovy( gfl2::math::ConvDegToRad( 35.0f / m_Zoom ) );
}

/**
 *  @brief  スティック入力値を加工する
 */
f32 ControllerTypePokeFinderForPlacement::processStickValue( f32 value )
{
  f32 fResult = value;
  f32 abs_value = gfl2::math::Abs<f32>( value );
  f32 sign = 1.0f; if( value >= 0.0f ){ sign = -1.0f; }

  // 足きり
  if( abs_value < m_Stick_Mini )
  {
    abs_value = 0.0f;
  }
  // 最大丸め込み
  if( m_Stick_Maxi < abs_value )
  {
    abs_value = m_Stick_Maxi;
  }

  fResult = abs_value * abs_value * sign;

  return fResult;
}

/**
 *  @brief  カウンターズーム
 */
f32 ControllerTypePokeFinderForPlacement::calcCounterZoom( void )
{
  f32 dst_zoom = -1.0f * ( m_Zoom - 1.0f ) * m_Stick_Counter + 1.0f;
  return dst_zoom;
}

GFL_NAMESPACE_END(Controller)
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(Field)
