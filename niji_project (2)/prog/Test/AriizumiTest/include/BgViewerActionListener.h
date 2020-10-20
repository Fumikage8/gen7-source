//==============================================================================
/**
 * @file	  BgViewerActionListener.h
 * @brief	  動作接続
 * @author	k.ohno
 * @date	  平成27年8月2日 日曜日
 */
// =============================================================================

#if !defined( __BG_VIEWER_ACTION_LISTENER_H__ )
#define	__BG_VIEWER_ACTION_LISTENER_H__
#pragma once
#if PM_DEBUG

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Ariizumi )

class BgViewerActionListener {

public:
  typedef enum{
    ACTION_READ_FIELD = 0,
    ACTION_SKY_ON,     ///< 天球切り替え
    ACTION_SKY_OFF,
    ACTION_ANIM_ADD,
    ACTION_ANIM_SUB,
    ACTION_ANIM_START,
    ACTION_GROUND_ADD,
    ACTION_GROUND_SUB,
    ACTION_NEAR_ADD,
    ACTION_NEAR_SUB,
    ACTION_FAR_ADD,
    ACTION_FAR_SUB,
    ACTION_ANIMREQ,
  } ACTION_ID_NUM;


public:

  //--------------------------------------------------------------------------
  /**
   * @brief   アクション通知
   */
  //--------------------------------------------------------------------------
  virtual int OnActionStart( int actionno ) = 0;

};

GFL_NAMESPACE_END( Ariizumi )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG
#endif	/* __BG_VIEWER_ACTION_LISTENER_H__ */
