//==============================================================================
/**
 * @file        GameOverDefine.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/07/23(木) 22:10:40
 */
//==============================================================================

#if !defined(__GAMEOVERDEFINE_H__)  // 重複定義防止
#define __GAMEOVERDEFINE_H__  // 重複定義防止マクロ
#pragma once


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)


////////////////////////////////////////////////////////////////////////////////
//
// メモリ GAMEOVER_MEM_
//
////////////////////////////////////////////////////////////////////////////////

// procのメモリ GAMEOVER_MEM_PROC_
// 親はHEAPID_APP、HEAPID_APP_DEVICE
// 一番大元のメモリ
static const s32 GAMEOVER_MEM_PROC_DEV_MEM_SIZE    = 0x1000000;

// implのメモリ GAMEOVER_MEM_IMPL_
// 親はGAMEOVER_MEM_PROC_
// proc内で確保する以外のものは全てここから確保する
static const s32 GAMEOVER_MEM_IMPL_HEAP_MEM_SIZE   = 0x80000 - 0x800;  // m_heapMemではなくm_devMemからヒープメモリを確保することにした。
static const s32 GAMEOVER_MEM_IMPL_DEV_MEM_SIZE    = GAMEOVER_MEM_PROC_DEV_MEM_SIZE  - 0x80000;

// frame_managerのメモリ ZUKAN_MEM_FRAME_MANAGER_
// 親はZUKAN_MEM_IMPL_
static const s32 GAMEOVER_MEM_FRAME_MANAGER_DEV_MEM_SIZE    = 0x1000;

GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)

#endif // __GAMEOVERDEFINE_H__ 重複定義防止
