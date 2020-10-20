//==============================================================================
/**
 * @file	  BoxLowMarkDrawListener.h
 * @brief	  描画からファンクションへリスナー
 * @author	k.ohno
 * @date	  February 18, 2015
 */
// =============================================================================

#if !defined( __BOX_LOW_MARK_DRAW_LISTENER_H__ )
#define	__BOX_LOW_MARK_DRAW_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxLowMarkDrawListener {


public:

	enum{
    LOWBG_ACTION_CANCEL,
    LOWBG_ACTION_DECIDE,
    LOWBG_ACTION_UP,
    LOWBG_ACTION_DOWN,
    LOWBG_ACTION_LEFT,
    LOWBG_ACTION_RIGHT,
    LOWBG_ACTION_L_CHANGE,
    LOWBG_ACTION_R_CHANGE,
    LOWBG_ACTION_MODE_CHANGE,
    LOWBG_ACTION_SELECT_CHANGE,
    LOWBG_ACTION_JUDGE,
    LOWBG_ACTION_FIND,
    LOWBG_ACTION_FOLDER,
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

#endif	/* __BOX_LOW_MARK_DRAW_LISTENER_H__ */
