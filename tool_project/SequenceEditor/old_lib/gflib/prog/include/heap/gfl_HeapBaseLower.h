#ifndef __GFL_HEAPBASE_LOWER_H__
#define __GFL_HEAPBASE_LOWER_H__
//======================================================================
/**
 * @file	gfl_HeapBaseLower.h
 * @brief	GFL�q�[�v���N���X
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#pragma once

#include <gfl_Macros.h>
#include <heap/gfl_HeapManager.h>
#include <heap/gfl_CtrHeapBase.h>

namespace gfl
{
namespace heap
{
  
//#pragma mark [> HeapBaseLower
class GflHeapBaseLower:public HeapBase
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(GflHeapBaseLower);
public:
  GflHeapBaseLower(GflHeapBase *parent);
  virtual ~GflHeapBaseLower(){};

  //----------------------------------------------------------------------------
  /**
   *  @brief  �m�ۂ����������̃��T�C�Y
   *
   *  @param  void* buf ���T�C�Y���郁����  
   *  @param  int       �ύX��̃T�C�Y
   */
  //-----------------------------------------------------------------------------
  virtual void ResizeMemory( void* buf , const int newSize);

  //----------------------------------------------------------------------------
  /**
   *  @brief  �������󋵂̕\��
   *          �Ǝ��ŗp�ӂ���MemoryHeader�����Ƀ_���v���܂�
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void Dump(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  �������󋵂̕\��
   *          ���C�u�����Ȃǂŗp�ӂ��ꂽ�֐����g���_���v���܂�
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void SystemDump(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�j��`�F�b�N
   *
   *  @param  none
   *  @return  bool true���Ă��Ȃ� false���Ă���
   */
  //-----------------------------------------------------------------------------
  virtual bool Check(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  �f�t�H���g�A���C�����g�l�̎擾
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual int GetDefaultAlign(void);
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  HEAPID�̎擾
   *
   *  @return  HEAPID
   */
  //-----------------------------------------------------------------------------
  virtual HEAPID GetHeapId(void) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�̃T�C�Y���擾���܂�
   *
   *  @return  int �T�C�Y
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalSize(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�̍��v�󂫃T�C�Y���擾���܂�
   *
   *  @return  int �T�C�Y
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalFreeSize(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�Ŋm�ۂł���ő�̃T�C�Y���擾���܂�
   *
   *  @return  int �T�C�Y
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalAllocatableSize(void);

protected:
  //AllocCount�̑���(HeapBaseLower���瑀�삪����̂ł��������g���I)
  virtual int GetAllocCount(void)const;
  virtual void IncAllocCount(void);
  virtual void DecAllocCount(void);


private:

  GflHeapBase *m_parentHeap;

  //�ȉ��������z�֐�
  //�p����Ŏ������Ă��������B
  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�̍쐬
   *          friend��gfl::heap::Manager����̂݌Ăяo��
   *
   *  @param  void *buf �m�ۗp�o�b�t�@
   *  @param  const int size �o�b�t�@�T�C�Y
   *  @param  void* param ���[�U�[�p�����[�^�[
   */
  //-----------------------------------------------------------------------------
  virtual void CreateHeap( void *buf , const int size , void *param );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�̍폜
   *          friend��gfl::heap::Manager����̂݌Ăяo��
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void DeleteHeap(void);
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v����Ɍ���m�ۂ���n���h�����擾���܂�
   */
  //-----------------------------------------------------------------------------
  virtual HeapBase* GetLowerHandle(){return this;}

  //���ۂ̃������m�ہE�J������
  virtual void* AllocAlign( const int size , const int align );
  virtual void Free( void* buf );
  //�����܂ŏ������z�֐�

#if GFL_HEAP_DEBUG
#if GFL_HIO_ENABLE
  virtual void SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum, const int align );
#else
  virtual void SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum );
#endif
#else
  //���[�U�[�w�b�_�[�Z�b�g(�f�t�H���g)
  virtual void SetMemoryHeader_UserData( void *userHeader );
#endif
  
};

} //namespace heap
} //namespace gfl

#endif // __GFL_HEAPBASE_LOWER_H__
