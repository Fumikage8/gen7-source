// ============================================================================
/*
 * @file JoinFestaSelectWordFrameResult.h
 * @brief 挨拶選択アプリのリザルトコードを定義します。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_FRAMERESULT_H_INCLUDE )
#define JOINFESTASELECTWORD_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)


enum MainFrameResultCode
{
  MAIN_FRAME_RESULT_BACK,
  MAIN_FRAME_RESULT_TO_DESCRIPTION
};


enum DescriptionFrameResultCode
{
  DESCRIPTION_FRAME_RESULT_BACK
};


GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORD_FRAMERESULT_H_INCLUDE
