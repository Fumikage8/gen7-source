//======================================================================
/**
 * @file  gfl_HeapNwAllocater.cpp
 * @brief NintendoWare�p�̃A���P�[�^�[
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

//����m�ۂ̐ݒ�
void FndAllocator::SetIsAllocLow( bool flg )
{
  m_AllocLow = flg;
}




} //namespace gfl
} //namespace heap


