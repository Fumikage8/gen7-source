//=============================================================================
/**
 * @file	  gfl_DebugBaseClassManager.cpp
 * @brief	  デバッグ用基底クラス
            デバッグの為にクラス名を文字列で管理します
            デバッグ用のため扱いやすさを最優先するため静的になっています
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


DebugBaseClassManager::ClassData* DebugBaseClassManager::mpClassDataStart=NULL; ///< クラスデータの先頭のアドレス
DebugBaseClassManager::ClassData* DebugBaseClassManager::mpClassDataEnd=NULL;   ///< クラスデータの最後のアドレス
unsigned long DebugBaseClassManager::mCount=0;               ///< カウンター
gfl::heap::HeapBase * DebugBaseClassManager::mpDebugHeap=NULL;   ///< デバッグ用ヒープアドレス


  
//----------------------------------------------------------------------------
/**
 *	@brief    初期化する
 *  @param    debugHeap デバッグヒープクラスポインタ
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
 *	@brief    クラスを記録する
 *  @param    name クラス名
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
 *	@brief    クラス名を得る
 *  @return   クラス名
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
  return NULL;  //見つからなかった
}

//----------------------------------------------------------------------------
/**
 *	@brief    クラスを消す
 *  @return   クラスアドレス
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
   //見つからなかった
}

//----------------------------------------------------------------------------
/**
 *	@brief    開放する
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
