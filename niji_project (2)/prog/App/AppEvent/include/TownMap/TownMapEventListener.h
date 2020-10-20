//==============================================================================
/**
 * @file	  TownMapEventListener.h
 * @brief	  ボックスイベントの戻りを得る
 * @author	k.ohno
 * @date	  2015.08.19
 */
// =============================================================================

#if !defined( __TOWNMAP_EVENT_LISTENER_H__ )
#define	__TOWNMAP_EVENT_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


class TownMapEventListener
{
public:
  //-----------------------------------------------------------------------------
  /**
   * @func    OnTermination
   * @brief   終了の通知
   * @param   warp_id  選ばれたワープID キャンセルの場合 App::TownMap::FLY_WARPID_CANCEL
   * @param   outTray  選んだトレイ番号 ※BoxAppParam.hの特殊な戻り値参照
   * @param   outPos   選んだ場所番号
   */
  //-----------------------------------------------------------------------------
  virtual void OnTermination(s32 warp_id) = 0;
};

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif  // __TOWNMAP_EVENT_LISTENER_H__

