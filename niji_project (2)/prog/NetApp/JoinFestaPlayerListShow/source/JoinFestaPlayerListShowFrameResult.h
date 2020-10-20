// ============================================================================
/*
 * @file JoinFestaPlayerListShowFrameResult.h
 * @brief 閲覧アプリのリザルトコードを定義します。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOW_FRAMERESULT_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOW_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)


enum SelectFrameResultCode
{
  SELECT_FRAME_RESULT_BACK,
  SELECT_FRAME_RESULT_SELECT_PLAYER,
  SELECT_FRAME_RESULT_TO_DESCRIPTION
};


enum DetailFrameResultCode
{
  DETAIL_FRAME_RESULT_BACK
};


enum DescriptionFrameResultCode
{
  DESCRIPTION_FRAME_RESULT_BACK
};


GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOW_FRAMERESULT_H_INCLUDE
