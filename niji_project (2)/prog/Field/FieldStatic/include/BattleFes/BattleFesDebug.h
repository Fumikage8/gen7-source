#if PM_DEBUG
//======================================================================
/**
 * @file BattleFesDebug.h
 * @date 2016/12/14
 * @author munakata_kai
 * @brief バトルフェスのデバッグ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BATTLE_FES_DEBUG_H_INCLUDED__
#define __BATTLE_FES_DEBUG_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(BattleFesDebug)

void InitializeBattleFesDebug();
void TerminateBattleFesDebug();

void SetWinCount();
void SetRank();
void SetResetCount();

bool GetFixedLevelFlg();
s32 GetFieldType();

GFL_NAMESPACE_END( BattleFesDebug )

#endif // __BATTLE_FES_DEBUG_H_INCLUDED__

#endif // PM_DEBUG
