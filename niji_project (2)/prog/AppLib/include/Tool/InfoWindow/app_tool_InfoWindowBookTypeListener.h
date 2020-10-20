//======================================================================
/**
 * @file app_tool_InfoWindowListener.h
 * @date 2015/07/02 15:47:18
 * @author kawada_koji
 * @brief インフォウィンドウのリスナー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWBOOKTYPELISTENER_H_INCLUDED__
#define __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWBOOKTYPELISTENER_H_INCLUDED__
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include "AppLib/source/Tool/InfoWindow/app_tool_InfoWindowBaseListener.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
/**
 @class  InfoWindowBookTypeListener
 @brief  インフォウィンドウ　ブックタイプのリスナー
 */
class InfoWindowBookTypeListener : public InfoWindowBaseListener
{
  GFL_FORBID_COPY_AND_ASSIGN(InfoWindowBookTypeListener);

public:
  InfoWindowBookTypeListener(void)
    : InfoWindowBaseListener()
  {}
  virtual ~InfoWindowBookTypeListener()
  {}

  //! @brief  クローズし切ったときに呼ばれるイベントハンドラ
  virtual void OnClose(void)
  {}

  //! @brief  ページが変わり切ったときに呼ばれるイベントハンドラ
  //! @param[in]  pageNo  これから表示するページ番号(0から開始。表示上の1ページ目が0になる。)
  //! @note  オープンし切ったときにも呼ばれる
  virtual void OnChangePage(u32 /*pageNo*/)
  {}

  //! @brief  コマンドのボタン(Aボタン)を押し切ったときに呼ばれるイベントハンドラ
  //! @param[in]  pageNo  表示されているページ番号(0から開始。表示上の1ページ目が0になる。)
  virtual void OnCommand(u32 /*pageNo*/)
  {}
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


#endif // __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWBOOKTYPELISTENER_H_INCLUDED__

