//===========================================================================
/**
 * @file    gfl_ReadPCFileManager.cpp
 * @brief   GAMEFREAK Library PC�t�@�C����ǂݍ��ނ��߂ɕK�v�ȏ������Ɖ���̃N���X
 * @author  k.ohno
 * @date    2010.11.18
 */
//===========================================================================


#ifdef NN_SWITCH_ENABLE_HOST_IO

#include <gfl_Base.h>
#include <fs/gfl_ReadPCFileManager.h>


namespace gfl {
namespace fs {

// �ϐ��̏�����
void* ReadPCFileManager::deviceMemory;


void ReadPCFileManager::Initialize(const gfl::heap::HEAPID heapId)
{
  if(deviceMemory==NULL){
    gfl::heap::HeapBase *deviceHeap = gfl::heap::Manager::GetHeapByHeapId( heapId );
    if(deviceHeap){
      deviceMemory = GflHeapAllocMemory( deviceHeap, nn::hio::CTR::WORKMEMORY_SIZE );
      if(deviceMemory){
        nn::hio::CTR::Initialize(reinterpret_cast<void*>(deviceMemory));
      }
    }
  }
}


void ReadPCFileManager::Finalize(void)
{
  if(deviceMemory){
    nn::hio::CTR::Finalize();
    GflHeapFreeMemory(deviceMemory);
    deviceMemory=NULL;
  }
}


}
}
#endif //NN_SWITCH_ENABLE_HOST_IO
