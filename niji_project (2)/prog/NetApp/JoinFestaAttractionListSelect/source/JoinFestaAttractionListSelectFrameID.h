// ============================================================================
/*
 * @file JoinFestaAttractionListSelectFrameID.h
 * @brief アトラクション開催参加アプリのフレームIDを定義します。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECT_FRAMEID_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECT_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)


enum JoinFestaAttractionListSelectFrameID
{
  JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_OPEN,
  JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_ENTRY,
  JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_DESCRIPTION
};

GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONLISTSELECT_FRAMEID_H_INCLUDE
