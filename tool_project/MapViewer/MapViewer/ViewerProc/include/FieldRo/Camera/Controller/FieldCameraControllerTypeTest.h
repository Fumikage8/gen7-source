/**
* @brief  �t�B�[���h�J�����̑���A�e�X�g�p
* @file   FieldCameraControllerTypeTest.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERACONTROLLERERTYPETEST_H_INCLUDED__ )
#define __FIELDCAMERACONTROLLERERTYPETEST_H_INCLUDED__
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
* @brief �t�B�[���h�J�����̑���A�e�X�g�p
*/
class ControllerTypeTest : public ControllerBase
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeTest );

public:
 
  /**
  * @brief �R���X�g���N�^
  *
  * @param pTarget  �����Ώ�
  *
  * @return ����
  */ 
  ControllerTypeTest( const Target::TargetBase* pTarget );

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~ControllerTypeTest( void );
  
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

private:

  //
  // ��������
  //
  f32                       m_Distance;   
  gfl2::math::Vector3       m_Rot;

  //
  // �O���I�u�W�F�N�g
  //
  const Target::TargetBase* m_pTarget;  // �����Ώ�

};  // class FieldControllerTypeTest


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERACONTROLLERERTYPETEST_H_INCLUDED__

  