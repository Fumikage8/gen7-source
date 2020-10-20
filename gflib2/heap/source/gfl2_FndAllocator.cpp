//======================================================================
/**
 * @file  gfl_HeapNwAllocater.cpp
 * @brief NintendoWare用のアロケーター
 * @author  ariizumi
 * @data  10/10/19
 */
//======================================================================
#include "gfl2_heap_local.h"

#include "../include/gfl2_FndAllocator.h"

#if defined(GF_PLATFORM_CTR)

namespace gfl2
{
namespace heap
{

FndAllocator::FndAllocator(HeapBase *heapBase) :m_heapBase(heapBase) ,m_AllocLow(false)
{

}

FndAllocator::~FndAllocator()
{

}


void* FndAllocator::Allocate( size_t size , s32 alignment )
{
  if( m_AllocLow )
  {
//    return GflHeapAllocMemoryLowAlign4096( m_heapBase , size );
    return GflHeapAllocMemoryAlign4096( m_heapBase , size );
  }
  else
  {
    return GflHeapAllocMemoryAlign4096( m_heapBase , size );
  }
}


void FndAllocator::Free( void *memory )
{
  GflHeapFreeMemory( memory );
}


HeapBase* FndAllocator::GetHeapBase(void)
{
  return m_heapBase;
}

//後方確保の設定
void FndAllocator::SetIsAllocLow( bool flg )
{
  m_AllocLow = flg;
}




} //namespace gfl
} //namespace heap

#endif //defined(GF_PLATFORM_CTR)

