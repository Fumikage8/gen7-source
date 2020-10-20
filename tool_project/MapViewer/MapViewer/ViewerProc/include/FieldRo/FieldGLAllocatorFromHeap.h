//===================================================================
/**
 * @file    FieldGLAllocatorFromHeap.h
 * @brief   �q�[�v�ݒ肪�ł���A���P�[�^�[
 * @author  saita_kazuki
 * @date    2015.04.14
 *
 * FieldTestProc���Ő錾����Ă������̂��ڐB
 */
//===================================================================

#if !defined( FIELD_GL_ALLOCATOR_FROM_HEAP_H_INCLUDED )
#define FIELD_GL_ALLOCATOR_FROM_HEAP_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include "System/include/GflvUse.h"
#include "System/include/nijiAllocator.h"

GFL_NAMESPACE_BEGIN( Field )

/*
 * @class GLAllocatorFromHeap
 */
class GLAllocatorFromHeap : public gfl2::gfx::IGLAllocator
{
public:
  
  //----------------------------------------------------------------------------
  /**
  *  @brief  �R���X�g���N�^
  */
  //-----------------------------------------------------------------------------
  GLAllocatorFromHeap():IGLAllocator(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  �f�X�g���N�^
  */
  //-----------------------------------------------------------------------------
  virtual ~GLAllocatorFromHeap(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  �R���X�g���N�^
  */
  //-----------------------------------------------------------------------------
  void  init(gfl2::heap::HeapBase* pHeap){ m_pHeap = pHeap; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ���C���������̎擾
  *  @param  �擾�������������T�C�Y
  *  @param  �A���C�����g
  *  @return ������
  */
  //-----------------------------------------------------------------------------
#if GFL_DEBUG
  virtual void*  _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum){ return m_pHeap->AllocMemoryAlign(size,alignment,filename,linenum);}
#else
  virtual void*  SystemMemoryMallocAlign(u32 size, u32 alignment){ return GflHeapAllocMemoryAlign( m_pHeap , size , alignment ); }
#endif


  //----------------------------------------------------------------------------
  /**
  *  @brief  ���C���������̉��
  *  @param  ����������������̃|�C���^
  */
  //-----------------------------------------------------------------------------
  virtual void  SystemMemoryFree(void* const address){ return GflHeapFreeMemory( address ); }

  //----------------------------------------------------------------------------
  /**
  *  @brief  �r�f�I�������̎擾
  *  @param  �擾�������������T�C�Y
  *  @param  �A���C�����g
  *  @return ������
  */
  //-----------------------------------------------------------------------------
  virtual void*  LocalMemoryMalloc(u32 size, u32 alignment){ return System::GflvUse::m_pAllocator->LocalMemoryMalloc(size,alignment); }

  //----------------------------------------------------------------------------
  /**
  *  @brief  �r�f�I�������̉��
  *  @param  ����������������̃|�C���^
  */
  //-----------------------------------------------------------------------------
  virtual void  LocalMemoryFree(void* const address){ System::GflvUse::m_pAllocator->LocalMemoryFree(address); }
  
private:

  gfl2::heap::HeapBase *m_pHeap;
};

GFL_NAMESPACE_END( Field )

#endif // FIELD_GL_ALLOCATOR_FROM_HEAP_H_INCLUDED