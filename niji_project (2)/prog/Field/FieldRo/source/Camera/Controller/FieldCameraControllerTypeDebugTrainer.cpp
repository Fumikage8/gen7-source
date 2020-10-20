//======================================================================
/**
 * @file FieldCameraControllerTypeDebugTrainer.cpp
 * @date 2015/10/05 12:43:45
 * @author miyachi_soichi
 * @brief デバッグカメラ：トレーナー用
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG
// GameSys
#include "GameSys/include/GameManager.h"
// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeDebugTrainer.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Camera)
GFL_NAMESPACE_BEGIN(Controller)

/**
 *  @class  ControllerTypeDebugTrainer
 *  @brief  トレーナーデバッグ用カメラ
 */
/**
 *  @brief  コンストラクタ
 */
ControllerTypeDebugTrainer::ControllerTypeDebugTrainer( const Target::TargetBase *pTarget )
: m_pTarget( pTarget )
{
  m_fDistance = 1000.0f;
}

/**
 *  @brief  デストラクタ
 */
ControllerTypeDebugTrainer::~ControllerTypeDebugTrainer( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化処理
 */
void ControllerTypeDebugTrainer::SetUp( poke_3d::model::BaseCamera *pCamera )
{
  gfl2::math::Quaternion rSrc = pCamera->GetRotationQuat();
  {
    gfl2::math::Vector3 vec1( 0.0f, 0.0f, 1.0f );
    gfl2::math::Matrix34 rotMat;
    rSrc.QuaternionToMatrix( rotMat );
    vec1 = rotMat * vec1;

    gfl2::math::Vector3 vec2( vec1.x, 0.0f, vec1.z );
    vec2 = vec2.Normalize();

    m_vRotation.y = atan2( vec2.x, vec2.z );

    // Y回転を打ち消す
    rotMat = gfl2::math::Matrix34::GetRotationY( - m_vRotation.y ) * rotMat;
    vec1 = rotMat * gfl2::math::Vector3( 0.0f, 0.0f, 1.0f );

    gfl2::math::Vector3 vec3( 0.0f, vec1.y, vec1.z );
    vec3 = vec3.Normalize();
    m_vRotation.x = atan2( vec3.y * -1.0f, vec3.z );
  }
}

/**
 *  @brief  更新処理
 */
void ControllerTypeDebugTrainer::Update( poke_3d::model::BaseCamera *pCamera )
{
  // 操作による更新
  updatePad( pCamera );
  // 対象座標の更新
  updateTarget( pCamera );
  // カメラの更新
  updateCamera( pCamera );
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  操作による更新
 */
void ControllerTypeDebugTrainer::updatePad( poke_3d::model::BaseCamera *pCamera )
{
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager *pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice *pVecDevice = pDeviceManager->GetStick( gfl2::ui::DeviceManager::STICK_STANDARD );
  gfl2::ui::Button *pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  f32 stickValueX = pVecDevice->GetX();
  f32 stickValueY = pVecDevice->GetY();

  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    f32 value = 10.0f;
    m_fDistance += stickValueY * value * -1.0f;
  }
  else
  {
    f32 moveValue = 0.05f;
    gfl2::math::Vector3 move = gfl2::math::Vector3::GetZero();

    move.x = stickValueX * moveValue * -1.0f;
    move.y = stickValueY * moveValue * -1.0f;

    m_vRotation += move;
  }
}

/**
 *  @brief  対象座標の更新
 */
void ControllerTypeDebugTrainer::updateTarget( poke_3d::model::BaseCamera *pCamera )
{
  if( m_pTarget )
  {
    gfl2::math::Vector3 pos;
    if( m_pTarget->GetCameraTargetPosition( &pos ) )
    {
      m_vTargetPosition = pos;
    }
  }
}

/**
 *  @brief  カメラの更新
 */
void ControllerTypeDebugTrainer::updateCamera( poke_3d::model::BaseCamera *pCamera )
{
  m_vRotation.x = gfl2::math::NormalizeRadianAngle( m_vRotation.x );
  m_vRotation.y = gfl2::math::NormalizeRadianAngle( m_vRotation.y );
  m_vRotation.z = gfl2::math::NormalizeRadianAngle( m_vRotation.z );

  gfl2::math::Vector3 targetPos = m_vTargetPosition;
  gfl2::math::Vector3 cameraPos;

  gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, m_fDistance );
  gfl2::math::Matrix34 mtx = gfl2::math::Matrix34::GetRotationZYX( m_vRotation.z, m_vRotation.x, m_vRotation.y );
  cameraOffset = mtx.TransformCoord( cameraOffset );

  cameraPos.x = targetPos.x + cameraOffset.x;
  cameraPos.y = targetPos.y + cameraOffset.y;
  cameraPos.z = targetPos.z + cameraOffset.z;

  pCamera->SetupCameraLookAt(
    cameraPos, targetPos,
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f )
    );

  m_vTargetPosition = targetPos;
  m_vCameraPosition = cameraPos;
}

GFL_NAMESPACE_END(Controller)
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(Field)
#endif // PM_DEUBG
