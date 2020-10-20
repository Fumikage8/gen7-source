// ============================================================================
/*
 * @file P2PTradeAppParam.h
 * @brief P2P交換アプリを起動する為のパラメータです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_APPPARAM_H_INCLUDE )
#define P2PTRADE_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/P2PTrade/source/P2PTradeDefineApp.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)


struct APP_PARAM
{
  u32       bootMode;     //! [in]起動モード( P2PTradeDefineApp.h の BootMode を参照 )
};


struct APP_RESULT
{
  bool      bIsSyncSaveCancel;
};


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADE_APPPARAM_H_INCLUDE
