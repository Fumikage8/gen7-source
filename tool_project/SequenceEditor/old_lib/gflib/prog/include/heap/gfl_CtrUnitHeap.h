#ifndef __GFL_CTRUNITHEAP_H__
#define __GFL_CTRUNITHEAP_H__
//======================================================================
/**
 * @file	gfl_CtrUnitHeap.cpp
 * @brief	3DS�̃��j�b�g�q�[�v
 * @author	ariizumi
 * @data	10/10/22
 */
//======================================================================
#pragma once

#include <nn.h>
#include <heap/gfl_CtrHeapBase.h>

namespace gfl
{
namespace heap
{

//UnitHeap�p�̏������\���̂ł�
//���j�b�g�T�C�Y�͎��ہA�Ǘ��̈悪MEMORY_HEADER_SIZE���A���C�����g��
//�傫���ق��̃T�C�Y���t������܂��B
//
typedef struct
{
  //���j�b�g�̃T�C�Y�ł��B���̃T�C�Y�ł����m�ۂł��܂���
  int unitSize;
  //���j�b�g�̃A���C�����g�ł��B
  s32 unitAlign;
}CTR_UNIT_HEAP_INIT;

//#pragma mark [> HeapBase
//�g���q�[�v
typedef nn::fnd::ThreadSafeUnitHeap CtrUnitHeapClass;

class CTRUnitHeap:public CtrHeapBase
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(CTRUnitHeap);
public:
  CTRUnitHeap(void){};
  virtual ~CTRUnitHeap(){};
  virtual void Dump(void);
  virtual void SystemDump(void);
  
  void DumpBlock( void *buf );

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

private:
  virtual void CreateHeap( void *buf , const int size , void *param );
  virtual void DeleteHeap(void);
  virtual void* AllocAlign( const int size , const int align );
  virtual void Free( void* buf );

  CtrUnitHeapClass *m_heapHandle;
  u8 handleBuff[sizeof(CtrUnitHeapClass)];

  CTR_UNIT_HEAP_INIT  m_initWork;
};



} //namespace heap
} //namespace gfl

#endif // __GFL_CTRUNITHEAP_H__
