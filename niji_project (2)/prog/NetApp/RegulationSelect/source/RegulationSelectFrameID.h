// ============================================================================
/*
 * @file RegulationSelectFrameID.h
 * @brief レギュレーション選択アプリのフレームIDを定義します。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECT_FRAMEID_H_INCLUDE )
#define REGULATIONSELECT_FRAMEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)


enum RegulationSelectFrameID
{
  REGULATIONSELECT_FRAME_ID_SELECT_LOAD,
  REGULATIONSELECT_FRAME_ID_SELECT_MAIN,
  REGULATIONSELECT_FRAME_ID_DL_LOAD,
  REGULATIONSELECT_FRAME_ID_DL_MAIN,
  REGULATIONSELECT_FRAME_ID_DL_SELECT,
  REGULATIONSELECT_FRAME_ID_DL_DELETE,
  REGULATIONSELECT_FRAME_ID_DETAIL
};

GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECT_FRAMEID_H_INCLUDE
