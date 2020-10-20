//==============================================================================
/**
 * @file	  BoxEventListener.h
 * @brief	  ボックスイベントの戻りを得る
 * @author	k.ohno
 * @date	  2015.08.19
 */
// =============================================================================

#if !defined( __BOX_EVENT_LISTENER_H__ )
#define	__BOX_EVENT_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


class BoxEventListener
{
public:
  //-----------------------------------------------------------------------------
  /**
   * @func    OnTermination
   * @brief   終了の通知
   * @param   outJumpMode  ジャンプモード
   * @param   outTray  選んだトレイ番号 ※BoxAppParam.hの特殊な戻り値参照
   * @param   outPos   選んだ場所番号
   */
  //-----------------------------------------------------------------------------
  virtual void OnTermination(u8 outJumpMode, u8 outTray,u8 outPos ) = 0;


  //-----------------------------------------------------------------------------
  /**
   * @func    OnBattleTray
   * @brief   終了の時のバトルトレイの場所を得る
   * @param   outTray  バトルトレイ番号0-5
   */
  //-----------------------------------------------------------------------------
  virtual void OnBattleTray(u8 outTray ){};
  

};

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif  // __BOX_EVENT_LISTENER_H__
