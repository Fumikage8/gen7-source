//================================================================================
/**
 * @file   gfl_ImageDBTypes.h
 * @brief  ImageDB定義ヘッダ
 * @author hisanao suzuki
 * @date   2012.2.13
 */
//================================================================================
#ifndef __GFL_IMAGE_DB_TYPES_H__
#define __GFL_IMAGE_DB_TYPES_H__
#pragma once

#include <nn.h>
#include <nw.h>
#include <gfl_Base.h>
#include <gfl_Heap.h>


// =========================================================================
// imagedb有効フラグ(デフォルトOFF)
// =========================================================================
#ifdef GFL_USE_IMAGE_DB
  #define GFL_ENABLE_IMAGE_DB 1

  #include <imgdb/ImageIO.h>
  #include <imgdb/ImageIO/ImageIOTypes.h>

#else
  #define GFL_ENABLE_IMAGE_DB 0
#endif




#endif
