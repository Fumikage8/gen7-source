//===================================================================
/**
 * @file    FieldGLAllocatorFromHeap.h
 * @brief   ヒープ設定ができるアロケーター
 * @author  saita_kazuki
 * @date    2015.04.14
 *
 * FieldTestProc内で宣言されていたものを移殖
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
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  GLAllocatorFromHeap():IGLAllocator(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~GLAllocatorFromHeap(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  void  init(gfl2::heap::HeapBase* pHeap){ m_pHeap = pHeap; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  メインメモリの取得
  *  @param  取得したいメモリサイズ
  *  @param  アライメント
  *  @return メモリ
  */
  //-----------------------------------------------------------------------------
#if GFL_DEBUG
  virtual void*  _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum){ return m_pHeap->AllocMemoryAlign(size,alignment,filename,linenum);}
#else
  virtual void*  SystemMemoryMallocAlign(u32 size, u32 alignment){ return GflHeapAllocMemoryAlign( m_pHeap , size , alignment ); }
#endif


  //----------------------------------------------------------------------------
  /**
  *  @brief  メインメモリの解放
  *  @param  解放したいメモリのポインタ
  */
  //-----------------------------------------------------------------------------
  virtual void  SystemMemoryFree(void* const address){ return GflHeapFreeMemory( address ); }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ビデオメモリの取得
  *  @param  取得したいメモリサイズ
  *  @param  アライメント
  *  @return メモリ
  */
  //-----------------------------------------------------------------------------
  virtual void*  LocalMemoryMalloc(u32 size, u32 alignment){ return System::GflvUse::m_pAllocator->LocalMemoryMalloc(size,alignment); }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ビデオメモリの解放
  *  @param  解放したいメモリのポインタ
  */
  //-----------------------------------------------------------------------------
  virtual void  LocalMemoryFree(void* const address){ System::GflvUse::m_pAllocator->LocalMemoryFree(address); }
  
private:

  gfl2::heap::HeapBase *m_pHeap;
};

GFL_NAMESPACE_END( Field )

#endif // FIELD_GL_ALLOCATOR_FROM_HEAP_H_INCLUDED