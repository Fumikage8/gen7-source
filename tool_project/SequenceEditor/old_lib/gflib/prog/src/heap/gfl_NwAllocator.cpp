//======================================================================
/**
 * @file  gfl_HeapNwAllocater.cpp
 * @brief NintendoWare用のアロケーター
 * @author  ariizumi
 * @data  10/10/19
 */
//======================================================================
#include <gflib.h>
#include "gfl_HeapLocal.h"
namespace gfl
{
namespace heap
{

NwAllocator::NwAllocator(HeapBase *heapBase)
  :m_heapBase(heapBase)
  ,m_AllocLow(false)
{
  
}


void* NwAllocator::Alloc( size_t size , u8 alignment )
{
  if( m_AllocLow )
  {
    return GflHeapAllocMemoryLowAlign( m_heapBase , size , alignment );
  }
  else
  {
    return GflHeapAllocMemoryAlign( m_heapBase , size , alignment );
  }
}
void* NwAllocator::Alloc( size_t size )
{
  void *temp = nw::os::IAllocator::Alloc(size);;
  return temp;
}

void NwAllocator::Free( void *memory )
{
  GflHeapFreeMemory( memory );
}

} //namespace gfl
} //namespace heap
