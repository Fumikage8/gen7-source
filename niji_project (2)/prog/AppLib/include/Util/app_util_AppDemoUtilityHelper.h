#if !defined(APP_UTIL_APPDEMOUTILITYHELPER_H_INCLUDED)
#define APP_UTIL_APPDEMOUTILITYHELPER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   app_util_AppDemoUtilityHelper.h
 * @date   2015/08/27 Thu. 12:13:13
 * @author muto_yu
 * @brief  App::AppDemo::AppDemoUtilityを扱うためのヘルパ
 *         具体的にはAppDemoUtilityがAppDemo.cro内にあるため、AppDemo.croの読み込みと破棄を提供する
 * @note   AppDemo.cro内など、すでにAppDemo.croが有効な状態で使用する必要はない
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <types/include/gfl2_Typedef.h>

#include  "App/AppDemo/include/AppDemoutility.h"


namespace app   {
namespace util  {
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @brief  
  *  @date   2015/08/27 Thu. 12:19:07
  */
//==============================================================================
class AppDemoUtilityHelper
{
  GFL_FORBID_COPY_AND_ASSIGN(AppDemoUtilityHelper);

public:
  AppDemoUtilityHelper(void);
  virtual ~AppDemoUtilityHelper();


  void  Begin(gfl2::heap::HeapBase* pHeapBase);
  void  End(void);

protected:
  gfl2::heap::HeapBase*   m_pHeapBase;
  gfl2::ro::Module**      m_ppROModules;

};


} /*  namespace util  */
} /*  namespace app   */
#endif  /*  #if !defined(APP_UTIL_APPDEMOUTILITYHELPER_H_INCLUDED)  */
