//======================================================================
/**
 * @file app_tool_InfoWindow.h
 * @date 2015/07/02 15:49:24
 * @author kawada_koji
 * @brief インフォウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================


#if !defined __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWDEFINE_H_INCLUDED__
#define __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWDEFINE_H_INCLUDED__
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
/**
 @class  InfoWindowDefine
 @brief  インフォウィンドウで使う定義
 */
class InfoWindowDefine
{
  // GFL_FORBID_COPY_AND_ASSIGNなし、デストラクタなし

public:
  //! @brief  無限ループ対策のカウントの最大値
  static const u32 LOOP_COUNT_MAX   = 1000*60;  // 1 milli second * 1000 * 60
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


#endif // __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWDEFINE_H_INCLUDED__

