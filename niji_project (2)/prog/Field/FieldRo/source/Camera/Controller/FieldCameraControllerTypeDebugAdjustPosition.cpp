/**
* @brief  フィールドカメラの操作、デバッグ用カメラ位置調整機能
* @file   FieldCameraControllerTypeDebugAdjustPosition.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

#if PM_DEBUG

// niji
#include "GameSys/include/GameManager.h"

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeDebugAdjustPosition.h"

// カメラ操作のユーティリティ関数
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"

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
  ,m_fovy( 0.0f)
  ,m_bank( 0.0f)
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
  // カメラパラメータの取得
  // @note  アニメーション再生中はTargetPositionが最後に指定された座標になっている
  Camera::Area::Utility::GetEventScriptCameraParameter( pCamera, &m_TargetPositon, &m_CameraPositon, &m_Rot, &m_Distance );

  // @fix Z成分はバンクとして別で保持し、オイラーからは削除
  // Camera::Area::Utility::GetEventScriptCameraParameterはカメラのクォータニオンからオイラー角を求めm_Rotに代入するが、
  // このクラスではz成分はバンクとして別で保持しているため、バンクを使用していたときに、SetUpを走らせた後とUpdate中のm_Rotに差異が出ていた
  m_bank = m_Rot.z;
  m_Rot.z = 0.0f;

  // 開始時点の情報を記憶
  m_StartDistance = m_Distance;
  m_StartRot = m_Rot;
  m_StartTargetPositon = m_TargetPositon;
  m_fovy = pCamera->GetFovy();
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
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
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
          if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
          {
            b32 speedUp( false );
            if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
            {
              speedUp = true;
            }

            // カメラ基準回転
            if( pButton->IsHold( gfl2::ui::BUTTON_X ) )
            {
              f32 moveValue( 0.025f );
              gfl2::math::Vector3 move( 0.0f, 0.0f, 0.0f );
              if( speedUp )
              {
                moveValue = 0.05f;
              }

              move.y = stickValueX * moveValue * -1.0f;
              move.x = stickValueY * moveValue;

              m_Rot += move;

              // X軸の回転には制限をつける
              m_Rot.x = gfl2::math::Clamp( m_Rot.x, X_LIMIT * -1.0f, X_LIMIT );

              gfl2::math::Vector3 rotWork( m_Rot );
              // rotWork.y *= -1.0f;
              // rotWork.x *= -1.0f;

              gfl2::math::Vector3 targetPosition;
              gfl2::math::Vector3 cameraPosition;
              targetPosition = m_TargetPositon;
              cameraPosition = m_CameraPositon;

              gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, -m_Distance );
              gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( rotWork.z, rotWork.y, rotWork.x ) );
              cameraOffset = rotMat.TransformCoord( cameraOffset );

              targetPosition.x = cameraPosition.x + cameraOffset.x;
              targetPosition.y = cameraPosition.y + cameraOffset.y;
              targetPosition.z = cameraPosition.z + cameraOffset.z;

              pCamera->SetupCameraLookAt(
                cameraPosition,
                targetPosition,
                gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
                );

              Camera::Area::Utility::GetEventScriptCameraParameter( pCamera, &m_TargetPositon, &m_CameraPositon, &m_Rot, &m_Distance );
            }
            // 上下左右平行移動
            else
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
            }
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
              f32 distanceWork = m_Distance;

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

                // カメラポジションそのままでターゲット座標移動
                if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
                {
                  gfl2::math::Vector3 cameraToTarget( m_TargetPositon - m_CameraPositon );
                  cameraToTarget.NormalizeRet();

                  m_TargetPositon = m_CameraPositon + cameraToTarget * m_Distance;
                }
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


            // 画角操作
            f32 fovyAdd( gfl2::math::ConvDegToRad( 0.1f) );
            if( speedUp)
            {
              fovyAdd *= 10.0f;
            }
            if( pButton->IsRepeat( gfl2::ui::BUTTON_UP))
            {
              m_fovy -= fovyAdd;
              m_fovy = gfl2::math::NormalizeRadianAngle( m_fovy);
            }
            else if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN))
            {
              m_fovy += fovyAdd;
              m_fovy = gfl2::math::NormalizeRadianAngle( m_fovy);
            }

            // バンク回転
            f32 bankAdd( gfl2::math::ConvDegToRad( 0.1f) );
            if( speedUp)
            {
              bankAdd *= 10.0f;
            }
            if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT))
            {
              m_bank -= bankAdd;
              m_bank = gfl2::math::NormalizeRadianAngle( m_bank);
            }
            else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT))
            {
              m_bank += bankAdd;
              m_bank = gfl2::math::NormalizeRadianAngle( m_bank);
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


        // バンク。回転処理はControllerTypeEventScriptと合わせています
        gfl2::math::Matrix34 rotBankMat( gfl2::math::Matrix34::GetRotationZ( m_bank ) );
        gfl2::math::Quaternion rot = pCamera->GetRotationQuat();
        gfl2::math::Quaternion zRot;
        zRot.MatrixToQuaternion( rotBankMat );
        rot = rot * zRot;
        pCamera->SetRotationQuat( rot );

        // 画角
        pCamera->SetFovy( m_fovy);


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

  GFL_RELEASE_PRINT( "-- Camera Info Start --\n" );
  GFL_RELEASE_PRINT( "target pos( x, y, z): %f, %f, %f\n", m_TargetPositon.x, m_TargetPositon.y, m_TargetPositon.z );
  GFL_RELEASE_PRINT( "camera pos( x, y, z): %f, %f, %f\n", m_CameraPositon.x, m_CameraPositon.y, m_CameraPositon.z );
  GFL_RELEASE_PRINT( "target -> camera distance: %f\n", m_Distance );
  GFL_RELEASE_PRINT( "camera rot degree( x, y, z) -> %f, %f, %f \n", rot.x, rot.y, rot.z );
  GFL_RELEASE_PRINT( "EvCameraMoveLookAt( , EASE, %f, %f, %f, %f, %f, %f );\n", m_CameraPositon.x, m_CameraPositon.y, m_CameraPositon.z, m_TargetPositon.x, m_TargetPositon.y, m_TargetPositon.z );
  GFL_RELEASE_PRINT( "-- Camera Info End --\n" );
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

/**
* @brief パラメーターログ出力
*/
void ControllerTypeAdjustPosition::DumpParameter()
{
  GFL_PRINT( "----- ControllerTypeAdjustPosition Dump Paramter Start -----\n" );

  GFL_PRINT( "m_ControlType = %d\n", m_ControlType );
  GFL_PRINT( "m_TargetPositon = %f, %f, %f\n", m_TargetPositon.x, m_TargetPositon.y, m_TargetPositon.z );
  GFL_PRINT( "m_CameraPositon = %f, %f, %f\n", m_CameraPositon.x, m_CameraPositon.y, m_CameraPositon.z );
  GFL_PRINT( "m_Rot = %f, %f, %f\n", m_Rot.x, m_Rot.y, m_Rot.z );
  GFL_PRINT( "m_Distance = %f\n", m_Distance );
  GFL_PRINT( "m_fovy = %f\n", m_fovy );
  GFL_PRINT( "m_bank = %f\n", m_bank );
  GFL_PRINT( "\n" );
  GFL_PRINT( "m_StartTargetPositon = %f, %f, %f\n", m_StartTargetPositon.x, m_StartTargetPositon.y, m_StartTargetPositon.z );
  GFL_PRINT( "m_StartRot = %f, %f, %f\n", m_StartRot.x, m_StartRot.y, m_StartRot.z );
  GFL_PRINT( "m_StartDistance = %f\n", m_StartDistance );
  GFL_PRINT( "m_pTarget = %x\n", m_pTarget );

  GFL_PRINT( "----- ControllerTypeAdjustPosition Dump Paramter End -----\n" );
}

GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // #if PM_DEBUG