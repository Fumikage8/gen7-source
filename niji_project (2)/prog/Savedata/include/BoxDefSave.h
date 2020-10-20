//============================================================================================
/**
 * @file		BoxDefSave.h
 * @brief		ボックス画面セーブデータの共通定義
 * @author	Hiroyuki Nakamura   -> k.ohno
 * @date		12.11.30
 *
 * namespace	savedata::box
 */
//============================================================================================
#pragma once
#if !defined( __BOX_DEF_SAVE_H__ )
#define __BOX_DEF_SAVE_H__

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(Savedata)
GFL_NAMESPACE_BEGIN(Box)

static const u32 TRAY_MAX = 32;		//!< トレイ最大数

GFL_NAMESPACE_END(Box)
GFL_NAMESPACE_END(Savedata)

#endif // __BOX_DEF_SAVE_H__
