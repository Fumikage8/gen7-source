#ifndef __GFL2_NWALLOCATOR_H__
#define __GFL2_NWALLOCATOR_H__
//======================================================================
/**
 * @file  gfl2_HeapNwAllocater.h
 * @brief NintendoWare用のアロケーター
 * @author  ariizumi
 * @data  10/10/19
 */
//======================================================================
#pragma once

#if defined(GF_PLATFORM_WIN32)
#include <nw/ut.h>
#elif defined(GF_PLATFORM_CTR)
#include <nw/os.h>
#endif
#include <heap/include/gfl2_heap_base.h>
#include <macro/include/gfl2_Macros.h>

//#include <gfl2_Heap.h> // 循環参照を回避　2011.1.26 shin kimura

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(heap)

#if defined(GF_PLATFORM_WIN32)
class NwAllocator: public nw::ut::IAllocator
#else
class NwAllocator: public nw::os::IAllocator
#endif
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(NwAllocator);

public:
  NwAllocator(heap::HeapBase *heapBase);
  virtual ~NwAllocator(){};

#if defined(GF_PLATFORM_WIN32)
	virtual void* NwAllocator::Alloc( size_t size , u32 alignment );
#elif defined(GF_PLATFORM_CTR)
	virtual void* NwAllocator::Alloc( size_t size , u8 alignment );
#endif
  virtual void Free( void *memory );
  
  void* Alloc( size_t size );
  
  heap::HeapBase* GetHeapBase(void){return m_heapBase;}
  
  //後方確保の設定
  inline void SetIsAllocLow( bool flg ){ m_AllocLow = flg; }
  
private:
  heap::HeapBase *m_heapBase;
  bool m_AllocLow;
};

GFL_NAMESPACE_END(heap)
GFL_NAMESPACE_END(gfl2)

#endif // __GFL2_NWALLOCATOR_H__
