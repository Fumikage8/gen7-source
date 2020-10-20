//======================================================================
/**
 * @file CharaViewerDefine.h
 * @date 2015/07/22 16:12:05
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined CHARA_VIEWER_DEFINE_H_INCLUDED
#define CHARA_VIEWER_DEFINE_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)


////////////////////////////////////////////////////////////////////////////////
//
// メモリ ZUKAN_MEM_
//
////////////////////////////////////////////////////////////////////////////////

// procのメモリ ZUKAN_MEM_PROC_
// 親はHEAPID_APP、HEAPID_APP_DEVICE
// 一番大元のメモリ
static const s32 CHARA_VIEWER_MEM_PROC_HEAP_MEM_SIZE   =    0x1000;
static const s32 CHARA_VIEWER_MEM_PROC_DEV_MEM_SIZE    = 0x1000000;

// implのメモリ ZUKAN_MEM_IMPL_
// 親はZUKAN_MEM_PROC_
// proc内で確保する以外のものは全てここから確保する
//static const s32 ZUKAN_MEM_IMPL_HEAP_MEM_SIZE   = ZUKAN_MEM_PROC_HEAP_MEM_SIZE -  0x800;
//static const s32 ZUKAN_MEM_IMPL_DEV_MEM_SIZE    = ZUKAN_MEM_PROC_DEV_MEM_SIZE  - 0x1000;
static const s32 CHARA_VIEWER_MEM_IMPL_HEAP_MEM_SIZE   = 0x80000 - 0x800;  // m_heapMemではなくm_devMemからヒープメモリを確保することにした。
static const s32 CHARA_VIEWER_MEM_IMPL_DEV_MEM_SIZE    = CHARA_VIEWER_MEM_PROC_DEV_MEM_SIZE  - 0x80000;

// frame_managerのメモリ ZUKAN_MEM_FRAME_MANAGER_
// 親はZUKAN_MEM_IMPL_
static const s32 CHARA_VIEWER_MEM_FRAME_MANAGER_DEV_MEM_SIZE    = 0x1000;


GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // CHARA_VIEWER_DEFINE_H_INCLUDED

#endif // PM_DEBUG
