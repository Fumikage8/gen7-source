/**
* @brief  �t�B�[���h�J�����̑���A�f�o�b�O�p�J�����ʒu�����@�\
* @file   FieldCameraControllerTypeDebugAdjustPosition.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERACONTROLLERERTYPEADUSTPOSITION_H_INCLUDED__ )
#define __FIELDCAMERACONTROLLERERTYPEADUSTPOSITION_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerBase.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetBase.h"

// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

/**
* @brief �t�B�[���h�J�����̑���A�f�o�b�O�p�J�����ʒu�����@�\
*/
class ControllerTypeAdjustPosition : public ControllerBase
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeAdjustPosition );

public:

  struct ControlType
  {
    enum Value
    {
      TARGET_POSITION = 0,    // �^�[�Q�b�g
    };
  };

 
  /**
  * @brief �R���X�g���N�^
  *
  * @param pTarget  �����Ώ�
  *
  * @return ����
  */ 
  ControllerTypeAdjustPosition( const Target::TargetBase* pTarget );

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~ControllerTypeAdjustPosition( void );
  
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
  * @brief �X�N���v�^�[��������Dump
  *
  * @return ����
  */
  void DumpForScript( void );

  /**
  * @brief �J�����̏�Ԃ𑀍�J�n���_�Ƀ��Z�b�g����
  *
  * @return ����
  */
  void Reset( void );

private:

  //
  // ��������
  //
  ControlType::Value        m_ControlType;    // ����^��
  gfl2::math::Vector3       m_TargetPositon;  // �^�[�Q�b�g���W
  gfl2::math::Vector3       m_CameraPositon;  // �J�������W
  f32                       m_Distance;   
  gfl2::math::Vector3       m_Rot;


  // �J�n���_���
  gfl2::math::Vector3       m_StartTargetPositon;
  f32                       m_StartDistance;   
  gfl2::math::Vector3       m_StartRot;


  //
  // �O���I�u�W�F�N�g
  //
  const Target::TargetBase* m_pTarget;  // �����Ώ�

};  // class FieldControllerTypeAdjustPosition


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERACONTROLLERERTYPEADUSTPOSITION_H_INCLUDED__

  