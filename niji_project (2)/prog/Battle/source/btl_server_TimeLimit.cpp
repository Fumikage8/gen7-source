//=============================================================================
/**
 * @file   btl_server_TimeLimit.cpp
 * @date   2016/02/09 15:04:46
 * @author obata_toshihiro
 * @brief  サーバー側で扱う制限時間情報
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <util/include/gfl2_std_string.h>
#include "./btl_server_TimeLimit.h"

GFL_NAMESPACE_BEGIN( btl )



//-----------------------------------------------------------------------------
/**
 * @brief 制限時間情報をコピーする
 * @param dest  コピー先
 * @param src   コピー元
 */
 //-----------------------------------------------------------------------------
void TimeLimit_Copy( TimeLimit* dest, const TimeLimit& src )
{
  gfl2::std::MemCopy( &src, dest, sizeof(TimeLimit) );
}

//-----------------------------------------------------------------------------
/**
 * @brief 制限時間情報をクリアする
 * @param data  クリア対象の制限時間情報
 */
//-----------------------------------------------------------------------------
void TimeLimit_Clear( TimeLimit* data )
{
  gfl2::std::MemClear( data, sizeof(TimeLimit) );
}


GFL_NAMESPACE_END( btl )