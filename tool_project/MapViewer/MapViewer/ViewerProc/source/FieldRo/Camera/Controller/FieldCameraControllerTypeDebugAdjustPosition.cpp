/**
* @brief  �t�B�[���h�J�����̑���A�f�o�b�O�p�J�����ʒu�����@�\
* @file   FieldCameraControllerTypeDebugAdjustPosition.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// niji
#include "GameSystem/include/GameManager.h"

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerTypeDebugAdjustPosition.h"

// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

static const f32 X_LIMIT( gfl2::math::ConvDegToRad( 85.0f ) );


/**
* @brief �R���X�g���N�^
*
* @param pTarget  �����Ώ�
*
* @return ����
*/ 
ControllerTypeAdjustPosition::ControllerTypeAdjustPosition( const Target::TargetBase* pTarget ) : 
  // ��������
  m_ControlType( ControlType::TARGET_POSITION )
  ,m_TargetPositon( 0.0f, 0.0f, 0.0f )
  ,m_CameraPositon( 0.0f, 0.0f, 0.0f )
  ,m_Distance( 0.0f )   
  ,m_Rot( 0.0f, 0.0f, 0.0f )
  ,m_StartTargetPositon( 0.0f, 0.0f, 0.0f )
  ,m_StartDistance( 0.0f )   
  ,m_StartRot( 0.0f, 0.0f, 0.0f )

  // �O���I�u�W�F�N�g
  ,m_pTarget( NULL )
{
  m_pTarget = pTarget;
}

/**
* @brief �f�X�g���N�^
*
* @return ����
*/
ControllerTypeAdjustPosition::~ControllerTypeAdjustPosition( void )
{

}

/**
* @brief ����������
* @note  Controller��CameraUnit�̐ݒ肳�ꂽ����Ɏ��s�����
*
* @param  pCamera ���삷��J�����̎���
*
* @return ����
*/
void ControllerTypeAdjustPosition::SetUp( poke_3d::model::BaseCamera* pCamera )
{
  // @todo ���[��c
  m_pTarget->GetCameraTargetPosition( &m_TargetPositon );
  m_Distance = ( pCamera->GetPosition() - m_TargetPositon ).Length();
  // pCamera->GetRotationQuat().QuaternionToRadianYXZ( &m_Rot.y, &m_Rot.x, &m_Rot.z );
  pCamera->GetRotationQuat().QuaternionToRadian( &m_Rot.x, &m_Rot.y, &m_Rot.z );
  m_CameraPositon = pCamera->GetPosition();

  // �J�n���_�̏����L��
  m_StartDistance = m_Distance;
  m_StartRot = m_Rot;
  m_StartTargetPositon = m_TargetPositon;
}

/**
* @brief �X�V����
*
* @param  pCamera ���삷��J�����̎���
*
* @return ����
*/
void ControllerTypeAdjustPosition::Update( poke_3d::model::BaseCamera* pCamera )
{
  // ����
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
        // ���ʓ��͒l���H����
        // 
        {
          // ���͒l���傫�����̂ݎg�p����
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
        // ����
        //
        {
          // �㉺���E���s�ړ�
          if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
          {
            f32 moveValue( 2.0f );
            gfl2::math::Vector3 move( 0.0f, 0.0f, 0.0f );

            // ����
            {
              if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
              {
                moveValue = 10.0f;
              }
            }

            // ���͒l���烍�[�J����Ԃł̈ړ��ʂ��Z�o
            {
              move.x = stickValueX * moveValue;
              move.y = stickValueY * moveValue;
            }

            // ���[�J���ړ��ʂ����[���h�ړ��ʂɕϊ����A���ݍ��W�ɑ�������
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
          // ���s���E���s�ړ�
          else if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
          {
            f32 moveValue( 2.0f );
            gfl2::math::Vector3 move( 0.0f, 0.0f, 0.0f );

            // ����
            {
              if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
              {
                moveValue = 10.0f;
              }
            }

            // ���͒l���烍�[�J����Ԃł̈ړ��ʂ��Z�o
            {
              move.x = stickValueX * moveValue;
              move.z = stickValueY * moveValue * -1.0f;
            }

            // ����
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

            // ���[�J���ړ��ʂ����[���h�ړ��ʂɕϊ����A���ݍ��W�ɑ�������
            {
              gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( m_Rot.z, m_Rot.y, m_Rot.x ) );
              move = rotMat.TransformCoord( move );
              m_TargetPositon += move;
            }
          }
          // ��]�A����
          else
          {
            b32 speedUp( false );
            if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
            {
              speedUp = true;
            }

            // ��]
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

              // X���̉�]�ɂ͐���������
              m_Rot.x = gfl2::math::Clamp( m_Rot.x, X_LIMIT * -1.0f, X_LIMIT );
            }


          } // ��]�A����

        } // ����

        break;
      } // case ControlType::TARGET_POSITION:

    } // switch( m_ControlType )
  } // ����

  // �l�̔��f
  {
    switch( m_ControlType )
    {
    case ControlType::TARGET_POSITION:
      {
        // ��]�ʂ𐳋K��
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

        // ���X�V
        m_CameraPositon = cameraPosition;
        m_TargetPositon = targetPosition;

        break;
      } // case ControlType::TARGET_POSITION:

    } //  switch( m_ControlType )


  } // �l�̔��f
}

/**
* @brief �X�N���v�^�[��������Dump
*
* @return ����
*/
void ControllerTypeAdjustPosition::DumpForScript( void )
{
  // ���W�A������x�ɕϊ�����Dump
  gfl2::math::Vector3 rot( m_Rot );
  rot.x = gfl2::math::ConvRadToDeg( m_Rot.x );
  rot.y = gfl2::math::ConvRadToDeg( m_Rot.y );
  rot.z = gfl2::math::ConvRadToDeg( m_Rot.z );

  // sango�ŏo�͂��Ă�����]�l�ɖ������ϊ� @todo �s�v�H 
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
* @brief �J�����̏�Ԃ𑀍�J�n���_�Ƀ��Z�b�g����
*
* @return ����
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