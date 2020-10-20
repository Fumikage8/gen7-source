//==============================================================================
/**
  *  @file   NetAppCommonMessageWindowManipulator.cpp
  *  @brief  NetApp用共通メッセージウインドウ操作クラス
  *          リソースは持たず、外部で構築され提供されたリソースに対する操作のみを行う
  *  @date   2015.06.05 Fri.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <AppLib/include/ui/UIView.h>
#include  <AppLib/include/tool/app_tool_TimeIcon.h>
#include  "NetStatic/NetAppLib/include/UI/NetAppCommonMessageWindowManipulator.h"


namespace NetAppLib {
namespace UI  {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     NetAppCommonMessageWindowManipulator
  *  @brief    ctor
  */
//------------------------------------------------------------------
NetAppCommonMessageWindowManipulator::NetAppCommonMessageWindowManipulator(void)
  : App::Tool::CommonMessageWindowManipulator()
{
}

//------------------------------------------------------------------
/**
  *  @func     ~NetAppCommonMessageWindowManipulator
  *  @brief    dtor
  */
//------------------------------------------------------------------
NetAppCommonMessageWindowManipulator::~NetAppCommonMessageWindowManipulator()
{
}


} /*  namespace UI        */
} /*  namespace NetAppLib */
