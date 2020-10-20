//======================================================================
/**
 * @file	gfl_CtrUnitHeap.cpp
 * @brief	3DS�̃��j�b�g�q�[�v
 * @author	ariizumi
 * @data	10/10/22
 */
//======================================================================
#include <string.h>

#include <nn.h>
#include <nn/fnd/fnd_UnitHeap.h>
#include <nn/init/init_Allocator.h>
#include <nn/os/os_LockPolicy.h>
#include <nn/os/os_Memory.h>
#include <nn/os/os_MemoryBlock.h>
#include <nn/os/os_CriticalSection.h>

#include <gflib.h>
#include "gfl_HeapLocal.h"

namespace gfl
{
namespace heap
{

//======================================================================
//	define
//======================================================================
//#pragma mark [> define

//======================================================================
//	enum
//======================================================================
//#pragma mark [> enum

//======================================================================
//	Class HeapCTRExp
//======================================================================
//#pragma mark [> HeapCTRExp
void CTRUnitHeap::CreateHeap( void *buf , int size , void *param )
{
  CTR_UNIT_HEAP_INIT *initWork = static_cast<CTR_UNIT_HEAP_INIT*>(param);
  m_heapHandle = new (reinterpret_cast<void*>(handleBuff)) 
                                CtrUnitHeapClass( static_cast<size_t>(initWork->unitSize),
                                                  reinterpret_cast<uptr>(buf),
                                                  static_cast<size_t>(size),
                                                  static_cast<s32>(initWork->unitAlign));

  std::MemCopy( initWork , &m_initWork , sizeof(CTR_UNIT_HEAP_INIT) );
}

void CTRUnitHeap::DeleteHeap(void)
{
  m_heapHandle->Finalize();
  //delete�͂ł��Ȃ��̂Ŗ����I�Ƀf�X�g���N�^
  m_heapHandle->~CtrUnitHeapClass();
}

void* CTRUnitHeap::AllocAlign( const int size , const int align )
{
  GFL_ASSERT_STOP_MSG( m_initWork.unitSize >= size , "GFL_HEAP UnitHeap size over! unitSize[%x] reqSize[%x]\n",m_initWork.unitSize,size );
  GFL_ASSERT_STOP_MSG( m_initWork.unitAlign >= align , "GFL_HEAP UnitHeap align over! unitAlign[%x] reqAlign[%x]\n",m_initWork.unitSize,size );
  
  void *buf = m_heapHandle->Allocate();
  return buf;
}

void CTRUnitHeap::Free( void* buf )
{
  m_heapHandle->Free(buf);
}

void CTRUnitHeap::Dump(void)
{
  void *cls = this;
  GFL_PRINT("-----------------CTRUnitHeap Dump-----------------\n");
  GFL_PRINT("HEAPID         :%d\n",GetHeapId());
  GFL_PRINT("TotalSize      :0x%08x\n",GetTotalSize());
  GFL_PRINT("FreeSize       :0x%08x\n",GetTotalFreeSize());
  GFL_PRINT("UnitSize       :0x%08x\n",GetTotalAllocatableSize());
  GFL_PRINT("FreeNum        :  %8d\n",m_heapHandle->GetAllocatableCount());
  GFL_PRINT("-----------------CTRUnitHeap Dump-----------------\n");
}

void CTRUnitHeap::SystemDump(void)
{
  m_heapHandle->Dump();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�̃T�C�Y���擾���܂�
 *
 *  @return  int �T�C�Y
 */
//-----------------------------------------------------------------------------
int CTRUnitHeap::GetTotalSize(void)
{
  return static_cast<int>(m_heapHandle->GetTotalSize());
}

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�̍��v�󂫃T�C�Y���擾���܂�
 *
 *  @return  int �T�C�Y
 */
//-----------------------------------------------------------------------------
int CTRUnitHeap::GetTotalFreeSize(void)
{
  return static_cast<int>(m_heapHandle->GetAllocatableCount()) * m_initWork.unitSize;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�Ŋm�ۂł���ő�̃T�C�Y���擾���܂�
 *
 *  @return  int �T�C�Y
 */
//-----------------------------------------------------------------------------
int CTRUnitHeap::GetTotalAllocatableSize(void)
{
  //�Ǘ��̈�������悤�ɏC��
  return m_initWork.unitSize-MEMORY_HEADER_SIZE;
}


} //namespace heap
} //namespace gfl
