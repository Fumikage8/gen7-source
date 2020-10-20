/**
* @brief  �t�B�[���h�J�����̑���C���^�[�t�F�C�X
* @file   FieldCameraControllerBase.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERATARGETCONTROLLERER_H_INCLUDED__ )
#define __FIELDCAMERATARGETCONTROLLERER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>

// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

/**
* @brief �t�B�[���h�J�����̑���C���^�[�t�F�C�X
*/
class ControllerBase
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerBase );

public:
 
  /**
  * @brief �R���X�g���N�^
  *
  * @return ����
  */ 
  ControllerBase( void ) {}

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~ControllerBase( void ) {}
  

  /**
  * @brief ����������
  * @note  Controller��CameraUnit�̐ݒ肳�ꂽ����Ɏ��s�����
  *
  * @param  pCamera ���삷��J�����̎���
  *
  * @return ����
  */
  virtual void SetUp( poke_3d::model::BaseCamera* pCamera ) = 0;

  /**
  * @brief �X�V����
  *
  * @param  pCamera ���삷��J�����̎���
  *
  * @return ����
  */
  virtual void Update( poke_3d::model::BaseCamera* pCamera ) = 0;

  /**
  * @brief ���݂̐ݒ�l���琶�������v���W�F�N�V�����s����擾����
  * @note  ���̊֐���false��Ԃ��ꍇ�́ABaseCamera����s����擾����
  * @note  ���̊֐��ɂ��virtual������Ă΂��+�C���^�[�t�F�C�X����Ȃ��Ȃ�̂ō폜����\��������
  *
  * @param  pOut �s��i�[��( �߂�l true ���̂ݒl������)
  *
  * @return ����
  */
  virtual b32 GetProjectionMatrixOverride( gfl2::math::Matrix44* pOut ) const { return false; }

  /**
  * @brief ���݂̐ݒ�l���琶�������r���[�s����擾����
  * @note  ���̊֐���false��Ԃ��ꍇ�́ABaseCamera����s����擾����
  * @note  ���̊֐��ɂ��virtual������Ă΂��+�C���^�[�t�F�C�X����Ȃ��Ȃ�̂ō폜����\��������
  *
  * @param  pOut �s��i�[��( �߂�l true ���̂ݒl������)
  *
  * @return ����
  */
  virtual b32 GetViewMatrixOverride( gfl2::math::Matrix34* pOut ) const { return false; }


private:


};  // class FieldControllerBase


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERATARGETCONTROLLERER_H_INCLUDED__
