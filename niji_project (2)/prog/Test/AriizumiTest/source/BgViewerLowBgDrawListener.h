//==============================================================================
/**
 * @file	  BgViewerLowBgDrawListener.h
 * @brief	  描画からファンクションへリスナー
 * @author	k.ohno
 * @date	  February 18, 2015
 */
// =============================================================================

#if !defined( __BG_VIEWER_LOW_BG_DRAW_LISTENER_H__ )
#define	__BG_VIEWER_LOW_BG_DRAW_LISTENER_H__
#pragma once
#if PM_DEBUG

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Ariizumi )

class BgViewerLowBgDrawListener {

public:
  typedef enum{
    BTN_ID_CANCEL = 0,     ///< タイトルイン
    BTN_ID_A,             ///< 
    BTN_ID_ANIM,          ///<
    BTN_ID_ANIM_REQ,     ///<
    BTN_ID_TEN_R,         ///< 
    BTN_ID_TEN_L,         ///< 
    BTN_ID_ANIM_R,        ///< 
    BTN_ID_ANIM_L,        ///< 
    BTN_ID_GROUND_R,      ///< 
    BTN_ID_GROUND_L,      ///< 
    BTN_ID_NEAR_R,        ///< 
    BTN_ID_NEAR_L,        ///< 
    BTN_ID_FAR_R,         ///< 
    BTN_ID_FAR_L,         ///<
  } BUTTON_ID_NUM;


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
   * @param		dir  キー方向
   * @retval  true = 操作実行
   * @retval  false = 操作失敗
   */
  //--------------------------------------------------------------------------
  virtual bool OnKeyAction( u32 dir ){ return false; }
};

GFL_NAMESPACE_END( Ariizumi )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG
#endif	/* __BG_VIEWER_LOW_BG_DRAW_LISTENER_H__ */
