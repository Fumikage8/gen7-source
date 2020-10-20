#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_NetZAdapter.cpp
 * @brief	 NETZのアダプター
 * @author k.ohno
 * @date	 2012.12.8
 */
//=============================================================================

#include "gflnet2/include/dev/gflnet2_NetZAdapter.h"
#include <NetZ/src/Core/DOCore/DirectStream.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(dev)


nn::nex::Session* NetZAdapter::GetSessionInstance(void)
{
  if(nn::nex::NetZ::GetInstance()==NULL){
    return NULL;
  }
  return nn::nex::Session::GetInstance();
}


nn::nex::Station* NetZAdapter::GetStationInstance(void)
{
  if(nn::nex::NetZ::GetInstance()==NULL){
    return NULL;
  }
  return nn::nex::Station::GetLocalInstance();
}


nn::nex::DirectStream* NetZAdapter::GetDirectStreamInstance(void)
{
  if(nn::nex::NetZ::GetInstance()==NULL){
    return NULL;
  }
  return nn::nex::DirectStream::GetInstance();
}



GFL_NAMESPACE_END(dev)
GFL_NAMESPACE_END(gflnet2)
#endif
