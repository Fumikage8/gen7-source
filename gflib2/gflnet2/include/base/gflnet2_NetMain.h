#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_NETMAIN_H__
#define __GFLNET_NETMAIN_H__
//=============================================================================
/**
 * @file	 gflnet_NetMain.h
 * @brief	 通信の元クラス
 * @author k.ohno
 * @date	 2012.5.13
 */
//=============================================================================
#pragma once



#include "gflnet2/include/beacon/gflnet2_BeaconManager.h"
#include "gflnet2/include/base/gflnet2_NetMain.h"


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
/**
* @namespace gflnet2::base
* @brief     基礎にしているproc,yield等とビーコンのベースが存在する
*/
GFL_NAMESPACE_BEGIN(base)


class NetMain
{
  GFL_FORBID_COPY_AND_ASSIGN(NetMain);

private:
  // ビーコンマネージャー
  BeaconManager mBeaconManager;


public:
  NetMain(void);
  ~NetMain();

  //------------------------------------------------------------------
  /**
   * @brief	  ライブラリが拾ったビーコンをここに入れる
   * @param   BeaconData構造体
   */
  //------------------------------------------------------------------
  BeaconManager* GetBeaconManager(void){ return &mBeaconManager; };

  
};





GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)
#endif  // __GFLNET_NETMAIN_H__
#endif
