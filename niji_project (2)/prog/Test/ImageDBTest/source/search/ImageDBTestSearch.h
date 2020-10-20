#if PM_DEBUG

#if !defined(__IMAGEDB_TEST_SEARCH_H_INCLUDED__)
#define __IMAGEDB_TEST_SEARCH_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ImageDBTestSearch.h
 * @date    2016/10/24 13:31:04
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <imagedb/include/gfl2_ImageDBSearcher.h>


using namespace gfl2::imagedb;


//----------------------------------------------------------------------
// @brief   前方宣言
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )
// ---[ end ]---


//----------------------------------------------------------------------
// @brief   ImageDB検索機能
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

class ImageDBSearch
{
public:
  static void CreatePictureInfo( gfl2::heap::HeapBase* heap, u32 num );
  static void DeletePictureInfo( void );

  static void ImageSearch( gfl2::heap::HeapBase* workHeap, gfl2::heap::HeapBase* infoHeap );

public:
  static ImageDBPictureInfo*    m_pPictureInfo;
  static u32                    m_PhotoNum;
};

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // defined(GF_PLATFORM_CTR)

#endif // __IMAGEDB_TEST_SEARCH_H_INCLUDED__

#endif // #if PM_DEBUG
