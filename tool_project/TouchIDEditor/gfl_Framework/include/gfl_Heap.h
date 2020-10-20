#ifndef __GFL_HEAP_H__
#define __GFL_HEAP_H__
//======================================================================
/**
 * @file	gfl_Heap.h
 * @brief	ヒープヘッダー
 * @author	ariizumi
 * @data	10/10/19
 */
//======================================================================
#pragma once

#include <heap/gfl_HeapManager.h>
#include <heap/gfl_HeapBase.h>
#include <heap/gfl_HeapBaseLower.h>
//プラットフォーム別処理
#include <heap/gfl_CtrHeapBase.h>
#include <heap/gfl_CtrExpHeap.h>
#include <heap/gfl_CtrUnitHeap.h>
#include <heap/gfl_NwAllocator.h>
#include <heap/gfl_FndAllocator.h>
//プラットフォーム別処理 ここまで
#include <heap/gfl_HeapMemory.h>
#endif // __GFL_HEAP_H__
