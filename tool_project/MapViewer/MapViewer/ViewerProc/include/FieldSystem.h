/**
* @brief  �t�B�[���h�V�X�e��(����)
* @file   FieldSystem.h
* @author ikuta_junya
* @data   2015.02.07
*/

#if !defined( __FIELDSYSTEM_H_INCLUDED__ )
#define __FIELDSYSTEM_H_INCLUDED__ 
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

// �O���錾
GFL_NAMESPACE_BEGIN( fld );
GFL_NAMESPACE_BEGIN( mmodel );
class MoveModelManager;
GFL_NAMESPACE_END( mmodel );
GFL_NAMESPACE_END( fld );

GFL_NAMESPACE_BEGIN( Field );

// �O���錾
GFL_NAMESPACE_BEGIN( Camera );
class FieldCamera;
GFL_NAMESPACE_END( Camera );

/**
* @brief �t�B�[���h�V�X�e��(����)
*/
class FieldSystem
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( FieldSystem );


public:

  /**
  * @brief �R���X�g���N�^
  *
  * @return ����
  */ 
  FieldSystem( void )
  :m_pFieldCamera(NULL)
  ,m_pMoveModelManager(NULL)
  {};

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~FieldSystem( void ){};


  /**
  * @brief ���ݎg�p����Ă���J�������擾����
  *
  * @return ���ݎg�p����Ă���J����
  */
  Camera::FieldCamera* GetActiveCamera( void )
  {
    return m_pFieldCamera;
  }

  /**
  * @brief ���@�ɒǏ]���Ă���J�������擾����
  *
  * @return �@�ɒǏ]���Ă���J����
  */
  Camera::FieldCamera* GetPlayerCamera( void )
  {
    return m_pFieldCamera;
  }

  /**
  * @brief ���샂�f���̃}�l�[�W���[���擾
  *
  * @return ���샂�f���̃}�l�[�W���[
  */
  fld::mmodel::MoveModelManager* GetMoveModelManager( void )
  {
    return m_pMoveModelManager;
  }

  /**
  * @brief ���샂�f���̃}�l�[�W���[��ݒ�
  *
  * @param ���샂�f���̃}�l�[�W���[
  */
  void SetMoveModelManager( fld::mmodel::MoveModelManager* pMoveModelManager )
  {
    m_pMoveModelManager = pMoveModelManager;
  }

  Camera::FieldCamera*            m_pFieldCamera; 
  fld::mmodel::MoveModelManager*  m_pMoveModelManager;

};  // class FieldSystem

GFL_NAMESPACE_END( Field );


#endif // __FIELDSYSTEM_H_INCLUDED__