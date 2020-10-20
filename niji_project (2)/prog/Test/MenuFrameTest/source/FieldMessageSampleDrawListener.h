//==============================================================================
/**
 * @file	  FieldMessageSampleDrawListener.h
 * @brief	  描画からファンクションへリスナー
 * @author	k.ohno
 * @date	  February 18, 2015
 */
// =============================================================================
#if PM_DEBUG

#if !defined( __FIELD_MESSAGE_SAMPLE_DRAW_LISTENER_H__ )
#define	__FIELD_MESSAGE_SAMPLE_DRAW_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldMessageTest )

class FieldMessageSampleDrawListener {


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

GFL_NAMESPACE_END( FieldMessageTest )
GFL_NAMESPACE_END( Test )

#endif	/* __FIELD_MESSAGE_SAMPLE_DRAW_LISTENER_H__ */

#endif // PM_DEBUG
