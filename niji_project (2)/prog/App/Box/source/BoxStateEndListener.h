//==============================================================================
/**
 * @file	  BoxStateEndListener.h
 * @brief	  結果を残す
 * @author	k.ohno
 * @date	  2016/03/01
 */
// =============================================================================

#if !defined( __BOXSTATEEND_LISTENER_H__ )
#define	__BOXSTATEEND_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxStateEndListener {

public:
  //--------------------------------------------------------------------------
  /**
   * @brief  終わったとき呼ばれる
   */
  //--------------------------------------------------------------------------
  virtual void OnEnd(u32 retcode) = 0;

};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif	/* __BOXSTATEEND_LISTENER_H__ */
