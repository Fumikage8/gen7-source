// ============================================================================
/*
 * @file JoinFestaSelectWordAppParam.h
 * @brief 挨拶選択アプリを起動する為のパラメータです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_APPPARAM_H_INCLUDE )
#define JOINFESTASELECTWORD_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)


struct APP_PARAM
{
  u32 editMenuID;
};



GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORD_APPPARAM_H_INCLUDE
