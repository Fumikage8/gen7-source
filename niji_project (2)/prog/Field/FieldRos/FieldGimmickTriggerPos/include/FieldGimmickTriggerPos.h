//=================================================================================================
/**
*  @file FieldGimmickTriggerPos.h
*  @brief �t�B�[���h�M�~�b�N �d�C����
*  @author ikuta_junya
*  @date 2016.02.17
*/
//=================================================================================================

#if !defined __FIELD_GIMMICK_TRIGGERPOS_H_INCLUDED__
#define __FIELD_GIMMICK_TRIGGERPOS_H_INCLUDED__
#pragma once

// niji
#include <model/include/gfl2_BaseModel.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include "System/include/nijiAllocator.h"

// field
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"
#include <niji_reference_files/script/FieldPawnTypes.h>

// reference
//#include <niji_reference_files/script/FieldPawnTypes.h>

GFL_NAMESPACE_BEGIN( Field )

/**
 *  @class �d�C�����M�~�b�N
 */
class FieldGimmickTriggerPos : public FieldGimmick
{
  // �R�s�[�֎~�N���X�ɂ���
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickTriggerPos );

public:

  /**
   *  @brief �R���X�g���N�^
   *  @param pHeap �q�[�v
   *  @param pFieldmap �t�B�[���h�}�b�v
   */
  FieldGimmickTriggerPos( gfl2::heap::HeapBase* pHeap, Fieldmap *pFieldmap );

  /**
   *  @brief �f�X�g���N�^
   */
  virtual ~FieldGimmickTriggerPos();

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

  /**
   *  @brief  �C�x���g�N���`�F�b�N
   */
  virtual bool EventRequestCheck(const EventCheckRequest *cpReq);

  // poke_3d::model::BaseModel* GetModel(s32 index){ return m_pModel; };
  
  /**
   *	@brief  �g���K�[POS�̗L���t���O��ݒ�B
   *  @note   �g���K�[POS�M�~�b�N���K�p����Ă��Ȃ��}�b�v�Ŏg�p����ƁA�A�T�[�g
    *	@note   �Ώۂ̃C�x���g���ێ����Ă���A�����蔻�����K���N���A���܂��B�Ώ�POS�����������ԂŎ��s���Ȃ��ł��������B
   *
   *	@param	event_id �C�x���gID
   *  @param  flag�@�@ true:�L�� false:����
   */
  void SetActiveFlag(u32 event_id, b32 flag);

private:

  void HitCheck(b32 isEvent, const gfl2::math::Vector3& rPos);

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

  struct EVENT_DATA;
  EVENT_DATA*                                   m_pEventDataTable;
  s32                                           m_EventDataTableSize;
};

/**
 *  @class UB�p�M�~�b�N
 */
class FieldGimmickUb01 : public FieldGimmick
{
  // �R�s�[�֎~�N���X�ɂ���
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickUb01 );

public:

  /**
   *  @brief �R���X�g���N�^
   *  @param pHeap �q�[�v
   *  @param pFieldmap �t�B�[���h�}�b�v
   */
  FieldGimmickUb01( gfl2::heap::HeapBase* pHeap, Fieldmap *pFieldmap );

  /**
   *  @brief �f�X�g���N�^
   */
  virtual ~FieldGimmickUb01();

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
};


GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_TRIGGERPOS_H_INCLUDED__