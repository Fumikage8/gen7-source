//======================================================================
/**
 * @file  gfl2_GLHeapAllocator.cpp
 * @brief GLHeapAllocatar
 * @author  ariizumi
 * @data  15/04/21
 */
//======================================================================

#include "util/include/gfl2_GLHeapAllocator.h"

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)

GLHeapAllocator::GLHeapAllocator(gfl2::heap::HeapBase *devHeap)
:IGLAllocator()
,mHeap(devHeap)
{
}

GLHeapAllocator::~GLHeapAllocator()
{
}

#if GFL_DEBUG
void* GLHeapAllocator::_SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
{
  return mHeap->AllocMemoryAlign(size,alignment,filename,linenum);
}
#else
void* GLHeapAllocator::SystemMemoryMallocAlign(u32 size, u32 alignment)
{
  return GflHeapAllocMemoryAlign( mHeap , size , alignment );
}
#endif


void GLHeapAllocator::SystemMemoryFree(void* const address)
{
  GflHeapFreeMemory( address );
}

void* GLHeapAllocator::LocalMemoryMalloc(u32 size, u32 alignment)
{
  return NULL;
}

void GLHeapAllocator::LocalMemoryFree(void* const address)
{
}


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)
