//==============================================================================
/**
 * @file	NijiNetworkErrorDialogStatus.C++
 * @brief	通信エラーのエラー表示の内部ステータス管理クラス　（表示中した時のタイマー、イベント発生したどうか情報）
 * @author	PETE
 * @date	2016/06/29
 */
// =============================================================================
#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include "NijiNetworkErrorDialogStatus.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

u64 NijiNetworkErrorDialogStatus::mDialogStartTick = 0;
bool NijiNetworkErrorDialogStatus::mIsNetworkErrorDialogEvent = false;

void NijiNetworkErrorDialogStatus::NetworkErrorDialogTrigger(void)
{
  NijiNetworkErrorDialogStatus::mDialogStartTick = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();    
  NijiNetworkErrorDialogStatus::mIsNetworkErrorDialogEvent = true;
}

void NijiNetworkErrorDialogStatus::ResetNetworkErrorDialogEvent(void)
{
  NijiNetworkErrorDialogStatus::mIsNetworkErrorDialogEvent = false;
}

bool NijiNetworkErrorDialogStatus::IsNetworkErrorDialogEvent(void)
{
  return(NijiNetworkErrorDialogStatus::mIsNetworkErrorDialogEvent);
}

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)
