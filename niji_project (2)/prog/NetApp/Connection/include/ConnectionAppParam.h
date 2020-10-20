// ============================================================================
/*
 * @file ConnectionAppParam.h
 * @brief インターネット接続アプリを起動する為のパラメータです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_APPPARAM_H_INCLUDE )
#define CONNECTION_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/Connection/source/ConnectionDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)


struct APP_PARAM
{
  u32       bootMode;     //! [in]起動モード( ConnectionDefine.h の BootMode を参照 )
};

struct APP_RESULT
{
  bool      bIsCanceled;  //! [out]接続(or切断)をキャンセルしたか[ true : 接続(or切断)をキャンセルした, false : 接続(or切断)をキャンセルしてない ]
};


GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_APPPARAM_H_INCLUDE
