
#if PM_DEBUG

#if !defined(EVOLUTIONDEBUG_DEMO_H_INCLUDED)
#define EVOLUTIONDEBUG_DEMO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionDebug_Demo.h
 * @date   2015/09/07 Mon. 11:55:13
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


namespace NetApp    {
namespace Evolution {
  class EvolutionWork;

namespace Debug {
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
  Demo(EvolutionWork* pWork, App::AppDemo::AppDemoUtility* pDemoUtil);
  virtual ~Demo();


  void  SetDemoUtility(App::AppDemo::AppDemoUtility* pDemoUtil) {m_pDemoUtil  = pDemoUtil;}

protected:
  static void DoSignal(void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void DoMemDump(void* _pThis, gfl2::debug::DebugWinItem* pItem);


  EvolutionWork*                    m_pWork;
  App::AppDemo::AppDemoUtility*     m_pDemoUtil;

  gfl2::debug::DebugWinGroup*       m_pGroup_Root;
};



} /*  namespace Debug     */
} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONDEBUG_DEMO_H_INCLUDED)  */

#endif