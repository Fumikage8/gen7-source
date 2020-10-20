//==============================================================================
/**
 * @file	NijiNetworkErrorDialogStatus.h
 * @brief	通信エラーのエラー表示の内部ステータス管理クラス　（表示中した時のタイマー、イベント発生したどうか情報）
 * @author	PETE
 * @date	2016/06/29
 */
// =============================================================================
#ifndef __NIJI_NETWORK_ERROR_DIALOG_STATUS_H__
#define __NIJI_NETWORK_ERROR_DIALOG_STATUS_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief	通信エラーのエラー表示ステータス（アプレット）管理クラス
 */
//------------------------------------------------------------------
class NijiNetworkErrorDialogStatus
{
  
private:
  static u64 mDialogStartTick; // Start tick for dialog event
  static bool mIsNetworkErrorDialogEvent;

public:
  // コンストラクタ
  NijiNetworkErrorDialogStatus() { }
  // デストラクタ
  virtual ~NijiNetworkErrorDialogStatus() { }

  static void NetworkErrorDialogTrigger(void); // Trigger event and record start tick
  static void ResetNetworkErrorDialogEvent(void); // Clear the event
  static bool IsNetworkErrorDialogEvent(void); // Check if Dialog Happened
  static u64 GetErrorDialogStartTick(void) { return(mDialogStartTick); }

  
};

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // __NIJI_NETWORK_ERROR_DIALOG_STATUS_H__
