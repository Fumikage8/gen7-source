/**
* @brief  �t�B�[���h�J�����̑���A�C�x���g�X�N���v�g�p
* @file   FieldCameraControllerTypeEventScript.cpp
* @author ikuta_junya
* @data   2015.05.21
*/


// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerTypeEventScript.h"



GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

/**
* @brief �R���X�g���N�^
*
* @param rInitDesc  �������ݒ�
*
* @return ����
*/ 
ControllerTypeEventScript::ControllerTypeEventScript( const InitializeDescription& rInitDesc ) :

  // ��������
  m_ActionState( ActionState::NONE )
  ,m_MoveEndFrame( 0 )
  ,m_MoveNowFrame( 0 )
  // ,m_MoveStartRotation
  ,m_MoveStartPosition( 0.0f ,0.0f, 0.0f )
  // ,m_MoveTargetRotation
  ,m_MoveTargetPosition( 0.0f ,0.0f, 0.0f )
  ,m_MoveEasingFunc( gfl2::math::Easing::LINEAR )
  // �O���I�u�W�F�N�g
  ,m_pBaseCamera( NULL )
{

}

/**
* @brief �f�X�g���N�^
*
* @return ����
*/
ControllerTypeEventScript::~ControllerTypeEventScript( void )
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
void ControllerTypeEventScript::SetUp( poke_3d::model::BaseCamera* pCamera )
{
  // �p�����[�^������
  m_ActionState = ActionState::NONE;
  m_MoveEndFrame = 0;
  m_MoveNowFrame = 0;
  // ,m_MoveStartRotation
  m_MoveStartPosition = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f );
  // ,m_MoveTargetRotation
  m_MoveTargetPosition = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f );
  m_MoveEasingFunc = gfl2::math::Easing::LINEAR;

  // �L��
  m_pBaseCamera = pCamera;
}

/**
* @brief �X�V����
*
* @param  pCamera ���삷��J�����̎���
*
* @return ����
*/
void ControllerTypeEventScript::Update( poke_3d::model::BaseCamera* pCamera )
{
  switch ( m_ActionState )
  {
  case ActionState::MOVE:
    {
      // �i�s
      m_MoveNowFrame ++;
      
      // �i�s�x���v�Z
      f32 t = static_cast<f32>( m_MoveNowFrame ) / m_MoveEndFrame;
      t = gfl2::math::Easing::GetValue( m_MoveEasingFunc, t );

      // ���
      gfl2::math::Quaternion rot = m_MoveStartRotation;
      gfl2::math::Vector3 pos = m_MoveStartPosition;
      rot.Slerp( m_MoveTargetRotation, t );
      pos.Lerp( m_MoveTargetPosition, t );

      // �J�������̂ɐݒ�
      pCamera->SetPosition( pos );
      pCamera->SetRotationQuat( rot );

      // �I��
      if( m_MoveNowFrame >= m_MoveEndFrame )
      {
        m_ActionState = ActionState::NONE;
        break;
      }

      break;
    } // case ActionState::MOVE:
  } // switch ( m_ActionState )
}

/**
* @brief �ړ����ߔ��s
*
* @param  rCameraPosition �J�����ʒu�A�ړ��ڕW
* @param  rTargetPosition �J�����^�[�Q�b�g�ʒu�A�ړ��ڕW
* @param  frame           �ړ��ɂ�����t���[��
* @param  easingFunc      �ړ��Ɏg�p����C�[�W���O�֐�
*
* @return ����
*/
b32 ControllerTypeEventScript::Move( const gfl2::math::Vector3& rCameraPosition, const gfl2::math::Vector3& rTargetPosition, u32 frame , gfl2::math::Easing::EaseFunc easingFunc )
{
  if( m_pBaseCamera == NULL )
  {
    return false;
  }

  if( frame < 1 )
  {
    // @todo
    return false;
  }

  // ���W���->��]
  {
    gfl2::math::Quaternion rotation;
    gfl2::math::Vector3 cameraUpVector( 0.0f, 1.0f, 0.0f );
    gfl2::math::Vector3 v = rCameraPosition - rTargetPosition;

    gfl2::math::Vector3 zAxis = v.Normalize();
    gfl2::math::Vector3 xAxis = cameraUpVector.Cross(zAxis).Normalize();
    gfl2::math::Vector3 yAxis = zAxis.Cross(xAxis).Normalize();

    gfl2::math::Matrix34 rotMatrix(
      gfl2::math::Vector4(xAxis.x, yAxis.x, zAxis.x, 0.0f),
      gfl2::math::Vector4(xAxis.y, yAxis.y, zAxis.y, 0.0f),
      gfl2::math::Vector4(xAxis.z, yAxis.z, zAxis.z, 0.0f)
      );
    rotation.MatrixToQuaternion( rotMatrix );

    return Move(
      rCameraPosition,
      rotation,
      frame,
      easingFunc
      );
  }
}

/**
* @brief �ړ����ߔ��s( �J�������W/��]�w��� )
*
* @param  rCameraPosition �J�����ʒu�A�ړ��ڕW
* @param  rRotation       �J������]
* @param  frame           �ړ��ɂ�����t���[��
* @param  easingFunc      �ړ��Ɏg�p����C�[�W���O�֐�
*
* @return ����
*/
b32 ControllerTypeEventScript::Move( const gfl2::math::Vector3& rCameraPosition, const gfl2::math::Quaternion& rRotation, u32 frame, gfl2::math::Easing::EaseFunc easingFunc )
{
  if( m_pBaseCamera == NULL )
  {
    return false;
  }

  if( frame < 1 )
  {
    // @todo
    return false;
  }

  //
  // �ړ��ݒ�L��
  //
  m_MoveEasingFunc = easingFunc;
  m_MoveEndFrame = frame;
  m_MoveTargetPosition = rCameraPosition;
  m_MoveTargetRotation = rRotation;

  //
  // ���݂̏�Ԃ��L��
  //
  m_MoveStartPosition = m_pBaseCamera->GetPosition();
  m_MoveStartRotation = m_pBaseCamera->GetRotationQuat();

  //
  // ��ԕύX
  //
  m_MoveNowFrame = 0;
  m_ActionState = ActionState::MOVE;


  return true;
}

/**
* @brief �����Ă��邩�擾
* @note  �A�j���[�V��������true��Ԃ�
*
* @return true �����Ă���  false �����Ă��Ȃ�
*/
b32 ControllerTypeEventScript::IsMoving( void ) const
{
  return ( m_ActionState != ActionState::NONE );
}

#if 0
/**
* @brief ��������p�����[�^���J�������̂ɔ��f
*
* @param  pCamera ���삷��J�����̎���
*
* @return ����
*/
void ControllerTypeEventScript::SetParam( poke_3d::model::BaseCamera* pCamera )
{

}
#endif

GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );