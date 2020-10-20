//=================================================================================================
/**
*  @file FieldGimmickShirenDenki.h
*  @brief �t�B�[���h�M�~�b�N �d�C����
*  @author ikuta_junya
*  @date 2016.02.17
*/
//=================================================================================================

#if !defined __FIELD_GIMMICK_SHIRENDENKI_H_INCLUDED__
#define __FIELD_GIMMICK_SHIRENDENKI_H_INCLUDED__
#pragma once

// niji
#include <model/include/gfl2_BaseModel.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include "System/include/nijiAllocator.h"

// field
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"
#include <niji_reference_files/script/FieldPawnTypes.h>
#include "Field/FieldRo/include/Collision/StaticActor.h"

// reference
//#include <niji_reference_files/script/FieldPawnTypes.h>

GFL_NAMESPACE_BEGIN( Field )


class FieldGimmickShirenDenkiCameraCallBack;

/**
 *  @class �d�C�����M�~�b�N
 */
class FieldGimmickShirenDenki : public FieldGimmick
{
  // �R�s�[�֎~�N���X�ɂ���
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickShirenDenki );

public:

  /**
   *  @brief �R���X�g���N�^
   *  @param pHeap �q�[�v
   *  @param pFieldmap �t�B�[���h�}�b�v
   */
  FieldGimmickShirenDenki( gfl2::heap::HeapBase* pHeap, Fieldmap *pFieldmap );

  /**
   *  @brief �f�X�g���N�^
   */
  virtual ~FieldGimmickShirenDenki();

  /**
   *  @brief  ����
   */
  virtual void Create(void);
  virtual void UpdateCreate(void);
  virtual bool IsCreated(void);

  /**
 *  @brief  Ready��1�x�����Ăяo��
 */
  virtual void Ready(void);

  /**
   *  @biref �j���X�V
   *  @return �j���������������ǂ���
   *
   *  �j������������܂Ń}�C�t���[���ďo���Ȃ����֐�
   */
  virtual bool Finalize( void );

  /**
   *  @brief �X�V
   *  Fieldmap::Update�ɂČďo
   *  UpdateNormal���I�����Ă���Ăяo�����
   */
  virtual void UpdateLast( void );


  poke_3d::model::BaseModel* GetModel(s32 index){ return m_pModel; };

private:

  /**
   *  @brief �M�~�b�N�̏��
   */
  enum State
  {
    STATE_NONE,       ///< �Ȃ�(���삵�Ă��Ȃ�)
    STATE_INIT,       ///< ��������
    STATE_RUNNIG,     ///< ���쒆
    STATE_END,        ///< �I��

    STATE_MAX
  };

private:

  s32                                           m_InitializeState;
  s32                                           m_FinalizeState;

  // �O���I�u�W�F�N�g
  gfl2::heap::HeapBase*                         m_pFieldDeviceHeap;
  gfl2::fs::AsyncFileManager*                   m_pFileReadManager;

  // �����I�u�W�F�N�g
  System::nijiAllocator*                                          m_pAllocator;
  FieldGimmickShirenDenkiCameraCallBack*                          m_pCameraCallBack;

  // ���f��
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pModelResource;
  poke_3d::model::BaseModel*                                      m_pModel;
  StaticActor*                                                    m_pModelActor;

  // �A�j��
  gfl2::animation::AnimationPackList                              m_AnimPackList;
  
  // ���[�h���\�[�X
  void*                                                           m_pResourceBuff;
  gfl2::fs::BinLinkerAccessor                                     m_BinLinker;
};

GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_SHIRENDENKI_H_INCLUDED__