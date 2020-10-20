//==============================================================================
/**
 * @file	gflnet2_P2pDefine.h
 * @brief	P2Pで使用する定義類
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 17:48:46
 */
// =============================================================================
#ifndef __GFLNET2_P2P_DEFINE__
#define __GFLNET2_P2P_DEFINE__
#pragma once

#include <macro/include/gfl2_Macros.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)

/**
 * @enum 接続種別定義
 */
enum CONNECT_TYPE{
  PARENT,   //! 親
  CHILD,    //! 子 
  AUTO,     //! 自動(wirelessは対応していない）
};

/**
 * @enum 最大接続人数
 */
enum {
  CONNECT_MAX_TWO   = 2,
  CONNECT_MAX_FOUR  = 4
};

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET2_P2P_DEFINE__
