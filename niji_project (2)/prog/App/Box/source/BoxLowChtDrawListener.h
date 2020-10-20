//==============================================================================
/**
 * @file	  BoxLowChtDrawListener.h
 * @brief	  描画からファンクションへリスナー
 * @author	k.ohno
 * @date	  February 18, 2015
 */
// =============================================================================

#if !defined( __BOX_LOW_CHT_DRAW_LISTENER_H__ )
#define	__BOX_LOW_CHT_DRAW_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxLowChtDrawListener {


public:

  enum
  {
    LOWCHT_ACTION_CANCEL = -1,
  };



  
  //--------------------------------------------------------------------------
  /**
   * @brief   ボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnButtonAction( int button_no ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @brief   キー操作通知
   * @param		dir  キー方向
   * @retval  true = 操作実行
   * @retval  false = 操作失敗
   */
  //--------------------------------------------------------------------------
  virtual bool OnKeyAction( u32 dir ){ return false; }
};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif	/* __BOX_LOW_CHT_DRAW_LISTENER_H__ */
