// ============================================================================
/*
 * @file JoinFestaPlayerDetailAppParam.h
 * @brief 詳細アプリを起動する為のパラメータです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_APPPARAM_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"
#include "NetApp/JoinFestaPlayerDetail/source/JoinFestaPlayerDetailDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)


struct APP_PARAM
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerData playerData;
};



GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERDETAIL_APPPARAM_H_INCLUDE
