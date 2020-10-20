// ============================================================================
/*
 * @file JoinFestaAttractionListSelectFrameResult.h
 * @brief アトラクション開催参加アプリのリザルトコードを定義します。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECT_FRAMERESULT_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECT_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)


enum OpenFrameResultCode
{
  OPEN_FRAME_RESULT_BACK,
  OPEN_FRAME_RESULT_TO_ENTRY,
  OPEN_FRAME_RESULT_TO_DESCRIPTION
};


enum EntryFrameResultCode
{
  ENTRY_FRAME_RESULT_BACK,
  ENTRY_FRAME_RESULT_TO_OPEN,
  ENTRY_FRAME_RESULT_TO_DESCRIPTION
};


enum DescriptionFrameResultCode
{
  DESCRIPTION_FRAME_RESULT_BACK_TO_OPEN,
  DESCRIPTION_FRAME_RESULT_BACK_TO_ENTRY
};


GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONLISTSELECT_FRAMERESULT_H_INCLUDE
