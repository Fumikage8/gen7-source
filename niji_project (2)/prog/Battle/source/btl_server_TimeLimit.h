//=============================================================================
/**
 * @file   btl_server_TimeLimit.h
 * @date   2016/02/09 15:04:46
 * @author obata_toshihiro
 * @brief  サーバー側で扱う制限時間情報
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_SERVER_TIMELIMIT_H_INCLUDED
#define BTL_SERVER_TIMELIMIT_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <battle/include/battle_SetupParam.h> // for BTL_CLIENT_NUM

GFL_NAMESPACE_BEGIN( btl )



/**
 * @brief サーバー側で扱う制限時間情報
 */
struct TimeLimit
{
  u16 clientLimitTime[ BTL_CLIENT_NUM ]; // 各クライアントの残り持ち時間[s]
};

/**
 * @brief 制限時間情報をコピーする
 * @param dest  コピー先
 * @param src   コピー元
 */
extern void TimeLimit_Copy( TimeLimit* dest, const TimeLimit& src );

/**
 * @brief 制限時間情報をクリアする
 * @param data  クリア対象の制限時間情報
 */
extern void TimeLimit_Clear( TimeLimit* data );



GFL_NAMESPACE_END( btl )

#endif // BTL_SERVER_TIMELIMIT_H_INCLUDED