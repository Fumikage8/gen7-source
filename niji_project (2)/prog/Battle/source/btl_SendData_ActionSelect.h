//=============================================================================
/**
 * @file   btl_SendData_ActionSelect.h
 * @date   2015/08/31 15:55:16
 * @author obata_toshihiro
 * @brief  クライアントからの送信データ構造( 行動選択の結果 )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_SENDDATA_ACTION_SELECT_H_INCLUDED
#define BTL_SENDDATA_ACTION_SELECT_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_const.h"
#include "./btl_action.h"

GFL_NAMESPACE_BEGIN( btl )



  //---------------------------------------------------------------------------
  /**
   * @brief クライアントからの送信データ構造( 行動選択の結果 )
   */
  //---------------------------------------------------------------------------
  struct SEND_DATA_ACTION_SELECT
  {
    u8               actionNum;                          // アクションの数
    u8               pad_0;
    u8               pad_1;
    u8               pad_2;
    BTL_ACTION_PARAM actionParam[ BTL_POSIDX_MAX * 2 ];  // メガ進化を別アクションとして保持するため、最大ポケ数*2が必要
  };



GFL_NAMESPACE_END( btl )

#endif // BTL_SENDDATA_ACTION_SELECT_H_INCLUDED