// ============================================================================
/*
 * @file MiracleTradeStartConfirmLowerViewListener.h
 * @brief ミラクル交換の開始確認を行う画面の入力イベントを受け取るリスナーです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADESTARTCONFIRMLOWERVIEWLISTENER_H_INCLUDE )
#define MIRACLETRADESTARTCONFIRMLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(StartConfirm)


class MiracleTradeStartConfirmLowerViewListener
{
public:
  virtual void OnTouchBackButtonFromMiracleTradeStartConfirmLower() = 0;
  virtual void OnTouchStartButton() = 0;
};


GFL_NAMESPACE_END(StartConfirm)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADESTARTCONFIRMLOWERVIEWLISTENER_H_INCLUDE
