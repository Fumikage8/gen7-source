// ============================================================================
/*
 * @file GameSyncAppParam.h
 * @brief ゲームシンクアプリを起動する為のパラメータです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_APPPARAM_H_INCLUDE )
#define GAMESYNC_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/GameSync/source/GameSyncDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)


struct APP_PARAM
{
  u32 dummy;
};



GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNC_APPPARAM_H_INCLUDE
