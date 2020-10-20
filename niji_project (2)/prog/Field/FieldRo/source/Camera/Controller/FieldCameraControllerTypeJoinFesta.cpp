//==============================================================================
/**
 * @file	FieldCameraControllerTypeJoinFesta.cpp
 * @brief	フィールドカメラの操作　ジョインフェスタ用
 * @author	ichiraku_katsuhiko
 * @date	2015/05/26 12:45:55
 */
// =============================================================================

// niji
#include "GameSys/include/GameManager.h"

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeJoinFesta.h"
#include "Field/FieldRo/include/Fieldmap.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Camera )
GFL_NAMESPACE_BEGIN( Controller )

f32 TEST_CAMERA_DISTANCE( 2310.0f );
f32 TEST_CAMERA_HEIGHT( 215.0f );
f32 TEST_TARGET_DISTANCE( -320.0f );
f32 TEST_TARGET_HEIGHT( -1830.0f );
gfl2::math::Vector3 TEST_CAMERA_ROTATION( gfl2::math::ConvDegToRad( -34.637991f ), 0.0f, 0.0f );

/**
* @brief コンストラクタ
*
* @param pTarget  注視対象
*
* @return 無し
*/ 
ControllerTypeJoinFesta::ControllerTypeJoinFesta( Field::Fieldmap* pFieldmap ) : 

  // 内部制御
  m_CameraDistance( TEST_CAMERA_DISTANCE )
  ,m_CameraHeight( TEST_CAMERA_HEIGHT )
  ,m_TargetDistance( TEST_TARGET_DISTANCE )
  ,m_TargetHeight( TEST_TARGET_HEIGHT )
  ,m_Rot( TEST_CAMERA_ROTATION )
  ,m_Interpolation()

  // 外部オブジェクト
  ,m_pFieldmap( pFieldmap )
{
  m_Interpolation.Clear();
}

/**
* @brief デストラクタ
*
* @return 無し
*/
ControllerTypeJoinFesta::~ControllerTypeJoinFesta( void )
{

}

/**
* @brief 初期化処理
* @note  ControllerがCameraUnitの設定された直後に実行される
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeJoinFesta::SetUp( poke_3d::model::BaseCamera* pCamera )
{
}

/**
* @brief 更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeJoinFesta::Update( poke_3d::model::BaseCamera* pCamera )
{
#if 0
  // 操作
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
    gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    gfl2::math::Vector3 pos_r( 0.0f, 0.0f, 0.0f );
    bool pad_u = false;
    bool pad_d = false;
    bool pad_l = false;
    bool pad_r = false;

    pos_r.x = pVecDevice->GetX();
    pos_r.y = pVecDevice->GetY();
    pad_u = pButton->IsHold( gfl2::ui::BUTTON_UP );
    pad_d = pButton->IsHold( gfl2::ui::BUTTON_DOWN );
    pad_l = pButton->IsHold( gfl2::ui::BUTTON_LEFT );
    pad_r = pButton->IsHold( gfl2::ui::BUTTON_RIGHT );

    // L
    if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
    {
      if( pos_r.y || pos_r.x )
      {
        // 注視点調整
        float rate = 10.0f;
        if( pos_r.y > 0){ m_TargetDistance += rate; }
        else            { m_TargetDistance -= rate; }
        if( pos_r.x > 0){ m_TargetHeight += rate; }
        else            { m_TargetHeight -= rate; }
      }

      // カメラ位置（距離）
      if( pad_u || pad_d )
      {
        float diff = 10.0f;// * field::FieldDebugInfo::m_CameraSpeed;
        if ( pad_u ) { m_CameraDistance -= diff; }
        if ( pad_d ) { m_CameraDistance += diff; }
      }
      // カメラ高さ
      if( pad_l || pad_r )
      {
        float diff = 5.0f;// * field::FieldDebugInfo::m_CameraSpeed;
        if ( pad_l ) { m_CameraHeight -= diff; }
        if ( pad_r ) { m_CameraHeight += diff; }
      }
      // リセット
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        m_Rot = TEST_CAMERA_ROTATION;
        m_CameraDistance = TEST_CAMERA_DISTANCE;
        m_CameraHeight = TEST_CAMERA_HEIGHT;
        m_TargetDistance = TEST_TARGET_DISTANCE;
        m_TargetHeight = TEST_TARGET_HEIGHT;
      }
    }
  }
#endif // PM_DEBUG

  /*
    interpolation
  */
  m_Interpolation.Update();


  // 内部制御パラメータをカメラ実体に反映
  {
    SetParam( pCamera );
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
b32 ControllerTypeJoinFesta::SetInterpolation( const gfl2::math::Vector3& rTargetPosition, const gfl2::math::Vector3& rCameraPosition, u32 frame, gfl2::math::Easing::EaseFunc easeFunc )
{
  m_Interpolation.SetUp( gfl2::math::Vector3(0,0,0), rCameraPosition, rTargetPosition, frame );
  m_Interpolation.SetEaseFunc( easeFunc );
  return true;
}



/**
* @brief 内部制御パラメータをカメラ実体に反映
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeJoinFesta::SetParam( poke_3d::model::BaseCamera* pCamera )
{
  const gfl2::math::Vector3 centerPosition( 4200.0f, 0.0f, 4200.f ); // マップの真ん中
  const gfl2::math::Vector3 playerPosition = m_pFieldmap->GetPlayerPosition();

  gfl2::math::Vector3 targetPosition; // 注視点
  gfl2::math::Vector3 cameraPosition; // カメラ位置

  //  
  {
    // 中心点からプレイヤーへのベクトル
    gfl2::math::Vector3 vec = playerPosition - centerPosition;
    vec.y = m_CameraHeight;
    vec = vec.Normalize();

    // 距離
    gfl2::math::Vector3 cameraOffset = vec * m_CameraDistance;

    //cameraPosition.x = targetPosition.x + cameraOffset.x;
    //cameraPosition.y = targetPosition.y + cameraOffset.y;
    //cameraPosition.z = targetPosition.z + cameraOffset.z;
    cameraPosition.x = centerPosition.x + cameraOffset.x;
    cameraPosition.y = centerPosition.y + cameraOffset.y;
    cameraPosition.z = centerPosition.z + cameraOffset.z;

    // 注視点
    // カメラからプレイヤーへのベクトル
    vec = cameraPosition - centerPosition;
    vec.y = m_TargetHeight;
    vec = vec.Normalize();
    gfl2::math::Vector3 targetOffset = vec * m_TargetDistance;
    targetPosition.x = playerPosition.x + targetOffset.x;
    targetPosition.y = playerPosition.y + targetOffset.y;
    targetPosition.z = playerPosition.z + targetOffset.z;
  }


  if( m_Interpolation.IsValid() )
  {
    m_Interpolation.Get(
      gfl2::math::Vector3(0,0,0),
      cameraPosition,
      targetPosition,
      &cameraPosition,
      &targetPosition
      );
  }

#if 0
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
    gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    if( pButton->IsTrigger( gfl2::ui::BUTTON_R ) )
    {
      GFL_PRINT("cameraPosition %.2f %.2f %.2f \n", cameraPosition.x, cameraPosition.y, cameraPosition.z );
      GFL_PRINT("targetPosition %.2f %.2f %.2f \n", targetPosition.x, targetPosition.y, targetPosition.z );
      GFL_PRINT("m_CameraDistance %f \n", m_CameraDistance );
      GFL_PRINT("m_CameraHeight %f \n", m_CameraHeight );
      GFL_PRINT("m_TargetDistance %f \n", m_TargetDistance );
      GFL_PRINT("m_TargetHeight %f \n", m_TargetHeight );
    }
  }
#endif // PM_DEUB

  //
  // カメラオブジェクトに設定
  //

  pCamera->SetupCameraLookAt(
    cameraPosition,
    targetPosition,
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
    );
}

GFL_NAMESPACE_END( Controller )
GFL_NAMESPACE_END( Camera )
GFL_NAMESPACE_END( Field )