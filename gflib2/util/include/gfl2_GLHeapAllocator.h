//======================================================================
/**
 * @file  gfl2_GLHeapAllocator.h
 * @brief GLHeapAllocatar
 * @author  ariizumi
 * @data  15/04/21
 */
//======================================================================
#ifndef __GFL_GLHEAPALLOCATOR_H__
#define __GFL_GLHEAPALLOCATOR_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_GFGL.h>
#include <Heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)


class GLHeapAllocator : public gfl2::gfx::IGLAllocator
{
  GFL_FORBID_COPY_AND_ASSIGN(GLHeapAllocator); //コピーコンストラクタ＋代入禁止
public:
	GLHeapAllocator( gfl2::heap::HeapBase *devHeap );
	virtual ~GLHeapAllocator();

#if GFL_DEBUG
  virtual void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum);
#else
  virtual void* SystemMemoryMallocAlign(u32 size, u32 alignment);
#endif
  virtual void SystemMemoryFree(void* const address);

  virtual void* LocalMemoryMalloc(u32 size, u32 alignment);
	virtual void LocalMemoryFree(void* const address);

  gfl2::heap::HeapBase* GetHeap(void){return mHeap;}
private:

  gfl2::heap::HeapBase *mHeap;
};

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif //__GFL_GLHEAPALLOCATOR_H__