/**
* @brief  フィールドカメラの操作、テスト用
* @file   FieldCameraControllerTypeTest.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// niji
#include "GameSystem/include/GameManager.h"

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerTypeTest.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

const static f32 TEST_CAMERA_DISTANCE( 579.0f );
const static gfl2::math::Vector3 TEST_CAMERA_ROTATION( gfl2::math::ConvDegToRad( -34.637991f ), 0.0f, 0.0f );

/**
* @brief コンストラクタ
*
* @param pTarget  注視対象
*
* @return 無し
*/ 
ControllerTypeTest::ControllerTypeTest( const Target::TargetBase* pTarget ) : 

  // 内部制御
  m_Distance( TEST_CAMERA_DISTANCE )
  ,m_Rot( TEST_CAMERA_ROTATION )

  // 外部オブジェクト
  ,m_pTarget( pTarget )
{

}

/**
* @brief デストラクタ
*
* @return 無し
*/
ControllerTypeTest::~ControllerTypeTest( void )
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
void ControllerTypeTest::SetUp( poke_3d::model::BaseCamera* pCamera )
{

}

/**
* @brief 更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeTest::Update( poke_3d::model::BaseCamera* pCamera )
{
  //
  // 最新ターゲット座標を取得
  //
  gfl2::math::Vector3 targetPosition;
  gfl2::math::Vector3 cameraPosition;
  if( m_pTarget->GetCameraTargetPosition( &targetPosition ) == false )
  {
    return;
  }

  //
  // 状態更新
  //
     
  // 操作
  {
    GameSystem::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
    gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    gfl2::math::Vector3 pos( 0.0f, 0.0f, 0.0f );
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
      // 回転
      if( pos_r.x || pos_r.y)
      {
        // 回転速度計算
        float rate = 0.05f; // * field::FieldDebugInfo::m_CameraSpeed;

        // 回転実行
        gfl2::math::Vector3 rot( pos_r.y * rate * -1.0f, pos_r.x * rate * -1.0f, 0.0f );
        m_Rot += rot;
      }
      // 距離
      if( pad_u || pad_d )
      {
        float diff = 5.0f;// * field::FieldDebugInfo::m_CameraSpeed;
        if ( pad_u ) { m_Distance -= diff; }
        if ( pad_d ) { m_Distance += diff; }
        m_Distance = gfl2::math::Clamp( m_Distance, 0.0f, 2048.0f );
      }
      // リセット
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        m_Rot = TEST_CAMERA_ROTATION;
        m_Distance = TEST_CAMERA_DISTANCE;
      }
    }
  }

  // 最終位置を計算
  {
    gfl2::math::Vector4 cameraOffset( 0.0f, 0.0f, m_Distance );
    gfl2::math::Matrix rotMat( gfl2::math::Matrix::GetRotationZYX( m_Rot.z, m_Rot.y, m_Rot.x ) );
    cameraOffset = rotMat.TransformCoord( cameraOffset );

    cameraPosition.x = targetPosition.x + cameraOffset.x;
    cameraPosition.y = targetPosition.y + cameraOffset.y;
    cameraPosition.z = targetPosition.z + cameraOffset.z;
  }

  //
  // カメラオブジェクトに設定
  //

  pCamera->SetupCameraLookAt(
    cameraPosition,
    targetPosition,
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
    );
}

GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );