#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_NETCONFIG_H__
#define __GFLNET_NETCONFIG_H__
//=============================================================================
/**
 * @file	 gflnet_NetConfig.h
 * @brief	 通信の基本となる情報を持っているクラス
 * @author k.ohno
 * @date	 2011.5.19
 */
//=============================================================================
#pragma once



#include <nex.h>
#include <gflnet2/include/gflnet2_P2p.h>
//#include <ObjDup/StationManager.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)

///< これはNetZManagerの各種設定のために用意した構造体です。
///< 以前はSampleCommon.hのマクロでコードを制御していましたが、
///< SampleCommon.hへの依存を切る必要が生じたため、各種の設定はこの構造体にセットすることにしました。

struct Config{
  Config(void){};
  ~Config(void){};

  // スレッドモード。
  int threadMode;

  bool useUserDO;

  // ストリーム関連？
  NAMESPACE_NEX qUnsignedInt32 maxSilenceTime;
  NAMESPACE_NEX qUnsignedInt32 keepAliveTimeout;
  NAMESPACE_NEX qUnsignedInt32 maxWindowMapSize;

  // ネットワークトポロジ関連。
  enum NAMESPACE_NEX NetworkTopology::_NetworkTopology networkTopology;
  bool testRouting;

  // メッセージバンドル関連
  bool enableMessageBundle;
  u16  messageBundleFlushDelay;
  u16  messageBundleFlushCheckPeriod;

  // パケットバンドル関連
  bool enablePacketBundle;
  u16  packetBundleFlushDelay;
};
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET_NETCONFIG_H__
#endif
