#ifndef __GFL_NWALLOCATOR_H__
#define __GFL_NWALLOCATOR_H__
//======================================================================
/**
 * @file  gfl_HeapNwAllocater.h
 * @brief NintendoWare�p�̃A���P�[�^�[
 * @author  ariizumi
 * @data  10/10/19
 */
//======================================================================
#pragma once

#include <nw.h>
#include <nw/os.h>

//#include <gfl_Heap.h> // �z�Q�Ƃ�����@2011.1.26 shin kimura

namespace gfl
{
namespace heap
{

class NwAllocator: public nw::os::IAllocator
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(NwAllocator);
public:
  NwAllocator(HeapBase *heapBase);
  virtual ~NwAllocator(){};

  virtual void* Alloc( size_t size , u8 alignment );
  virtual void Free( void *memory );
  
  void* Alloc( size_t size );
  
  HeapBase* GetHeapBase(void){return m_heapBase;}
  
  //����m�ۂ̐ݒ�
  inline void SetIsAllocLow( bool flg ){ m_AllocLow = flg; }
  
private:
  HeapBase *m_heapBase;
  bool m_AllocLow;
};

} //namespace gfl
} //namespace heap
#endif // __GFL_NWALLOCATOR_H__
