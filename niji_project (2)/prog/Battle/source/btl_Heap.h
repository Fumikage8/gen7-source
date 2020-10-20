//=============================================================================
/**
 * @file   btl_Heap.h
 * @date   2015/12/25 18:19:18
 * @author obata_toshihiro
 * @brief  バトル内のヒープに関する定義
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_HEAP_H_INCLUDED
#define BTL_HEAP_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN( btl )


/**
 * @brief バトル内のヒープに関する定義
 */
 class Heap
 {
 public:

   // 総ヒープサイズ
   static const u32 HEAPSIZE_TOTAL  = 0x2581400;

   // HEAPSIZE_TOTAL から確保するヒープのサイズ
   static const u32 HEAPSIZE_SYSTEM = 0x014E500;
   static const u32 HEAPSIZE_VIEW   = 0x2400500;
   static const u32 HEAPSIZE_NET    = 0x0032500;


   // AIスクリプトバイナリ読み込み領域のサイズ
   static const u32 HEAPSIZE_AI_SCRIPTBINARY = 0xB0000;

   // AI実行時に必要なヒープサイズ
   static const u32 HEAPSIZE_AI_EXEC = 0x80000;



 private:

   // インスタンス化を禁止
   Heap( void ){}
   ~Heap(){}
 };


GFL_NAMESPACE_END( btl )

#endif // BTL_HEAP_H_INCLUDED