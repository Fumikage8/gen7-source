//======================================================================
/**
 * @file	gfl_HeapMemory.cpp
 * @brief	new delete�̃I�[�o�[���C�h
 * @author	ariizumi
 * @data	10/10/19
 */
//======================================================================
#include <new>
#include <nn.h>

#include <gflib.h>
#include "gfl_HeapLocal.h"

extern "C"{


#if GFL_DEBUG
static bool DebugDefaultNewInterdiction = false;
#endif


// ABI �̃I�[�o�[���C�h
void* malloc(size_t size)
{
  void* pM = GflHeapAllocMemory( gfl::heap::HEAPID_SYSTEM , size );
#if GFL_DEBUG
  if(DebugDefaultNewInterdiction){
    GFL_ASSERT_MSG(0,"new�֎~\n");  //assert�ɕύX
//    nn::Result result( nn::Result::LEVEL_USAGE,nn::Result::SUMMARY_OUT_OF_RESOURCE,nn::Result::MODULE_APPLICATION,nn::Result::DESCRIPTION_OUT_OF_MEMORY );
//    NN_ERR_THROW_FATAL_ALL(result);  //�f�t�H���gnew�֎~
  }
  else{
    OHNO_PRINT("�֎~new�̃T�C�Y�m�F %d %x\n",size, (u32)pM);
  }
#endif
  return pM;
}

// ABI �̃I�[�o�[���C�h
void free(void* p)
{
  if (p)
  {
   // OHNO_PRINT("�֎~new�� ����m�F  %x\n", (u32)p);
    GFL_PRINT_EX( HEAP_PRINT1 , "GFL_NEW\n");
    gfl::heap::GflHeapBase::FreeMemoryAuto(p);
//    gfl::heap::HeapBase *heap = gfl::heap::Manager::GetHeapByHeapId( gfl::heap::HEAPID_SYSTEM );
//    return heap->FreeMemory(p);
  }
}

} // extern "C"
/*
void* operator new ( size_t size )
{
  NN_LOG("NORMAL NEW(%d) v2 Abort!!\n", size);
  for (;;);
  return 0;
}

void* operator new[] ( size_t size )
{
  NN_LOG("NORMAL NEW(%d) v2 Abort!!\n", size);
  for (;;);
  return 0;
}
*/

//GF_NEW�p
#if GFL_DEBUG
void* operator new (size_t size, gfl::heap::HeapBase *heapBase , const char * filename, int line_no) throw()
{
  //NN_LOG("NEW[%s,%d]v2 \n", filename, line_no);
  //file��line���ĂԂ��ߒ��Ă�
  GFL_PRINT_EX( HEAP_PRINT2 , "GFL_NEW[%s][%d]\n", filename , line_no);
  return heapBase->AllocMemory(size , filename , line_no );
}
void* operator new[] (size_t size, gfl::heap::HeapBase *heapBase , const char * filename, int line_no ) throw()
{
  //file��line���ĂԂ��ߒ��Ă�
  GFL_PRINT_EX( HEAP_PRINT2 , "GFL_NEW[] [%s][%d]\n", filename , line_no);
  return heapBase->AllocMemory(size , filename , line_no );
}
//����m�ۗp
void* operator new (size_t size, gfl::heap::HeapBase *heapBase , const bool isLow , const char * filename, int line_no) throw()
{
  GFL_UNUSED(isLow);
  //NN_LOG("NEW[%s,%d]v2 \n", filename, line_no);
  //file��line���ĂԂ��ߒ��Ă�
  GFL_PRINT_EX( HEAP_PRINT2 , "GFL_NEW_LOW[%s][%d]\n", filename , line_no);
  return heapBase->AllocMemoryAlign(size , -heapBase->GetDefaultAlign(), filename , line_no );
}
void* operator new[] (size_t size, gfl::heap::HeapBase *heapBase , const bool isLow , const char * filename, int line_no ) throw()
{
  GFL_UNUSED(isLow);
  //file��line���ĂԂ��ߒ��Ă�
  GFL_PRINT_EX( HEAP_PRINT2 , "GFL_NEW_LOW[] [%s][%d]\n", filename , line_no);
  return heapBase->AllocMemoryAlign(size , -heapBase->GetDefaultAlign(), filename , line_no );
}
#else

void* operator new (size_t size, gfl::heap::HeapBase *heapBase ) throw()
{
  //NN_LOG("NEWv2 \n");
  //file��line���ĂԂ��ߒ��Ă�
  return heapBase->AllocMemory(size);
}

void* operator new[] (size_t size, gfl::heap::HeapBase *heapBase ) throw()
{
  //NN_LOG("NEW[]v2 \n");
  //file��line���ĂԂ��ߒ��Ă�
  return heapBase->AllocMemory(size);
}
void* operator new (size_t size, gfl::heap::HeapBase *heapBase , const bool isLow ) throw()
{
  GFL_UNUSED(isLow);
  //NN_LOG("NEWv2 \n");
  //file��line���ĂԂ��ߒ��Ă�
  return heapBase->AllocMemoryAlign(size, -heapBase->GetDefaultAlign());
}

void* operator new[] (size_t size, gfl::heap::HeapBase *heapBase , const bool isLow ) throw()
{
  GFL_UNUSED(isLow);
  //NN_LOG("NEW[]v2 \n");
  //file��line���ĂԂ��ߒ��Ă�
  return heapBase->AllocMemoryAlign(size, -heapBase->GetDefaultAlign());
}
#endif

void operator delete (void* p) throw()
{
  free(p);
}

void operator delete[] (void* p) throw()
{
  operator delete(p);
}


#if GFL_DEBUG

void GflDebugDefaultHeapInterdiction(bool flg)
{
  DebugDefaultNewInterdiction = flg;
}

bool IsGflDebugDefaultHeapInterdiction(void)
{
  return DebugDefaultNewInterdiction;
}

#endif

