//=============================================================================
/**
 * @file    app_tool_MenuWindowListener.h
 * @brief   汎用メニューウィンドウリスナー
 * @author  Hiroyuki Nakamura
 * @date    2015.03.10
 */
//=============================================================================
#if !defined( APP_TOOL_MENU_WINDOW_LISTENER_H_INCLUDED )
#define APP_TOOL_MENU_WINDOW_LISTENER_H_INCLUDED

#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

//=============================================================================
/**
 * @class MenuWindowListener
 * @brief メニューウィンドウリスナー
 * @date  2015.03.10
 */
//=============================================================================
class MenuWindowListener
{
public:
  //-----------------------------------------------------------------------------
  /**
   * @func    SelectMenu
   * @brief   項目選択時のリスナー
   * @date    2015.03.10
   *
   * @param   id  メニュー項目のID
   */
  //-----------------------------------------------------------------------------
  virtual void SelectMenu( u32 id ){};
};

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif  // APP_TOOL_MENU_WINDOW_LISTENER_H_INCLUDED