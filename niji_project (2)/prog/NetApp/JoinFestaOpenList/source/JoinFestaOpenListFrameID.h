// ============================================================================
/*
 * @file JoinFestaOpenListFrameID.h
 * @brief 開催選択アプリのフレームIDを定義します。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLIST_FRAMEID_H_INCLUDE )
#define JOINFESTAOPENLIST_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)


enum JoinFestaOpenListFrameID
{
  JOINFESTAOPENLIST_FRAME_ID_SELECT,
  JOINFESTAOPENLIST_FRAME_ID_DESCRIPTION
};

GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAOPENLIST_FRAMEID_H_INCLUDE
