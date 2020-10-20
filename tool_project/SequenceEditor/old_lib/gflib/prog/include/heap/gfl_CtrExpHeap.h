#ifndef __GFL_CTREXPHEAP_H__
#define __GFL_CTREXPHEAP_H__
//======================================================================
/**
 * @file	gfl_CtrExpHeap.h
 * @brief	3DS�̊g���q�[�v
 * @author	ariizumi
 * @data	10/10/18
 */
//======================================================================
#pragma once

#include <nn.h>
#include <heap/gfl_CtrHeapBase.h>

namespace gfl
{
namespace heap
{

//#pragma mark [> HeapBase
//�g���q�[�v
typedef nn::fnd::ThreadSafeExpHeap CtrExpHeapClass;

class CTRExpHeap:public CtrHeapBase
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(CTRExpHeap);
public:
  CTRExpHeap(void){};
  virtual ~CTRExpHeap(){};
  virtual void Dump(void);
  virtual void DumpId(u8 id);
  virtual void SystemDump(void);
  virtual bool Check(void);
  
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

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�̐擪�A�h���X��Ԃ��܂�(�Ǘ��̈���܂܂Ȃ�
   *
   *  @return  void* 
   */
  //-----------------------------------------------------------------------------
  virtual void* GetStartAddress(void);

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
   *  @brief  �m�ۂ��ꂽ�̈�̃T�C�Y���擾(HeapManager�ɂ�����܂�)
   *          �p�����Ă��Ȃ���Δ�Ή�
   *
   *  @param  const void mem ���ׂ郁����
   *  @return int �������T�C�Y(-1�͎��s)
   */
  //-----------------------------------------------------------------------------
  virtual int GetMemorySize( const void *mem );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �n���ꂽ���������������ǂ������`�F�b�N���܂��B
   *
   *  @param  const void mem ���ׂ郁����
   *  @return bool �`�F�b�N���s��false
   */
  //-----------------------------------------------------------------------------
  virtual bool CheckMemoryBlock( const void *mem );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ���ׂẴ�������������܂��B
   *
   *  @return bool ���s������false
   */
  //-----------------------------------------------------------------------------
  virtual bool FreeAll(void);

#if GFL_HIO_ENABLE
  //----------------------------------------------------------------------------
  /**
   *  @brief  HIO�ł̃������󋵂̊Ď���ON/OFF
   *
   *  @param[in]  flag    true:�Ď��J�n false:�Ď��I��
   */
  //-----------------------------------------------------------------------------
  virtual void HioDump( bool flag );
#endif

private:
  virtual void CreateHeap( void *buf , const int size , void *param );
  virtual void DeleteHeap(void);
  virtual void* AllocAlign( const int size , const int align );
  virtual void Free( void* buf );

  static void BlockVisitorCallback( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
  static void BlockVisitorCallbackID( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
  static void BlockVisitorCallback_Free( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
#if GFL_HIO_ENABLE
  static void BlockVisitorHioCallback( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
#endif

  // �_���v�������[�J���֐�
  void dump( bool chk_id = false, u8 id = 0 );

  CtrExpHeapClass *m_heapHandle;
  u8 handleBuff[sizeof(CtrExpHeapClass)];

  //<�V�X�e���q�[�v�p�A���P�[�^�I�u�W�F�N�g�̎���
  //nn::util::aligned_storage<sizeof(CtrExpHeapClass::Allocator), nn::util::alignment_of<CtrExpHeapClass::Allocator>::value >::type allocaterBuffer;
  //CtrExpHeapClass::Allocator *allocater;

};



} //namespace heap
} //namespace gfl

#endif // __GFL_CTREXPHEAP_H__
