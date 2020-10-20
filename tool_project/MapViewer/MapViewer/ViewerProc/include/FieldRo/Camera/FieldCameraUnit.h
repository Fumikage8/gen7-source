/**
* @brief  �t�B�[���h�J�����A1����P�ʃN���X
* @file   FieldCameraUnit.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERAUNITER_H_INCLUDED__ )
#define __FIELDCAMERAUNITER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerBase.h"



// �O���錾
GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( renderingengine );
GFL_NAMESPACE_BEGIN( scenegraph );
GFL_NAMESPACE_BEGIN( instance );
class DrawEnvNode;
GFL_NAMESPACE_END( instance );
GFL_NAMESPACE_END( scenegraph );
GFL_NAMESPACE_END( renderingengine );
GFL_NAMESPACE_END( gfl2 );

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

/**
* @brief �t�B�[���h�J�����A1����P�ʃN���X
*/
class CameraUnit
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( CameraUnit );

public:

  /** @brief  �������ݒ� */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;
    gfl2::heap::HeapBase*         m_pDeviceHeap;
    gfl2::gfx::IGLAllocator*      m_pDeviceAllocator;
    //gfl2::fs::BinLinkerAccessor*  pAreaResourceDataAccessor;   // 1�G���A����3D���\�[�X�f�[�^(�e�N�X�`��/�V�F�[�_�[/�A�j���[�V����)
    //gfl2::fs::AsyncFileUnit*   m_pFileUnit;              // �t�@�C���ǂݍ��݃}�l�[�W���[
  };

  /**
  * @brief �R���X�g���N�^
  *
  * @return ����
  */ 
  CameraUnit( void );

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~CameraUnit( void );
  
  /**
  * @brief ����������
  *
  * @param  rInitDesc   �������ݒ�
  *
  * @return ����
  */
  void Initialize( const InitializeDescription& rInitDesc );

  /**
  * @brief �I������
  *
  * @return ����
  */
  void Terminate( void );

  /**
  * @brief �X�V����
  *
  * @return ����
  */
  void Update( void );

  /**
  * @brief ����@�\�ύX
  *
  * @param  pController �ύX��̋@�\  (�������Ǘ�����)
  *
  * @return ����
  */
  void ChangeController( Controller::ControllerBase* pController );


  /**
  * @brief �J�������̂��擾
  *
  * @return �J��������
  */
  const poke_3d::model::BaseCamera* GetBaseCamera( void ) const
  {
     return m_pBaseCamera;
  }

  /**
  * @brief �J������Y����]�ʂ��擾 @todo ���̊֐��͏�������
  *
  * @return Y����]��(���W�A���AZ��������==0.0f)
  */
  f32 GetRotationY( void ) const;

  /**
  * @brief �X�V�t���O��ݒ�
  *
  * @param  updateFlag  �X�V�t���O
  *
  * @return ����
  */
  void SetUpdateFlag( b32 updateFlag );

  /**
  * @brief �J�����̊�{�p�����[�^���R�s�[����
  * @note  View�s��A�ˉe�s��Ɋւ��p�����[�^�݂̂��R�s�[����B�A�j���[�V�����֘A�A�R���g���[���[�֘A�̒l�̓R�s�[����܂���
  *
  * @param  rSrc �R�s�[��
  *
  * @return ����
  */
  void CopyBaseParam( const CameraUnit& rSrc );

  /**
  * @brief ���݂̐ݒ�l���琶�������v���W�F�N�V�����s����擾����
  *
  * @param  pOut �s��i�[��
  *
  * @return �����œn�����|�C���^
  */
  gfl2::math::Matrix44* GetProjectionMatrix( gfl2::math::Matrix44* pOut ) const;

  /**
  * @brief ���݂̐ݒ�l���琶�������r���[�s����擾����
  *
  * @param  pOut �s��i�[��
  *
  * @return �����œn�����|�C���^
  */
  gfl2::math::Matrix34* GetViewMatrix( gfl2::math::Matrix34* pOut ) const;


private:


  //
  // ����
  //
  b32                         m_IsUpdate;                                     // �X�V����Ȃ�true

  //
  // �����I�u�W�F�N�g
  //
  poke_3d::model::BaseCamera* m_pBaseCamera;                                  // �J��������
  Controller::ControllerBase* m_pController;                                  // �J��������@�\
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*  m_pDrawEnvNode;  // �J�����A�j���[�V�����ŕK�v

  //
  // �O���I�u�W�F�N�g
  //

};  // class FieldCameraUnit


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAUNITER_H_INCLUDED__