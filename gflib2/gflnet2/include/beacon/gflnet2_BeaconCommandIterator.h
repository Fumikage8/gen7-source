#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_BEACONCOMMANDITERATOR_H__
#define __GFLNET_BEACONCOMMANDITERATOR_H__
//=============================================================================
/**
 * @file	 gflnet_BeaconCommandIterator.h
 * @brief	 ビーコンのコマンドイテレータクラス
 * @author k.ohno
 * @date	 2012.5.8
 */
//=============================================================================
#pragma once

#include <nn.h>
#include <nn/uds.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <gflnet2/include/beacon/gflnet2_BeaconCommand.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)


class Beacon;

class BeaconCommandIterator{
  GFL_FORBID_COPY_AND_ASSIGN(BeaconCommandIterator);

private:
  int mIndex;    // 確認している場所
  Beacon* mpBeacon;  // Beaconのクラスポインタ

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタデストラクタ
   * @param   pBeaconCommand  ビーコンコマンド
   */
  //------------------------------------------------------------------

  BeaconCommandIterator(Beacon* pBeacon);
  ~BeaconCommandIterator();

  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンコマンドを一つづつ得る
   * @param   pCommandBody    コマンドのボディーのポインタ
   * @return  BeaconCommand   ビーコンコマンド構造体  無ければNULL
   */
  //------------------------------------------------------------------
  BeaconCommand* GetNext(u32* pCommandBody);


};


GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)
#endif // __GFLNET_BEACONCOMMANDITERATOR_H__








#endif
