#if !defined(NETAPPCOMMONMESSAGEWINDOWMANIPULATOR_H_INCLUDED)
#define NETAPPCOMMONMESSAGEWINDOWMANIPULATOR_H_INCLUDED
#pragma once
//==============================================================================
/**
  *  @file   NetAppCommonMessageWindowManipulator.h
  *  @brief  NetApp用共通メッセージウインドウ操作クラス
  *          リソースは持たず、外部で構築され提供されたリソースに対する操作のみを行う
  *  @date   2015.06.05 Fri.
  *  @author Yu Muto
  */
//==============================================================================
#include  <Layout/include/gfl2_Layout.h>
#include  <Str/include/gfl2_Str.h>

#include  <AppLib/include/Tool/CommonMessageWindowManipulator.h>

// =============================================================================
/**/
//==============================================================================
namespace app {
namespace util  {
  class  Heap;
}

namespace ui  {
  class   UIView;
}
}


namespace NetAppLib {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  NetAppCommonMessageWindowManipulator
  *  @brief  NetApp用共通メッセージウインドウ操作クラス
  *          リソースは持たず、外部で構築され提供されたリソースに対する操作のみを行う
  *          （つまり、SetupInfo.pTargetView->GetG2DUtil()に対する操作のHelper）
  *  @note   メッセージウインドウは次の構造の部品Paneとして作成されている
  *          [Root]
  *             +- All
  *               +- BG_00
  *               +- Null_Pos
  *                 +- TextBox_2      <- immediate用
  *                 +- Null_Text1
  *                   +- TextBox_1a   <- stream用1of2
  *                   +- TextBox_1b   <- stream用2of2
  *          必要に応じて、ここにTimerIconやMessageCursorを追加する
  *
  *  @date   2015.06.05 Fri.
  */
//==============================================================================
class NetAppCommonMessageWindowManipulator
  : public  App::Tool::CommonMessageWindowManipulator
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppCommonMessageWindowManipulator );

public:
  //------------------------------------------------------------------
  /**
    *  @func     NetAppCommonMessageWindowManipulator
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  NetAppCommonMessageWindowManipulator(void);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~NetAppCommonMessageWindowManipulator();

protected:
};



} /*  namespace UI        */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPCOMMONMESSAGEWINDOWMANIPULATOR_H_INCLUDED)  */
