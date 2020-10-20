//================================================================================
/**
 * @file   gfl2_ImageDBTypes.h
 * @brief  ImageDB定義ヘッダ
 * @author hisanao suzuki
 * @date   2012.2.13
 * @date   2015.11.30 niji用に移植 takahashi tomoya 
 */
//================================================================================
#ifndef __GFL2_IMAGE_DB_TYPES_H__
#define __GFL2_IMAGE_DB_TYPES_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// =========================================================================
// imagedb有効フラグ(デフォルトOFF)
// =========================================================================
#ifdef GFL2_USE_IMAGE_DB // CCFLAG指定のマクロ
  #define GFL2_ENABLE_IMAGE_DB 1

  #if defined(GF_PLATFORM_CTR)
  #include <imgdb/ImageIO.h>
  #include <imgdb/ImageIO/ImageIOTypes.h>
  #endif

#else
  #define GFL2_ENABLE_IMAGE_DB 0
#endif




#endif
