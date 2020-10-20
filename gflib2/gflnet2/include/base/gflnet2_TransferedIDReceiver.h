#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_TRANSFEREDIDRECEIVER_H__
#define __GFLNET_TRANSFEREDIDRECEIVER_H__

//=============================================================================
/**
 * @file   gflnet_TransferedIDReceiver.h
 * @brief  相手側TransferedIDを取得 返すクラス
 * @author ohno
 */
//=============================================================================
#pragma once
#include <nn.h>



#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)

class TransferedIDReceiver
{
  //GFL_FORBID_COPY_AND_ASSIGN(BeaconListener); //コピーコンストラクタ＋代入禁止
  u64 TransferedID;
  bool bMaster;
  
public:
  TransferedIDReceiver(){ TransferedID=0; bMaster=false;  };
  virtual ~TransferedIDReceiver(){};
  u64 Get(void){ return TransferedID; };
  void Set(u64 tid){ TransferedID = tid; };

  bool IsMaster(void){ return bMaster; };
  void SetMaster(void){ bMaster = true; };
  void SetChild(void){ bMaster = false; };


};


GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)


#endif // __GFLNET_TRANSFEREDIDRECEIVER_H__
#endif
