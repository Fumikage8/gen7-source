//======================================================================
/**
 * @file PokeViewerHeapDefine.h
 * @date 2015/11/24 19:38:57
 * @author araki_syo
 * @brief PokeViewerで使用するヒープサイズ設定
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#if !defined POKEVIEWER_HEAPDEFINE
#define POKEVIEWER_HEAPDEFINE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(pokeviewer)

// procのメモリ ZUKAN_MEM_PROC_
// 親はHEAPID_APP、HEAPID_APP_DEVICE
// 一番大元のメモリ
static const s32 POKEVIEWER_MEM_PROC_HEAP_MEM_SIZE   =    0x1000;
static const s32 POKEVIEWER_MEM_PROC_DEV_MEM_SIZE    = 0x1000000;

// implのメモリ
// proc内で確保する以外のものは全てここから確保する
static const s32 POKEVIEWER_MEM_IMPL_HEAP_MEM_SIZE   = 0x80000 - 0x800;  // m_heapMemではなくm_devMemからヒープメモリを確保することにした。
static const s32 POKEVIEWER_MEM_IMPL_DEV_MEM_SIZE    = POKEVIEWER_MEM_PROC_DEV_MEM_SIZE  - 0x80000;

// frame_managerのメモリ
static const s32 POKEVIEWER_MEM_FRAME_MANAGER_DEV_MEM_SIZE    = 0x1000;

  GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // POKEVIEWER_HEAPDEFINE

#endif // PM_DEBUG
