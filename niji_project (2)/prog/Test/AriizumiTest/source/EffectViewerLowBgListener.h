//======================================================================
/**
 * @file EffectViewerLowBgListener.h
 * @date 2015/12/01 19:14:57
 * @author kijima_satoshi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================



#if !defined __EFFECT_VIEWER_LOWBG_LISTENER_H__
#define __EFFECT_VIEWER_LOWBG_LISTENER_H__
#pragma once
#if PM_DEBUG

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Ariizumi )

class EffectViewerLowBgListener {


 public:

  //--------------------------------------------------------------------------
  /**
   * @brief   ボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnButtonAction( int button_no ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   キー操作通知
   * @param             dir  キー方向
   * @retval  true = 操作実行
   * @retval  false = 操作失敗
   */
  //--------------------------------------------------------------------------
  virtual bool OnKeyAction( u32 dir ){ return false; }



};  // End of class




GFL_NAMESPACE_END( Ariizumi )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG

#endif // __EFFECT_VIEWER_LOWBG_LISTENER_H__
