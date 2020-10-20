// ============================================================================
/*
 * @file GTSFrameID.h
 * @brief GTSフレームのIDを定義します。
 * @date 2015.03.19
 */
// ============================================================================
#if !defined( GTSFRAMEID_H_INCLUDE )
#define GTSFRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)


enum GTSFrameID
{
  GTS_FRAME_ID_STATE_CONFIRM,
  GTS_FRAME_ID_TOP_MENU,
  GTS_FRAME_ID_SEARCH_POKEMON,
  GTS_FRAME_ID_TRADE_PERSON_SELECT,
  GTS_FRAME_ID_TRADE_PERSON_REFINE_SEARCH,
  GTS_FRAME_ID_TRADE_POKEMON,
  GTS_FRAME_ID_UPLOAD_POKEMON,
  GTS_FRAME_ID_DOWNLOAD_MY_POKEMON,
  GTS_FRAME_ID_NAME_INPUT
};

GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSFRAMEID_H_INCLUDE
