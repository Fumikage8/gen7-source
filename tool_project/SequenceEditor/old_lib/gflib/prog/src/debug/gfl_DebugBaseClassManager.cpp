//=============================================================================
/**
 * @file	  gfl_DebugBaseClassManager.cpp
 * @brief	  �f�o�b�O�p���N���X
            �f�o�b�O�ׂ̈ɃN���X���𕶎���ŊǗ����܂�
            �f�o�b�O�p�̂��߈����₷�����ŗD�悷�邽�ߐÓI�ɂȂ��Ă��܂�
 * @author  k.ohno
 * @date	  2011.3.7
 */
//=============================================================================

#if GFL_DEBUG

#include "gfl_Base.h"
#include "gfl_Heap.h"
#include "debug/gfl_DebugBaseClassManager.h"

namespace gfl{
namespace debug{


DebugBaseClassManager::ClassData* DebugBaseClassManager::mpClassDataStart=NULL; ///< �N���X�f�[�^�̐擪�̃A�h���X
DebugBaseClassManager::ClassData* DebugBaseClassManager::mpClassDataEnd=NULL;   ///< �N���X�f�[�^�̍Ō�̃A�h���X
unsigned long DebugBaseClassManager::mCount=0;               ///< �J�E���^�[
gfl::heap::HeapBase * DebugBaseClassManager::mpDebugHeap=NULL;   ///< �f�o�b�O�p�q�[�v�A�h���X


  
//----------------------------------------------------------------------------
/**
 *	@brief    ����������
 *  @param    debugHeap �f�o�b�O�q�[�v�N���X�|�C���^
 */
//-----------------------------------------------------------------------------

void DebugBaseClassManager::Initialize( gfl::heap::HeapBase *debugHeap )
{
  mpDebugHeap = debugHeap;
  mCount=0;
  mpClassDataStart = NULL;
  mpClassDataEnd = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief    �N���X���L�^����
 *  @param    name �N���X��
 */
//-----------------------------------------------------------------------------
void DebugBaseClassManager::AddClass(const char* name,void* classAddr)
{
  if(!mpDebugHeap){
    return;
  }

  ClassData* data = GFL_NEW(mpDebugHeap) ClassData;  //GflHeapAllocMemory(mpDebugHeap, sizeof(ClassData));
  if(data == NULL){
    GFL_PRINT("ClassLogFailed\n");
    return;
  }
  if(!mpClassDataStart){
    mpClassDataStart = data;
  }
  else if(mpClassDataEnd){
    mpClassDataEnd->Next = static_cast<void*>(data);
  }
  mpClassDataEnd = data;
  data->ClassPtr = classAddr;
  gfl::std::MemCopy(name, data->Name, CLASSNAME_SIZE);
  data->Name[CLASSNAME_SIZE-1] = 0;
  data->count= mCount;
  mCount++;
}


//----------------------------------------------------------------------------
/**
 *	@brief    �N���X���𓾂�
 *  @return   �N���X��
 */
//-----------------------------------------------------------------------------

const char* DebugBaseClassManager::GetClassName(void* classAddr)
{
  ClassData* data = mpClassDataStart;
  
  for(;;){
    if(data->ClassPtr == classAddr){
      return data->Name;
    }
    data = static_cast<ClassData*>(data->Next);
    if(data->Next==NULL){
      break;
    }
  }
  return NULL;  //������Ȃ�����
}

//----------------------------------------------------------------------------
/**
 *	@brief    �N���X������
 *  @return   �N���X�A�h���X
 */
//-----------------------------------------------------------------------------
void DebugBaseClassManager::RemoveClass(void* classAddr)
{
  ClassData* data = mpClassDataStart;
  ClassData* before = data;
  
  for(;;){
    if(data->ClassPtr == classAddr){
      before->Next = data->Next;
      if(mpClassDataStart == data){
        mpClassDataStart = static_cast<ClassData*>(data->Next);
        before = NULL;
      }
      if(mpClassDataEnd==data){
        mpClassDataEnd = before;
      }
      GFL_DELETE(data);
      return;
    }
    data = static_cast<ClassData*>(data->Next);
  }
   //������Ȃ�����
}

//----------------------------------------------------------------------------
/**
 *	@brief    �J������
 */
//-----------------------------------------------------------------------------
void DebugBaseClassManager::Finalize( void )
{
  ClassData* data = mpClassDataStart;
  ClassData* data2;

  while(data){
    data2 = static_cast<ClassData*>(data->Next);
    GFL_DELETE(data);
    data = data2;
  }
}


}  //namespace debug
}  //namespace gfl

#endif //GFL_DEBUG
