/**
* @brief  �t�B�[���h�J�����̑���A�J�����G���A�p
* @file   FieldCameraControllerTypeArea.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERACONTROLLERERTYPEAREA_H_INCLUDED__ )
#define __FIELDCAMERACONTROLLERERTYPEAREA_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Easing.h>
#include <Collision/include/gfl2_CollisionModel.h>

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerBase.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetBase.h"



GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );
class AreaDataManager;
struct DATA;
class MeshHitData;
GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );



GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

/**
* @brief �t�B�[���h�J�����̑���A�J�����G���A�p
*/
class ControllerTypeArea : public ControllerBase
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeArea );

public:

  /** @brief  �������ݒ� */
  class InitializeDescription
  {
  public:
    const Area::AreaDataManager* m_pAreaDataManager;
    const Target::TargetBase*    m_pTarget;             // �����Ώ�
    // const gfl2::collision::CollisionModel* m_pCameraAreaClollisionModel;  // @todo �R���W�����֐���const���ƌĂׂȂ��̂�
    gfl2::collision::CollisionModel* m_pCameraAreaClollisionModel;
    gfl2::heap::HeapBase*        m_pSystemHeap;
  };

  /**
  * @brief �R���X�g���N�^
  *
  * @param rInitDesc  �������ݒ�
  *
  * @return ����
  */ 
  ControllerTypeArea( const InitializeDescription& rInitDesc );

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~ControllerTypeArea( void );
  

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

  /**
  * @brief �X�V����( DataType::FULL )
  *
  * @param  pCamera ���삷��J�����̎���
  *
  * @return ����
  */
  void UpdateTypeFull( poke_3d::model::BaseCamera* pCamera );


  /**
  * @brief �X�V����( DataType::POINT_INTERPOLATION )
  *
  * @param  pCamera ���삷��J�����̎���
  *
  * @return ����
  */
  void UpdateTypePointInterpolation( poke_3d::model::BaseCamera* pCamera );

#if PM_DEBUG
  /**
  * @brief �f�o�b�O����ɂ��e����S�ăN���A
  *
  * @return ����
  */
  void ClearDebugEffect( void );

#endif // #if PM_DEBUG

  //
  // ��������
  //
  u32                             m_InterpolationNowFrame;  // ���݃t���[��
  s32                             m_NowAreaDataId;          // ���݂̃G���A�f�[�^�ԍ�
  s32                             m_AreaDataType;           // ���݂̃G���A�f�[�^�̌^��
  gfl2::math::Vector3             m_PrevAreaCheckPosition;  // �O��G���A�f�[�^�擾�Ɏg�p�����^�[�Q�b�g���W 
  gfl2::math::Vector3             m_PrevTargetPosition;     // �O��^�[�Q�b�g���W 
  Area::DATA*                     m_pData;
  Area::DATA*                     m_pPrevData;
  Area::MeshHitData*              m_pMeshHitData;

  gfl2::math::Vector3             m_InterpolationCameraPosition;
  gfl2::math::Vector3             m_InterpolationTargetPosition;

  gfl2::math::Vector3             m_PrevCameraTargetPosition;
  gfl2::math::Vector3             m_PrevCameraPosition;
  gfl2::math::Vector3             m_CameraPosition;
  gfl2::math::Vector3             m_TargetPosition;

#if PM_DEBUG
  // �f�o�b�O���
  gfl2::math::Vector3             m_DebugRotation;
  f32                             m_DebugDistance;
#endif // #if PM_DEBUG

  //
  // �O���I�u�W�F�N�g
  //
  poke_3d::model::BaseCamera*     m_pBaseCamera;         // �J�������� @todo �V�X�e���ŋz���H 
  const Area::AreaDataManager*    m_pAreaDataManager;    // �G���A�ݒ�f�[�^
  const Target::TargetBase*       m_pTarget;             // �����Ώ�
  // const gfl2::collision::CollisionModel* m_pCameraAreaClollisionModel;
  gfl2::collision::CollisionModel* m_pCameraAreaClollisionModel; // @todo �R���W�����֐���const���ƌĂׂȂ��̂�


};  // class FieldControllerTypeArea


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERACONTROLLERERTYPEAREA_H_INCLUDED__

  