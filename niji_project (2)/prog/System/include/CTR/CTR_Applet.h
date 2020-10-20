#if defined(GF_PLATFORM_CTR)
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Applet.h
 *	@brief  アプレット　管理
 *	@author tomoya takahashi
 *	@date		2013.02.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __GFL_APPLET_H__
#define __GFL_APPLET_H__

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( CTR )


  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレット起動前後管理関数群
   */
  //-----------------------------------------------------------------------------
  class Applet
  {
  public:

    //----------------------------------------------------------------------------
    /**
     *  @brief  アプレットコール前処理
     *
     *  @note 内部で、AppletInterface->SaveApplication()をコール
     */
    //-----------------------------------------------------------------------------
    static void AppletCallBefore( void );

    //----------------------------------------------------------------------------
    /**
     *  @brief  アプレットコール後処理　（アプリケーションに復帰するときのみ使用する）
     *
     *  @note 内部で、AppletInterface->RecoverApplication()をコール
     */
    //-----------------------------------------------------------------------------
    static void AppletCallAfter( void );

  };
GFL_NAMESPACE_END( CTR )
GFL_NAMESPACE_END( System )

#endif // __GFL_APPLET_H__
#endif