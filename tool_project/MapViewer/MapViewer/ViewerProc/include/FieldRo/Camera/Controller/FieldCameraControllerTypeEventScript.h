/**
* @brief  �t�B�[���h�J�����̑���A�C�x���g�X�N���v�g�p
* @file   FieldCameraControllerTypeEventScript.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERACONTROLLERERTYPEEVENTSCRIPT2_H_INCLUDED__ )
#define __FIELDCAMERACONTROLLERERTYPEEVENTSCRIPT2_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Easing.h>

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerBase.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetBase.h"


// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( MoveModel );
class FieldMoveModel;
GFL_NAMESPACE_END( MoveModel );
GFL_NAMESPACE_END( Field );


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

/**
* @brief �t�B�[���h�J�����̑���A�C�x���g�X�N���v�g�p
*/
class ControllerTypeEventScript : public ControllerBase
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeEventScript );

public:

  // �s���̏�Ԏ��ʎq
  struct ActionState
  {
    enum Value
    {
      NONE = 0,     // �������Ă��Ȃ�
      MOVE,         // �ړ����ߎ��s��
      ANIMATION,    // �A�j���[�V������

      MAX
    };
  };
 
  /** @brief  �������ݒ� */
  class InitializeDescription
  {
  public:
    // MoveModel::FieldMoveModel* m_pPlayerModel;
    // const MoveModel::FieldMoveModel* m_pPlayerModel; @todo const�ō��W����铝�ׂ��Ȃ��I

    u32 dummy;
    // @note ��]�����Ȃǂ��ǉ������
  };

  /**
  * @brief �R���X�g���N�^
  *
  * @param rInitDesc  �������ݒ�
  *
  * @return ����
  */ 
  ControllerTypeEventScript( const InitializeDescription& rInitDesc );

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~ControllerTypeEventScript( void );
  

  /**
  * @brief ����������
  * @note  Controller��CameraUnit�̐ݒ肳�ꂽ����Ɏ��s�����
  *
  * @param  pCamera ���삷��J�����̎���
  *
  * @return ����
  */
  void SetUp( poke_3d::model::BaseCamera* pCamera );

  /**
  * @brief �X�V����
  *
  * @param  pCamera ���삷��J�����̎���
  *
  * @return ����
  */
  void Update( poke_3d::model::BaseCamera* pCamera );

  /**
  * @brief �ړ����ߔ��s( �J�������W/�^�[�Q�b�g���W�w��� )
  *
  * @param  rCameraPosition �J�����ʒu�A�ړ��ڕW
  * @param  rTargetPosition �J�����^�[�Q�b�g�ʒu�A�ړ��ڕW
  * @param  frame           �ړ��ɂ�����t���[��
  * @param  easingFunc      �ړ��Ɏg�p����C�[�W���O�֐�
  *
  * @return ����
  */
  b32 Move( const gfl2::math::Vector3& rCameraPosition, const gfl2::math::Vector3& rTargetPosition, u32 frame , gfl2::math::Easing::EaseFunc easingFunc = gfl2::math::Easing::LINEAR );

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
  b32 Move( const gfl2::math::Vector3& rCameraPosition, const gfl2::math::Quaternion& rRotation, u32 frame, gfl2::math::Easing::EaseFunc easingFunc = gfl2::math::Easing::LINEAR );
  
  /**
  * @brief �����Ă��邩�擾
  * @note  �A�j���[�V��������true��Ԃ�
  *
  * @return true �����Ă���  false �����Ă��Ȃ�
  */
  b32 IsMoving( void ) const;

private:

#if 0
  /**
  * @brief ��������p�����[�^���J�������̂ɔ��f
  *
  * @param  pCamera ���삷��J�����̎���
  *
  * @return ����
  */
  void SetParam( poke_3d::model::BaseCamera* pCamera );
#endif

  //
  // ��������
  //
  ActionState::Value              m_ActionState;        // �s���̏�Ԏ��ʎq


  // �ړ����ߐ���
  u32                             m_MoveEndFrame;       // �ړ��I���t���[��
  u32                             m_MoveNowFrame;       // ���݃t���[��
  gfl2::math::Quaternion          m_MoveStartRotation;  // �ړ��J�n(��])
  gfl2::math::Vector3             m_MoveStartPosition;  // �ړ��J�n(�ʒu)
  gfl2::math::Quaternion          m_MoveTargetRotation; // �ړ��ڕW(��])
  gfl2::math::Vector3             m_MoveTargetPosition; // �ړ��ڕW(�ʒu)
  gfl2::math::Easing::EaseFunc    m_MoveEasingFunc;     // �ړ��Ɏg�p����C�[�W���O�֐�


  //
  // �O���I�u�W�F�N�g
  //
  poke_3d::model::BaseCamera*     m_pBaseCamera;        // �J�������� @todo �V�X�e���ŋz���H
  // MoveModel::FieldMoveModel* m_pPlayerModel;    // ���@
  // const MoveModel::FieldMoveModel* m_pPlayerModel; @todo const�ō��W����铝�ׂ��Ȃ��I

};  // class FieldControllerTypeEventScript


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERACONTROLLERERTYPEEVENTSCRIPT2_H_INCLUDED__

  