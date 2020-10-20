//======================================================================
/**
 * @file	gfl2_HeapLocal.h
 * @brief	ヒープ用ローカル定義
 * @author	ariizumi
 * @data	10/10/18
 */
//======================================================================
#pragma once
namespace gfl2
{
namespace heap
{

//基本的な出力
#define HEAP_PRINT1 (gfl2::debug::PRINT_FLAG_LEVEL1|gfl2::debug::PRINT_FLAG_CATEGORY_HEAP)
//細かい出力
#define HEAP_PRINT2 (gfl2::debug::PRINT_FLAG_LEVEL2|gfl2::debug::PRINT_FLAG_CATEGORY_HEAP)  //一般的なデバッグ

//未使用(限定使用)
#define HEAP_PRINT3 (gfl2::debug::PRINT_FLAG_LEVEL3|gfl2::debug::PRINT_FLAG_CATEGORY_HEAP)
#define HEAP_PRINT4 (gfl2::debug::PRINT_FLAG_LEVEL4|gfl2::debug::PRINT_FLAG_CATEGORY_HEAP)
  
}
}

