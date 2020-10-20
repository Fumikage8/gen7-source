#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_H__
#define __GFLNET_H__
#pragma once

//===========================================================================
/**
 * @file  gflnet.h
 * @brief GAMEFREAK 通信ライブラリ統括ヘッダ
 * @author  GAMEFREAK inc.
 * @date  2010.09.09
 */
//===========================================================================

//#include "gflib_network.h"


#if GFL_DEBUG
#define GFL_NET_DEVEROPMENT (1)  //開発中
#else  //GFL_DEBUG
#define GFL_NET_DEVEROPMENT (0)  //製品版
#endif //GFL_DEBUG

//#include <gflnet2/include/gflnet2_NetworkDefine.h>
//任天堂ライブラリ
#include <nex.h>
#include <pia/common.h>
#include <pia/local.h>
#include <pia/local/local_UdsNetwork.h>


//#include <pia/common/common_CallContext.h>

#include <gflnet2/include/base/gflnet2_NetThread.h>

#include <gflnet2/include/base/gflnet2_TransferedIDReceiver.h>
#include <gflnet2/include/beacon/gflnet2_BeaconManager.h>
#include <gflnet2/include/base/gflnet2_BaseProcess.h>

#include <gflnet2/include/base/gflnet2_NetStateListener.h>
#include <gflnet2/include/base/gflnet2_GlobalAccess.h>
#include <gflnet2/include/base/gflnet2_Base.h>

#include <gflnet2/include/dev/gflnet2_NetZAdapter.h>

#include <gflnet2/include/scan/gflnet2_Manager.h>

#include <gflnet2/include/gflnet2_P2p.h>
#include <gflnet2/include/connect/gflnet2_AutoIrConnect.h>

#include <gflnet2/include/wifi/gflnet2_WifiSystem.h>
#include <gflnet2/include/socket/gflnet2_SocketSystem.h>
#include <gflnet2/include/friends/gflnet2_FriendManager.h>

#include <gflnet2/include/http/gflnet2_HttpSystem.h>
#include <gflnet2/include/http/gflnet2_HttpConnector.h>

#include <gflnet2/include/util/gflnet2_Base64Utility.h>

#include <gflnet2/include/boss/gflnet2_BossManager.h>

#include <gflnet2/include/nex/gflnet2_NexMessageClient.h>
#include <gflnet2/include/nex/gflnet2_NexDataStoreClient.h>
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>



#endif // __GFLNET_H__
#endif
