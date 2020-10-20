//==============================================================================
/**
 * @file	  BoxLowDrawListener.h
 * @brief	  描画からファンクションへリスナー
 * @author	k.ohno
 * @date	  Dec/13/2015
 */
// =============================================================================

#if !defined( __BOX_LOW_DRAW_LISTENER_H__ )
#define	__BOX_LOW_DRAW_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxLowDrawListener {


public:

  //--------------------------------------------------------------------------
  /**
   * @brief  マウスモードが終わったとき呼ばれる
   */
  //--------------------------------------------------------------------------
  virtual void OnMouseModeEnd(void) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   ページのスクロールが完了
   */
  //--------------------------------------------------------------------------
  virtual void OnPageScrollEnd(int speed) = 0;

  
  
};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif	/* __BOX_LOW_DRAW_LISTENER_H__ */
