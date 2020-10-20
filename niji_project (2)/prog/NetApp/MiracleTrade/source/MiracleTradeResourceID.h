// ============================================================================
/*
 * @file MiracleTradeResourceID.h
 * @brief ミラクル交換アプリで使用するリソースのIDです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADERESOURCEID_H_INCLUDE )
#define MIRACLETRADERESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)


enum MiracleTradeResourceID
{
  MIRACLETRADE_RESOURCE_ID_LAYOUT,
  MIRACLETRADE_RESOURCE_ID_MESSAGE_MIRACLETRADE,
  MIRACLETRADE_RESOURCE_ID_MESSAGE_PHRASE,
  MIRACLETRADE_RESOURCE_ID_MENU_CURSOR,
  MIRACLETRADE_RESOURCE_ID_BG_MODEL,
  MIRACLETRADE_RESOURCE_ID_BG_MOTION,
  MIRACLETRADE_RESOURCE_ID_EFFECT_BALL,
  MIRACLETRADE_RESOURCE_ID_EFFECT_WINDOW,
  MIRACLETRADE_RESOURCE_MAX,
};

GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADERESOURCEID_H_INCLUDE
