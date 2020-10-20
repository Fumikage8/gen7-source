//======================================================================
/**
 * @file CharaViewerProcParam.h
 * @date 2015/07/21 18:35:51
 * @author araki_syo
 * @brief CharaViewerのProcに渡す引数
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined CHARA_VIEWER_PROC_PARAM_H_INCLUDED
#define CHARA_VIEWER_PROC_PARAM_H_INCLUDED
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)


//==============================================================================
/**
 @class     CharaProcParam
 @brief     CharaViewerのProcに渡す引数
 */
//==============================================================================
class CharaViewerProcParam
{
public:
  b32 fieldDebugModeEnabled;

public:
  CharaViewerProcParam(void)
    : fieldDebugModeEnabled(false)
  {}
};


GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // CHARA_VIEWER_PROC_PARAM_H_INCLUDED

#endif // PM_DEBUG
