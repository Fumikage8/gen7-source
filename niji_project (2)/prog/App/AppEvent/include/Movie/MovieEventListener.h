//==============================================================================
/**
 * @file	  MovieEventListener.h
 * @brief	  ムービーイベントの戻りを得る
 * @author	k.ohno
 * @date	  2015.10.18
 */
// =============================================================================

#if !defined( __MOVIE_EVENT_LISTENER_H__ )
#define	__MOVIE_EVENT_LISTENER_H__
#pragma once

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


class MovieEventListener
{
public:
  //-----------------------------------------------------------------------------
  /**
   * @func    OnTermination
   * @brief   終了の通知
   */
  //-----------------------------------------------------------------------------
  virtual void OnTermination( void ) = 0;
};

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif  // __MOVIE_EVENT_LISTENER_H__
