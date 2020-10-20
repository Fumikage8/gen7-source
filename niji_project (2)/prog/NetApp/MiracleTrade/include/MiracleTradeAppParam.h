// ============================================================================
/*
 * @file MiracleTradeAppParam.h
 * @brief ミラクル交換アプリを起動する為のパラメータです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEAPPPARAM_H_INCLUDE )
#define MIRACLETRADEAPPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)


struct APP_PARAM
{
  NetAppLib::System::FrameID    startFrameID;
	u8                            outJumpMode;
  u8                            outTray;
  u8                            outPos;
};


GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEAPPPARAM_H_INCLUDE
