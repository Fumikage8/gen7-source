// ============================================================================
/*
 * @file GTSAppParam.h
 * @brief GTSアプリを起動する為のパラメータです。
 * @date 2015.03.19
 */
// ============================================================================
#if !defined( GTSAPPPARAM_H_INCLUDE )
#define GTSAPPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h"
#include "NetApp/GTS/source/GTSDefine.h"
#include <str/include/gfl2_StrBuf.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)


struct APP_PARAM
{
  NetAppLib::System::FrameID    startFrameID;     // 開始フレーム
	u8                            outJumpMode;
  u8                            outTray;
  u8                            outPos;
};


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSAPPPARAM_H_INCLUDE
