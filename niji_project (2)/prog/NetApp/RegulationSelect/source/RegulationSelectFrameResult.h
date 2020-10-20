// ============================================================================
/*
 * @file RegulationSelectFrameResult.h
 * @brief レギュレーション選択アプリのリザルトコードを定義します。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECT_FRAMERESULT_H_INCLUDE )
#define REGULATIONSELECT_FRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)


enum SelectLoadResultCode
{
  SELECT_LOAD_RESULT_SUCCESS_TO_MAIN
};


enum SelectMainResultCode
{
  SELECT_MAIN_RESULT_BACK,
  SELECT_MAIN_RESULT_SELECT_REGULATION,
  SELECT_MAIN_RESULT_TO_DETAIL
};


enum DLLoadResultCode
{
  DL_LOAD_RESULT_SUCCESS_TO_MAIN
};


enum DLMainResultCode
{
  DL_MAIN_RESULT_BACK,
  DL_MAIN_RESULT_SUCCESS
};


enum DLSelectResultCode
{
  DL_SELECT_RESULT_BACK,
  DL_SELECT_RESULT_TO_DELETE,
  DL_SELECT_RESULT_TO_DETAIL
};


enum DLDeleteResultCode
{
  DL_DELETE_RESULT_BACK,
  DL_DELETE_RESULT_TO_DETAIL
};


enum DetailResultCode
{
  DETAIL_RESULT_BACK_TO_SELECT_MAIN,
  DETAIL_RESULT_BACK_TO_DL_SELECT,
  DETAIL_RESULT_BACK_TO_DL_DELETE,
  DETAIL_RESULT_BACK_TO_BATTLE_TEAM_SELECT
};


GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECT_FRAMERESULT_H_INCLUDE
