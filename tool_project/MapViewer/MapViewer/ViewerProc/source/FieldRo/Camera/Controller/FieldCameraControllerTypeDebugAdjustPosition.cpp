/**
* @brief  フィールドカメラの操作、デバッグ用カメラ位置調整機能
* @file   FieldCameraControllerTypeDebugAdjustPosition.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// niji
#include "GameSystem/include/GameManager.h"

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerTypeDebugAdjustPosition.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

static const f32 X_LIMIT( gfl2::math::ConvDegToRad( 85.0f ) );


/**
* @brief コンストラクタ
*
* @param pTarget  注視対象
*
* @return 無し
*/ 
ControllerTypeAdjustPosition::ControllerTypeAdjustPosition( const Target::TargetBase* pTarget ) : 
  // 内部制御
  m_ControlType( ControlType::TARGET_POSITION )
  ,m_TargetPositon( 0.0f, 0.0f, 0.0f )
  ,m_CameraPositon( 0.0f, 0.0f, 0.0f )
  ,m_Distance( 0.0f )   
  ,m_Rot( 0.0f, 0.0f, 0.0f )
  ,m_StartTargetPositon( 0.0f, 0.0f, 0.0f )
  ,m_StartDistance( 0.0f )   
  ,m_StartRot( 0.0f, 0.0f, 0.0f )

  // 外部オブジェクト
  ,m_pTarget( NULL )
{
  m_pTarget = pTarget;
}

/**
* @brief デストラクタ
*
* @return 無し
*/
ControllerTypeAdjustPosition::~ControllerTypeAdjustPosition( void )
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
void ControllerTypeAdjustPosition::SetUp( poke_3d::model::BaseCamera* pCamera )
{
  // @todo うーん…
  m_pTarget->GetCameraTargetPosition( &m_TargetPositon );
  m_Distance = ( pCamera->GetPosition() - m_TargetPositon ).Length();
  // pCamera->GetRotationQuat().QuaternionToRadianYXZ( &m_Rot.y, &m_Rot.x, &m_Rot.z );
  pCamera->GetRotationQuat().QuaternionToRadian( &m_Rot.x, &m_Rot.y, &m_Rot.z );
  m_CameraPositon = pCamera->GetPosition();

  // 開始時点の情報を記憶
  m_StartDistance = m_Distance;
  m_StartRot = m_Rot;
  m_StartTargetPositon = m_TargetPositon;
}

/**
* @brief 更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeAdjustPosition::Update( poke_3d::model::BaseCamera* pCamera )
{
  // 操作
  {
    GameSystem::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
    gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    
    f32 stickValueX = pVecDevice->GetX();
    f32 stickValueY = pVecDevice->GetY();


    switch( m_ControlType )
    {
    case ControlType::TARGET_POSITION:
      {
        //
        // 共通入力値加工処理
        // 
        {
          // 入力値が大きい軸のみ使用する
          if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
          {
            if( gfl2::math::Abs( stickValueX ) >  gfl2::math::Abs( stickValueY ) )
            {
              stickValueY = 0.0f;
            }
            else
            {
              stickValueX = 0.0f;
            }
          }
        }

        //
        // 操作
        //
        {
          // 上下左右平行移動
          if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
          {
            f32 moveValue( 2.0f );
            gfl2::math::Vector3 move( 0.0f, 0.0f, 0.0f );

            // 加速
            {
              if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
              {
                moveValue = 10.0f;
              }
            }

            // 入力値からローカル空間での移動量を算出
            {
              move.x = stickValueX * moveValue;
              move.y = stickValueY * moveValue;
            }

            // ローカル移動量をワールド移動量に変換し、現在座標に足しこむ
            {
              gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( m_Rot.z, m_Rot.y, m_Rot.x ) );
              move = rotMat.TransformCoord( move );
              m_TargetPositon += move;
            }

#if 0
            if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
            {
              move.z -= moveValue;
            }
            else if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
            {
              move.z += moveValue;
            }
            else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
            {
              move.x += moveValue;
            }
            else if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
            {
              move.x -= moveValue;
            }
#endif
          }
          // 奥行左右平行移動
          else if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
          {
            f32 moveValue( 2.0f );
            gfl2::math::Vector3 move( 0.0f, 0.0f, 0.0f );

            // 加速
            {
              if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
              {
                moveValue = 10.0f;
              }
            }

            // 入力値からローカル空間での移動量を算出
            {
              move.x = stickValueX * moveValue;
              move.z = stickValueY * moveValue * -1.0f;
            }

            // 距離
            {
              f32 changeValue( 0.0f );
              if( pButton->IsHold( gfl2::ui::BUTTON_UP ) )
              {
                changeValue = -moveValue;
              }
              else if( pButton->IsHold( gfl2::ui::BUTTON_DOWN ) )
              {
                changeValue = moveValue;
              }

              if( changeValue != 0.0f )
              {
                m_Distance += changeValue;
                m_Distance = gfl2::math::Clamp( m_Distance, 1.0f, 2048.0f );
              }
            }

            // ローカル移動量をワールド移動量に変換し、現在座標に足しこむ
            {
              gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( m_Rot.z, m_Rot.y, m_Rot.x ) );
              move = rotMat.TransformCoord( move );
              m_TargetPositon += move;
            }
          }
          // 回転、距離
          else
          {
            b32 speedUp( false );
            if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
            {
              speedUp = true;
            }

            // 回転
            {
              f32 moveValue( 0.025f );
              gfl2::math::Vector3 move( 0.0f, 0.0f, 0.0f );
              if( speedUp )
              {
                moveValue = 0.05f;
              }

              move.y = stickValueX * moveValue * -1.0f;
              move.x = stickValueY * moveValue * -1.0f;

              m_Rot += move;

              // X軸の回転には制限をつける
              m_Rot.x = gfl2::math::Clamp( m_Rot.x, X_LIMIT * -1.0f, X_LIMIT );
            }


          } // 回転、距離

        } // 操作

        break;
      } // case ControlType::TARGET_POSITION:

    } // switch( m_ControlType )
  } // 操作

  // 値の反映
  {
    switch( m_ControlType )
    {
    case ControlType::TARGET_POSITION:
      {
        // 回転量を正規化
        m_Rot.x = gfl2::math::NormalizeRadianAngle( m_Rot.x );
        m_Rot.y = gfl2::math::NormalizeRadianAngle( m_Rot.y );
        m_Rot.z = gfl2::math::NormalizeRadianAngle( m_Rot.z );


        gfl2::math::Vector3 targetPosition;
        gfl2::math::Vector3 cameraPosition;
        targetPosition = m_TargetPositon;

        gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, m_Distance );
        gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( m_Rot.z, m_Rot.y, m_Rot.x ) );
        cameraOffset = rotMat.TransformCoord( cameraOffset );

        cameraPosition.x = targetPosition.x + cameraOffset.x;
        cameraPosition.y = targetPosition.y + cameraOffset.y;
        cameraPosition.z = targetPosition.z + cameraOffset.z;

        pCamera->SetupCameraLookAt(
          cameraPosition,
          targetPosition,
          gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
          );

        // 情報更新
        m_CameraPositon = cameraPosition;
        m_TargetPositon = targetPosition;

        break;
      } // case ControlType::TARGET_POSITION:

    } //  switch( m_ControlType )


  } // 値の反映
}

/**
* @brief スクリプター向け情報をDump
*
* @return 無し
*/
void ControllerTypeAdjustPosition::DumpForScript( void )
{
  // ラジアンから度に変換してDump
  gfl2::math::Vector3 rot( m_Rot );
  rot.x = gfl2::math::ConvRadToDeg( m_Rot.x );
  rot.y = gfl2::math::ConvRadToDeg( m_Rot.y );
  rot.z = gfl2::math::ConvRadToDeg( m_Rot.z );

  // sangoで出力していた回転値に無理やり変換 @todo 不要？ 
  // rot.x = gfl2::math::NormalizeDegreeAngle( rot.x + 180.0f);
  rot.x *= -1.0f;
  rot.y = gfl2::math::NormalizeDegreeAngle( rot.y + 180.0f);
  // rot.z = gfl2::math::NormalizeDegreeAngle( rot.z + 180.0f);
	/**
  GFL_RELEASE_PRINT( "-- Camera Info Start --\n" );
  GFL_RELEASE_PRINT( "target pos( x, y, z): %f, %f, %f\n", m_TargetPositon.x, m_TargetPositon.y, m_TargetPositon.z );
  GFL_RELEASE_PRINT( "camera pos( x, y, z): %f, %f, %f\n", m_CameraPositon.x, m_CameraPositon.y, m_CameraPositon.z );
  GFL_RELEASE_PRINT( "target -> camera distance: %f\n", m_Distance );
  GFL_RELEASE_PRINT( "camera rot degree( x, y, z) -> %f, %f, %f \n", rot.x, rot.y, rot.z );
  GFL_RELEASE_PRINT( "EvCameraMoveLookAt( , EASE, %f, %f, %f, %f, %f, %f );\n", m_CameraPositon.x, m_CameraPositon.y, m_CameraPositon.z, m_TargetPositon.x, m_TargetPositon.y, m_TargetPositon.z );
  GFL_RELEASE_PRINT( "-- Camera Info End --\n" );
	*///@saito_del
}

/**
* @brief カメラの状態を操作開始時点にリセットする
*
* @return 無し
*/
void ControllerTypeAdjustPosition::Reset( void )
{
  m_Distance = m_StartDistance;
  m_Rot = m_StartRot;
  m_TargetPositon = m_StartTargetPositon;
}


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );