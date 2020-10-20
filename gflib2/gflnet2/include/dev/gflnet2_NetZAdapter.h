#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_NETZADAPTER_H__
#define __GFLNET_NETZADAPTER_H__
//=============================================================================
/**
 * @file	 gflnet_NetZAdapter.h
 * @brief	 通信管理クラス NetZAdapter用
 * @author k.ohno
 * @date	 2012.12.8
 */
//=============================================================================

#include <nex.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
/**
* @namespace gflnet2::dev
* @brief     通信の低レベル関数を扱っている 直接は使わない
*/
GFL_NAMESPACE_BEGIN(dev)

class NetZAdapter{
  GFL_FORBID_COPY_AND_ASSIGN(NetZAdapter);


public:


  static nn::nex::Session* GetSessionInstance(void);
  static nn::nex::Station* GetStationInstance(void);
  static nn::nex::DirectStream* GetDirectStreamInstance(void);

  
private:


  
};


GFL_NAMESPACE_END(dev)
GFL_NAMESPACE_END(gflnet2)

#endif//__GFLNET_NETZADAPTER_H__
#endif
