#if PM_DEBUG

#if !defined(EGGHATCHINGDEBUG_DEMO_H_INCLUDED)
#define EGGHATCHINGDEBUG_DEMO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EggHatchingDebug_Demo.h
 * @date   2015/11/24 Tue. 12:10:25
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <Debug/include/gfl2_DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinItemTypes.h>

//#include  "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include  "App/AppDemo/include/AppDemoUtility.h"


namespace NetApp      {
namespace EggHatching {
  class EggHatchingWork;

namespace Debug       {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  デモ用デバッグウインドウ
  *  @date   2015/09/07 Mon. 11:55:49
  */
//==============================================================================
class Demo
{
  GFL_FORBID_COPY_AND_ASSIGN(Demo);
public:
  Demo(EggHatchingWork* pWork, App::AppDemo::AppDemoUtility* pDemoUtil);
  virtual ~Demo();


  void  SetDemoUtility(App::AppDemo::AppDemoUtility* pDemoUtil) {m_pDemoUtil  = pDemoUtil;}

protected:
  EggHatchingWork*                  m_pWork;
  App::AppDemo::AppDemoUtility*     m_pDemoUtil;

  gfl2::debug::DebugWinGroup*       m_pGroup_Root;
};



} /*  namespace Debug       */
} /*  namespace EggHatching */
} /*  namespace NetApp      */
#endif  /*  #if !defined(EGGHATCHINGDEBUG_DEMO_H_INCLUDED)  */

#endif
