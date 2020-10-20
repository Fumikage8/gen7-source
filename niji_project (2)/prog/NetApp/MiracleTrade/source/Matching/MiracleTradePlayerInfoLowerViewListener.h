// ============================================================================
/*
 * @file MiracleTradePlayerInfoLowerViewListener.h
 * @brief ミラクル交換の交換相手の情報を表示する画面の入力イベントを受け取るリスナーです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEPLAYERINFOLOWERVIEWLISTENER_H_INCLUDE )
#define MIRACLETRADEPLAYERINFOLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(Matching)


class MiracleTradePlayerInfoLowerViewListener
{
public:
  virtual void OnTouchScreen() = 0;
};


GFL_NAMESPACE_END(Matching)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEPLAYERINFOLOWERVIEWLISTENER_H_INCLUDE
