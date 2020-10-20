//=============================================================================
/**
 * @file   btl_SendData_ClientLimitTime.h
 * @date   2015/08/31 15:55:16
 * @author obata_toshihiro
 * @brief  送信データ構造( 残り手持ち時間 )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_SENDDATA_CLIENT_LIMIT_TIME_H_INCLUDED
#define BTL_SENDDATA_CLIENT_LIMIT_TIME_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN( btl )



  //---------------------------------------------------------------------------
  /**
   * @brief クライアントからの送信データ構造( 残り手持ち時間 )
   */
  //---------------------------------------------------------------------------
  struct SEND_DATA_CLIENT_LIMIT_TIME
  {
    u16 limitTime;  // 残り時間[s]
  };

  //---------------------------------------------------------------------------
  /**
   * @brief サーバーからの送信データ構造( 残り手持ち時間 )
   */
  //---------------------------------------------------------------------------
  struct SEND_DATA_CLIENT_LIMIT_TIME_SYNC
  {
    u16 limitTime[ BTL_CLIENT_NUM ];  // 残り時間[s]
  };



GFL_NAMESPACE_END( btl )

#endif // BTL_SENDDATA_CLIENT_LIMIT_TIME_H_INCLUDED