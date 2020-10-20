// ============================================================================
/*
 * @file JoinFestaOpenListFrameResult.h
 * @brief 開催選択アプリのリザルトコードを定義します。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLIST_FRAMERESULT_H_INCLUDE )
#define JOINFESTAOPENLIST_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)


enum SelectFrameResultCode
{
  SELECT_FRAME_RESULT_BACK,
  SELECT_FRAME_RESULT_TO_DESCRIPTION
};


enum DescriptionFrameResultCode
{
  DESCRIPTION_FRAME_RESULT_BACK
};


GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAOPENLIST_FRAMERESULT_H_INCLUDE
