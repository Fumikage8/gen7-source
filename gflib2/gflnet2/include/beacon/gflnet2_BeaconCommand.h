#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_BEACONCOMMAND_H__
#define __GFLNET_BEACONCOMMAND_H__
//=============================================================================
/**
 * @file	 gflnet_BeaconCommand.h
 * @brief	 ビーコンのコマンドクラス
 * @author k.ohno
 * @date	 2012.5.8
 */
//=============================================================================
#pragma once

#include <nn.h>
#include <nn/uds.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)


typedef struct{
  u8 size;              ///< BeaconCommonのビーコンサイズ
  u8 beaconType;        ///< タイプ
  u8 beaconFunctionNo;          ///< NO
  u8 dummy;             ///< dummy
} BeaconCommand;        ///< ビーコンコマンド


#define GFLNET_SYSTEM_COMMNAD  (0) // システムで使うコマンド
#define GFLNET_COMMNAD_CALLID  (1) // TransferedIDがサイズ分入っているコマンド
#define GFLNET_COMMNAD_FREE_PACKET  (2)   // 古い仕組みのビーコンのため
#define GFLNET_COMMAND_P2P (3) //P2Pシステムが使う




GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)
#endif // __GFLNET_BEACONCOMMAND_H__











#endif
