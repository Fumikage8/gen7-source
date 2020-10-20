#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_BeaconCommandIterator.cpp
 * @brief	 ビーコンのコマンドイテレータクラス
 * @author k.ohno
 * @date	 2012.5.8
 */
//=============================================================================

#include <nn.h>
#include <nn/uds.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include "gflnet2/include/beacon/gflnet2_BeaconCommandIterator.h"
#include "gflnet2/include/beacon/gflnet2_Beacon.h"


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)



BeaconCommandIterator::BeaconCommandIterator(Beacon* pBeacon)
{
  mIndex = 0;
  mpBeacon = pBeacon;
}


BeaconCommandIterator::~BeaconCommandIterator()
{
}

//------------------------------------------------------------------
/**
 * @brief	  ビーコンコマンドを一つづつ得る
 * @param   pCommandBody    コマンドのボディーのポインタ
 * @return  BeaconCommand   ビーコンコマンド構造体  無ければNULL
 */
//------------------------------------------------------------------
BeaconCommand* BeaconCommandIterator::GetNext(u32* pCommandBody)
{
  BeaconData* pBeaconData = mpBeacon->GetBeaconData();
  int commandnum = pBeaconData->header.commandnum;
  BeaconCommand* pBC;
  char* pData;
  int size = 0;
  int i;



  pBC = (BeaconCommand*)pBeaconData->body;
  pData = (char*)pBC;
  *pCommandBody = (u32)&pData[sizeof(BeaconCommand)];
  for(i = 0 ; i < mIndex; i++){
    if(commandnum <= i){
      return NULL;
    }
    size += pBC->size;
    pBC = (BeaconCommand*)&pBeaconData->body[size];
    *pCommandBody = (u32)&pData[sizeof(BeaconCommand) + size];

  }
  mIndex++;
  
  return pBC;
}


GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)


#endif
