//======================================================================
/**
 * @file  gfl2_HeapNwAllocater.cpp
 * @brief NintendoWare用のアロケーター
 * @author  ariizumi
 * @data  10/10/19
 */
//======================================================================
//#include <gflib.h>
//#include "gfl2_heap_local.h"

#include <heap/include/gfl2_NwAllocator.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(heap)

NwAllocator::NwAllocator(heap::HeapBase *heapBase)
  :m_heapBase(heapBase)
  ,m_AllocLow(false)
{
  
}


#if defined(GF_PLATFORM_WIN32)
void* NwAllocator::Alloc( size_t size , u32 alignment )
#elif defined(GF_PLATFORM_CTR)
void* NwAllocator::Alloc( size_t size , u8 alignment )
#endif
{
  if( m_AllocLow )
  {
    return GflHeapAllocMemoryLowAlign( m_heapBase , size , static_cast<int>(alignment) );
  }
  else
  {
    return GflHeapAllocMemoryAlign( m_heapBase , size , static_cast<int>(alignment) );
  }
}
void* NwAllocator::Alloc( size_t size )
{
#if defined(GF_PLATFORM_WIN32)
  void *temp = nw::ut::IAllocator::Alloc(size);
#elif defined(GF_PLATFORM_CTR)
  void *temp = nw::os::IAllocator::Alloc(size);
#endif
	return temp;
}

void NwAllocator::Free( void *memory )
{
  GflHeapFreeMemory( memory );
}

GFL_NAMESPACE_END(heap)
GFL_NAMESPACE_END(gfl2)
